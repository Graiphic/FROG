from __future__ import annotations

import copy
import json
from pathlib import Path

import jsonschema
import pytest

from Implementations.Reference.Loader.reference_loader import load_source
from Implementations.Reference.Validator.reference_validator import validate_source
from Implementations.Reference.Validator.source_envelope import validate_envelope

ROOT = Path(__file__).resolve().parents[4]
SCHEMA = json.loads((ROOT / "Expression/schema/frog.schema.json").read_text(encoding="utf-8"))


def source():
    return load_source(str(ROOT / "Examples/01_pure_addition/main.frog"))


@pytest.mark.parametrize("raw,code", [
    (b'{"a":1,"a":2}', "duplicate_json_member"),
    (b'{"nested":{"a":1,"a":2}}', "duplicate_json_member"),
    (b'{"a":1,"\\u0061":2}', "duplicate_json_member"),
    (b'{"value":NaN}', "invalid_json_number"),
    (b'{"value":Infinity}', "invalid_json_number"),
    (b'{"value":-Infinity}', "invalid_json_number"),
    (b'{"value":1e400}', "unsupported_json_number"),
    (b'{"value":1e-400}', "unsupported_json_number"),
    (b'{"value":-1e-400}', "unsupported_json_number"),
    (b'{"value":"\xff"}', "invalid_utf8"),
    (b'{"value":', "invalid_json"),
    (b'[]', "invalid_top_level"),
])
def test_ambiguous_or_unreadable_json_fails_without_a_partial_document(tmp_path, raw, code):
    path = tmp_path / "source.frog"
    path.write_bytes(raw)
    loaded = load_source(str(path)).artifact
    assert loaded["status"] == "load_failed"
    assert loaded["document"] is None
    assert loaded["diagnostics"][0]["error_code"] == code


def test_json_payloads_and_large_integer_values_are_not_rewritten(tmp_path):
    path = tmp_path / "source.frog"
    doc = {"a": {"id": "same"}, "b": {"id": "same"}, "max": 18446744073709551615,
           "exact": "18446744073709551615", "text": "NaN / Infinity / é / 🐸"}
    path.write_text(json.dumps(doc, ensure_ascii=False), encoding="utf-8")
    assert load_source(str(path)).artifact["document"] == doc


def test_json_integer_capacity_produces_a_diagnostic(tmp_path):
    import sys
    limit = sys.get_int_max_str_digits()
    if not limit:
        pytest.skip("This interpreter explicitly disables the integer digit limit")
    path = tmp_path / "large.frog"
    path.write_text('{"value":' + ('9' * (limit + 1)) + '}', encoding="utf-8")
    loaded = load_source(str(path)).artifact
    assert loaded["status"] == "load_failed"
    assert loaded["diagnostics"][0]["error_code"] == "unsupported_json_number"


def test_finite_subnormal_and_explicit_zero_are_not_rejected(tmp_path):
    path = tmp_path / "small.frog"
    path.write_text('{"small":5e-324,"zero":0e-400,"negative_zero":-0.0}', encoding="utf-8")
    loaded = load_source(str(path)).artifact
    assert loaded["status"] == "ok"
    assert loaded["document"]["small"] > 0.0


@pytest.mark.parametrize("version", ["0.1", 0.1])
def test_both_published_version_encodings_are_accepted(version):
    loaded = source()
    loaded.artifact["document"]["spec_version"] = version
    assert validate_source(loaded).artifact["status"] == "ok"


@pytest.mark.parametrize("version", ["99.0", 99, "0.2"])
def test_unknown_version_is_not_certified_as_valid_or_mutated(version):
    loaded = source()
    loaded.artifact["document"]["spec_version"] = version
    before = copy.deepcopy(loaded.artifact)
    result = validate_source(loaded).artifact
    assert result["status"] == "unsupported_source"
    assert result["diagnostics"][0]["code"] == "unsupported_spec_version"
    assert "validated_program" not in result
    assert loaded.artifact == before


def test_future_envelope_is_not_judged_by_current_required_sections():
    loaded = source()
    loaded.artifact["document"] = {"spec_version": "99.0", "new_envelope": {}}
    before = copy.deepcopy(loaded.artifact)
    result = validate_source(loaded).artifact
    assert result["status"] == "unsupported_source"
    assert result["diagnostics"][0]["code"] == "unsupported_spec_version"
    assert loaded.artifact == before


@pytest.mark.parametrize("version", [None, True, {}, []])
def test_malformed_version_has_a_structural_diagnostic(version):
    loaded = source()
    loaded.artifact["document"]["spec_version"] = version
    result = validate_source(loaded).artifact
    assert result["status"] == "structural_invalid"
    assert result["diagnostics"][0]["code"] == "invalid_spec_version"


@pytest.mark.parametrize("section", list(SCHEMA["properties"]))
def test_section_projection_tracks_schema_shape(section):
    if section == "spec_version":
        return  # Version capability and malformed values are tested separately.
    for invalid in (None, [], False, "text", 12):
        doc = copy.deepcopy(source().artifact["document"])
        doc[section] = invalid
        assert not jsonschema.Draft202012Validator(SCHEMA).is_valid(doc)
        assert validate_envelope(doc)[0] == "structural_invalid"


@pytest.mark.parametrize("section", SCHEMA["required"])
def test_required_section_projection_tracks_schema(section):
    doc = copy.deepcopy(source().artifact["document"])
    del doc[section]
    assert not jsonschema.Draft202012Validator(SCHEMA).is_valid(doc)
    assert validate_envelope(doc)[1][0]["code"] == "missing_top_level_sections"


def test_unknown_root_is_rejected_but_unknown_descriptive_metadata_is_preserved():
    loaded = source()
    loaded.artifact["document"]["metadata"]["vendor.note"] = {"text": "preserve me"}
    assert validate_source(loaded).artifact["validated_program"]["document"]["metadata"]["vendor.note"] == {"text": "preserve me"}
    loaded.artifact["document"]["future_executable_section"] = {}
    assert validate_source(loaded).artifact["diagnostics"][0]["code"] == "unknown_top_level_section"


def test_loaded_nesting_beyond_validator_capacity_is_diagnosed_without_mutation(tmp_path):
    doc = source().artifact["document"]
    raw = json.dumps(doc).replace('"metadata": {',
        '"metadata": {"vendor.note": ' + ('{"nested":' * 500) + '0' + ('}' * 500) + ',')
    path = tmp_path / "nested.frog"
    path.write_text(raw, encoding="utf-8")
    loaded = load_source(str(path))
    assert loaded.artifact["status"] == "ok"
    original = loaded.artifact["document"]
    result = validate_source(loaded).artifact
    assert result["status"] == "unsupported_source"
    assert result["diagnostics"][0]["code"] == "source_resource_limit"
    assert "validated_program" not in result
    assert loaded.artifact["document"] is original
    cursor = original["metadata"]["vendor.note"]
    for _ in range(500):
        cursor = cursor["nested"]
    assert cursor == 0


@pytest.mark.parametrize("value", [None, 1, [], {}])
def test_metadata_name_does_not_crash_the_validator(value):
    loaded = source()
    loaded.artifact["document"]["metadata"]["name"] = value
    assert validate_source(loaded).artifact["diagnostics"][0]["code"] == "invalid_metadata_name"


@pytest.mark.parametrize("section,policy,code,status", [
    ("host", {"version": 99}, "unsupported_policy_version", "unsupported_source"),
    ("execution_policy", {"version": 2}, "unsupported_policy_version", "unsupported_source"),
    ("execution_policy", {"version": 1, "priority": "high"}, "unsupported_execution_policy", "unsupported_source"),
    ("execution_policy", {"version": 1, "debugging_allowed": "false"}, "invalid_execution_policy", "structural_invalid"),
    ("host", {"version": 1, "launch": {"trigger": "always"}}, "invalid_host_launch", "structural_invalid"),
    ("host", {"version": 1, "front_panel_window": []}, "invalid_host_window", "structural_invalid"),
    ("host", {"version": 1, "front_panel_window": {"minimum_width": -1}}, "invalid_host_window", "structural_invalid"),
    ("host", {"version": 1, "front_panel_window": {"allow_close": "false"}}, "invalid_host_window", "structural_invalid"),
    ("host", {"version": 1, "front_panel_window": {"style": "unknown"}}, "invalid_host_window", "structural_invalid"),
])
def test_unsupported_or_invalid_policy_is_not_silently_guessed(section, policy, code, status):
    loaded = source()
    loaded.artifact["document"][section] = policy
    before = copy.deepcopy(loaded.artifact)
    result = validate_source(loaded).artifact
    assert result["status"] == status
    assert result["diagnostics"][0]["code"] == code
    assert loaded.artifact == before


def test_known_host_and_execution_policy_are_preserved_without_starting_execution():
    loaded = source()
    loaded.artifact["document"]["host"] = {"version": 1, "launch": {"trigger": "on_open"},
        "front_panel_window": {"title_mode": "custom", "custom_title": "Display", "minimum_width": 0}}
    loaded.artifact["document"]["execution_policy"] = {"version": 1, "debugging_allowed": False}
    result = validate_source(loaded).artifact
    assert result["status"] == "ok"
    assert result["validated_program"]["document"]["host"] == loaded.artifact["document"]["host"]
    assert result["validated_program"]["document"]["execution_policy"] == loaded.artifact["document"]["execution_policy"]


def test_missing_descriptive_metadata_is_diagnosed():
    loaded = source()
    del loaded.artifact["document"]["metadata"]["description"]
    assert validate_source(loaded).artifact["diagnostics"][0]["code"] == "invalid_metadata_description"


@pytest.mark.parametrize("cross_direction", [False, True])
def test_interface_identity_is_unique_across_inputs_and_outputs(cross_direction):
    loaded = source()
    interface = loaded.artifact["document"]["interface"]
    duplicate = copy.deepcopy(interface["inputs"][0])
    interface["outputs" if cross_direction else "inputs"].append(duplicate)
    assert validate_source(loaded).artifact["diagnostics"][0]["code"] == "duplicate_interface_port_id"


def test_duplicate_edge_identity_is_rejected_before_semantics():
    loaded = source()
    edges = loaded.artifact["document"]["diagram"]["edges"]
    edges[1]["id"] = edges[0]["id"]
    result = validate_source(loaded).artifact
    assert result["status"] == "structural_invalid"
    assert result["diagnostics"][0]["code"] == "duplicate_edge_id"


@pytest.mark.parametrize("location", ["node", "edge", "port", "endpoint"])
def test_non_string_identifiers_never_raise_unhashable_key_errors(location):
    loaded = source()
    doc = loaded.artifact["document"]
    if location == "node":
        doc["diagram"]["nodes"][0]["id"] = []
    elif location == "edge":
        doc["diagram"]["edges"][0]["id"] = []
    elif location == "port":
        doc["interface"]["inputs"][0]["id"] = []
    else:
        doc["diagram"]["edges"][0]["to"]["node"] = []
    assert validate_source(loaded).artifact["status"] == "structural_invalid"
