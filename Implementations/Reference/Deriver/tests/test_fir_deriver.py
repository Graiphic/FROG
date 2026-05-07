from __future__ import annotations

import copy
import json
import sys
from pathlib import Path

import pytest


ROOT = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(ROOT / "Implementations" / "Reference" / "Deriver"))

from fir_deriver import DerivationError, canonical_json_bytes, derive_fir_from_source  # noqa: E402


def load_json(path: str) -> dict:
    return json.loads((ROOT / path).read_text(encoding="utf-8"))


def test_derivation_rule_selection_does_not_trust_metadata_name() -> None:
    source = load_json("Examples/01_pure_addition/main.frog")
    expected = load_json("Examples/01_pure_addition/main.fir.json")

    mutated = copy.deepcopy(source)
    mutated["metadata"]["name"] = "intentionally wrong metadata name"

    observed = derive_fir_from_source(mutated, "Examples/01_pure_addition/main.frog")

    assert canonical_json_bytes(observed) == canonical_json_bytes(expected)


def test_unsupported_source_pattern_reports_derivation_error() -> None:
    source = {
        "metadata": {"name": "unsupported"},
        "interface": {"inputs": [], "outputs": []},
        "diagram": {"nodes": [], "edges": []},
    }

    with pytest.raises(DerivationError, match="unsupported source pattern"):
        derive_fir_from_source(source, "Examples/unsupported/main.frog")
