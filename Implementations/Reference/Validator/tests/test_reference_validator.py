from __future__ import annotations

import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(ROOT))

from Implementations.Reference.Loader.reference_loader import load_source  # noqa: E402
from Implementations.Reference.Validator.reference_validator import validate_source  # noqa: E402


def _loaded_addition_source():
    return load_source(str(ROOT / "Examples" / "01_pure_addition" / "main.frog"))


def test_incomplete_wire_fragments_are_semantically_rejected() -> None:
    loaded = _loaded_addition_source()
    loaded.artifact["document"]["diagram"]["wire_fragments"] = [
        {
            "id": "fragment_1",
            "points": [{"x": 10, "y": 20}, {"x": 40, "y": 20}],
            "source": {"node": "in_a", "port": "value"},
        }
    ]

    result = validate_source(loaded)

    assert result.artifact["status"] == "semantic_rejected"
    assert result.artifact["diagnostics"][0]["code"] == "incomplete_wire_fragments"
    assert result.artifact["diagnostics"][0]["location"] == "diagram.wire_fragments"


def test_wire_fragments_collection_must_be_an_array() -> None:
    loaded = _loaded_addition_source()
    loaded.artifact["document"]["diagram"]["wire_fragments"] = {"id": "fragment_1"}

    result = validate_source(loaded)

    assert result.artifact["status"] == "structural_invalid"
    assert result.artifact["diagnostics"][0]["code"] == "invalid_diagram_collections"
