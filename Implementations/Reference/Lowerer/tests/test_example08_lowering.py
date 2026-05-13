from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

from Implementations.Reference.Lowerer.fir_lowerer import load_json, lower_fir_artifact


ROOT = Path(__file__).resolve().parents[4]
LOWERER = ROOT / "Implementations" / "Reference" / "Lowerer" / "lower_fir.py"
FIR = ROOT / "Examples" / "08_enum_value_roundtrip" / "main.fir.json"
EXPECTED = ROOT / "Examples" / "08_enum_value_roundtrip" / "main.lowering.json"


def test_example08_lowerer_check_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(LOWERER), "--fir", str(FIR), "--expected", str(EXPECTED), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Lowering check: ok" in result.stdout


def test_example08_lowerer_writes_json(tmp_path: Path) -> None:
    output = tmp_path / "main.generated.lowering.json"
    result = subprocess.run(
        [sys.executable, str(LOWERER), "--fir", str(FIR), "--output", str(output)],
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


def test_example08_rule_module_lowers_expected_artifact() -> None:
    fir = load_json(FIR)
    generated = lower_fir_artifact(fir, "Examples/08_enum_value_roundtrip/main.fir.json")
    expected = load_json(EXPECTED)
    assert generated == expected
