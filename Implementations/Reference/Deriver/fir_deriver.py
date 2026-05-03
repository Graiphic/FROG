"""Rule-oriented FIR derivation helpers for the non-normative reference workspace.

This module is still intentionally narrow. It supports the frozen Example 05
corridor, but it separates reusable source-graph helpers and FIR projection
rules from the CLI wrapper.

The goal is to move from a one-off script toward a small staged deriver without
claiming general FROG compiler completeness.
"""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any


class DerivationError(RuntimeError):
    """Raised when a supported FIR derivation cannot proceed."""


@dataclass(frozen=True)
class EdgeEndpoint:
    node: str
    port: str


@dataclass(frozen=True)
class SourceGraph:
    source: dict[str, Any]
    nodes: dict[str, dict[str, Any]]
    edges: list[dict[str, Any]]

    @classmethod
    def from_source(cls, source: dict[str, Any]) -> "SourceGraph":
        diagram = require_object(source.get("diagram"), "source.diagram")
        nodes_raw = require_list(diagram.get("nodes"), "source.diagram.nodes")
        edges = require_list(diagram.get("edges"), "source.diagram.edges")

        nodes: dict[str, dict[str, Any]] = {}
        for index, node in enumerate(nodes_raw):
            obj = require_object(node, f"source.diagram.nodes[{index}]")
            node_id = obj.get("id")
            if not isinstance(node_id, str):
                raise DerivationError(f"source.diagram.nodes[{index}] must have string id")
            if node_id in nodes:
                raise DerivationError(f"duplicate node id: {node_id}")
            nodes[node_id] = obj

        for index, edge in enumerate(edges):
            require_object(edge, f"source.diagram.edges[{index}]")

        return cls(source=source, nodes=nodes, edges=edges)

    def find_node_by_kind(self, kind: str, *, widget: str | None = None) -> dict[str, Any]:
        matches = []
        for node in self.nodes.values():
            if node.get("kind") == kind and (widget is None or node.get("widget") == widget):
                matches.append(node)
        if len(matches) != 1:
            extra = f" for widget {widget}" if widget else ""
            raise DerivationError(f"expected exactly one node kind {kind}{extra}, found {len(matches)}")
        return matches[0]

    def incoming(self, node_id: str, port: str) -> EdgeEndpoint:
        matches: list[EdgeEndpoint] = []
        for edge in self.edges:
            dst = parse_endpoint(edge.get("to"))
            if dst.node == node_id and dst.port == port:
                matches.append(parse_endpoint(edge.get("from")))
        if len(matches) != 1:
            raise DerivationError(f"expected exactly one incoming edge to {node_id}.{port}, found {len(matches)}")
        return matches[0]

    def constant_value(self, node_id: str) -> tuple[str, Any]:
        node = self.nodes.get(node_id)
        if not node:
            raise DerivationError(f"unknown constant node: {node_id}")
        if node.get("kind") != "constant":
            raise DerivationError(f"node {node_id} is not a constant")
        value_type = node.get("type")
        if not isinstance(value_type, str):
            raise DerivationError(f"constant {node_id} has no string type")
        return value_type, node.get("value")


def require_object(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise DerivationError(f"{name} must be an object")
    return value


def require_list(value: Any, name: str) -> list[Any]:
    if not isinstance(value, list):
        raise DerivationError(f"{name} must be an array")
    return value


def parse_endpoint(raw: Any) -> EdgeEndpoint:
    obj = require_object(raw, "edge endpoint")
    node = obj.get("node")
    port = obj.get("port")
    if not isinstance(node, str) or not isinstance(port, str):
        raise DerivationError("edge endpoint must contain node and port strings")
    return EdgeEndpoint(node=node, port=port)


def load_json(path: Path) -> dict[str, Any]:
    try:
        with path.open("r", encoding="utf-8") as handle:
            data = json.load(handle)
    except FileNotFoundError as exc:
        raise DerivationError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise DerivationError(f"invalid JSON in {path}: {exc}") from exc

    if not isinstance(data, dict):
        raise DerivationError(f"{path} must contain a JSON object")
    return data


def dump_json(data: dict[str, Any], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="\n") as handle:
        json.dump(data, handle, indent=2)
        handle.write("\n")


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def source_metadata(source: dict[str, Any]) -> dict[str, Any]:
    return require_object(source.get("metadata"), "source.metadata")


def source_interface(source: dict[str, Any]) -> tuple[list[dict[str, str]], list[dict[str, str]]]:
    interface = require_object(source.get("interface"), "source.interface")

    def normalize_ports(key: str) -> list[dict[str, str]]:
        ports = require_list(interface.get(key), f"source.interface.{key}")
        out: list[dict[str, str]] = []
        for index, port in enumerate(ports):
            obj = require_object(port, f"source.interface.{key}[{index}]")
            port_id = obj.get("id")
            port_type = obj.get("type")
            if not isinstance(port_id, str) or not isinstance(port_type, str):
                raise DerivationError(f"source.interface.{key}[{index}] must contain string id and type")
            out.append({"id": port_id, "type": port_type})
        return out

    return normalize_ports("inputs"), normalize_ports("outputs")


def front_panel_package_path(source: dict[str, Any], source_rel: str) -> str:
    front_panel = require_object(source.get("front_panel"), "source.front_panel")
    package_refs = require_list(front_panel.get("package_refs"), "source.front_panel.package_refs")
    if not package_refs or not isinstance(package_refs[0], str):
        raise DerivationError("source.front_panel.package_refs must contain at least one package path string")
    return (Path(source_rel).parent / package_refs[0]).as_posix()


def derive_reference_writes(graph: SourceGraph) -> list[dict[str, Any]]:
    writes: list[dict[str, Any]] = []

    for node in graph.nodes.values():
        if node.get("kind") != "primitive" or node.get("type") != "frog.ui.property_write":
            continue

        node_id = node.get("id")
        if not isinstance(node_id, str):
            raise DerivationError("property_write node without id")

        widget_member = require_object(node.get("widget_member"), f"{node_id}.widget_member")
        member = widget_member.get("member")
        if not isinstance(member, str):
            raise DerivationError(f"{node_id}.widget_member.member must be a string")

        ref_src = graph.incoming(node_id, "ref")
        value_src = graph.incoming(node_id, "value")

        ref_node = graph.nodes.get(ref_src.node)
        if not ref_node or ref_node.get("kind") != "widget_reference":
            raise DerivationError(f"{node_id}.ref must be fed by a widget_reference node")

        widget_id = ref_node.get("widget")
        if not isinstance(widget_id, str):
            raise DerivationError(f"widget_reference node {ref_src.node} must carry widget id")

        value_type, value_literal = graph.constant_value(value_src.node)

        writes.append(
            {
                "widget_id": widget_id,
                "member": member,
                "value_type": value_type,
                "value_literal": value_literal,
            }
        )

    # Keep deterministic output for the frozen Example 05 publication.
    preferred_order = {"ctrl_input": 0, "ind_result": 1}
    writes.sort(key=lambda item: preferred_order.get(item["widget_id"], 100))
    return writes


def derive_loop_model(graph: SourceGraph) -> tuple[int, str, int, str]:
    loop = graph.nodes.get("loop_accumulate")
    if not loop or loop.get("kind") != "for_loop":
        raise DerivationError("Example 05 requires a for_loop node named loop_accumulate")

    count_from = require_object(loop.get("count_from"), "loop_accumulate.count_from")
    count_node = count_from.get("node")
    if not isinstance(count_node, str):
        raise DerivationError("loop_accumulate.count_from.node must be a string")

    _, count_value = graph.constant_value(count_node)
    if not isinstance(count_value, int):
        raise DerivationError("loop_accumulate count must be an integer")

    initial_edge = graph.incoming("loop_accumulate", "loop_initial_state")
    state_type, initial_state = graph.constant_value(initial_edge.node)
    if not isinstance(initial_state, int):
        raise DerivationError("loop initial state must be an integer")

    # Current supported source pattern.
    expression = "state_next = state_current + input_value"
    return count_value, state_type, initial_state, expression


def derive_example05_fir(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    metadata = source_metadata(source)
    example_id = metadata.get("name")
    if example_id != "05_bounded_ui_accumulator":
        raise DerivationError("this deriver currently supports only 05_bounded_ui_accumulator")

    graph = SourceGraph.from_source(source)
    inputs, outputs = source_interface(source)

    if len(inputs) != 1 or len(outputs) != 1:
        raise DerivationError("Example 05 expects one public input and one public output")

    ctrl_widget_node = graph.find_node_by_kind("widget_value", widget="ctrl_input")
    ind_widget_node = graph.find_node_by_kind("widget_value", widget="ind_result")

    control_widget_id = ctrl_widget_node.get("widget")
    indicator_widget_id = ind_widget_node.get("widget")
    if not isinstance(control_widget_id, str) or not isinstance(indicator_widget_id, str):
        raise DerivationError("widget_value nodes must carry widget identifiers")

    iteration_count, state_type, initial_state, expression = derive_loop_model(graph)
    reference_writes = derive_reference_writes(graph)

    return {
        "artifact_kind": "frog_fir_unit",
        "artifact_governance_ref": {
            "path": "Versioning/Readme.md"
        },
        "source_ref": {
            "example_id": example_id,
            "path": source_rel,
            "entry_unit": "main"
        },
        "front_panel_ref": {
            "package_path": front_panel_package_path(source, source_rel),
            "panel_id": "main_panel"
        },
        "units": [
            {
                "unit_id": "main",
                "kind": "bounded_stateful_ui_unit",
                "public_interface": {
                    "inputs": inputs,
                    "outputs": outputs
                },
                "ui_bindings": {
                    "control_bindings": [
                        {
                            "widget_id": control_widget_id,
                            "mode": "widget_value",
                            "public_input_id": inputs[0]["id"]
                        }
                    ],
                    "indicator_bindings": [
                        {
                            "widget_id": indicator_widget_id,
                            "mode": "widget_value",
                            "public_output_id": outputs[0]["id"]
                        }
                    ],
                    "reference_writes": reference_writes
                },
                "state_model": {
                    "explicit_state": True,
                    "carrier": {
                        "primitive": "frog.core.delay",
                        "state_id": "accumulator_state",
                        "type": state_type,
                        "initial_value": initial_state
                    },
                    "commit_rule": "state_next becomes state_current at the loop iteration commit point"
                },
                "execution_model": {
                    "structure": "for_loop",
                    "iteration_count": iteration_count,
                    "iteration_variable": "i",
                    "body_rule": {
                        "kind": "accumulate_with_explicit_state",
                        "expression": expression
                    },
                    "final_result_rule": "state_current after the final commit is the published result"
                },
                "publications": [
                    {
                        "target": f"public_output.{outputs[0]['id']}",
                        "source": "state_current"
                    },
                    {
                        "target": f"widget.{indicator_widget_id}.value",
                        "source": "state_current"
                    }
                ],
                "notes": [
                    "This FIR is execution-facing and remains downstream from canonical source.",
                    "The widget package remains authoritative for panel realization, assets, and host binding details.",
                    "The lowering published for Example 05 must remain a compatible projection of this FIR."
                ]
            }
        ]
    }


def derive_fir_from_source(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    """Dispatch FIR derivation for the currently supported reference examples."""
    metadata = source_metadata(source)
    example_id = metadata.get("name")

    if example_id == "05_bounded_ui_accumulator":
        return derive_example05_fir(source, source_rel)

    raise DerivationError(f"unsupported source example for FIR derivation: {example_id!r}")
