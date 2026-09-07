"""Structural boundaries only: schema-valid is never an execution claim."""
from __future__ import annotations

import copy
import json
from pathlib import Path

import jsonschema
import pytest
from referencing import Registry, Resource

ROOT = Path(__file__).resolve().parents[4]
SCHEMA_FILES = sorted((ROOT / "IR/schema").glob("*.json")) + sorted((ROOT / "Expression/schema").glob("*.json"))


def read(path):
    return json.loads(path.read_text(encoding="utf-8"))


@pytest.mark.parametrize("path", SCHEMA_FILES, ids=lambda path: path.name)
def test_public_source_and_ir_schemas_are_valid_draft202012(path):
    jsonschema.Draft202012Validator.check_schema(read(path))


def ir_validator():
    schemas = [read(path) for path in (ROOT / "IR/schema").glob("*.json")]
    registry = Registry().with_resources((schema["$id"], Resource.from_contents(schema)) for schema in schemas)
    return jsonschema.Draft202012Validator(read(ROOT / "IR/schema/frog.execution-ir.schema.json"), registry=registry)


def test_canonical_ir_and_reference_fir_unit_are_not_the_same_artifact():
    # Deliberately empty structural fixture, NOT a semantically accepted program.
    canonical = {"schema_family": "frog.execution_ir", "schema_version": "0.1", "document_id": "audit:ir",
        "unit": {"id": "main", "objects": [], "connections": [], "regions": [], "source_map": [], "correspondence": []}}
    validator = ir_validator()
    assert list(validator.iter_errors(canonical)) == []
    for field in ("source_map", "correspondence", "regions"):
        invalid = copy.deepcopy(canonical)
        del invalid["unit"][field]
        assert not validator.is_valid(invalid)
    # The bounded reference artifact is useful and tested, but needs an explicit
    # adapter before anyone can claim the canonical Execution IR schema contract.
    reference = read(ROOT / "Examples/01_pure_addition/main.fir.json")
    assert reference["artifact_kind"] == "frog_fir_unit"
    assert not validator.is_valid(reference)


def test_private_draft_envelope_does_not_pass_as_canonical_source():
    validator = jsonschema.Draft202012Validator(read(ROOT / "Expression/schema/frog.schema.json"))
    draft = {"format": "frog.document.draft", "draft_revision": 2, "frontPanel": {"widgets": []},
             "diagram": {"nodes": [], "edges": []}}
    assert not validator.is_valid(draft)


@pytest.mark.parametrize("path", sorted((ROOT / "Conformance/valid").glob("*/case.frog")), ids=lambda path: path.parent.name)
def test_public_valid_fixtures_satisfy_the_conservative_root_schema(path):
    validator = jsonschema.Draft202012Validator(read(ROOT / "Expression/schema/frog.schema.json"))
    assert list(validator.iter_errors(read(path))) == []


@pytest.mark.parametrize("path", sorted((ROOT / "Examples").rglob("*.frog")),
                         ids=lambda path: str(path.relative_to(ROOT)))
def test_all_published_example_sources_have_a_strict_canonical_envelope(path):
    from Implementations.Reference.common import load_json_file
    _, document = load_json_file(path)
    validator = jsonschema.Draft202012Validator(read(ROOT / "Expression/schema/frog.schema.json"))
    assert list(validator.iter_errors(document)) == []
