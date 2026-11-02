"""P01 S003/S004/S083: real contract intake, never package execution claims.

Run from FROG with ``python -B -m pytest sdk/ecosystem/tests/test_contracts.py``.
Fixtures describe an explicitly non-production semantic profile. The consumers
are invoked independently on the same bytes; this file implements no validator.
"""
from __future__ import annotations

import copy
import hashlib
import io
import json
import os
from pathlib import Path
import stat
import subprocess
import sys

import jsonschema
import pytest

FROG_ROOT = Path(__file__).resolve().parents[3]
SDK_ROOT = Path(__file__).resolve().parents[1]
FIXTURES = SDK_ROOT / "fixtures"
if str(FROG_ROOT) not in sys.path:
    sys.path.insert(0, str(FROG_ROOT))

from sdk.ecosystem.builder import prepare_build
from sdk.ecosystem.contract import (
    ContractError,
    MAX_MANIFEST_BYTES,
    context_fingerprint,
    inspect_manifest,
    negotiate_bridge,
    read_document,
    validate_bridge_message,
    validate_context,
    validate_manifest,
    validate_schema,
)
from sdk.ecosystem.manager import inspect_install


MANIFESTS = [
    ("library-source.json", ".bfrog", "plan_library_install"),
    ("library-compiled.json", ".bfrog", "plan_library_install"),
    ("library-mixed.json", ".bfrog", "plan_library_install"),
    ("project.json", ".pfrog", "prepare_working_copy"),
    ("example.json", ".efrog", "preview_then_working_copy"),
    ("bridge.json", ".bridgefrog", "plan_bridge_install"),
    ("driver.json", ".driverfrog", "plan_driver_prerequisites"),
]
INVALID_MANIFESTS = [
    ("future-format.json", ".bfrog", "contract.format_version", "/format_version"),
    ("duplicate-contribution.json", ".bfrog", "contract.duplicate_identity", "/contributions/1"),
    ("inline-widget-definition.json", ".bfrog", "contract.schema", "/contributions/0/contract"),
    ("working-copy-installs-palette.json", ".efrog", "contract.contribution_profile", "/contributions/0"),
]


def fixture(name):
    return json.loads((FIXTURES / name).read_text(encoding="utf-8"))


def modern_bridge_inputs():
    """Identified contract declarations, not evidence of FIR or execution.

    API-3 production derivation is covered by bridge_protocol_smoke's real C API
    calls. These isolated dictionaries exercise document dispatch/negotiation
    against the existing JSON schemas without implementing language semantics.
    Historical API-2 and S083 fixture files remain byte-for-byte unchanged.
    """
    context = fixture("frontend-context-api2.json")
    context["schema_version"] = "0.3"
    context["frontend"]["api_version"] = "3"
    context["fir"]["document_id"] = "fixture.api3.independent-fir"
    context["profiles"] = ["fixture.only.numeric.v1", "fixture.only.text@2"]
    context["operations"] = ["fixture.only.add", "fixture.only.text.literal"]
    capabilities = fixture("frontend-capabilities-api2.json")
    capabilities["frontend"]["api_version"] = "3"
    capabilities["frontend_apis"] = ["3"]
    capabilities["profiles"] = copy.deepcopy(context["profiles"])
    descriptor = fixture("bridge-describe.json")
    descriptor.update({
        "schema_version": "0.2",
        "bridge_id": "fixture.only.api3.bridge",
        "software_version": "0.1.0-local",
        "engine_id": "fixture.only.cpu",
        "fir_formats": [{"family": "frog.execution_ir", "version": "0.1"}],
        "profiles": [{"id": "fixture.only.numeric.v1", "version": "1"},
                     {"id": "fixture.only.text@2", "version": "2"}],
        "operations_exhaustive": True,
        "type_families": ["numeric", "text"],
        "services": ["execution"],
        "commands": ["start", "pause"],
        "targets": [{"id": "fixture.only.target", "hardware_id": "fixture.only.hardware",
                     "backend_id": "fixture.only.backend", "profiles": ["fixture.only.fast"]}],
        "constraints": {"schema": "fixture.only.constraints", "schema_version": "0.1",
                        "complete_graph_validate_required": True},
    })
    descriptor["limits"].update({"maximum_progress_messages": 256, "maximum_stderr_bytes": 65536})
    return context, descriptor, capabilities


def context_inputs():
    return fixture("frontend-context.json"), fixture("frontend-capabilities.json")


def changed(document, path, value):
    result = copy.deepcopy(document)
    owner = result
    for component in path[:-1]:
        owner = owner[component]
    owner[path[-1]] = value
    return result


def rejects(call, code, path):
    with pytest.raises(ContractError) as captured:
        call()
    diagnostic = captured.value.as_dict()
    assert diagnostic["code"] == code, diagnostic
    assert diagnostic["path"] == path, diagnostic
    assert diagnostic["message"].strip(), diagnostic
    return diagnostic


def test_e5_api3_discovery_dispatch_preserves_independent_fir_identity_and_inputs():
    context, descriptor, capabilities = modern_bridge_inputs()
    original = copy.deepcopy((context, descriptor, capabilities))
    validate_schema(context, "frontend-context-0.3")
    validate_schema(capabilities, "frontend-capabilities-0.2")
    validate_schema(descriptor, "bridge-describe-0.2")
    assert context["source"]["document_id"] != context["fir"]["document_id"]
    result = negotiate_bridge(context, descriptor, capabilities)
    assert result == descriptor and result is not descriptor
    result["commands"].append("fixture.only.command")
    assert (context, descriptor, capabilities) == original
    # Negotiation returned a declaration, never an admission receipt/Run handle.
    assert "receipt" not in result and "run_handle" not in result
    assert {"describe", "validate"} <= set(result["methods"])


@pytest.mark.parametrize("owner,path,value,code,pointer", [
    ("context", ("frontend", "api_version"), "4", "contract.frontend_api", "/frontend/api_version"),
    ("capabilities", ("frontend", "api_version"), "2", "contract.frontend_api", "/frontend/api_version"),
    ("capabilities", ("frontend_apis",), ["2"], "contract.frontend_api", "/frontend/api_version"),
    ("capabilities", ("schema_version",), "0.3", "contract.schema", "/schema_version"),
    ("context", ("source", "version"), "0.2", "contract.source_version", "/source"),
    ("context", ("fir", "version"), "0.2", "contract.fir_version", "/fir"),
    ("context", ("closure_complete",), False, "contract.incomplete_closure", "/closure_complete"),
    ("descriptor", ("protocol_version",), "0.2", "contract.bridge_protocol", "/protocol_version"),
    ("descriptor", ("schema_version",), "0.1", "contract.schema", "/schema_version"),
    ("descriptor", ("fir_formats", 0, "version"), "0.2", "contract.bridge_fir", "/fir_formats"),
    ("descriptor", ("profiles", 0, "version"), "0.1", "contract.bridge_profile", "/profiles"),
    ("descriptor", ("profiles", 0, "id"), "fixture.only.numeric", "contract.bridge_profile", "/profiles"),
    ("descriptor", ("operations",), [], "contract.bridge_operation", "/operations"),
    ("descriptor", ("methods",), ["describe"], "contract.bridge_methods", "/methods"),
])
def test_e5_modern_negotiation_axes_refuse_without_version_fallback(owner, path, value, code, pointer):
    context, descriptor, capabilities = modern_bridge_inputs()
    documents = {"context": context, "descriptor": descriptor, "capabilities": capabilities}
    documents[owner] = changed(documents[owner], path, value)
    before = copy.deepcopy(documents)
    rejects(lambda: negotiate_bridge(documents["context"], documents["descriptor"], documents["capabilities"]),
            code, pointer)
    assert documents == before


@pytest.mark.parametrize("identity", ["fixture.only.no-version", "fixture.only.v", "fixture.only@", "fixture.only@x.v1"])
def test_e5_modern_profile_version_is_explicit_and_matches_codec_suffix_rule(identity):
    context, descriptor, capabilities = modern_bridge_inputs()
    context["profiles"][0] = identity
    rejects(lambda: negotiate_bridge(context, descriptor, capabilities),
            "contract.bridge_profile_version", "/profiles/0")


def test_e5_modern_full_profile_id_version_and_software_are_independent_of_fir_wire_versions():
    context, descriptor, capabilities = modern_bridge_inputs()
    context["frontend"]["software_version"] = "7.4.2-preview.1"
    descriptor["software_version"] = "4.3.2"
    context["profiles"][0] = "fixture.only.numeric.v27"
    descriptor["profiles"][0] = {"id": "fixture.only.numeric.v27", "version": "27"}
    # A nonexhaustive Frontend profile catalogue is not a semantic whitelist.
    assert capabilities["profiles_exhaustive"] is False
    assert negotiate_bridge(context, descriptor, capabilities) == descriptor
    assert context["fir"]["version"] == "0.1" and descriptor["protocol_version"] == "0.1"
    capabilities["profiles_exhaustive"] = True
    rejects(lambda: negotiate_bridge(context, descriptor, capabilities), "contract.schema", "/profiles_exhaustive")


@pytest.mark.parametrize("exhaustive", [False, True])
def test_e5_modern_discovery_always_requires_complete_graph_validation(exhaustive):
    context, descriptor, capabilities = modern_bridge_inputs()
    descriptor["operations_exhaustive"] = exhaustive
    if not exhaustive:
        descriptor["operations"] = []
    assert negotiate_bridge(context, descriptor, capabilities) == descriptor
    descriptor["methods"] = ["describe", "run"]
    rejects(lambda: negotiate_bridge(context, descriptor, capabilities), "contract.bridge_methods", "/methods")


@pytest.mark.parametrize("field,code,pointer", [
    ("profiles", "contract.duplicate_identity", "/profiles/2"),
    ("targets", "contract.duplicate_identity", "/targets/1"),
])
def test_e5_modern_duplicate_discovery_identities_are_not_overwritten(field, code, pointer):
    context, descriptor, capabilities = modern_bridge_inputs()
    duplicate = copy.deepcopy(descriptor[field][0])
    if field == "targets":
        duplicate["backend_id"] = "fixture.only.other-backend"
    descriptor[field].append(duplicate)
    rejects(lambda: negotiate_bridge(context, descriptor, capabilities), code, pointer)


def test_e5_modern_fir_document_id_is_provenance_not_part_of_format_comparison():
    context, descriptor, capabilities = modern_bridge_inputs()
    context["fir"]["document_id"] = "fixture.api3.other-independent-fir"
    assert negotiate_bridge(context, descriptor, capabilities) == descriptor
    # Even explicitly advertised newer FIR does not upgrade the wire version.
    context["fir"]["version"] = "0.2"
    capabilities["fir_formats"].append({"family": "frog.execution_ir", "version": "0.2"})
    descriptor["fir_formats"].append({"family": "frog.execution_ir", "version": "0.2"})
    assert negotiate_bridge(context, descriptor, capabilities)["protocol_version"] == "0.1"
    descriptor["protocol_version"] = "0.2"
    rejects(lambda: negotiate_bridge(context, descriptor, capabilities), "contract.bridge_protocol", "/protocol_version")


def tree_bytes(root):
    return {path.relative_to(root).as_posix(): hashlib.sha256(path.read_bytes()).hexdigest()
            for path in root.rglob("*") if path.is_file()}


def cli(module, path, suffix, cwd):
    environment = os.environ.copy()
    environment["PYTHONDONTWRITEBYTECODE"] = "1"
    environment["PYTHONPATH"] = os.pathsep.join(
        part for part in (str(FROG_ROOT), environment.get("PYTHONPATH", "")) if part)
    result = subprocess.run(
        [sys.executable, "-B", "-m", "sdk.ecosystem." + module, str(path), "--suffix", suffix],
        cwd=cwd, env=environment, capture_output=True, text=True, encoding="utf-8", timeout=20,
    )
    assert result.stderr == "", result.stderr
    return result.returncode, json.loads(result.stdout)


@pytest.mark.parametrize("path", sorted((SDK_ROOT / "schemas").glob("*.schema.json")),
                         ids=lambda path: path.name)
def test_p01_schemas_are_valid_draft202012(path):
    schema = json.loads(path.read_text(encoding="utf-8"))
    assert schema["$schema"] == "https://json-schema.org/draft/2020-12/schema"
    jsonschema.Draft202012Validator.check_schema(schema)


@pytest.mark.parametrize("name,suffix,action", MANIFESTS)
def test_s004_same_read_only_bytes_reach_both_real_consumers(tmp_path, name, suffix, action):
    path = tmp_path / name
    path.write_bytes((FIXTURES / name).read_bytes())
    before = tree_bytes(tmp_path)
    path.chmod(stat.S_IREAD)
    try:
        manifest = inspect_manifest(path, suffix)
        plans = [prepare_build(path, suffix), inspect_install(path, suffix)]
        for index, module in enumerate(("builder", "manager")):
            exit_code, plan = cli(module, path, suffix, tmp_path)
            assert exit_code == 0, plan
            assert plan == plans[index]
            for field in ("identity", "profile", "dependencies", "contributions", "license", "required_rights"):
                assert plan[field] == manifest[field]
            assert plan["validated_manifest"] == manifest
        assert plans[0]["files"] == manifest["files"]
        assert plans[0]["content_mode"] == manifest["content_mode"]
        assert plans[0]["targets"] == manifest.get("targets", [])
        assert plans[0]["executable_build_performed"] is False
        assert plans[1]["next_action"] == action
        for field in ("installed", "trusted", "authorized", "ready_to_execute"):
            assert plans[1][field] is False
        assert tree_bytes(tmp_path) == before
    finally:
        path.chmod(stat.S_IREAD | stat.S_IWRITE)


@pytest.mark.parametrize("name,suffix,code,pointer", INVALID_MANIFESTS)
def test_s003_invalid_fixtures_fail_in_both_cli_consumers(tmp_path, name, suffix, code, pointer):
    path = tmp_path / name
    path.write_bytes((FIXTURES / "invalid" / name).read_bytes())
    before = tree_bytes(tmp_path)
    expected = rejects(lambda: inspect_manifest(path, suffix), code, pointer)
    for module in ("builder", "manager"):
        exit_code, output = cli(module, path, suffix, tmp_path)
        assert exit_code == 2
        assert output == {"status": "error", "diagnostic": expected}
    assert tree_bytes(tmp_path) == before


@pytest.mark.parametrize("name,suffix,_action", MANIFESTS)
def test_s003_suffix_does_not_override_content_profile(name, suffix, _action):
    document = fixture(name)
    wrong_suffix = ".pfrog" if suffix != ".pfrog" else ".bfrog"
    rejects(lambda: validate_manifest(document, wrong_suffix), "contract.suffix_profile", "/profile")
    assert validate_manifest(document, suffix.upper()) == document


@pytest.mark.parametrize("raw,code", [
    (b'{"format_version":"0.1","format_version":"99.0"}', "contract.duplicate_json_member"),
    (b'{"identity":{"package":"a","package":"b"}}', "contract.duplicate_json_member"),
    (b'{"name":1,"\\u006eame":2}', "contract.duplicate_json_member"),
    (b'{"value":NaN}', "contract.invalid_json_number"),
    (b'{"value":Infinity}', "contract.invalid_json_number"),
    (b'{"value":1e400}', "contract.unsupported_json_number"),
    (b'{"value":"\xff"}', "contract.invalid_utf8"),
    (b'{"value":', "contract.invalid_json"),
    (b'[]', "contract.invalid_top_level"),
])
def test_s003_ambiguous_json_has_no_partial_plan_and_no_rewrite(tmp_path, raw, code):
    path = tmp_path / "ambiguous.json"
    path.write_bytes(raw)
    for consumer in (prepare_build, inspect_install):
        rejects(lambda: consumer(path, ".bfrog"), code, "/")
    assert path.read_bytes() == raw


def test_s003_manifest_read_errors_and_budget_are_explicit(tmp_path):
    rejects(lambda: inspect_manifest(tmp_path / "missing.json", ".bfrog"), "contract.source_not_found", "/")
    path = tmp_path / "oversize.json"
    path.write_bytes(b" " * (MAX_MANIFEST_BYTES + 1))
    rejects(lambda: inspect_manifest(path, ".bfrog"), "contract.resource_limit", "/")


@pytest.mark.parametrize("unsafe", [
    "../escape.frog", "src/../escape.frog", "/absolute.frog", "C:/absolute.frog",
    "src\\file.frog", "src//file.frog", "./file.frog", "src/CON.txt",
    "src/file. ", "src/file.", "src/file:stream", "src/line\nfile.frog",
])
def test_s003_nonportable_inventory_paths_are_rejected(unsafe):
    document = changed(fixture("library-source.json"), ("files", 2, "path"), unsafe)
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.path", "/files/2/path")


@pytest.mark.parametrize("field", ["files", "contributions", "dependencies", "targets"])
def test_s003_duplicate_identity_has_location(field):
    document = fixture("library-compiled.json" if field == "targets" else "library-source.json")
    document[field].append(copy.deepcopy(document[field][0]))
    pointer = f"/{field}/{len(document[field]) - 1}"
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.duplicate_identity", pointer)


def test_s003_case_insensitive_inventory_alias_and_dependency_identity_are_rejected():
    document = fixture("library-source.json")
    document["files"].append(changed(document["files"][0], ("path",), "license.TXT"))
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.duplicate_identity", "/files/3")
    document = fixture("library-source.json")
    alias = changed(document["dependencies"][0], ("id",), "another.alias")
    document["dependencies"].append(alias)
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.duplicate_identity", "/dependencies/1")
    alias["identity"]["publisher"] = "org.example.other-fixture-publisher"
    assert validate_manifest(document, ".bfrog")["dependencies"] == document["dependencies"]


@pytest.mark.parametrize("path,value,code,pointer", [
    (("contributions", 0, "payload"), "absent.frog", "contract.missing_inventory", "/contributions/0/payload"),
    (("contributions", 0, "contract", "sha256"), "f" * 64, "contract.reference_hash", "/contributions/0/contract/path"),
    (("contributions", 0, "contract", "path"), "../escape.json", "contract.path", "/contributions/0/contract/path"),
    (("license", "path"), "absent.txt", "contract.missing_inventory", "/license/path"),
    (("required_rights",), ["unknown-right"], "contract.schema", "/required_rights/0"),
    (("profile",), "unknown-profile", "contract.schema", "/profile"),
])
def test_s003_references_and_unknown_contract_values_fail(path, value, code, pointer):
    document = changed(fixture("library-source.json"), path, value)
    before = copy.deepcopy(document)
    rejects(lambda: validate_manifest(document, ".bfrog"), code, pointer)
    assert document == before


@pytest.mark.parametrize("kind,definition", [
    ("export", {"ports": [{"id": "in", "type": "f64"}]}),
    ("widget", {"properties": {"value": {"type": "f64"}}}),
])
def test_s004_manifest_cannot_become_second_semantic_registry(kind, definition):
    document = fixture("library-source.json")
    document["contributions"][0]["kind"] = kind
    document["contributions"][0]["contract"].update(definition)
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.schema", "/contributions/0/contract")


@pytest.mark.parametrize("profile,forbidden_kind", [
    ("project", "palette"), ("example", "palette"), ("library", "bridge"),
    ("library", "driver"), ("bridge", "driver"), ("driver", "bridge"),
])
def test_s003_contributions_respect_distribution_profile(profile, forbidden_kind):
    name = "library-source.json" if profile == "library" else profile + ".json"
    document = fixture(name)
    if not document["contributions"]:
        document["contributions"] = fixture("library-source.json")["contributions"]
        document["contributions"][0]["payload"] = document["entry_point"]
    document["contributions"][0]["kind"] = forbidden_kind
    suffix = {"library": ".bfrog", "project": ".pfrog", "example": ".efrog",
              "bridge": ".bridgefrog", "driver": ".driverfrog"}[profile]
    rejects(lambda: validate_manifest(document, suffix), "contract.contribution_profile", "/contributions/0")


@pytest.mark.parametrize("name,mode", [("library-compiled.json", "source"),
                                       ("library-compiled.json", "mixed")])
def test_s003_content_mode_cannot_disguise_declared_files(name, mode):
    document = changed(fixture(name), ("content_mode",), mode)
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.content_mode", "/content_mode")


def test_s003_compiled_only_cannot_silently_include_source_files():
    document = changed(fixture("library-mixed.json"), ("content_mode",), "compiled")
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.content_mode", "/content_mode")


def test_s003_compiled_requires_artifact_and_target_points_to_artifact():
    document = fixture("library-compiled.json")
    next(f for f in document["files"] if f["role"] == "artifact")["role"] = "asset"
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.content_mode", "/content_mode")
    document = fixture("library-compiled.json")
    document["targets"][0]["artifact"] = document["license"]["path"]
    document["targets"][0]["sha256"] = document["files"][0]["sha256"]
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.content_mode", "/targets/0/artifact")


@pytest.mark.parametrize("version", ["0.0.9", "0.2.0"])
def test_s003_dependency_version_must_be_inside_declared_half_open_interval(version):
    document = changed(fixture("library-source.json"), ("dependencies", 0, "identity", "version"), version)
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.version_constraint", "/dependencies/0/identity/version")


@pytest.mark.parametrize("path,value,pointer", [
    (("dependencies", 0, "consumption_phases"), ["develop"], "/dependencies/0/consumption_phases/0"),
    (("dependencies", 0, "consumption_phases"), ["update"], "/dependencies/0/consumption_phases/0"),
    (("dependencies", 0, "required_location"), "machine", "/dependencies/0/required_location"),
    (("dependencies", 0, "installation_scope"), "target", "/dependencies/0/installation_scope"),
    (("required_rights",), ["debug"], "/required_rights/0"),
    (("required_rights",), ["test"], "/required_rights/0"),
])
def test_s003_entitlement_rights_consumption_phases_and_locations_are_not_interchangeable(path, value, pointer):
    document = changed(fixture("library-source.json"), path, value)
    rejects(lambda: validate_manifest(document, ".bfrog"), "contract.schema", pointer)


def test_s003_new_dependency_axes_are_mandatory_and_preserved():
    document = fixture("library-source.json")
    dependency = document["dependencies"][0]
    dependency["consumption_phases"] = ["build", "run", "deploy", "debug", "test"]
    dependency["required_location"] = "target"
    dependency["installation_scope"] = "machine"
    document["required_rights"] = ["develop", "update"]
    assert validate_manifest(document, ".bfrog") == document
    for field in ("consumption_phases", "required_location", "installation_scope"):
        incomplete = copy.deepcopy(document)
        del incomplete["dependencies"][0][field]
        rejects(lambda: validate_manifest(incomplete, ".bfrog"), "contract.schema", "/dependencies/0")
    legacy = copy.deepcopy(document)
    legacy["dependencies"][0]["scopes"] = ["build"]
    legacy["dependencies"][0]["location"] = "machine"
    rejects(lambda: validate_manifest(legacy, ".bfrog"), "contract.schema", "/dependencies/0")


@pytest.mark.parametrize("surrogate", ["\ud800", "\udfff"])
@pytest.mark.parametrize("in_key", [False, True])
def test_s003_isolated_unicode_surrogate_is_refused_before_schema_or_fingerprint(tmp_path, surrogate, in_key):
    context, capabilities = context_inputs()
    if in_key:
        context[surrogate] = "fixture"
    else:
        context["dependencies"][0]["origin"] = surrogate
    path = tmp_path / "isolated-surrogate.json"
    path.write_text(json.dumps(context, ensure_ascii=True), encoding="utf-8")
    rejects(lambda: read_document(path), "contract.invalid_unicode", "/")
    rejects(lambda: validate_schema(context, "frontend-context"), "contract.invalid_unicode", "/")
    rejects(lambda: context_fingerprint(context, capabilities), "contract.invalid_unicode", "/")


def test_s003_deep_json_is_refused_before_schema_traversal(tmp_path):
    context, capabilities = context_inputs()
    nested = "fixture"
    for _ in range(65):
        nested = {"child": nested}
    context["overdeep"] = nested
    path = tmp_path / "overdeep.json"
    path.write_text(json.dumps(context), encoding="utf-8")
    rejects(lambda: read_document(path), "contract.resource_limit", "/")
    rejects(lambda: validate_schema(context, "frontend-context"), "contract.resource_limit", "/")
    rejects(lambda: context_fingerprint(context, capabilities), "contract.resource_limit", "/")


def test_s003_real_read_budget_handles_content_larger_than_stat(tmp_path, monkeypatch):
    path = tmp_path / "growing-manifest.json"
    path.write_bytes(b"{}")
    assert path.stat().st_size == 2
    original_open = Path.open
    requested_sizes = []

    class GrowingInput(io.BytesIO):
        def read(self, size=-1):
            requested_sizes.append(size)
            assert size == MAX_MANIFEST_BYTES + 1, "Intake attempted an unbounded or incorrectly bounded read"
            return super().read(size)

    def open_input(candidate, mode="r", *args, **kwargs):
        if candidate == path:
            assert mode == "rb"
            return GrowingInput(b" " * (MAX_MANIFEST_BYTES + 1))
        return original_open(candidate, mode, *args, **kwargs)

    monkeypatch.setattr(Path, "open", open_input)
    rejects(lambda: read_document(path), "contract.resource_limit", "/")
    assert requested_sizes == [MAX_MANIFEST_BYTES + 1]


def test_s083_context_and_bridge_valid_fixtures_are_independent_owned_results():
    context, capabilities = context_inputs()
    descriptor = fixture("bridge-describe.json")
    original = copy.deepcopy((context, capabilities, descriptor))
    admitted = validate_context(context, capabilities)
    negotiated = negotiate_bridge(context, descriptor, capabilities)
    assert admitted == context and admitted is not context
    assert negotiated == descriptor and negotiated is not descriptor
    admitted["frontend"]["software_version"] = "9.9.9"
    negotiated["methods"].append("build")
    assert (context, capabilities, descriptor) == original
    assert all(p["id"].startswith("fixture.only.") for p in context["profiles"])
    assert set(descriptor["methods"]) == {"describe", "validate"}


@pytest.mark.parametrize("path,value,code,pointer", [
    (("schema_version",), "99.0", "contract.schema", "/schema_version"),
    (("source", "version"), "99.0", "contract.source_version", "/source"),
    (("source", "family"), "unknown.source", "contract.source_version", "/source"),
    (("fir", "version"), "99.0", "contract.fir_version", "/fir"),
    (("fir", "family"), "unknown.fir", "contract.fir_version", "/fir"),
    (("frontend", "api_version"), "99", "contract.frontend_api", "/frontend/api_version"),
    (("profiles", 0, "id"), "unknown.profile", "contract.profile", "/profiles/0"),
    (("profiles", 0, "version"), "99.0", "contract.profile", "/profiles/0"),
    (("operations", 0), "unknown.operation", "contract.operation", "/operations/0"),
])
def test_s083_each_unknown_axis_is_refused_without_fallback(path, value, code, pointer):
    context, capabilities = context_inputs()
    context = changed(context, path, value)
    before = copy.deepcopy((context, capabilities))
    rejects(lambda: validate_context(context, capabilities), code, pointer)
    rejects(lambda: negotiate_bridge(context, fixture("bridge-describe.json"), capabilities), code, pointer)
    assert (context, capabilities) == before


@pytest.mark.parametrize("version", ["0.1.1", "4.2.0", "0.2.0-preview.1"])
def test_s083_software_version_is_provenance_not_source_or_fir_format(version):
    context, capabilities = context_inputs()
    newer = changed(context, ("frontend", "software_version"), version)
    assert validate_context(newer, capabilities) == newer
    assert newer["source"] == context["source"] and newer["fir"] == context["fir"]
    assert newer["frontend"]["api_version"] == context["frontend"]["api_version"]
    assert negotiate_bridge(newer, fixture("bridge-describe.json"), capabilities)
    assert context_fingerprint(newer, capabilities) != context_fingerprint(context, capabilities)


def test_s083_only_selected_profiles_supply_operations_and_multiple_capability_versions_are_supported():
    context, capabilities = context_inputs()
    context["profiles"] = context["profiles"][:1]
    rejects(lambda: validate_context(context, capabilities), "contract.operation", "/operations/1")
    context["operations"] = context["operations"][:1]
    second = changed(capabilities["profiles"][0], ("version",), "0.2")
    capabilities["profiles"].append(second)
    assert validate_context(context, capabilities) == context
    context["profiles"][0]["version"] = "0.2"
    assert validate_context(context, capabilities) == context


@pytest.mark.parametrize("owner,field", [("context", "profiles"), ("context", "dependencies"),
                                        ("capabilities", "profiles"), ("bridge", "profiles")])
def test_s003_duplicate_semantic_identities_are_not_silently_overwritten(owner, field):
    context, capabilities = context_inputs()
    bridge = fixture("bridge-describe.json")
    value = {"context": context, "capabilities": capabilities, "bridge": bridge}[owner]
    value[field].append(copy.deepcopy(value[field][0]))
    pointer = f"/{field}/{len(value[field]) - 1}"
    rejects(lambda: negotiate_bridge(context, bridge, capabilities), "contract.duplicate_identity", pointer)


@pytest.mark.parametrize("path,value,code,pointer", [
    (("protocol_version",), "99.0", "contract.bridge_protocol", "/protocol_version"),
    (("schema_version",), "99.0", "contract.schema", "/schema_version"),
    (("fir_formats", 0, "version"), "99.0", "contract.bridge_fir", "/fir_formats"),
    (("profiles", 0, "version"), "99.0", "contract.bridge_profile", "/profiles"),
    (("operations",), ["fixture.only.add"], "contract.bridge_operation", "/operations"),
    (("methods",), ["describe"], "contract.bridge_methods", "/methods"),
    (("methods",), ["describe", "validate", "teleport"], "contract.schema", "/methods/2"),
])
def test_s083_bridge_protocol_fir_and_capabilities_have_distinct_diagnostics(path, value, code, pointer):
    context, capabilities = context_inputs()
    descriptor = changed(fixture("bridge-describe.json"), path, value)
    before = copy.deepcopy(descriptor)
    rejects(lambda: negotiate_bridge(context, descriptor, capabilities), code, pointer)
    assert descriptor == before


def test_s083_known_new_fir_does_not_implicitly_upgrade_bridge_protocol():
    context, capabilities = context_inputs()
    descriptor = fixture("bridge-describe.json")
    context["fir"]["version"] = "0.2"
    capabilities["fir_formats"].append(copy.deepcopy(context["fir"]))
    rejects(lambda: negotiate_bridge(context, descriptor, capabilities), "contract.bridge_fir", "/fir_formats")
    descriptor["fir_formats"].append(copy.deepcopy(context["fir"]))
    assert negotiate_bridge(context, descriptor, capabilities)["protocol_version"] == "0.1"
    descriptor["protocol_version"] = "0.2"
    rejects(lambda: negotiate_bridge(context, descriptor, capabilities), "contract.bridge_protocol", "/protocol_version")


@pytest.mark.parametrize("path,value", [
    (("source", "sha256"), "e" * 64),
    (("frontend", "commit"), "2" * 40),
    (("frontend", "sha256"), "f" * 64),
    (("dependencies", 0, "version"), "0.1.1"),
    (("dependencies", 0, "sha256"), "e" * 64),
    (("dependencies", 0, "origin"), "fixture://different-origin"),
])
def test_s083_fingerprint_invalidates_for_dependency_and_frontend_provenance(path, value):
    context, capabilities = context_inputs()
    changed_context = changed(context, path, value)
    assert context_fingerprint(changed_context, capabilities) != context_fingerprint(context, capabilities)


def test_s083_fingerprint_is_deterministic_for_unordered_contract_sets_without_mutation():
    context, capabilities = context_inputs()
    reordered = dict(reversed(list(copy.deepcopy(context).items())))
    for field in ("dependencies", "profiles", "operations"):
        reordered[field].reverse()
    before = copy.deepcopy(reordered)
    expected = context_fingerprint(context, capabilities)
    assert len(expected) == 64
    assert context_fingerprint(reordered, capabilities) == expected
    assert reordered == before


@pytest.mark.parametrize("bad", [None, [], "0.1"])
def test_s003_wrong_root_values_raise_contract_diagnostics(bad):
    rejects(lambda: validate_manifest(bad, ".bfrog"), "contract.schema", "/")
    context, capabilities = context_inputs()
    rejects(lambda: negotiate_bridge(context, bad, capabilities), "contract.schema", "/")


@pytest.mark.parametrize("name", ["bridge-request.json", "bridge-response.json", "bridge-progress.json"])
def test_s083_bridge_envelopes_admit_only_the_expected_request_session_generation(name):
    message = fixture(name)
    expected = dict(request_id="fixture.request", session_id="fixture.session", generation=7)
    admitted = validate_bridge_message(message, **expected)
    assert admitted == message and admitted is not message
    for field, value in (("request_id", "fixture.other-request"),
                         ("session_id", "fixture.other-session"), ("generation", 6)):
        stale = changed(message, (field,), value)
        rejects(lambda: validate_bridge_message(stale, **expected), "contract.stale_message", "/generation")
    future = changed(message, ("protocol_version",), "99.0")
    rejects(lambda: validate_bridge_message(future, **expected), "contract.schema", "/protocol_version")


def test_s083_progress_cannot_report_more_work_than_total():
    message = changed(fixture("bridge-progress.json"), ("progress", "completed"), 4)
    rejects(lambda: validate_bridge_message(message, request_id="fixture.request",
            session_id="fixture.session", generation=7), "contract.progress", "/progress")


@pytest.mark.parametrize("name,schema_name", [("diagnostic.json", "diagnostic"),
                                             ("evidence-unexecuted.json", "evidence")])
def test_diagnostic_and_unexecuted_evidence_have_valid_explicit_contracts(name, schema_name):
    value = fixture(name)
    schema = json.loads((SDK_ROOT / "schemas" / (schema_name + ".schema.json")).read_text(encoding="utf-8"))
    jsonschema.Draft202012Validator(schema).validate(value)
    invalid = copy.deepcopy(value)
    del invalid["source" if schema_name == "diagnostic" else "limits"]
    assert not jsonschema.Draft202012Validator(schema).is_valid(invalid)
    if schema_name == "evidence":
        assert value["status"] == "unsupported" and value["artifacts"] == []
