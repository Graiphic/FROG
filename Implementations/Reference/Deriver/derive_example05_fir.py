#!/usr/bin/env python3
"""Derive the published Example 05 FIR from its canonical .frog source.

This tool is intentionally narrow. It is the first reproducible .frog -> FIR
reference step for the frozen Example 05 corridor. It does not implement a
general FROG compiler.

Run from the repository root:

    python Implementations/Reference/Deriver/derive_example05_fir.py --check

or:

    python Implementations/Reference/Deriver/derive_example05_fir.py \
      --source Examples/05_bounded_ui_accumulator/main.frog \
      --output build/generated/main.fir.json
"""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[3]

DEFAULT_SOURCE = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.frog"
DEFAULT_EXPECTED_FIR = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.fir.json"


class DerivationError(RuntimeError):
    """Raised when the supported Example 05 derivation cannot proceed."""


@dataclass(frozen=True)
class EdgeEndpoint:
    node: str
    port: str


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


def node_map(source: dict[str, Any]) -> dict[str, dict[str, Any]]:
    diagram = source.get("diagram")
    if not isinstance(diagram, dict):
        raise DerivationError("source.diagram must be an object")

    nodes = diagram.get("nodes")
    if not isinstance(nodes, list):
        raise DerivationError("source.diagram.nodes must be an array")

    out: dict[str, dict[str, Any]] = {}
    for node in nodes:
        if not isinstance(node, dict):
            raise DerivationError("source.diagram.nodes entries must be objects")
        node_id = node.get("id")
        if not isinstance(node_id, str):
            raise DerivationError("every node must have a string id")
        if node_id in out:
            raise DerivationError(f"duplicate node id: {node_id}")
        out[node_id] = node
    return out


def edge_list(source: dict[str, Any]) -> list[dict[str, Any]]:
    diagram = source.get("diagram")
    if not isinstance(diagram, dict):
        raise DerivationError("source.diagram must be an object")
    edges = diagram.get("edges")
    if not isinstance(edges, list):
        raise DerivationError("source.diagram.edges must be an array")
    for edge in edges:
        if not isinstance(edge, dict):
            raise DerivationError("source.diagram.edges entries must be objects")
    return edges


def endpoint(raw: Any) -> EdgeEndpoint:
    if not isinstance(raw, dict):
        raise DerivationError("edge endpoint must be an object")
    node = raw.get("node")
    port = raw.get("port")
    if not isinstance(node, str) or not isinstance(port, str):
        raise DerivationError("edge endpoint must have node and port strings")
    return EdgeEndpoint(node=node, port=port)


def find_incoming(edges: list[dict[str, Any]], node_id: str, port: str) -> EdgeEndpoint:
    matches = []
    for edge in edges:
        dst = endpoint(edge.get("to"))
        if dst.node == node_id and dst.port == port:
            matches.append(endpoint(edge.get("from")))
    if len(matches) != 1:
        raise DerivationError(f"expected exactly one incoming edge to {node_id}.{port}, found {len(matches)}")
    return matches[0]


def find_node_by_kind(nodes: dict[str, dict[str, Any]], kind: str, *, widget: str | None = None) -> dict[str, Any]:
    matches = []
    for node in nodes.values():
        if node.get("kind") == kind and (widget is None or node.get("widget") == widget):
            matches.append(node)
    if len(matches) != 1:
        extra = f" for widget {widget}" if widget else ""
        raise DerivationError(f"expected exactly one node kind {kind}{extra}, found {len(matches)}")
    return matches[0]


def constant_value(nodes: dict[str, dict[str, Any]], node_id: str) -> tuple[str, Any]:
    node = nodes.get(node_id)
    if not node:
        raise DerivationError(f"unknown constant node: {node_id}")
    if node.get("kind") != "constant":
        raise DerivationError(f"node {node_id} is not a constant")
    value_type = node.get("type")
    if not isinstance(value_type, str):
        raise DerivationError(f"constant {node_id} has no string type")
    return value_type, node.get("value")


def interface_io(source: dict[str, Any]) -> tuple[list[dict[str, str]], list[dict[str, str]]]:
    interface = source.get("interface")
    if not isinstance(interface, dict):
        raise DerivationError("source.interface must be an object")

    def normalize_ports(key: str) -> list[dict[str, str]]:
        ports = interface.get(key)
        if not isinstance(ports, list):
            raise DerivationError(f"source.interface.{key} must be an array")
        out = []
        for port in ports:
            if not isinstance(port, dict):
                raise DerivationError(f"source.interface.{key} entries must be objects")
            port_id = port.get("id")
            port_type = port.get("type")
            if not isinstance(port_id, str) or not isinstance(port_type, str):
                raise DerivationError(f"source.interface.{key} entries must contain string id and type")
            out.append({"id": port_id, "type": port_type})
        return out

    return normalize_ports("inputs"), normalize_ports("outputs")


def derive_reference_writes(source: dict[str, Any], nodes: dict[str, dict[str, Any]]) -> list[dict[str, Any]]:
    edges = edge_list(source)
    writes: list[dict[str, Any]] = []

    for node in nodes.values():
        if node.get("kind") != "primitive" or node.get("type") != "frog.ui.property_write":
            continue

        node_id = node.get("id")
        if not isinstance(node_id, str):
            raise DerivationError("property_write node without id")

        widget_member = node.get("widget_member")
        if not isinstance(widget_member, dict) or not isinstance(widget_member.get("member"), str):
            raise DerivationError(f"property_write node {node_id} must define widget_member.member")
        member = widget_member["member"]

        ref_src = find_incoming(edges, node_id, "ref")
        value_src = find_incoming(edges, node_id, "value")

        ref_node = nodes.get(ref_src.node)
        if not ref_node or ref_node.get("kind") != "widget_reference":
            raise DerivationError(f"{node_id}.ref must be fed by a widget_reference node")

        widget_id = ref_node.get("widget")
        if not isinstance(widget_id, str):
            raise DerivationError(f"widget_reference node {ref_src.node} must carry widget id")

        value_type, value_literal = constant_value(nodes, value_src.node)

        writes.append(
            {
                "widget_id": widget_id,
                "member": member,
                "value_type": value_type,
                "value_literal": value_literal,
            }
        )

    # Preserve the published ordering by widget id used in Example 05.
    order = {"ctrl_input": 0, "ind_result": 1}
    writes.sort(key=lambda item: order.get(item["widget_id"], 100))
    return writes


def derive_loop(source: dict[str, Any], nodes: dict[str, dict[str, Any]]) -> tuple[int, str, int, str]:
    loop = nodes.get("loop_accumulate")
    if not loop or loop.get("kind") != "for_loop":
        raise DerivationError("Example 05 requires a for_loop node named loop_accumulate")

    count_from = loop.get("count_from")
    if not isinstance(count_from, dict) or not isinstance(count_from.get("node"), str):
        raise DerivationError("loop_accumulate.count_from must reference a constant node")

    _, count_value = constant_value(nodes, count_from["node"])
    if not isinstance(count_value, int):
        raise DerivationError("loop_accumulate count must be an integer")

    edges = edge_list(source)
    initial_edge = find_incoming(edges, "loop_accumulate", "loop_initial_state")
    state_type, initial_state = constant_value(nodes, initial_edge.node)
    if not isinstance(initial_state, int):
        raise DerivationError("loop initial state must be an integer")

    # This first deriver only supports the frozen Example 05 accumulation pattern.
    return count_value, state_type, initial_state, "state_next = state_current + input_value"


def derive_fir(source: dict[str, Any], source_path: Path) -> dict[str, Any]:
    metadata = source.get("metadata")
    if not isinstance(metadata, dict):
        raise DerivationError("source.metadata must be an object")

    example_id = metadata.get("name")
    if example_id != "05_bounded_ui_accumulator":
        raise DerivationError("this deriver currently supports only 05_bounded_ui_accumulator")

    nodes = node_map(source)
    inputs, outputs = interface_io(source)

    ctrl_widget_node = find_node_by_kind(nodes, "widget_value", widget="ctrl_input")
    ind_widget_node = find_node_by_kind(nodes, "widget_value", widget="ind_result")

    control_widget_id = ctrl_widget_node.get("widget")
    indicator_widget_id = ind_widget_node.get("widget")
    if not isinstance(control_widget_id, str) or not isinstance(indicator_widget_id, str):
        raise DerivationError("widget_value nodes must carry widget identifiers")

    if len(inputs) != 1 or len(outputs) != 1:
        raise DerivationError("Example 05 expects one public input and one public output")

    iteration_count, state_type, initial_state, expression = derive_loop(source, nodes)
    reference_writes = derive_reference_writes(source, nodes)

    source_rel = source_path.relative_to(ROOT).as_posix() if source_path.is_absolute() else source_path.as_posix()
    example_dir = Path(source_rel).parent.as_posix()

    front_panel = source.get("front_panel")
    if not isinstance(front_panel, dict):
        raise DerivationError("source.front_panel must be an object")

    package_refs = front_panel.get("package_refs")
    if not isinstance(package_refs, list) or not package_refs or not isinstance(package_refs[0], str):
        raise DerivationError("source.front_panel.package_refs must contain at least one package path")

    package_path = (Path(example_dir) / package_refs[0]).as_posix()

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
            "package_path": package_path,
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


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Derive Example 05 FIR from canonical .frog source.")
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE, help="Path to the Example 05 .frog source.")
    parser.add_argument("--output", type=Path, default=None, help="Optional path where generated FIR should be written.")
    parser.add_argument("--expected", type=Path, default=DEFAULT_EXPECTED_FIR, help="Expected FIR artifact used by --check.")
    parser.add_argument("--check", action="store_true", help="Compare generated FIR against the expected published FIR.")
    parser.add_argument("--print", action="store_true", dest="print_json", help="Print generated FIR to stdout.")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    source_path = args.source
    if not source_path.is_absolute():
        source_path = ROOT / source_path

    expected_path = args.expected
    if not expected_path.is_absolute():
        expected_path = ROOT / expected_path

    try:
        source = load_json(source_path)
        generated = derive_fir(source, source_path)

        if args.output:
            out_path = args.output if args.output.is_absolute() else ROOT / args.output
            dump_json(generated, out_path)
            print(f"wrote generated FIR: {out_path}")

        if args.print_json:
            json.dump(generated, sys.stdout, indent=2)
            sys.stdout.write("\n")

        if args.check:
            expected = load_json(expected_path)
            if canonical_json_bytes(generated) != canonical_json_bytes(expected):
                print("FIR derivation check: FAILED", file=sys.stderr)
                print(f"expected: {expected_path}", file=sys.stderr)
                return 1
            print("FIR derivation check: ok")
            print(f"source:   {source_path.relative_to(ROOT)}")
            print(f"expected: {expected_path.relative_to(ROOT)}")

        return 0
    except DerivationError as exc:
        print(f"FIR derivation error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
