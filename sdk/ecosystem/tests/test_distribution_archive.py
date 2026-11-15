"""P02 archive codec coverage. These payloads do not claim executable capability."""
import hashlib
import json
from pathlib import Path
import stat
import struct
import subprocess
import sys
from zipfile import ZIP_DEFLATED, ZipFile, ZipInfo

import pytest

from sdk.ecosystem.archive import build_archive, inspect_archive, stage_archive
from sdk.ecosystem import archive as codec
from sdk.ecosystem.contract import ContractError, SUFFIXES

ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ("library-source", "library-compiled", "library-mixed", "project", "example", "bridge", "driver")


def payload(tmp_path, fixture):
    source = tmp_path / "payload"
    manifest = json.loads((ROOT / "fixtures" / (fixture + ".json")).read_text(encoding="utf-8"))
    hashes = {}
    for item in manifest["files"]:
        content = ("Test payload for " + item["path"] + "\n").encode()
        file = source / item["path"]
        file.parent.mkdir(parents=True, exist_ok=True)
        file.write_bytes(content)
        item["size"] = len(content)
        item["sha256"] = hashes[item["path"]] = hashlib.sha256(content).hexdigest()
    for contribution in manifest["contributions"]:
        contract = contribution["contract"]
        contract["sha256"] = hashes[contract["path"]]
    for target in manifest.get("targets", []):
        target["sha256"] = hashes[target["artifact"]]
    descriptor = tmp_path / "manifest.json"
    descriptor.write_text(json.dumps(manifest), encoding="utf-8")
    return source, manifest, descriptor


@pytest.mark.parametrize("fixture", FIXTURES)
def test_s005_all_profiles_round_trip_and_are_reproducible(tmp_path, fixture):
    source, manifest, descriptor = payload(tmp_path, fixture)
    suffix = SUFFIXES[manifest["profile"]]
    first, second = tmp_path / ("first" + suffix), tmp_path / ("second" + suffix)
    assert build_archive(descriptor, source, first) == manifest
    assert build_archive(descriptor, source, second) == manifest
    assert first.read_bytes() == second.read_bytes()
    assert inspect_archive(first) == manifest
    with ZipFile(first) as archive:
        assert archive.read("manifest.json") == descriptor.read_bytes()
        for file in manifest["files"]:
            assert archive.read(file["path"]) == (source / file["path"]).read_bytes()


@pytest.mark.parametrize(("defect", "code"), [
    ("hash", "archive.hash"), ("size", "archive.size"), ("extra", "archive.inventory"),
    ("missing", "archive.inventory"), ("traversal", "contract.path"),
    ("device", "archive.path"), ("short-alias", "archive.path"),
    ("alias", "archive.path_collision"), ("prefix", "archive.path_collision"),
    ("symlink", "archive.member_kind"), ("compressed", "archive.compression"),
    ("revision", "contract.format_version"), ("duplicate-json", "contract.duplicate_json_member"),
    ("suffix", "contract.suffix_profile"), ("truncated", "archive.invalid_zip"),
    ("hidden-member", "archive.inventory"), ("local-flags", "archive.invalid_zip"),
])
def test_s006_s079_invalid_archive_is_refused(tmp_path, defect, code):
    source, manifest, _ = payload(tmp_path, "library-source")
    output = tmp_path / ("invalid.pfrog" if defect == "suffix" else "invalid.bfrog")
    item = manifest["files"][0]
    if defect == "revision":
        manifest["format_version"] = "99"
    raw = json.dumps(manifest)
    if defect == "duplicate-json":
        raw = raw.replace('"schema":', '"schema":"duplicate","schema":', 1)
    with ZipFile(output, "w") as archive:
        archive.writestr("manifest.json", raw)
        for file in manifest["files"]:
            if file == item and defect == "missing":
                continue
            name = file["path"]
            data = (source / name).read_bytes()
            if file == item and defect == "hash":
                data = b"!" + data[1:]
            if file == item and defect == "size":
                data += b"!"
            archive.writestr(name, data)
        extra = {"extra": "unexpected.txt", "traversal": "../outside", "device": "COM¹.txt",
                 "short-alias": "LICENS~1.TXT", "alias": "SRC/extra.frog", "prefix": "src",
                 "symlink": "link", "compressed": "compressed"}.get(defect)
        if extra is not None:
            info = ZipInfo(extra)
            if defect == "symlink":
                info.create_system = 3
                info.external_attr = (stat.S_IFLNK | 0o777) << 16
            if defect == "compressed":
                info.compress_type = ZIP_DEFLATED
            archive.writestr(info, b"payload")
    if defect == "truncated":
        output.write_bytes(output.read_bytes()[:-5])
    if defect in ("hidden-member", "local-flags"):
        contents = bytearray(output.read_bytes())
        if defect == "hidden-member":
            end = contents.rindex(b"PK\x05\x06")
            start = struct.unpack_from("<I", contents, end + 16)[0]
            struct.pack_into("<I", contents, end + 16, start + 1)
            contents[start:start] = b"!"
        else:
            struct.pack_into("<H", contents, 6, 1)
        output.write_bytes(contents)
    before = output.read_bytes()
    with pytest.raises(ContractError) as rejected:
        inspect_archive(output)
    assert rejected.value.code == code
    assert output.read_bytes() == before
    assert not (tmp_path.parent / "outside").exists()


def test_failed_and_duplicate_assembly_preserve_previous_output(tmp_path):
    source, manifest, descriptor = payload(tmp_path, "library-source")
    output = tmp_path / "library.bfrog"
    build_archive(descriptor, source, output)
    initial = output.read_bytes()
    with pytest.raises(ContractError, match="already exists"):
        build_archive(descriptor, source, output)
    first = source / manifest["files"][0]["path"]
    first.write_bytes(b"!" + first.read_bytes()[1:])
    with pytest.raises(ContractError) as rejected:
        build_archive(descriptor, source, tmp_path / "new.bfrog")
    assert rejected.value.code == "archive.hash"
    assert output.read_bytes() == initial
    assert not (tmp_path / "new.bfrog").exists()
    assert not list(tmp_path.glob(".frog-distribution-*"))


def test_archive_directory_budget_is_checked_before_parsing(tmp_path, monkeypatch):
    source, _, descriptor = payload(tmp_path, "library-source")
    output = tmp_path / "library.bfrog"
    build_archive(descriptor, source, output)
    monkeypatch.setattr(codec, "MAX_CENTRAL_BYTES", 1)

    def unexpected(*args, **kwargs):
        raise AssertionError("ZipFile reached before directory admission")

    monkeypatch.setattr(codec, "ZipFile", unexpected)
    with pytest.raises(ContractError) as rejected:
        inspect_archive(output)
    assert rejected.value.code == "archive.resource_limit"


def test_verified_staging_and_interruption_leave_the_host_root_clean(tmp_path):
    source, manifest, descriptor = payload(tmp_path, "library-source")
    output = tmp_path / "library.bfrog"
    build_archive(descriptor, source, output)
    parent = tmp_path / "staging"
    parent.mkdir()
    with stage_archive(output, parent) as (staging, inspected):
        assert inspected == manifest
        assert (staging / "manifest.json").read_bytes() == descriptor.read_bytes()
        for file in manifest["files"]:
            assert (staging / file["path"]).read_bytes() == (source / file["path"]).read_bytes()
    assert list(parent.iterdir()) == []

    # The first member was written when the following cancellation is observed.
    checks = iter([False, False, False, False, True])
    with pytest.raises(ContractError) as rejected:
        with stage_archive(output, parent, cancelled=lambda: next(checks)):
            pytest.fail("An interrupted extraction must never reach the transaction")
    assert rejected.value.code == "archive.cancelled"
    assert list(parent.iterdir()) == []


def test_cli_build_and_inspect_share_the_manifest_without_granting_trust(tmp_path):
    source, manifest, descriptor = payload(tmp_path, "library-source")
    output = tmp_path / "Unicode é and spaces.bfrog"

    def command(module, *arguments):
        result = subprocess.run(
            [sys.executable, "-B", "-m", module, *map(str, arguments)],
            cwd=ROOT.parents[1], capture_output=True, text=True, check=True,
        )
        return json.loads(result.stdout)

    built = command("sdk.ecosystem.builder", descriptor, "--payload-root", source, "--output", output)
    inspected = command("sdk.ecosystem.manager", output, "--archive")
    assert built["archive_written"] and not built["executable_build_performed"]
    assert inspected["payload_integrity_verified"]
    assert not any(inspected[key] for key in ("installed", "trusted", "authorized", "ready_to_execute"))
    assert built["validated_manifest"] == inspected["validated_manifest"] == manifest
