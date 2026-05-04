"""Rule-oriented FIR derivation helpers for the non-normative reference workspace.

This module supports the published examples 01 through 05 as explicit
source-to-FIR rules. It is intentionally narrow and does not claim general FROG
compiler completeness.
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

    def incoming(self, node_id: str, port: str) -> EdgeEndpoint:
        matches = []
        for edge in self.edges:
            dst = parse_endpoint(edge.get("to"))
            if dst.node == node_id and dst.port == port:
                matches.append(parse_endpoint(edge.get("from")))
        if len(matches) != 1:
            raise DerivationError(f"expected exactly one incoming edge to {node_id}.{port}, found {len(matches)}")
        return matches[0]

    def outgoing(self, node_id: str, port: str) -> EdgeEndpoint:
        matches = []
        for edge in self.edges:
            src = parse_endpoint(edge.get("from"))
            if src.node == node_id and src.port == port:
                matches.append(parse_endpoint(edge.get("to")))
        if len(matches) != 1:
            raise DerivationError(f"expected exactly one outgoing edge from {node_id}.{port}, found {len(matches)}")
        return matches[0]

    def outgoing_all(self, node_id: str, port: str) -> list[EdgeEndpoint]:
        matches = []
        for edge in self.edges:
            src = parse_endpoint(edge.get("from"))
            if src.node == node_id and src.port == port:
                matches.append(parse_endpoint(edge.get("to")))
        return matches

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
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise DerivationError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise DerivationError(f"invalid JSON in {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise DerivationError(f"{path} must contain a JSON object")
    return data


def dump_json(data: dict[str, Any], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def source_metadata(source: dict[str, Any]) -> dict[str, Any]:
    return require_object(source.get("metadata"), "source.metadata")


def source_interface(source: dict[str, Any]) -> tuple[list[dict[str, str]], list[dict[str, str]]]:
    interface = require_object(source.get("interface"), "source.interface")
    def ports(key: str) -> list[dict[str, str]]:
        out = []
        for index, port in enumerate(require_list(interface.get(key), f"source.interface.{key}")):
            obj = require_object(port, f"source.interface.{key}[{index}]")
            port_id, port_type = obj.get("id"), obj.get("type")
            if not isinstance(port_id, str) or not isinstance(port_type, str):
                raise DerivationError(f"source.interface.{key}[{index}] must contain string id and type")
            out.append({"id": port_id, "type": port_type})
        return out
    return ports("inputs"), ports("outputs")


def example_id_from_metadata(source: dict[str, Any]) -> str:
    raw = source_metadata(source).get("name")
    aliases = {
        "01_pure_addition": "01_pure_addition",
        "02_ui_value_roundtrip": "02_ui_value_roundtrip",
        "UI Property Write": "03_ui_property_write",
        "Stateful Feedback with Explicit Delay": "04_stateful_feedback_delay",
        "05_bounded_ui_accumulator": "05_bounded_ui_accumulator",
    }
    if raw not in aliases:
        raise DerivationError(f"unsupported source example for FIR derivation: {raw!r}")
    return aliases[str(raw)]


def interface_input_node(graph: SourceGraph, port: str) -> dict[str, Any]:
    matches = [n for n in graph.nodes.values() if n.get("kind") == "interface_input" and n.get("interface_port") == port]
    if len(matches) != 1:
        raise DerivationError(f"expected one interface_input for {port}, found {len(matches)}")
    return matches[0]


def interface_output_node(graph: SourceGraph, port: str) -> dict[str, Any]:
    matches = [n for n in graph.nodes.values() if n.get("kind") == "interface_output" and n.get("interface_port") == port]
    if len(matches) != 1:
        raise DerivationError(f"expected one interface_output for {port}, found {len(matches)}")
    return matches[0]


def widget_value_type(source: dict[str, Any], widget_id: str) -> str:
    widgets = require_list(require_object(source.get("front_panel"), "source.front_panel").get("widgets"), "source.front_panel.widgets")
    for widget in widgets:
        obj = require_object(widget, "source.front_panel.widgets[]")
        if obj.get("id") == widget_id:
            value_type = obj.get("value_type")
            if not isinstance(value_type, str):
                raise DerivationError(f"widget {widget_id} must have value_type")
            return value_type
    raise DerivationError(f"widget not found: {widget_id}")


def derive_example01(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    graph = SourceGraph.from_source(source)
    inputs, outputs = source_interface(source)
    if inputs != [{"id": "a", "type": "f64"}, {"id": "b", "type": "f64"}] or outputs != [{"id": "result", "type": "f64"}]:
        raise DerivationError("Example 01 expects a:f64, b:f64 -> result:f64")
    if graph.incoming("add_1", "a") != EdgeEndpoint(str(interface_input_node(graph, "a")["id"]), "value"):
        raise DerivationError("Example 01 invalid edge to add_1.a")
    if graph.incoming("add_1", "b") != EdgeEndpoint(str(interface_input_node(graph, "b")["id"]), "value"):
        raise DerivationError("Example 01 invalid edge to add_1.b")
    if graph.outgoing("add_1", "result") != EdgeEndpoint(str(interface_output_node(graph, "result")["id"]), "value"):
        raise DerivationError("Example 01 invalid publication edge")
    return artifacts_example01(source_rel)


def derive_example02(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    graph = SourceGraph.from_source(source)
    inputs, outputs = source_interface(source)
    if inputs or outputs:
        raise DerivationError("Example 02 expects no public IO")
    if graph.incoming("add_1", "a").node != "ctrl_a_value":
        raise DerivationError("Example 02 invalid edge to add_1.a")
    if graph.incoming("add_1", "b").node != "ctrl_b_value":
        raise DerivationError("Example 02 invalid edge to add_1.b")
    if graph.outgoing("add_1", "result").node != "ind_result_value":
        raise DerivationError("Example 02 invalid indicator publication edge")
    data = artifacts_example02(source_rel)
    data["units"][0]["ui_bindings"]["control_bindings"][0]["value_type"] = widget_value_type(source, "ctrl_a")
    data["units"][0]["ui_bindings"]["control_bindings"][1]["value_type"] = widget_value_type(source, "ctrl_b")
    data["units"][0]["ui_bindings"]["indicator_bindings"][0]["value_type"] = widget_value_type(source, "ind_result")
    return data


def derive_example03(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    graph = SourceGraph.from_source(source)
    inputs, outputs = source_interface(source)
    if inputs != [{"id": "status", "type": "string"}] or outputs:
        raise DerivationError("Example 03 expects status:string and no outputs")
    if graph.incoming("write_label_text", "value") != EdgeEndpoint(str(interface_input_node(graph, "status")["id"]), "value"):
        raise DerivationError("Example 03 invalid value edge")
    if graph.incoming("write_label_text", "ref") != EdgeEndpoint("ctrl_gain_ref", "ref"):
        raise DerivationError("Example 03 invalid ref edge")
    return artifacts_example03(source_rel)


def derive_example04(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    graph = SourceGraph.from_source(source)
    inputs, outputs = source_interface(source)
    if inputs != [{"id": "x", "type": "f64"}] or outputs != [{"id": "y", "type": "f64"}]:
        raise DerivationError("Example 04 expects x:f64 -> y:f64")
    delay = graph.nodes.get("delay_1")
    if not delay or delay.get("type") != "frog.core.delay" or delay.get("initial") != 0.0:
        raise DerivationError("Example 04 expects delay_1 frog.core.delay initial 0.0")
    if graph.incoming("add_1", "a").node != "input_x":
        raise DerivationError("Example 04 invalid input edge")
    if graph.incoming("add_1", "b") != EdgeEndpoint("delay_1", "out"):
        raise DerivationError("Example 04 invalid delay edge")
    outs = graph.outgoing_all("add_1", "result")
    if EdgeEndpoint("delay_1", "in") not in outs or EdgeEndpoint("output_y", "value") not in outs:
        raise DerivationError("Example 04 missing feedback/output edge")
    return artifacts_example04(source_rel)


def derive_example05(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    # This function preserves the existing published Example 05 shape.
    graph = SourceGraph.from_source(source)
    inputs, outputs = source_interface(source)
    loop = graph.nodes.get("loop_accumulate")
    count_node = require_object(loop.get("count_from"), "loop_accumulate.count_from").get("node") if isinstance(loop, dict) else None
    if not isinstance(count_node, str):
        raise DerivationError("Example 05 invalid loop count source")
    _, count = graph.constant_value(count_node)
    state_type, initial = graph.constant_value(graph.incoming("loop_accumulate", "loop_initial_state").node)
    writes = []
    for node in graph.nodes.values():
        if node.get("kind") == "primitive" and node.get("type") == "frog.ui.property_write":
            node_id = str(node["id"])
            ref_node = graph.nodes[graph.incoming(node_id, "ref").node]
            value_type, value_literal = graph.constant_value(graph.incoming(node_id, "value").node)
            writes.append({"widget_id": ref_node["widget"], "member": require_object(node.get("widget_member"), "widget_member")["member"], "value_type": value_type, "value_literal": value_literal})
    writes.sort(key=lambda x: {"ctrl_input": 0, "ind_result": 1}.get(x["widget_id"], 100))
    return {
        "artifact_kind": "frog_fir_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "05_bounded_ui_accumulator", "path": source_rel, "entry_unit": "main"},
        "front_panel_ref": {"package_path": (Path(source_rel).parent / "ui/accumulator_panel.wfrog").as_posix(), "panel_id": "main_panel"},
        "units": [{
            "unit_id": "main",
            "kind": "bounded_stateful_ui_unit",
            "public_interface": {"inputs": inputs, "outputs": outputs},
            "ui_bindings": {
                "control_bindings": [{"widget_id": "ctrl_input", "mode": "widget_value", "public_input_id": inputs[0]["id"]}],
                "indicator_bindings": [{"widget_id": "ind_result", "mode": "widget_value", "public_output_id": outputs[0]["id"]}],
                "reference_writes": writes
            },
            "state_model": {"explicit_state": True, "carrier": {"primitive": "frog.core.delay", "state_id": "accumulator_state", "type": state_type, "initial_value": initial}, "commit_rule": "state_next becomes state_current at the loop iteration commit point"},
            "execution_model": {"structure": "for_loop", "iteration_count": count, "iteration_variable": "i", "body_rule": {"kind": "accumulate_with_explicit_state", "expression": "state_next = state_current + input_value"}, "final_result_rule": "state_current after the final commit is the published result"},
            "publications": [{"target": f"public_output.{outputs[0]['id']}", "source": "state_current"}, {"target": "widget.ind_result.value", "source": "state_current"}],
            "notes": ["This FIR is execution-facing and remains downstream from canonical source.", "The widget package remains authoritative for panel realization, assets, and host binding details.", "The lowering published for Example 05 must remain a compatible projection of this FIR."]
        }]
    }


# Artifact factories for source_rel injection.
def artifacts_example01(source_rel: str) -> dict[str, Any]:
    return {
        "artifact_kind": "frog_fir_unit", "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "01_pure_addition", "path": source_rel, "entry_unit": "main"},
        "units": [{
            "unit_id": "main", "kind": "pure_dataflow_arithmetic_unit",
            "public_interface": {"inputs": [{"id": "a", "type": "f64"}, {"id": "b", "type": "f64"}], "outputs": [{"id": "result", "type": "f64"}]},
            "execution_model": {"structure": "acyclic_dataflow_graph", "primitive_operations": [{"node_id": "add_1", "primitive": "frog.core.add", "inputs": {"a": "public_input.a", "b": "public_input.b"}, "outputs": {"result": "add_1.result"}}], "edges": [{"from": "public_input.a", "to": "add_1.a"}, {"from": "public_input.b", "to": "add_1.b"}, {"from": "add_1.result", "to": "public_output.result"}]},
            "publications": [{"target": "public_output.result", "source": "add_1.result"}],
            "notes": ["This FIR is execution-facing and remains downstream from canonical source.", "This example has no front-panel participation, widget participation, structured control, or explicit local state.", "The published derivation preserves public-boundary participation and the frog.core.add primitive identity."]
        }]
    }


def artifacts_example02(source_rel: str) -> dict[str, Any]:
    return {
        "artifact_kind": "frog_fir_unit", "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "02_ui_value_roundtrip", "path": source_rel, "entry_unit": "main"},
        "front_panel_ref": {"kind": "inline_source_front_panel", "source_section": "front_panel"},
        "units": [{
            "unit_id": "main", "kind": "ui_value_roundtrip_unit", "public_interface": {"inputs": [], "outputs": []},
            "ui_bindings": {"control_bindings": [{"widget_id": "ctrl_a", "mode": "widget_value", "value_type": "f64"}, {"widget_id": "ctrl_b", "mode": "widget_value", "value_type": "f64"}], "indicator_bindings": [{"widget_id": "ind_result", "mode": "widget_value", "value_type": "f64"}]},
            "execution_model": {"structure": "acyclic_dataflow_graph", "primitive_operations": [{"node_id": "add_1", "primitive": "frog.core.add", "inputs": {"a": "widget.ctrl_a.value", "b": "widget.ctrl_b.value"}, "outputs": {"result": "add_1.result"}}], "edges": [{"from": "widget.ctrl_a.value", "to": "add_1.a"}, {"from": "widget.ctrl_b.value", "to": "add_1.b"}, {"from": "add_1.result", "to": "widget.ind_result.value"}]},
            "publications": [{"target": "widget.ind_result.value", "source": "add_1.result"}],
            "notes": ["This FIR preserves natural widget_value participation.", "This example has no public interface IO, object-style widget reference, structured control, or explicit local state."]
        }]
    }


def artifacts_example03(source_rel: str) -> dict[str, Any]:
    return {
        "artifact_kind": "frog_fir_unit", "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "03_ui_property_write", "path": source_rel, "entry_unit": "main"},
        "front_panel_ref": {"kind": "inline_source_front_panel", "source_section": "front_panel"},
        "units": [{
            "unit_id": "main", "kind": "ui_property_write_unit",
            "public_interface": {"inputs": [{"id": "status", "type": "string"}], "outputs": []},
            "ui_bindings": {"widget_reference_support": [{"widget_id": "ctrl_gain", "supported_members": ["label.text"]}], "reference_writes": [{"widget_id": "ctrl_gain", "member": "label.text", "value_type": "string", "value_source": "public_input.status"}]},
            "execution_model": {"structure": "single_ui_effect", "effects": [{"operation": "frog.ui.property_write", "widget_id": "ctrl_gain", "member": "label.text", "value_source": "public_input.status"}]},
            "publications": [],
            "notes": ["This FIR preserves object-style widget_reference participation.", "The property write is an explicit UI effect and does not redefine widget class law."]
        }]
    }


def artifacts_example04(source_rel: str) -> dict[str, Any]:
    return {
        "artifact_kind": "frog_fir_unit", "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "04_stateful_feedback_delay", "path": source_rel, "entry_unit": "main"},
        "units": [{
            "unit_id": "main", "kind": "stateful_feedback_delay_unit",
            "public_interface": {"inputs": [{"id": "x", "type": "f64"}], "outputs": [{"id": "y", "type": "f64"}]},
            "state_model": {"explicit_state": True, "carrier": {"primitive": "frog.core.delay", "state_id": "delay_1", "type": "f64", "initial_value": 0.0}, "commit_rule": "add_1.result becomes the next delayed state at the step commit point"},
            "execution_model": {"structure": "single_step_feedback_with_explicit_delay", "body_rule": {"kind": "add_input_to_delayed_state", "expression": "state_next = state_current + x"}, "edges": [{"from": "public_input.x", "to": "add_1.a"}, {"from": "state.delay_1.out", "to": "add_1.b"}, {"from": "add_1.result", "to": "state.delay_1.in"}, {"from": "add_1.result", "to": "public_output.y"}]},
            "publications": [{"target": "public_output.y", "source": "add_1.result"}],
            "notes": ["This FIR preserves explicit feedback through frog.core.delay.", "The feedback cycle is valid only because delay_1 provides an explicit initial value."]
        }]
    }


def derive_fir_from_source(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    example_id = example_id_from_metadata(source)
    if example_id == "01_pure_addition": return derive_example01(source, source_rel)
    if example_id == "02_ui_value_roundtrip": return derive_example02(source, source_rel)
    if example_id == "03_ui_property_write": return derive_example03(source, source_rel)
    if example_id == "04_stateful_feedback_delay": return derive_example04(source, source_rel)
    if example_id == "05_bounded_ui_accumulator": return derive_example05(source, source_rel)
    raise DerivationError(f"unsupported source example for FIR derivation: {example_id!r}")
