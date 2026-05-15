from __future__ import annotations

import copy
import json
import sys
from pathlib import Path

import pytest


ROOT = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(ROOT / "Implementations" / "Reference" / "ContractEmitter"))

from emit_backend_contract import (  # noqa: E402
    ContractEmissionError as Example05ContractEmissionError,
    emit_reference_host_runtime_contract,
)
from emit_examples01_04_contracts import (  # noqa: E402
    ContractEmissionError as Examples0104ContractEmissionError,
    emit_contract,
)


def load_json(path: str) -> dict:
    return json.loads((ROOT / path).read_text(encoding="utf-8"))


def test_examples0104_contract_emission_uses_lowered_unit_kind_not_example_id() -> None:
    lowering = load_json("Examples/01_pure_addition/main.lowering.json")

    mutated = copy.deepcopy(lowering)
    mutated["source_ref"]["example_id"] = "intentionally_wrong_example_id"

    contract = emit_contract(mutated)

    assert contract["backend_family"] == "reference_pure_dataflow_arithmetic"
    assert contract["example_id"] == "intentionally_wrong_example_id"
    assert contract["units"][0]["kind"] == "pure_addition_kernel"


def test_examples0104_contract_emission_rejects_unsupported_lowered_unit_kind() -> None:
    lowering = {
        "artifact_kind": "frog_lowered_unit",
        "source_ref": {
            "example_id": "unsupported",
            "path": "Examples/unsupported/main.frog",
            "entry_unit": "main",
        },
        "fir_ref": {"path": "Examples/unsupported/main.fir.json", "unit_id": "main"},
        "lowered_units": [
            {
                "unit_id": "main",
                "kind": "unsupported_lowered_unit_kind",
            }
        ],
    }

    with pytest.raises(Examples0104ContractEmissionError, match="unsupported lowered unit kind"):
        emit_contract(lowering)


def test_example05_contract_emission_uses_lowered_unit_kind_not_example_id() -> None:
    lowering = load_json("Examples/05_bounded_ui_accumulator/main.lowering.json")

    mutated = copy.deepcopy(lowering)
    mutated["source_ref"]["example_id"] = "intentionally_wrong_example_id"

    contract = emit_reference_host_runtime_contract(
        mutated,
        ui_package_path="Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog",
    )

    assert contract["backend_family"] == "reference_host_runtime_ui_binding"
    assert contract["example_id"] == "intentionally_wrong_example_id"
    assert contract["units"][0]["kind"] == "bounded_executable_ui_unit"


def test_example05_contract_emission_rejects_unsupported_lowered_unit_kind() -> None:
    lowering = load_json("Examples/05_bounded_ui_accumulator/main.lowering.json")

    mutated = copy.deepcopy(lowering)
    mutated["lowered_units"][0]["kind"] = "unsupported_lowered_unit_kind"

    with pytest.raises(Example05ContractEmissionError, match="Unsupported lowered unit kind"):
        emit_reference_host_runtime_contract(mutated)


@pytest.mark.parametrize(
    "example_rel,expected_kind",
    [
        ("06_boolean_value_roundtrip", "boolean_value_roundtrip_ui_unit"),
        ("07_string_value_roundtrip", "string_value_roundtrip_ui_unit"),
        ("08_enum_value_roundtrip", "enum_value_roundtrip_ui_unit"),
        ("09_path_value_roundtrip", "path_value_roundtrip_ui_unit"),
        ("10_button_press_to_boolean", "button_press_to_boolean_ui_unit"),
    ],
)
def test_scalar_widget_contract_emission_matches_published_contract(example_rel: str, expected_kind: str) -> None:
    lowering = load_json(f"Examples/{example_rel}/main.lowering.json")
    contract = emit_reference_host_runtime_contract(lowering, lowering_path=ROOT / f"Examples/{example_rel}/main.lowering.json")
    expected = load_json(f"Implementations/Reference/ContractEmitter/examples/{example_rel}.reference_host_runtime_ui_binding.contract.json")

    assert contract == expected
    assert contract["units"][0]["kind"] == expected_kind
