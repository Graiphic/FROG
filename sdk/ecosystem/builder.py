"""Read a P01 manifest and produce the future builder's immutable input plan.

This command does not yet construct a distribution archive or compile FROG.
"""
from __future__ import annotations
import argparse
import json
from pathlib import Path
from sdk.ecosystem.contract import ContractError, inspect_manifest


def prepare_build(manifest_path: Path, suffix: str) -> dict:
    value = inspect_manifest(manifest_path, suffix)
    return {"schema": "graiphic.builder.input-plan", "version": "0.1",
            "identity": value["identity"], "profile": value["profile"],
            "content_mode": value["content_mode"], "files": value["files"],
            "contributions": value["contributions"], "dependencies": value["dependencies"],
            "targets": value.get("targets", []), "license": value["license"],
            "required_rights": value["required_rights"], "validated_manifest": value,
            "executable_build_performed": False}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--suffix", required=True)
    args = parser.parse_args()
    try:
        print(json.dumps(prepare_build(args.manifest, args.suffix), sort_keys=True))
        return 0
    except ContractError as error:
        print(json.dumps({"status": "error", "diagnostic": error.as_dict()}))
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
