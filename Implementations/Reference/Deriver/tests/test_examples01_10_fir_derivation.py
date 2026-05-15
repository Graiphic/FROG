from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

import pytest

from Implementations.Reference.Deriver.fir_deriver import derive_fir_from_source, load_json


ROOT = Path(__file__).resolve().parents[4]
DERIVER = ROOT / "Implementations" / "Reference" / "Deriver" / "derive_fir.py"

CASES = [
    ("01_pure_addition", "Examples/01_pure_addition/main.frog", "Examples/01_pure_addition/main.fir.json"),
    ("02_ui_value_roundtrip", "Examples/02_ui_value_roundtrip/main.frog", "Examples/02_ui_value_roundtrip/main.fir.json"),
    ("03_ui_property_write", "Examples/03_ui_property_write/main.frog", "Examples/03_ui_property_write/main.fir.json"),
    ("04_stateful_feedback_delay", "Examples/04_stateful_feedback_delay/main.frog", "Examples/04_stateful_feedback_delay/main.fir.json"),
    ("05_bounded_ui_accumulator", "Examples/05_bounded_ui_accumulator/main.frog", "Examples/05_bounded_ui_accumulator/main.fir.json"),
    ("06_boolean_value_roundtrip", "Examples/06_boolean_value_roundtrip/main.frog", "Examples/06_boolean_value_roundtrip/main.fir.json"),
    ("07_string_value_roundtrip", "Examples/07_string_value_roundtrip/main.frog", "Examples/07_string_value_roundtrip/main.fir.json"),
    ("08_enum_value_roundtrip", "Examples/08_enum_value_roundtrip/main.frog", "Examples/08_enum_value_roundtrip/main.fir.json"),
    ("09_path_value_roundtrip", "Examples/09_path_value_roundtrip/main.frog", "Examples/09_path_value_roundtrip/main.fir.json"),
    ("10_button_press_to_boolean", "Examples/10_button_press_to_boolean/main.frog", "Examples/10_button_press_to_boolean/main.fir.json"),
]


@pytest.mark.parametrize("case_name,source_rel,expected_rel", CASES)
def test_deriver_check_passes(case_name: str, source_rel: str, expected_rel: str) -> None:
    result = subprocess.run(
        [sys.executable, str(DERIVER), "--source", source_rel, "--expected", expected_rel, "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "FIR derivation check: ok" in result.stdout


@pytest.mark.parametrize("case_name,source_rel,expected_rel", CASES)
def test_rule_module_derives_expected_fir(case_name: str, source_rel: str, expected_rel: str) -> None:
    source = load_json(ROOT / source_rel)
    generated = derive_fir_from_source(source, source_rel)
    expected = load_json(ROOT / expected_rel)
    assert generated == expected
