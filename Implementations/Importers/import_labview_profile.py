# SPDX-License-Identifier: Apache-2.0
"""Translate the decoded, standalone scalar LabVIEW diagram profile."""

from pathlib import Path
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
ATTR_CLASS = LVheap.attributeNameToId("class")
ATTR_UID = LVheap.attributeNameToId("uid")
ATTR_COUNT = LVheap.attributeNameToId("elements")

UNPROJECTED_NODES = {
    "commentNode": ("disable_structure", "Disable structure selection and its region boundaries have no verified translation."),
    "select": ("case_structure", "Case selectors, regions and tunnels have no verified translation."),
    "iUse": ("subvi_call", "The LabVIEW connector pattern and call execution have no verified SubFROG translation."),
    "propNode": ("property_node", "The saved LabVIEW reference class, property and error flow require a verified translation."),
    "invokeNode": ("invoke_node", "The saved LabVIEW reference class, method and error flow require a verified translation."),
    "mergeErrors": ("merge_errors", "The source ordering and precedence of the error merge have no verified translation."),
}


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


def unprojected_node(item):
    category = kind(item)
    if category == "select" and not children(item, "diagramList"):
        return None
    return UNPROJECTED_NODES.get(category)


def diagnostic_label(node, name):
    values = children(node, name)
    if len(values) == 1:
        value = values[0].content
        if isinstance(value, bytes) and len(value) <= 128 and value.isascii():
            value = value.decode("ascii")
            if value.isprintable():
                return value
    return None


def projection_requirement_diagnostics(vi):
    """Attribute unsupported semantics after a projection refusal, without deriving a graph."""
    groups = {}

    def record(family, item, resource, code, message):
        uid = item.attribs.get(ATTR_UID)
        if type(uid) is not int or uid <= 0:
            return
        source = f"{resource}/uid:{uid}"
        diagnostic = groups.setdefault(family, {"code": "labview_import." + code, "resource": source,
                                               "source": source, "message": message, "occurrences": []})
        location = {"uid": uid, "source": source}
        scope = item.parent
        while scope is not None and kind(scope) != "diag":
            scope = scope.parent
        if scope is not None and ATTR_UID in scope.attribs:
            location["diagram_uid"] = scope.attribs[ATTR_UID]
        for field_name, key in (("nodeName", "reference_class"), ("methName", "method")):
            if value := diagnostic_label(item, field_name):
                location[key] = value
        if family == "propNode":
            names = []
            for bindings in children(item, "dcoList"):
                for binding in children(bindings, "SL__arrayElement"):
                    definition = property_definitions.get(binding.attribs.get(ATTR_UID))
                    if definition is None:
                        continue
                    for prop_list in children(definition, "propList"):
                        for prop in children(prop_list, "SL__arrayElement"):
                            if kind(prop) == "propItemInfo" and (name := diagnostic_label(prop, "PropItemName")):
                                names.append(name)
            if names:
                location["property"] = ", ".join(names)
        diagnostic["occurrences"].append(location)

    section = one_section(vi, b"BDHb")
    objects = [item for item in section.objects if item.getScopeInfo() != LVheap.NODE_SCOPE.TagClose]
    property_definitions = {item.attribs[ATTR_UID]: item for item in objects
                            if kind(item) == "propItem" and ATTR_UID in item.attribs}
    for item in objects:
        if requirement := unprojected_node(item):
            record(kind(item), item, "BDHb", *requirement)

    section = one_section(vi, b"FPHb")
    for item in section.objects:
        if item.getScopeInfo() == LVheap.NODE_SCOPE.TagClose or kind(item) != "fPDCO":
            continue
        displays = children(item, "ddo")
        if len(displays) == 1 and kind(displays[0]) == "stdClust":
            record("panel_cluster", item, "FPHb", "panel_cluster",
                   "Front Panel clusters and their saved element types need a verified widget projection.")

    for diagnostic in groups.values():
        diagnostic["count"] = len(diagnostic["occurrences"])
    return list(groups.values())


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


def constant_value(descriptor, dtype, location):
    data = field(descriptor, "ConstValue", location).content
    return decode_constant_value(data, dtype, location)


def decode_constant_value(data, dtype, location):
    require(isinstance(data, bytes), "constant_payload", "A constant has no readable data bytes.", location)
    # LV20 can append a NUL to the all-zero heap literal, including an empty
    # string's 32-bit length. Leave non-zero or differently sized data strict.
    width = 4 if dtype == "string" else 1 if dtype == "bool" else int(dtype[1:]) // 8 if dtype in TYPES.values() else 0
    if width and data == bytes(width + 1):
        data = data[:width]
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
    # The common widget codec serializes intrinsic zero as the JSON number 0
    # even for DBL. The explicit widget type still retains its F64 meaning.
    return 0


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
    document, mapping = _project_dataflow(vi, source, source_sha256)
    if "presentation" not in mapping:
        from import_labview_bundle import member_presentation

        presentation, evidence, _ = member_presentation(vi, document["frontPanel"]["widgets"], source.stem)
        document.update(presentation)
        mapping["presentation"] = evidence
    return document, mapping


def _project_dataflow(vi, source, source_sha256):
    diagram = field(heap_root(vi, b"BDHb"), "root", "BDHb")
    require(kind(diagram) == "diag", "diagram_root", "A source diagram is required.", "BDHb")
    plane = items(field(diagram, "zPlaneList", "BDHb"), "BDHb")
    definitions = {identity(item, "BDHb/zPlaneList"): item for item in plane if kind(item) is not None}
    root_nodes = items(field(diagram, "nodeList", "BDHb"), "BDHb")
    physical = [definitions.get(identity(item, "BDHb/nodeList"), item) if kind(item) is None else item
                for item in root_nodes]
    if len(physical) == 1 and kind(physical[0]) == "sRN" and plane and not items(field(diagram, "signalList", "BDHb"), "BDHb"):
        terms = items(field(physical[0], "termList", "BDHb"), "BDHb")
        if terms and all(kind(term) == "fPTerm" for term in terms):
            return _project_unwired_panel(vi, source, source_sha256, plane, terms)
    if any(kind(item) in ("flatSequence", "forLoop", "aBuild") for item in plane) or any(
            kind(item) == "prim" and (number(item, "primResID", "BDHb"), number(item, "primIndex", "BDHb"))
            in {(1053, 106), (1201, 73)} for item in physical):
        from import_labview_heap_graph import decode_graph
        from import_labview_structured import project as project_structured

        return project_structured(decode_graph(vi), source, source_sha256)
    if {kind(item) for item in physical} <= {"sRN", "prim", "nMux", "mergeErrors"} and any(
            kind(item) == "prim" and (number(item, "primResID", "BDHb"), number(item, "primIndex", "BDHb"))
            in {(9108, 479), (9113, 485)} for item in physical):
        from import_labview_editable import project_editable
        documents, mapping = project_editable(source, vi)
        require(mapping.get("execution_profile"), "queue_service",
                "The Queue wrapper has unverified operations, bindings or defaults.", "BDHb")
        mapping.update(profile=mapping["execution_profile"], source_sha256=source_sha256)
        return documents[Path("model.frog")], mapping
    for item in physical:
        requirement = unprojected_node(item)
        if requirement is not None:
            code, message = requirement
            raise ProfileRefusal(code, message, f"BDHb/uid:{identity(item, 'BDHb/nodeList')}")
    require(len(physical) <= 2 and sum(kind(item) == "sRN" for item in physical) == 1
            and all(kind(item) in ("sRN", "prim") for item in physical),
            "diagram_topology", "Expected a standalone root with at most one scalar primitive.", "BDHb/nodeList")
    # Use the same default-value reader, NI parameter identities and typed
    # operations as the corresponding graph inside a sequence or loop.
    from import_labview_heap_graph import decode_graph
    from import_labview_structured import project as project_structured

    graph = decode_graph(vi)
    signals = graph.diagrams[graph.root].signals
    endpoints = [uid for signal in signals for uid in (signal.source, *signal.targets)]
    require(len(endpoints) == len(set(endpoints)) and set(endpoints) == graph.terms.keys(),
            "signal_coverage", "Each standalone terminal must belong to exactly one saved signal.", "BDHb/signalList")
    document, mapping = project_structured(graph, source, source_sha256)
    mapping.update(profile="labview-standalone-scalar-v1", scope="Editable standalone scalar dataflow.")
    return document, mapping


def _project_unwired_panel(vi, source, source_sha256, plane, terms):
    """An unwired panel returns immediately, preserving each control and indicator."""
    from import_labview_bundle import member_presentation
    from import_labview_editable import project_document
    from import_labview_panel import project_panel
    from import_labview_source_diagram import decode_source_diagram

    graph = decode_source_diagram(vi)
    widgets, issues = project_panel(vi)
    # A LabVIEW grouper belongs to front-panel editing. Each grouped DCO
    # must still be accounted for by the terminal checks below.
    groups = [issue for issue in issues if issue.get("kind") == "grouper"]
    issues = [issue for issue in issues if issue.get("kind") != "grouper"]
    terminal_ids = {identity(term, "BDHb") for term in terms}
    widget_ids = {identity(field(term, "dco", "BDHb"), "BDHb") for term in terms}
    require(not issues and not graph.issues and len(graph.diagrams) == 1 and len(graph.nodes) == 1
            and not graph.signals and len(terms) == len(terminal_ids) == len(widget_ids) == len(plane)
            and terminal_ids == graph.terminals.keys() == {identity(obj, "BDHb") for obj in plane}
            and widget_ids == widgets.keys() == graph.bindings.keys(),
            "panel_coverage", "The unwired terminals and saved panel controls do not match exactly.", "BDHb")
    document, mapping, regions = project_document(source, vi, widgets, issues, {}, graph, Path("model.frog"))
    presentation, evidence, problems = member_presentation(vi, list(widgets.values()), source.stem)
    require(not regions and not mapping["notes"] and not problems
            and len(document["diagram"]["nodes"]) == len(terms)
            and all(node["kind"] == "widget_value" for node in document["diagram"]["nodes"]),
            "panel_coverage", "A panel object or connector has unverified semantics.", "FPHb")
    document.update(presentation)
    mapping.update(profile="labview-unwired-panel-v1", presentation=evidence, source_sha256=source_sha256)
    if groups:
        mapping["panel_groups"] = groups
    return document, mapping
