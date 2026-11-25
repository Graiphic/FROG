# SPDX-License-Identifier: Apache-2.0
"""Import editable source graphs, retaining unsupported operations and their real terminals."""

from dataclasses import asdict
from collections import Counter
from functools import cache
import base64
import os
from pathlib import Path
import struct

from pylabview import LVmisc
from import_labview_interfaces import ExactRead, InterfaceTypes, read_connector
from import_labview_panel import project_panel, resource_payload
from import_labview_profile import (ATTR_UID, TYPES, ProfileRefusal, bounds, convert_constant, decode_constant_value,
                                    field, heap_root, identity, items, kind, projection_requirement_diagnostics, require)
from import_labview_regions import (NATIVE_TERMINALS, STRUCTURE_KINDS, _terminal_record, event_control_displays,
                                   loop_tunnel_modes, panel_close_responses, project_region_owner)
from import_labview_structured import OPERATIONS, append_ports, boundary_port, empty_array_constant, matches_operation_types, result_binding, structure
from import_labview_source_diagram import decode_source_diagram, is_quit_application, merge_error_ports
from import_labview_bundle import Bundle, member_presentation, panel_terminal_type, project_calls, resolve_bundle
from import_labview_classes import clocked_value_member, private_field_nodes


def draft_diagram():
    return {"nodes": [], "edges": [], "zoom_percent": 100}


def source_field(record):
    saved = {key: value for key, value in (("tag", record.tag), ("class", record.kind), ("uid", record.uid),
                                           ("value", record.value)) if value is not None}
    if record.payload is not None:
        saved["data_base64"] = base64.b64encode(record.payload).decode("ascii")
    if record.children:
        saved["children"] = [source_field(child) for child in record.children]
    return saved


def layout(rectangle, width=72, height=32):
    if rectangle is None:
        return bounds(0, 0, width, height)
    x, y, right, bottom = rectangle
    return bounds(x, y, max(right - x, width), max(bottom - y, height))


class BindingTypes:
    def __init__(self, vi):
        self.table = InterfaceTypes(vi)
        self.encoding = vi.textEncoding
        self.cache = {}
        self.enums = {}

    def schema(self, binding):
        if binding is None or binding.type_id is None:
            return None
        index = binding.type_id
        if index not in self.cache:
            schema = None
            if 1 <= index <= self.table.slice.tdCount:
                descriptor = self.table.consolidated.getTopType(self.table.slice.indexShift + index - 1)
                schema = self.table.describe(descriptor, binding.uid)
            self.cache[index] = schema
        return self.cache[index]

    def __call__(self, binding):
        schema = self.schema(binding)
        if schema is None:
            return "unknown"
        field = schema.single_user_event_field()
        if field:
            source_type = field.native_type(self.encoding, self.enums)
            if source_type:
                return f'ref<"frog.events.registration",cluster<"source_1:user_event":{source_type}>>'
        return schema.native_type(self.encoding, self.enums) or "unknown"


def named_cluster_fields(schema, prefix, encoding):
    # NI's saved row index counts nested Cluster fields in preorder. Arrays,
    # resources and their element descriptors occupy a single field.
    while schema and schema.family == "TypeDef" and len(schema.children) == 1:
        schema = schema.children[0]
    if not schema or schema.family != "Cluster":
        return
    labels = [child.field_label() for child in schema.children]
    counts = Counter(labels)
    for child, label in zip(schema.children, labels):
        path = (*prefix, label.decode(encoding)) if prefix is not None and label and counts[label] == 1 else None
        yield child, path
        yield from named_cluster_fields(child, path, encoding)


def named_cluster_operation(node, graph, types):
    if node.kind != "nMux" or not {"dcoAgg", "dcoList"} <= node.fields.keys():
        return None
    aggregate_id = node.fields["dcoAgg"].uid
    rows = [row.uid for row in node.fields["dcoList"].children]
    terms = [graph.terminals[uid] for uid in node.terminals]
    aggregate_terms = [term for term in terms if term.binding == aggregate_id]
    directions = {term.direction for term in aggregate_terms}
    write = directions == {"source", "sink"}
    if len(directions) != len(aggregate_terms) or not (write or directions == {"sink"}):
        return None
    fields = {term.binding: term for term in terms if term.binding != aggregate_id}
    if not rows or set(rows) != fields.keys() or len(fields) != len(rows) or len(terms) != len(rows) + len(aggregate_terms):
        return None
    aggregate = graph.bindings.get(aggregate_id)
    if not aggregate or aggregate.kind != "nmxDCO":
        return None
    source_fields = list(named_cluster_fields(types.schema(aggregate), (), types.encoding))
    node_id = f"lv-node-{node.uid}"
    ports = {term.uid: (node_id, "result" if term.direction == "source" else "cluster") for term in aggregate_terms}
    paths = []
    for row, binding_id in enumerate(rows, 1):
        binding, term = graph.bindings.get(binding_id), fields[binding_id]
        if not binding or binding.kind != "nmxDCO" or term.direction != ("sink" if write else "source"):
            return None
        index = binding.fields["i"].value if "i" in binding.fields else 0
        if type(index) is not int or not 0 <= index < len(source_fields):
            return None
        field, path = source_fields[index]
        saved = types.schema(binding)
        if (not path or not saved or not field.native_type(types.encoding) or
                field.representation(include_label=True) != saved.representation(include_label=True)):
            return None
        if write and any(list(path[:len(previous)]) == previous or previous[:len(path)] == list(path)
                         for previous in paths):
            # Concurrent writes to the same field or an ancestor need explicit
            # precedence. Keep that source operation available for review.
            return None
        paths.append(list(path))
        ports[term.uid] = (node_id, f"element{row}")
    op = "cluster.bundle_by_name" if write else "cluster.unbundle_by_name"
    return ({"id": node_id, "kind": "primitive", "type": "frog.studio." + op,
             "palette_catalog_id": op, "cluster_function_terminals": len(paths),
             "cluster_field_names": [".".join(p) for p in paths], "cluster_field_id_paths": paths,
             "label_visible": False}, ports)


def enum_literal(uid, domain, scalar, labels, selected):
    if not 0 <= selected < len(labels):
        return None
    return {"id": f"lv-constant-{uid}", "kind": "primitive", "label_visible": False,
            "type": "frog.studio.numeric.enum_constant", "palette_catalog_id": "numeric.enum_constant",
            "value_type": scalar, "enum_constant": {"domain_id": domain, "selected_value": selected,
            "items": [{"id": f"lv-ordinal-{i}", "value": i, "label": name} for i, name in enumerate(labels)]}}


def timestamp_literal(uid, data):
    if not isinstance(data, bytes) or len(data) != 16:
        return None
    ticks = int.from_bytes(data, "big", signed=True)
    scale = 1 << 64
    seconds = ticks / scale
    numerator, denominator = seconds.as_integer_ratio()
    if numerator * scale != ticks * denominator:
        return None  # The current Studio constant stores binary64 seconds.
    return {"id": f"lv-constant-{uid}", "kind": "primitive", "type": "frog.studio.timestamp.constant",
            "palette_catalog_id": "timestamp.constant", "label_visible": False,
            "value_type": "timestamp", "text": repr(seconds)}


def constant(uid, binding, types):
    dtype = types(binding)
    if binding.empty_array_element and dtype == f"array<{binding.empty_array_element}>":
        return empty_array_constant(f"lv-constant-{uid}", binding.empty_array_element)
    saved = binding.fields.get("ConstValue")
    data = saved.payload if saved else None
    if dtype == "timestamp":
        return timestamp_literal(uid, data)
    if dtype in types.enums and data is not None:
        domain, scalar, labels = types.enums[dtype]
        width = int(scalar[1:]) // 8
        if width in (2, 4) and data == bytes(width + 1) and binding.zero_ring_item_table is not None:
            # Some saved zero Enum constants have one additional zero octet.
            # Require independent agreement with the selected display item and
            # its complete ordered Pascal-string table before reading that form.
            entries = [label.encode(types.encoding) for label in labels]
            if all(len(entry) <= 255 for entry in entries):
                table = b"".join(bytes([len(entry)]) + entry for entry in entries)
                if table == binding.zero_ring_item_table:
                    data = data[:width]
        selected = decode_constant_value(data, scalar, f"BDHb/uid:{uid}")
        return enum_literal(uid, domain, scalar, labels, selected)
    if dtype in TYPES.values():
        if binding.padded_scalar_zero and ((dtype == "u32" and data == bytes(5)) or
                                           (dtype == "bool" and data == bytes(2))):
            # The LV20 constant and its display independently identify the scalar.
            # Strip only the extra zero byte of these saved zero encodings.
            data = data[:-1]
        value = decode_constant_value(data, dtype, f"BDHb/uid:{uid}")
        return convert_constant(uid, dtype, value, 0, 0)
    return None


def cluster_literal(uid, binding, types, classes):
    """Decode immutable owned fields only when the entire saved byte stream is accounted for."""
    schema = types.schema(binding)
    while schema and schema.family == "TypeDef":
        schema = schema.children[0]
    value = binding.fields.get("ConstValue")
    if not schema or schema.family != "Cluster" or not schema.native_type(types.encoding) or not value or value.payload is None:
        return None
    stream = ExactRead(value.payload)
    children = []

    def decode(source, key, depth):
        if depth > 32 or len(children) >= 4096:
            return None
        field = source
        while field.family == "TypeDef":
            field = field.children[0]
        dtype = source.native_type(types.encoding, types.enums)
        location = f"BDHb/uid:{uid}/{key}"
        ident = "lv-constant-" + str(key)
        if dtype == "timestamp":
            return timestamp_literal(key, stream.read(16))
        if dtype in types.enums:
            domain, scalar, labels = types.enums[dtype]
            selected = decode_constant_value(stream.read(int(scalar[1:]) // 8), scalar, location)
            return enum_literal(key, domain, scalar, labels, selected)
        if field.family == "Array" and dtype and len(field.children) == 1:
            element = field.children[0].native_type(types.encoding)
            if element not in TYPES.values():
                return None
            shape = [int.from_bytes(stream.read(4), "big", signed=True) for _ in field.detail]
            if 0 not in shape or min(shape) < 0:
                return None
            return empty_array_constant(ident, element, shape)
        if field.family == "Cluster":
            node = {"id": ident, "kind": "primitive", "type": "frog.studio.cluster.constant",
                    "palette_catalog_id": "cluster.constant", "cluster_constant_collapsed": True, "label_visible": False}
            for i, item in enumerate(field.children):
                name = item.field_label()
                if not name:
                    return None
                child = decode(item, f"{key}-field-{i}", depth + 1)
                if child is None:
                    return None
                label = name.decode(types.encoding)
                child.update(constant_owner_node_id=ident, constant_field_id=label, constant_field_order=i,
                             label=label, label_visible=False, layout=bounds(8, 24 + i * 32, 72, 28))
                children.append(child)
            return node
        if field.class_id:
            owner = classes.get(field.class_id)
            if owner is None or len(field.detail[1]) != 1 or stream.read(4) != bytes((0, 0, 0, 1)):
                return None
            name = field.detail[1][0]
            saved = LVmisc.readPStr(stream, 4, None)
            if len(name) > 255:
                return None
            qualified = bytes((len(name),)) + name
            if saved != qualified + (b"\0" if len(qualified) % 2 == 0 else b"") or stream.read(8) != bytes(8):
                return None
            return {"id": ident, "kind": "frog_call", "type": "frog.studio.frog_call", "label_visible": False,
                    "call": {"document_path": "", "document_title": name.decode(types.encoding),
                             "icon_svg": "", "ports": [{"id": "value", "label": "value", "value_type": dtype,
                             "direction": "output", "connection_requirement": "optional", "normalized_x": 1.0, "normalized_y": 0.5}],
                             "class_operation": {"kind": "default", "class": owner, "member_id": ""}}}
        if field.family == "Refnum":
            cls = field.reference_class()
            values = field.value_children()
            if not cls or stream.read(4) != bytes(4) or len(values) != 1:
                return None
            return {"id": ident, "kind": "primitive", "type": "frog.studio.application_control.reference",
                    "palette_catalog_id": "application_control.reference", "label_visible": False,
                    "studio_application_control": {"profile": "frog.studio.application_control@1", "kind": "reference",
                    "expected_class": cls, "target": {"kind": "unbound", "owner_document": "", "widget_id": ""},
                    "method_id": "", "properties": [], "parameters": [],
                    "value_schema": resource_payload(values[0], types.encoding)}}
        if dtype == "path":
            if stream.read(12) != b"PTH0" + bytes((0, 0, 0, 4)) + bytes(4):
                return None
            return {"id": ident, "kind": "primitive", "type": "frog.studio.path.constant",
                    "value_type": "path", "text": "", "label_visible": False}
        if field.family == "Map":
            if stream.read(4) != bytes(4) or len(field.children) != 2:
                return None
            key_type, value_type = [resource_payload(child, types.encoding) for child in field.children]
            return {"id": ident, "kind": "primitive", "type": "frog.studio.collection.map.constant",
                    "palette_catalog_id": "collection.map.constant", "label_visible": False,
                    "studio_collection_constant": {"profile": "frog.studio.collection@1", "kind": "map",
                    "comparison_policy": "frog.studio.collection.order@1", "key_type": key_type,
                    "value_type": value_type, "data": {"entries": []}}}
        if dtype == "variant":
            version, count, size, td, has_value, index, attributes = struct.unpack(">IIHHHHI", stream.read(20))
            if LVmisc.decodeVersion(version)["major"] != 20 or (count, size, td, has_value, index, attributes) != (1, 4, 0, 1, 0, 0):
                return None
            return {"id": ident, "kind": "primitive", "type": "frog.studio.variant.constant", "palette_catalog_id": "variant.constant", "value_type": "variant"}
        if dtype not in TYPES.values():
            return None
        if dtype == "string":
            count = stream.read(4)
            data = count + stream.read(int.from_bytes(count, "big"))
        else:
            data = stream.read(1 if dtype == "bool" else int(dtype[1:]) // 8)
        return convert_constant(key, dtype, decode_constant_value(data, dtype, location), 0, 0)

    try:
        root = decode(schema, uid, 0)
        return (root, children) if root and not stream.read() else None
    except EOFError:
        return None


_ERROR = 'cluster<"status":bool,"code":i32,"source":string>'


def register_user_event(node, graph, types, wired):
    if node.kind != "eventRegNode" or len(node.terminals) != 5:
        return None
    terms = [graph.terminals[uid] for uid in node.terminals]
    bindings = [graph.bindings.get(t.binding) for t in terms]
    if any(b is None for b in bindings):
        return None
    rows, fixed = node.fields.get("dcoList"), node.fields.get("permDCOList")
    if (not rows or [row.uid for row in rows.children] != [bindings[4].uid] or not fixed
            or [row.uid for row in fixed.children] != [b.uid for b in bindings[:4]]
            or any(b.kind != "hGrowCItem" for b in bindings[:4]) or bindings[4].kind != "eventRegItem"
            or [b.flags for b in bindings[:4]] != [0x10000, 0x40001, 0x20000, 0x80001]):
        return None
    directions = ("sink", "source", "sink", "source", "sink")
    if (any(t.uid in wired and t.direction != direction for t, direction in zip(terms, directions))
            or terms[4].uid not in wired or any(types(b) != _ERROR for b in bindings[2:4])):
        return None
    previous, result, source = (types.schema(bindings[i]) for i in (0, 1, 4))
    if not previous or not result or not source or previous.representation() != result.representation():
        return None
    member = result.single_user_event_field()
    if (not member or source.representation() != result.children[0].representation()
            or not member.native_type(types.encoding)):
        return None
    field = member.field_label().decode(types.encoding)
    ident = f"lv-node-{node.uid}"
    register = {"id": ident, "kind": "primitive", "type": "frog.studio.dialog.events.register_for_events",
                "palette_catalog_id": "dialog.events.register_for_events", "label_visible": False,
                "studio_event_registration_rows": [{"id": "source_1", "event_kind": "user_event"}]}
    box = layout(node.bounds)
    unbundle = {"id": ident + "-source", "kind": "primitive", "type": "frog.studio.cluster.unbundle_by_name",
                "palette_catalog_id": "cluster.unbundle_by_name", "cluster_function_terminals": 1,
                "cluster_field_names": [field], "cluster_field_id_paths": [[field]], "label_visible": False,
                "layout": bounds(box["x"] - 168, box["y"], 144, box["height"])}
    names = ("registration_in", "registration_out", "error_in", "error_out")
    ports = {t.uid: (ident, name) for t, name in zip(terms[:4], names)}
    ports[terms[4].uid] = (unbundle["id"], "cluster")
    edge = {"id": ident + "-source-wire", "from": {"node": unbundle["id"], "port": "element1"},
            "to": {"node": ident, "port": "source:source_1"}}
    return register, ports, [unbundle], [edge]


def front_panel_open(node, graph, types, wired):
    """Translate the VI Server invoke and retain the saved window-state and activation inputs."""
    signature = {"nodeName": b"VI", "methName": b"FP.Open", "methCode": bytes((4, 56)),
                 "invokeFlags": bytes((8,))}
    if node.kind != "invokeNode" or any(node.fields.get(k) is None or node.fields[k].payload != v
                                        for k, v in signature.items()):
        return None
    # The object-manager ID saved for the same VI method varies between VIs.
    # Identify the operation by its class, method selector and full terminal contract.
    object_id = node.fields.get("oMId")
    if not object_id or not object_id.payload or len(object_id.payload) != 2:
        return None
    rows = [(graph.terminals[uid], graph.bindings.get(graph.terminals[uid].binding)) for uid in node.terminals]
    expected = (0x10000, 0x40001, 0x20000, 0x80001, 0x30000, 0x30001, 0x40000, 0x60001, 0x40000, 0x60001)
    if tuple(b.flags if b else None for t, b in rows) != expected:
        return None
    if any(b.kind != ("hGrowCItem" if i < 4 else "invokeItem") for i, (_, b) in enumerate(rows)):
        return None
    origin = f"lv-node-{node.uid}"
    ports, helpers, edges = {}, [], []
    for (t, b), (direction, dtype, port) in zip(rows[:4], (
            ("sink", 'ref<"frog.program">', "reference_in"), ("source", 'ref<"frog.program">', "reference_out"),
            ("sink", _ERROR, "error_in"), ("source", _ERROR, "error_out")), strict=True):
        if types(b) != dtype or (t.uid in wired and t.direction != direction):
            return None
        ports[t.uid] = (origin, port)
    for i in (4, 5, 7, 9):
        t, b = rows[i]
        schema = types.schema(b)
        if t.uid in wired or not schema or schema.family != "Void":
            return None
    activate, state = rows[6][0], rows[8][0]
    if types(rows[6][1]) != "bool" or any(t.uid in wired and t.direction != "sink" for t in (activate, state)):
        return None
    dtype = types(rows[8][1])
    choices = types.enums.get(dtype)
    if not choices or choices[1:] != ("u32", ["Invalid", "Standard", "Closed", "Hidden", "Minimized", "Maximized"]):
        return None
    cfg = {"profile": "frog.studio.application_control@1", "kind": "invoke", "expected_class": "frog.program",
           "method_id": "front_panel.open", "properties": [], "parameters": [],
           "target": {"kind": "unbound" if rows[0][0].uid in wired else "this_frog", "owner_document": "", "widget_id": ""},
           "value_schema": {"canonical": "unknown", "children": [], "choice_kind": "none", "choices": [], "field_names": []}}
    def connect(source, output, parameter):
        edges.append({"id": f"{origin}-{parameter}-wire", "from": {"node": source, "port": output},
                      "to": {"node": origin, "port": f"parameter:{parameter}:in"}})
    box = layout(node.bounds)
    ports[activate.uid] = (origin, "parameter:activate:in")
    if activate.uid not in wired:
        # The saved VI Server default is false, while GS normally activates its panel.
        literal = convert_constant(f"{node.uid}-activate", "bool", False, box["x"] - 100, box["y"] - 40)
        helpers.append(literal)
        connect(literal["id"], "value", "activate")
    if state.uid in wired:
        conversion = {"id": origin + "-state-u32", "kind": "primitive", "type": "frog.studio.numeric.conversion.to_u32",
                      "palette_catalog_id": "numeric.conversion.to_u32", "label_visible": False,
                      "layout": bounds(box["x"] - 68, box["y"] + 12, 42, 30)}
        helpers.append(conversion)
        ports[state.uid] = (conversion["id"], "value")
        connect(conversion["id"], "result", "state")
    else:
        ports[state.uid] = (origin, "parameter:state:in")
    return ({"id": origin, "kind": "primitive", "type": "frog.studio.application_control.invoke_node",
             "palette_catalog_id": "application_control.invoke_node", "label_visible": False,
             "studio_application_control": cfg}, ports, helpers, edges)


def application_kind(node, graph, types, wired):
    """Adapt a VI Server App.Kind read to the saved enum and preserve its reference/error chain."""
    if (node.kind != "propNode" or len(node.terminals) != 5 or
            any(key not in node.fields or node.fields[key].payload != value
                for key, value in (("nodeName", b"App"), ("oMId", b"\x00m")))):
        return None
    terms = {}
    for uid in node.terminals:
        term = graph.terminals[uid]
        binding = graph.bindings.get(term.binding)
        if not binding or binding.flags in terms:
            return None
        terms[binding.flags] = (term, binding)
    if terms.keys() != {0x10000, 0x40001, 0x20000, 0x80001, 1}:
        return None
    origin = f"lv-node-{node.uid}"
    ports = {}
    for key, port, direction, dtype in (
        (0x10000, "reference_in", "sink", 'ref<"frog.application">'),
        (0x40001, "reference_out", "source", 'ref<"frog.application">'),
        (0x20000, "error_in", "sink", _ERROR), (0x80001, "error_out", "source", _ERROR),
    ):
        term, binding = terms[key]
        if binding.kind != "hGrowCItem" or types(binding) != dtype or (term.uid in wired and term.direction != direction):
            return None
        ports[term.uid] = (origin, port)
    term, binding = terms[1]
    props = binding.fields.get("propList")
    if binding.kind != "propItem" or term.direction != "source" or not props or len(props.children) != 1:
        return None
    item = props.children[0]
    fields = {field.tag: field.payload for field in item.children}
    if item.kind != "propItemInfo" or fields.get("PropItemName") != b"App.Kind" or fields.get("PropItemCode") != b"\x03":
        return None
    dtype = types(binding)
    choices = types.enums.get(dtype)
    if not choices:
        return None
    domain, scalar, labels = choices
    if scalar != "u32" or labels != ["invalid app kind", "Development System", "Run Time System", "Student Edition",
                                     "Embedded LabVIEW", "Evaluation", "Custom"]:
        return None
    bound = terms[0x10000][0].uid in wired
    read = {"id": origin, "kind": "primitive", "type": "frog.studio.application_control.property_node",
            "palette_catalog_id": "application_control.property_node", "label_visible": False,
            "studio_application_control": {"profile": "frog.studio.application_control@1", "kind": "property",
                "expected_class": "frog.application", "method_id": "", "parameters": [],
                "properties": [{"id": "kind", "member_id": "kind", "mode": "read"}],
                "target": {"kind": "unbound" if bound else "this_application", "owner_document": "", "widget_id": ""},
                "value_schema": {"canonical": "unknown", "children": [], "field_names": [],
                                 "choice_kind": "none", "choices": []}}}
    case_id = origin + "-kind-enum"
    case = structure(case_id, "case_structure")
    case.update(structure_type="case", case_profile="frog.case-editing@1", binding_profile="frog.typed-binding@1",
                structure_terminals={"selector": _terminal_record("string", "selector", "selector", "selected_case", True)})
    case["boundary"]["outputs"].append({"id": "kind", "type": dtype})
    case["structure_tunnels"].append({"id": "kind", "side": "right", "normalized_position": 0.5,
                                      "value_type": dtype, "output_from_structure": True})
    box = layout(node.bounds)
    case["layout"] = bounds(box["x"] + box["width"] + 32, box["y"], 218, 116)
    for token, ordinal in (("IDE", 1), ("Runtime", 2), (None, 0)):
        key = "invalid" if token is None else token.lower()
        literal = {"id": case_id + "-" + key, "kind": "primitive", "type": "frog.studio.numeric.enum_constant",
                   "palette_catalog_id": "numeric.enum_constant", "label_visible": False,
                   "value_type": scalar, "layout": bounds(20, 30, 148, 30),
                   "enum_constant": {"domain_id": domain, "selected_value": ordinal,
                       "items": [{"id": f"lv-ordinal-{i}", "value": i, "label": label} for i, label in enumerate(labels)]}}
        body = draft_diagram()
        body["nodes"].append(literal)
        body["edges"].append({"id": literal["id"] + "-out", "from": {"node": literal["id"], "port": "value"},
                              "to": {"node": case_id, "port": boundary_port("kind", "input")}})
        row = {"id": case_id + "-region-" + key, "title": token or "Default", "diagram": body,
               "matches": [] if token is None else [{"kind": "string_exact", "value": token}]}
        if token is None:
            row["default"] = True
        case["regions"].append(row)
    ports[term.uid] = (case_id, boundary_port("kind", "output"))
    edge = {"id": case_id + "-select", "from": {"node": origin, "port": "property:kind"},
            "to": {"node": case_id, "port": "selector"}}
    return read, ports, case, edge


def close_own_front_panel(node, graph, types, wired):
    """Translate LV20 VI.FP.Close on the containing VI, preserving its error chain."""
    if node.kind != "invokeNode":
        return None
    fields = node.fields
    signature = {"nodeName": b"VI", "methName": b"FP.Close", "methCode": b"\x04\x25",
                 "oMId": b"\x00\x6c", "invokeFlags": b"\x08"}
    if any(name not in fields or fields[name].payload != value for name, value in signature.items()):
        return None
    # Use the saved DCO roles. The display label of an input can also read
    # "reference out" in a saved VI and does not establish direction.
    roles = {}
    for uid in node.terminals:
        term = graph.terminals[uid]
        binding = graph.bindings.get(term.binding)
        if binding is None or binding.flags in roles:
            return None
        roles[binding.flags] = (term, binding)
    if set(roles) != {0x10000, 0x40001, 0x20000, 0x80001, 0x30000, 0x30001}:
        return None
    for role in (0x10000, 0x40001):
        term, binding = roles[role]
        schema = types.schema(binding)
        if (term.uid in wired or binding.kind != "hGrowCItem" or schema is None or
                schema.family != "Refnum" or schema.children or
                schema.detail != (8, (), (("ctlflags", 2), ("hasitem", 0), ("itmident", b"UNKN")))):
            return None
    for role in (0x30000, 0x30001):
        term, binding = roles[role]
        schema = types.schema(binding)
        if term.uid in wired or binding.kind != "invokeItem" or schema is None or schema.family != "Void":
            return None
    identity = f"lv-node-{node.uid}"
    ports = {}
    for role, name, direction in ((0x20000, "error_in", "sink"), (0x80001, "error_out", "source")):
        term, binding = roles[role]
        if binding.kind != "hGrowCItem" or types(binding) != _ERROR or (term.uid in wired and term.direction != direction):
            return None
        ports[term.uid] = (identity, name)
    return ({"id": identity, "kind": "primitive", "type": "frog.studio.application_control.invoke_node",
             "palette_catalog_id": "application_control.invoke_node", "label_visible": False,
             "studio_application_control": {"profile": "frog.studio.application_control@1", "kind": "invoke",
                 "expected_class": "frog.program", "method_id": "front_panel.close", "properties": [], "parameters": [],
                 "target": {"kind": "this_frog", "owner_document": "", "widget_id": ""},
                 "value_schema": {"canonical": "unknown", "children": [], "field_names": [],
                                  "choice_kind": "none", "choices": []}}}, ports)


_SERVICE_PORT_TYPES = {
    "name": "string", "create_if_not_found": "bool", "max_size": "i32",
    "timeout_ms": "i32", "timed_out": "bool", "created_new": "bool", "force_destroy": "bool",
    "error_in": _ERROR, "error_out": _ERROR,
}
# (LabVIEW resource, primitive index) -> native operation and saved parameter indices.
# An absent parmIndex encodes slot zero. Port names are the public FROG binding names.
_ENQUEUE = ("synchronization.queue.enqueue", {0: "queue", 1: "element", 2: "timeout_ms", 3: "error_in",
                                              8: "queue_out", 10: "timed_out", 11: "error_out"})
_ENQUEUE_AT_FRONT = (9129, 487)
_RESOURCE_PRIMITIVES = {
    (9108, 479): ("synchronization.queue.obtain", {0: "name", 1: "element_type", 2: "create_if_not_found",
                  3: "error_in", 4: "max_size", 8: "queue_out", 10: "created_new", 11: "error_out"}),
    (9109, 480): ("synchronization.queue.release", {0: "queue", 2: "force_destroy", 3: "error_in",
                  8: "name", 9: "remaining_elements", 11: "error_out"}),
    (9111, 482): _ENQUEUE,
    _ENQUEUE_AT_FRONT: _ENQUEUE,
    (9113, 485): ("synchronization.queue.dequeue", {0: "queue", 2: "timeout_ms", 3: "error_in",
                  8: "queue_out", 9: "element", 10: "timed_out", 11: "error_out"}),
    (2073, 495): ("dialog.events.create_user_event", {0: "data_type", 3: "error_in", 8: "user_event", 11: "error_out"}),
    (2074, 496): ("dialog.events.generate_user_event", {0: "user_event", 1: "data", 3: "error_in",
                  4: "priority", 8: "user_event_out", 11: "error_out"}),
    (2075, 497): ("dialog.events.destroy_user_event", {0: "user_event", 3: "error_in", 7: "error_out"}),
    (2076, 498): ("dialog.events.unregister_for_events", {0: "registration_in", 3: "error_in", 7: "error_out"}),
}


def cluster_to_array(node, graph, types, wired):
    """Bind the source Cluster and the element of the saved rank-one Array view."""
    if len(node.terminals) != 2:
        return None
    ports, schemas = {}, {}
    node_id = f"lv-node-{node.uid}"
    for uid in node.terminals:
        terminal = graph.terminals[uid]
        binding = graph.bindings.get(terminal.binding)
        if not binding or binding.kind != "parm":
            return None
        slot = binding.fields["parmIndex"].value if "parmIndex" in binding.fields else 0
        if slot not in (0, 1) or slot in schemas or bool((binding.flags or 0) & 1) != (slot == 0):
            return None
        if uid in wired and terminal.direction != ("source" if slot == 0 else "sink"):
            return None
        schemas[slot] = types.schema(binding)
        ports[uid] = (node_id, "array" if slot == 0 else "cluster")
    source, target = schemas[1], schemas[0]
    while source and source.family == "TypeDef" and len(source.children) == 1:
        source = source.children[0]
    if (not source or source.family != "Cluster" or not source.native_type(types.encoding) or
            not target or target.family not in ("Array", "SubArray") or
            target.detail != ((255, 0xFFFFFF),) or len(target.children) != 1):
        return None
    element = target.children[0]
    if not element.native_type(types.encoding) or any(
            field.representation() != element.representation() for field in source.children):
        return None
    operation = "array.conversion.cluster_to_array"
    return ({"id": node_id, "kind": "primitive", "type": "frog.studio." + operation,
             "palette_catalog_id": operation, "label_visible": False}, ports)


def merge_error_array(node, graph, types):
    """Fold one indexed Array of error records in the source element order."""
    if node.kind != "mergeErrors" or set(node.field_names) - {
            "termList", "bounds", "shortCount", "clumpNum", "dcoList", "dcoAgg"}:
        return None
    arguments, result = node.fields.get("dcoList"), node.fields.get("dcoAgg")
    if not arguments or len(arguments.children) != 1 or not result or len(node.terminals) != 2:
        return None
    terms = {graph.terminals[uid].binding: graph.terminals[uid] for uid in node.terminals}
    array, scalar = terms.get(arguments.children[0].uid), terms.get(result.uid)
    if not array or not scalar or array.direction != "sink" or scalar.direction != "source":
        return None
    array_type = f"array<{_ERROR}>"
    for term, expected in ((array, array_type), (scalar, _ERROR)):
        binding = graph.bindings.get(term.binding)
        if not binding or binding.kind != "mergeErrorsDCO" or types(binding) != expected:
            return None
    node_id = f"lv-node-{node.uid}"
    loop = structure(node_id, "for_loop")
    loop["structure_terminals"] = {
        key: _terminal_record(dtype, role, outer, inner, read_only)
        for key, dtype, role, outer, inner, read_only in (NATIVE_TERMINALS["iteration"], NATIVE_TERMINALS["count"])}
    for name, dtype, right, mode in (
            ("elements", array_type, False, {"tunnel_mode": "indexing"}),
            ("previous", _ERROR, False, {"shift_register_pair_id": "retained"}),
            ("result", _ERROR, True, {"shift_register_pair_id": "retained"})):
        loop["boundary"]["outputs" if right else "inputs"].append({"id": name, "type": dtype, **mode})
        loop["structure_tunnels"].append({
            "id": name, "side": "right" if right else "left", "normalized_position": 0.37 if name == "elements" else 0.67,
            "value_type": dtype, "output_from_structure": right, **mode})
    def wire(suffix, producer, source_port, consumer, target_port):
        return {"id": node_id + "-" + suffix, "from": {"node": producer, "port": source_port},
                "to": {"node": consumer, "port": target_port}}
    body = draft_diagram()
    fold = node_id + "-retain"
    body["nodes"].append({"id": fold, "kind": "primitive", "type": "frog.studio.error.retain_first",
                          "palette_catalog_id": "error.retain_first", "label_visible": False,
                          "retain_first_error_inputs": 2, "layout": bounds(72, 54, 72, 50)})
    body["edges"] = [wire("carry", node_id, boundary_port("previous", "output"), fold, "a"),
                     wire("element", node_id, boundary_port("elements", "output"), fold, "b"),
                     wire("next", fold, "error_out", node_id, boundary_port("result", "input"))]
    loop["regions"] = [{"id": "body", "diagram": body}]
    # The identity of this fold is a no-error value, also returned for an empty Array.
    seed = node_id + "-no-error"
    box = layout(node.bounds)
    helpers = [{"id": seed, "kind": "primitive", "type": "frog.studio.error.constant",
                "palette_catalog_id": "error.constant", "cluster_constant_collapsed": True,
                "label_visible": False, "layout": bounds(box["x"] - 96, box["y"] + 64, 62, 28)}]
    for order, (name, dtype, value) in enumerate((("status", "bool", False), ("code", "i32", 0), ("source", "string", ""))):
        field = convert_constant(seed + "-" + name, dtype, value, 8, 24 + order * 32)
        field.update(constant_owner_node_id=seed, constant_field_id=name, constant_field_order=order, label=name)
        helpers.append(field)
    initial = wire("initial", seed, "value", node_id, boundary_port("previous", "input"))
    return loop, {array.uid: (node_id, boundary_port("elements", "input")),
                  scalar.uid: (node_id, boundary_port("result", "output"))}, helpers, initial


def initialize_array_1d(node, graph, types):
    if node.kind != "aInit" or len(node.terminals) != 3:
        return None
    source, dimensions, output = (node.fields.get(key) for key in ("srcDCO", "dcoList", "dcoAgg"))
    if not source or not dimensions or len(dimensions.children) != 1 or not output:
        return None
    roles = {source.uid: "element", dimensions.children[0].uid: "size", output.uid: "result"}
    if None in roles or len(roles) != 3:
        return None
    ports, schemas = {}, {}
    owner = f"lv-node-{node.uid}"
    for uid in node.terminals:
        term = graph.terminals[uid]
        binding = graph.bindings.get(term.binding)
        role = roles.get(term.binding)
        if not role or role in schemas or not binding or binding.kind != "aInitDCO":
            return None
        direction = "source" if role == "result" else "sink"
        if term.direction != direction or bool((binding.flags or 0) & 1) != (role == "result"):
            return None
        schemas[role] = types.schema(binding)
        ports[uid] = (owner, role)
    array, element, size = (schemas[key] for key in ("result", "element", "size"))
    if (not array or array.family != "Array" or array.detail != ((255, 0xFFFFFF),) or len(array.children) != 1
            or not size or size.native_type(types.encoding) != "i32" or not element
            or element.native_type(types.encoding) != "bool" or element.representation() != array.children[0].representation()):
        return None
    return ({"id": owner, "kind": "primitive", "type": "frog.studio.array.initialize",
             "palette_catalog_id": "array.initialize", "label_visible": False}, ports)


def array_view_size_1d(node, graph, types):
    if len(node.terminals) != 2:
        return None
    owner = f"lv-node-{node.uid}"
    ports, roles = {}, set()
    for uid in node.terminals:
        term = graph.terminals[uid]
        binding = graph.bindings.get(term.binding)
        if not binding or binding.kind != "parm":
            return None
        slot = binding.fields.get("parmIndex")
        slot = slot.value if slot else 0
        if slot in roles or slot not in (0, 1) or term.direction != ("sink" if slot else "source"):
            return None
        if bool((binding.flags or 0) & 1) != (slot == 0):
            return None
        schema = types.schema(binding)
        if not schema:
            return None
        if slot == 0 and schema.native_type(types.encoding) != "i32":
            return None
        if slot == 1 and (schema.family not in ("Array", "SubArray") or schema.detail != ((255, 0xFFFFFF),)
                          or len(schema.children) != 1 or not schema.children[0].native_type(types.encoding)):
            return None
        roles.add(slot)
        ports[uid] = (owner, "array" if slot else "size")
    return ({"id": owner, "kind": "primitive", "type": "frog.studio.array.size",
             "palette_catalog_id": "array.size", "label_visible": False}, ports)


def primitive_identity(node):
    return tuple(node.fields[name].value if name in node.fields else None for name in ("primResID", "primIndex"))


# LV20 parameter numbers belong to the saved primitive signature. Variant To
# Data, in particular, does not number all its outputs after its inputs.
# A None schema denotes the source's exact, wired polymorphic type.
_VALUE_PRIMITIVES = {
    (1303, 132): ("timestamp.get_current_time", {0: ("current_time", "source", "timestamp")}),
    (8201, 371): ("variant.to_variant", {0: ("variant", "source", "variant"), 1: ("data", "sink", None)}),
    (8003, 348): ("variant.to_data", {
        0: ("error_in", "sink", _ERROR), 3: ("error_out", "source", _ERROR),
        8: ("variant", "sink", "variant"), 9: ("type", "sink", None), 11: ("data", "source", None)}),
}


def value_primitive(node, graph, types, wired, signature):
    operation, slots = signature
    owner = f"lv-node-{node.uid}"
    ports, seen, polymorphic_types = {}, set(), []
    for uid in node.terminals:
        term = graph.terminals[uid]
        binding = graph.bindings.get(term.binding)
        if not binding or binding.kind != "parm":
            return None
        slot = binding.fields["parmIndex"].value if "parmIndex" in binding.fields else 0
        if slot in seen or slot not in slots:
            return None
        seen.add(slot)
        port, direction, dtype = slots[slot]
        if bool((binding.flags or 0) & 1) != (direction == "source") or (uid in wired and term.direction != direction):
            return None
        if direction == "sink" and port != "error_in" and uid not in wired:
            return None
        actual = types(binding)
        if actual == "unknown" or dtype is not None and actual != dtype:
            return None
        if dtype is None:
            polymorphic_types.append(types.schema(binding).representation())
        ports[uid] = owner, port
    if seen != slots.keys() or any(t != polymorphic_types[0] for t in polymorphic_types[1:]):
        return None
    return ({"id": owner, "kind": "primitive", "type": "frog.studio." + operation,
             "palette_catalog_id": operation, "label_visible": False}, ports)


def append_array(node, graph, types):
    if node.kind != "aBuild":
        return None
    operands, result = node.fields.get("dcoList"), node.fields.get("dcoAgg")
    if not operands or not result:
        return None
    terminals = []
    for uid in node.terminals:
        term = graph.terminals[uid]
        binding = graph.bindings.get(term.binding)
        if not binding or term.direction not in ("source", "sink"):
            return None
        terminals.append((uid, binding.uid, binding.kind, types(binding), binding.flags or 0, term.direction == "source"))
    ports = append_ports(node.flags, [v.uid for v in operands.children], result.uid, terminals)
    if not ports:
        return None
    node_id = f"lv-node-{node.uid}"
    return ({"id": node_id, "kind": "primitive", "type": "frog.studio.array.build", "palette_catalog_id": "array.build",
             "build_array_inputs": 2, "build_array_concatenate": False, "label_visible": False},
            {uid: (node_id, port) for uid, port in ports.items()})


def resource_primitive(node, graph, types, wired):
    if node.kind != "prim":
        return None
    key = primitive_identity(node)
    if key in _VALUE_PRIMITIVES:
        return value_primitive(node, graph, types, wired, _VALUE_PRIMITIVES[key])
    if key in OPERATIONS:
        kind_, palette, _, _, params = OPERATIONS[key]
        obj = {"id": f"lv-node-{node.uid}", "kind": "primitive", "type": kind_,
               "palette_catalog_id": palette, "label_visible": False}
        slots = dict(params)
        slots[0] = "result" if params else "count"
        assigned = {}
        representations = {}
        for uid in node.terminals:
            term = graph.terminals[uid]
            binding = graph.bindings.get(term.binding)
            if binding is None:
                return None
            slot = binding.fields["parmIndex"].value if "parmIndex" in binding.fields else 0
            overridden = binding.fields.get("override")
            expected = result_binding(key) if slot == 0 else ("parm", None)
            if (slot not in slots or slot in assigned or term.direction != ("sink" if slot else "source")
                    or (binding.kind, overridden.value if overridden else None) != expected):
                return None
            assigned[slot] = uid
            representations[slot] = types(binding)
        if assigned.keys() == slots.keys() and matches_operation_types(
                key, [representations[slot] for slot, _ in params], representations[0]):
            return obj, {uid: (obj["id"], slots[slot]) for slot, uid in assigned.items()}
        return None
    if key == (1809, 139):
        return array_view_size_1d(node, graph, types)
    if key == (1169, 162):
        return cluster_to_array(node, graph, types, wired)
    if key not in _RESOURCE_PRIMITIVES:
        return None
    operation, slots = _RESOURCE_PRIMITIVES[key]
    node_id = f"lv-node-{node.uid}"
    ports, schemas, seen = {}, {}, set()
    for uid in node.terminals:
        terminal = graph.terminals[uid]
        binding = graph.bindings.get(terminal.binding)
        if binding is None or binding.kind != "parm":
            return None
        slot = binding.fields["parmIndex"].value if "parmIndex" in binding.fields else 0
        if slot not in slots or slot in seen:
            return None
        seen.add(slot)
        port = slots[slot]
        output = slot >= 7
        if bool((binding.flags or 0) & 1) != output:
            return None
        if uid in wired and terminal.direction != ("source" if output else "sink"):
            return None
        dtype = types(binding)
        if dtype == "unknown" or dtype != _SERVICE_PORT_TYPES.get(port, dtype):
            return None
        if port == "priority":
            # Both services default to normal. A wired LV enum requires a typed conversion.
            if uid in wired or types.enums.get(dtype, (None, None, []))[2] != ["high priority", "normal priority"]:
                return None
        if not output and uid not in wired and port in ("queue", "element", "element_type", "data_type", "data", "user_event", "registration_in"):
            return None
        schemas[port] = dtype
        ports[uid] = (node_id, port)
    if seen != slots.keys():
        return None
    is_queue = operation.startswith("synchronization.queue.")
    ref_ports = ("queue", "queue_out") if is_queue else ("user_event", "user_event_out")
    prefix = 'ref<"frog.sync.queue",' if is_queue else 'ref<"frog.events.user",'
    if operation == "dialog.events.unregister_for_events":
        ref_ports, prefix = ("registration_in",), 'ref<"frog.events.registration",'
    references = {schemas[name] for name in ref_ports if name in schemas}
    if len(references) != 1:
        return None
    reference = references.pop()
    if not reference.startswith(prefix) or not reference.endswith(">"):
        return None
    payload = reference[len(prefix):-1]
    if "remaining_elements" in schemas and schemas["remaining_elements"] != f"array<{payload}>":
        return None
    if any(schemas[name] != payload for name in ("element", "element_type", "data_type", "data") if name in schemas):
        return None
    return ({"id": node_id, "kind": "primitive", "type": "frog.studio." + operation,
             "palette_catalog_id": operation, "label_visible": False}, ports)


def marker(node_id, title, terms, graph, types, reason, rect, connector=()):
    """A non-executable editable object. Every visible port has a saved source UID."""
    ports, endpoints = [], {}
    saved_ports = {row["terminal_uid"]: row for row in connector}
    for source, direction in (("source", "output"), ("sink", "input")):
        side = [graph.terminals[t] for t in terms if (saved_ports[t]["direction"] == direction if t in saved_ports
                                                     else graph.terminals[t].direction == source)]
        for index, terminal in enumerate(side):
            binding = graph.bindings.get(terminal.binding)
            key = f"lv-port-{terminal.uid}"
            label = binding.type_label if binding and binding.type_label else f"Terminal {terminal.uid}"
            declaration = saved_ports.get(terminal.uid, {})
            ports.append({"id": key, "source_port_id": key, "direction": direction,
                          "label": declaration.get("label", label), "value_type": declaration.get("value_type") or types(binding),
                          "normalized_x": int(source == "source"), "normalized_y": (index + 0.5) / len(side)})
            endpoints[terminal.uid] = (node_id, key)
    obj = {"id": node_id, "kind": "primitive", "type": "frog.studio.import.labview.untranslated",
           "label": title, "label_visible": True, "documentation": {"summary": reason},
           "call": {"ports": ports}, "layout": layout(rect, 92, max(36, len(ports) * 6))}
    return obj, endpoints


def fit_regions(diagram):
    """Place each saved coordinate space inside its owner, allowing for native node sizes."""
    nodes = [obj for obj in diagram["nodes"] if not obj.get("constant_owner_node_id")]
    for obj in nodes:
        box = obj["layout"]
        for region in obj.get("regions", ()):
            width, height = fit_regions(region["diagram"])
            box["width"] = max(box["width"], width + 112)
            box["height"] = max(box["height"], height + 96)
    if not nodes:
        return 0, 0
    used = set()
    for obj in nodes:
        box = obj["layout"]
        while (box["x"], box["y"]) in used:
            box["y"] += box["height"] + 24
        used.add((box["x"], box["y"]))
    # Expand source coordinate intervals only where native objects need more room.
    # Preserve an aligned row or column when its source coordinates coincide.
    for axis, extent in (("x", "width"), ("y", "height")):
        starts = sorted({obj["layout"][axis] for obj in nodes})
        arranged = {pos: pos - starts[0] for pos in starts}
        other, other_size = ("y", "height") if axis == "x" else ("x", "width")
        for pos in starts:
            for obj in (o for o in nodes if o["layout"][axis] == pos):
                box = obj["layout"]
                for following in nodes:
                    next_box = following["layout"]
                    next_pos = next_box[axis]
                    if next_pos <= pos:
                        continue
                    if box[other] < next_box[other] + next_box[other_size] and next_box[other] < box[other] + box[other_size]:
                        arranged[next_pos] = max(arranged[next_pos], arranged[pos] + box[extent] + 32)
        for obj in nodes:
            obj["layout"][axis] = arranged[obj["layout"][axis]]
    fields = {}
    for field in diagram["nodes"]:
        if owner := field.get("constant_owner_node_id"):
            fields.setdefault(owner, []).append(field)
    owners = list(nodes)
    while owners:
        owner = owners.pop()
        for field in fields.pop(owner["id"], ()):
            field["layout"]["x"] += owner["layout"]["x"]
            field["layout"]["y"] += owner["layout"]["y"]
            owners.append(field)
    return (max(obj["layout"]["x"] + obj["layout"]["width"] for obj in nodes),
            max(obj["layout"]["y"] + obj["layout"]["height"] for obj in nodes))


def offset_regions(diagram, x, y):
    for obj in diagram["nodes"]:
        box = obj["layout"]
        box["x"] += x
        box["y"] += y
        for region in obj.get("regions", ()):
            offset_regions(region["diagram"], box["x"] + 56, box["y"] + 56)


def display_targets(vi, displays, types):
    if not displays or b"FPHb" not in vi.blocks:
        return {}
    root = field(heap_root(vi, b"FPHb"), "root", "FPHb")
    pane = field(root, "paneHierarchy", "FPHb")
    targets = {}
    for widget in items(field(pane, "zPlaneList", "FPHb"), "FPHb"):
        if kind(widget) != "fPDCO":
            continue
        body = field(widget, "ddo", "FPHb", optional=True)
        if body is None:
            continue
        display = body.attribs.get(ATTR_UID)
        if display not in displays:
            continue
        require(display not in targets, "control_reference_target", "Multiple controls own the same display.", "FPHb")
        uid = identity(widget, "FPHb")
        location = f"FPHb/uid:{uid}"
        target = {"display_uid": display, "widget_uid": uid}
        try:
            td = types.table.heap(widget, "typeDesc", location)
            target["value_type"] = types.table.describe(td, location).native_type(types.encoding)
        except ProfileRefusal as exc:
            target["type_diagnostic"] = str(exc)
        targets[display] = target
    return targets


def static_control_targets(vi, graph, types):
    references = {node.uid: node.fields["ddo"].uid for node in graph.nodes.values()
                  if node.kind == "ctlRefConst" and "ddo" in node.fields and node.fields["ddo"].uid}
    targets = display_targets(vi, set(references.values()), types)
    return {node: targets[display] for node, display in references.items() if display in targets}


def boolean_control_access(node, graph, types, wired, target, panel):
    """Keep a static control's DDO identity and a wired Boolean Visible property's error chain."""
    if node.kind not in ("ctlRefConst", "propNode"):
        return None
    uid = f"lv-node-{node.uid}"
    terms = [graph.terminals[t] for t in node.terminals]
    ref = 'ref<"frog.control.boolean",bool>'
    if node.kind == "ctlRefConst":
        widget = panel.get(target["widget_uid"]) if target else None
        if (not widget or len(terms) != 1 or terms[0].direction != "source"
                or target.get("value_type") != "bool" or widget["valueType"] != "bool"):
            return None
        binding = graph.bindings.get(terms[0].binding)
        if not binding or binding.kind != "ctlRefDCO" or types(binding) != ref:
            return None
        kind, operation, rows = "reference", "reference", []
        address = {"kind": "widget", "owner_document": "", "widget_id": widget["id"]}
        ports = {terms[0].uid: (uid, "reference_out")}
        value = types.schema(binding).value_children()[0]
    else:
        name = node.fields.get("nodeName")
        if not name or name.payload != b"Bool" or len(terms) != 5:
            return None
        slots = {0x10000: ("reference_in", "sink", ref), 0x40001: ("reference_out", "source", ref),
                 0x20000: ("error_in", "sink", _ERROR), 0x80001: ("error_out", "source", _ERROR)}
        ports = {}
        for term in terms:
            binding = graph.bindings.get(term.binding)
            if not binding:
                return None
            if binding.kind == "propItem":
                prop = binding.fields.get("propList")
                if not prop or len(prop.children) != 1 or prop.children[0].kind != "propItemInfo":
                    return None
                fields = {f.tag: f.payload for f in prop.children[0].children}
                if (fields.get("PropItemName") != b"Visible" or fields.get("PropItemCode") != b"\x063\x20\x00"
                        or binding.flags not in (None, 0) or term.uid not in wired):
                    return None
                port, direction, dtype = "property:visible", "sink", "bool"
                value = types.schema(binding)
            elif binding.kind == "hGrowCItem" and binding.flags in slots:
                port, direction, dtype = slots[binding.flags]
            else:
                return None
            if (types(binding) != dtype or (term.uid in wired and term.direction != direction)
                    or (port == "reference_in" and term.uid not in wired)):
                return None
            ports[term.uid] = (uid, port)
        if {port for _, port in ports.values()} != {"reference_in", "reference_out", "error_in", "error_out", "property:visible"}:
            return None
        kind, operation = "property", "property_node"
        rows = [{"id": "visible", "member_id": "interaction.visible", "mode": "write"}]
        address = {"kind": "unbound", "owner_document": "", "widget_id": ""}
    catalog = "application_control." + operation
    return ({"id": uid, "kind": "primitive", "type": "frog.studio." + catalog, "palette_catalog_id": catalog,
             "label_visible": False, "studio_application_control": {"profile": "frog.studio.application_control@1",
             "kind": kind, "expected_class": "frog.control.boolean", "method_id": "", "properties": rows,
             "parameters": [], "target": address, "value_schema": resource_payload(value, types.encoding)}}, ports)


def project_document(source, vi, panel, panel_notes, calls, graph, output, member_class=None, class_refs=None):
    diagrams = {uid: draft_diagram() for uid in graph.diagrams}
    types = BindingTypes(vi)
    control_targets = static_control_targets(vi, graph, types)
    loop_modes = loop_tunnel_modes(graph, types) if vi.getFileVersion()["major"] == 20 else {}
    notes = list(panel_notes)
    endpoints, signals, source_nodes, expansions = {}, {}, {}, {}
    field_readers = {}
    wired = {term for signal in graph.signals.values() for term in signal.endpoints}
    event_displays = {node.uid: event_control_displays(node) for node in graph.nodes.values() if node.kind == "eventStruct"}
    display_owners = display_targets(vi, {display for row in event_displays.values() for display in row.values()}, types)
    event_controls = {}
    for owner, by_index in event_displays.items():
        event_controls[owner] = {}
        for region in graph.nodes[owner].regions:
            target = display_owners.get(by_index.get(graph.diagrams[region].index))
            if target and target["widget_uid"] in panel:
                event_controls[owner][region] = panel[target["widget_uid"]]
    close_filters = {node.uid: panel_close_responses(graph, node, types, wired)
                     for node in graph.nodes.values() if node.kind == "eventStruct"}
    response_owners = {response: (owner, region, terminal)
                       for owner, regions in close_filters.items() for region, (response, terminal) in regions.items()}
    diagnostics = {entry["uid"]: entry for group in projection_requirement_diagnostics(vi)
                   for entry in group["occurrences"] if entry["source"].startswith("BDHb/")}

    def insert(diagram_uid, obj, ports):
        require(not (endpoints.keys() & ports.keys()), "duplicate_terminal", "Two objects claim a terminal.", "BDHb")
        diagrams[diagram_uid]["nodes"].append(obj)
        endpoints.update(ports)

    for node in graph.nodes.values():
        if node.kind == "sRN":
            continue
        if node.uid in response_owners:
            owner, region, terminal = response_owners[node.uid]
            owner_id = f"lv-node-{owner}"
            require(terminal not in endpoints, "duplicate_terminal", "Two objects claim the filter response terminal.", "BDHb")
            endpoints[terminal] = (owner_id, "event_response.discard")
            source_nodes[node.uid] = {"node": owner_id, "class": node.kind, "diagram": region,
                                      "bounds": node.bounds, "terminals": list(node.terminals),
                                      "fields": {name: source_field(info) for name, info in node.fields.items()},
                                      "response_port": "event_response.discard"}
            continue
        call, ports, call_notes, defaults = calls.get(node.uid, (None, {}, [], []))
        notes.extend(call_notes)
        merge = merge_error_ports(graph, node) if node.kind == "mergeErrors" else None
        translated = (resource_primitive(node, graph, types, wired) or append_array(node, graph, types) or initialize_array_1d(node, graph, types)
                      or named_cluster_operation(node, graph, types) or close_own_front_panel(node, graph, types, wired)
                      or boolean_control_access(node, graph, types, wired, control_targets.get(node.uid), panel))
        private = private_field_nodes(node, graph, types, *member_class, wired) if member_class else None
        if node.regions and node.kind in STRUCTURE_KINDS:
            obj, ports, region_notes = project_region_owner(graph, node, diagrams, types, close_responses=close_filters.get(node.uid),
                                                           event_controls=event_controls.get(node.uid),
                                                           loop_modes=loop_modes)
            notes.extend(region_notes)
        elif call is not None:
            obj = call
            input_y = {p["id"]: p["normalized_y"] for p in obj["call"]["ports"] if p["direction"] == "input"}
            bottom = None
            for slot, _, dtype, value in sorted(defaults, key=lambda entry: input_y[entry[0]]):
                literal = convert_constant(f"call-{node.uid}-{slot}", dtype, value, 0, 0)
                box = literal["layout"]
                box["x"] = node.bounds[0] - box["width"] - 20
                y = node.bounds[1] + (node.bounds[3] - node.bounds[1]) * input_y[slot] - box["height"] / 2
                box["y"] = y if bottom is None else max(bottom + 6, y)
                bottom = box["y"] + box["height"]
                diagrams[node.diagram]["nodes"].append(literal)
                diagrams[node.diagram]["edges"].append({
                    "id": literal["id"] + "-wire", "from": {"node": literal["id"], "port": "value"},
                    "to": {"node": obj["id"], "port": slot}})
        elif opened := (front_panel_open(node, graph, types, wired) or register_user_event(node, graph, types, wired)):
            obj, ports, helpers, links = opened
            diagrams[node.diagram]["nodes"].extend(helpers)
            diagrams[node.diagram]["edges"].extend(links)
        elif app_kind := application_kind(node, graph, types, wired):
            obj, ports, selector, edge = app_kind
            diagrams[node.diagram]["nodes"].append(selector)
            diagrams[node.diagram]["edges"].append(edge)
        elif translated is not None:
            obj, ports = translated
            if obj.get("palette_catalog_id") == _ENQUEUE[0] and primitive_identity(node) == _ENQUEUE_AT_FRONT:
                box = layout(node.bounds)
                option = convert_constant(f"{node.uid}-at-front", "bool", True, 0, 0)
                option["layout"]["x"] = box["x"] - option["layout"]["width"] - 24
                option["layout"]["y"] = box["y"] + box["height"] + 8
                diagrams[node.diagram]["nodes"].append(option)
                diagrams[node.diagram]["edges"].append({
                    "id": option["id"] + "-wire", "from": {"node": option["id"], "port": "value"},
                    "to": {"node": obj["id"], "port": "at_front"}})
        elif private:
            operations, links, ports, receiver, readers = private
            obj = operations[0]
            for index, op in enumerate(operations[1:], 1):
                op["layout"] = layout(node.bounds)
                if readers:
                    op["layout"]["y"] += index * (op["layout"]["height"] + 24)
                else:
                    op["layout"]["x"] += index * (op["layout"]["width"] + 32)
            diagrams[node.diagram]["nodes"].extend(operations[1:])
            diagrams[node.diagram]["edges"].extend(links)
            field_readers[receiver] = readers
        elif folded := merge_error_array(node, graph, types):
            obj, ports, helpers, initial = folded
            diagrams[node.diagram]["nodes"].extend(helpers)
            diagrams[node.diagram]["edges"].append(initial)
        elif merge is not None:
            obj = {"id": f"lv-node-{node.uid}", "kind": "primitive", "type": "frog.studio.error.retain_first",
                   "palette_catalog_id": "error.retain_first", "label_visible": False,
                   "retain_first_error_inputs": len(merge) - 1}
            ports = {uid: (obj["id"], name) for uid, name in merge.items()}
        else:
            evidence = diagnostics.get(node.uid, {})
            title = evidence.get("property") or ".".join(filter(None, (evidence.get("reference_class"), evidence.get("method"))))
            title = title or next((n.get("target") or n.get("name") for n in call_notes if n.get("target") or n.get("name")), None) or node.kind
            if node.kind == "prim" and len(node.terminals) == 1 and is_quit_application(
                    node, graph.bindings.get(graph.terminals[node.terminals[0]].binding)):
                title = "Quit LabVIEW"
            reason = f"LabVIEW {title} ({node.kind}), source UID {node.uid}. Execution requires a native implementation."
            for region in node.regions:
                name = output.parent / (output.stem + ".regions") / f"diagram-{region}.frog"
                expansions[region] = name
                reason += f" Saved internal diagram {region}: {name.as_posix()}."
            if call_notes and call_notes[0].get("member"):
                reason += " Source member: " + call_notes[0]["member"]
            connector = next((note["ports"] for note in call_notes if note.get("ports")), ())
            terms = node.terminals
            if connector:
                assigned = {row["terminal_uid"] for row in connector}
                require(set(terms) & wired <= assigned, "call_binding",
                        "A wired terminal is absent from its saved connector.", f"BDHb/uid:{node.uid}")
                terms = [row["terminal_uid"] for row in connector]
            obj, ports = marker(f"lv-node-{node.uid}", str(title), terms, graph, types, reason, node.bounds, connector)
            notes.append({"node_uid": node.uid, "code": "untranslated_operation", "object": str(title),
                          "message": reason})
        obj.setdefault("layout", layout(node.bounds))
        source_nodes[node.uid] = {"node": obj["id"], "class": node.kind, "diagram": node.diagram, "bounds": node.bounds,
                                  "fields": {name: source_field(info) for name, info in node.fields.items()}}
        if node.uid in control_targets:
            source_nodes[node.uid]["static_control_target"] = control_targets[node.uid]
        if defaults:
            source_nodes[node.uid]["callee_defaults"] = {slot: widget for slot, widget, _, _ in defaults}
        insert(node.diagram, obj, ports)

    for terminal in graph.terminals.values():
        if terminal.uid in endpoints:
            continue
        if terminal.uid not in wired and graph.nodes[terminal.owner].kind != "sRN":
            continue
        location = f"BDHb/uid:{terminal.uid}"
        binding = graph.bindings.get(terminal.binding)
        widget = panel.get(binding.uid) if binding and binding.kind == "fPDCO" else None
        obj = None
        if widget is not None:
            obj = {"id": f"lv-terminal-{terminal.uid}", "kind": "widget_value", "widget": widget["id"],
                   "type": panel_terminal_type(widget), "label": widget["label"]}
        elif binding and binding.kind == "bDConstDCO":
            try:
                obj = constant(terminal.uid, binding, types)
                if obj is None:
                    literal = cluster_literal(terminal.uid, binding, types, class_refs or {})
                    if literal:
                        obj, fields = literal
                        diagrams[terminal.diagram]["nodes"].extend(fields)
            except ProfileRefusal as error:
                notes.append({"resource": location, "message": str(error)})
        rect = terminal.bounds or (binding.bounds if binding else None)
        if obj is not None:
            obj["layout"] = layout(rect, 96 if "enum_constant" in obj else 64, 30)
            ports = {terminal.uid: (obj["id"], "value")}
        else:
            title = binding.type_label or binding.kind if binding else "Terminal"
            reason = f"LabVIEW {title}, terminal UID {terminal.uid} in diagram {terminal.diagram}. Review the original binding."
            obj, ports = marker(f"lv-terminal-{terminal.uid}", title, [terminal.uid], graph, types, reason, rect)
            notes.append({"resource": location, "code": "terminal_reference", "object": title, "message": reason})
        insert(terminal.diagram, obj, ports)

    for signal in graph.signals.values():
        if signal.source is None or not signal.targets:
            reason = "The source does not identify a unique producer and its consumers. Review the saved signal."
            obj, _ = marker(f"lv-unresolved-signal-{signal.uid}", f"LabVIEW signal {signal.uid}", (), graph, types, reason, None)
            diagrams[signal.diagram]["nodes"].append(obj)
            notes.append({"resource": f"BDHb/uid:{signal.uid}", "code": "signal_orientation", "message": reason})
            signals[signal.uid] = {"diagram": signal.diagram, "endpoints": signal.endpoints, "edges": [], "state": signal.state}
            continue
        require(signal.source in endpoints and signal.targets and all(t in endpoints for t in signal.targets),
                "signal_orientation", "An oriented signal or a saved terminal could not be projected.", f"BDHb/uid:{signal.uid}")
        ids = []
        for target in signal.targets:
            wire = f"lv-wire-{signal.uid}-{target}"
            a, b = endpoints[signal.source], endpoints[target]
            diagrams[signal.diagram]["edges"].append({"id": wire, "from": {"node": a[0], "port": a[1]},
                                                       "to": {"node": b[0], "port": b[1]}})
            ids.append(wire)
            for reader, port in field_readers.get(target, ()):
                branch = wire + "-" + reader
                diagrams[signal.diagram]["edges"].append({"id": branch, "from": {"node": a[0], "port": a[1]},
                                                          "to": {"node": reader, "port": port}})
                ids.append(branch)
        signals[signal.uid] = {"diagram": signal.diagram, "source": signal.source, "targets": signal.targets,
                               "state": signal.state, "edges": ids}
        if signal.route_data:
            signals[signal.uid]["source_route_base64"] = base64.b64encode(signal.route_data).decode("ascii")
    root = diagrams[graph.root]
    require(all(len(d["nodes"]) <= 1024 for d in diagrams.values()), "layout_budget",
            "A diagram exceeds the local layout budget.", "BDHb")
    for uid in [graph.root, *expansions]:
        fit_regions(diagrams[uid])
        offset_regions(diagrams[uid], 96, 64)
    document = {"format": "frog.document.draft", "draft_revision": 3, "language": "FROG", "title": source.stem,
                "metadata": {"name": source.stem, "description": f"Source (LabVIEW): {source}\nSee labview-mapping.json for translation details."},
                "diagram": root, "frontPanel": {"widgets": list(panel.values())}, "storage": {"version": 1, "array_mode": "automatic"}}
    mapping = {"source_nodes": source_nodes, "diagrams": {i: {"owner": d.owner, "index": d.index} for i, d in graph.diagrams.items()},
               "endpoints": {i: {"node": p[0], "port": p[1], "binding": graph.terminals[i].binding} for i, p in endpoints.items()},
               "bindings": {b.uid: {"class": b.kind, "resource": b.resource, "source_type_id": b.type_id,
                                     "source_type": b.type_kind, "source_type_code": b.type_code,
                                     "fields": {key: source_field(value) for key, value in b.fields.items()}}
                            for b in graph.bindings.values()},
               "signals": signals, "notes": notes, "reader_issues": [asdict(issue) for issue in graph.issues]}
    mapping["detached_regions"] = {uid: path.as_posix() for uid, path in expansions.items()}
    members = {}
    for uid, path in expansions.items():
        title = f"{source.stem} — LabVIEW diagram {uid}"
        reason = f"Saved internal diagram of LabVIEW node {graph.diagrams[uid].owner}. Its host supplies the boundary."
        warning, _ = marker(f"lv-region-host-{uid}", "LabVIEW host required", (), graph, None, reason, None)
        diagrams[uid]["nodes"].append(warning)
        members[path] = {**document, "title": title,
                         "metadata": {"name": title, "description": f"Source (LabVIEW): {source}\nDiagram {uid}. {reason}"},
                         "diagram": diagrams[uid], "frontPanel": {"widgets": []}}
    return document, mapping, members


def project_editable(source, vi, load=None, class_readers=None):
    source = Path(source).resolve()
    bundle = resolve_bundle(source, vi, load, class_readers) if load else Bundle(source.parent)
    if not load:
        bundle[source] = vi, Path("model.frog")
    # Class identities are now resolved for this snapshot. Share the immutable
    # connector between its panel and every caller; a new import gets a new cache.
    read_interface = cache(read_connector)
    graphs, panels, panel_lists, presentations, presentation_evidence, notes, skipped = {}, {}, {}, {}, {}, {}, {}
    for path, (member, output) in bundle.items():
        try:
            graph = decode_source_diagram(member)
            widgets, issues = project_panel(member)
            panel_lists[path] = list(widgets.values())
            presentation, evidence, binding_issues = member_presentation(
                member, panel_lists[path], path.stem, class_method=path in bundle.members, read_interface=read_interface)
            presentations[path], presentation_evidence[path] = presentation, evidence
            issues.extend(binding_issues)
            graphs[path], panels[path], notes[path] = graph, widgets, issues
        except ProfileRefusal as error:
            if path == source:
                raise
            skipped[output.as_posix()] = {"resource": error.resource, "reason": str(error)}
    available = Bundle(source.parent)
    available.update({path: record for path, record in bundle.items() if path in graphs})
    available.classes.update(bundle.classes)
    available.members.update(bundle.members)
    available.unavailable.update(getattr(bundle, "unavailable", {}))
    for path in bundle:
        if path not in graphs:
            available.unavailable[path] = skipped[bundle[path][1].as_posix()]["reason"]
    documents, maps = {}, {}
    for path, (member, output) in available.items():
        calls = project_calls(member, graphs[path], path, available, panel_lists, presentations, read_interface)
        member_class = None
        if path in bundle.members:
            class_id = bundle.members[path]["class_id"]
            relative, cls = next((p, c) for p, c in bundle.classes.items() if c["id"] == class_id)
            member_class = ({"id": class_id, "revision": cls["revision"],
                             "path": Path(os.path.relpath(relative, output.parent)).as_posix()}, cls)
        class_refs = {cls["id"]: {"id": cls["id"], "revision": cls["revision"],
                      "path": Path(os.path.relpath(relative, output.parent)).as_posix()}
                      for relative, cls in bundle.classes.items()}
        document, mapping, regions = project_document(path, member, panels[path], notes[path], calls, graphs[path],
                                                       output, member_class, class_refs)
        require(not (regions.keys() & documents.keys()) and not (regions.keys() & {p for _, p in available.values()}),
                "dependency_output_collision", "An internal diagram path collides with another member.", "BDHb")
        documents.update(regions)
        document.update(presentations[path])
        class_ids = set(getattr(member, "local_class_ids", {}).values())
        referenced_classes = {p: cls for p, cls in bundle.classes.items()
                              if path == source or cls["id"] in class_ids}
        if referenced_classes:
            document["classes"] = [{"id": definition["id"], "revision": definition["revision"],
                                    "path": Path(os.path.relpath(relative, output.parent)).as_posix()}
                                   for relative, definition in referenced_classes.items()]
        if path in bundle.members:
            document["class_member"] = bundle.members[path]
        if "classes" in document or "class_member" in document:
            # The native reader admits by-value class declarations from draft 10.
            document["draft_revision"] = max(document["draft_revision"], 10)
        mapping["presentation"] = presentation_evidence[path]
        # If every operation happens to have a rendering, untranslated semantics
        # can still exist in the VI's panel, entry point, or region conditions.
        qualified = (queue_service_profile(document, mapping) or user_event_service_profile(document, mapping)
                     or clocked_value_member(document, mapping))
        if qualified:
            mapping["execution_profile"] = qualified
        elif not any(obj.get("type") == "frog.studio.import.labview.untranslated"
                     for obj in walk_nodes(document["diagram"])):
            reason = "The complete execution semantics of this imported VI require review."
            obj, _ = marker("lv-review-required", "LabVIEW execution review", (), graphs[path], None, reason, None)
            obj["layout"].update(x=96, y=0)
            document["diagram"]["nodes"].append(obj)
        maps[output.as_posix()] = mapping
        documents[output] = document
    root_map = maps.pop("model.frog")
    root_map.update(profile="labview-editable-source-v1", dependencies=maps, skipped_members=skipped)
    root_map["unreadable_members"] = {p.relative_to(source.parent).as_posix(): str(err) for p, err in available.unavailable.items()}
    root_map["class_diagnostics"] = bundle.class_issues
    root_map["verified_class_typedefs"] = bundle.typedefs
    documents.update(bundle.classes)
    return documents, root_map


def queue_service_profile(document, mapping):
    """Recognize complete queue wrappers, including the Boolean priority switch."""
    if mapping["reader_issues"] or mapping["notes"]:
        return None
    if len(mapping["diagrams"]) == 3:
        root = [n for n in document["diagram"]["nodes"] if n["kind"] != "widget_value" and not n.get("constant_owner_node_id")]
        if sorted(n.get("palette_catalog_id", "") for n in root) != [
                "cluster.bundle_by_name", "cluster.constant", "structures.case_structure"]:
            return None
        case = next(n for n in root if n["palette_catalog_id"] == "structures.case_structure")
        branches = set()
        for region in case["regions"]:
            nodes = region["diagram"]["nodes"]
            writers = [n for n in nodes if n.get("palette_catalog_id") == _ENQUEUE[0]]
            if len(writers) != 1:
                return None
            writer = writers[0]["id"]
            edges = region["diagram"]["edges"]
            inputs = {e["to"]["port"]: e["from"] for e in edges if e["to"]["node"] == writer}
            priority = "at_front" in inputs
            if region.get("matches") != [{"kind": "boolean_exact", "value": priority}]:
                return None
            if priority:
                literal = next((n for n in nodes if n["type"] == "frog.studio.boolean.true_constant"), None)
                if not literal or inputs.pop("at_front") != {"node": literal["id"], "port": "value"}:
                    return None
            if (len(nodes) != 1 + priority or set(inputs) != {"queue", "element", "error_in"}
                    or any(source["node"] != case["id"] for source in inputs.values())
                    or not any(e["from"] == {"node": writer, "port": "error_out"}
                               and e["to"]["node"] == case["id"] for e in edges)):
                return None
            branches.add(priority)
        return "labview-queue-priority-writer-v1" if branches == {True, False} else None
    if len(mapping["diagrams"]) != 1:
        return None
    operations = []
    for obj in document["diagram"]["nodes"]:
        if obj["kind"] == "widget_value" or obj.get("constant_owner_node_id"):
            continue
        operations.append(obj.get("palette_catalog_id"))
    if (operations.count("synchronization.queue.dequeue") == 1 and operations.count("cluster.unbundle_by_name") == 1 and
            all(op in ("synchronization.queue.dequeue", "cluster.unbundle_by_name", "error.retain_first") for op in operations)):
        return "labview-queue-reader-v1"
    if len(operations) == 2 and set(operations) == {"cluster.constant", "synchronization.queue.obtain"}:
        return "labview-queue-obtain-v1"
    return None


def user_event_service_profile(document, mapping):
    """Recognize fully mapped Boolean event emitters and cleanup wrappers."""
    if mapping["reader_issues"] or mapping["notes"]:
        return None
    interface = document["interface"]
    event_ref = 'ref<"frog.events.user",bool>'
    if (sorted(p["type"] for p in interface["inputs"]) != sorted((_ERROR, event_ref)) or
            [p["type"] for p in interface["outputs"]] != [_ERROR]):
        return None
    operations = sorted(node["type"] for node in walk_nodes(document["diagram"]) if node["kind"] != "widget_value")
    if len(mapping["diagrams"]) == 1 and operations == sorted((
            "frog.studio.dialog.events.destroy_user_event", "frog.studio.error.retain_first")):
        return "labview-user-event-destroy-v1"
    if len(mapping["diagrams"]) == 3 and operations == sorted((
            "frog.studio.dialog.events.generate_user_event", "frog.studio.structures.case_structure",
            "frog.studio.boolean.true_constant")):
        return "labview-user-event-emit-v1"
    return None


def walk_nodes(diagram):
    for node in diagram["nodes"]:
        yield node
        for region in node.get("regions", ()):
            yield from walk_nodes(region["diagram"])
