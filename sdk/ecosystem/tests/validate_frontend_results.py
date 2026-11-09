#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Validate real versioned Frontend result JSON against SDK/IR schemas, without deriving FIR.

Inputs are explicit output files from the actual library/CLI. No source file is
modified, frontend code executed, network schema fetched or artifact fabricated.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import sys

from jsonschema import Draft202012Validator
from referencing import Registry, Resource

SDK_ROOT = Path(__file__).resolve().parents[1]
FROG_ROOT = Path(__file__).resolve().parents[3]
SCHEMAS = {
    "diagnostics": "diagnostic-0.2",
    "provenance": "frontend-context-0.2",
    "descriptor": "frontend-capabilities-0.2",
    "source_maps": "frontend-source-maps",
}
SCHEMAS_BY_API = {
    "2": dict(SCHEMAS),
    "3": dict(SCHEMAS, provenance="frontend-context-0.3"),
}
DEFAULT_API = "3"


def schemas_for_api(expected_api: str) -> dict:
    if not isinstance(expected_api, str) or expected_api not in SCHEMAS_BY_API:
        raise ValueError("Unsupported inspection API version; select explicitly 2 or 3")
    return SCHEMAS_BY_API[expected_api]


def schema_registry() -> Registry:
    resources = []
    for directory in (SDK_ROOT / "schemas", FROG_ROOT / "IR/schema"):
        for path in sorted(directory.glob("*.schema.json")):
            schema = json.loads(path.read_text(encoding="utf-8"))
            resources.append((schema["$id"], Resource.from_contents(schema)))
    # Registry has no network retriever. Every referenced IR owner is local.
    return Registry().with_resources(resources)


def strict_json(raw: bytes):
    def pairs(items):
        result = {}
        for key, value in items:
            if key in result:
                raise ValueError("Duplicate JSON member: " + key)
            result[key] = value
        return result

    def nonfinite(value):
        raise ValueError("Non-finite JSON constant: " + value)

    value = json.loads(raw.decode("utf-8"), object_pairs_hook=pairs, parse_constant=nonfinite)
    pending = [value]
    while pending:
        item = pending.pop()
        if isinstance(item, str):
            if any(0xD800 <= ord(char) <= 0xDFFF for char in item):
                raise ValueError("Isolated Unicode surrogate in result JSON")
        elif isinstance(item, dict):
            pending.extend(item)
            pending.extend(item.values())
        elif isinstance(item, list):
            pending.extend(item)
    return value


def validate_document(kind: str, value, registry: Registry | None = None,
                      expected_api: str = DEFAULT_API) -> None:
    schemas = schemas_for_api(expected_api)
    if kind not in schemas:
        raise ValueError("Unknown Frontend result document kind: " + kind)
    schema_name = schemas[kind]
    schema = json.loads((SDK_ROOT / "schemas" / (schema_name + ".schema.json")).read_text(encoding="utf-8"))
    validator = Draft202012Validator(schema, registry=registry or schema_registry())
    if kind == "diagnostics":
        if not isinstance(value, list):
            raise ValueError("diagnostics_json must be a complete array, including when empty")
        for diagnostic in value:
            validator.validate(diagnostic)
            if diagnostic["category"] == "frontend-capability" and diagnostic["capability_id"] != diagnostic["code"]:
                raise ValueError("The emitted capability identity must equal its stable diagnostic code")
    else:
        validator.validate(value)
        if kind in ("provenance", "descriptor") and value["frontend"]["api_version"] != expected_api:
            raise ValueError("Result implementation API identity differs from the requested API check")
        if kind == "descriptor" and expected_api not in value["frontend_apis"]:
            raise ValueError("Descriptor does not declare the API it identifies")


def validate_documents(documents: dict, expected_api: str = DEFAULT_API) -> None:
    schemas_for_api(expected_api)
    registry = schema_registry()
    for kind, value in documents.items():
        validate_document(kind, value, registry, expected_api)
    provenance, maps = documents.get("provenance"), documents.get("source_maps")
    descriptor = documents.get("descriptor")
    if provenance is not None and descriptor is not None and provenance["frontend"] != descriptor["frontend"]:
        raise ValueError("Descriptor and provenance identify different Frontend implementations")
    if provenance is not None and maps is not None:
        if "authored_source_sha256" in maps and maps["authored_source_sha256"] != provenance["source"]["sha256"]:
            raise ValueError("Source maps and provenance identify different captured authored bytes")
        if "transformations" in maps and maps["transformations"] != provenance["transformations"]:
            raise ValueError("Source maps and provenance contain different transformation chains")
        if "fir_source_sha256" in maps:
            projected = provenance.get("projected_source_sha256", provenance["source"]["sha256"])
            if maps["fir_source_sha256"].removeprefix("sha256:") != projected:
                raise ValueError("FIR source identity differs from its declared authored/resolved projection")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    for kind in SCHEMAS:
        parser.add_argument("--" + kind.replace("_", "-"), type=Path)
    parser.add_argument("--expected-api", choices=tuple(SCHEMAS_BY_API), default=DEFAULT_API,
                        help="Versioned result API to inspect (3 current; 2 historical, never auto-detected)")
    parser.add_argument("--max-document-bytes", type=int, default=64 * 1024 * 1024,
                        help="Read guard for this inspection tool, not an admission limit of the frontend")
    args = parser.parse_args()
    documents, evidence = {}, []
    try:
        if args.max_document_bytes < 1:
            raise ValueError("The inspection byte ceiling must be positive")
        for kind in SCHEMAS:
            path = getattr(args, kind)
            if path is None:
                continue
            with path.open("rb") as stream:
                raw = stream.read(args.max_document_bytes + 1)
            if len(raw) > args.max_document_bytes:
                raise ValueError("Inspection byte ceiling exceeded for " + path.name)
            documents[kind] = strict_json(raw)
            evidence.append(dict(kind=kind, file=path.name, sha256=hashlib.sha256(raw).hexdigest()))
        if not documents:
            raise ValueError("Supply at least one actual result document")
        validate_documents(documents, args.expected_api)
        print(json.dumps(dict(outcome="pass", documents=evidence, api=args.expected_api,
                              frontend_invoked=False, compilation="NOT_RUN"), indent=2))
        return 0
    except Exception as error:
        # Validation failures are reports from this inspection tool, not
        # machine diagnostics attributed to the language/frontend producer.
        print(json.dumps(dict(outcome="fail", message=str(error)), ensure_ascii=True), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
