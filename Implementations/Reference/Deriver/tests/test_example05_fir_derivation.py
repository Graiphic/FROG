from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
DERIVER = ROOT / "Implementations" / "Reference" / "Deriver" / "derive_example05_fir.py"
SOURCE = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.frog"
EXPECTED = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.fir.json"


def test_example05_deriver_check_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(DERIVER), "--source", str(SOURCE), "--expected", str(EXPECTED), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "FIR derivation check: ok" in result.stdout


def test_example05_deriver_writes_json(tmp_path: Path) -> None:
    output = tmp_path / "main.generated.fir.json"
    result = subprocess.run(
        [sys.executable, str(DERIVER), "--source", str(SOURCE), "--output", str(output)],
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
