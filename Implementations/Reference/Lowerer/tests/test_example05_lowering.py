from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
LOWERER = ROOT / "Implementations" / "Reference" / "Lowerer" / "lower_example05_fir.py"
FIR = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.fir.json"
EXPECTED = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.lowering.json"


def test_example05_lowerer_check_passes() -> None:
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


def test_example05_lowerer_writes_json(tmp_path: Path) -> None:
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
