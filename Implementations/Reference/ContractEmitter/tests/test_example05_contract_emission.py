from __future__ import annotations

import subprocess
import sys
from pathlib import Path

from Implementations.Reference.ContractEmitter.reference_contract_emitter import load_json


ROOT = Path(__file__).resolve().parents[4]
CONTRACT = ROOT / "Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json"
EMITTER = ROOT / "Implementations/Reference/ContractEmitter/reference_contract_emitter.py"

LEGACY_KEYS = {
    "legacy_runtime_shape",
    "public_interface",
    "ui_binding",
    "state_model",
    "execution_model",
    "property_writes",
    "public_output_publication",
}


def test_example05_contract_emission_check_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(EMITTER), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Contract emission check: ok" in result.stdout


def test_example05_contract_is_normalized_only() -> None:
    contract = load_json(CONTRACT)
    unit = contract["units"][0]

    assert contract["example_id"] == "05_bounded_ui_accumulator"
    assert contract["artifact_refs"]["lowering_path"] == "Examples/05_bounded_ui_accumulator/main.lowering.json"
    assert contract["artifact_refs"]["wfrog_path"] == "Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog"

    assert unit["kind"] == "bounded_executable_ui_unit"
    assert set(["public_io", "ui_bindings", "execution_kernel", "effects", "publications"]).issubset(unit.keys())
    assert LEGACY_KEYS.isdisjoint(unit.keys())

    assert unit["execution_kernel"]["state_id"] == "accumulator_state"
    assert unit["execution_kernel"]["overflow_behavior"] == "reject_execution_on_u16_overflow"
