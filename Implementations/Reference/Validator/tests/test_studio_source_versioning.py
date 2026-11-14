# SPDX-License-Identifier: Apache-2.0
"""Check the released format boundary against byte-exact GS evidence."""

import hashlib
import json
import os
from pathlib import Path
import subprocess

import pytest
from jsonschema import Draft202012Validator


ROOT = Path(__file__).resolve().parents[4]
SOURCE = "frog.source"
DRAFT = "frog.document.draft"


def read_json(path):
    return json.loads(path.read_bytes())


BASELINE = read_json(ROOT / "Conformance/studio-save-open/fixtures.json")
FIXTURES = {item["id"]: item for item in BASELINE["fixtures"]}
SCHEMAS = {
    SOURCE: read_json(ROOT / "Expression/schema/frog.schema.json"),
    DRAFT: read_json(ROOT / "Expression/Schemas/frog.studio-document.schema.json"),
}


def test_schema_version_boundaries():
    for schema in SCHEMAS.values():
        Draft202012Validator.check_schema(schema)
    source_versions = SCHEMAS[SOURCE]["properties"]["spec_version"]["oneOf"]
    assert source_versions[0]["enum"] == BASELINE["canonical_source_versions"]
    assert [str(version) for version in source_versions[1]["enum"]] == BASELINE["canonical_source_versions"]
    draft_revision = SCHEMAS[DRAFT]["properties"]["draft_revision"]
    assert draft_revision["minimum"] == BASELINE["studio_schema_revisions"]["first"]
    assert draft_revision["maximum"] == BASELINE["studio_schema_revisions"]["last"]


@pytest.mark.parametrize("fixture_id", FIXTURES)
def test_sealed_fixture_matches_its_envelope(fixture_id):
    fixture = FIXTURES[fixture_id]
    contents = (ROOT / fixture["path"]).read_bytes()
    assert hashlib.sha256(contents).hexdigest() == fixture["sha256"]
    doc = json.loads(contents)
    schema = SCHEMAS[fixture["format"]]
    Draft202012Validator(schema).validate(doc)
    assert fixture["studio_save_open"]["version"]

    if fixture["format"] == DRAFT:
        assert str(doc["draft_revision"]) == fixture["version"]
        assert not Draft202012Validator(SCHEMAS[SOURCE]).is_valid(doc)
    else:
        assert doc["spec_version"] == fixture["version"]
        assert not Draft202012Validator(SCHEMAS[DRAFT]).is_valid(doc)


def test_canonical_schema_preserves_legacy_numeric_version_spelling():
    fixture = FIXTURES["canonical-scalar"]
    doc = read_json(ROOT / fixture["path"])
    doc["spec_version"] = float(fixture["version"])
    Draft202012Validator(SCHEMAS[SOURCE]).validate(doc)


@pytest.mark.parametrize(
    ("fixture_id", "change"),
    [
        ("canonical-scalar", {"spec_version": "0.2"}),
        ("canonical-scalar", {"spec_version": 0.2}),
        ("studio-variant-cluster", {"draft_revision": 10}),
        ("studio-variant-cluster", {"spec_version": "0.1"}),
        ("studio-compact-array", {"draft_revision": 2}),
        ("studio-compact-array", {"draft_revision": 9}),
    ],
)
def test_schema_rejects_undeclared_or_incompatible_revision(fixture_id, change):
    fixture = FIXTURES[fixture_id]
    doc = read_json(ROOT / fixture["path"])
    doc.update(change)
    assert not Draft202012Validator(SCHEMAS[fixture["format"]]).is_valid(doc)


@pytest.fixture(scope="module")
def cli():
    executable = os.environ.get("FROG_VERSIONING_FRONTEND")
    if not executable:
        pytest.skip("Set FROG_VERSIONING_FRONTEND to exercise the common Frontend")
    executable = Path(executable).resolve()
    assert executable.is_file(), executable
    return executable


def run_frontend(cli, operation, *args, expected=0):
    result = subprocess.run(
        [str(cli), operation, *map(str, args)],
        capture_output=True,
        encoding="utf-8",
        timeout=90,
    )
    assert result.returncode == expected, f"{result.args}\n{result.stdout}\n{result.stderr}"
    return json.loads(result.stdout)


def frontend_args(fixture, source=None, version=None):
    return [
        "--source", source or ROOT / fixture["path"],
        "--source-uri", f"fixture:/{fixture['id']}.frog",
        "--document-id", f"source:{fixture['id']}",
        "--fir-document-id", f"fir:{fixture['id']}",
        "--source-format", fixture["format"],
        "--source-version", version or fixture["version"],
        "--fir-version", "0.1",
    ]


def test_frontend_advertises_the_qualified_format_boundary(cli):
    description = run_frontend(cli, "describe")
    formats = description["source_formats"]
    assert {item["version"] for item in formats if item["family"] == SOURCE} == set(
        BASELINE["canonical_source_versions"]
    )
    revisions = BASELINE["studio_reader_revisions"]
    assert {item["version"] for item in formats if item["family"] == DRAFT} == {
        str(revision) for revision in range(revisions["first"], revisions["last"] + 1)
    }


@pytest.mark.parametrize("fixture_id", [key for key, item in FIXTURES.items() if item["frontend_validation"]])
def test_frontend_validates_qualified_fixture(cli, fixture_id):
    diagnostics = run_frontend(cli, "validate", *frontend_args(FIXTURES[fixture_id]))
    assert not any(item["severity"] == "error" for item in diagnostics)


def test_frontend_derives_fir_from_the_source_saved_in_gs(cli, tmp_path):
    fixture = FIXTURES["canonical-scalar"]
    fir = tmp_path / "scalar-add.fir"
    provenance = tmp_path / "provenance.json"
    run_frontend(
        cli, "derive", *frontend_args(fixture),
        "--output", fir, "--provenance-output", provenance,
    )
    assert read_json(fir)["schema_version"] == "0.1"
    source = read_json(provenance)["source"]
    assert (source["family"], source["version"], source["sha256"]) == (
        fixture["format"], fixture["version"], fixture["sha256"],
    )


@pytest.mark.parametrize(
    ("fixture_id", "field", "version"),
    [
        ("canonical-scalar", "spec_version", "0.2"),
        ("studio-variant-cluster", "draft_revision", 10),
    ],
)
def test_frontend_refuses_future_format_without_fir(cli, tmp_path, fixture_id, field, version):
    fixture = FIXTURES[fixture_id]
    doc = read_json(ROOT / fixture["path"])
    doc[field] = version
    changed = tmp_path / "future.frog"
    changed.write_text(json.dumps(doc), encoding="utf-8")
    fir = tmp_path / "future.fir"
    diagnostics = run_frontend(
        cli, "derive", *frontend_args(fixture, source=changed, version=str(version)),
        "--output", fir, expected=3,
    )
    assert {item["code"] for item in diagnostics} == {"frontend.format_unsupported"}
    assert not fir.exists()


@pytest.mark.parametrize(
    ("fixture_id", "version", "outcome", "diagnostic"),
    [
        ("canonical-scalar", "0.1", 3, "frontend.source_version_unsupported"),
        ("studio-variant-cluster", "3", 2, "frontend.source_revision_mismatch"),
    ],
)
def test_frontend_refuses_version_relabeling(cli, tmp_path, fixture_id, version, outcome, diagnostic):
    fixture = FIXTURES[fixture_id]
    source = ROOT / fixture["path"]
    if fixture["format"] == SOURCE:
        doc = read_json(source)
        doc["spec_version"] = "0.2"
        source = tmp_path / "future.frog"
        source.write_text(json.dumps(doc), encoding="utf-8")
    diagnostics = run_frontend(
        cli, "validate", *frontend_args(fixture, source=source, version=version), expected=outcome,
    )
    assert {item["code"] for item in diagnostics if item["severity"] == "error"} == {diagnostic}
