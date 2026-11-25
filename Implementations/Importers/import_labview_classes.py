# SPDX-License-Identifier: Apache-2.0
"""Translate a local LV20 value class with an embedded scalar record and static members."""

import base64
import hashlib
from pathlib import PureWindowsPath

from import_labview_heap_graph import HeapTypes, members
from import_labview_interfaces import read_class_xml, read_connector, read_dependencies, resolve_local_path
from import_labview_panel import project_panel
from import_labview_profile import ProfileRefusal, field, heap_root, kind, require


def verify_class_typedefs(project, loaded, root, declarations, read_bytes, load_control):
    """Check cached by-value class aggregates against the separately saved control."""
    cache = {}
    evidence = {}
    for caller, links in project.items():
        vi = loaded[caller]
        if not getattr(vi, "local_class_ids", {}):
            continue
        try:
            connector = read_connector(vi)
        except ProfileRefusal:
            # The connector owner reports its own refusal during normal projection.
            continue
        for port in connector.bindings:
            wrapper = port.datatype
            if wrapper.family != "TypeDef" or len(wrapper.definition) != 1 or len(wrapper.children) != 1:
                continue
            cached = wrapper.children[0]
            if cached.family != "Cluster":
                continue
            pending = [cached]
            while pending:
                item = pending.pop()
                if item.class_id:
                    break
                pending.extend(item.children)
            else:
                continue
            name = wrapper.definition[0].decode(vi.textEncoding).casefold()
            targets = {link.target for link in links if link.status == "local" and link.target
                       and link.reference.kind in (b"VICC", b"TDCC") and link.target.name.casefold() == name}
            require(len(targets) == 1, "typedef_dependency",
                    "A class-valued typedef requires one saved local control dependency.", name)
            path = targets.pop()
            if path not in cache:
                payload = read_bytes(path)
                ctl = load_control(payload, path)
                ctl.local_class_ids = {}
                for dep in read_dependencies(ctl, b"LIvi"):
                    if dep.kind != b"VIPI":
                        continue
                    owner = resolve_local_path(dep, path, root, ctl.textEncoding)
                    if owner in declarations:
                        key = owner.name.casefold()
                        require(ctl.local_class_ids.setdefault(key, declarations[owner]) == declarations[owner],
                                "typedef_class", "Several class owners share the same saved control name.", path.name)
                pane = field(field(heap_root(ctl, b"FPHb"), "root", "FPHb"), "paneHierarchy", "FPHb")
                dcos = [d for d in members(pane, "zPlaneList", "FPHb") if kind(d) == "fPDCO"]
                require(len(dcos) == 1, "typedef_root", "Expected one control definition.", path.name)
                types = HeapTypes(ctl)
                actual = types.describe(types.heap(dcos[0], "typeDesc", "FPHb"), "FPHb")
                widgets, errors = project_panel(ctl)
                require(not errors and len(widgets) == 1 and actual.native_type(ctl.textEncoding),
                        "typedef_root", "The stored control or its value cannot be translated.", path.name)
                cache[path] = actual
                evidence[path.relative_to(root).as_posix()] = {
                    "sha256": hashlib.sha256(payload).hexdigest(), "type": actual.native_type(ctl.textEncoding),
                    "classes": sorted(ctl.local_class_ids.values())}
            actual = cache[path]
            require(cached.representation() == actual.representation() and
                    cached.native_type(vi.textEncoding) == actual.native_type(vi.textEncoding),
                    "typedef_mismatch", "The cached connector differs from the saved control definition.", path.name)
    return evidence


def private_field_nodes(node, graph, types, owner, definition, wired):
    """Use private field operations only inside a member of the verified owning class."""
    if node.kind != "nMux" or not {"dcoAgg", "dcoList"} <= node.fields.keys():
        return None
    aggregate = node.fields["dcoAgg"].uid
    receiver = graph.bindings.get(aggregate)
    schema = types.schema(receiver)
    if schema is None or schema.class_id != definition["id"]:
        return None
    terms = [graph.terminals[uid] for uid in node.terminals]
    sources = [t.uid for t in terms if t.binding == aggregate and t.direction == "source"]
    sinks = [t.uid for t in terms if t.binding == aggregate and t.direction == "sink"]
    if len(sources) > 1 or len(sinks) != 1 or sinks[0] not in wired:
        return None
    write = bool(sources)
    rows = [field.uid for field in node.fields["dcoList"].children]
    fields = {t.binding: t for t in terms if t.binding != aggregate}
    if not rows or set(rows) != fields.keys() or len(terms) != len(rows) + len(sinks) + len(sources):
        return None
    objects, edges, ports, readers, seen = [], [], {}, [], set()
    dtype = schema.native_type(types.encoding)
    for row, binding_id in enumerate(rows):
        term = fields[binding_id]
        binding = graph.bindings[binding_id]
        index = binding.fields["i"].value if "i" in binding.fields else 0
        if (type(index) is not int or index in seen or not 0 <= index < len(definition["fields"]) or
                term.direction != ("sink" if write else "source") or (write and term.uid not in wired)):
            return None
        seen.add(index)
        field = definition["fields"][index]
        if binding.type_label != field["label"] or types(binding) != field["type"]:
            return None
        uid = f"lv-node-{node.uid}" if not row else f"lv-node-{node.uid}-field-{index}"
        params = [("object", "input", dtype), ("value", "input" if write else "output", field["type"])]
        if write:
            params.append(("updated", "output", dtype))
        call_ports = [{"id": name, "source_port_id": name, "direction": direction, "value_type": value,
                       "label": name, "connection_requirement": "required" if direction == "input" else "optional",
                       "normalized_x": 0 if direction == "input" else 1,
                       "normalized_y": (0.25 if name == "object" else 0.75) if write and direction == "input" else 0.5}
                      for name, direction, value in params]
        objects.append({"id": uid, "kind": "frog_call", "type": "frog.studio.frog_call", "label": field["label"],
                        "call": {"document_path": "", "document_title": field["label"], "icon_svg": "",
                                 "ports": call_ports, "class_operation": {"kind": "write_field" if write else "read_field",
                                 "class": owner, "member_id": field["id"]}}})
        ports[term.uid] = (uid, "value")
        if row and write:
            edges.append({"id": uid + "-object", "from": {"node": objects[-2]["id"], "port": "updated"},
                          "to": {"node": uid, "port": "object"}})
        elif row:
            readers.append((uid, "object"))
    ports[sinks[0]] = (objects[0]["id"], "object")
    if write:
        ports[sources[0]] = (objects[-1]["id"], "updated")
    return objects, edges, ports, sinks[0], readers


def private_control_bytes(xml, loc):
    header = bytes.fromhex("20008000000000020005000500000c00400001ffffffff000000010001")
    properties = xml.findall("Property[@Name='NI.LVClass.FlattenedPrivateDataCTL'][@Type='Bin']")
    require(len(properties) == 1 and properties[0].text, "class_private_data",
            "Expected one embedded private control.", loc)
    text = properties[0].text
    require(len(text) <= 512 * 1024 and all(33 <= ord(c) <= 96 for c in text) and len(text) % 4 != 1,
            "class_private_data", "Invalid LabVIEW binary property encoding.", loc)
    alphabet = b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    encoded = bytes(alphabet[ord(c) - 33] for c in text)
    payload = base64.b64decode(encoded + b"=" * (-len(encoded) % 4), validate=True)
    require(base64.b64encode(payload).rstrip(b"=") == encoded and payload.startswith(header) and
            len(payload) >= len(header) + 8 and payload[-4:] == bytes(4), "class_private_data",
            "Unrecognized private-control envelope.", loc)
    length = int.from_bytes(payload[len(header):len(header) + 4], "big")
    control = payload[len(header) + 4:-4]
    require(length == len(control), "class_private_data", "Private control length disagrees with its envelope.", loc)
    return control


def translate_class(path, root, payload, load_control, load_vi):
    loc = path.name
    xml = read_class_xml(path, payload)
    genealogy = xml.findall("Property[@Name='NI.LVClass.Geneology']")
    private = xml.findall("Item[@Type='Class Private Data']")
    require(xml.get("LVVersion") == "20008000" and len(genealogy) == 1 and not genealogy[0].text and
            len(private) == 1 and private[0].get("Name") == private[0].get("URL") and
            private[0].findtext("Property[@Name='NI.LibItem.Scope']") == "2",
            "class_profile", "Expected a standalone value class with one private control.", loc)
    widgets, issues = project_panel(load_control(private_control_bytes(xml, loc), path.with_suffix(".ctl")))
    require(len(widgets) == 1 and not issues, "class_private_data", "Private control could not be projected.", loc)
    elements = next(iter(widgets.values()))["props"].get("cluster.elements", ())
    require(0 < len(elements) <= 64 and all("state" in e["widget"] and
            e["widget"]["valueType"] in {"bool","u8","u16","u32","u64","i8","i16","i32","i64","f32","f64"}
            for e in elements), "class_private_data", "Expected scalar private fields with verified defaults.", loc)
    class_id = "labview." + hashlib.sha256(path.relative_to(root).as_posix().casefold().encode()).hexdigest()[:32]
    fields = [{"id": f"field-{e['logical_order']}", "label": e["technical_name"], "type": e["widget"]["valueType"],
               "default": e["widget"]["state"]["value"]} for e in elements]
    methods, members = [], {}
    items = xml.findall(".//Item")
    require(len(items) <= 65 and all(i.get("Type") in {"Class Private Data", "VI"} for i in items),
            "class_members", "Unsupported class member or class member count.", loc)
    for item in items:
        if item.get("Type") != "VI":
            continue
        name = item.get("Name", "")
        part = PureWindowsPath(name)
        require(part.name == name and not part.drive and not part.is_reserved() and part.suffix.lower() == ".vi" and
                not any(ord(c) < 32 or c in '<>:"|?*' for c in name) and item.get("URL") == "../" + name and
                item.findtext("Property[@Name='NI.ClassItem.IsStaticMethod']") == "true" and
                item.findtext("Property[@Name='NI.ClassItem.MethodScope']") == "1" and
                item.find("Property[@Name='NI.LibItem.Scope']") is None, "class_method",
                "Only local public static VI members are supported by this class profile.", loc)
        member = (path.parent / name).resolve()
        require(member.is_relative_to(root) and member.parent == path.parent and member not in members,
                "class_method", "Invalid or duplicate class member path.", loc)
        vi = load_vi(member)
        owners = [resolve_local_path(d, member, root, vi.textEncoding) for d in read_dependencies(vi, b"LIvi")
                  if d.kind == b"VILB"]
        require(owners == [path], "class_method", "The member VI does not declare the same library owner.", loc)
        method_id = "method-" + hashlib.sha256(name.casefold().encode()).hexdigest()[:20]
        ports = []
        for binding in read_connector(vi).bindings:
            schema = binding.datatype
            receiver = (schema.family == "Refnum" and not schema.children and len(schema.detail[1]) == 1 and
                        schema.detail[0] == 30 and schema.detail[2] == (("field0", 0), ("field2", 0), ("multiItem", 1)) and
                        schema.detail[1][0].decode(vi.textEncoding).casefold() == loc.casefold())
            dtype = schema.native_type(vi.textEncoding)
            require(receiver or dtype is not None, "class_method", "Unsupported class connector type.", loc)
            slot = f"slot-{binding.slot}"
            ports.append({"slot": slot, "port_id": slot, "direction": "output" if binding.output else "input",
                          "kind": "object" if receiver else "data", "type": None if receiver else dtype})
        require([p["direction"] for p in ports if p["kind"] == "object"].count("input") == 1, "class_method",
                "The method must have exactly one receiver.", loc)
        impl = class_id + "." + method_id
        methods.append({"id": method_id, "name": member.stem, "visibility": "public", "dispatch": "static",
                        "overrides": None, "implementation": {"id": impl, "path": member.with_suffix(".frog").name,
                        "revision": 1}, "ports": ports})
        members[member] = {"class_id": class_id, "method_id": method_id, "implementation_id": impl, "revision": 1}
    return {"format": "frog.class", "version": 1, "name": path.stem, "id": class_id, "parent": None,
            "revision": 1, "fields": fields, "methods": methods}, members


def clocked_value_member(document, mapping):
    """Only closed, exactly wired U32 class members with a saved error Case."""
    member = document.get("class_member")
    if not member or mapping["reader_issues"] or mapping["detached_regions"]:
        return None
    if any(note.get("code") != "unwired_boundary_role" for note in mapping["notes"]):
        return None
    if any(not signal.get("targets") for signal in mapping["signals"].values()):
        return None
    diagrams = [document["diagram"]]
    for diagram in diagrams:
        diagrams.extend(region["diagram"] for n in diagram["nodes"] for region in n.get("regions", ()))
    edges = {edge["id"]: edge for diagram in diagrams for edge in diagram["edges"]}
    instances = {n["id"]: n for diagram in diagrams for n in diagram["nodes"]}
    endpoints = mapping["endpoints"]
    for signal in mapping["signals"].values():
        if any(uid not in endpoints for uid in (signal["source"], *signal["targets"])):
            return None
        source = endpoints[signal["source"]]
        targets = {(endpoints[uid]["node"], endpoints[uid]["port"]) for uid in signal["targets"]}
        actual = set()
        for edge_id in signal["edges"]:
            edge = edges.get(edge_id)
            if not edge or (edge["from"]["node"], edge["from"]["port"]) != (source["node"], source["port"]):
                return None
            dest = edge["to"]["node"], edge["to"]["port"]
            actual.add(dest)
            if dest not in targets:
                # A multi-field NI read becomes one real class field read per
                # field, all receiving that same untouched object value.
                op = instances.get(dest[0], {}).get("call", {}).get("class_operation", {})
                if dest[1] != "object" or op.get("kind") != "read_field" or op.get("class", {}).get("id") != member["class_id"]:
                    return None
        if not targets.issubset(actual):
            return None
    roots = [obj for obj in document["diagram"]["nodes"] if obj["kind"] != "widget_value"]
    if len(roots) != 1 or roots[0]["type"] != "frog.studio.structures.case_structure":
        return None
    cases = roots[0]["regions"]
    if len(cases) != 2:
        return None
    by_status = {}
    for region in cases:
        matches = region.get("matches", [])
        if len(matches) != 1 or matches[0].get("kind") != "error_status":
            return None
        by_status[matches[0].get("value")] = region["diagram"]
    if set(by_status) != {"true", "false"} or by_status["true"]["nodes"]:
        return None
    for tunnel in roots[0]["structure_tunnels"]:
        if not tunnel["output_from_structure"]:
            continue
        port = "tunnel:" + tunnel["id"] + ":input"
        for region in cases:
            driven = any(e["to"] == {"node": roots[0]["id"], "port": port} for e in region["diagram"]["edges"])
            if not driven and not tunnel.get("use_default_if_unwired"):
                return None
    active = by_status["false"]["nodes"]
    if sum(n["type"] == "frog.studio.structures.tick_count" for n in active) != 1:
        return None
    field_write = False
    for node in active:
        if node["kind"] == "frog_call":
            call = node["call"].get("class_operation", {})
            if (call.get("kind") not in ("read_field", "write_field") or
                    call.get("class", {}).get("id") != member["class_id"]):
                return None
            field_write |= call["kind"] == "write_field"
        elif node["type"] not in ("frog.studio.structures.tick_count", "frog.core.sub",
                                  "frog.core.add", "frog.studio.comparison.greater", "frog.studio.numeric.numeric_constant"):
            return None
        elif node["type"] == "frog.studio.numeric.numeric_constant" and node.get("value_type") != "u32":
            return None
    if field_write:
        return "labview-clocked-value-member-u32-v1"
    return None
