"""Bounded distribution ZIP codec for the P01 manifest.

Inspection checks integrity without loading or extracting a payload. Package
trust, dependency resolution and installation belong to their own services.
"""
from __future__ import annotations

from contextlib import contextmanager, nullcontext
import hashlib
import os
from pathlib import Path
import re
import stat
import struct
import tempfile
import unicodedata
from zipfile import BadZipFile, ZIP_STORED, ZipFile, ZipInfo

from sdk.ecosystem.contract import (
    MAX_MANIFEST_BYTES, fail, read_manifest_bytes, safe_path, validate_manifest,
)

MANIFEST_ENTRY = "manifest.json"
MAX_ARCHIVE_BYTES = 1024**3 + 16 * 1024**2
MAX_CENTRAL_BYTES = 8 * 1024**2
MAX_MEMBERS = 4097
MAX_DEPTH = 32
CHUNK_BYTES = 1024**2
END_RECORD = struct.Struct("<4s4H2LH")
LOCAL_HEADER = struct.Struct("<4s5H3L2H")
DOS_DEVICE = re.compile(
    r"^(?:CON|PRN|AUX|NUL|CONIN\$|CONOUT\$|(?:COM|LPT)[1-9¹²³])(?:\.|$)",
    re.IGNORECASE,
)
SHORT_ALIAS = re.compile(r"^[^.]{1,6}~[0-9]+(?:\.[^.]{1,3})?$", re.IGNORECASE)


def _inventory(names):
    """Reject ambiguous paths before looking at or moving any payload bytes."""
    paths = {}
    leaves = set()
    for name in names:
        safe_path(name, "/archive")
        parts = name.split("/")
        if (len(parts) > MAX_DEPTH or name != unicodedata.normalize("NFC", name)
                or any(DOS_DEVICE.match(part.partition(".")[0].rstrip(" ")) or SHORT_ALIAS.fullmatch(part)
                       for part in parts)):
            fail("archive.path", "/archive", f"Unsupported member path: {name}")
        leaf = name.casefold()
        if leaf in paths:
            fail("archive.path_collision", "/archive", f"Ambiguous member path: {name}")
        for length in range(1, len(parts) + 1):
            prefix = "/".join(parts[:length])
            key = prefix.casefold()
            if key in leaves or paths.setdefault(key, prefix) != prefix:
                fail("archive.path_collision", "/archive", f"Ambiguous member path: {name}")
        leaves.add(leaf)


def _check_directory(stream):
    # Bound the central directory before ZipFile reads it. This profile uses
    # classic single-volume ZIP and has no need for ZIP64.
    size = stream.seek(0, os.SEEK_END)
    if size > MAX_ARCHIVE_BYTES:
        fail("archive.resource_limit", "/archive", "ZIP exceeds the archive byte limit.")
    tail_start = max(0, size - END_RECORD.size - 65535)
    stream.seek(tail_start)
    tail = stream.read()
    marker = tail.rfind(b"PK\x05\x06")
    if marker < 0 or marker + END_RECORD.size > len(tail):
        fail("archive.invalid_zip", "/archive", "ZIP end record is missing or truncated.")
    _, disk, central_disk, disk_count, count, length, offset, comment = END_RECORD.unpack_from(tail, marker)
    if (disk or central_disk or disk_count != count or
            marker + END_RECORD.size + comment != len(tail) or offset + length != tail_start + marker):
        fail("archive.invalid_zip", "/archive", "Expected a single classic ZIP archive.")
    if count > MAX_MEMBERS or length > MAX_CENTRAL_BYTES:
        fail("archive.resource_limit", "/archive", "ZIP directory exceeds its limits.")
    return count, offset


@contextmanager
def _archive_errors():
    try:
        yield
    except (BadZipFile, EOFError, NotImplementedError, UnicodeError) as error:
        fail("archive.invalid_zip", "/archive", str(error))
    except OSError as error:
        fail("archive.io", "/archive", str(error))


def _check_member(info):
    unix_kind = stat.S_IFMT(info.external_attr >> 16) if info.create_system == 3 else 0
    if (info.orig_filename != info.filename or info.is_dir() or unix_kind not in (0, stat.S_IFREG)
            or info.external_attr & 0x418):
        fail("archive.member_kind", "/archive", f"Expected a regular file: {info.filename}")
    if info.flag_bits & ~0x800 or info.compress_type != ZIP_STORED or info.extract_version >= 45:
        fail("archive.compression", "/archive", f"Expected a stored classic ZIP member: {info.filename}")
    if info.file_size != info.compress_size:
        fail("archive.size", "/archive", f"Stored member sizes disagree: {info.filename}")


def _check_payload_layout(stream, directory_offset, members):
    # Every byte before the central directory must belong to a declared member.
    # Use ZipFile for member contents and CRC, and constrain this stored profile
    # to matching local headers without data descriptors or opaque gaps.
    cursor = 0
    for info in sorted(members, key=lambda entry: entry.header_offset):
        if cursor != info.header_offset:
            fail("archive.inventory", "/archive", "Unexpected bytes between ZIP members.")
        stream.seek(cursor)
        raw = stream.read(LOCAL_HEADER.size)
        if len(raw) != LOCAL_HEADER.size:
            fail("archive.invalid_zip", "/archive", "Truncated ZIP local header.")
        signature, version, flags, compression, _, _, crc, stored, expanded, name, extra = LOCAL_HEADER.unpack(raw)
        if (signature != b"PK\x03\x04" or version != info.extract_version
                or flags != info.flag_bits or compression != info.compress_type or crc != info.CRC
                or stored != info.compress_size or expanded != info.file_size or extra or info.extra):
            fail("archive.invalid_zip", "/archive", f"Inconsistent ZIP header: {info.filename}")
        cursor += LOCAL_HEADER.size + name + stored
    if cursor != directory_offset:
        fail("archive.inventory", "/archive", "Unexpected bytes before the ZIP directory.")


def _verify_payload(source, item, destination=None, cancelled=None):
    digest = hashlib.sha256()
    remaining = item["size"]
    while remaining:
        _check_cancelled(cancelled)
        block = source.read(min(remaining, CHUNK_BYTES))
        if not block:
            break
        remaining -= len(block)
        digest.update(block)
        if destination is not None:
            destination.write(block)
    if remaining or source.read(1):
        fail("archive.size", "/files", f"Size differs from inventory: {item['path']}")
    if digest.hexdigest() != item["sha256"]:
        fail("archive.hash", "/files", f"SHA-256 differs from inventory: {item['path']}")


def _check_cancelled(cancelled):
    if cancelled is not None and cancelled():
        fail("archive.cancelled", "/archive", "Archive staging was cancelled.")


def _read_archive(path: Path, staging: Path | None = None, cancelled=None) -> tuple[dict, str]:
    _check_cancelled(cancelled)
    with _archive_errors(), path.open("rb") as stream:
        count, directory_offset = _check_directory(stream)
        with ZipFile(stream) as archive:
            members = archive.infolist()
            if len(members) != count:
                fail("archive.inventory", "/archive", "ZIP entry count disagrees with its directory.")
            _inventory(info.orig_filename for info in members)
            for info in members:
                _check_member(info)
            _check_payload_layout(stream, directory_offset, members)
            by_name = {info.filename: info for info in members}
            manifest_entry = by_name.pop(MANIFEST_ENTRY, None)
            if manifest_entry is None:
                fail("archive.manifest_missing", "/archive", "Missing root manifest.json.")
            if manifest_entry.file_size > MAX_MANIFEST_BYTES:
                fail("contract.resource_limit", "/", "Manifest exceeds the configured byte limit.")
            with archive.open(manifest_entry) as source:
                raw = source.read(MAX_MANIFEST_BYTES + 1)
            manifest = validate_manifest(read_manifest_bytes(raw, str(path)), path.suffix)
            files = {item["path"]: item for item in manifest["files"]}
            if files.keys() != by_name.keys():
                fail("archive.inventory", "/files", "ZIP entries differ from the declared inventory.")
            if staging is not None:
                (staging / MANIFEST_ENTRY).write_bytes(raw)
            for name, item in files.items():
                _check_cancelled(cancelled)
                member = by_name[name]
                if member.file_size != item["size"]:
                    fail("archive.size", "/files", f"Size differs from inventory: {name}")
                destination = nullcontext()
                if staging is not None:
                    output = staging / name
                    output.parent.mkdir(parents=True, exist_ok=True)
                    destination = output.open("xb")
                with destination as target, archive.open(member) as source:
                    _verify_payload(source, item, target, cancelled)
            _check_cancelled(cancelled)
            return manifest, hashlib.sha256(raw).hexdigest()


def inspect_archive(path: Path) -> dict:
    """Return the exact admitted manifest after checking all declared bytes."""
    manifest, _ = _read_archive(path)
    return manifest


def inspect_archive_with_digest(path: Path, cancelled=None) -> tuple[dict, str]:
    """Verify the archive and identify the exact manifest that commits its payload."""
    return _read_archive(path, cancelled=cancelled)


@contextmanager
def stage_archive(path: Path, staging_parent: Path, cancelled=None):
    """Provide verified files in a temporary directory under a host-owned root.

    The host must hand the verified bytes to its transaction before leaving
    this context. No destination is activated and no files are retained here.
    """
    _check_cancelled(cancelled)
    with tempfile.TemporaryDirectory(prefix="frog-archive-", dir=staging_parent) as directory:
        staging = Path(directory)
        manifest, _ = _read_archive(path, staging, cancelled)
        yield staging, manifest


def _payload_path(root: Path, name: str) -> Path:
    path = root
    parts = name.split("/")
    for index, component in enumerate(parts):
        path = path / component
        metadata = path.lstat()
        kind = stat.S_ISREG if index == len(parts) - 1 else stat.S_ISDIR
        if not kind(metadata.st_mode) or getattr(metadata, "st_file_attributes", 0) & 0x400:
            fail("archive.member_kind", "/files", f"Expected a regular payload below its root: {name}")
    return path


def _entry(name):
    entry = ZipInfo(name)
    entry.create_system = 3
    entry.external_attr = (stat.S_IFREG | 0o644) << 16
    entry.compress_type = ZIP_STORED
    return entry


def build_archive(manifest_path: Path, payload_root: Path, output: Path) -> dict:
    """Write a reproducible verified ZIP, refusing to replace an existing output."""
    with _archive_errors(), manifest_path.open("rb") as stream:
        raw = stream.read(MAX_MANIFEST_BYTES + 1)
        manifest = validate_manifest(read_manifest_bytes(raw, str(manifest_path)), output.suffix)
        files = sorted(manifest["files"], key=lambda item: item["path"])
        _inventory([MANIFEST_ENTRY, *(item["path"] for item in files)])
        root = payload_root.resolve(strict=True)
        if not root.is_dir():
            fail("archive.payload_root", "/files", "Expected a payload directory.")
        descriptor, temporary = tempfile.mkstemp(prefix=".frog-distribution-", dir=output.parent)
        try:
            with os.fdopen(descriptor, "w+b") as target:
                with ZipFile(target, "w", allowZip64=False) as archive:
                    archive.writestr(_entry(MANIFEST_ENTRY), raw)
                    for item in files:
                        path = _payload_path(root, item["path"])
                        with path.open("rb") as source, archive.open(_entry(item["path"]), "w") as member:
                            _verify_payload(source, item, member)
                target.flush()
                _check_directory(target)
                os.fsync(target.fileno())
            os.link(temporary, output)
        except FileExistsError:
            fail("archive.output_exists", "/archive", "The output file already exists.")
        finally:
            os.unlink(temporary)
    return manifest
