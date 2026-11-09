"""Existing IR schemas against immutable real common-Frontend E7 artifacts.

No producer, backend, compiler or graph evaluator runs here. The two backend
refusals are still valid Source/FIR documents; realization is a later boundary.
Run from FROG: python -B -m pytest sdk/ecosystem/tests/test_reference_c_fir_contract.py
"""
from __future__ import annotations

import copy
import hashlib
import json
from pathlib import Path

import pytest
from jsonschema import Draft202012Validator, ValidationError

from sdk.ecosystem.tests.validate_frontend_results import schema_registry, strict_json


FROG_ROOT = Path(__file__).resolve().parents[3]
SCHEMAS = FROG_ROOT / "IR/schema"
BRIDGE = FROG_ROOT / "Implementations/Bridges/reference-c"
CORPUS = BRIDGE / "examples"
PROBES = BRIDGE / "tests/frontend-probe"
NAMES = (
    "core-add", "scalar-exact", "array-32", "for-0", "for-3", "for-32",
    "unsupported-enum-u32", "unsupported-subtract",
)


def digest(raw):
    return hashlib.sha256(raw).hexdigest()


def artifact(name):
    return strict_json((PROBES / f"{name}.fir.json").read_bytes())


@pytest.fixture(scope="module")
def validator():
    schema = json.loads((SCHEMAS / "frog.execution-ir.schema.json").read_text(encoding="utf-8"))
    # Reuse the existing local-only SDK/IR registry, with no network retriever.
    return Draft202012Validator(schema, registry=schema_registry())


@pytest.mark.parametrize("path", sorted(SCHEMAS.glob("*.schema.json")), ids=lambda p: p.name)
def test_existing_ir_components_remain_valid_2020_12_schemas(path):
    Draft202012Validator.check_schema(json.loads(path.read_text(encoding="utf-8")))


@pytest.mark.parametrize("name", NAMES)
def test_actual_common_frontend_fir_is_schema_valid_without_rewriting(name, validator):
    manifest = strict_json((CORPUS / "corpus.json").read_bytes())
    if name.startswith("unsupported-"):
        record = next(r for r in manifest["negative_sources"] if r["id"] == name)
        source_name, source_hash = record["source"], record["source_sha256"]
        assert record["frontend_outcome"] == "success"
        assert record["expected_bridge_status"] == "unsupported"
        assert record["fallback_permitted"] is False
    else:
        record = next(r for r in manifest["sources"] if r["path"] == f"{name}.frog")
        source_name, source_hash = record["path"], record["sha256"]
    source_path, fir_path = CORPUS / source_name, PROBES / f"{name}.fir.json"
    source_raw, fir_raw = source_path.read_bytes(), fir_path.read_bytes()
    assert digest(source_raw) == source_hash
    assert digest(fir_raw) == record["probe_fir_sha256"]
    assert strict_json((PROBES / f"{name}.diagnostics.json").read_bytes()) == []
    provenance = strict_json((PROBES / f"{name}.provenance.json").read_bytes())
    assert provenance["source"]["family"] == "frog.source"
    assert provenance["source"]["version"] == "0.1"
    assert provenance["source"]["sha256"] == source_hash
    document = strict_json(fir_raw)
    before = copy.deepcopy(document)
    validator.validate(document)
    assert document == before
    assert source_path.read_bytes() == source_raw
    assert fir_path.read_bytes() == fir_raw


def test_modern_nominal_schema_is_retained_instead_of_flattened_to_u32(validator):
    document = artifact("unsupported-enum-u32")
    node = document["unit"]["objects"][0]
    port = node["ports"][0]
    assert node["family"] == "constant" and node["role"] == ""
    assert "role" not in port
    assert port["type_ref"] == "u32"
    complete_type = copy.deepcopy(port["studio_type_schema"])
    assert complete_type == node["attributes"]["studio_type_schema"]
    assert complete_type["canonical"] == 'enum<"test.collection.nonzero",u32>'
    assert complete_type["children"][0]["canonical"] == "u32"
    assert [choice["value"] for choice in complete_type["choices"]] == [2, 7]
    validator.validate(document)
    assert port["studio_type_schema"] == complete_type


@pytest.mark.parametrize("name,count", [("for-0", 0), ("for-3", 3), ("for-32", 32)])
def test_actual_for_preserves_region_and_explicit_shift_register_contract(name, count, validator):
    document = artifact(name)
    objects = document["unit"]["objects"]
    loop = next(node for node in objects if node["family"] == "structure")
    bound = next(node for node in objects if node["attributes"]["source_node_id"] == "count")
    assert bound["attributes"]["literal_value"] == count
    assert loop["kind"] == "for_loop"
    assert loop["regions_owned"] == ["region:6c6f6f70:626f6479"]
    assert loop["attributes"]["shift_registers"] == [{
        "pair_id": "accumulator", "result_port": "tunnel:next:output",
        "stages": [{"history_index": 0, "initializer_port": "tunnel:seed:input",
                    "read_port": "tunnel:seed:output", "tunnel_id": "seed", "type_ref": "i64"}],
        "write_port": "tunnel:next:input",
    }]
    expected = strict_json((CORPUS / "expected" / {
        "for-0": "for-zero.json", "for-3": "for-three.json", "for-32": "for-thirty-two.json",
    }[name]).read_bytes())
    exterior = [row["port_id"] for row in expected["outputs"] if row["node_id"] == loop["id"]]
    assert exterior == ["tunnel:next:output"]
    before = copy.deepcopy(document)
    validator.validate(document)
    assert document == before


@pytest.mark.parametrize("boundary", ["family", "document_metadata", "port"])
def test_closed_owners_still_refuse_unknown_fields_and_families(boundary, validator):
    document = artifact("core-add")
    node = document["unit"]["objects"][0]
    if boundary == "family":
        node["family"] = "unknown_backend_family"
    elif boundary == "document_metadata":
        document["metadata"]["unknown_backend_metadata"] = True
    else:
        node["ports"][0]["unknown_backend_port_field"] = True
    with pytest.raises(ValidationError):
        validator.validate(document)


def test_legacy_port_still_requires_its_real_role(validator):
    document = artifact("core-add")
    port = document["unit"]["objects"][0]["ports"][0]
    assert "studio_type_schema" not in port and port.pop("role") == "data"
    with pytest.raises(ValidationError):
        validator.validate(document)


def test_removing_full_schema_cannot_leave_a_roleless_physical_alias(validator):
    document = artifact("unsupported-enum-u32")
    port = document["unit"]["objects"][0]["ports"][0]
    del port["studio_type_schema"]
    assert port["type_ref"] == "u32" and "role" not in port
    with pytest.raises(ValidationError):
        validator.validate(document)


@pytest.mark.parametrize("schema", [{}, [], "u32", None])
def test_empty_or_nonobject_schema_cannot_bypass_the_legacy_port_role(schema, validator):
    document = artifact("unsupported-enum-u32")
    document["unit"]["objects"][0]["ports"][0]["studio_type_schema"] = schema
    with pytest.raises(ValidationError):
        validator.validate(document)


def test_nonconstant_empty_object_role_remains_forbidden(validator):
    document = artifact("core-add")
    node = document["unit"]["objects"][0]
    assert node["family"] == "widget_value"
    node["role"] = ""
    with pytest.raises(ValidationError):
        validator.validate(document)


@pytest.mark.parametrize("field,value", [
    ("profile", ""), ("profile", "x" * 257), ("profile", []),
    ("source_sha256", "f" * 64), ("source_sha256", "sha256:" + "f" * 63),
    ("source_sha256", "sha256:" + "F" * 64), ("source_sha256", "sha256:" + "f" * 65),
])
def test_added_metadata_fields_retain_explicit_bounds_and_algorithm(field, value, validator):
    document = artifact("unsupported-enum-u32")
    document["metadata"][field] = value
    with pytest.raises(ValidationError):
        validator.validate(document)
