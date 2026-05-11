#!/usr/bin/env python3
"""Build and test Python dynamic loading of LLVM-produced native kernels."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

from build_native_kernel_library import build_native_kernel_library, shared_library_suffix


ROOT = Path(__file__).resolve().parents[3]
BUILD_DIR = ROOT / "b" / "pyn"
EXAMPLE05_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json"
EXAMPLE06_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json"


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    print("$ " + " ".join(command))
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit {result.returncode}: {' '.join(command)}")
    return result


def main() -> int:
    try:
        library05 = build_native_kernel_library(
            manifest_path=EXAMPLE05_MANIFEST,
            output_path=BUILD_DIR / f"example05_kernel{shared_library_suffix()}",
        )
        library06 = build_native_kernel_library(
            manifest_path=EXAMPLE06_MANIFEST,
            output_path=BUILD_DIR / f"example06_kernel{shared_library_suffix()}",
        )

        result05 = run([
            sys.executable,
            "Implementations/Reference/Runtime/python/cli.py",
            "run",
            "6",
            "--native-kernel-manifest",
            str(EXAMPLE05_MANIFEST),
            "--native-kernel-library",
            str(library05),
        ])
        artifact05 = json.loads(result05.stdout)
        assert artifact05["outputs"]["public"]["result"] == 30
        assert artifact05["outputs"]["ui"]["ind_result"] == 30

        result06 = run([
            sys.executable,
            "Implementations/Reference/Runtime/python/cli.py",
            "run",
            "true",
            "--example",
            "06",
            "--native-kernel-manifest",
            str(EXAMPLE06_MANIFEST),
            "--native-kernel-library",
            str(library06),
        ])
        artifact06 = json.loads(result06.stdout)
        assert artifact06["outputs"]["public"]["result"] is True
        assert artifact06["outputs"]["ui"]["bool_result"] is True

        print("Python dynamic native kernel bridge check: ok")
        return 0
    except (RuntimeError, AssertionError, json.JSONDecodeError) as exc:
        print(f"Python dynamic native kernel bridge check: FAILED: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
