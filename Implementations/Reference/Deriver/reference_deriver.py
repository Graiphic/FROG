from __future__ import annotations

from copy import deepcopy
from typing import Any, Dict, List, Optional

from Implementations.Reference.common import (
    DEMO_ARTIFACT_VERSION,
    DerivedIR,
    FrogPipelineError,
    ValidationResult,
    ensure,
)


def _build_port(port_id: str, direction: str, value_type: str) -> Dict[str, Any]:
    return {
        "id": port_id,
        "direction": direction,
        "value_type": value_type,
    }


def _find_widget(widget_by_id: Dict[str, Dict[str, Any]], widget_id: str) -> Dict[str, Any]:
    widget = widget_by_id.get(widget_id)
    if widget is None:
        raise FrogPipelineError(
            stage="derive-ir",
            error_code="missing_widget_metadata",
            message=f"Missing widget metadata for '{widget_id}' during derivation.",
        )
    return widget


def _find_constant_value(node_id_to_node: Dict[str, Dict[str, Any]], node_id: str) -> Optional[Any]:
    node = node_id_to_node.get(node_id)
    if node is None:
        return None
    if node.get("kind") != "constant":
        return None
    return node.get("value")


def _normalize_numeric_default(raw: Any, value_type: str) -> Any:
    if value_type == "f64":
        return float(raw)
    if value_type in {"u16", "i32", "i64"}:
        return int(raw)
    return raw


def _build_region_node_map(region_nodes: List[Dict[str, Any]]) -> Dict[str, Dict[str, Any]]:
    return {node["id"]: node for node in region_nodes}


def _build_region_incoming(region_edges: List[Dict[str, Any]]) -> Dict[tuple[str, str], List[Dict[str, Any]]]:
    incoming: Dict[tuple[str, str], List[Dict[str, Any]]] = {}
    for edge in region_edges:
        key = (edge["to"]["node"], edge["to"]["port"])
        incoming.setdefault(key, []).append(edge)
    return incoming


def _find_region_constant_input_value(
    *,
    target_node_id: str,
    target_port: str,
    region_incoming: Dict[tuple[str, str], List[Dict[str, Any]]],
    region_node_map: Dict[str, Dict[str, Any]],
) -> Optional[Any]:
    candidates = region_incoming.get((target_node_id, target_port), [])
    if len(candidates) != 1:
        return None
    edge = candidates[0]
    source_node = region_node_map.get(edge["from"]["node"])
    if source_node is None:
        return None
    if source_node.get("kind") != "constant":
        return None
    return source_node.get("value")


def _resolve_top_level_node_output_type(
    *,
    node: Dict[str, Any],
    port: str,
    interface_input_types: Dict[str, str],
    interface_output_types: Dict[str, str],
    widget_by_id: Dict[str, Dict[str, Any]],
    structure_output_types: Dict[tuple[str, str], str],
) -> Optional[str]:
    kind = node["kind"]

    if kind == "constant":
        return node.get("type")

    if kind == "interface_input" and port == "value":
        return interface_input_types.get(node["interface_port"])

    if kind == "interface_output" and port == "value":
        return interface_output_types.get(node["interface_port"])

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


def _resolve_region_node_output_type(
    *,
    node: Dict[str, Any],
    port: str,
    boundary_input_types: Dict[str, str],
    boundary_output_types: Dict[str, str],
    structure_terminal_types: Dict[str, str],
) -> Optional[str]:
    kind = node["kind"]

    if kind == "boundary_input" and port == "value":
        return boundary_input_types.get(node["boundary_port"])

    if kind == "boundary_output" and port == "value":
        return boundary_output_types.get(node["boundary_port"])

    if kind == "structure_terminal" and port == "value":
        return structure_terminal_types.get(node["terminal"])

    if kind == "primitive":
        primitive_ref = node["type"]
        if primitive_ref == "frog.core.add" and port in {"a", "b", "result"}:
            return node.get("_resolved_value_type")
        if primitive_ref == "frog.core.delay" and port in {"in", "initial", "out"}:
            return node.get("_resolved_value_type")

    if kind == "constant":
        return node.get("type")

    return None


def _derive_for_loop_object(
    *,
    node: Dict[str, Any],
) -> Dict[str, Any]:
    node_id = node["id"]
    object_id = f"obj:{node_id}"

    boundary = node["boundary"]
    structure_terminals = node["structure_terminals"]
    regions = node["regions"]

    boundary_input_types = {item["id"]: item["type"] for item in boundary["inputs"]}
    boundary_output_types = {item["id"]: item["type"] for item in boundary["outputs"]}
    terminal_types = {name: terminal["type"] for name, terminal in structure_terminals.items()}

    body_region = next(region for region in regions if region["id"] == "body")
    body_diagram = body_region["diagram"]
    region_nodes = deepcopy(body_diagram["nodes"])
    region_edges = deepcopy(body_diagram["edges"])

    region_node_map = _build_region_node_map(region_nodes)
    region_incoming = _build_region_incoming(region_edges)

    iteration_count = int(node["_resolved_iteration_count"])
    value_type = node["_resolved_value_type"]

    region_objects: List[Dict[str, Any]] = []
    region_connections: List[Dict[str, Any]] = []

    for region_node in region_nodes:
        region_node_id = region_node["id"]
        region_object_id = f"obj:{node_id}:region:body:{region_node_id}"
        kind = region_node["kind"]

        if kind == "boundary_input":
            boundary_port = region_node["boundary_port"]
            boundary_type = boundary_input_types[boundary_port]
            region_objects.append(
                {
                    "id": region_object_id,
                    "kind": "structure_boundary_input",
                    "boundary_port": boundary_port,
                    "direction": "out",
                    "value_type": boundary_type,
                    "ports": [
                        _build_port("value", "out", boundary_type),
                    ],
                    "sources": [
                        f"diagram.node:{node_id}.region:body.node:{region_node_id}",
                    ],
                }
            )

        elif kind == "boundary_output":
            boundary_port = region_node["boundary_port"]
            boundary_type = boundary_output_types[boundary_port]
            region_objects.append(
                {
                    "id": region_object_id,
                    "kind": "structure_boundary_output",
                    "boundary_port": boundary_port,
                    "direction": "in",
                    "value_type": boundary_type,
                    "ports": [
                        _build_port("value", "in", boundary_type),
                    ],
                    "sources": [
                        f"diagram.node:{node_id}.region:body.node:{region_node_id}",
                    ],
                }
            )

        elif kind == "structure_terminal":
            terminal_name = region_node["terminal"]
            terminal_type = terminal_types[terminal_name]
            region_objects.append(
                {
                    "id": region_object_id,
                    "kind": "structure_terminal_projection",
                    "terminal": terminal_name,
                    "direction": "out",
                    "value_type": terminal_type,
                    "ports": [
                        _build_port("value", "out", terminal_type),
                    ],
                    "sources": [
                        f"diagram.node:{node_id}.region:body.node:{region_node_id}",
                    ],
                }
            )

        elif kind == "primitive":
            primitive_ref = region_node["type"]

            if primitive_ref == "frog.core.add":
                resolved_type = region_node.get("_resolved_value_type", value_type)
                region_objects.append(
                    {
                        "id": region_object_id,
                        "kind": "primitive",
                        "primitive_ref": primitive_ref,
                        "value_type": resolved_type,
                        "ports": [
                            _build_port("a", "in", resolved_type),
                            _build_port("b", "in", resolved_type),
                            _build_port("result", "out", resolved_type),
                        ],
                        "sources": [
                            f"diagram.node:{node_id}.region:body.node:{region_node_id}",
                            f"library.primitive:{primitive_ref}",
                        ],
                    }
                )

            elif primitive_ref == "frog.core.delay":
                resolved_type = region_node.get("_resolved_value_type", value_type)
                state_id = f"state:{node_id}:region:body:{region_node_id}"

                initial_value = region_node.get("_coerced_initial")
                if initial_value is None:
                    initial_value = _find_region_constant_input_value(
                        target_node_id=region_node_id,
                        target_port="initial",
                        region_incoming=region_incoming,
                        region_node_map=region_node_map,
                    )
                if initial_value is None and resolved_type == "u16":
                    initial_value = 0

                region_objects.append(
                    {
                        "id": region_object_id,
                        "kind": "explicit_local_memory_primitive",
                        "primitive_ref": primitive_ref,
                        "state_id": state_id,
                        "state_kind": "explicit_local_memory",
                        "value_type": resolved_type,
                        "initial": _normalize_numeric_default(initial_value, resolved_type) if initial_value is not None else None,
                        "ports": [
                            _build_port("in", "in", resolved_type),
                            _build_port("initial", "in", resolved_type),
                            _build_port("out", "out", resolved_type),
                        ],
                        "sources": [
                            f"diagram.node:{node_id}.region:body.node:{region_node_id}",
                            f"library.primitive:{primitive_ref}",
                        ],
                    }
                )
            else:
                raise FrogPipelineError(
                    stage="derive-ir",
                    error_code="unsupported_region_primitive",
                    message=f"Unsupported primitive inside for_loop body during derivation: {primitive_ref}",
                )

        elif kind == "constant":
            const_type = region_node["type"]
            region_objects.append(
                {
                    "id": region_object_id,
                    "kind": "constant",
                    "value_type": const_type,
                    "value": _normalize_numeric_default(region_node["value"], const_type),
                    "ports": [
                        _build_port("value", "out", const_type),
                    ],
                    "sources": [
                        f"diagram.node:{node_id}.region:body.node:{region_node_id}",
                    ],
                }
            )

        else:
            raise FrogPipelineError(
                stage="derive-ir",
                error_code="unsupported_region_node_kind",
                message=f"Unsupported region node kind during derivation: {kind}",
            )

    region_node_id_to_object_id = {
        region_node["id"]: f"obj:{node_id}:region:body:{region_node['id']}"
        for region_node in region_nodes
    }

    for region_edge in region_edges:
        region_connections.append(
            {
                "id": f"conn:{node_id}:region:body:{region_edge['id']}",
                "from": {
                    "object": region_node_id_to_object_id[region_edge["from"]["node"]],
                    "port": region_edge["from"]["port"],
                },
                "to": {
                    "object": region_node_id_to_object_id[region_edge["to"]["node"]],
                    "port": region_edge["to"]["port"],
                },
                "sources": [
                    f"diagram.node:{node_id}.region:body.edge:{region_edge['id']}",
                ],
            }
        )

    return {
        "id": object_id,
        "kind": "counted_loop_region",
        "structure_type": "for_loop",
        "iteration_count": iteration_count,
        "value_type": value_type,
        "boundary": deepcopy(boundary),
        "structure_terminals": deepcopy(structure_terminals),
        "ports": [
            _build_port("count", "in", "i64"),
            _build_port("input_value", "in", boundary_input_types["input_value"]),
            _build_port("initial_state", "in", boundary_input_types["initial_state"]),
            _build_port("final_value", "out", boundary_output_types["final_value"]),
        ],
        "region_units": [
            {
                "id": f"unit:{node_id}:body",
                "role": "structure_region_body",
                "region_id": "body",
                "objects": region_objects,
                "connections": region_connections,
            }
        ],
        "uses_explicit_local_memory": any(
            obj["kind"] == "explicit_local_memory_primitive"
            for obj in region_objects
        ),
        "sources": [f"diagram.node:{node_id}"],
    }


def derive_execution_ir(validation: ValidationResult) -> DerivedIR:
    ensure(
        validation.artifact.get("status") == "ok",
        stage="derive-ir",
        error_code="validation_not_ok",
        message="Derivation requires a successful validation result.",
    )

    validated_program = validation.artifact["validated_program"]
    doc = validated_program["document"]
    interface = doc["interface"]
    diagram = doc["diagram"]
    front_panel = doc.get("front_panel", {})

    input_types = {item["id"]: item["type"] for item in interface["inputs"]}
    output_types = {item["id"]: item["type"] for item in interface["outputs"]}
    widget_by_id = {item["id"]: item for item in front_panel.get("widgets", [])}
    node_id_to_node = {node["id"]: node for node in diagram["nodes"]}

    structure_output_types: Dict[tuple[str, str], str] = {}
    for node in diagram["nodes"]:
        if node.get("kind") == "structure" and node.get("structure_type") == "for_loop":
            for output_item in node["boundary"]["outputs"]:
                structure_output_types[(node["id"], output_item["id"])] = output_item["type"]

    objects: List[Dict[str, Any]] = []
    connections: List[Dict[str, Any]] = []

    for node in diagram["nodes"]:
        kind = node["kind"]
        node_id = node["id"]
        object_id = f"obj:{node_id}"

        if kind == "interface_input":
            port_id = node["interface_port"]
            objects.append(
                {
                    "id": object_id,
                    "kind": "public_input_boundary",
                    "interface_port": port_id,
                    "direction": "out",
                    "value_type": input_types[port_id],
                    "ports": [
                        _build_port("value", "out", input_types[port_id]),
                    ],
                    "sources": [f"diagram.node:{node_id}"],
                }
            )

        elif kind == "interface_output":
            port_id = node["interface_port"]
            objects.append(
                {
                    "id": object_id,
                    "kind": "public_output_boundary",
                    "interface_port": port_id,
                    "direction": "in",
                    "value_type": output_types[port_id],
                    "ports": [
                        _build_port("value", "in", output_types[port_id]),
                    ],
                    "sources": [f"diagram.node:{node_id}"],
                }
            )

        elif kind == "constant":
            value_type = node["type"]
            value = node["value"]
            objects.append(
                {
                    "id": object_id,
                    "kind": "constant",
                    "value_type": value_type,
                    "value": _normalize_numeric_default(value, value_type),
                    "ports": [
                        _build_port("value", "out", value_type),
                    ],
                    "sources": [f"diagram.node:{node_id}"],
                }
            )

        elif kind == "widget_value":
            widget_id = node["widget"]
            widget = _find_widget(widget_by_id, widget_id)

            direction = "out" if widget["role"] == "control" else "in"
            obj: Dict[str, Any] = {
                "id": object_id,
                "kind": "widget_value_participation",
                "widget_id": widget_id,
                "widget_role": widget["role"],
                "widget_class": widget["widget"],
                "participation_kind": "widget_value",
                "direction": direction,
                "value_type": widget["value_type"],
                "ports": [
                    _build_port("value", direction, widget["value_type"]),
                ],
                "persisted_presentation": deepcopy(widget.get("props", {})),
                "sources": [f"diagram.node:{node_id}", f"front_panel.widget:{widget_id}"],
            }

            default_value = widget.get("props", {}).get("default_value")
            if widget["role"] == "control" and default_value is not None:
                obj["default_value"] = _normalize_numeric_default(default_value, widget["value_type"])

            objects.append(obj)

        elif kind == "widget_reference":
            widget_id = node["widget"]
            widget = _find_widget(widget_by_id, widget_id)

            objects.append(
                {
                    "id": object_id,
                    "kind": "widget_reference_participation",
                    "widget_id": widget_id,
                    "widget_role": widget["role"],
                    "widget_class": widget["widget"],
                    "participation_kind": "widget_reference",
                    "ports": [
                        _build_port("ref", "out", "widget_reference"),
                    ],
                    "persisted_presentation": deepcopy(widget.get("props", {})),
                    "sources": [f"diagram.node:{node_id}", f"front_panel.widget:{widget_id}"],
                }
            )

        elif kind == "primitive":
            primitive_ref = node["type"]

            if primitive_ref == "frog.core.add":
                value_type = node.get("_resolved_value_type", node.get("value_type", "u16"))
                objects.append(
                    {
                        "id": object_id,
                        "kind": "primitive",
                        "primitive_ref": primitive_ref,
                        "value_type": value_type,
                        "ports": [
                            _build_port("a", "in", value_type),
                            _build_port("b", "in", value_type),
                            _build_port("result", "out", value_type),
                        ],
                        "sources": [f"diagram.node:{node_id}", f"library.primitive:{primitive_ref}"],
                    }
                )

            elif primitive_ref == "frog.core.delay":
                value_type = node.get("_resolved_value_type", node.get("value_type", "u16"))
                state_id = f"state:{node_id}"

                initial_value = node.get("_coerced_initial")
                if initial_value is None and "initial" in node:
                    initial_value = node["initial"]

                objects.append(
                    {
                        "id": object_id,
                        "kind": "explicit_local_memory_primitive",
                        "primitive_ref": primitive_ref,
                        "state_id": state_id,
                        "state_kind": "explicit_local_memory",
                        "value_type": value_type,
                        "initial": _normalize_numeric_default(initial_value, value_type) if initial_value is not None else None,
                        "ports": [
                            _build_port("in", "in", value_type),
                            _build_port("initial", "in", value_type),
                            _build_port("out", "out", value_type),
                        ],
                        "sources": [f"diagram.node:{node_id}", f"library.primitive:{primitive_ref}"],
                    }
                )

            elif primitive_ref == "frog.ui.property_write":
                member = deepcopy(node["widget_member"])
                value_type = node.get("_resolved_value_type")
                if value_type is None:
                    raise FrogPipelineError(
                        stage="derive-ir",
                        error_code="missing_property_write_value_type",
                        message="frog.ui.property_write requires a resolved value type.",
                    )

                objects.append(
                    {
                        "id": object_id,
                        "kind": "primitive",
                        "primitive_ref": primitive_ref,
                        "ui_operation_kind": "property_write",
                        "widget_member": member,
                        "target_widget_id": node.get("_resolved_widget_id"),
                        "target_widget_class": node.get("_resolved_widget_class"),
                        "ports": [
                            _build_port("ref", "in", "widget_reference"),
                            _build_port("value", "in", value_type),
                        ],
                        "sources": [f"diagram.node:{node_id}", f"library.primitive:{primitive_ref}"],
                    }
                )

            else:
                raise FrogPipelineError(
                    stage="derive-ir",
                    error_code="unsupported_primitive",
                    message=f"Unsupported primitive during derivation: {primitive_ref}",
                )

        elif kind == "structure":
            structure_type = node.get("structure_type")
            if structure_type != "for_loop":
                raise FrogPipelineError(
                    stage="derive-ir",
                    error_code="unsupported_structure_type",
                    message=f"Unsupported structure during derivation: {structure_type}",
                )
            objects.append(_derive_for_loop_object(node=node))

        else:
            raise FrogPipelineError(
                stage="derive-ir",
                error_code="unsupported_node_kind",
                message=f"Unsupported node kind during derivation: {kind}",
            )

    node_id_to_object_id = {node["id"]: f"obj:{node['id']}" for node in diagram["nodes"]}

    for edge in diagram["edges"]:
        connections.append(
            {
                "id": f"conn:{edge['id']}",
                "from": {
                    "object": node_id_to_object_id[edge["from"]["node"]],
                    "port": edge["from"]["port"],
                },
                "to": {
                    "object": node_id_to_object_id[edge["to"]["node"]],
                    "port": edge["to"]["port"],
                },
                "sources": [f"diagram.edge:{edge['id']}"],
            }
        )

    artifact = {
        "artifact_kind": "frog_derived_execution_ir",
        "artifact_version": DEMO_ARTIFACT_VERSION,
        "source_ref": dict(validation.artifact["source_ref"]),
        "program_id": validated_program["program_id"],
        "execution_unit": {
            "id": "unit:main",
            "role": "entry_unit",
            "objects": objects,
            "connections": connections,
            "ui_declarations": {
                "widgets": [deepcopy(widget) for widget in front_panel.get("widgets", [])],
            },
            "non_semantic_ui_metadata": {
                "persisted_widgets": [
                    {
                        "widget_id": widget["id"],
                        "props": deepcopy(widget.get("props", {})),
                    }
                    for widget in front_panel.get("widgets", [])
                ]
            },
        },
        "diagnostics": [],
    }

    return DerivedIR(artifact=artifact)
