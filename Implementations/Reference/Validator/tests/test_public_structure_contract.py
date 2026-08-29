from __future__ import annotations

import copy
import json
from pathlib import Path

import pytest

try:
    import jsonschema
except ImportError:  # The reference workspace requires only pytest by default.
    jsonschema = None

ROOT = Path(__file__).resolve().parents[4]
SCHEMA_PATH = ROOT / "Expression" / "schema" / "frog.structure-node.schema.json"
FIXTURE_PATH = (
    ROOT
    / "Conformance"
    / "valid"
    / "11_public_structure_families"
    / "case.frog"
)


def _schema() -> dict:
    return json.loads(SCHEMA_PATH.read_text(encoding="utf-8"))


def _nodes() -> list[dict]:
    document = json.loads(FIXTURE_PATH.read_text(encoding="utf-8"))
    return document["diagram"]["nodes"]


def _errors(node: dict) -> list:
    assert jsonschema is not None
    validator = jsonschema.Draft202012Validator(_schema())
    return list(validator.iter_errors(node))


def test_public_schema_and_fixture_publish_the_exact_shared_contract() -> None:
    schema = _schema()
    nodes = {node["structure_type"]: node for node in _nodes()}
    properties = schema["$defs"]["structureNode"]["properties"]

    assert properties["structure_type"]["enum"] == [
        "case",
        "for_loop",
        "while_loop",
        "event_structure",
        "disabled_structure",
        "conditional_disable_structure",
    ]
    assert "structure_tunnels" in properties
    assert "case_structure_true_background_rgb" in properties
    assert "for_loop_iteration_normalized_x" in properties
    assert "for_loop_subdiagram_label_visible" in properties
    assert "for_loop_subdiagram_label_text" in properties
    assert "for_loop_subdiagram_label_height" in properties
    assert "while_loop_condition_normalized_y" in properties
    assert "event_structure_data_node_normalized_y" in properties
    assert nodes["case"]["structure_terminals"]["selector"]["body_port"] == "selected_case"
    assert nodes["for_loop"]["structure_terminals"]["count"]["type"] == "i64"
    assert nodes["for_loop"]["structure_terminals"]["index"]["body_port"] == "iteration"
    assert nodes["for_loop"]["for_loop_subdiagram_label_visible"] is True
    assert nodes["for_loop"]["for_loop_subdiagram_label_text"] == "Acquisition"
    assert nodes["for_loop"]["for_loop_subdiagram_label_height"] == 72
    assert nodes["while_loop"]["structure_terminals"]["condition"]["body_port"] == "condition"
    assert nodes["event_structure"]["regions"][1]["event"] == {
        "kind": "value_change",
        "source": "widget:temperature",
    }
    assert nodes["disabled_structure"]["regions"][1]["activation"] == "disabled"
    assert nodes["conditional_disable_structure"]["regions"][0]["condition"] == {
        "any": [
            {
                "all": [
                    {
                        "symbol": "TARGET_TYPE",
                        "operator": "==",
                        "value": "Windows",
                    }
                ]
            }
        ]
    }
    assert nodes["conditional_disable_structure"]["regions"][1]["default"] is True


@pytest.mark.skipif(jsonschema is None, reason="jsonschema is not installed")
def test_public_fixture_covers_and_validates_all_standard_structure_families() -> None:
    nodes = _nodes()

    assert {node["structure_type"] for node in nodes} == {
        "case",
        "for_loop",
        "while_loop",
        "event_structure",
        "disabled_structure",
        "conditional_disable_structure",
    }
    for node in nodes:
        assert _errors(node) == []


@pytest.mark.skipif(jsonschema is None, reason="jsonschema is not installed")
def test_for_loop_public_contract_rejects_legacy_i32_count() -> None:
    node = copy.deepcopy(next(node for node in _nodes() if node["structure_type"] == "for_loop"))
    node["structure_terminals"]["count"]["type"] = "i32"

    assert _errors(node)


@pytest.mark.skipif(jsonschema is None, reason="jsonschema is not installed")
def test_event_public_contract_requires_explicit_descriptors_and_typed_fields() -> None:
    node = copy.deepcopy(next(node for node in _nodes() if node["structure_type"] == "event_structure"))
    del node["regions"][1]["event"]
    node["structure_terminals"]["event_data"]["fields"][2]["type"] = "i64"

    assert _errors(node)


@pytest.mark.skipif(jsonschema is None, reason="jsonschema is not installed")
def test_conditional_disable_requires_one_default_and_exact_operators() -> None:
    node = copy.deepcopy(
        next(
            node
            for node in _nodes()
            if node["structure_type"] == "conditional_disable_structure"
        )
    )
    node["regions"].append(
        {"id": "other_default", "default": True, "diagram": {"nodes": [], "edges": []}}
    )
    assert _errors(node)

    node = copy.deepcopy(
        next(
            node
            for node in _nodes()
            if node["structure_type"] == "conditional_disable_structure"
        )
    )
    node["regions"][0]["condition"]["any"][0]["all"][0]["operator"] = ">"
    assert _errors(node)

    node["regions"][0]["condition"]["any"][0]["all"][0]["operator"] = "=="
    node["structure_terminals"] = {"selector": {}}
    assert _errors(node)


@pytest.mark.skipif(jsonschema is None, reason="jsonschema is not installed")
def test_case_public_contract_uses_structure_kind_not_primitive_kind() -> None:
    node = copy.deepcopy(next(node for node in _nodes() if node["structure_type"] == "case"))
    node["kind"] = "primitive"

    assert _errors(node)
