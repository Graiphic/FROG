"""Pinned exports for the local source-document library profile.

This is package/reference I/O, not a FROG reader or a second signature/FIR
authority. A source-document export binds one manifest symbol to the document's
existing callable interface. The common Frontend must still admit that actual
interface and implementation. Compiled/mixed and separate API contracts are
outside this first host profile; the general archive contracts remain unchanged.
"""
from __future__ import annotations

from contextlib import contextmanager
from dataclasses import dataclass
import hashlib
from pathlib import Path
import re
from urllib.parse import parse_qsl, quote, unquote, urlencode, urlsplit

from sdk.ecosystem.archive import MANIFEST_ENTRY, _payload_path, stage_archive
from sdk.ecosystem.contract import fail
from sdk.ecosystem.library_resolution import LocalLibraryArchive, resolve_library_lock

PROFILE = "source-document@1"
MAX_SOURCE_BYTES = 8 * 1024 * 1024
_DIGEST = re.compile(r"[0-9a-f]{64}\Z")


@dataclass(frozen=True)
class SourceReference:
    publisher: str
    package: str
    version: str
    export_id: str
    symbol: str
    origin: str
    manifest_sha256: str
    contract_sha256: str

    def encode(self):
        path = "/".join(quote(x, safe="") for x in
                        (self.publisher, self.package, self.version, self.export_id))
        query = urlencode((("profile", PROFILE), ("symbol", self.symbol),
                           ("origin", self.origin), ("manifest", self.manifest_sha256),
                           ("contract", self.contract_sha256)))
        return "bfrog:///" + path + "?" + query


def parse_source_reference(value: str) -> SourceReference:
    if not isinstance(value, str):
        fail("library.reference_invalid", "/reference", "Expected a source library reference string.")
    try:
        length = len(value.encode("utf-8"))
    except UnicodeError:
        fail("library.reference_invalid", "/reference", "The source reference is not UTF-8 text.")
    if not value.startswith("bfrog:///") or length > 8192 or "\0" in value:
        fail("library.reference_invalid", "/reference", "Expected a bounded source library reference.")
    try:
        uri = urlsplit(value)
        parts = [unquote(x, errors="strict") for x in uri.path.removeprefix("/").split("/")]
        pairs = parse_qsl(uri.query, keep_blank_values=True, strict_parsing=True,
                         errors="strict", max_num_fields=5)
        query = dict(pairs)
    except (ValueError, UnicodeError):
        fail("library.reference_invalid", "/reference", "Malformed source library reference.")
    if uri.scheme != "bfrog" or uri.netloc or uri.fragment or len(parts) != 4 or any(not x for x in parts) or len(pairs) != len(query) or set(query) != {"profile", "symbol", "origin", "manifest", "contract"}:
        fail("library.reference_invalid", "/reference", "Unknown or ambiguous source reference fields.")
    if query["profile"] != PROFILE or not query["symbol"] or not 1 <= len(query["origin"]) <= 512 or not _DIGEST.fullmatch(query["manifest"]) or not _DIGEST.fullmatch(query["contract"]) or any(ord(c) < 32 or ord(c) == 127 for x in (*parts, query["symbol"], query["origin"]) for c in x):
        fail("library.reference_invalid", "/reference", "Unsupported profile or incomplete pinned export identity.")
    result = SourceReference(*parts, query["symbol"], query["origin"], query["manifest"], query["contract"])
    if result.encode() != value:
        fail("library.reference_invalid", "/reference", "The source reference is not in its canonical encoded form.")
    return result


@dataclass(frozen=True)
class SourceExport:
    reference: SourceReference
    payload: str
    size: int


@dataclass(frozen=True)
class SourceSnapshot:
    reference: SourceReference
    payload: str
    sha256: str
    bytes: bytes


@dataclass(frozen=True)
class SourceLibraryCapture:
    directory: Path
    manifest: dict
    manifest_sha256: str
    origin: str
    lock: dict
    exports: tuple[SourceExport, ...]

    def receipt(self):
        """Host capture metadata; not a semantic admission or trust decision."""
        return {"schema": "graiphic.manager.source_exports", "version": "0.1",
                "identity": self.manifest["identity"], "origin": self.origin,
                "manifest_sha256": self.manifest_sha256, "validated_manifest": self.manifest,
                "dependency_lock": self.lock,
                "exports": [{"id": x.reference.export_id, "symbol": x.reference.symbol,
                             "reference": x.reference.encode(), "payload": x.payload,
                             "source_sha256": x.reference.contract_sha256, "size": x.size}
                            for x in self.exports],
                "installed": False, "frontend_invoked": False, "execution_qualified": False}

    def resolve(self, reference: str) -> SourceSnapshot:
        wanted = parse_source_reference(reference)
        identity = self.exports[0].reference
        if (wanted.publisher, wanted.package, wanted.version) != (identity.publisher, identity.package, identity.version):
            fail("library.reference_identity", "/reference", "The selected package/version differs from the pinned reference.")
        if wanted.origin != self.origin:
            fail("library.origin_mismatch", "/reference/origin", "The selected local origin differs from the pinned reference.")
        if wanted.manifest_sha256 != self.manifest_sha256:
            fail("library.manifest_changed", "/reference/manifest", "Package bytes differ from the pinned manifest; no replacement is selected.")
        export = next((x for x in self.exports if x.reference.export_id == wanted.export_id), None)
        if export is None:
            fail("library.export_missing", "/reference/export", "The declared export is absent from this exact package.")
        if wanted.symbol != export.reference.symbol:
            fail("library.symbol_mismatch", "/reference/symbol", "The declared symbol differs from the selected export.")
        if wanted.contract_sha256 != export.reference.contract_sha256:
            fail("library.contract_changed", "/reference/contract", "The callable document differs from the pinned contract.")
        # This is the verified codec's private staging directory. Recheck the
        # exact selected bytes before passing them to the common Frontend/host.
        with _payload_path(self.directory, export.payload).open("rb") as stream:
            data = stream.read(MAX_SOURCE_BYTES + 1)
        if len(data) != export.size or hashlib.sha256(data).hexdigest() != wanted.contract_sha256:
            fail("library.payload_changed", "/files", "The staged callable source changed after capture.")
        return SourceSnapshot(wanted, export.payload, wanted.contract_sha256, data)


@contextmanager
def capture_source_library(root: LocalLibraryArchive, staging_parent: Path,
                           candidates=(), *, phase="build", expected_lock=None, cancelled=None):
    if not 1 <= len(root.origin) <= 512 or any(ord(c) < 32 or ord(c) == 127 for c in root.origin):
        fail("library.origin", "/origin", "An explicit local origin is required.")
    with stage_archive(root.path, staging_parent, cancelled) as (directory, manifest):
        if manifest["profile"] != "library" or manifest["content_mode"] != "source":
            fail("library.source_profile_unsupported", "/content_mode", "This host profile accepts source-document libraries only; compiled/mixed are not silently substituted.")
        digest = hashlib.sha256((directory / MANIFEST_ENTRY).read_bytes()).hexdigest()
        lock = resolve_library_lock(root, candidates, phase, expected=expected_lock, cancelled=cancelled)
        if lock["root"]["manifest_sha256"] != digest:
            fail("library.capture_changed", "/archive", "The archive changed between staging and dependency resolution.")
        inventory = {x["path"]: x for x in manifest["files"]}
        identity = manifest["identity"]
        exports = []
        symbols = set()
        for index, entry in enumerate(manifest["contributions"]):
            pointer = f"/contributions/{index}"
            contract, payload = entry["contract"], entry["payload"]
            file = inventory[payload]
            if entry["kind"] != "export" or contract["path"] != payload or file["role"] != "source" or not payload.lower().endswith(".frog"):
                fail("library.source_profile_unsupported", pointer, "This source-document profile binds each export to its existing .frog interface and source; other contributions/API layouts remain unsupported.")
            if file["size"] > MAX_SOURCE_BYTES:
                fail("library.source_limit", pointer + "/payload", "The source exceeds the common Frontend source-byte budget.")
            if any(ord(c) < 32 or ord(c) == 127 for c in contract["symbol"]):
                fail("library.source_profile_unsupported", pointer + "/contract/symbol", "A local document symbol cannot contain control characters.")
            if contract["symbol"] in symbols:
                fail("library.symbol_ambiguous", pointer + "/contract/symbol", "A symbol cannot identify two document entries in one source library.")
            symbols.add(contract["symbol"])
            reference = SourceReference(identity["publisher"], identity["package"], identity["version"],
                                        entry["id"], contract["symbol"], root.origin, digest, contract["sha256"])
            exports.append(SourceExport(reference, payload, file["size"]))
        if not exports:
            fail("library.export_missing", "/contributions", "A callable source-document library requires an explicit export.")
        yield SourceLibraryCapture(directory, manifest, digest, root.origin, lock, tuple(exports))
