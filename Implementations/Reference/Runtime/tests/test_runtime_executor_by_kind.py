from __future__ import annotations

import copy
from pathlib import Path

import pytest

from Implementations.Reference.Runtime.contract_executor import (
    ContractExecutionError,
    execute_acceptance,
    load_json,
)


ROOT = Path(__file__).resolve().parents[4]


def test_runtime_execution_uses_contract_unit_kind_not_example_id_for_examples0104() -> None:
    acceptance = load_json(
        ROOT / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example01_pure_addition.acceptance.json"
    )
    contract = load_json(ROOT / acceptance["artifact_refs"]["contract_path"])

    mutated = copy.deepcopy(contract)
    mutated["example_id"] = "intentionally_wrong_example_id"

    observed = execute_acceptance(acceptance, mutated)

    assert observed["status"] == "ok"
    assert observed["example_id"] == "intentionally_wrong_example_id"
    assert observed["public_outputs"] == {"result": 6.0}


def test_runtime_execution_rejects_unsupported_contract_unit_kind() -> None:
    acceptance = load_json(
        ROOT / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example01_pure_addition.acceptance.json"
    )
    contract = load_json(ROOT / acceptance["artifact_refs"]["contract_path"])

    mutated = copy.deepcopy(contract)
    mutated["units"][0]["kind"] = "unsupported_contract_unit_kind"

    with pytest.raises(ContractExecutionError, match="unsupported contract unit kind"):
        execute_acceptance(acceptance, mutated)


def test_runtime_execution_uses_contract_unit_kind_not_example_id_for_example05() -> None:
    acceptance = load_json(
        ROOT / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example05_runtime_family.acceptance.json"
    )
    contract = load_json(ROOT / acceptance["artifact_refs"]["contract_path"])
    wfrog = load_json(ROOT / acceptance["artifact_refs"]["wfrog_path"])

    mutated = copy.deepcopy(contract)
    mutated["example_id"] = "intentionally_wrong_example_id"
    mutated["source_ref"]["example_id"] = "intentionally_wrong_example_id"

    observed = execute_acceptance(acceptance, mutated, {"wfrog": wfrog})

    assert observed["status"] == "ok"
    assert observed["outputs"]["public"] == {"result": 15}
    assert observed["contract_ref"]["source_ref"]["example_id"] == "intentionally_wrong_example_id"
