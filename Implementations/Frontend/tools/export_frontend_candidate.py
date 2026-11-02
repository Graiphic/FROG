#!/usr/bin/env python3
"""Capture/verify the existing E3 local-source-kit format; never build or publish.

The frontend_identity byte/order algorithm matches the common CMake owner,
including the CLI implementation in the component's content identity.
This bounded source exporter adds no archive, compiler, registry or installer.
"""
from __future__ import annotations
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path, PurePosixPath
import re
import stat
import sys

sys.dont_write_bytecode = True
FRONT = "Implementations/Frontend"
ALLOWED = {".cpp", ".hpp", ".h", ".inc", ".json", ".md", ".cmake", ".py", ".frog", ".c", ".txt"}
EXCLUDED = {".git", ".codex", ".agents", "__pycache__", ".pytest_cache", ".cache", "bin", "env", ".venv", "venv", "archives", "artifacts", "cmakefiles"}
GENERATED = {"evidence/frontend-closure.json", "evidence/studio-frontend-pin.json"}
MAX_FILES, MAX_FILE, MAX_TOTAL = 8192, 64*1024*1024, 512*1024*1024


class ExportError(ValueError):
    pass


def require(value, message):
    if not value:
        raise ExportError(message)


def digest(raw):
    return hashlib.sha256(raw).hexdigest()


def encoded(value):
    return (json.dumps(value, ensure_ascii=False, indent=2) + "\n").encode("utf-8")


def unique(pairs):
    out = {}
    for key, value in pairs:
        require(key not in out, "Duplicate JSON member: " + key)
        out[key] = value
    return out


def document(raw):
    return json.loads(raw.decode("utf-8"), object_pairs_hook=unique,
                      parse_constant=lambda token: (_ for _ in ()).throw(ExportError("Non-JSON number: " + token)))


def relative_name(name):
    require(type(name) is str and name and len(name) < 1024 and not any(c in name for c in "\\:\0\r\n"), "Invalid portable kit path")
    path = PurePosixPath(name)
    require(not path.is_absolute() and path.as_posix() == name and all(p not in (".", "..") and not p.endswith((".", " ")) for p in path.parts), "Escaping/noncanonical kit path: " + name)
    devices = {"con", "prn", "aux", "nul"} | {prefix+str(i) for prefix in ("com", "lpt") for i in range(1, 10)}
    require(not any(p.split(".", 1)[0].casefold() in devices for p in path.parts), "Reserved device kit path: " + name)
    require(not any(p.casefold() in {"frog-studio", "frog-runtime"} for p in path.parts), "Private owner in kit path: " + name)
    return path


def ignored(part):
    """Excluded directory name, including generated build trees."""
    part = part.casefold()
    return part in EXCLUDED or part.startswith("build") or part.startswith(".build")


def approved(name):
    path = relative_name(name)
    return (not any(ignored(p) for p in path.parts[:-1])
            and path.name.casefold() not in EXCLUDED
            and (path.suffix.lower() in ALLOWED or path.name in ("LICENSE", "NOTICE", "CMakeLists.txt")))


def permitted(name):
    relative_name(name)
    if name in ("LICENSE", "NOTICE") or name in GENERATED:
        return True
    prefixes = (FRONT + "/", "IR/schema/", "IR/docs/", "sdk/ecosystem/docs/", "sdk/ecosystem/schemas/", "sdk/ecosystem/fixtures/")
    if name.startswith(prefixes):
        return approved(name)
    if name.startswith("IR/") and len(PurePosixPath(name).parts) == 2:
        return PurePosixPath(name).suffix.lower() in (".md", ".txt")
    return name in ("sdk/ecosystem/tests/validate_frontend_results.py", "sdk/ecosystem/tests/test_frontend_documents.py")


def portable_owner(name):
    return "generated read-only closure evidence" if name.startswith("evidence/") else "FROG/" + (FRONT + "/NOTICE" if name == "NOTICE" else name)


def no_link(path):
    info = path.lstat()
    require(not stat.S_ISLNK(info.st_mode) and not getattr(info, "st_file_attributes", 0) & 0x400,
            "Links/reparse points are outside the source-kit profile: " + path.name)


def tree_files(root):
    no_link(root)
    require(root.is_dir(), "Required source directory is absent: " + root.name)
    pending = [root]
    while pending:
        for path in sorted(pending.pop().iterdir()):
            no_link(path)
            if path.is_dir():
                if not ignored(path.name):
                    pending.append(path)
            elif path.is_file() and path.name.casefold() not in EXCLUDED:
                yield path


def read(path):
    no_link(path)
    require(path.is_file() and path.stat().st_size <= MAX_FILE, "Missing/oversized source-kit input: " + path.name)
    raw = path.read_bytes()
    require(len(raw) <= MAX_FILE, "Input grew beyond its source-kit budget")
    return raw


def frontend_identity(root):
    """Exact FrontendInputs.cmake record order and SHA256 definition."""
    origins = document(read(root / "source-origin.json"))
    records, seen = "", set()
    for entry in origins["sources"]:
        name = entry["path"]
        relative_name(name)
        require(name.casefold() not in seen, "Duplicate source-origin identity: " + name)
        seen.add(name.casefold())
        actual = digest(read(root / name))
        if entry["ownership"] == "snapshot":
            require(actual == entry["sha256"], "Declaration snapshot drift: " + name)
        records += name + ":" + actual + "\n"
    for name in ("src/FrontendApi.cpp", "src/FrontendOperation.cpp", "src/FrontendCli.cpp", "CMakeLists.txt", "production-sources.cmake"):
        records += name + ":" + digest(read(root / name)) + "\n"
    return digest(records.encode("utf-8"))


def implementation(root):
    cmake = read(root / "CMakeLists.txt").decode("utf-8-sig")
    def setting(name):
        values = re.findall(r'\bset\(\s*' + name + r'\s+"([^"\r\n]+)"\s*\)', cmake)
        require(len(values) == 1, "Missing/ambiguous Frontend implementation identity: " + name)
        return values[0]
    header = read(root / "snapshot/api/frog/frontend.h").decode("utf-8-sig")
    api = re.findall(r'^#define FROG_FRONTEND_API_VERSION UINT32_C\(([0-9]+)\)$', header, re.M)
    require(len(api) == 1, "Missing exact Frontend API declaration")
    return {"software_version": setting("FROG_FRONTEND_SOFTWARE_VERSION"), "api_version": int(api[0]),
            "base_revision": setting("FROG_FRONTEND_BASE_REVISION"), "content_sha256": frontend_identity(root)}


def exact_pin(pin, actual):
    require(pin.get("format") == "frog.studio.frontend-pin" and pin.get("version") == 1, "Unsupported Studio Frontend pin")
    for key in ("software_version", "api_version", "base_revision", "content_sha256"):
        require(pin.get(key) == actual[key], "Studio Frontend pin differs: " + key)


def source_selection(frog):
    selected = {}
    def add(name, path):
        require(permitted(name), "Unapproved source-kit path: " + name)
        require(name.casefold() not in {n.casefold() for n in selected}, "Case-colliding source-kit paths: " + name)
        selected[name] = path
    for prefix in (FRONT, "IR/schema", "sdk/ecosystem/docs", "sdk/ecosystem/schemas", "sdk/ecosystem/fixtures"):
        for path in tree_files(frog / prefix):
            name = path.relative_to(frog).as_posix()
            if approved(name):
                add(name, path)
    if (frog / "IR/docs").is_dir():
        for path in tree_files(frog / "IR/docs"):
            name = path.relative_to(frog).as_posix()
            if approved(name):
                add(name, path)
    for path in sorted((frog / "IR").iterdir()):
        no_link(path)
        if path.is_file() and path.suffix.lower() in (".md", ".txt"):
            add(path.relative_to(frog).as_posix(), path)
    for name in ("LICENSE", "sdk/ecosystem/tests/validate_frontend_results.py", "sdk/ecosystem/tests/test_frontend_documents.py"):
        add(name, frog / name)
    add("NOTICE", frog / FRONT / "NOTICE")
    return selected


def check_inventory(front, frontend_names):
    inventory = document(read(front / "docs/license-inventory.json"))
    require(inventory.get("schemaVersion") == 3 and inventory.get("redistributionRightsGrantedForSelectedGraiphicOriginals") is True,
            "The final accepted-scope licence inventory is required")
    entries = {}
    for entry in inventory["files"]:
        if entry.get("root") != "frontend":
            continue
        name = entry["path"]
        relative_name(name)
        require(name not in entries, "Duplicate licence-inventory input: " + name)
        entries[name] = entry
    require(set(entries) == set(frontend_names), "Final licence inventory does not cover the exact selected Frontend files")
    for name, entry in entries.items():
        if name == "docs/license-inventory.json":
            require(entry.get("current_hash") is None and entry.get("current_bytes") is None and entry.get("hash_exclusion"),
                    "Licence inventory must exclude its own hash/size without a cycle")
        else:
            raw = read(front / name)
            require(entry.get("current_hash") == digest(raw) and entry.get("current_bytes") == len(raw), "Licence inventory is stale: " + name)
    return inventory


def audit_closure(front):
    # Import the existing reviewed checker beside this exporter, not code from
    # an unverified input kit. It performs literal read-only source inspection.
    path = Path(__file__).with_name("audit_frontend_closure.py")
    spec = importlib.util.spec_from_file_location("frog_candidate_existing_closure", path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    report = module.audit(front.resolve(), front.resolve() / "production-sources.cmake", [],
                          ["src/FrontendApi.cpp", "src/FrontendOperation.cpp", "src/FrontendCli.cpp"])
    require(report["outcome"] == "pass", "Existing Frontend closure audit refused: " + json.dumps(report["issues"]))
    return report


def verify(kit, studio_pin=None):
    no_link(Path(kit).absolute())
    kit = Path(kit).resolve(strict=True)
    no_link(kit)
    # Reject every link before opening any manifest-controlled relative input.
    actual, pending, directories = set(), [kit], 0
    while pending:
        directories += 1
        require(directories <= MAX_FILES*4, "Kit directory budget exceeded")
        for path in pending.pop().iterdir():
            no_link(path)
            if path.is_dir():
                pending.append(path)
            elif path.is_file():
                actual.add(path.relative_to(kit).as_posix())
                require(len(actual) <= MAX_FILES+1, "Kit file budget exceeded")
    manifest = document(read(kit / "source-kit.json"))
    require(manifest.get("format") == "frog.frontend.local-source-kit" and manifest.get("version") == 1 and
            manifest.get("manifest_self_excluded") is True, "Unsupported local-source-kit manifest")
    records, folded = {}, set()
    for entry in manifest["files"]:
        name = entry["path"]
        require(permitted(name) and name.casefold() not in folded, "Unknown/escaping/case-colliding manifest input: " + name)
        require(entry.get("owner") == portable_owner(name), "Nonportable or unknown source owner: " + name)
        folded.add(name.casefold())
        raw = read(kit / name)
        require(len(raw) == entry["bytes"] and digest(raw) == entry["sha256"], "Kit input differs from manifest: " + name)
        records[name] = entry
    require(len(records) <= MAX_FILES and sum(row["bytes"] for row in records.values()) <= MAX_TOTAL, "Kit exceeds its file/byte budget")
    # Includes excluded build/cache files too: this is a pristine source kit.
    require(actual == set(records) | {"source-kit.json"}, "Kit file closure differs: missing or unlisted inputs")
    front = kit / FRONT
    identity = implementation(front)
    require(identity["content_sha256"] == manifest["frontend_content_sha256"] and identity["api_version"] == manifest["api_version"] and
            identity["software_version"] == manifest["software_version"] and identity["base_revision"] == manifest["frontend_base_revision"],
            "Copied Frontend identity differs from its capture")
    origins = document(read(front / "source-origin.json"))
    require(all(FRONT + "/" + row["path"] in records for row in origins["sources"]), "A required production-origin input is absent")
    check_inventory(front, [name[len(FRONT)+1:] for name in records if name.startswith(FRONT + "/")])
    captured_pin = manifest.get("expected_studio_pin")
    if captured_pin is not None:
        exact_pin(captured_pin, identity)
        require(document(read(kit / "evidence/studio-frontend-pin.json")) == captured_pin, "Captured Studio pin differs from its exact expected value")
    if studio_pin is not None:
        supplied_pin = document(read(Path(studio_pin)))
        exact_pin(supplied_pin, identity)
        if captured_pin is not None:
            require(supplied_pin == captured_pin, "Expected Studio pin differs from the exact captured pin")
    report = audit_closure(front)
    require(document(read(kit / "evidence/frontend-closure.json")) == report, "Captured closure evidence differs from the current exact kit closure")
    return {"format": "frog.frontend.local-kit-verification", "version": 1, "status": "verified",
            "files": len(records), "frontend_content_sha256": identity["content_sha256"],
            "manifest_sha256": digest(read(kit / "source-kit.json")), "build_or_test_executed": False}


def export(frog, output, studio_pin=None):
    frog, output = Path(frog).resolve(strict=True), Path(output).absolute()
    require(not output.exists() and not output.is_symlink(), "Output must be a new path; no kit is overwritten")
    require(not output.resolve().is_relative_to(frog), "Output must be outside the selected source owner")
    selected = source_selection(frog)
    require(len(selected) <= MAX_FILES, "Source selection exceeds its file budget")
    payloads, originals = {}, {}
    total = 0
    for name, path in sorted(selected.items()):
        raw = read(path)
        total += len(raw)
        require(total <= MAX_TOTAL, "Source selection exceeds its byte budget")
        payloads[name], originals[name] = raw, digest(raw)
    front = frog / FRONT
    identity = implementation(front)
    inventory = check_inventory(front, [name[len(FRONT)+1:] for name in selected if name.startswith(FRONT + "/")])
    pin_raw = read(Path(studio_pin)) if studio_pin is not None else None
    pin = document(pin_raw) if pin_raw is not None else None
    if pin is not None:
        exact_pin(pin, identity)
        payloads["evidence/studio-frontend-pin.json"] = pin_raw
    closure = audit_closure(front)
    payloads["evidence/frontend-closure.json"] = encoded(closure)
    require(len(payloads) <= MAX_FILES and sum(map(len, payloads.values())) <= MAX_TOTAL, "Complete capture exceeds its file/byte budget")
    require(set(source_selection(frog)) == set(selected), "Source file selection changed during capture")
    for name, path in selected.items():
        require(digest(read(path)) == originals[name], "Source changed during capture: " + name)
    if pin_raw is not None:
        require(read(Path(studio_pin)) == pin_raw, "Studio pin changed during capture")
    output.mkdir(parents=True, exist_ok=False)
    records = []
    for name, raw in sorted(payloads.items()):
        target = output / name
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(raw)
        require(read(target) == raw, "Copied source bytes differ: " + name)
        owner = portable_owner(name)
        records.append({"path": name, "sha256": digest(raw), "bytes": len(raw), "owner": owner})
    for name, path in selected.items():
        require(digest(read(path)) == originals[name], "Source changed while copying: " + name)
    require(set(source_selection(frog)) == set(selected), "Source file selection changed while copying")
    if pin_raw is not None:
        require(read(Path(studio_pin)) == pin_raw, "Studio pin changed while copying")
    metadata = {"format": "frog.frontend.local-source-kit", "version": 1,
                "software_version": identity["software_version"], "api_version": identity["api_version"],
                "status": "candidate-source-captured-qualification-pending", "dissemination": "local-only",
                "standalone_reconstruction_passed": False, "installed_c_consumer_passed": False,
                "clean_windows_qualified": False, "remote_publication": False, "local_overlay_required": True,
                "redistributionRightsGrantedForSelectedGraiphicOriginals": inventory["redistributionRightsGrantedForSelectedGraiphicOriginals"],
                "thirdPartyContentRelicensed": False, "frontend_content_sha256": identity["content_sha256"],
                "frontend_base_revision": identity["base_revision"], "expected_studio_pin": pin,
                "source_capture_before_after_verified": True, "manifest_self_excluded": True, "files": records}
    (output / "source-kit.json").write_bytes(encoded(metadata))
    return verify(output, studio_pin)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--frog-root", type=Path, help="Public FROG source root, required for capture")
    modes = parser.add_mutually_exclusive_group(required=True)
    modes.add_argument("--output", type=Path, help="New source-kit directory, outside the source owner")
    modes.add_argument("--verify", type=Path, help="Verify a pristine captured kit's exact file closure")
    parser.add_argument("--studio-pin", type=Path, help="Optional exact existing Studio Frontend pin; never changed")
    args = parser.parse_args()
    try:
        if args.verify is not None:
            require(args.frog_root is None, "--verify reads the kit; it does not substitute current source files")
            result = verify(args.verify, args.studio_pin)
        else:
            require(args.frog_root is not None, "Capture requires --frog-root")
            result = export(args.frog_root, args.output, args.studio_pin)
        print(json.dumps(result, ensure_ascii=False))
        return 0
    except (ExportError, OSError, ValueError, KeyError, TypeError) as error:
        print(json.dumps({"status": "refused", "code": "frontend.candidate.capture_invalid", "message": str(error)}, ensure_ascii=False), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
