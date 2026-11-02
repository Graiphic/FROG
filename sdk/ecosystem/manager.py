"""Inspect the common manifest; explicit local bridge actions delegate to the product transaction owner."""
from __future__ import annotations
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path
from sdk.ecosystem.contract import ContractError, inspect_manifest


def inspect_install(manifest_path: Path, suffix: str) -> dict:
    value = inspect_manifest(manifest_path, suffix)
    action = {"library": "plan_library_install", "project": "prepare_working_copy",
              "example": "preview_then_working_copy", "bridge": "plan_bridge_install",
              "driver": "plan_driver_prerequisites"}[value["profile"]]
    return {"schema": "graiphic.manager.intake", "version": "0.1",
            "identity": value["identity"], "profile": value["profile"], "next_action": action,
            "dependencies": value["dependencies"], "contributions": value["contributions"],
            "required_rights": value["required_rights"], "license": value["license"],
            "validated_manifest": value, "installed": False,
            "trusted": False, "authorized": False, "ready_to_execute": False}


def local_bridge_command(argv):
    parser = argparse.ArgumentParser(description="Explicit local-development bridge installation; no author trust is inferred.")
    parser.add_argument("action", choices=("install", "list", "remove", "recover"))
    parser.add_argument("--installer", type=Path, required=True)
    parser.add_argument("--installer-sha256", required=True)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--source", type=Path)
    parser.add_argument("--compiler-config", type=Path)
    parser.add_argument("--package", type=Path)
    parser.add_argument("--choice", choices=("keep", "restore"))
    parser.add_argument("--authorize-local-development", action="store_true")
    args = parser.parse_args(argv)
    if not args.installer.is_absolute() or not args.root.is_absolute():
        raise ValueError("Installer and installation root must be explicit absolute paths.")
    if hashlib.sha256(args.installer.read_bytes()).hexdigest() != args.installer_sha256.lower():
        raise ValueError("The selected native installer differs from its expected digest.")
    command = [str(args.installer), args.action, "--root", str(args.root)]
    if args.action == "install":
        if args.source is None or not args.source.is_absolute() or args.compiler_config is None or not args.compiler_config.is_absolute():
            raise ValueError("Install requires an explicit payload directory and local compiler configuration.")
        manifest = args.source / "distribution.json"
        inspected = inspect_install(manifest, ".bridgefrog")
        if inspected["profile"] != "bridge":
            raise ValueError("This command installs only the local bridge profile.")
        command += ["--source", str(args.source), "--manifest-sha256", hashlib.sha256(manifest.read_bytes()).hexdigest(),
                    "--compiler-config", str(args.compiler_config)]
        if args.authorize_local_development:
            command += ["--authorize-local-development"]
    elif args.action in ("remove", "recover"):
        if args.package is None or not args.package.is_absolute():
            raise ValueError("An exact absolute installed package path is required.")
        command += ["--package", str(args.package)]
        if args.action == "recover":
            if args.choice is None:
                raise ValueError("Recovery requires an explicit keep/restore choice.")
            command += ["--choice", args.choice]
    # No extraction, shell/PATH launch or second transaction implementation.
    # Native owner checks the exact manifest and payload again before commit.
    completed = subprocess.run(command, shell=False, check=False, capture_output=True,
                               encoding="utf-8", errors="strict", timeout=120)
    if len(completed.stdout.encode("utf-8")) > 1024 * 1024 or len(completed.stderr.encode("utf-8")) > 1024 * 1024:
        raise ValueError("Native installer output exceeded its local bound.")
    result = json.loads(completed.stdout)
    print(json.dumps(result, ensure_ascii=False, sort_keys=True))
    return completed.returncode


def main():
    if len(sys.argv) > 1 and sys.argv[1] in ("install", "list", "remove", "recover"):
        try:
            return local_bridge_command(sys.argv[1:])
        except (ValueError, OSError, subprocess.TimeoutExpired, ContractError) as error:
            diagnostic = error.as_dict() if isinstance(error, ContractError) else {"code": "manager.bridge.operation_failed", "message": str(error)}
            print(json.dumps({"status": "error", "diagnostic": diagnostic}, ensure_ascii=False))
            return 2
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--suffix", required=True)
    args = parser.parse_args()
    try:
        print(json.dumps(inspect_install(args.manifest, args.suffix), sort_keys=True))
        return 0
    except ContractError as error:
        print(json.dumps({"status": "error", "diagnostic": error.as_dict()}))
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
