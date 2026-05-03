from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
LOWERING = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.lowering.json"
EXPECTED = (
    ROOT
    / "Implementations"
    / "Reference"
    / "ContractEmitter"
    / "examples"
    / "05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json"
)


def test_example05_contract_emission_check_passes() -> None:
    result = subprocess.run(
        [
            sys.executable,
            "-m",
            "Implementations.Reference.ContractEmitter.reference_contract_emitter",
            "--lowering",
            str(LOWERING),
            "--expected",
            str(EXPECTED),
            "--check",
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Contract emission check: ok" in result.stdout


def test_example05_contract_emitter_writes_json(tmp_path: Path) -> None:
    output = tmp_path / "contract.generated.json"
    result = subprocess.run(
        [
            sys.executable,
            "-m",
            "Implementations.Reference.ContractEmitter.reference_contract_emitter",
            "--lowering",
            str(LOWERING),
            "--output",
            str(output),
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    generated = json.loads(output.read_text(encoding="utf-8"))
    expected = json.loads(EXPECTED.read_text(encoding="utf-8"))
    assert generated == expected
