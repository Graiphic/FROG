from __future__ import annotations

import subprocess
import sys
from pathlib import Path

from Implementations.Reference.Runtime.contract_executor import execute_acceptance, load_json
from Implementations.Reference.Runtime.python.runtime_core import Slice05RuntimeCore


ROOT = Path(__file__).resolve().parents[4]
CONTRACT = ROOT / "Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json"
WFROG = ROOT / "Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog"
SNAPSHOT = ROOT / "Implementations/Reference/Runtime/acceptance/example05_input_3.snapshot.json"
ACCEPTANCE = ROOT / "Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json"

LEGACY_KEYS = {
    "legacy_runtime_shape",
    "public_interface",
    "ui_binding",
    "state_model",
    "execution_model",
    "property_writes",
    "public_output_publication",
}


def test_slice05_runtime_core_accepts_normalized_only_contract() -> None:
    runtime = Slice05RuntimeCore(contract_path=CONTRACT, wfrog_path=WFROG)
    observed = runtime.execute(control_value=3)
    expected = load_json(SNAPSHOT)
    assert observed == expected


def test_generic_contract_executor_accepts_normalized_only_example05_contract() -> None:
    acceptance = load_json(ACCEPTANCE)
    contract = load_json(CONTRACT)
    wfrog = load_json(WFROG)
    observed = execute_acceptance(acceptance, contract, {"wfrog": wfrog})
    expected = load_json(SNAPSHOT)
    assert observed == expected
    assert LEGACY_KEYS.isdisjoint(contract["units"][0].keys())


def test_specialized_runtime_checker_passes_with_normalized_only_contract() -> None:
    result = subprocess.run(
        [sys.executable, "Implementations/Reference/Runtime/check_example05_runtime_acceptance.py"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Runtime acceptance check: ok" in result.stdout
