from __future__ import annotations

from copy import deepcopy
from typing import Any, Dict, List, Optional, Tuple

from Implementations.Reference.common import (
    DEMO_ARTIFACT_VERSION,
    LoadedSource,
    ValidationResult,
)


SUPPORTED_PRIMITIVES = {
    "frog.core.add",
    "frog.core.delay",
    "frog.ui.property_write",
}

SUPPORTED_WIDGET_CLASSES = {
    "frog.ui.standard.numeric_control",
    "frog.ui.standard.numeric_indicator",
}

SUPPORTED_WIDGET_ROLES = {
    "control",
    "indicator",
}

SUPPORTED_NUMERIC_TYPES = {
    "u16",
    "i32",
    "f64",
}

SUPPORTED_CONSTANT_TYPES = {
    "u16",
    "i32",
    "i64",
    "f64",
    "frog.ui.color",
}

SUPPORTED_COLOR_TYPE = "frog.ui.color"

SUPPORTED_FACE_TEMPLATE_FORMATS = {
    "svg",
}


def _fail(status: str, source_ref: Dict[str, Any], diagnostics: List[Dict[str, Any]]) -> ValidationResult:
    artifact = {
        "artifact_kind": "frog_validation_result",
        "artifact_version": DEMO_ARTIFACT_VERSION,
        "status": status,
        "source_ref": source_ref,
        "diagnostics": diagnostics,
    }
    return ValidationResult(artifact=artifact)


def _is_object(value: Any) -> bool:
    return isinstance(value, dict)


def _is_array(value: Any) -> bool:
    return isinstance(value, list)


def _diag(code: str, message: str, *, location: Optional[str] = None, severity: str = "error") -> Dict[str, Any]:
    diag = {
        "code": code,
        "severity": severity,
        "message": message,
    }
    if location is not None:
        diag["location"] = location
    return diag


def _require_top_level_sections(doc: Dict[str, Any], source_ref: Dict[str, Any]) -> Optional[ValidationResult]:
    required = ["spec_version", "metadata", "interface", "diagram"]
    missing = [name for name in required if name not in doc]
    if missing:
        return _fail(
            "structural_invalid",
            source_ref,
            [_diag("missing_top_level_sections", f"Missing top-level sections: {', '.join(missing)}.")],
        )
    return None


def _validate_interface(
    interface: Dict[str, Any],
    source_ref: Dict[str, Any],
) -> Tuple[Optional[ValidationResult], Dict[str, str], Dict[str, str]]:
    if not _is_object(interface):
        return (
            _fail(
                "structural_invalid",
                source_ref,
                [_diag("invalid_interface_shape", "Top-level 'interface' must be an object.")],
            ),
            {},
            {},
        )

    inputs = interface.get("inputs", [])
    outputs = interface.get("outputs", [])

    if not _is_array(inputs) or not _is_array(outputs):
        return (
            _fail(
                "structural_invalid",
                source_ref,
                [_diag("invalid_interface_ports_shape", "'interface.inputs' and 'interface.outputs' must be arrays.")],
            ),
            {},
            {},
        )

    input_types: Dict[str, str] = {}
    output_types: Dict[str, str] = {}
    diagnostics: List[Dict[str, Any]] = []

    for item in inputs:
        if not _is_object(item) or "id" not in item or "type" not in item:
            diagnostics.append(_diag("invalid_input_port", "Each interface input must contain 'id' and 'type'."))
            continue
        input_types[item["id"]] = item["type"]

    for item in outputs:
        if not _is_object(item) or "id" not in item or "type" not in item:
            diagnostics.append(_diag("invalid_output_port", "Each interface output must contain 'id' and 'type'."))
            continue
        output_types[item["id"]] = item["type"]

    if diagnostics:
        return _fail("structural_invalid", source_ref, diagnostics), {}, {}

    return None, input_types, output_types


def _validate_face_template(
    value: Any,
    *,
    widget_id: str,
    diagnostics: List[Dict[str, Any]],
) -> None:
    if not _is_object(value):
        diagnostics.append(
            _diag(
                "invalid_face_template_shape",
                "Widget 'face_template' must be an object.",
                location=f"front_panel.widget:{widget_id}.props.face_template",
            )
        )
        return

    kind = value.get("kind")
    fmt = value.get("format")
    path = value.get("path")

    if kind != "resource":
        diagnostics.append(
            _diag(
                "invalid_face_template_kind",
                "Widget 'face_template.kind' must equal 'resource' in the reference subset.",
                location=f"front_panel.widget:{widget_id}.props.face_template.kind",
            )
        )

    if fmt not in SUPPORTED_FACE_TEMPLATE_FORMATS:
        diagnostics.append(
            _diag(
                "unsupported_face_template_format",
                f"Unsupported face_template format in reference subset: {fmt}.",
                location=f"front_panel.widget:{widget_id}.props.face_template.format",
            )
        )

    if not isinstance(path, str) or not path:
        diagnostics.append(
            _diag(
                "invalid_face_template_path",
                "Widget 'face_template.path' must be a non-empty string.",
                location=f"front_panel.widget:{widget_id}.props.face_template.path",
            )
        )


def _validate_front_panel(
    front_panel: Dict[str, Any],
    source_ref: Dict[str, Any],
) -> Tuple[Optional[ValidationResult], Dict[str, Dict[str, Any]]]:
    if not _is_object(front_panel):
        return (
            _fail(
                "structural_invalid",
                source_ref,
                [_diag("invalid_front_panel_shape", "Top-level 'front_panel' must be an object when present.")],
            ),
            {},
        )

    widgets = front_panel.get("widgets", [])
    if not _is_array(widgets):
        return (
            _fail(
                "structural_invalid",
                source_ref,
                [_diag("invalid_front_panel_widgets_shape", "'front_panel.widgets' must be an array when present.")],
            ),
            {},
        )

    widget_by_id: Dict[str, Dict[str, Any]] = {}
    diagnostics: List[Dict[str, Any]] = []

    for widget in widgets:
        if not _is_object(widget):
            diagnostics.append(_diag("invalid_widget_shape", "Each front-panel widget must be an object."))
            continue

        widget_id = widget.get("id")
        widget_class = widget.get("widget")
        widget_role = widget.get("role")
        value_type = widget.get("value_type")
        props = widget.get("props", {})

        if not widget_id or not widget_class or not widget_role:
            diagnostics.append(_diag("missing_widget_fields", "Each widget must define 'id', 'widget', and 'role'."))
            continue

        if widget_id in widget_by_id:
            diagnostics.append(_diag("duplicate_widget_id", f"Duplicate widget id '{widget_id}'."))
            continue

        if widget_class not in SUPPORTED_WIDGET_CLASSES:
            diagnostics.append(
                _diag(
                    "unsupported_widget_class",
                    f"Unsupported widget class in reference subset: {widget_class}.",
                    location=f"front_panel.widget:{widget_id}",
                )
            )

        if widget_role not in SUPPORTED_WIDGET_ROLES:
            diagnostics.append(
                _diag(
                    "unsupported_widget_role",
                    f"Unsupported widget role in reference subset: {widget_role}.",
                    location=f"front_panel.widget:{widget_id}",
                )
            )

        if value_type != "u16":
            diagnostics.append(
                _diag(
                    "unsupported_widget_value_type",
                    f"Unsupported widget value_type in reference subset: {value_type}. Expected 'u16'.",
                    location=f"front_panel.widget:{widget_id}",
                )
            )

        if not _is_object(props):
            diagnostics.append(
                _diag(
                    "invalid_widget_props_shape",
                    "Widget 'props' must be an object when present.",
                    location=f"front_panel.widget:{widget_id}.props",
                )
            )
            props = {}

        if "face_color" in props and not isinstance(props["face_color"], str):
            diagnostics.append(
                _diag(
                    "invalid_face_color_shape",
                    "Widget 'face_color' must be a string in canonical source.",
                    location=f"front_panel.widget:{widget_id}.props.face_color",
                )
            )

        if "face_template" in props:
            _validate_face_template(props["face_template"], widget_id=widget_id, diagnostics=diagnostics)

        widget_by_id[widget_id] = deepcopy(widget)

    if diagnostics:
        return _fail("unsupported_but_valid", source_ref, diagnostics), {}

    return None, widget_by_id


def _collect_diagram_node_map(
    diagram: Dict[str, Any],
    source_ref: Dict[str, Any],
) -> Tuple[Optional[ValidationResult], Dict[str, Dict[str, Any]], List[Dict[str, Any]]]:
    if not _is_object(diagram):
        return (
            _fail(
                "structural_invalid",
                source_ref,
                [_diag("invalid_diagram_shape", "Top-level 'diagram' must be an object.")],
            ),
            {},
            [],
        )

    nodes = diagram.get("nodes", [])
    edges = diagram.get("edges", [])
    wire_fragments = diagram.get("wire_fragments", [])

    if not _is_array(nodes) or not _is_array(edges) or not _is_array(wire_fragments):
        return (
            _fail(
                "structural_invalid",
                source_ref,
                [
                    _diag(
                        "invalid_diagram_collections",
                        "'diagram.nodes', 'diagram.edges', and optional 'diagram.wire_fragments' must be arrays.",
                    )
                ],
            ),
            {},
            [],
        )

    if wire_fragments:
        return (
            _fail(
                "semantic_rejected",
                source_ref,
                [
                    _diag(
                        "incomplete_wire_fragments",
                        "The diagram contains incomplete authoring wires; repair or remove them before validation.",
                        location="diagram.wire_fragments",
                    )
                ],
            ),
            {},
            [],
        )

    node_map: Dict[str, Dict[str, Any]] = {}
    diagnostics: List[Dict[str, Any]] = []

    for node in nodes:
        if not _is_object(node) or "id" not in node or "kind" not in node:
            diagnostics.append(_diag("invalid_node_shape", "Each diagram node must define 'id' and 'kind'."))
            continue
        node_id = node["id"]
        if node_id in node_map:
            diagnostics.append(_diag("duplicate_node_id", f"Duplicate diagram node id '{node_id}'."))
            continue
        node_map[node_id] = deepcopy(node)

    if diagnostics:
        return _fail("structural_invalid", source_ref, diagnostics), {}, []

    return None, node_map, deepcopy(edges)


def _build_incoming_edge_map(
    edges: List[Dict[str, Any]],
    source_ref: Dict[str, Any],
) -> Tuple[Optional[ValidationResult], Dict[Tuple[str, str], List[Dict[str, Any]]]]:
    incoming: Dict[Tuple[str, str], List[Dict[str, Any]]] = {}
    diagnostics: List[Dict[str, Any]] = []

    for edge in edges:
        if not _is_object(edge):
            diagnostics.append(_diag("invalid_edge_shape", "Each diagram edge must be an object."))
            continue

        edge_id = edge.get("id")
        edge_from = edge.get("from")
        edge_to = edge.get("to")

        if not edge_id or not _is_object(edge_from) or not _is_object(edge_to):
            diagnostics.append(_diag("invalid_edge_fields", "Each edge must define 'id', 'from', and 'to'."))
            continue

        from_node = edge_from.get("node")
        from_port = edge_from.get("port")
        to_node = edge_to.get("node")
        to_port = edge_to.get("port")

        if not from_node or not from_port or not to_node or not to_port:
            diagnostics.append(_diag("invalid_edge_endpoints", "Each edge endpoint must define 'node' and 'port'."))
            continue

        key = (to_node, to_port)
        incoming.setdefault(key, []).append(deepcopy(edge))

    if diagnostics:
        return _fail("structural_invalid", source_ref, diagnostics), {}

    return None, incoming


def _find_constant_input_value(
    *,
    target_node_id: str,
    target_port: str,
    incoming: Dict[Tuple[str, str], List[Dict[str, Any]]],
    node_map: Dict[str, Dict[str, Any]],
) -> Optional[Any]:
    candidates = incoming.get((target_node_id, target_port), [])
    if len(candidates) != 1:
        return None
    edge = candidates[0]
    source_node = node_map.get(edge["from"]["node"])
    if source_node is None or source_node.get("kind") != "constant":
        return None
    return source_node.get("value")


def _resolve_source_port_type(
    *,
    node: Dict[str, Any],
    port: str,
    input_types: Dict[str, str],
    output_types: Dict[str, str],
    widget_by_id: Dict[str, Dict[str, Any]],
    structure_output_types: Dict[Tuple[str, str], str],
) -> Optional[str]:
    kind = node["kind"]

    if kind == "constant":
        return node.get("type")

    if kind == "interface_input" and port == "value":
        return input_types.get(node["interface_port"])

    if kind == "interface_output" and port == "value":
        return output_types.get(node["interface_port"])

    if kind == "widget_value" and port == "value":
        widget = widget_by_id.get(node["widget"])
        return None if widget is None else widget.get("value_type")

    if kind == "widget_reference" and port == "ref":
        return "widget_reference"

    if kind == "primitive":
        primitive_ref = node["type"]
        if primitive_ref == "frog.core.add" and port in {"a", "b", "result"}:
            return node.get("_resolved_value_type")
        if primitive_ref == "frog.core.delay" and port in {"in", "initial", "out"}:
            return node.get("_resolved_value_type")
        if primitive_ref == "frog.ui.property_write":
            if port == "ref":
                return "widget_reference"
            if port == "value":
                return node.get("_resolved_value_type")

    if kind == "structure":
        return structure_output_types.get((node["id"], port))

    return None


def _resolve_region_port_type(
    *,
    region_node: Dict[str, Any],
    port: str,
    boundary_input_types: Dict[str, str],
    boundary_output_types: Dict[str, str],
    structure_terminals: Dict[str, Dict[str, Any]],
) -> Optional[str]:
    kind = region_node.get("kind")

    if kind == "boundary_input" and port == "value":
        return boundary_input_types.get(region_node.get("boundary_port"))

    if kind == "boundary_output" and port == "value":
        return boundary_output_types.get(region_node.get("boundary_port"))

    if kind == "structure_terminal":
        terminal_name = region_node.get("terminal")
        terminal = structure_terminals.get(terminal_name, {})
        if port == "value":
            return terminal.get("type")

    if kind == "primitive":
        primitive_ref = region_node.get("type")
        if primitive_ref == "frog.core.add" and port in {"a", "b", "result"}:
            return region_node.get("_resolved_value_type")
        if primitive_ref == "frog.core.delay" and port in {"in", "initial", "out"}:
            return region_node.get("_resolved_value_type")

    return None


def _build_region_incoming_edge_map(
    region_edges: List[Dict[str, Any]],
    *,
    source_ref: Dict[str, Any],
    location_prefix: str,
) -> Tuple[Optional[ValidationResult], Dict[Tuple[str, str], List[Dict[str, Any]]]]:
    incoming: Dict[Tuple[str, str], List[Dict[str, Any]]] = {}
    diagnostics: List[Dict[str, Any]] = []

    for edge in region_edges:
        if not _is_object(edge):
            diagnostics.append(_diag("invalid_region_edge_shape", "Each region edge must be an object.", location=location_prefix))
            continue

        edge_id = edge.get("id")
        edge_from = edge.get("from")
        edge_to = edge.get("to")

        if not edge_id or not _is_object(edge_from) or not _is_object(edge_to):
            diagnostics.append(
                _diag("invalid_region_edge_fields", "Each region edge must define 'id', 'from', and 'to'.", location=location_prefix)
            )
            continue

        from_node = edge_from.get("node")
        from_port = edge_from.get("port")
        to_node = edge_to.get("node")
        to_port = edge_to.get("port")

        if not from_node or not from_port or not to_node or not to_port:
            diagnostics.append(
                _diag("invalid_region_edge_endpoints", "Each region edge endpoint must define 'node' and 'port'.", location=location_prefix)
            )
            continue

        incoming.setdefault((to_node, to_port), []).append(deepcopy(edge))

    if diagnostics:
        return _fail("structural_invalid", source_ref, diagnostics), {}

    return None, incoming


def _validate_for_loop_structure(
    *,
    node_id: str,
    node: Dict[str, Any],
    node_map: Dict[str, Dict[str, Any]],
    incoming: Dict[Tuple[str, str], List[Dict[str, Any]]],
    widget_by_id: Dict[str, Dict[str, Any]],
    input_types: Dict[str, str],
    output_types: Dict[str, str],
    structure_output_types: Dict[Tuple[str, str], str],
    source_ref: Dict[str, Any],
) -> Optional[ValidationResult]:
    diagnostics: List[Dict[str, Any]] = []

    if node.get("kind") != "structure" or node.get("structure_type") != "for_loop":
        diagnostics.append(
            _diag(
                "unsupported_structure_kind",
                "Reference subset expects a canonical structure node with structure_type 'for_loop'.",
                location=f"diagram.node:{node_id}",
            )
        )
        return _fail("unsupported_but_valid", source_ref, diagnostics)

    boundary = node.get("boundary")
    structure_terminals = node.get("structure_terminals")
    regions = node.get("regions")

    if not _is_object(boundary):
        diagnostics.append(_diag("invalid_structure_boundary", "for_loop 'boundary' must be an object.", location=f"diagram.node:{node_id}"))
        return _fail("structural_invalid", source_ref, diagnostics)

    if not _is_object(structure_terminals):
        diagnostics.append(
            _diag("invalid_structure_terminals", "for_loop 'structure_terminals' must be an object.", location=f"diagram.node:{node_id}")
        )
        return _fail("structural_invalid", source_ref, diagnostics)

    if not _is_array(regions):
        diagnostics.append(_diag("invalid_structure_regions", "for_loop 'regions' must be an array.", location=f"diagram.node:{node_id}"))
        return _fail("structural_invalid", source_ref, diagnostics)

    boundary_inputs = boundary.get("inputs", [])
    boundary_outputs = boundary.get("outputs", [])

    if not _is_array(boundary_inputs) or not _is_array(boundary_outputs):
        diagnostics.append(
            _diag(
                "invalid_structure_boundary_collections",
                "for_loop boundary inputs and outputs must be arrays.",
                location=f"diagram.node:{node_id}",
            )
        )
        return _fail("structural_invalid", source_ref, diagnostics)

    boundary_input_types: Dict[str, str] = {}
    boundary_output_types: Dict[str, str] = {}

    expected_inputs = {"input_value", "initial_state"}
    expected_outputs = {"final_value"}

    actual_inputs = set()
    actual_outputs = set()

    for item in boundary_inputs:
        if not _is_object(item) or "id" not in item or "type" not in item:
            diagnostics.append(
                _diag(
                    "invalid_for_loop_boundary_input",
                    "Each for_loop boundary input must define 'id' and 'type'.",
                    location=f"diagram.node:{node_id}",
                )
            )
            continue
        actual_inputs.add(item["id"])
        boundary_input_types[item["id"]] = item["type"]

    for item in boundary_outputs:
        if not _is_object(item) or "id" not in item or "type" not in item:
            diagnostics.append(
                _diag(
                    "invalid_for_loop_boundary_output",
                    "Each for_loop boundary output must define 'id' and 'type'.",
                    location=f"diagram.node:{node_id}",
                )
            )
            continue
        actual_outputs.add(item["id"])
        boundary_output_types[item["id"]] = item["type"]

    if actual_inputs != expected_inputs:
        diagnostics.append(
            _diag(
                "unsupported_for_loop_boundary_inputs",
                "Reference subset expects exactly boundary inputs 'input_value' and 'initial_state'.",
                location=f"diagram.node:{node_id}",
            )
        )

    if actual_outputs != expected_outputs:
        diagnostics.append(
            _diag(
                "unsupported_for_loop_boundary_outputs",
                "Reference subset expects exactly boundary output 'final_value'.",
                location=f"diagram.node:{node_id}",
            )
        )

    if diagnostics:
        return _fail("unsupported_but_valid", source_ref, diagnostics)

    input_value_type = boundary_input_types["input_value"]
    initial_state_type = boundary_input_types["initial_state"]
    final_value_type = boundary_output_types["final_value"]

    if input_value_type != initial_state_type or input_value_type != final_value_type:
        diagnostics.append(
            _diag(
                "invalid_for_loop_boundary_types",
                "for_loop boundary input/output types must match in the reference subset.",
                location=f"diagram.node:{node_id}",
            )
        )

    if input_value_type != "u16":
        diagnostics.append(
            _diag(
                "unsupported_for_loop_value_type",
                f"Unsupported for_loop value type in reference subset: {input_value_type}. Expected 'u16'.",
                location=f"diagram.node:{node_id}",
            )
        )

    final_output = next((item for item in boundary_outputs if item["id"] == "final_value"), None)
    if final_output is None or final_output.get("mode") != "last_value":
        diagnostics.append(
            _diag(
                "invalid_for_loop_output_mode",
                "Reference subset expects boundary output 'final_value' with mode 'last_value'.",
                location=f"diagram.node:{node_id}",
            )
        )
    elif final_output.get("zero_iteration_value") != 0:
        diagnostics.append(
            _diag(
                "invalid_for_loop_zero_iteration_value",
                "Reference subset expects boundary output 'final_value' zero_iteration_value to equal 0.",
                location=f"diagram.node:{node_id}",
            )
        )

    count_terminal = structure_terminals.get("count")
    index_terminal = structure_terminals.get("index")

    if not _is_object(count_terminal) or count_terminal.get("type") != "i64" or count_terminal.get("outer_visible") is not True:
        diagnostics.append(
            _diag(
                "invalid_for_loop_count_terminal",
                "Reference subset expects a 'count' terminal of type 'i64' with outer_visible true.",
                location=f"diagram.node:{node_id}",
            )
        )

    if not _is_object(index_terminal) or index_terminal.get("type") != "i64" or index_terminal.get("exposed_in_body") is not True:
        diagnostics.append(
            _diag(
                "invalid_for_loop_index_terminal",
                "Reference subset expects an 'index' terminal of type 'i64' exposed in the body.",
                location=f"diagram.node:{node_id}",
            )
        )

    incoming_count = incoming.get((node_id, "count"), [])
    incoming_input_value = incoming.get((node_id, "input_value"), [])
    incoming_initial_state = incoming.get((node_id, "initial_state"), [])

    if len(incoming_count) != 1:
        diagnostics.append(
            _diag(
                "invalid_for_loop_count_input",
                "Reference subset expects exactly one incoming edge for structure terminal 'count'.",
                location=f"diagram.node:{node_id}",
            )
        )
    if len(incoming_input_value) != 1 or len(incoming_initial_state) != 1:
        diagnostics.append(
            _diag(
                "invalid_for_loop_boundary_inputs_edges",
                "Reference subset expects exactly one incoming edge for 'input_value' and 'initial_state'.",
                location=f"diagram.node:{node_id}",
            )
        )

    if diagnostics:
        return _fail("semantic_rejected", source_ref, diagnostics)

    count_source_edge = incoming_count[0]
    count_source_node = node_map.get(count_source_edge["from"]["node"])
    count_source_port = count_source_edge["from"]["port"]

    count_source_type = None
    if count_source_node is not None:
        count_source_type = _resolve_source_port_type(
            node=count_source_node,
            port=count_source_port,
            input_types=input_types,
            output_types=output_types,
            widget_by_id=widget_by_id,
            structure_output_types=structure_output_types,
        )

    if count_source_node is None or count_source_node.get("kind") != "constant" or count_source_port != "value":
        diagnostics.append(
            _diag(
                "unsupported_for_loop_count_source",
                "Reference subset expects structure terminal 'count' to be driven by a constant node 'value' port.",
                location=f"diagram.node:{node_id}",
            )
        )
    elif count_source_type != "i64" or int(count_source_node.get("value")) != 5:
        diagnostics.append(
            _diag(
                "unsupported_for_loop_iteration_count",
                "Reference subset currently supports only a constant i64 iteration count of exactly 5.",
                location=f"diagram.node:{node_id}",
            )
        )

    input_source_edge = incoming_input_value[0]
    initial_source_edge = incoming_initial_state[0]

    input_source_node = node_map.get(input_source_edge["from"]["node"])
    initial_source_node = node_map.get(initial_source_edge["from"]["node"])

    input_source_type = None
    initial_source_type = None

    if input_source_node is not None:
        input_source_type = _resolve_source_port_type(
            node=input_source_node,
            port=input_source_edge["from"]["port"],
            input_types=input_types,
            output_types=output_types,
            widget_by_id=widget_by_id,
            structure_output_types=structure_output_types,
        )

    if initial_source_node is not None:
        initial_source_type = _resolve_source_port_type(
            node=initial_source_node,
            port=initial_source_edge["from"]["port"],
            input_types=input_types,
            output_types=output_types,
            widget_by_id=widget_by_id,
            structure_output_types=structure_output_types,
        )

    if input_source_type != "u16" or initial_source_type != "u16":
        diagnostics.append(
            _diag(
                "invalid_for_loop_input_types",
                "Reference subset expects both 'input_value' and 'initial_state' inputs to resolve to type 'u16'.",
                location=f"diagram.node:{node_id}",
            )
        )

    if len(regions) != 1 or not _is_object(regions[0]) or regions[0].get("id") != "body":
        diagnostics.append(
            _diag(
                "invalid_for_loop_regions",
                "Reference subset expects exactly one for_loop region with id 'body'.",
                location=f"diagram.node:{node_id}",
            )
        )
        return _fail("semantic_rejected", source_ref, diagnostics)

    body_region = regions[0]
    body_diagram = body_region.get("diagram")

    if not _is_object(body_diagram):
        diagnostics.append(
            _diag(
                "invalid_for_loop_body_diagram",
                "for_loop body region must define a valid 'diagram' object.",
                location=f"diagram.node:{node_id}.regions:body",
            )
        )
        return _fail("structural_invalid", source_ref, diagnostics)

    region_nodes = body_diagram.get("nodes", [])
    region_edges = body_diagram.get("edges", [])

    if not _is_array(region_nodes) or not _is_array(region_edges):
        diagnostics.append(
            _diag(
                "invalid_for_loop_body_collections",
                "for_loop body diagram must define 'nodes' and 'edges' arrays.",
                location=f"diagram.node:{node_id}.regions:body",
            )
        )
        return _fail("structural_invalid", source_ref, diagnostics)

    region_node_map: Dict[str, Dict[str, Any]] = {}
    for region_node in region_nodes:
        if not _is_object(region_node) or "id" not in region_node or "kind" not in region_node:
            diagnostics.append(
                _diag(
                    "invalid_region_node_shape",
                    "Each region node must define 'id' and 'kind'.",
                    location=f"diagram.node:{node_id}.regions:body",
                )
            )
            continue
        region_node_id = region_node["id"]
        if region_node_id in region_node_map:
            diagnostics.append(
                _diag(
                    "duplicate_region_node_id",
                    f"Duplicate region node id '{region_node_id}'.",
                    location=f"diagram.node:{node_id}.regions:body",
                )
            )
            continue
        region_node_map[region_node_id] = deepcopy(region_node)

    if diagnostics:
        return _fail("structural_invalid", source_ref, diagnostics)

    region_incoming_failure, region_incoming = _build_region_incoming_edge_map(
        region_edges,
        source_ref=source_ref,
        location_prefix=f"diagram.node:{node_id}.regions:body",
    )
    if region_incoming_failure is not None:
        return region_incoming_failure

    expected_region_kinds = {
        "body_input_value": "boundary_input",
        "body_initial_state": "boundary_input",
        "body_index": "structure_terminal",
        "delay_state": "primitive",
        "add_step": "primitive",
        "body_final_value": "boundary_output",
    }

    for expected_id, expected_kind in expected_region_kinds.items():
        region_node = region_node_map.get(expected_id)
        if region_node is None or region_node.get("kind") != expected_kind:
            diagnostics.append(
                _diag(
                    "unsupported_for_loop_region_shape",
                    f"Reference subset expects region node '{expected_id}' of kind '{expected_kind}'.",
                    location=f"diagram.node:{node_id}.regions:body",
                )
            )

    if diagnostics:
        return _fail("unsupported_but_valid", source_ref, diagnostics)

    if region_node_map["body_input_value"].get("boundary_port") != "input_value":
        diagnostics.append(
            _diag(
                "invalid_region_boundary_input_mapping",
                "Region node 'body_input_value' must reference boundary port 'input_value'.",
                location=f"diagram.node:{node_id}.regions:body.node:body_input_value",
            )
        )

    if region_node_map["body_initial_state"].get("boundary_port") != "initial_state":
        diagnostics.append(
            _diag(
                "invalid_region_boundary_input_mapping",
                "Region node 'body_initial_state' must reference boundary port 'initial_state'.",
                location=f"diagram.node:{node_id}.regions:body.node:body_initial_state",
            )
        )

    if region_node_map["body_final_value"].get("boundary_port") != "final_value":
        diagnostics.append(
            _diag(
                "invalid_region_boundary_output_mapping",
                "Region node 'body_final_value' must reference boundary port 'final_value'.",
                location=f"diagram.node:{node_id}.regions:body.node:body_final_value",
            )
        )

    if region_node_map["body_index"].get("terminal") != "index":
        diagnostics.append(
            _diag(
                "invalid_region_structure_terminal_mapping",
                "Region node 'body_index' must reference structure terminal 'index'.",
                location=f"diagram.node:{node_id}.regions:body.node:body_index",
            )
        )

    if region_node_map["delay_state"].get("type") != "frog.core.delay":
        diagnostics.append(
            _diag(
                "invalid_region_delay_primitive",
                "Region node 'delay_state' must be primitive 'frog.core.delay'.",
                location=f"diagram.node:{node_id}.regions:body.node:delay_state",
            )
        )

    if region_node_map["add_step"].get("type") != "frog.core.add":
        diagnostics.append(
            _diag(
                "invalid_region_add_primitive",
                "Region node 'add_step' must be primitive 'frog.core.add'.",
                location=f"diagram.node:{node_id}.regions:body.node:add_step",
            )
        )

    if diagnostics:
        return _fail("semantic_rejected", source_ref, diagnostics)

    add_node = region_node_map["add_step"]
    delay_node = region_node_map["delay_state"]

    add_incoming_a = region_incoming.get(("add_step", "a"), [])
    add_incoming_b = region_incoming.get(("add_step", "b"), [])
    delay_incoming_in = region_incoming.get(("delay_state", "in"), [])
    delay_incoming_initial = region_incoming.get(("delay_state", "initial"), [])
    body_final_incoming = region_incoming.get(("body_final_value", "value"), [])

    if len(add_incoming_a) != 1 or len(add_incoming_b) != 1:
        diagnostics.append(
            _diag(
                "invalid_region_add_inputs",
                "Region primitive 'add_step' requires exactly one incoming edge on ports 'a' and 'b'.",
                location=f"diagram.node:{node_id}.regions:body.node:add_step",
            )
        )

    if len(delay_incoming_in) != 1 or len(delay_incoming_initial) != 1:
        diagnostics.append(
            _diag(
                "invalid_region_delay_inputs",
                "Region primitive 'delay_state' requires exactly one incoming edge on ports 'in' and 'initial'.",
                location=f"diagram.node:{node_id}.regions:body.node:delay_state",
            )
        )

    if len(body_final_incoming) != 1:
        diagnostics.append(
            _diag(
                "invalid_region_boundary_output_input",
                "Region boundary output 'body_final_value' requires exactly one incoming edge on port 'value'.",
                location=f"diagram.node:{node_id}.regions:body.node:body_final_value",
            )
        )

    if diagnostics:
        return _fail("semantic_rejected", source_ref, diagnostics)

    add_a_source = region_node_map[add_incoming_a[0]["from"]["node"]]
    add_b_source = region_node_map[add_incoming_b[0]["from"]["node"]]
    delay_in_source = region_node_map[delay_incoming_in[0]["from"]["node"]]
    delay_initial_source = region_node_map[delay_incoming_initial[0]["from"]["node"]]
    body_final_source = region_node_map[body_final_incoming[0]["from"]["node"]]

    add_a_type = _resolve_region_port_type(
        region_node=add_a_source,
        port=add_incoming_a[0]["from"]["port"],
        boundary_input_types=boundary_input_types,
        boundary_output_types=boundary_output_types,
        structure_terminals=structure_terminals,
    )
    add_b_type = _resolve_region_port_type(
        region_node=add_b_source,
        port=add_incoming_b[0]["from"]["port"],
        boundary_input_types=boundary_input_types,
        boundary_output_types=boundary_output_types,
        structure_terminals=structure_terminals,
    )

    if add_a_type != "u16" or add_b_type != "u16" or add_a_type != add_b_type:
        diagnostics.append(
            _diag(
                "invalid_region_add_types",
                "Region primitive 'add_step' requires both inputs to resolve to type 'u16' in the reference subset.",
                location=f"diagram.node:{node_id}.regions:body.node:add_step",
            )
        )
    else:
        add_node["_resolved_value_type"] = "u16"

    delay_initial_type = _resolve_region_port_type(
        region_node=delay_initial_source,
        port=delay_incoming_initial[0]["from"]["port"],
        boundary_input_types=boundary_input_types,
        boundary_output_types=boundary_output_types,
        structure_terminals=structure_terminals,
    )
    delay_in_type = _resolve_region_port_type(
        region_node=delay_in_source,
        port=delay_incoming_in[0]["from"]["port"],
        boundary_input_types=boundary_input_types,
        boundary_output_types=boundary_output_types,
        structure_terminals=structure_terminals,
    )

    if delay_initial_type != "u16" or delay_in_type != "u16" or delay_initial_type != delay_in_type:
        diagnostics.append(
            _diag(
                "invalid_region_delay_types",
                "Region primitive 'delay_state' requires both 'in' and 'initial' to resolve to type 'u16' in the reference subset.",
                location=f"diagram.node:{node_id}.regions:body.node:delay_state",
            )
        )
    else:
        delay_node["_resolved_value_type"] = "u16"

    if not (body_final_source.get("id") == "add_step" and body_final_incoming[0]["from"]["port"] == "result"):
        diagnostics.append(
            _diag(
                "invalid_region_boundary_output_source",
                "Region boundary output 'body_final_value' must be driven from 'add_step.result' in the reference subset.",
                location=f"diagram.node:{node_id}.regions:body.node:body_final_value",
            )
        )

    if not (add_a_source.get("id") == "body_input_value" and add_incoming_a[0]["from"]["port"] == "value"):
        diagnostics.append(
            _diag(
                "invalid_region_add_a_source",
                "Region primitive 'add_step.a' must be driven by 'body_input_value.value' in the reference subset.",
                location=f"diagram.node:{node_id}.regions:body.node:add_step",
            )
        )

    if not (add_b_source.get("id") == "delay_state" and add_incoming_b[0]["from"]["port"] == "out"):
        diagnostics.append(
            _diag(
                "invalid_region_add_b_source",
                "Region primitive 'add_step.b' must be driven by 'delay_state.out' in the reference subset.",
                location=f"diagram.node:{node_id}.regions:body.node:add_step",
            )
        )

    if not (delay_in_source.get("id") == "add_step" and delay_incoming_in[0]["from"]["port"] == "result"):
        diagnostics.append(
            _diag(
                "invalid_region_delay_in_source",
                "Region primitive 'delay_state.in' must be driven by 'add_step.result' in the reference subset.",
                location=f"diagram.node:{node_id}.regions:body.node:delay_state",
            )
        )

    if not (delay_initial_source.get("id") == "body_initial_state" and delay_incoming_initial[0]["from"]["port"] == "value"):
        diagnostics.append(
            _diag(
                "invalid_region_delay_initial_source",
                "Region primitive 'delay_state.initial' must be driven by 'body_initial_state.value' in the reference subset.",
                location=f"diagram.node:{node_id}.regions:body.node:delay_state",
            )
        )

    if diagnostics:
        return _fail("semantic_rejected", source_ref, diagnostics)

    node["_resolved_value_type"] = "u16"
    structure_output_types[(node_id, "final_value")] = "u16"
    return None


def _validate_and_enrich_nodes(
    *,
    node_map: Dict[str, Dict[str, Any]],
    edges: List[Dict[str, Any]],
    incoming: Dict[Tuple[str, str], List[Dict[str, Any]]],
    widget_by_id: Dict[str, Dict[str, Any]],
    input_types: Dict[str, str],
    output_types: Dict[str, str],
    source_ref: Dict[str, Any],
) -> Optional[ValidationResult]:
    diagnostics: List[Dict[str, Any]] = []
    structure_output_types: Dict[Tuple[str, str], str] = {}

    for node_id, node in node_map.items():
        kind = node["kind"]

        if kind == "constant":
            node_type = node.get("type")
            if node_type not in SUPPORTED_CONSTANT_TYPES:
                diagnostics.append(
                    _diag(
                        "unsupported_constant_type",
                        f"Unsupported constant type in reference subset: {node_type}.",
                        location=f"diagram.node:{node_id}",
                    )
                )

        elif kind == "interface_input":
            port_id = node.get("interface_port")
            if port_id not in input_types:
                diagnostics.append(
                    _diag(
                        "unknown_interface_input",
                        f"Unknown interface input '{port_id}'.",
                        location=f"diagram.node:{node_id}",
                    )
                )

        elif kind == "interface_output":
            port_id = node.get("interface_port")
            if port_id not in output_types:
                diagnostics.append(
                    _diag(
                        "unknown_interface_output",
                        f"Unknown interface output '{port_id}'.",
                        location=f"diagram.node:{node_id}",
                    )
                )

        elif kind == "widget_value":
            widget_id = node.get("widget")
            widget = widget_by_id.get(widget_id)
            if widget is None:
                diagnostics.append(
                    _diag(
                        "unknown_widget_for_widget_value",
                        f"Unknown widget '{widget_id}' for widget_value node.",
                        location=f"diagram.node:{node_id}",
                    )
                )

        elif kind == "widget_reference":
            widget_id = node.get("widget")
            widget = widget_by_id.get(widget_id)
            if widget is None:
                diagnostics.append(
                    _diag(
                        "unknown_widget_for_widget_reference",
                        f"Unknown widget '{widget_id}' for widget_reference node.",
                        location=f"diagram.node:{node_id}",
                    )
                )

        elif kind == "primitive":
            primitive_ref = node.get("type")
            if primitive_ref not in SUPPORTED_PRIMITIVES:
                diagnostics.append(
                    _diag(
                        "unsupported_primitive",
                        f"Unsupported primitive in reference subset: {primitive_ref}.",
                        location=f"diagram.node:{node_id}",
                    )
                )
                continue

            if primitive_ref == "frog.core.add":
                incoming_a = incoming.get((node_id, "a"), [])
                incoming_b = incoming.get((node_id, "b"), [])
                if len(incoming_a) != 1 or len(incoming_b) != 1:
                    diagnostics.append(
                        _diag(
                            "invalid_add_inputs",
                            "frog.core.add requires exactly one incoming edge for ports 'a' and 'b'.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                src_a = node_map[incoming_a[0]["from"]["node"]]
                src_b = node_map[incoming_b[0]["from"]["node"]]
                type_a = _resolve_source_port_type(
                    node=src_a,
                    port=incoming_a[0]["from"]["port"],
                    input_types=input_types,
                    output_types=output_types,
                    widget_by_id=widget_by_id,
                    structure_output_types=structure_output_types,
                )
                type_b = _resolve_source_port_type(
                    node=src_b,
                    port=incoming_b[0]["from"]["port"],
                    input_types=input_types,
                    output_types=output_types,
                    widget_by_id=widget_by_id,
                    structure_output_types=structure_output_types,
                )

                if type_a is None or type_b is None:
                    diagnostics.append(
                        _diag(
                            "unresolved_add_input_type",
                            "frog.core.add requires resolvable input types.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                if type_a not in SUPPORTED_NUMERIC_TYPES or type_b not in SUPPORTED_NUMERIC_TYPES:
                    diagnostics.append(
                        _diag(
                            "unsupported_add_type",
                            "frog.core.add supports numeric types only in the reference subset.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                if type_a != type_b:
                    diagnostics.append(
                        _diag(
                            "type_mismatch_add",
                            f"frog.core.add input type mismatch: '{type_a}' vs '{type_b}'.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                node["_resolved_value_type"] = type_a

            elif primitive_ref == "frog.core.delay":
                incoming_in = incoming.get((node_id, "in"), [])
                incoming_initial = incoming.get((node_id, "initial"), [])

                if len(incoming_in) != 1 or len(incoming_initial) != 1:
                    diagnostics.append(
                        _diag(
                            "invalid_delay_inputs",
                            "frog.core.delay requires exactly one incoming edge for ports 'in' and 'initial'.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                src_in = node_map[incoming_in[0]["from"]["node"]]
                src_initial = node_map[incoming_initial[0]["from"]["node"]]

                type_in = _resolve_source_port_type(
                    node=src_in,
                    port=incoming_in[0]["from"]["port"],
                    input_types=input_types,
                    output_types=output_types,
                    widget_by_id=widget_by_id,
                    structure_output_types=structure_output_types,
                )
                type_initial = _resolve_source_port_type(
                    node=src_initial,
                    port=incoming_initial[0]["from"]["port"],
                    input_types=input_types,
                    output_types=output_types,
                    widget_by_id=widget_by_id,
                    structure_output_types=structure_output_types,
                )

                if type_in is None or type_initial is None:
                    diagnostics.append(
                        _diag(
                            "unresolved_delay_input_type",
                            "frog.core.delay requires resolvable input types.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                if type_in != type_initial:
                    diagnostics.append(
                        _diag(
                            "type_mismatch_delay",
                            f"frog.core.delay input type mismatch: '{type_in}' vs '{type_initial}'.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                if type_in not in SUPPORTED_NUMERIC_TYPES:
                    diagnostics.append(
                        _diag(
                            "unsupported_delay_type",
                            f"Unsupported frog.core.delay type in reference subset: {type_in}.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                node["_resolved_value_type"] = type_in

                const_initial = _find_constant_input_value(
                    target_node_id=node_id,
                    target_port="initial",
                    incoming=incoming,
                    node_map=node_map,
                )
                if const_initial is not None:
                    if type_in == "f64":
                        node["_coerced_initial"] = float(const_initial)
                    elif type_in in {"u16", "i32"}:
                        node["_coerced_initial"] = int(const_initial)

            elif primitive_ref == "frog.ui.property_write":
                incoming_ref = incoming.get((node_id, "ref"), [])
                incoming_value = incoming.get((node_id, "value"), [])

                if len(incoming_ref) != 1 or len(incoming_value) != 1:
                    diagnostics.append(
                        _diag(
                            "invalid_property_write_inputs",
                            "frog.ui.property_write requires exactly one incoming edge for ports 'ref' and 'value'.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                ref_source = node_map[incoming_ref[0]["from"]["node"]]
                if ref_source["kind"] != "widget_reference":
                    diagnostics.append(
                        _diag(
                            "invalid_property_write_ref_source",
                            "frog.ui.property_write 'ref' must come from a widget_reference node.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                widget_id = ref_source["widget"]
                widget = widget_by_id.get(widget_id)
                if widget is None:
                    diagnostics.append(
                        _diag(
                            "unknown_property_write_widget",
                            f"Unknown widget '{widget_id}' in property_write reference path.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                member = node.get("widget_member")
                if not _is_object(member) or "member" not in member:
                    diagnostics.append(
                        _diag(
                            "invalid_property_write_member",
                            "frog.ui.property_write requires a 'widget_member' object with a 'member' field.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                part_name = member.get("part")
                member_name = member["member"]

                if part_name is not None:
                    diagnostics.append(
                        _diag(
                            "unsupported_part_property_write",
                            "The current reference subset supports only root-level property writes.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                if member_name != "face_color":
                    diagnostics.append(
                        _diag(
                            "unsupported_property_write_member",
                            f"Unsupported property_write target in reference subset: {member_name}.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                value_source = node_map[incoming_value[0]["from"]["node"]]
                value_type = _resolve_source_port_type(
                    node=value_source,
                    port=incoming_value[0]["from"]["port"],
                    input_types=input_types,
                    output_types=output_types,
                    widget_by_id=widget_by_id,
                    structure_output_types=structure_output_types,
                )

                if value_type != SUPPORTED_COLOR_TYPE:
                    diagnostics.append(
                        _diag(
                            "invalid_property_write_value_type",
                            f"face_color property_write requires value type '{SUPPORTED_COLOR_TYPE}', got '{value_type}'.",
                            location=f"diagram.node:{node_id}",
                        )
                    )
                    continue

                node["_resolved_value_type"] = SUPPORTED_COLOR_TYPE
                node["_resolved_widget_id"] = widget_id
                node["_resolved_widget_class"] = widget["widget"]

        elif kind == "structure":
            structure_type = node.get("structure_type")
            if structure_type != "for_loop":
                diagnostics.append(
                    _diag(
                        "unsupported_structure_type",
                        f"Unsupported structure_type in reference subset: {structure_type}.",
                        location=f"diagram.node:{node_id}",
                    )
                )
                continue

            structure_failure = _validate_for_loop_structure(
                node_id=node_id,
                node=node,
                node_map=node_map,
                incoming=incoming,
                widget_by_id=widget_by_id,
                input_types=input_types,
                output_types=output_types,
                structure_output_types=structure_output_types,
                source_ref=source_ref,
            )
            if structure_failure is not None:
                return structure_failure

        else:
            diagnostics.append(
                _diag(
                    "unsupported_node_kind",
                    f"Unsupported diagram node kind in reference subset: {kind}.",
                    location=f"diagram.node:{node_id}",
                )
            )

    if diagnostics:
        status = "unsupported_but_valid"
        if any(
            diag["code"].startswith("invalid_")
            or diag["code"].startswith("missing_")
            or diag["code"].startswith("type_mismatch")
            for diag in diagnostics
        ):
            status = "semantic_rejected"
        return _fail(status, source_ref, diagnostics)

    return None


def validate_source(loaded: LoadedSource) -> ValidationResult:
    source_ref = dict(loaded.artifact["source"])

    if loaded.artifact.get("status") != "ok":
        return _fail(
            "load_failed",
            source_ref,
            deepcopy(loaded.artifact.get("diagnostics", [])),
        )

    doc = deepcopy(loaded.artifact["document"])

    top_level_failure = _require_top_level_sections(doc, source_ref)
    if top_level_failure is not None:
        return top_level_failure

    interface_failure, input_types, output_types = _validate_interface(doc["interface"], source_ref)
    if interface_failure is not None:
        return interface_failure

    front_panel = doc.get("front_panel", {"widgets": []})
    front_panel_failure, widget_by_id = _validate_front_panel(front_panel, source_ref)
    if front_panel_failure is not None:
        return front_panel_failure

    diagram_failure, node_map, edges = _collect_diagram_node_map(doc["diagram"], source_ref)
    if diagram_failure is not None:
        return diagram_failure

    incoming_failure, incoming = _build_incoming_edge_map(edges, source_ref)
    if incoming_failure is not None:
        return incoming_failure

    enrich_failure = _validate_and_enrich_nodes(
        node_map=node_map,
        edges=edges,
        incoming=incoming,
        widget_by_id=widget_by_id,
        input_types=input_types,
        output_types=output_types,
        source_ref=source_ref,
    )
    if enrich_failure is not None:
        return enrich_failure

    validated_nodes = [node_map[node["id"]] for node in doc["diagram"]["nodes"]]

    validated_document = deepcopy(doc)
    validated_document["diagram"]["nodes"] = validated_nodes
    validated_document["diagram"]["edges"] = edges

    validated_subset = {
        "core_primitives": True,
        "public_interface": True,
        "widget_value": any(node["kind"] == "widget_value" for node in validated_nodes),
        "widget_reference": any(node["kind"] == "widget_reference" for node in validated_nodes),
        "ui_object_primitives": any(
            node["kind"] == "primitive" and node.get("type") == "frog.ui.property_write"
            for node in validated_nodes
        ),
        "explicit_local_memory": any(
            node["kind"] == "primitive" and node.get("type") == "frog.core.delay"
            for node in validated_nodes
        ),
        "bounded_loop": any(
            node["kind"] == "structure" and node.get("structure_type") == "for_loop"
            for node in validated_nodes
        ),
        "minimal_u16_widget_family": all(
            widget.get("widget") in SUPPORTED_WIDGET_CLASSES and widget.get("value_type") == "u16"
            for widget in widget_by_id.values()
        ),
        "presentation_metadata_persisted_in_source": any(
            "face_template" in widget.get("props", {}) or "face_color" in widget.get("props", {})
            for widget in widget_by_id.values()
        ),
    }

    artifact = {
        "artifact_kind": "frog_validation_result",
        "artifact_version": DEMO_ARTIFACT_VERSION,
        "status": "ok",
        "source_ref": source_ref,
        "validated_subset": validated_subset,
        "validated_program": {
            "program_id": f"prog:{doc['metadata']['name']}",
            "entry_kind": "single_frog_program",
            "document": validated_document,
        },
        "diagnostics": [],
    }

    return ValidationResult(artifact=artifact)
