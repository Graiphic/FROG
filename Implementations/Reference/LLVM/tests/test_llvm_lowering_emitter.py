from __future__ import annotations

import copy
import json
import sys
from pathlib import Path

import pytest


ROOT = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(ROOT / "Implementations" / "Reference" / "LLVM" / "tools"))

from llvm_lowering_emitter import LLVMEmissionError, emit_llvm_from_lowering  # noqa: E402


def load_json(path: str) -> dict:
    return json.loads((ROOT / path).read_text(encoding="utf-8"))


def test_llvm_emission_uses_lowered_unit_kind_not_example_id() -> None:
    lowering = load_json("Examples/01_pure_addition/main.lowering.json")

    mutated = copy.deepcopy(lowering)
    mutated["source_ref"]["example_id"] = "intentionally_wrong_example_id"

    module_text = emit_llvm_from_lowering(mutated)

    assert "define double @frog_example01_add" in module_text
    assert "result=%f" in module_text


def test_unsupported_lowered_unit_kind_reports_llvm_error() -> None:
    lowering = {
        "artifact_kind": "frog_lowered_unit",
        "source_ref": {
            "example_id": "unsupported",
            "path": "Examples/unsupported/main.frog",
            "entry_unit": "main",
        },
        "lowered_units": [
            {
                "unit_id": "main",
                "kind": "unsupported_lowered_unit_kind",
            }
        ],
    }

    with pytest.raises(LLVMEmissionError, match="unsupported lowered unit kind"):
        emit_llvm_from_lowering(lowering)
