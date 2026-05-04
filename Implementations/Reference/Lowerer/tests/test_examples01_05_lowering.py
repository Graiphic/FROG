from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

from Implementations.Reference.Lowerer.fir_lowerer import load_json, lower_fir_artifact


ROOT = Path(__file__).resolve().parents[4]
LOWERER = ROOT / "Implementations" / "Reference" / "Lowerer" / "lower_fir.py"

CASES = [
    ("01_pure_addition", "Examples/01_pure_addition/main.fir.json", "Examples/01_pure_addition/main.lowering.json"),
    ("02_ui_value_roundtrip", "Examples/02_ui_value_roundtrip/main.fir.json", "Examples/02_ui_value_roundtrip/main.lowering.json"),
    ("03_ui_property_write", "Examples/03_ui_property_write/main.fir.json", "Examples/03_ui_property_write/main.lowering.json"),
    ("04_stateful_feedback_delay", "Examples/04_stateful_feedback_delay/main.fir.json", "Examples/04_stateful_feedback_delay/main.lowering.json"),
    ("05_bounded_ui_accumulator", "Examples/05_bounded_ui_accumulator/main.fir.json", "Examples/05_bounded_ui_accumulator/main.lowering.json"),
]


@pytest.mark.parametrize("case_name,fir_rel,expected_rel", CASES)
def test_lowerer_check_passes(case_name: str, fir_rel: str, expected_rel: str) -> None:
    result = subprocess.run(
        [sys.executable, str(LOWERER), "--fir", fir_rel, "--expected", expected_rel, "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Lowering check: ok" in result.stdout


@pytest.mark.parametrize("case_name,fir_rel,expected_rel", CASES)
def test_lowerer_module_derives_expected_lowering(case_name: str, fir_rel: str, expected_rel: str) -> None:
    fir = load_json(ROOT / fir_rel)
    generated = lower_fir_artifact(fir, fir_rel)
    expected = load_json(ROOT / expected_rel)
    assert generated == expected
