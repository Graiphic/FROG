from __future__ import annotations

import subprocess
import sys
from pathlib import Path

import pytest

from Implementations.Reference.Runtime.contract_executor import (
    check_acceptance_against_snapshot,
    execute_acceptance,
    load_json,
)


ROOT = Path(__file__).resolve().parents[4]

CASES = [
    "example01_pure_addition",
    "example02_ui_value_roundtrip",
    "example03_ui_property_write",
    "example04_stateful_feedback_delay",
]


def acceptance_path(case_name: str) -> Path:
    return ROOT / "Implementations" / "Reference" / "Runtime" / "acceptance" / f"{case_name}.acceptance.json"


@pytest.mark.parametrize("case_name", CASES)
def test_generic_contract_executor_matches_published_snapshot(case_name: str) -> None:
    acceptance = load_json(acceptance_path(case_name))
    refs = acceptance["artifact_refs"]
    contract = load_json(ROOT / refs["contract_path"])
    snapshot = load_json(ROOT / refs["snapshot_path"])

    observed = execute_acceptance(acceptance, contract)

    assert observed == snapshot
    check_acceptance_against_snapshot(acceptance, contract, snapshot)


@pytest.mark.parametrize("case_name", CASES)
def test_execute_reference_contract_cli_check_passes(case_name: str) -> None:
    result = subprocess.run(
        [
            sys.executable,
            "Implementations/Reference/Runtime/execute_reference_contract.py",
            "--acceptance",
            str(acceptance_path(case_name)),
            "--check",
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Reference contract execution check ok" in result.stdout


def test_example04_contract_exposes_state_id() -> None:
    contract = load_json(
        ROOT
        / "Implementations"
        / "Reference"
        / "ContractEmitter"
        / "examples"
        / "04_stateful_feedback_delay.reference_stateful_feedback_delay.contract.json"
    )
    kernel = contract["units"][0]["execution_kernel"]
    assert kernel["state_id"] == "delay_1"
