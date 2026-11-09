"""Prepare one local directory payload using the existing distribution contract.

No machine paths, compiler installation or author authorization enter this
publisher payload. The product installer owns those host-bound decisions.
This does not implement a new archive format or install anything.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import shutil
from sdk.ecosystem.contract import validate_manifest, validate_schema


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bridge", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--compiler-sha256", required=True)
    parser.add_argument("--compiler-version", required=True)
    args = parser.parse_args()
    here = Path(__file__).resolve().parents[1]
    if args.output.exists():
        raise ValueError("A new output directory is required; no package is overwritten.")
    profile = json.loads((here / "profile.json").read_text(encoding="utf-8"))
    validate_schema(profile, "bridge-describe-0.2")
    args.output.mkdir(parents=True)
    executable = "frog-reference-c-bridge.exe"
    shutil.copyfile(args.bridge, args.output / executable)
    shutil.copyfile(here / "profile.json", args.output / "profile.json")
    # Do not assign a blanket license to the shared extracted Frontend bodies.
    (args.output / "LICENSE.txt").write_text(
        "Local qualification only. NOASSERTION for redistribution. Preserve the public SDK and Frontend notices and their source-origin/license inventory.\n",
        encoding="utf-8")
    def digest(path): return hashlib.sha256(path.read_bytes()).hexdigest()
    if len(args.compiler_sha256) != 64 or any(c not in "0123456789abcdef" for c in args.compiler_sha256) or not args.compiler_version.startswith("19.44."):
        raise ValueError("The actual MSVC19.44 compiler identity is required.")
    spec = here.parents[2]
    frontend = spec / "Implementations/Frontend"
    origins = json.loads((frontend / "source-origin.json").read_text(encoding="utf-8"))
    frontend_identity = "".join(f"{row['path']}:{digest(frontend / row['path'])}\n" for row in origins["sources"])
    for name in ("src/FrontendApi.cpp", "src/FrontendOperation.cpp", "CMakeLists.txt", "production-sources.cmake"):
        frontend_identity += f"{name}:{digest(frontend / name)}\n"
    source_files = [here / "CMakeLists.txt", here / "profile.json", spec / "sdk/ecosystem/CMakeLists.txt"]
    for folder in (here / "src", spec / "sdk/ecosystem/src", spec / "sdk/ecosystem/include"):
        source_files += [p for p in folder.rglob("*") if p.is_file()]
    provenance = {"schema": "frog.bridge.local-build-provenance", "schema_version": "0.1",
                  "realization": "public-cxx-sdk-consumer", "artifact_sha256": digest(args.output / executable),
                  "compiler": {"family": "msvc", "version": args.compiler_version, "sha256": args.compiler_sha256},
                  "frontend_source_content_sha256": hashlib.sha256(frontend_identity.encode("utf-8")).hexdigest(),
                  "source_inputs": [{"path": p.relative_to(spec).as_posix(), "sha256": digest(p)} for p in sorted(source_files)],
                  "attribution": "Input declaration tied to the artifact; exact configure/build commands are retained by the qualification owner. This is not frog-to-FIR output provenance."}
    (args.output / "build-provenance.json").write_text(json.dumps(provenance, sort_keys=True, indent=2) + "\n", encoding="utf-8")
    template = {"schema": "frog.bridge.registration", "schema_version": "0.1",
                "origin": "local-development:org.graiphic.local/reference-c-bridge",
                "description": profile,
                "executable": {"path": executable, "sha256": digest(args.output / executable)},
                "arguments": ["--compiler-config", "compiler-config.json"],
                "dependencies": [{"path": "profile.json", "sha256": digest(args.output / "profile.json"),
                                  "scope": scope, "identity": "public-profile"} for scope in ("run", "build", "test")]}
    (args.output / "registration-template.json").write_text(json.dumps(template, ensure_ascii=False, sort_keys=True, indent=2) + "\n", encoding="utf-8")
    inventory = [{"path": p.name, "sha256": digest(p), "size": p.stat().st_size,
                  "role": "artifact" if p.name == executable else "license" if p.name == "LICENSE.txt" else "contract"}
                 for p in sorted(args.output.iterdir())]
    manifest = {"schema": "graiphic.distribution", "format_version": "0.1", "profile": "bridge",
                "identity": {"publisher": "org.graiphic.local", "package": "reference-c-bridge", "version": "0.1.0"},
                "content_mode": "compiled", "files": inventory,
                "contributions": [{"id": "graiphic.reference-c", "kind": "bridge",
                                   "contract": {"path": "profile.json", "sha256": digest(args.output / "profile.json"), "symbol": "graiphic.reference-c.finite@1"},
                                   "payload": executable}], "dependencies": [],
                "targets": [{"id": "windows-x64", "abi": "frog.bridge.ipc/0.1;windows-x64",
                             "artifact": executable, "sha256": digest(args.output / executable),
                             "build_provenance": "build-provenance.json"}],
                "license": {"expression": "NOASSERTION", "path": "LICENSE.txt"}, "required_rights": ["build", "run"]}
    validate_manifest(manifest, ".bridgefrog")
    path = args.output / "distribution.json"
    path.write_text(json.dumps(manifest, ensure_ascii=False, sort_keys=True, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"manifest": str(path), "sha256": digest(path), "profile": "bridge", "installed": False,
                      "trusted": False, "authorized": False, "host_configuration_in_payload": False}))


if __name__ == "__main__":
    main()
