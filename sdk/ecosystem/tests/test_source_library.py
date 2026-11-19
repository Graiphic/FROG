"""Real archives/reference I/O only; these tests do not claim a Run or install."""
from contextlib import contextmanager
from dataclasses import replace
import hashlib
import json
from pathlib import Path
import os
import subprocess
import sys

import pytest

from sdk.ecosystem.archive import build_archive
from sdk.ecosystem.contract import ContractError, ROOT as SDK_ROOT
from sdk.ecosystem.library_resolution import LocalLibraryArchive
from sdk.ecosystem.source_library import capture_source_library, parse_source_reference
from sdk.ecosystem.tests.test_distribution_archive import payload
from sdk.ecosystem.tests.test_library_resolution import dependency

FROG = SDK_ROOT.parents[1]


@contextmanager
def refused(code):
    with pytest.raises(ContractError) as error:
        yield
    assert error.value.code == code


def library(tmp_path, *, package="addition", version="1.0.0", contribution=True):
    folder = tmp_path / (package + "-" + version)
    folder.mkdir()
    source = (FROG / "Examples/01_pure_addition/main.frog").read_bytes()
    license_bytes = (FROG / "LICENSE").read_bytes()
    (folder / "Addition.frog").write_bytes(source)
    (folder / "LICENSE").write_bytes(license_bytes)
    digest = hashlib.sha256(source).hexdigest()
    manifest = {"schema": "graiphic.distribution", "format_version": "0.1", "profile": "library",
                "content_mode": "source", "identity": {"publisher": "org.example.source-library", "package": package, "version": version},
                "files": [{"path": "Addition.frog", "sha256": digest, "size": len(source), "role": "source"},
                          {"path": "LICENSE", "sha256": hashlib.sha256(license_bytes).hexdigest(), "size": len(license_bytes), "role": "license"}],
                "contributions": [{"id": "add", "kind": "export", "contract": {"path": "Addition.frog", "sha256": digest, "symbol": "add.entry"}, "payload": "Addition.frog"}] if contribution else [],
                "dependencies": [], "license": {"expression": "Apache-2.0", "path": "LICENSE"}, "required_rights": ["build"]}
    descriptor = folder / "manifest.json"
    descriptor.write_text(json.dumps(manifest), encoding="utf-8")
    archive = folder / (package + ".bfrog")
    build_archive(descriptor, folder, archive)
    return LocalLibraryArchive(archive, "local:test"), source


def test_native_install_handoff_uses_exact_capture_and_existing_transaction_owner(tmp_path, monkeypatch, capsys):
    from sdk.ecosystem import manager
    archive, source = library(tmp_path)
    helper = tmp_path / "native-installer.exe"
    helper.write_bytes(b"not-executed-unit-test-host")
    stages = []
    def native(command, **options):
        assert command[:2] == [str(helper), "install-source-library"]
        assert options["shell"] is False and options["timeout"] == 120
        stage = Path(command[command.index("--source") + 1])
        stages.append(stage)
        receipt = json.loads((stage / "source-library-capture.json").read_text(encoding="utf-8"))
        manifest_digest = command[command.index("--manifest-sha256") + 1]
        assert receipt["manifest_sha256"] == manifest_digest
        assert receipt["dependency_lock"]["root"]["manifest_sha256"] == manifest_digest
        assert (stage / "Addition.frog").read_bytes() == source
        assert receipt["frontend_invoked"] is False
        return subprocess.CompletedProcess(command, 0, json.dumps({"success": True, "execution_qualified": False}), "")
    monkeypatch.setattr(manager.subprocess, "run", native)
    args = [str(archive.path), "--origin", archive.origin, "--root", str(tmp_path / "installed"), "--native-installer", str(helper)]
    assert manager.install_source_library_command(args) == 0
    assert json.loads(capsys.readouterr().out)["success"] is True
    assert stages and not stages[0].exists()
    assert not (tmp_path / "installed").exists()  # Unit host did not install anything.


@pytest.mark.parametrize("reply,exit_code", [(json.dumps({"success": False, "code": "library.install.version_conflict"}), 2), ("[]", 2), ("invalid-json", 2), (json.dumps({"success": True}), 2), (json.dumps({"success": False}), 0)])
def test_native_install_failure_is_not_reported_as_success(tmp_path, monkeypatch, capsys, reply, exit_code):
    from sdk.ecosystem import manager
    archive, _ = library(tmp_path)
    helper = tmp_path / "native-installer.exe"; helper.write_bytes(b"unit-host")
    monkeypatch.setattr(manager.subprocess, "run", lambda command, **options: subprocess.CompletedProcess(command, exit_code, reply, ""))
    assert manager.install_source_library_command([str(archive.path), "--origin", archive.origin,
        "--root", str(tmp_path / "installed"), "--native-installer", str(helper)]) == 2
    result = json.loads(capsys.readouterr().out)
    assert result["success"] is False
    assert not (tmp_path / "installed").exists()


def test_native_install_never_searches_path_for_a_missing_host(tmp_path, capsys):
    from sdk.ecosystem import manager
    archive, _ = library(tmp_path)
    assert manager.install_source_library_command([str(archive.path), "--origin", archive.origin,
        "--root", str(tmp_path / "installed"), "--native-installer", "native-installer.exe"]) == 2
    assert json.loads(capsys.readouterr().out)["diagnostic"]["code"] == "library.install.host_required"


def test_real_fixture_producer_preserves_authored_ports_wires_and_panel(tmp_path):
    from sdk.ecosystem import source_library
    producer = Path(source_library.__file__).parent / "examples/source-document-library/build.py"
    output, consumer = tmp_path / "addition.bfrog", tmp_path / "consumer.frog"
    completed = subprocess.run([sys.executable, "-B", "-X", "utf8", str(producer), "--output", str(output),
        "--consumer", str(consumer), "--origin", "local:studio"], capture_output=True, text=True, check=True)
    receipt = json.loads(completed.stdout)
    template = json.loads((producer.parent / "consumer-template.frog").read_text(encoding="utf-8"))
    document = json.loads(consumer.read_text(encoding="utf-8"))
    with capture_source_library(LocalLibraryArchive(output, "local:studio"), tmp_path) as captured:
        expected = captured.exports[0].reference.encode()
        assert receipt["reference"] == expected
        template["diagram"]["nodes"][0]["call"]["document_path"] = expected
        assert document == template
        assert len(document["diagram"]["edges"]) == 3
        assert [(p["id"], p["value_type"]) for p in document["diagram"]["nodes"][0]["call"]["ports"]] == [("x", "f64"), ("y", "f64"), ("sum", "f64")]
        assert receipt["consumer_sha256"] == hashlib.sha256(consumer.read_bytes()).hexdigest()
        assert receipt["execution_qualified"] is False


def test_real_fixture_producer_refuses_an_existing_consumer_before_any_output(tmp_path):
    from sdk.ecosystem import source_library
    producer = Path(source_library.__file__).parent / "examples/source-document-library/build.py"
    output, consumer = tmp_path / "not-created.bfrog", tmp_path / "keep.frog"
    consumer.write_bytes(b"existing user document")
    completed = subprocess.run([sys.executable, "-B", str(producer), "--output", str(output),
        "--consumer", str(consumer)], capture_output=True, text=True, check=False)
    assert completed.returncode == 2
    assert consumer.read_bytes() == b"existing user document" and not output.exists()


def test_pinned_source_export_uses_verified_bytes_and_existing_archive_lifetime(tmp_path):
    archive, source = library(tmp_path)
    with capture_source_library(archive, tmp_path) as captured:
        staged = captured.directory
        entry = captured.exports[0]
        reference = entry.reference.encode()
        assert parse_source_reference(reference) == entry.reference
        snapshot = captured.resolve(reference)
        assert snapshot.bytes == source and snapshot.sha256 == hashlib.sha256(source).hexdigest()
        assert snapshot.reference.export_id == "add" and snapshot.reference.symbol == "add.entry"
        assert captured.lock["root"]["manifest_sha256"] == captured.manifest_sha256
        assert captured.manifest["license"]["expression"] == "Apache-2.0"
    assert not staged.exists()


@pytest.mark.parametrize("field,value,code", [
    ("version", "2.0.0", "library.reference_identity"),
    ("origin", "local:other", "library.origin_mismatch"),
    ("manifest_sha256", "0" * 64, "library.manifest_changed"),
    ("export_id", "missing", "library.export_missing"),
    ("symbol", "missing", "library.symbol_mismatch"),
    ("contract_sha256", "0" * 64, "library.contract_changed"),
])
def test_wrong_exact_identity_or_symbol_never_substitutes_another_export(tmp_path, field, value, code):
    archive, _ = library(tmp_path)
    with capture_source_library(archive, tmp_path) as captured:
        wrong = replace(captured.exports[0].reference, **{field: value}).encode()
        with refused(code):
            captured.resolve(wrong)


def test_same_export_symbol_in_two_packages_remains_distinct(tmp_path):
    first, _ = library(tmp_path, package="first")
    second, _ = library(tmp_path, package="second")
    with capture_source_library(first, tmp_path) as a, capture_source_library(second, tmp_path) as b:
        assert a.exports[0].reference.symbol == b.exports[0].reference.symbol
        assert a.exports[0].reference.encode() != b.exports[0].reference.encode()
        with refused("library.reference_identity"):
            b.resolve(a.exports[0].reference.encode())


def test_staged_source_tampering_is_refused_without_mutating_archive(tmp_path):
    archive, _ = library(tmp_path)
    before = archive.path.read_bytes()
    with capture_source_library(archive, tmp_path) as captured:
        reference = captured.exports[0].reference.encode()
        (captured.directory / captured.exports[0].payload).write_bytes(b"changed")
        with refused("library.payload_changed"):
            captured.resolve(reference)
    assert archive.path.read_bytes() == before


@pytest.mark.parametrize("mode", ["library-compiled", "library-mixed"])
def test_unimplemented_native_modes_are_refused_after_normal_codec_validation(tmp_path, mode):
    folder, _, descriptor = payload(tmp_path, mode)
    archive = tmp_path / "compiled.bfrog"
    build_archive(descriptor, folder, archive)
    with refused("library.source_profile_unsupported"):
        with capture_source_library(LocalLibraryArchive(archive, "local:test"), tmp_path):
            pytest.fail("An unsupported mode was silently accepted")


def test_package_without_callable_export_stays_non_callable(tmp_path):
    archive, _ = library(tmp_path, contribution=False)
    with refused("library.export_missing"):
        with capture_source_library(archive, tmp_path):
            pytest.fail("The historical manifest-only witness became callable")


@pytest.mark.parametrize("reference", ["Addition.frog", "bfrog:///x", "bfrog:///a/b/1/e?profile=unknown", "bfrog:///a/b/1/e?profile=x&profile=x", "\ud800"])
def test_malformed_references_have_an_explicit_diagnostic(reference):
    with refused("library.reference_invalid"):
        parse_source_reference(reference)


def test_unresolved_dependency_is_reported_by_the_existing_closure_owner(tmp_path):
    archive, _ = library(tmp_path)
    descriptor = archive.path.parent / "manifest.json"
    manifest = json.loads(descriptor.read_bytes())
    manifest["dependencies"] = [dependency("absent")]
    descriptor.write_text(json.dumps(manifest), encoding="utf-8")
    selected = archive.path.parent / "requiring.bfrog"
    build_archive(descriptor, archive.path.parent, selected)
    with refused("library.missing_offline"):
        with capture_source_library(LocalLibraryArchive(selected, archive.origin), tmp_path):
            pytest.fail("A missing dependency was silently ignored")


def test_cancellation_and_absent_archive_preserve_the_host_directory(tmp_path):
    archive, _ = library(tmp_path)
    before = set(tmp_path.iterdir())
    with refused("archive.cancelled"):
        with capture_source_library(archive, tmp_path, cancelled=lambda: True):
            pytest.fail("Cancelled staging continued")
    assert set(tmp_path.iterdir()) == before
    with refused("archive.io"):
        with capture_source_library(LocalLibraryArchive(tmp_path / "absent.bfrog", archive.origin), tmp_path):
            pytest.fail("An absent package was substituted")
    assert set(tmp_path.iterdir()) == before


def test_public_cli_uses_the_same_export_capture_and_preserves_its_limits(tmp_path):
    archive, _ = library(tmp_path)
    env = dict(os.environ, PYTHONPATH=os.pathsep.join(str(Path(p).resolve()) for p in sys.path if p))
    completed = subprocess.run([sys.executable, "-B", "-m", "sdk.ecosystem.manager", "list-source-exports",
                                str(archive.path), "--origin", archive.origin, "--staging-parent", str(tmp_path)],
                               capture_output=True, text=True, encoding="utf-8", env=env, check=False)
    assert completed.returncode == 0, completed.stderr
    result = json.loads(completed.stdout)
    with capture_source_library(archive, tmp_path) as captured:
        assert result["exports"][0]["reference"] == captured.exports[0].reference.encode()
        assert result["dependency_lock"] == captured.lock
        assert result["validated_manifest"] == captured.manifest
    assert result["installed"] is False and result["frontend_invoked"] is False and result["execution_qualified"] is False


def test_cli_never_emits_a_success_receipt_before_staging_cleanup(tmp_path, monkeypatch, capsys):
    from sdk.ecosystem import source_library, manager
    archive, _ = library(tmp_path)
    actual = source_library.stage_archive

    @contextmanager
    def cleanup_failure(*args, **kwargs):
        with actual(*args, **kwargs) as captured:
            yield captured
        raise PermissionError("Injected failure at the real cleanup boundary")

    monkeypatch.setattr(source_library, "stage_archive", cleanup_failure)
    code = manager.source_library_command([str(archive.path), "--origin", archive.origin,
                                           "--staging-parent", str(tmp_path)])
    assert code == 2
    lines = capsys.readouterr().out.splitlines()
    assert len(lines) == 1
    result = json.loads(lines[0])
    assert result["status"] == "error" and result["diagnostic"]["code"] == "library.stage_io"
    assert "exports" not in result
