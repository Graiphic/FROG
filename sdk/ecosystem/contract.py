"""P01 contract intake. No archive extraction, installation or FROG derivation.

Builder and manager consume this same validator. Semantic declarations are
references to their owning catalogue, never definitions in a package manifest.
"""
from __future__ import annotations

import copy
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
from typing import Any

from jsonschema import Draft202012Validator
from Implementations.Reference.common import FrogPipelineError, load_json_file

ROOT = Path(__file__).resolve().parent
SUFFIXES = {"library": ".bfrog", "project": ".pfrog", "example": ".efrog",
            "bridge": ".bridgefrog", "driver": ".driverfrog"}
MAX_MANIFEST_BYTES = 4 * 1024 * 1024


class ContractError(ValueError):
    def __init__(self, code: str, path: str, message: str):
        super().__init__(message)
        self.code, self.path, self.message = code, path, message

    def as_dict(self):
        return {"code": self.code, "path": self.path, "message": self.message}


def fail(code, path, message):
    raise ContractError(code, path, message)


def read_document(path: Path) -> dict[str, Any]:
    """Reuse the repository's strict JSON reader, with a manifest byte budget."""
    try:
        _, value = load_json_file(path, max_bytes=MAX_MANIFEST_BYTES)
        validate_text_tree(value)
        return value
    except FrogPipelineError as error:
        if error.error_code == "source_resource_limit":
            fail("contract.resource_limit", "/", error.message)
        fail("contract." + error.error_code, "/", error.message)
    except OSError as error:
        fail("contract.read_error", "/", str(error))


def validate_text_tree(value):
    """Check Unicode scalars and nesting before schemas or canonical encoding."""
    pending = [(value, 0)]
    while pending:
        item, depth = pending.pop()
        if depth > 64:
            fail("contract.resource_limit", "/", "Contract nesting exceeds 64 levels.")
        if isinstance(item, str):
            if any(0xD800 <= ord(char) <= 0xDFFF for char in item):
                fail("contract.invalid_unicode", "/", "Contract contains an isolated Unicode surrogate.")
        elif isinstance(item, dict):
            pending.extend((child, depth + 1) for pair in item.items() for child in pair)
        elif isinstance(item, list):
            pending.extend((child, depth + 1) for child in item)


def validate_schema(value, name):
    validate_text_tree(value)
    schema = json.loads((ROOT / "schemas" / (name + ".schema.json")).read_text(encoding="utf-8"))
    errors = sorted(Draft202012Validator(schema).iter_errors(value),
                    key=lambda error: (tuple(map(str, error.absolute_path)), error.message))
    if errors:
        error = errors[0]
        pointer = "/" + "/".join(str(x).replace("~", "~0").replace("/", "~1")
                                   for x in error.absolute_path)
        fail("contract.schema", pointer, error.message)


def unique(items, key, path):
    seen = set()
    for index, item in enumerate(items):
        identity = key(item)
        if identity in seen:
            fail("contract.duplicate_identity", f"{path}/{index}", f"Duplicate identity: {identity}")
        seen.add(identity)


def safe_path(value: str, pointer: str) -> str:
    # This is a manifest namespace check, not an archive extractor or symlink check.
    parts = value.split("/")
    reserved = re.compile(r"^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\.|$)", re.I)
    if (PurePosixPath(value).is_absolute() or any(c in value for c in '\\:*?"<>|')
            or any(ord(c) < 32 for c in value)
            or any(p in ("", ".", "..") or p.endswith((".", " ")) or reserved.match(p) for p in parts)):
        fail("contract.path", pointer, "Expected a portable relative package path.")
    return value


def version_key(text):
    # Numeric core only here: range endpoints must be stable versions. P03 owns
    # resolution and any future prerelease policy, not the package intake.
    if "-" in text or "+" in text:
        fail("contract.version_constraint", "/dependencies", "Range versions must be stable numeric cores in 0.1.")
    return tuple(int(part) for part in text.split("."))


def validate_manifest(value: dict, suffix: str) -> dict:
    if not isinstance(value, dict):
        fail("contract.schema", "/", "Expected a manifest object.")
    if value.get("format_version") != "0.1":
        fail("contract.format_version", "/format_version", "Unsupported distribution format version.")
    validate_schema(value, "manifest")
    if SUFFIXES[value["profile"]] != suffix.lower():
        fail("contract.suffix_profile", "/profile", "Suffix and manifest profile disagree.")
    unique(value["files"], lambda f: f["path"].casefold(), "/files")
    unique(value["contributions"], lambda c: c["id"], "/contributions")
    unique(value["dependencies"], lambda d: d["id"], "/dependencies")
    unique(value["dependencies"], lambda d: (d["identity"]["publisher"], d["identity"]["package"]), "/dependencies")
    unique(value.get("targets", []), lambda t: t["id"], "/targets")
    inventory = {}
    total = 0
    for index, file in enumerate(value["files"]):
        safe_path(file["path"], f"/files/{index}/path")
        inventory[file["path"]] = file
        total += file["size"]
    if total > 1024 * 1024 * 1024:
        fail("contract.resource_limit", "/files", "Declared expanded package exceeds 1 GiB.")
    roles = {file["role"] for file in value["files"]}
    mode = value["content_mode"]
    if mode == "source" and ("artifact" in roles or "targets" in value):
        fail("contract.content_mode", "/content_mode", "Source-only packages cannot declare compiled artifacts or targets.")
    if mode in ("compiled", "mixed") and "artifact" not in roles:
        fail("contract.content_mode", "/content_mode", "Compiled payload requires an artifact entry.")
    if mode == "compiled" and "source" in roles:
        fail("contract.content_mode", "/content_mode", "Compiled-only packages cannot declare source entries; use mixed.")
    if mode == "mixed" and "source" not in roles:
        fail("contract.content_mode", "/content_mode", "Mixed payload requires both source and compiled artifact entries.")

    def require_file(path, pointer, expected_hash=None):
        safe_path(path, pointer)
        if path not in inventory:
            fail("contract.missing_inventory", pointer, "Referenced file is absent from the inventory.")
        if expected_hash is not None and inventory[path]["sha256"] != expected_hash:
            fail("contract.reference_hash", pointer, "Reference and inventory hashes disagree.")

    require_file(value["license"]["path"], "/license/path")
    if "entry_point" in value:
        require_file(value["entry_point"], "/entry_point")
        expected = ".frprj" if value["profile"] == "project" else ".frog"
        if value["profile"] in ("project", "example") and not value["entry_point"].lower().endswith(expected):
            fail("contract.entry_profile", "/entry_point", f"Expected a {expected} entry point.")
    for index, contribution in enumerate(value["contributions"]):
        pointer = f"/contributions/{index}"
        require_file(contribution["payload"], pointer + "/payload")
        ref = contribution["contract"]
        require_file(ref["path"], pointer + "/contract/path", ref["sha256"])
        if value["profile"] in ("project", "example") and contribution["kind"] in ("bridge", "driver", "palette"):
            fail("contract.contribution_profile", pointer, "A working-copy profile cannot install contributions.")
        if contribution["kind"] in ("bridge", "driver") and contribution["kind"] != value["profile"]:
            fail("contract.contribution_profile", pointer, "Technical contributions require their dedicated profile.")
    for index, dep in enumerate(value["dependencies"]):
        constraint = dep["constraint"]
        if version_key(constraint["minimum"]) >= version_key(constraint["maximum_exclusive"]):
            fail("contract.version_constraint", f"/dependencies/{index}/constraint", "Empty version interval.")
        requested = version_key(dep["identity"]["version"])
        if not version_key(constraint["minimum"]) <= requested < version_key(constraint["maximum_exclusive"]):
            fail("contract.version_constraint", f"/dependencies/{index}/identity/version", "Declared dependency version is outside its requested interval.")
    for index, target in enumerate(value.get("targets", [])):
        require_file(target["artifact"], f"/targets/{index}/artifact", target["sha256"])
        # Schema admission requires exactly one provenance reference; only the
        # bridge profile can identify an independently compiled native binary.
        provenance = "build_provenance" if "build_provenance" in target else "frontend_provenance"
        require_file(target[provenance], f"/targets/{index}/{provenance}")
        if inventory[target["artifact"]]["role"] != "artifact":
            fail("contract.content_mode", f"/targets/{index}/artifact", "Target must refer to a compiled artifact.")
    return copy.deepcopy(value)


def validate_context(context: dict, capabilities: dict) -> dict:
    validate_schema(context, "frontend-context")
    validate_schema(capabilities, "frontend-capabilities")
    unique(context["profiles"], lambda p: p["id"], "/profiles")
    unique(capabilities["profiles"], lambda p: (p["id"], p["version"]), "/profiles")
    unique(context["dependencies"], lambda d: d["id"], "/dependencies")
    for field in ("source", "fir"):
        requested = {key: context[field][key] for key in ("family", "version")}
        if requested not in capabilities[field + "_formats"]:
            fail("contract." + field + "_version", "/" + field, "Unsupported format family/version.")
    if context["frontend"]["api_version"] not in capabilities["frontend_apis"]:
        fail("contract.frontend_api", "/frontend/api_version", "Unsupported Frontend API.")
    # Software version is provenance, never a substitute for format admission.
    available = {(p["id"], p["version"]): set(p["operations"]) for p in capabilities["profiles"]}
    operations = set()
    for index, profile in enumerate(context["profiles"]):
        key = (profile["id"], profile["version"])
        if key not in available:
            fail("contract.profile", f"/profiles/{index}", "Unknown semantic profile/version.")
        operations.update(available[key])
    for index, operation in enumerate(context["operations"]):
        if operation not in operations:
            fail("contract.operation", f"/operations/{index}", "Operation absent from the selected semantic profiles.")
    return copy.deepcopy(context)


def negotiate_bridge(context: dict, descriptor: dict, capabilities: dict) -> dict:
    # An explicit modern document selects its own contract. Historical context
    # validation and S083 negotiation below retain their original 0.1 behavior.
    if isinstance(context, dict) and context.get("schema_version") == "0.3":
        return _negotiate_bridge_api3(context, descriptor, capabilities)
    validate_context(context, capabilities)
    if not isinstance(descriptor, dict):
        fail("contract.schema", "/", "Expected a bridge descriptor object.")
    if descriptor.get("protocol_version") != "0.1":
        fail("contract.bridge_protocol", "/protocol_version", "Unsupported bridge protocol.")
    validate_schema(descriptor, "bridge-describe")
    unique(descriptor["profiles"], lambda p: (p["id"], p["version"]), "/profiles")
    if context["fir"] not in descriptor["fir_formats"]:
        fail("contract.bridge_fir", "/fir_formats", "Bridge does not admit the requested FIR format.")
    if any(p not in descriptor["profiles"] for p in context["profiles"]):
        fail("contract.bridge_profile", "/profiles", "Bridge does not admit all required profiles.")
    if not set(context["operations"]) <= set(descriptor["operations"]):
        fail("contract.bridge_operation", "/operations", "Bridge does not admit all required operations.")
    if "describe" not in descriptor["methods"] or "validate" not in descriptor["methods"]:
        fail("contract.bridge_methods", "/methods", "Describe and validate are mandatory.")
    return copy.deepcopy(descriptor)


def _bridge_profile_api3(identity: str, index: int) -> dict[str, str]:
    """Keep the complete owner ID; only its own .vN/@N suffix is a version."""
    separator = identity.rfind("@")
    start = separator + 1
    if separator < 0:
        separator = identity.rfind(".v")
        start = separator + 2
    version = identity[start:] if separator >= 0 else ""
    if not re.fullmatch(r"[0-9]+", version):
        fail("contract.bridge_profile_version", f"/profiles/{index}",
             "Frontend profile requires its own explicit .vN or @N version.")
    return {"id": identity, "version": version}


def _negotiate_bridge_api3(context: dict, descriptor: dict, capabilities: dict) -> dict:
    """Discovery compatibility only, never target admission or a Run receipt.

    Provenance 0.3, capabilities 0.2, Frontend API 3, FIR format and bridge wire
    0.1 have independent identities. Complete-graph bridge validate is mandatory
    even when every discovery list happens to contain the graph's declarations.
    """
    validate_schema(context, "frontend-context-0.3")
    validate_schema(capabilities, "frontend-capabilities-0.2")
    if (context["frontend"]["api_version"] != "3"
            or capabilities["frontend"]["api_version"] != "3"
            or "3" not in capabilities["frontend_apis"]):
        fail("contract.frontend_api", "/frontend/api_version",
             "Modern bridge discovery requires explicit Frontend API 3 support.")
    for field in ("source", "fir"):
        # FIR provenance also owns document_id. Format negotiation compares
        # only family/version and preserves that independent identity in input.
        requested = {key: context[field][key] for key in ("family", "version")}
        if requested not in capabilities[field + "_formats"]:
            fail("contract." + field + "_version", "/" + field,
                 "Unsupported format family/version.")
    if not context["closure_complete"]:
        fail("contract.incomplete_closure", "/closure_complete",
             "A complete Frontend dependency closure is required before discovery.")
    # Capabilities 0.2 explicitly requires profiles_exhaustive=false. It is not
    # an operation catalogue and cannot disprove an owner's selected profile.
    required_profiles = [_bridge_profile_api3(identity, index)
                         for index, identity in enumerate(context["profiles"])]
    if not isinstance(descriptor, dict):
        fail("contract.schema", "/", "Expected a bridge descriptor object.")
    if descriptor.get("protocol_version") != "0.1":
        fail("contract.bridge_protocol", "/protocol_version", "Unsupported bridge protocol.")
    validate_schema(descriptor, "bridge-describe-0.2")
    unique(descriptor["profiles"], lambda p: (p["id"], p["version"]), "/profiles")
    unique(descriptor["targets"], lambda target: target["id"], "/targets")
    requested_fir = {key: context["fir"][key] for key in ("family", "version")}
    if requested_fir not in descriptor["fir_formats"]:
        fail("contract.bridge_fir", "/fir_formats", "Bridge does not admit the requested FIR format.")
    if any(profile not in descriptor["profiles"] for profile in required_profiles):
        fail("contract.bridge_profile", "/profiles",
             "Bridge does not declare all required independently versioned profiles.")
    if (descriptor["operations_exhaustive"]
            and not set(context["operations"]) <= set(descriptor["operations"])):
        fail("contract.bridge_operation", "/operations",
             "Bridge's exhaustive declaration omits required operations.")
    if "describe" not in descriptor["methods"] or "validate" not in descriptor["methods"]:
        fail("contract.bridge_methods", "/methods", "Describe and complete-graph validate are mandatory.")
    # A nonexhaustive operation list defers admission to validate. Neither the
    # missing nor the present entries prove that the real target accepts FIR.
    return copy.deepcopy(descriptor)


def context_fingerprint(context: dict, capabilities: dict) -> str:
    result = validate_context(context, capabilities)
    for name in ("dependencies", "profiles"):
        result[name].sort(key=lambda item: item["id"])
    result["operations"].sort()
    encoded = json.dumps(result, sort_keys=True, separators=(",", ":"), ensure_ascii=False).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()


def validate_bridge_message(message: dict, *, request_id: str, session_id: str, generation: int) -> dict:
    validate_schema(message, "bridge-message")
    if (message["request_id"], message["session_id"], message["generation"]) != (request_id, session_id, generation):
        fail("contract.stale_message", "/generation", "Message does not belong to the expected request/session generation.")
    if message["kind"] == "progress" and message["progress"]["completed"] > message["progress"]["total"]:
        fail("contract.progress", "/progress", "Completed work exceeds the declared total.")
    return copy.deepcopy(message)


def inspect_manifest(path: Path, suffix: str) -> dict:
    return validate_manifest(read_document(path), suffix)
