# SPDX-License-Identifier: Apache-2.0
"""Translate the decoded, standalone scalar LabVIEW diagram profile."""

from dataclasses import dataclass
import json
import math
import struct

from pylabview import LVheap


class ProfileRefusal(ValueError):
    def __init__(self, code, message, resource):
        super().__init__(message)
        self.code = code
        self.resource = resource


TYPES = {
    "NumInt8": "i8", "NumInt16": "i16", "NumInt32": "i32", "NumInt64": "i64",
    "NumUInt8": "u8", "NumUInt16": "u16", "NumUInt32": "u32", "NumUInt64": "u64",
    "NumFloat32": "f32", "NumFloat64": "f64", "Boolean": "bool", "String": "string",
}
OPERATIONS = {
    (1052, 105): ("frog.core.mul", "numeric.multiply"),
}
ATTR_CLASS = LVheap.attributeNameToId("class")
ATTR_UID = LVheap.attributeNameToId("uid")
ATTR_COUNT = LVheap.attributeNameToId("elements")


def require(condition, code, message, resource):
    if not condition:
        raise ProfileRefusal(code, message, resource)


def tag(node):
    return LVheap.tagEnToName(node.tagEn, node.parent)


def kind(node):
    value = node.attribs.get(ATTR_CLASS)
    return LVheap.classEnToName(value) if value is not None else None


def children(node, name):
    return [child for child in node.childs
            if child.getScopeInfo() != LVheap.NODE_SCOPE.TagClose and tag(child) == name]


def field(node, name, resource, optional=False):
    matches = children(node, name)
    require(len(matches) == 1 or optional and not matches, "heap_field",
            f"Expected one readable '{name}' field.", resource)
    return matches[0] if matches else None


def items(node, resource):
    values = children(node, "SL__arrayElement")
    require(node.attribs.get(ATTR_COUNT) == len(values), "heap_list",
            "The declared heap list size does not match its decoded entries.", resource)
    return values


def number(node, name, resource):
    item = field(node, name, resource)
    value = getattr(item, "value", None)
    require(item.format == "inline" and type(value) is int, "heap_integer",
            f"'{name}' is not a decoded integer.", resource)
    return value


def identity(node, resource):
    uid = node.attribs.get(ATTR_UID)
    require(type(uid) is int and uid > 0, "heap_uid", "An object has no valid UID.", resource)
    return uid


def text(data, resource):
    require(isinstance(data, bytes) and len(data) <= 512 and data.isascii(), "scalar_text",
            "This profile requires ASCII source labels and strings of at most 512 bytes.", resource)
    return data.decode("ascii")


def bounds(x, y, width, height):
    return {"x": x, "y": y, "width": width, "height": height}


def one_section(vi, name):
    block = vi.blocks.get(name)
    require(block is not None and len(block.sections) == 1, "resource_section",
            "Exactly one decoded section is required.", name.decode("ascii"))
    section = next(iter(block.sections.values()))
    require(not section.parse_failed, "resource_section", "The required section did not decode.", name.decode("ascii"))
    return section


def heap_root(vi, resource):
    section = one_section(vi, resource)
    roots = [node for node in section.objects if node.parent is None
             and node.getScopeInfo() != LVheap.NODE_SCOPE.TagClose]
    location = resource.decode("ascii")
    require(len(roots) == 1, "heap_root", "Expected exactly one decoded heap root.", location)
    return roots[0]


class Types:
    def __init__(self, vi):
        self.slice = one_section(vi, b"DTHP")
        one_section(vi, b"VCTP")
        self.consolidated = vi.blocks[b"VCTP"]

    def of(self, descriptor, resource):
        heap_id = number(descriptor, "typeDesc", resource)
        require(1 <= heap_id <= self.slice.tdCount, "type_reference", "Heap TypeID is out of range.", resource)
        dtype = self.consolidated.getTopType(self.slice.indexShift + heap_id - 1)
        family = dtype.fullType().name if dtype is not None else "unknown"
        require(family in TYPES, "unsupported_type", f"Scalar type '{family}' is outside this profile.", resource)
        return TYPES[family]


@dataclass(frozen=True)
class PanelObject:
    label: str
    dtype: str
    role: str
    source_bounds: tuple[int, int, int, int]


@dataclass(frozen=True)
class Port:
    node: str
    name: str
    dtype: str
    source: bool


def panel(vi, types):
    root = field(heap_root(vi, b"FPHb"), "root", "FPHb")
    pane = field(root, "paneHierarchy", "FPHb")
    require(kind(pane) == "pane", "panel_pane", "This profile requires a single panel pane.", "FPHb")
    members = items(field(pane, "zPlaneList", "FPHb"), "FPHb")
    result = {}
    for widget in members:
        uid = identity(widget, "FPHb")
        location = f"FPHb/uid:{uid}"
        require(kind(widget) == "fPDCO" and uid not in result, "panel_object",
                "The panel contains an unsupported object or duplicate UID.", location)
        body = field(widget, "ddo", location)
        dtype = types.of(widget, location)
        family = "stdString" if dtype == "string" else "stdBool" if dtype == "bool" else "stdNum"
        require(kind(body) == family, "panel_widget", f"Unsupported display class '{kind(body)}'.", location)
        parts = items(field(body, "partsList", location), location)
        labels = [part for part in parts if kind(part) == "label" and number(part, "partID", location) == 16]
        require(len(labels) == 1, "panel_label", "The control has no unique readable label.", location)
        record = field(labels[0], "textRec", location)
        name = text(field(record, "text", location).content, location)
        rectangle = field(body, "bounds", location)
        require(rectangle.format == "inline" and isinstance(rectangle, LVheap.HeapNodeRect),
                "panel_bounds", "The control has no decoded bounds.", location)
        role = "indicator" if number(widget, "objFlags", location) & 1 else "control"
        result[uid] = PanelObject(name, dtype, role, (rectangle.left, rectangle.top, rectangle.right, rectangle.bottom))
    require(1 <= len(result) <= 16, "panel_count", "This profile requires one to sixteen scalar panel objects.", "FPHb")
    aliases = [identity(item, "FPHb/ddoList") for item in items(field(root, "ddoList", "FPHb"), "FPHb/ddoList")]
    require(set(aliases) == result.keys() and len(aliases) == len(result), "panel_coverage",
            "Panel references and objects do not match.", "FPHb/ddoList")
    return result


def constant_value(descriptor, dtype, location):
    data = field(descriptor, "ConstValue", location).content
    require(isinstance(data, bytes), "constant_payload", "A constant has no readable data bytes.", location)
    if dtype == "string":
        require(len(data) >= 4 and int.from_bytes(data[:4], "big") == len(data) - 4,
                "constant_payload", "The stored string length does not match its data.", location)
        return text(data[4:], location)
    if dtype == "bool":
        require(data in (b"\x00", b"\x01"), "constant_payload", "Invalid Boolean constant.", location)
        return data == b"\x01"
    require(len(data) * 8 == int(dtype[1:]), "constant_payload", "The scalar has an invalid byte width.", location)
    if dtype[0] != "f":
        return int.from_bytes(data, "big", signed=dtype[0] == "i")
    value = struct.unpack(">f" if dtype == "f32" else ">d", data)[0]
    require(math.isfinite(value), "constant_payload", "Non-finite floating constants are outside the profile.", location)
    return value


def initial_value(dtype):
    if dtype == "string":
        return ""
    if dtype == "bool":
        return False
    return 0.0 if dtype[0] == "f" else 0


def convert_constant(uid, dtype, value, x, y):
    family = "numeric" if dtype not in ("string", "bool") else dtype
    if family == "bool":
        token = str(value).lower()
        primitive = f"frog.studio.boolean.{token}_constant"
    elif family == "string":
        token = value
        primitive = "frog.studio.string.constant"
    else:
        token = repr(value)
        primitive = "frog.studio.numeric.numeric_constant"
    return {"id": f"lv-constant-{uid}", "kind": "primitive", "type": primitive, "value_type": dtype,
            "text": token, "label_visible": False, "layout": bounds(x, y, 76, 29.75)}


def project(vi, source, source_sha256):
    diagram = field(heap_root(vi, b"BDHb"), "root", "BDHb")
    require(kind(diagram) == "diag", "diagram_root", "A source diagram is required.", "BDHb")
    plane = items(field(diagram, "zPlaneList", "BDHb"), "BDHb")
    if any(kind(item) in ("flatSequence", "forLoop", "aBuild") for item in plane):
        from import_labview_heap_graph import decode_graph
        from import_labview_structured import project as project_structured

        return project_structured(decode_graph(vi), source, source_sha256)
    if b"DFDS" in vi.blocks:
        defaults = one_section(vi, b"DFDS")
        require(not defaults.content, "stored_defaults",
                "Stored default values are outside the standalone scalar profile.", "DFDS")
    types = Types(vi)
    widgets = panel(vi, types)
    definitions = {identity(item, "BDHb/zPlaneList"): item for item in plane if kind(item) is not None}
    root_nodes = items(field(diagram, "nodeList", "BDHb"), "BDHb")
    physical = [definitions.get(identity(item, "BDHb/nodeList"), item) if kind(item) is None else item
                for item in root_nodes]
    require(len(physical) <= 2 and sum(kind(item) == "sRN" for item in physical) == 1,
            "diagram_topology", "Only a standalone root diagram with at most one scalar primitive is supported.", "BDHb/nodeList")

    port_by_uid, node_by_uid = {}, {}
    mapped_widgets = set()
    for item in physical:
        uid = identity(item, "BDHb/nodeList")
        location = f"BDHb/uid:{uid}"
        category = kind(item)
        require(category in ("prim", "sRN"), "diagram_node", f"Unsupported diagram class '{category}'.", location)
        terms = items(field(item, "termList", location), location)
        if category == "prim":
            op = (number(item, "primResID", location), number(item, "primIndex", location))
            require(op in OPERATIONS and len(terms) == 3, "primitive", f"Unqualified LabVIEW primitive {op}.", location)
            operation, palette_id = OPERATIONS[op]
            op_id = f"lv-operation-{uid}"
            node_by_uid[uid] = {"id": op_id, "kind": "primitive", "type": operation,
                                "palette_catalog_id": palette_id, "label_visible": False,
                                "layout": bounds(380, 110, 48, 48)}
        for term in terms:
            term_uid = identity(term, location)
            term_location = f"BDHb/uid:{term_uid}"
            require(term_uid not in port_by_uid, "diagram_uid", "Duplicate terminal UID.", term_location)
            dco = field(term, "dco", term_location)
            if category == "prim":
                dtype = types.of(dco, term_location)
                require(dtype == "f64" and kind(term) == "term", "primitive_type",
                        "The qualified Multiply requires three DBL terminals.", term_location)
                incoming = bool(number(term, "objFlags", term_location) & 0x8000)
                if incoming:
                    parameter = number(dco, "parmIndex", term_location)
                    require(kind(dco) == "parm" and parameter in (1, 2), "primitive_port",
                            "Unexpected Multiply input parameter.", term_location)
                    port = "a" if parameter == 1 else "b"
                else:
                    require(kind(dco) == "overridableParm" and number(dco, "override", term_location) == 3,
                            "primitive_port", "Unsupported Multiply output configuration.", term_location)
                    port = "result"
                port_by_uid[term_uid] = Port(op_id, port, dtype, not incoming)
            elif kind(term) == "fPTerm" and kind(dco) is None:
                widget_uid = identity(dco, term_location)
                require(widget_uid in widgets and widget_uid not in mapped_widgets, "terminal_reference",
                        "The terminal does not identify one unique panel control.", term_location)
                mapped_widgets.add(widget_uid)
                widget = widgets[widget_uid]
                incoming = bool(number(term, "objFlags", term_location) & 0x8000)
                require(incoming == (widget.role == "indicator"), "terminal_direction",
                        "The panel role and diagram terminal direction disagree.", term_location)
                node_id = f"lv-terminal-{term_uid}"
                node_by_uid[term_uid] = {"id": node_id, "kind": "widget_value", "type": widget.dtype,
                                         "widget": f"lv-widget-{widget_uid}", "label_visible": True,
                                         "layout": bounds(200 if widget.role == "control" else 500, 110, 66.3, 29.75)}
                port_by_uid[term_uid] = Port(node_id, "value", widget.dtype, widget.role == "control")
            else:
                require(kind(term) == "term" and kind(dco) == "bDConstDCO", "diagram_terminal",
                        "Unsupported root terminal or data object.", term_location)
                dtype = types.of(dco, term_location)
                value = constant_value(dco, dtype, term_location)
                node_by_uid[term_uid] = convert_constant(term_uid, dtype, value, 200, 110)
                port_by_uid[term_uid] = Port(f"lv-constant-{term_uid}", "value", dtype, True)

    # Validate both lists. An ignored z-plane entry could otherwise conceal a
    # diagram object absent from this projection.
    visible_uids = [identity(item, "BDHb/zPlaneList") for item in plane]
    require(set(visible_uids) == set(node_by_uid) and len(visible_uids) == len(node_by_uid)
            and mapped_widgets == widgets.keys(), "diagram_coverage",
            "Diagram objects, terminal references and panel declarations do not match.", "BDHb")
    require(len(set(port_by_uid.values())) == len(port_by_uid), "diagram_port",
            "Several LabVIEW terminals resolve to the same FROG binding.", "BDHb")

    edges, edge_sources, used, signal_uids = [], [], set(), set()
    signals = items(field(diagram, "signalList", "BDHb"), "BDHb/signalList")
    for signal in signals:
        uid = identity(signal, "BDHb/signalList")
        location = f"BDHb/uid:{uid}"
        require(kind(signal) == "signal" and uid not in signal_uids,
                "signal_class", "Unsupported signal record or duplicate signal UID.", location)
        require(number(signal, "state", location) in (1, 33), "signal_state",
                "The saved wire state is outside the verified profile.", location)
        signal_uids.add(uid)
        endpoints = [identity(item, location) for item in items(field(signal, "termList", location), location)]
        require(len(endpoints) >= 2 and len(set(endpoints)) == len(endpoints) and not (used & set(endpoints))
                and all(endpoint in port_by_uid for endpoint in endpoints), "signal_reference",
                "Signal endpoints are missing, reused, or not distinct.", location)
        used.update(endpoints)
        producers = [endpoint for endpoint in endpoints if port_by_uid[endpoint].source]
        require(len(producers) == 1, "signal_direction", "Each signal must have one producer.", location)
        producer = producers[0]
        origin = port_by_uid[producer]
        for sink in endpoints:
            if sink == producer:
                continue
            target = port_by_uid[sink]
            require(origin.dtype == target.dtype and origin.node != target.node,
                    "signal_semantics", "Implicit coercions and self-connections are outside the profile.", location)
            edge_id = f"lv-signal-{uid}-{sink}"
            edges.append({"id": edge_id, "from": {"node": origin.node, "port": origin.name},
                          "to": {"node": target.node, "port": target.name}})
            edge_sources.append({"id": edge_id, "signal_uid": uid, "producer_uid": producer, "consumer_uid": sink})
    require(used == port_by_uid.keys() and len({edge["id"] for edge in edges}) == len(edges), "signal_coverage",
            "Each supported terminal must belong to exactly one decoded signal.", "BDHb/signalList")

    source_nodes = [node_by_uid[uid] for uid in visible_uids if uid in port_by_uid and port_by_uid[uid].source]
    sink_nodes = [node_by_uid[uid] for uid in visible_uids if uid in port_by_uid and not port_by_uid[uid].source]
    input_order = {edge["from"]["node"]: edge["to"]["port"] for edge in edges if edge["to"]["port"] in ("a", "b")}
    source_nodes.sort(key=lambda node: input_order.get(node["id"], "value"))
    for row, node in enumerate(source_nodes):
        node["layout"]["y"] = 70 + 70 * row
    for row, node in enumerate(sink_nodes):
        node["layout"]["y"] = 121 + 70 * row

    declarations, source_widgets = [], []
    rows = {"control": 0, "indicator": 0}
    for uid, widget in widgets.items():
        position = rows[widget.role]
        rows[widget.role] += 1
        family = {"bool": "boolean", "string": "string"}.get(widget.dtype, "numeric")
        initial = initial_value(widget.dtype)
        properties = {"text.value" if family == "string" else "value": initial}
        if family == "numeric":
            properties["data_type.representation"] = widget.dtype
        declaration = {"id": f"lv-widget-{uid}", "classId": f"frog.widgets.{family}_{widget.role}",
                       "label": widget.label, "role": widget.role, "valueType": widget.dtype,
                       "bounds": bounds(32 if widget.role == "control" else 350, 55 + position * 74, 152, 32),
                       "props": properties}
        if family != "string":
            declaration["state"] = {"value": initial}
        properties["data.default"] = json.dumps(declaration, ensure_ascii=False)
        declarations.append(declaration)
        source_widgets.append({"id": declaration["id"], "uid": uid, "label": widget.label, "role": widget.role,
                               "value_type": widget.dtype, "source_bounds": widget.source_bounds,
                               "initial_origin": "implicit_labview_scalar_default"})
    document = {"format": "frog.document.draft", "draft_revision": 2, "language": "FROG", "title": source.stem,
                "metadata": {"name": source.stem, "description": f"Standalone LabVIEW scalar diagram. Source SHA-256 {source_sha256}."},
                "frontPanel": {"widgets": declarations}, "diagram": {"zoom_percent": 100,
                             "nodes": list(node_by_uid.values()), "edges": edges}}
    mapping = {"profile": "labview-standalone-scalar-v1", "source_sha256": source_sha256, "widgets": source_widgets,
               "nodes": [{"id": node["id"], "uid": uid} for uid, node in node_by_uid.items()], "edges": edge_sources,
               "scope": "Editable local dataflow; original appearance and connector pane were not translated."}
    return document, mapping
