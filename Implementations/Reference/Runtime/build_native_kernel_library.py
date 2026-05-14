#!/usr/bin/env python3
"""Build a shared native kernel library from a published LLVM kernel artifact."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]


EXAMPLE_MANIFESTS = {
    "05": ROOT / "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json",
    "5": ROOT / "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json",
    "06": ROOT / "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json",
    "6": ROOT / "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json",
    "07": ROOT / "Implementations/Reference/LLVM/examples/07_string_value_roundtrip/native_kernel_manifest.json",
    "7": ROOT / "Implementations/Reference/LLVM/examples/07_string_value_roundtrip/native_kernel_manifest.json",
    "08": ROOT / "Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/native_kernel_manifest.json",
    "8": ROOT / "Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/native_kernel_manifest.json",
    "09": ROOT / "Implementations/Reference/LLVM/examples/09_path_value_roundtrip/native_kernel_manifest.json",
    "9": ROOT / "Implementations/Reference/LLVM/examples/09_path_value_roundtrip/native_kernel_manifest.json",
    "10": ROOT / "Implementations/Reference/LLVM/examples/10_button_press_to_boolean/native_kernel_manifest.json",
}


def shared_library_suffix() -> str:
    if sys.platform == "win32":
        return ".dll"
    if sys.platform == "darwin":
        return ".dylib"
    return ".so"


def default_output_path(example: str) -> Path:
    normalized = {
        "5": "05",
        "05": "05",
        "6": "06",
        "06": "06",
        "7": "07",
        "07": "07",
        "8": "08",
        "08": "08",
        "9": "09",
        "09": "09",
        "10": "10",
    }[example]
    return ROOT / "build" / "frog_native_kernels" / f"example{normalized}_kernel{shared_library_suffix()}"


def load_manifest(path: Path) -> dict:
    data = json.loads(path.read_text(encoding="utf-8"))
    if data.get("artifact_kind") != "frog_native_kernel_manifest":
        raise RuntimeError(f"unexpected native kernel manifest kind in {path}")
    return data


def kernel_source_path(manifest: dict) -> Path:
    source = manifest["kernel"]["backend"]["source_artifact"]
    path = ROOT / source
    if not path.exists():
        raise RuntimeError(f"missing LLVM kernel artifact: {source}")
    return path


def build_native_kernel_library(
    *,
    manifest_path: Path,
    output_path: Path,
    clang: str = "clang",
) -> Path:
    if shutil.which(clang) is None:
        raise RuntimeError(f"required tool not found on PATH: {clang}")

    manifest = load_manifest(manifest_path)
    kernel_ll = kernel_source_path(manifest)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    command = [clang, "-shared", "-o", str(output_path), str(kernel_ll)]
    if sys.platform == "win32":
        command.insert(2, "-Wl,--export-all-symbols")

    print("$ " + " ".join(command))
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        raise RuntimeError(f"clang failed with exit {result.returncode}")
    return output_path


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Build a native kernel shared library from published kernel.ll.")
    parser.add_argument("--example", choices=sorted(EXAMPLE_MANIFESTS), required=True)
    parser.add_argument("--manifest", type=Path, default=None)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--clang", default="clang")
    args = parser.parse_args(argv)

    manifest_path = (args.manifest or EXAMPLE_MANIFESTS[args.example]).resolve()
    output_path = (args.output or default_output_path(args.example)).resolve()

    try:
        built = build_native_kernel_library(
            manifest_path=manifest_path,
            output_path=output_path,
            clang=args.clang,
        )
    except RuntimeError as exc:
        print(f"Native kernel library build: FAILED: {exc}", file=sys.stderr)
        return 1

    print(str(built.relative_to(ROOT)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
