from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

from Implementations.Reference.Deriver.fir_deriver import derive_fir_from_source, load_json


ROOT = Path(__file__).resolve().parents[4]
DERIVER = ROOT / "Implementations" / "Reference" / "Deriver" / "derive_fir.py"
SOURCE = ROOT / "Examples" / "07_string_value_roundtrip" / "main.frog"
EXPECTED = ROOT / "Examples" / "07_string_value_roundtrip" / "main.fir.json"


def test_example07_deriver_check_passes() -> None:
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


def test_example07_deriver_writes_json(tmp_path: Path) -> None:
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


def test_example07_rule_module_derives_expected_fir() -> None:
    source = load_json(SOURCE)
    generated = derive_fir_from_source(source, "Examples/07_string_value_roundtrip/main.frog")
    expected = load_json(EXPECTED)
    assert generated == expected
