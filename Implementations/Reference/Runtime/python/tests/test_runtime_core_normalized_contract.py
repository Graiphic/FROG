from __future__ import annotations

import copy
import json
import subprocess
import sys
from pathlib import Path

from Implementations.Reference.Runtime.python.runtime_core import Slice05RuntimeCore


ROOT = Path(__file__).resolve().parents[5]

CONTRACT = ROOT / "Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json"
WFROG = ROOT / "Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog"
SNAPSHOT = ROOT / "Implementations/Reference/Runtime/acceptance/example05_input_3.snapshot.json"


LEGACY_KEYS = [
    "legacy_runtime_shape",
    "public_interface",
    "ui_binding",
    "state_model",
    "execution_model",
    "property_writes",
    "public_output_publication",
]


def load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def normalized_only_contract(tmp_path: Path) -> Path:
    contract = load_json(CONTRACT)
    unit = copy.deepcopy(contract["units"][0])
    for key in LEGACY_KEYS:
        unit.pop(key, None)
    contract["units"] = [unit]
    path = tmp_path / "example05.normalized-only.contract.json"
    path.write_text(json.dumps(contract, indent=2) + "\n", encoding="utf-8")
    return path


def test_slice05_runtime_core_uses_normalized_contract_surface_only(tmp_path: Path) -> None:
    contract_path = normalized_only_contract(tmp_path)
    runtime = Slice05RuntimeCore(contract_path=contract_path, wfrog_path=WFROG)
    observed = runtime.execute(control_value=3)
    expected = load_json(SNAPSHOT)
    assert observed == expected


def test_slice05_runtime_core_rejects_contract_without_normalized_surface(tmp_path: Path) -> None:
    contract = load_json(CONTRACT)
    unit = copy.deepcopy(contract["units"][0])
    unit.pop("public_io", None)
    contract["units"] = [unit]
    path = tmp_path / "example05.missing-public-io.contract.json"
    path.write_text(json.dumps(contract, indent=2) + "\n", encoding="utf-8")

    try:
        Slice05RuntimeCore(contract_path=path, wfrog_path=WFROG)
    except Exception as exc:
        assert "unit.public_io" in str(exc)
    else:
        raise AssertionError("runtime accepted a contract without normalized public_io")


def test_specialized_runtime_checker_passes_with_published_homogenized_contract() -> None:
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
