"""Resolve exact library dependencies from an explicitly supplied offline cache."""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from sdk.ecosystem.archive import inspect_archive_with_digest
from sdk.ecosystem.contract import fail


PHASES = ("build", "run", "deploy", "debug", "test")
MAX_LIBRARIES = 256
MAX_EDGES = 4096


@dataclass(frozen=True)
class LocalLibraryArchive:
    path: Path
    origin: str


def _identity(manifest):
    identity = manifest["identity"]
    return identity["publisher"], identity["package"], identity["version"]


def _receipt(manifest, digest, origin):
    return {
        "identity": manifest["identity"],
        "origin": origin,
        "manifest_sha256": digest,
        "targets": manifest.get("targets", []),
    }


def _inspect(candidate, cancelled):
    if not 1 <= len(candidate.origin) <= 512:
        fail("library.origin", "/origin", "An explicit cache origin is required.")
    manifest, digest = inspect_archive_with_digest(candidate.path, cancelled)
    if manifest["profile"] != "library":
        fail("library.profile", "/profile", "Offline library resolution requires a .bfrog package.")
    return manifest, _receipt(manifest, digest, candidate.origin)


def resolve_library_lock(root, candidates, phase, *, expected=None, cancelled=None):
    """Verify a bounded offline closure and return a deterministic lock receipt.

    The 0.1 manifest names the exact requested version and admits a stable
    interval around it. Resolution preserves the requested version, origin,
    phase, location and scope. A host must supply the allowed origin of each
    local file. Integrity does not establish publisher trust or activation.
    """
    if phase not in PHASES:
        fail("library.phase", "/phase", "Unknown dependency consumption phase.")
    root_manifest, root_receipt = _inspect(root, cancelled)
    root_key = _identity(root_manifest)
    root_reference = (*root_key, root.origin)
    cache = {root_reference: (root_manifest, root_receipt)}
    known_identities = {root_key}
    known_packages = {root_key[:2]}
    for candidate in candidates:
        if len(cache) >= MAX_LIBRARIES:
            fail("library.limit", "/dependencies", "Offline library cache exceeds the package limit.")
        manifest, receipt = _inspect(candidate, cancelled)
        identity = _identity(manifest)
        key = (*identity, candidate.origin)
        if key in cache:
            fail("library.duplicate", "/dependencies", "Multiple cache entries declare " + " / ".join(key))
        cache[key] = manifest, receipt
        known_identities.add(identity)
        known_packages.add(identity[:2])

    selected = {}
    active = []
    edges = []

    def visit(key):
        if key in active:
            cycle = active[active.index(key):] + [key]
            fail("library.cycle", "/dependencies", " -> ".join("/".join(item) for item in cycle))
        previous = selected.get(key[:2])
        if previous == key:
            return
        if previous is not None:
            fail("library.conflict", "/dependencies",
                 f"Cannot select {' / '.join(key)} together with {' / '.join(previous)}.")
        selected[key[:2]] = key
        active.append(key)
        manifest, receipt = cache[key]
        for requirement in sorted(manifest["dependencies"], key=lambda entry: entry["id"]):
            if phase not in requirement["consumption_phases"]:
                continue
            if cancelled is not None and cancelled():
                fail("library.cancelled", "/dependencies", "Library resolution was cancelled.")
            if len(edges) >= MAX_EDGES:
                fail("library.limit", "/dependencies", "Offline library closure exceeds the edge limit.")
            identity = requirement["identity"]
            wanted = (identity["publisher"], identity["package"], identity["version"])
            dependency = (*wanted, requirement["origin"])
            if dependency not in cache:
                message = " / ".join(dependency)
                if wanted in known_identities:
                    fail("library.origin_mismatch", "/dependencies", "The exact version exists only at another origin: " + message)
                if wanted[:2] in known_packages:
                    fail("library.version_missing", "/dependencies", "The exact version is missing from the cache: " + message)
                fail("library.missing_offline", "/dependencies", "The dependency is absent from the offline cache: " + message)
            edges.append({"from": receipt["identity"], "requirement": requirement})
            visit(dependency)
        active.pop()

    visit(root_reference)
    lock = {
        "schema": "graiphic.library.offline_lock",
        "version": "0.1",
        "phase": phase,
        "root": root_receipt,
        "dependencies": [cache[key][1] for key in sorted(selected.values()) if key != root_reference],
        "requirements": edges,
    }
    if expected is not None and lock != expected:
        fail("library.lock_mismatch", "/lock", "Verified dependency closure differs from the expected lock.")
    return lock
