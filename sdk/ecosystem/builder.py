"""Plan distribution input or assemble an archive from existing payload files."""
from __future__ import annotations
import argparse
import json
from pathlib import Path
from sdk.ecosystem.archive import build_archive
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
    parser.add_argument("--suffix")
    parser.add_argument("--output", type=Path)
    parser.add_argument("--payload-root", type=Path)
    args = parser.parse_args()
    if args.output is not None:
        if args.payload_root is None or args.suffix is not None:
            parser.error("Archive assembly requires --output and --payload-root. The output name supplies the suffix.")
    elif args.suffix is None or args.payload_root is not None:
        parser.error("A plan requires --suffix.")
    try:
        if args.output is None:
            result = prepare_build(args.manifest, args.suffix)
        else:
            manifest = build_archive(args.manifest, args.payload_root, args.output)
            result = {"archive": str(args.output), "archive_written": True,
                      "executable_build_performed": False, "validated_manifest": manifest}
        print(json.dumps(result, sort_keys=True))
        return 0
    except ContractError as error:
        print(json.dumps({"status": "error", "diagnostic": error.as_dict()}))
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
