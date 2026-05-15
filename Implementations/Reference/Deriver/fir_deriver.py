"""Rule-oriented FIR derivation helpers for the non-normative reference workspace.

This module supports the published examples 01 through 10 through explicit
source-pattern recognition and source-to-FIR derivation rules.

It is intentionally narrow and does not claim general FROG compiler
completeness. The important boundary is that rule selection is based on the
validated source shape being recognized, not on trusting a document-local
example name as semantic authority.
"""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable


class DerivationError(RuntimeError):
    """Raised when a supported FIR derivation cannot proceed."""


@dataclass(frozen=True)
class EdgeEndpoint:
    node: str
    port: str


@dataclass(frozen=True)
class DerivationRule:
    """Bounded reference derivation rule for one supported source pattern."""

    rule_id: str
    fir_example_id: str
    derive: Callable[[dict[str, Any], str], dict[str, Any]]


@dataclass(frozen=True)
class RuleAttempt:
    rule_id: str
    fir_example_id: str
    result: dict[str, Any] | None
    error: str | None


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
        node = require_node(self, node_id, "constant")
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
            normalized = {"id": port_id, "type": port_type}
            enum_domain = obj.get("enum_domain")
            if isinstance(enum_domain, str):
                normalized["enum_domain"] = enum_domain
            out.append(normalized)
        return out

    return ports("inputs"), ports("outputs")


def assert_interface(
    source: dict[str, Any],
    expected_inputs: list[dict[str, str]],
    expected_outputs: list[dict[str, str]],
    context: str,
) -> tuple[list[dict[str, str]], list[dict[str, str]]]:
    inputs, outputs = source_interface(source)
    if inputs != expected_inputs or outputs != expected_outputs:
        raise DerivationError(f"{context} expects inputs={expected_inputs!r}, outputs={expected_outputs!r}")
    return inputs, outputs


def metadata_name_for_diagnostics(source: dict[str, Any]) -> str:
    try:
        name = source_metadata(source).get("name")
    except DerivationError:
        return "<missing metadata.name>"
    return str(name) if isinstance(name, str) else repr(name)


def require_node(graph: SourceGraph, node_id: str, context: str) -> dict[str, Any]:
    node = graph.nodes.get(node_id)
    if not isinstance(node, dict):
        raise DerivationError(f"{context} expected node {node_id!r}")
    return node


def require_primitive(graph: SourceGraph, node_id: str, primitive_type: str, context: str) -> dict[str, Any]:
    node = require_node(graph, node_id, context)
    if node.get("kind") != "primitive" or node.get("type") != primitive_type:
        raise DerivationError(f"{context} expected {node_id} to be primitive {primitive_type}")
    return node


def require_constant(
    graph: SourceGraph,
    node_id: str,
    context: str,
    *,
    expected_type: str | None = None,
    expected_value: Any | None = None,
) -> tuple[str, Any]:
    value_type, value = graph.constant_value(node_id)
    if expected_type is not None and value_type != expected_type:
        raise DerivationError(f"{context} expected constant {node_id} type {expected_type}, got {value_type}")
    if expected_value is not None and value != expected_value:
        raise DerivationError(f"{context} expected constant {node_id} value {expected_value!r}, got {value!r}")
    return value_type, value


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


def property_write_nodes(graph: SourceGraph) -> list[dict[str, Any]]:
    return [
        node
        for node in graph.nodes.values()
        if node.get("kind") == "primitive" and node.get("type") == "frog.ui.property_write"
    ]


def widget_ref_target(graph: SourceGraph, ref_node_id: str, context: str) -> str:
    ref_node = require_node(graph, ref_node_id, context)
    widget_id = ref_node.get("widget")
    if not isinstance(widget_id, str):
        raise DerivationError(f"{context} expected widget reference node {ref_node_id} to expose string widget")
    return widget_id


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


def front_panel_widget(source: dict[str, Any], widget_id: str) -> dict[str, Any]:
    widgets = require_list(require_object(source.get("front_panel"), "source.front_panel").get("widgets"), "source.front_panel.widgets")
    for widget in widgets:
        obj = require_object(widget, "source.front_panel.widgets[]")
        if obj.get("id") == widget_id:
            return obj
    raise DerivationError(f"widget not found: {widget_id}")


def require_widget_class(source: dict[str, Any], widget_id: str, class_ref: str, context: str) -> dict[str, Any]:
    widget = front_panel_widget(source, widget_id)
    if widget.get("class_ref") != class_ref:
        raise DerivationError(f"{context} expected widget {widget_id} class_ref {class_ref}")
    return widget


def derive_example01(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "pure addition pattern"
    graph = SourceGraph.from_source(source)
    assert_interface(
        source,
        [{"id": "a", "type": "f64"}, {"id": "b", "type": "f64"}],
        [{"id": "result", "type": "f64"}],
        context,
    )
    require_primitive(graph, "add_1", "frog.core.add", context)
    if graph.incoming("add_1", "a") != EdgeEndpoint(str(interface_input_node(graph, "a")["id"]), "value"):
        raise DerivationError(f"{context} has invalid edge to add_1.a")
    if graph.incoming("add_1", "b") != EdgeEndpoint(str(interface_input_node(graph, "b")["id"]), "value"):
        raise DerivationError(f"{context} has invalid edge to add_1.b")
    if graph.outgoing("add_1", "result") != EdgeEndpoint(str(interface_output_node(graph, "result")["id"]), "value"):
        raise DerivationError(f"{context} has invalid publication edge")
    return artifacts_example01(source_rel)


def derive_example02(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "UI value roundtrip pattern"
    graph = SourceGraph.from_source(source)
    assert_interface(source, [], [], context)
    require_primitive(graph, "add_1", "frog.core.add", context)
    if graph.incoming("add_1", "a").node != "ctrl_a_value":
        raise DerivationError(f"{context} has invalid edge to add_1.a")
    if graph.incoming("add_1", "b").node != "ctrl_b_value":
        raise DerivationError(f"{context} has invalid edge to add_1.b")
    if graph.outgoing("add_1", "result").node != "ind_result_value":
        raise DerivationError(f"{context} has invalid indicator publication edge")
    data = artifacts_example02(source_rel)
    data["units"][0]["ui_bindings"]["control_bindings"][0]["value_type"] = widget_value_type(source, "ctrl_a")
    data["units"][0]["ui_bindings"]["control_bindings"][1]["value_type"] = widget_value_type(source, "ctrl_b")
    data["units"][0]["ui_bindings"]["indicator_bindings"][0]["value_type"] = widget_value_type(source, "ind_result")
    return data


def derive_example03(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "UI property-write pattern"
    graph = SourceGraph.from_source(source)
    assert_interface(source, [{"id": "status", "type": "string"}], [], context)
    require_primitive(graph, "write_label_text", "frog.ui.property_write", context)
    if graph.incoming("write_label_text", "value") != EdgeEndpoint(str(interface_input_node(graph, "status")["id"]), "value"):
        raise DerivationError(f"{context} has invalid value edge")
    if graph.incoming("write_label_text", "ref") != EdgeEndpoint("ctrl_gain_ref", "ref"):
        raise DerivationError(f"{context} has invalid ref edge")
    widget_ref_target(graph, "ctrl_gain_ref", context)
    return artifacts_example03(source_rel)


def derive_example04(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "stateful feedback delay pattern"
    graph = SourceGraph.from_source(source)
    assert_interface(source, [{"id": "x", "type": "f64"}], [{"id": "y", "type": "f64"}], context)
    require_primitive(graph, "add_1", "frog.core.add", context)
    delay = require_node(graph, "delay_1", context)
    if delay.get("type") != "frog.core.delay" or delay.get("initial") != 0.0:
        raise DerivationError(f"{context} expects delay_1 frog.core.delay initial 0.0")
    if graph.incoming("add_1", "a").node != "input_x":
        raise DerivationError(f"{context} has invalid input edge")
    if graph.incoming("add_1", "b") != EdgeEndpoint("delay_1", "out"):
        raise DerivationError(f"{context} has invalid delay edge")
    outs = graph.outgoing_all("add_1", "result")
    if EdgeEndpoint("delay_1", "in") not in outs or EdgeEndpoint("output_y", "value") not in outs:
        raise DerivationError(f"{context} is missing feedback/output edge")
    return artifacts_example04(source_rel)


def derive_example05(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "bounded UI accumulator pattern"
    graph = SourceGraph.from_source(source)
    inputs, outputs = assert_interface(
        source,
        [{"id": "input_value", "type": "u16"}],
        [{"id": "result", "type": "u16"}],
        context,
    )
    loop = require_node(graph, "loop_accumulate", context)
    count_node = require_object(loop.get("count_from"), "loop_accumulate.count_from").get("node")
    if not isinstance(count_node, str):
        raise DerivationError(f"{context} has invalid loop count source")
    _, count = graph.constant_value(count_node)
    state_type, initial = require_constant(
        graph,
        graph.incoming("loop_accumulate", "loop_initial_state").node,
        context,
        expected_type="u16",
        expected_value=0,
    )
    writes = []
    for node in property_write_nodes(graph):
        node_id = str(node["id"])
        ref_endpoint = graph.incoming(node_id, "ref")
        widget_id = widget_ref_target(graph, ref_endpoint.node, context)
        value_type, value_literal = graph.constant_value(graph.incoming(node_id, "value").node)
        writes.append({"widget_id": widget_id, "member": require_object(node.get("widget_member"), "widget_member")["member"], "value_type": value_type, "value_literal": value_literal})
    writes.sort(key=lambda x: {"ctrl_input": 0, "ind_result": 1}.get(x["widget_id"], 100))
    if [write["widget_id"] for write in writes] != ["ctrl_input", "ind_result"]:
        raise DerivationError(f"{context} expects property writes for ctrl_input and ind_result")
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


def derive_example06(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "Boolean value roundtrip pattern"
    metadata = source_metadata(source)
    if metadata.get("name") != "06_boolean_value_roundtrip":
        raise DerivationError(f"{context} expects metadata.name=06_boolean_value_roundtrip")
    inputs, outputs = assert_interface(
        source,
        [{"id": "input_value", "type": "bool"}],
        [{"id": "result", "type": "bool"}],
        context,
    )
    require_widget_class(source, "bool_input", "frog.widgets.boolean_control", context)
    require_widget_class(source, "bool_result", "frog.widgets.boolean_indicator", context)
    graph = SourceGraph.from_source(source)
    require_node(graph, "bool_input_value", context)
    require_node(graph, "bool_result_value", context)
    require_node(graph, "output_result", context)
    if graph.nodes["bool_input_value"].get("kind") != "widget_value" or graph.nodes["bool_input_value"].get("widget") != "bool_input":
        raise DerivationError(f"{context} expects bool_input_value widget_value for bool_input")
    if graph.nodes["bool_result_value"].get("kind") != "widget_value" or graph.nodes["bool_result_value"].get("widget") != "bool_result":
        raise DerivationError(f"{context} expects bool_result_value widget_value for bool_result")
    if graph.nodes["output_result"].get("kind") != "interface_output" or graph.nodes["output_result"].get("interface_port") != "result":
        raise DerivationError(f"{context} expects output_result interface output result")
    outs = graph.outgoing_all("bool_input_value", "value")
    if sorted(outs, key=lambda endpoint: endpoint.node) != [
        EdgeEndpoint("bool_result_value", "value"),
        EdgeEndpoint("output_result", "value"),
    ]:
        raise DerivationError(f"{context} expects bool_input_value.value to feed result widget and public output")
    return {
        "artifact_kind": "frog_fir_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "06_boolean_value_roundtrip", "path": source_rel, "entry_unit": "main"},
        "front_panel_ref": {"package_path": (Path(source_rel).parent / "ui/boolean_panel.wfrog").as_posix(), "panel_id": "main_panel"},
        "units": [{
            "unit_id": "main",
            "kind": "boolean_value_roundtrip_ui_unit",
            "public_interface": {"inputs": inputs, "outputs": outputs},
            "ui_bindings": {
                "control_bindings": [{"widget_id": "bool_input", "mode": "widget_value", "public_input_id": "input_value", "value_type": "bool"}],
                "indicator_bindings": [{"widget_id": "bool_result", "mode": "widget_value", "public_output_id": "result", "value_type": "bool"}],
            },
            "execution_model": {"structure": "single_value_copy", "body_rule": {"kind": "copy_widget_value_to_output", "expression": "result = input_value"}},
            "publications": [{"target": "public_output.result", "source": "input_value"}, {"target": "widget.bool_result.value", "source": "input_value"}],
            "notes": [
                "This FIR is a bounded scalar widget pilot and remains downstream from canonical source.",
                "The reference C++ closure may consume an LLVM-produced native bool kernel through an explicit manifest without making the runtime LLVM-only.",
            ],
        }],
    }


def derive_example07(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "string value roundtrip pattern"
    metadata = source_metadata(source)
    if metadata.get("name") != "07_string_value_roundtrip":
        raise DerivationError(f"{context} expects metadata.name=07_string_value_roundtrip")
    inputs, outputs = assert_interface(
        source,
        [{"id": "input_text", "type": "string"}],
        [{"id": "result_text", "type": "string"}],
        context,
    )
    graph = SourceGraph.from_source(source)
    require_node(graph, "str_input_value", context)
    require_node(graph, "str_result_value", context)
    require_node(graph, "output_result_text", context)
    if graph.nodes["str_input_value"].get("kind") != "widget_value" or graph.nodes["str_input_value"].get("widget") != "str_input":
        raise DerivationError(f"{context} expects str_input_value widget_value for str_input")
    if graph.nodes["str_result_value"].get("kind") != "widget_value" or graph.nodes["str_result_value"].get("widget") != "str_result":
        raise DerivationError(f"{context} expects str_result_value widget_value for str_result")
    if graph.nodes["output_result_text"].get("kind") != "interface_output" or graph.nodes["output_result_text"].get("interface_port") != "result_text":
        raise DerivationError(f"{context} expects output_result_text interface output result_text")
    outs = graph.outgoing_all("str_input_value", "value")
    if sorted(outs, key=lambda endpoint: endpoint.node) != [
        EdgeEndpoint("output_result_text", "value"),
        EdgeEndpoint("str_result_value", "value"),
    ]:
        raise DerivationError(f"{context} expects str_input_value.value to feed result widget and public output")
    return {
        "artifact_kind": "frog_fir_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "07_string_value_roundtrip", "path": source_rel, "entry_unit": "main"},
        "front_panel_ref": {"package_path": (Path(source_rel).parent / "ui/string_panel.wfrog").as_posix(), "panel_id": "main_panel"},
        "units": [{
            "unit_id": "main",
            "kind": "string_value_roundtrip_ui_unit",
            "public_interface": {"inputs": inputs, "outputs": outputs},
            "ui_bindings": {
                "control_bindings": [{"widget_id": "str_input", "mode": "widget_value", "public_input_id": "input_text", "value_type": "string"}],
                "indicator_bindings": [{"widget_id": "str_result", "mode": "widget_value", "public_output_id": "result_text", "value_type": "string"}],
            },
            "execution_model": {"structure": "single_value_copy", "body_rule": {"kind": "copy_widget_value_to_output", "expression": "result_text = input_text"}},
            "publications": [{"target": "public_output.result_text", "source": "input_text"}, {"target": "widget.str_result.value", "source": "input_text"}],
            "notes": [
                "This FIR is a bounded scalar widget pilot and remains downstream from canonical source.",
                "The reference C++ closure consumes an LLVM-produced native string kernel through an explicit manifest without making the runtime LLVM-only.",
            ],
        }],
    }


def derive_example08(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "enum value roundtrip pattern"
    metadata = source_metadata(source)
    if metadata.get("name") != "08_enum_value_roundtrip":
        raise DerivationError(f"{context} expects metadata.name=08_enum_value_roundtrip")
    inputs, outputs = assert_interface(
        source,
        [{"id": "mode_value", "type": "enum_item_id", "enum_domain": "example08.mode"}],
        [{"id": "result_mode", "type": "enum_item_id", "enum_domain": "example08.mode"}],
        context,
    )
    graph = SourceGraph.from_source(source)
    require_node(graph, "mode_input_value", context)
    require_node(graph, "mode_result_value", context)
    require_node(graph, "output_result_mode", context)
    if graph.nodes["mode_input_value"].get("kind") != "widget_value" or graph.nodes["mode_input_value"].get("widget") != "mode_input":
        raise DerivationError(f"{context} expects mode_input_value widget_value for mode_input")
    if graph.nodes["mode_result_value"].get("kind") != "widget_value" or graph.nodes["mode_result_value"].get("widget") != "mode_result":
        raise DerivationError(f"{context} expects mode_result_value widget_value for mode_result")
    if graph.nodes["output_result_mode"].get("kind") != "interface_output" or graph.nodes["output_result_mode"].get("interface_port") != "result_mode":
        raise DerivationError(f"{context} expects output_result_mode interface output result_mode")
    outs = graph.outgoing_all("mode_input_value", "value")
    if sorted(outs, key=lambda endpoint: endpoint.node) != [
        EdgeEndpoint("mode_result_value", "value"),
        EdgeEndpoint("output_result_mode", "value"),
    ]:
        raise DerivationError(f"{context} expects mode_input_value.value to feed result widget and public output")
    return {
        "artifact_kind": "frog_fir_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "08_enum_value_roundtrip", "path": source_rel, "entry_unit": "main"},
        "front_panel_ref": {"package_path": (Path(source_rel).parent / "ui/enum_panel.wfrog").as_posix(), "panel_id": "main_panel"},
        "units": [{
            "unit_id": "main",
            "kind": "enum_value_roundtrip_ui_unit",
            "public_interface": {"inputs": inputs, "outputs": outputs},
            "ui_bindings": {
                "control_bindings": [{"widget_id": "mode_input", "mode": "widget_value", "public_input_id": "mode_value", "value_type": "enum_item_id", "enum_domain": "example08.mode"}],
                "indicator_bindings": [{"widget_id": "mode_result", "mode": "widget_value", "public_output_id": "result_mode", "value_type": "enum_item_id", "enum_domain": "example08.mode"}],
            },
            "execution_model": {"structure": "single_value_copy", "body_rule": {"kind": "copy_widget_value_to_output", "expression": "result_mode = mode_value"}},
            "publications": [{"target": "public_output.result_mode", "source": "mode_value"}, {"target": "widget.mode_result.value", "source": "mode_value"}],
            "notes": [
                "This FIR is a bounded scalar widget pilot and remains downstream from canonical source.",
                "The reference C++ closure consumes an LLVM-produced native enum kernel through an explicit manifest without making the runtime LLVM-only.",
                "The enum item vocabulary remains a front-panel/widget instance definition and is consumed by the runtime from .frog.",
            ],
        }],
    }


def derive_example09(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "path value roundtrip pattern"
    metadata = source_metadata(source)
    if metadata.get("name") != "09_path_value_roundtrip":
        raise DerivationError(f"{context} expects metadata.name=09_path_value_roundtrip")
    inputs, outputs = assert_interface(
        source,
        [{"id": "input_path", "type": "path"}],
        [{"id": "result_path", "type": "path"}],
        context,
    )
    require_widget_class(source, "path_input", "frog.widgets.path_control", context)
    require_widget_class(source, "path_result", "frog.widgets.path_indicator", context)
    graph = SourceGraph.from_source(source)
    require_node(graph, "path_input_value", context)
    require_node(graph, "path_result_value", context)
    require_node(graph, "output_result_path", context)
    if graph.nodes["path_input_value"].get("kind") != "widget_value" or graph.nodes["path_input_value"].get("widget") != "path_input":
        raise DerivationError(f"{context} expects path_input_value widget_value for path_input")
    if graph.nodes["path_result_value"].get("kind") != "widget_value" or graph.nodes["path_result_value"].get("widget") != "path_result":
        raise DerivationError(f"{context} expects path_result_value widget_value for path_result")
    if graph.nodes["output_result_path"].get("kind") != "interface_output" or graph.nodes["output_result_path"].get("interface_port") != "result_path":
        raise DerivationError(f"{context} expects output_result_path interface output result_path")
    outs = graph.outgoing_all("path_input_value", "value")
    if sorted(outs, key=lambda endpoint: endpoint.node) != [
        EdgeEndpoint("output_result_path", "value"),
        EdgeEndpoint("path_result_value", "value"),
    ]:
        raise DerivationError(f"{context} expects path_input_value.value to feed result widget and public output")
    return {
        "artifact_kind": "frog_fir_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "09_path_value_roundtrip", "path": source_rel, "entry_unit": "main"},
        "front_panel_ref": {"package_path": (Path(source_rel).parent / "ui/path_panel.wfrog").as_posix(), "panel_id": "main_panel"},
        "units": [{
            "unit_id": "main",
            "kind": "path_value_roundtrip_ui_unit",
            "public_interface": {"inputs": inputs, "outputs": outputs},
            "ui_bindings": {
                "control_bindings": [{"widget_id": "path_input", "mode": "widget_value", "public_input_id": "input_path", "value_type": "path"}],
                "indicator_bindings": [{"widget_id": "path_result", "mode": "widget_value", "public_output_id": "result_path", "value_type": "path"}],
            },
            "execution_model": {"structure": "single_value_copy", "body_rule": {"kind": "copy_widget_value_to_output", "expression": "result_path = input_path"}},
            "publications": [{"target": "public_output.result_path", "source": "input_path"}, {"target": "widget.path_result.value", "source": "input_path"}],
            "notes": [
                "This FIR is a bounded scalar Path widget pilot and remains downstream from canonical source.",
                "The reference C++ closure consumes an LLVM-produced native path kernel through an explicit manifest without making the runtime LLVM-only.",
            ],
        }],
    }


def derive_example10(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    context = "Button press to Boolean pattern"
    metadata = source_metadata(source)
    if metadata.get("name") != "10_button_press_to_boolean":
        raise DerivationError(f"{context} expects metadata.name=10_button_press_to_boolean")
    assert_interface(
        source,
        [{"id": "trigger_pressed", "type": "bool"}],
        [{"id": "pressed", "type": "bool"}],
        context,
    )
    button = require_widget_class(source, "trigger_button", "frog.widgets.button", context)
    indicator = require_widget_class(source, "pressed_indicator", "frog.widgets.boolean_indicator", context)
    if require_object(button.get("binding"), "trigger_button.binding").get("mode") != "widget_event_value":
        raise DerivationError(f"{context} expects trigger_button widget_event_value binding")
    if require_object(button.get("binding"), "trigger_button.binding").get("event") != "pressed":
        raise DerivationError(f"{context} expects trigger_button pressed event binding")
    if require_object(indicator.get("binding"), "pressed_indicator.binding").get("mode") != "widget_value":
        raise DerivationError(f"{context} expects pressed_indicator widget_value binding")

    graph = SourceGraph.from_source(source)
    require_node(graph, "trigger_button_pressed", context)
    require_node(graph, "pressed_indicator_value", context)
    require_node(graph, "output_pressed", context)
    trigger_node = graph.nodes["trigger_button_pressed"]
    if (
        trigger_node.get("kind") != "widget_event_value"
        or trigger_node.get("widget") != "trigger_button"
        or trigger_node.get("event") != "pressed"
        or trigger_node.get("value_type") != "bool"
    ):
        raise DerivationError(f"{context} expects trigger_button_pressed to publish trigger_button.pressed bool")
    if graph.nodes["pressed_indicator_value"].get("kind") != "widget_value" or graph.nodes["pressed_indicator_value"].get("widget") != "pressed_indicator":
        raise DerivationError(f"{context} expects pressed_indicator_value widget_value for pressed_indicator")
    if graph.nodes["output_pressed"].get("kind") != "interface_output" or graph.nodes["output_pressed"].get("interface_port") != "pressed":
        raise DerivationError(f"{context} expects output_pressed interface output pressed")
    outs = graph.outgoing_all("trigger_button_pressed", "value")
    if sorted(outs, key=lambda endpoint: endpoint.node) != [
        EdgeEndpoint("output_pressed", "value"),
        EdgeEndpoint("pressed_indicator_value", "value"),
    ]:
        raise DerivationError(f"{context} expects trigger_button_pressed.value to feed indicator and public output")

    return {
        "artifact_kind": "frog_fir_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": {"example_id": "10_button_press_to_boolean", "path": source_rel, "entry_unit": "main"},
        "front_panel_ref": {"package_path": (Path(source_rel).parent / "ui/button_panel.wfrog").as_posix(), "panel_id": "main_panel"},
        "units": [{
            "unit_id": "main",
            "kind": "button_press_to_boolean_ui_unit",
            "public_interface": {
                "inputs": [{"id": "trigger_pressed", "type": "bool", "binding_origin": "widget.trigger_button.pressed"}],
                "outputs": [{"id": "pressed", "type": "bool"}],
            },
            "ui_bindings": {
                "control_bindings": [{"widget_id": "trigger_button", "mode": "widget_event_value", "event": "pressed", "public_input_id": "trigger_pressed", "value_type": "bool"}],
                "indicator_bindings": [{"widget_id": "pressed_indicator", "mode": "widget_value", "public_output_id": "pressed", "value_type": "bool"}],
            },
            "execution_model": {"structure": "single_button_event_copy", "body_rule": {"kind": "copy_widget_event_to_output", "expression": "pressed = trigger_pressed"}},
            "publications": [{"target": "public_output.pressed", "source": "trigger_pressed"}, {"target": "widget.pressed_indicator.value", "source": "trigger_pressed"}],
            "notes": [
                "This FIR is a C++-first Button widget pilot and remains downstream from canonical source.",
                "The Button control is a command widget and is not the Default Boolean rectangular value-control realization.",
                "The button visual grammar is provided by the Default Button SVG realization package; this FIR does not own SVG geometry.",
                "The reference C++ closure consumes a native bool kernel through an explicit manifest without making the runtime LLVM-only.",
            ],
        }],
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


DERIVATION_RULES = [
    DerivationRule("pure_public_addition", "01_pure_addition", derive_example01),
    DerivationRule("ui_value_roundtrip", "02_ui_value_roundtrip", derive_example02),
    DerivationRule("ui_property_write", "03_ui_property_write", derive_example03),
    DerivationRule("stateful_feedback_delay", "04_stateful_feedback_delay", derive_example04),
    DerivationRule("bounded_ui_accumulator", "05_bounded_ui_accumulator", derive_example05),
    DerivationRule("boolean_value_roundtrip", "06_boolean_value_roundtrip", derive_example06),
    DerivationRule("string_value_roundtrip", "07_string_value_roundtrip", derive_example07),
    DerivationRule("enum_value_roundtrip", "08_enum_value_roundtrip", derive_example08),
    DerivationRule("path_value_roundtrip", "09_path_value_roundtrip", derive_example09),
    DerivationRule("button_press_to_boolean", "10_button_press_to_boolean", derive_example10),
]


def try_rule(rule: DerivationRule, source: dict[str, Any], source_rel: str) -> RuleAttempt:
    try:
        return RuleAttempt(rule.rule_id, rule.fir_example_id, rule.derive(source, source_rel), None)
    except (DerivationError, KeyError, TypeError, IndexError) as exc:
        return RuleAttempt(rule.rule_id, rule.fir_example_id, None, str(exc))


def derive_fir_from_source(source: dict[str, Any], source_rel: str) -> dict[str, Any]:
    attempts = [try_rule(rule, source, source_rel) for rule in DERIVATION_RULES]
    matches = [attempt for attempt in attempts if attempt.result is not None]

    if len(matches) == 1:
        return require_object(matches[0].result, f"{matches[0].rule_id}.result")

    if len(matches) > 1:
        matched_rules = ", ".join(attempt.rule_id for attempt in matches)
        raise DerivationError(f"ambiguous FIR derivation: source matches multiple derivation rules: {matched_rules}")

    diagnostics = "; ".join(f"{attempt.rule_id}: {attempt.error}" for attempt in attempts)
    metadata_name = metadata_name_for_diagnostics(source)
    raise DerivationError(f"unsupported source pattern for FIR derivation: metadata.name={metadata_name}; attempts: {diagnostics}")
