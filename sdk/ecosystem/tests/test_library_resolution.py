"""Offline resolution over actual ZIP files with P01-validated manifests."""
import hashlib
import json
from pathlib import Path
import subprocess
import sys

import pytest

from sdk.ecosystem.archive import build_archive
from sdk.ecosystem.contract import ContractError
from sdk.ecosystem.library_resolution import LocalLibraryArchive, resolve_library_lock


ORIGIN = "local:team"
PUBLISHER = "org.example.resolution"
FROG_ROOT = Path(__file__).resolve().parents[3]


def dependency(package, version="1.0.0", *, origin=ORIGIN, phases=("build", "run"),
               location="build", scope="project"):
    return {
        "id": package,
        "identity": {"publisher": PUBLISHER, "package": package, "version": version},
        "origin": origin,
        "constraint": {"minimum": "1.0.0", "maximum_exclusive": "3.0.0"},
        "consumption_phases": list(phases),
        "required_location": location,
        "installation_scope": scope,
    }


def library(tmp_path, name, version="1.0.0", *, deps=(), origin=ORIGIN, content=None):
    package = tmp_path / (name + "-" + version)
    package.mkdir()
    license_bytes = (FROG_ROOT / "LICENSE").read_bytes()
    source_bytes = content if content is not None else (
        FROG_ROOT / "Examples" / "01_pure_addition" / "main.frog"
    ).read_bytes()
    inventory = []
    for path, role, data in (("LICENSE", "license", license_bytes), ("add.frog", "source", source_bytes)):
        (package / path).write_bytes(data)
        inventory.append({"path": path, "size": len(data), "role": role,
                          "sha256": hashlib.sha256(data).hexdigest()})
    manifest = {
        "schema": "graiphic.distribution", "format_version": "0.1", "profile": "library",
        "identity": {"publisher": PUBLISHER, "package": name, "version": version},
        "content_mode": "source", "files": inventory, "contributions": [],
        "dependencies": list(deps), "license": {"expression": "Apache-2.0", "path": "LICENSE"},
        "required_rights": ["build"],
    }
    descriptor = package / "manifest.json"
    descriptor.write_text(json.dumps(manifest), encoding="utf-8")
    archive = package / (name + ".bfrog")
    build_archive(descriptor, package, archive)
    return LocalLibraryArchive(archive, origin)


def test_s007_s074_s075_exact_transitive_versions_can_coexist(tmp_path):
    root_a = library(tmp_path, "project-a", deps=[dependency("shared")])
    root_b = library(tmp_path, "project-b", deps=[dependency("shared", "2.0.0")])
    shared_one = library(tmp_path, "shared", deps=[dependency("leaf", location="target", scope="user")])
    shared_two = library(tmp_path, "shared", "2.0.0")
    leaf = library(tmp_path, "leaf")
    cache = [shared_one, shared_two, leaf]
    a = resolve_library_lock(root_a, cache, "run")
    b = resolve_library_lock(root_b, cache, "run")
    assert [(x["identity"]["package"], x["identity"]["version"]) for x in a["dependencies"]] == [
        ("leaf", "1.0.0"), ("shared", "1.0.0")]
    assert [(x["identity"]["package"], x["identity"]["version"]) for x in b["dependencies"]] == [
        ("shared", "2.0.0")]
    requirement = a["requirements"][1]["requirement"]
    assert requirement["required_location"] == "target" and requirement["installation_scope"] == "user"
    assert resolve_library_lock(root_a, reversed(cache), "run", expected=a) == a


def test_consumption_phase_does_not_confuse_location_or_scope(tmp_path):
    root = library(tmp_path, "root", deps=[dependency("shared", phases=("build",), location="target")])
    assert not resolve_library_lock(root, [], "run")["dependencies"]
    with pytest.raises(ContractError) as rejected:
        resolve_library_lock(root, [], "build")
    assert rejected.value.code == "library.missing_offline"


@pytest.mark.parametrize(("entry", "diagnostic"), [
    ("wrong-origin", "library.origin_mismatch"),
    ("wrong-version", "library.version_missing"),
    ("cycle", "library.cycle"),
    ("conflict", "library.conflict"),
    ("duplicate", "library.duplicate"),
])
def test_s007_offline_diagnostics(tmp_path, entry, diagnostic):
    root_deps = [dependency("shared")]
    shared_deps = []
    if entry == "cycle":
        shared_deps = [dependency("root")]
    if entry == "conflict":
        root_deps.append(dependency("other"))
    root = library(tmp_path, "root", deps=root_deps)
    origin = "local:wrong" if entry == "wrong-origin" else ORIGIN
    version = "2.0.0" if entry == "wrong-version" else "1.0.0"
    shared = library(tmp_path, "shared", version, origin=origin, deps=shared_deps)
    cache = [shared]
    if entry == "conflict":
        cache += [library(tmp_path, "other", deps=[dependency("shared", "2.0.0")]),
                  library(tmp_path, "shared", "2.0.0")]
    if entry == "duplicate":
        cache.append(shared)
    with pytest.raises(ContractError) as rejected:
        resolve_library_lock(root, cache, "run")
    assert rejected.value.code == diagnostic


def test_s074_same_version_changed_content_invalidates_lock(tmp_path):
    root = library(tmp_path, "root", deps=[dependency("shared")])
    shared = library(tmp_path, "shared")
    expected = resolve_library_lock(root, [shared], "run")
    replacement = tmp_path / "replacement"
    replacement.mkdir()
    changed = library(replacement, "shared", content=b"{}")
    assert resolve_library_lock(root, [changed], "run") != expected
    with pytest.raises(ContractError) as rejected:
        resolve_library_lock(root, [changed], "run", expected=expected)
    assert rejected.value.code == "library.lock_mismatch"


def test_resolution_can_be_cancelled_before_opening_an_archive(tmp_path):
    with pytest.raises(ContractError) as rejected:
        resolve_library_lock(LocalLibraryArchive(tmp_path / "absent.bfrog", ORIGIN), [],
                             "run", cancelled=lambda: True)
    assert rejected.value.code == "archive.cancelled"


def test_manager_round_trips_a_lock_without_writing_installation(tmp_path):
    root = library(tmp_path, "root", deps=[dependency("shared")])
    shared = library(tmp_path, "shared")
    command = [sys.executable, "-B", "-m", "sdk.ecosystem.manager", "resolve-library",
               str(root.path), "--origin", root.origin, "--phase", "run",
               "--candidate", shared.origin, str(shared.path)]
    initial = set(tmp_path.rglob("*"))
    resolved = subprocess.run(command, cwd=FROG_ROOT, capture_output=True, check=True)
    lock = tmp_path / "lock.json"
    lock.write_bytes(resolved.stdout)
    repeated = subprocess.run([*command, "--expect-lock", str(lock)], cwd=FROG_ROOT,
                              capture_output=True, check=True)
    assert repeated.stdout == resolved.stdout
    assert set(tmp_path.rglob("*")) == initial | {lock}
