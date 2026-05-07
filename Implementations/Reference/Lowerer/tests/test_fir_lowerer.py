from __future__ import annotations

import copy
import json
import sys
from pathlib import Path

import pytest


ROOT = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(ROOT / "Implementations" / "Reference" / "Lowerer"))

from fir_lowerer import LoweringError, lower_fir_artifact  # noqa: E402


def load_json(path: str) -> dict:
    return json.loads((ROOT / path).read_text(encoding="utf-8"))


def test_lowering_rule_selection_uses_fir_unit_kind_not_example_id() -> None:
    fir = load_json("Examples/01_pure_addition/main.fir.json")

    mutated = copy.deepcopy(fir)
    mutated["source_ref"]["example_id"] = "intentionally_wrong_example_id"

    lowered = lower_fir_artifact(mutated, "Examples/01_pure_addition/main.fir.json")

    assert lowered["artifact_kind"] == "frog_lowered_unit"
    assert lowered["source_ref"]["example_id"] == "intentionally_wrong_example_id"
    assert lowered["lowered_units"][0]["kind"] == "pure_addition_kernel"


def test_unsupported_fir_unit_kind_reports_lowering_error() -> None:
    fir = {
        "artifact_kind": "frog_fir_unit",
        "source_ref": {
            "example_id": "unsupported",
            "path": "Examples/unsupported/main.frog",
            "entry_unit": "main",
        },
        "units": [
            {
                "unit_id": "main",
                "kind": "unsupported_fir_unit_kind",
            }
        ],
    }

    with pytest.raises(LoweringError, match="unsupported FIR unit kind"):
        lower_fir_artifact(fir, "Examples/unsupported/main.fir.json")
