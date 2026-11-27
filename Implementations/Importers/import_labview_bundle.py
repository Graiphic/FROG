# SPDX-License-Identifier: Apache-2.0
"""Stage a bounded set of saved VI dependencies for editable FROG projection."""

from html import escape
from itertools import groupby
import json
import os
from pathlib import Path

from import_labview_interfaces import (
    match_call, read_call_links, read_connector, read_dependencies, read_project_dependencies, resolve_local_path,
)
from import_labview_profile import ProfileRefusal, TYPES, bounds, decode_constant_value, require, vi_document_path


_PATTERNS_BY_ROW = {1: "pattern_04", 2: "pattern_06", 3: "pattern_12", 4: "pattern_16",
                    5: "pattern_35", 6: "pattern_35"}
# GS pattern 35 enumerates all cells row-major, including its eight interior cells.
_SIX_ROW_SIDES = ((1, 7, 9, 11, 17, 19), (6, 8, 10, 16, 18, 20))
_CALL_CLASSES = {"iUse", "polyIUse"}
_SIMPLE_ERROR_HANDLER_PATH = (b"<vilib>", b"Utility", b"error.llb", b"Simple Error Handler.vi")
_ERROR_CLUSTER = 'cluster<"status":bool,"code":i32,"source":string>'
_SIMPLE_ERROR_HANDLER_CONNECTOR = (
    (0, True, 2304, "bool"),
    (1, True, 2304, "i32"),
    (3, False, 0, "i32"),
    (4, False, 131072, "string"),
    (5, True, 2304, "string"),
    (6, True, 3337, _ERROR_CLUSTER),
    (7, True, 2304, "string"),
    (8, False, 2048, "u16"),
    (9, False, 2560, _ERROR_CLUSTER),
)
_ERROR_FROM_CODE_PATH = (b"<vilib>", b"Utility", b"error.llb", b"Error Cluster From Error Code.vi")
_ERROR_FROM_CODE_CONNECTOR = (
    (0, True, 3334, _ERROR_CLUSTER),
    (4, False, 512, "bool"),
    (5, False, 2560, "bool"),
    (6, False, 2560, _ERROR_CLUSTER),
    (8, False, 0, "string"),
    (9, False, 4096, "i32"),
)
_INVALID_MESSAGE_FORMAT = 'The invalid message string "%s" was received in the Message Handling Loop.'


class Bundle(dict):
    """Output members plus the diagnostics for referenced VIs that could not be read."""

    def __init__(self, root):
        super().__init__()
        self.root = root
        self.unavailable = {}
        self.classes = {}
        self.members = {}
        self.class_issues = {}
        self.typedefs = {}


def resolve_bundle(source, vi, load, class_readers=None):
    """Return the original parsed VIs and their paths within the output project."""
    root_output = vi_document_path(source)
    source = Path(source).resolve()
    root = source.parent
    loaded = {source: vi}

    def load_once(path):
        if path not in loaded:
            loaded[path] = load(path)
        return loaded[path]

    bundle = Bundle(root)

    def unavailable(path, error):
        if isinstance(error, (OSError, ValueError, EOFError)) or hasattr(error, "diagnostic"):
            bundle.unavailable[path] = str(error)
            return True
        return False

    project = read_project_dependencies(source, root, load_once, on_member_error=unavailable)
    if class_readers:
        from import_labview_classes import translate_class, verify_class_typedefs
        read_bytes, load_control = class_readers
        owners = {}
        for caller, links in project.items():
            for link in links:
                if (link.target and link.status in {"local", "embedded"} and link.target.suffix.lower() == ".lvclass" and
                        link.reference.kind in {b"VIPI", b"FPPI", b"DDPI", b"VILB", b"VICC", b"TDCC"}):
                    owners.setdefault(link.target, set()).add(caller)
        for path, callers in owners.items():
            relative = path.relative_to(root).with_suffix(".fclass")
            try:
                definition, methods = translate_class(path, root, read_bytes(path), load_control, load_once)
                require(source not in methods, "class_entry", "Import this method from its enclosing project.", path.name)
                for member in methods.keys() - project.keys():
                    dependencies = read_project_dependencies(member, root, load_once)
                    require(dependencies.keys() <= project.keys() | methods.keys(), "class_method",
                            "Additional method dependencies require their own projection.", path.name)
                require(len(project.keys() | methods.keys()) <= 512, "dependency_budget",
                        "The selected project exceeds its VI dependency budget.", path.name)
            except Exception as error:
                if isinstance(error, (OSError, ValueError, EOFError)) or hasattr(error, "diagnostic"):
                    bundle.class_issues[relative.as_posix()] = str(error)
                    continue
                raise
            bundle.classes[relative] = definition
            bundle.members.update(methods)
            for member in methods:
                project.setdefault(member, ())
            for caller in callers | methods.keys():
                ids = getattr(loaded[caller], "local_class_ids", {})
                name = path.name.casefold()
                if name in ids and ids[name] != definition["id"]:
                    ids[name] = None
                else:
                    ids[name] = definition["id"]
                loaded[caller].local_class_ids = ids
        if bundle.classes:
            declarations = {path: bundle.classes[path.relative_to(root).with_suffix(".fclass")]["id"]
                            for path in owners if path.relative_to(root).with_suffix(".fclass") in bundle.classes}
            bundle.typedefs = verify_class_typedefs(project, loaded, root, declarations, read_bytes, load_control)
    destinations = set()
    for member in project:
        relative = root_output if member == source else member.relative_to(root).with_suffix(".frog")
        identity = relative.as_posix().casefold()
        require(identity not in destinations, "dependency_output_collision",
                f"Several saved dependencies map to the same output '{relative.as_posix()}'.", "LIvi")
        bundle[member] = loaded[member], relative
        destinations.add(identity)
    return bundle


def _note(resource, reason, code, **provenance):
    return {"resource": resource, "reason": reason, "code": code, **provenance}


def _source_type(datatype):
    if not datatype.complete:
        return None
    if datatype.family in TYPES:
        return TYPES[datatype.family]
    if datatype.family == "Cluster":
        fields = [_source_type(field) for field in datatype.children]
        return "cluster:" + "|".join(fields) if fields and all(fields) else None
    if datatype.family == "Array" and datatype.detail == ((255, 0xFFFFFF),):
        element = _source_type(datatype.children[0])
        if element in TYPES.values():
            return "array:" + element
    return None


def _descriptor_evidence(datatype, encoding):
    def decode(value):
        if isinstance(value, bytes):
            return value.decode(encoding, errors="backslashreplace")
        if isinstance(value, tuple):
            return [decode(item) for item in value]
        return value

    return {"kind": datatype.family, "label": decode(datatype.label),
            "definition": decode(datatype.definition), "detail": decode(datatype.detail),
            "children": [_descriptor_evidence(child, encoding) for child in datatype.children]}


def source_marker_ports(call, encoding):
    """Preserve each occupied connector slot on an untranslated ordinary call."""
    result = []
    for terminal in call.terminals:
        descriptor = terminal.datatype
        label_descriptor = descriptor
        while not label_descriptor.label and label_descriptor.family == "TypeDef" and label_descriptor.children:
            label_descriptor = label_descriptor.children[0]
        label = (label_descriptor.label.decode(encoding, errors="backslashreplace") if label_descriptor.label
                 else f"Slot {terminal.slot}")
        port = {"terminal_uid": terminal.terminal_uid, "binding_uid": terminal.binding_uid, "slot": terminal.slot,
                "label": label, "direction": "output" if terminal.output else "input",
                "source_flags": terminal.flags, "source_type": _descriptor_evidence(descriptor, encoding)}
        dtype = descriptor.native_type(encoding)
        if dtype is not None:
            port["value_type"] = dtype
        result.append(port)
    return result


def panel_terminal_type(widget):
    if widget["classId"] in {"frog.widgets.enum_control", "frog.widgets.enum_indicator"}:
        props = widget["props"]
        return f'enum<{json.dumps(props["enum.domain_id"])},{props["data_type.representation"]}>'
    return widget["valueType"]


def _native_type(widget, datatype, encoding):
    canonical = datatype.native_type(encoding)
    if canonical is None:
        return None
    while datatype.family == "TypeDef":
        datatype = datatype.children[0]
    if datatype.family == "Cluster":
        fields = sorted(widget.get("props", {}).get("cluster.elements", ()), key=lambda field: field["logical_order"])
        if len(fields) != len(datatype.children):
            return None
        for field, child in zip(fields, datatype.children):
            label = child.field_label()
            if not label or label.decode(encoding) != field["field_id"] or _native_type(field["widget"], child, encoding) is None:
                return None
        panel_spelling = "cluster:" + "|".join(field["widget"]["valueType"] for field in fields)
        return canonical if panel_terminal_type(widget) in (canonical, panel_spelling) else None
    return canonical if panel_terminal_type(widget) in (canonical, _source_type(datatype)) else None


def connector_input_policy(binding):
    # Both saved function descriptors must agree on the wire rule.
    # Unknown and contradictory flags do not authorize an unwired call.
    if binding.connector_flags == binding.call_flags == 0:
        return "optional"
    rules = {0x0800: "recommended", 0x1000: "required"}
    rule = binding.connector_flags & 0x1800
    return rules.get(rule) if rule == (binding.call_flags & 0x1800) else None


def bind_panel(vi, widgets, evidence, *, class_method=False, read_interface=read_connector):
    """Bind complete, exactly represented native panels to their real source slots."""
    try:
        connector = read_interface(vi)
    except ProfileRefusal as error:
        issues = [_note(error.resource, str(error), error.code)]
        evidence.update(status="unavailable", diagnostics=issues)
        return issues
    evidence.update(pattern=connector.pattern, pattern_source_id=connector.source_id,
                    slots=connector.slots, bindings=[], source_connection_policy="unverified")
    # Native by-value methods accept only calls with an actual producer for each input.
    input_policy = "required" if class_method else "optional"
    widgets_by_id = {widget["id"]: widget for widget in widgets}
    required_rows = max(sum(b.output == direction for b in connector.bindings) for direction in (False, True))
    rows = {True: 0, False: 0}
    assignments = []
    result = []
    for binding in connector.bindings:
        widget_id = f"lv-widget-{binding.widget_uid}"
        widget = widgets_by_id.get(widget_id)
        location = f"FPHb/uid:{binding.widget_uid}"
        direction = "indicator" if binding.output else "control"
        dtype = _native_type(widget, binding.datatype, vi.textEncoding) if widget is not None else None
        evidence["bindings"].append({"slot": binding.slot, "widget_uid": binding.widget_uid,
                                    "direction": "output" if binding.output else "input",
                                    "source_type": binding.datatype.family,
                                    "connector_flags": binding.connector_flags, "call_flags": binding.call_flags})
        if dtype is None or widget["role"] != direction:
            result.append(_note(location, "The complete source connector slot cannot be represented by a native widget.",
                                "connector_widget", slot=binding.slot,
                                source_type=binding.datatype.family))
            continue
        row = rows[binding.output]
        zone = (_SIX_ROW_SIDES[int(binding.output)][row] if 4 < required_rows <= 6
                else row * 2 + (2 if binding.output else 1))
        slot = f"zone_{zone}"
        rows[binding.output] += 1
        port = f"slot-{binding.slot}"
        mapping = {"mode": "widget_value", "interface_map_slot": slot,
                   "connection_requirement": ("optional" if binding.output else "required" if class_method
                                              else connector_input_policy(binding) or input_policy)}
        mapping["public_output_id" if binding.output else "public_input_id"] = port
        assignments.append((widget, dtype, mapping))
    if max(rows.values()) > max(_PATTERNS_BY_ROW):
        result.append(_note("FPHb/conPane", "The source uses more positions than this FROG connector can expose.",
                            "connector_capacity"))
    if not result:
        for widget, dtype, mapping in assignments:
            if widget["valueType"] != "enum_item_id":
                widget["valueType"] = dtype
            widget["binding"] = mapping
        if assignments:
            evidence["geometry"] = "frogs_native_connector"
        if rows[False]:
            evidence["input_connection_policy"] = ", ".join(sorted({
                mapping["connection_requirement"] for _, _, mapping in assignments if "public_input_id" in mapping}))
            evidence["source_connection_policy"] = "per_slot"
    evidence.update(status="unsupported" if result else "verified", diagnostics=result)
    return result


def _ports(widgets):
    inputs = []
    outputs = []
    for widget in widgets:
        binding = widget.get("binding")
        if not binding:
            continue
        output = "public_output_id" in binding
        port = binding.get("public_output_id" if output else "public_input_id")
        if port is None:
            continue
        description = {"id": port, "source_port_id": port, "direction": "output" if output else "input",
                       "label": widget["label"] or port, "value_type": panel_terminal_type(widget),
                       "connection_requirement": binding["connection_requirement"],
                       "interface_map_slot": binding["interface_map_slot"]}
        (outputs if output else inputs).append(description)
    for values in (inputs, outputs):
        values.sort(key=lambda port: int(port["interface_map_slot"].removeprefix("zone_")))
    return inputs, outputs


def panel_interface(widgets):
    """Project native ports only from verified widget bindings."""
    inputs, outputs = _ports(widgets)
    interface = {
        "inputs": [{"id": port["id"], "label": port["label"], "type": port["value_type"],
                    "connection": port["connection_requirement"]} for port in inputs],
        "outputs": [{"id": port["id"], "label": port["label"], "type": port["value_type"]} for port in outputs],
    }
    rows = max(len(inputs), len(outputs))
    if rows:
        interface["map"] = {"binding_map_profile": "square_40x40_v1",
                            "layout_pattern_id": _PATTERNS_BY_ROW[rows], "visual_transforms": []}
    return interface


def _fallback_icon(title):
    glyph = escape(title[:2].upper() or "LV", quote=True)
    return ('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 40 40">'
            '<rect x="3" y="3" width="34" height="34" rx="4" fill="#485362" '
            'stroke="#aebdcb" stroke-width="2"/>'
            f'<text x="20" y="25" text-anchor="middle" font-size="14" fill="#fff">{glyph}</text>'
            '</svg>')


def saved_vi_icon(vi):
    """Translate the VI's fixed-size indexed artwork to self-contained SVG."""
    from pylabview.LVmisc import LABVIEW_COLOR_PALETTE_2, LABVIEW_COLOR_PALETTE_16, LABVIEW_COLOR_PALETTE_256

    invalid = []
    for ident, bits, palette in ((b"icl8", 8, LABVIEW_COLOR_PALETTE_256),
                                 (b"icl4", 4, LABVIEW_COLOR_PALETTE_16),
                                 (b"ICON", 1, LABVIEW_COLOR_PALETTE_2)):
        block = vi.blocks.get(ident)
        if block is None:
            continue
        name = ident.decode("ascii")
        if len(block.sections) != 1 or block.size != 32 * 32 * bits // 8:
            invalid.append(name)
            continue
        try:
            raw = block.getRawData(next(iter(block.sections)))
        except (EOFError, OSError, ValueError):
            invalid.append(name)
            continue
        if raw is None or len(raw) != block.size:
            invalid.append(name)
            continue
        index_mask = (1 << bits) - 1
        shifts = range(8 - bits, -1, -bits)
        pixels = [palette[(byte >> shift) & index_mask] for byte in raw for shift in shifts]
        paths = {}
        for y in range(32):
            x = 0
            for color, run in groupby(pixels[y * 32:(y + 1) * 32]):
                width = sum(1 for _ in run)
                paths.setdefault(color, []).append(f"M{x} {y}h{width}v1h-{width}z")
                x += width
        artwork = ''.join(f'<path fill="#{color:06x}" d="{"".join(runs)}"/>' for color, runs in paths.items())
        return ('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 32 32">' + artwork + '</svg>',
                {"status": "saved", "resource": name, "bits_per_pixel": bits, "width": 32, "height": 32,
                 "invalid_resources": invalid})
    return None, {"status": "unavailable" if invalid else "absent", "resource": None, "invalid_resources": invalid}


def member_presentation(vi, widgets, title, *, class_method=False, read_interface=read_connector):
    connector = {}
    issues = bind_panel(vi, widgets, connector, class_method=class_method, read_interface=read_interface)
    icon, source = saved_vi_icon(vi)
    return {"interface": panel_interface(widgets), "icon": {"svg": icon or _fallback_icon(title)}}, {
        "connector": connector, "icon": source}, issues


def _unwired_call_default(terminal, binding, port, widget):
    requirement = port["connection_requirement"]
    flags = (terminal.flags, binding.connector_flags, binding.call_flags)
    if requirement == "recommended":
        if all(flag in (0x0800, 0x0a00) for flag in flags):
            return True
        # LV20 also saves a recommended array with these caller and connector
        # flags. The callee's decoded control supplies the actual empty value.
        if flags != (0x10a00, 0x10a00, 0x0800) or not widget or binding.datatype.family != "Array":
            return False
        props = widget["props"]
        return (widget["classId"] == "frog.widgets.array_control" and widget["role"] == "control"
                and widget["binding"].get("public_input_id") == port["id"]
                and props.get("dimensions.rank") == 1 and props.get("dimensions.shape") == [0]
                and widget["valueType"] == port["value_type"] == f'array<{props.get("element.value_type")}>')
    if (requirement == "optional" and flags == (0, 0, 0) and binding.datatype.family == "Boolean"
            and widget and widget["valueType"] == "bool" and widget["binding"].get("public_input_id") == port["id"]):
        saved = widget.get("state", {}).get("value")
        return isinstance(saved, bool) and widget.get("props", {}).get("value") is saved
    return False


def _method_integer_default(terminal, binding, port, widget):
    # Native class calls require a producer even for NI's recommended inputs.
    if (binding.output or terminal.output or not widget or widget["role"] != "control"
            or widget["binding"].get("public_input_id") != port["id"]
            or connector_input_policy(binding) != "recommended"
            or terminal.flags != binding.connector_flags or terminal.flags != binding.call_flags):
        return None
    dtype = TYPES.get(binding.datatype.family)
    if not dtype or dtype[0] not in ("i", "u") or dtype != port["value_type"] or dtype != widget["valueType"]:
        return None
    value = widget.get("state", {}).get("value")
    configured = widget.get("props", {}).get("value")
    if type(value) is not int or type(configured) is not int or configured != value:
        return None
    bits = int(dtype[1:])
    minimum = -(1 << (bits - 1)) if dtype[0] == "i" else 0
    return (port["id"], widget["id"], dtype, value) if minimum <= value < minimum + (1 << bits) else None


def _call_projection(node, call, target, source, bundle, panels, connected, read_interface, presentations):
    location = f"BDHb/uid:{node.uid}"
    if call.specialization is not None:
        require(isinstance(bundle, Bundle) and node.kind == "polyIUse" and call.dependency.kind == b"PUPV" and call.specialization.kind == b"IUVI"
                and target == resolve_local_path(call.specialization, source, bundle.root, bundle[source][0].textEncoding),
                "call_specialization", "The call does not name a verified local concrete instance.", location)
    cause = getattr(bundle, "unavailable", {}).get(target)
    if cause is not None:
        raise ProfileRefusal("call_dependency_unreadable", f"The saved call target could not be read: {cause}", location)
    require(target in bundle, "call_dependency",
            "The saved call target is outside the converted local VI bundle.", location)
    callee_vi, callee_path = bundle[target]
    pairs = match_call(call, read_interface(callee_vi), selected_variant=call.specialization is not None)
    require(target in panels, "call_panel", "The converted target has no available front panel.", location)
    inputs, outputs = _ports(panels[target])
    by_port = {port["id"]: port for port in inputs + outputs}
    expected_ports = {f"slot-{binding.slot}" for _, binding in pairs}
    require(expected_ports == by_port.keys(), "call_panel",
            "The converted target does not expose every source connector slot.", location)
    widgets = {w["id"]: w for w in panels[target]}
    method = getattr(bundle, "members", {}).get(target)
    defaults = []
    for terminal, binding in pairs:
        if not terminal.output and terminal.terminal_uid not in connected:
            port = by_port[f"slot-{binding.slot}"]
            widget = widgets.get(f"lv-widget-{binding.widget_uid}")
            saved = _method_integer_default(terminal, binding, port, widget) if method else None
            if saved is not None:
                defaults.append(saved)
                continue
            require(_unwired_call_default(terminal, binding, port, widget),
                    "call_default", f"Unwired input {port['id']} ({port['label']}) requires a verified callee default.", location)
    rows = max(len(inputs), len(outputs))
    if rows == 5:
        rows = 6
    for output, ports in ((False, inputs), (True, outputs)):
        for index, port in enumerate(ports):
            port.update(normalized_x=1.0 if output else 0.0, normalized_y=(index + 0.5) / rows)
    node_id = f"lv-call-{node.uid}"
    endpoints = {terminal.terminal_uid: (node_id, f"slot-{binding.slot}") for terminal, binding in pairs}
    require(node.bounds is not None, "call_layout", "The saved call has no readable bounds.", location)
    left, top, right, bottom = node.bounds
    title = target.stem
    icon = presentations[target]["icon"]
    member = {
        "id": node_id, "kind": "frog_call", "type": "frog.studio.frog_call",
        "label": title, "label_visible": True,
        "layout": bounds(left, top, max(1, right - left), max(1, bottom - top)),
        "call": {"document_path": Path(os.path.relpath(callee_path, bundle[source][1].parent)).as_posix(),
                 "document_title": title, "icon_svg": icon["svg"], "ports": inputs + outputs},
    }
    if method:
        relative, definition = next((p, cls) for p, cls in bundle.classes.items()
                                    if cls["id"] == method["class_id"])
        member["call"]["document_path"] = ""
        member["call"]["class_operation"] = {
            "kind": "static_call", "member_id": method["method_id"],
            "class": {"id": definition["id"], "revision": definition["revision"],
                      "path": Path(os.path.relpath(relative, bundle[source][1].parent)).as_posix()},
        }
    return member, endpoints, defaults


def _simple_error_handler_projection(node, call, encoding, wired, connected_inputs, connected_outputs):
    location = f"BDHb/uid:{node.uid}"
    layout = node.bounds
    require(layout is not None, "call_layout", "The saved call has no readable bounds.", location)
    connector = tuple((terminal.slot, terminal.output, terminal.flags, terminal.datatype.native_type(encoding))
                      for terminal in call.terminals)
    require(node.kind == "iUse" and call.specialization is None and (call.pattern, call.slots) == (161, 10)
            and connector == _SIMPLE_ERROR_HANDLER_CONNECTOR, "simple_error_handler_connector",
            "The saved Simple Error Handler connector differs from the qualified connector.", location)
    by_slot = {terminal.slot: terminal for terminal in call.terminals}
    dialog = by_slot[8].datatype
    require(dialog.family == "TypeDef" and dialog.definition == (b"DialogType.ctl",)
            and dialog.detail == (3101996562,), "simple_error_handler_connector",
            "The Simple Error Handler dialog type does not have the verified NI default.", location)
    input_uid = by_slot[9].terminal_uid
    message_uid = by_slot[7].terminal_uid
    require(set(node.terminals) & wired == {input_uid, message_uid} and
            input_uid in connected_inputs and message_uid in connected_outputs, "simple_error_handler_wiring",
            "Simple Error Handler requires a connected error in and message, with all other ports unwired.", location)
    node_id = f"lv-node-{node.uid}"
    left, top, right, bottom = layout
    member = {
        "id": node_id, "kind": "primitive", "type": "frog.studio.error.simple_handler",
        "palette_catalog_id": "error.simple_handler", "label": "Simple Error Handler.vi", "label_visible": True,
        "documentation": {"summary": f"Saved LabVIEW library dependency: <vilib>/Utility/error.llb/Simple Error Handler.vi. Source UID {node.uid}."},
        "layout": bounds(left, top, max(1, right - left), max(1, bottom - top)),
    }
    return member, {input_uid: (node_id, "error_in"), message_uid: (node_id, "message")}, []


def invalid_message_xnode_ports(node, graph, calls, types):
    """Return the host ports only for the saved LV20 invalid-message expansion."""
    if node.kind != "xNode" or len(node.regions) != 1 or len(node.terminals) != 2 or node.frames:
        return None
    region = graph.diagrams.get(node.regions[0])
    if region is None or region.owner != node.uid or len(region.nodes) != 3 or len(region.signals) != 7:
        return None
    children = {graph.nodes[uid].kind: graph.nodes[uid] for uid in region.nodes}
    if children.keys() != {"sRN", "printf", "iUse"} or any(
            child.regions or child.frames or child.boundary_bindings for child in children.values()):
        return None
    boundary, formatter, constructor = (children[kind] for kind in ("sRN", "printf", "iUse"))

    def binding(uid):
        return graph.bindings.get(graph.terminals[uid].binding)

    host, inner = {}, {}
    boundary_bindings = set()
    for uid in node.terminals:
        terminal = graph.terminals[uid]
        connector = binding(uid)
        if connector is None or connector.kind != "xTunnel":
            return None
        name = connector.fields.get("id")
        signature = (terminal.direction, (name.payload if name else None), types(connector))
        if signature == ("sink", b"parameter 0", "string"):
            port, child_direction = "message", "source"
        elif signature == ("source", b"error out", _ERROR_CLUSTER):
            port, child_direction = "error_out", "sink"
        else:
            return None
        mates = set(connector.terminals) - {uid}
        if uid not in connector.terminals or len(mates) != 1:
            return None
        (mate,) = mates
        child_term = graph.terminals.get(mate)
        if (child_term is None or child_term.owner != boundary.uid or child_term.direction != child_direction
                or child_term.binding != connector.uid):
            return None
        boundary_bindings.add(connector.uid)
        host[port], inner[port] = uid, mate
    if host.keys() != {"message", "error_out"} or set(node.boundary_bindings) != boundary_bindings:
        return None

    call = calls.get(constructor.uid)
    if call is None or call.specialization is not None or (call.pattern, call.slots) != (208, 10):
        return None
    dependency = call.dependency
    if (dependency.kind, dependency.path_encoding, dependency.path_type, dependency.path_components,
            dependency.qualified_name) != (b"IUVI", b"PTH0", 0, _ERROR_FROM_CODE_PATH, _ERROR_FROM_CODE_PATH[-1:]):
        return None
    if tuple((port.slot, port.output, port.flags, port.datatype.native_type(types.encoding))
             for port in call.terminals) != _ERROR_FROM_CODE_CONNECTOR:
        return None
    slots = {port.slot: port.terminal_uid for port in call.terminals}
    if not set(slots.values()) <= set(constructor.terminals):
        return None

    format_ports = [binding(uid) for uid in formatter.terminals]
    if any(port is None or port.kind != "printfArg" for port in format_ports):
        return None
    if tuple((graph.terminals[uid].direction, types(port), port.flags, port.type_label)
             for uid, port in zip(formatter.terminals, format_ports, strict=True)) != (
                 ("sink", "string", 65536, None), ("source", "string", 65536, None),
                 ("source", "string", 65537, None), ("source", _ERROR_CLUSTER, 65536, "error IO"),
                 ("source", _ERROR_CLUSTER, 65537, "error IO"), ("sink", "string", None, "parameter 0")):
        return None
    format_in, _, message_out, _, _, parameter_in = formatter.terminals

    sources = {}
    for uid in region.signals:
        signal = graph.signals[uid]
        if signal.source is None or len(signal.targets) != 1:
            return None
        sources[signal.targets[0]] = signal.source
    literals = {format_in: ("string", _INVALID_MESSAGE_FORMAT), slots[4]: ("bool", False),
                slots[5]: ("bool", True), slots[9]: ("i32", 1)}
    wires = {parameter_in: inner["message"], slots[8]: message_out, inner["error_out"]: slots[0]}
    if sources.keys() != wires.keys() | literals.keys() or any(sources[target] != uid for target, uid in wires.items()):
        return None
    if set(boundary.terminals) != set(inner.values()) | {sources[target] for target in literals}:
        return None
    for target, (datatype, expected) in literals.items():
        constant = binding(sources[target])
        if constant is None or constant.kind != "bDConstDCO" or types(constant) != datatype:
            return None
        value = constant.fields.get("ConstValue")
        try:
            if not value or decode_constant_value(value.payload, datatype, f"BDHb/uid:{constant.uid}") != expected:
                return None
        except ProfileRefusal:
            return None
    return host


def project_calls(vi, graph, source, bundle, panels, presentations, read_interface, types=None):
    """Project only verified native calls and return local notes for preserved source markers."""
    nodes = {node.uid: node for node in graph.nodes.values() if node.kind in _CALL_CLASSES}
    if not nodes:
        return {}
    source = Path(source).resolve()
    root = bundle.root
    try:
        dependencies = read_dependencies(vi, b"LIbd")
    except ProfileRefusal as error:
        return {uid: (None, {}, [_note(error.resource, str(error), error.code, uid=uid)], []) for uid in nodes}
    try:
        calls = read_call_links(vi, dependencies)
        call_error = None
    except ProfileRefusal as error:
        calls, call_error = {}, error

    poly_error = None
    poly_issues = {}
    if any(node.kind == "polyIUse" for node in nodes.values()):
        try:
            calls.update(read_call_links(vi, dependencies, polymorphic=True, occurrence_errors=poly_issues))
        except ProfileRefusal as error:
            poly_error = error
    connected = {uid for signal in graph.signals.values() if signal.source is not None for uid in signal.targets}
    wired = {uid for signal in graph.signals.values() for uid in signal.endpoints}
    producers = {signal.source for signal in graph.signals.values() if signal.source is not None and signal.targets}
    linked_nodes = {}
    for dependency in dependencies:
        if dependency.kind in {b"IUVI", b"PUPV"}:
            for uid in dependency.diagram_offsets:
                if dependency.kind == b"PUPV" or uid not in linked_nodes:
                    linked_nodes[uid] = dependency
    result = {}
    for uid, node in nodes.items():
        location = f"BDHb/uid:{uid}"
        call = calls.get(uid)
        dependency = (call.specialization or call.dependency) if call is not None else linked_nodes.get(uid)
        provenance = {"uid": uid}
        try:
            target = None
            if dependency:
                name = "::".join(part.decode(vi.textEncoding, errors="backslashreplace")
                                 for part in dependency.qualified_name)
                if name:
                    provenance["target"] = name
                if (dependency.path_encoding == b"PTH0" and dependency.path_type == 0
                        and dependency.path_components[:1] == (b"<vilib>",)):
                    provenance["library_target"] = "/".join(
                        part.decode(vi.textEncoding, errors="backslashreplace") for part in dependency.path_components)
                target = resolve_local_path(dependency, source, root, vi.textEncoding)
                if target in bundle:
                    provenance["member"] = bundle[target][1].as_posix()
            error = poly_issues.get(uid, poly_error) if node.kind == "polyIUse" else call_error
            if error is not None:
                raise error
            require(call is not None, "call_specialization",
                    "This saved call is not an ordinary VI connector binding.", location)
            if (dependency.kind == b"IUVI" and dependency.path_encoding == b"PTH0" and dependency.path_type == 0
                    and dependency.path_components == _SIMPLE_ERROR_HANDLER_PATH
                    and dependency.qualified_name == _SIMPLE_ERROR_HANDLER_PATH[-1:]):
                member, endpoints, defaults = _simple_error_handler_projection(
                    node, call, vi.textEncoding, wired, connected, producers)
            else:
                member, endpoints, defaults = _call_projection(node, call, target, source, bundle, panels, connected,
                                                               read_interface, presentations)
            result[uid] = (member, endpoints, [], defaults)
        except ProfileRefusal as error:
            if call is not None:
                provenance["ports"] = source_marker_ports(call, vi.textEncoding)
            result[uid] = (None, {}, [_note(error.resource, str(error), error.code, **provenance)], [])
    if types is not None and call_error is None and vi.getFileVersion()["major"] == 20:
        for node in graph.nodes.values():
            ports = invalid_message_xnode_ports(node, graph, calls, types)
            if ports is None:
                continue
            node_id = f"lv-node-{node.uid}"
            obj = {"id": node_id, "kind": "primitive", "type": "frog.studio.error.invalid_message",
                   "palette_catalog_id": "error.invalid_message", "label": "Invalid Message", "label_visible": True,
                   "documentation": {"summary": f"LabVIEW invalid-message xNode, source UID {node.uid}, saved diagram {node.regions[0]}."}}
            result[node.uid] = (obj, {uid: (node_id, port) for port, uid in ports.items()}, [], [])
    return result
