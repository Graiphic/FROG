"""API-2/API-3 contract shapes; synthetic fixtures are not derivation evidence."""
from __future__ import annotations

import copy
import json
from pathlib import Path

from jsonschema import ValidationError
import pytest

from sdk.ecosystem.tests.validate_frontend_results import (
    DEFAULT_API, main, schema_registry, strict_json, validate_document, validate_documents,
)

FIXTURES = Path(__file__).resolve().parents[1] / "fixtures"


def fixture(name):
    return json.loads((FIXTURES / name).read_text(encoding="utf-8"))


def result_documents(api="3"):
    documents = {"diagnostics": [fixture("diagnostic-api2.json")],
            "provenance": fixture("frontend-context-api2.json"),
            "descriptor": fixture("frontend-capabilities-api2.json"),
            "source_maps": fixture("frontend-source-maps.json")}
    if api == "3":
        documents["provenance"]["schema_version"] = "0.3"
        documents["provenance"]["frontend"]["api_version"] = "3"
        documents["provenance"]["fir"]["document_id"] = "fixture.api3.independent-fir"
        documents["descriptor"]["frontend"]["api_version"] = "3"
        documents["descriptor"]["frontend_apis"] = ["3"]
    else:
        assert api == "2"
    return documents


def test_api2_synthetic_documents_use_the_real_sdk_and_ir_shapes():
    documents = result_documents("2")
    before = copy.deepcopy(documents)
    validate_documents(documents, expected_api="2")
    assert documents == before


def test_historical_api1_contract_fixture_is_not_relabelled_api2_provenance():
    historical = fixture("frontend-context.json")
    assert historical["schema_version"] == "0.1"
    assert historical["frontend"]["api_version"] == "1"
    with pytest.raises(ValidationError):
        validate_document("provenance", historical)


@pytest.mark.parametrize("api", ["2", "3"])
def test_describe_and_early_refusals_can_record_no_captured_source_or_closure(api):
    provenance = result_documents(api)["provenance"]
    provenance["source"].update(family="", version="", uri="", document_id="", sha256="")
    provenance["fir"]["version"] = ""
    if api == "3":
        provenance["fir"]["document_id"] = ""
    for name in ("profiles", "operations", "dependencies", "transformations"):
        provenance[name] = []
    provenance["closure_complete"] = False
    provenance["semantic_context"] = {}
    del provenance["projected_source_sha256"]
    validate_document("provenance", provenance, expected_api=api)
    validate_documents({"provenance": provenance, "diagnostics": [],
                        "source_maps": {"schema": "frog.frontend.source_maps", "schema_version": "0.1"}}, expected_api=api)


@pytest.mark.parametrize("kind,field,value", [
    ("provenance", "schema_version", "99"),
    ("descriptor", "schema_version", "99"),
    ("source_maps", "schema_version", "99"),
    ("descriptor", "native_execution_claim", True),
    ("descriptor", "profiles_exhaustive", True),
])
def test_unknown_versions_and_execution_or_exhaustiveness_claims_are_refused(kind, field, value):
    document = result_documents()[kind]
    document[field] = value
    with pytest.raises(ValidationError):
        validate_document(kind, document)


def test_fir_locations_do_not_require_a_fabricated_source_pointer():
    diagnostic = fixture("diagnostic-api2.json")
    assert diagnostic["source"]["pointer"] == ""
    assert diagnostic["derived_pointer"].startswith("/unit/")
    validate_document("diagnostics", [diagnostic])
    del diagnostic["capability_id"]
    with pytest.raises(ValidationError):
        validate_document("diagnostics", [diagnostic])


def test_unknown_diagnostic_domain_and_bad_pointer_are_refused():
    diagnostic = fixture("diagnostic-api2.json")
    diagnostic["category"] = "unknown-private-category"
    with pytest.raises(ValidationError):
        validate_document("diagnostics", [diagnostic])
    diagnostic = fixture("diagnostic-api2.json")
    diagnostic["source"]["pointer"] = "/unknown~2escape"
    with pytest.raises(ValidationError):
        validate_document("diagnostics", [diagnostic])


def test_maps_use_the_existing_ir_component_contract_and_exact_projection_identity():
    maps = fixture("frontend-source-maps.json")
    maps["source_map"][0]["target_kind"] = "invented-target"
    with pytest.raises(ValidationError):
        validate_document("source_maps", maps)
    documents = result_documents()
    documents["source_maps"]["authored_source_sha256"] = "f" * 64
    with pytest.raises(ValueError, match="different captured authored bytes"):
        validate_documents(documents)


@pytest.mark.parametrize("api", ["2", "3"])
def test_both_owner_hash_representations_are_accepted_without_changing_identity(api):
    documents = result_documents(api)
    documents["source_maps"]["fir_source_sha256"] = "d" * 64
    validate_documents(documents, expected_api=api)


@pytest.mark.parametrize("api", ["2", "3"])
def test_unknown_context_is_retained_as_evidence_for_a_rejected_request(api):
    provenance = result_documents(api)["provenance"]
    provenance["closure_complete"] = False
    provenance["semantic_context"] = {"unknown.future.option": True}
    validate_document("provenance", provenance, expected_api=api)


@pytest.mark.parametrize("raw", [b'{"a":1,"a":2}', b'{"x":NaN}', b'"\\ud800"'])
def test_result_inspector_refuses_ambiguous_or_non_scalar_json(raw):
    with pytest.raises(ValueError):
        strict_json(raw)


def test_api3_source_and_fir_identities_are_independent_versioned_contracts():
    provenance = fixture("frontend-context-api2.json")
    provenance["schema_version"] = "0.3"
    provenance["frontend"]["api_version"] = "3"
    provenance["fir"]["document_id"] = "fir:independent-from-source"
    before = copy.deepcopy(provenance)
    validate_document("provenance", provenance, expected_api="3")
    validate_document("provenance", provenance)  # Current default is API 3.
    assert provenance == before
    with pytest.raises(ValidationError):
        validate_document("provenance", provenance, expected_api="2")
    del provenance["fir"]["document_id"]
    with pytest.raises(ValidationError):
        validate_document("provenance", provenance, expected_api="3")


def test_api3_default_and_local_registry_preserve_the_complete_result_set():
    documents = result_documents()
    before = copy.deepcopy(documents)
    assert DEFAULT_API == "3"
    registry = schema_registry()
    assert registry.get("https://graiphic.io/frog/sdk/ecosystem/0.2/frontend-context.schema.json") is not None
    assert registry.get("https://graiphic.io/frog/sdk/ecosystem/0.3/frontend-context.schema.json") is not None
    # Source-map records resolve the existing IR schemas from that local
    # registry; no derivation, external schema retriever or network is installed.
    validate_documents(documents)
    assert documents == before
    assert documents["provenance"]["fir"]["document_id"] != documents["provenance"]["source"]["document_id"]


@pytest.mark.parametrize("api", ["2", "3"])
def test_historical_and_current_provenance_cannot_be_cross_relabelled(api):
    other = "3" if api == "2" else "2"
    with pytest.raises(ValidationError):
        validate_document("provenance", result_documents(api)["provenance"], expected_api=other)
    provenance = result_documents(api)["provenance"]
    provenance["frontend"]["api_version"] = other
    with pytest.raises(ValueError, match="API identity differs"):
        validate_document("provenance", provenance, expected_api=api)


@pytest.mark.parametrize("api", ["1", "4", "99", "03", "", 3, True, None])
def test_unknown_api_dispatch_is_refused_even_without_a_provenance_document(api):
    with pytest.raises(ValueError, match="Unsupported inspection API"):
        validate_document("diagnostics", [], expected_api=api)
    with pytest.raises(ValueError, match="Unsupported inspection API"):
        validate_documents({}, expected_api=api)


@pytest.mark.parametrize("field,value", [
    ("software_version", "0.9.0"), ("commit", "2" * 40), ("sha256", "f" * 64),
])
def test_descriptor_and_provenance_must_identify_the_same_implementation(field, value):
    documents = result_documents()
    documents["descriptor"]["frontend"][field] = value
    with pytest.raises(ValueError, match="different Frontend implementations"):
        validate_documents(documents)


@pytest.mark.parametrize("kind,path,value", [
    ("provenance", ("frontend", "api_version"), 3),
    ("provenance", ("fir", "document_id"), 123),
    ("provenance", ("source", "sha256"), 123),
    ("provenance", ("limits", "max_source_bytes"), "8388608"),
    ("provenance", ("limits", "max_source_bytes"), True),
    ("descriptor", ("frontend_apis",), [3]),
    ("source_maps", ("fir_source_sha256",), 123),
])
def test_result_identity_and_limit_types_are_never_coerced(kind, path, value):
    document = result_documents()[kind]
    parent = document
    for field in path[:-1]:
        parent = parent[field]
    parent[path[-1]] = value
    with pytest.raises(ValidationError):
        validate_document(kind, document)
    assert parent[path[-1]] == value


def test_large_json_integer_identity_is_read_exactly_without_a_float_intermediate():
    integer = 18446744073709551615
    value = strict_json(b'{"integer":18446744073709551615}')
    assert type(value["integer"]) is int and value["integer"] == integer
    documents = result_documents()
    documents["provenance"]["semantic_context"] = value
    documents["provenance"]["limits"]["max_source_bytes"] = integer
    validate_documents(documents)
    assert documents["provenance"]["limits"]["max_source_bytes"] == integer


@pytest.mark.parametrize("field,value,message", [
    ("fir_source_sha256", "sha256:" + "f" * 64, "FIR source identity differs"),
    ("transformations", [], "different transformation chains"),
])
def test_maps_cannot_borrow_the_hash_or_transformation_chain_of_another_projection(field, value, message):
    documents = result_documents()
    documents["source_maps"][field] = value
    with pytest.raises(ValueError, match=message):
        validate_documents(documents)


@pytest.mark.parametrize("api", ["2", "3"])
def test_inspector_cli_defaults_to_api3_and_retains_explicit_api2(tmp_path, monkeypatch, capsys, api):
    documents = result_documents(api)
    arguments = ["validate_frontend_results.py"]
    for kind, value in documents.items():
        path = tmp_path / (kind + ".json")
        path.write_text(json.dumps(value), encoding="utf-8")
        arguments.extend(["--" + kind.replace("_", "-"), str(path)])
    if api == "2":
        arguments.extend(["--expected-api", "2"])
    monkeypatch.setattr("sys.argv", arguments)
    assert main() == 0
    evidence = json.loads(capsys.readouterr().out)
    assert evidence["outcome"] == "pass" and evidence["api"] == api
    assert evidence["frontend_invoked"] is False and evidence["compilation"] == "NOT_RUN"


def test_cli_never_auto_relabels_historical_provenance_or_accepts_an_unknown_api(tmp_path, monkeypatch, capsys):
    path = tmp_path / "historical-api2.json"
    path.write_text(json.dumps(fixture("frontend-context-api2.json")), encoding="utf-8")
    monkeypatch.setattr("sys.argv", ["validate_frontend_results.py", "--provenance", str(path)])
    assert main() == 1
    assert json.loads(capsys.readouterr().err)["outcome"] == "fail"
    monkeypatch.setattr("sys.argv", ["validate_frontend_results.py", "--expected-api", "4"])
    with pytest.raises(SystemExit) as stopped:
        main()
    assert stopped.value.code == 2
