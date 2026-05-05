from __future__ import annotations

import subprocess
import sys
from pathlib import Path

import pytest

from Implementations.Reference.LLVM.tools.llvm_lowering_emitter import emit_llvm_from_lowering_path, normalize_text


ROOT = Path(__file__).resolve().parents[4]

CASES = [
    ("01", "Examples/01_pure_addition/main.lowering.json", "Implementations/Reference/LLVM/examples/01_pure_addition/module.ll"),
    ("02", "Examples/02_ui_value_roundtrip/main.lowering.json", "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll"),
    ("03", "Examples/03_ui_property_write/main.lowering.json", "Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll"),
    ("04", "Examples/04_stateful_feedback_delay/main.lowering.json", "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll"),
    ("05", "Examples/05_bounded_ui_accumulator/main.lowering.json", "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll"),
]


@pytest.mark.parametrize("key,lowering_rel,module_rel", CASES)
def test_generic_lowering_to_llvm_emitter_matches_published_module(key: str, lowering_rel: str, module_rel: str) -> None:
    generated = emit_llvm_from_lowering_path(ROOT / lowering_rel)
    expected = normalize_text((ROOT / module_rel).read_text(encoding="utf-8"))
    assert generated == expected


@pytest.mark.parametrize("key,lowering_rel,module_rel", CASES)
def test_generic_lowering_to_llvm_cli_check_passes(key: str, lowering_rel: str, module_rel: str) -> None:
    result = subprocess.run(
        [
            sys.executable,
            "Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py",
            "--lowering",
            lowering_rel,
            "--expected",
            module_rel,
            "--check",
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "LLVM module emission check: ok" in result.stdout
