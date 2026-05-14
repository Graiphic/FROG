#!/usr/bin/env python3
"""Build and test Rust dynamic loading of LLVM-produced native kernels."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from pathlib import Path

from build_native_kernel_library import build_native_kernel_library, shared_library_suffix


ROOT = Path(__file__).resolve().parents[3]
BUILD_DIR = ROOT / "b" / "rsn"
RUST_DIR = ROOT / "Implementations/Reference/Runtime/rust"
EXAMPLE05_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json"
EXAMPLE06_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json"
EXAMPLE07_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/07_string_value_roundtrip/native_kernel_manifest.json"
EXAMPLE08_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/native_kernel_manifest.json"
EXAMPLE09_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/09_path_value_roundtrip/native_kernel_manifest.json"
EXAMPLE10_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/10_button_press_to_boolean/native_kernel_manifest.json"


def require_tool(name: str) -> None:
    if shutil.which(name) is None:
        raise RuntimeError(f"required tool not found on PATH: {name}")


def run(command: list[str], *, cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    print("$ " + " ".join(command))
    result = subprocess.run(command, cwd=cwd, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit {result.returncode}: {' '.join(command)}")
    return result


def main() -> int:
    try:
        require_tool("cargo")

        library05 = build_native_kernel_library(
            manifest_path=EXAMPLE05_MANIFEST,
            output_path=BUILD_DIR / f"example05_kernel{shared_library_suffix()}",
        )
        library06 = build_native_kernel_library(
            manifest_path=EXAMPLE06_MANIFEST,
            output_path=BUILD_DIR / f"example06_kernel{shared_library_suffix()}",
        )
        library07 = build_native_kernel_library(
            manifest_path=EXAMPLE07_MANIFEST,
            output_path=BUILD_DIR / f"example07_kernel{shared_library_suffix()}",
        )
        library08 = build_native_kernel_library(
            manifest_path=EXAMPLE08_MANIFEST,
            output_path=BUILD_DIR / f"example08_kernel{shared_library_suffix()}",
        )
        library09 = build_native_kernel_library(
            manifest_path=EXAMPLE09_MANIFEST,
            output_path=BUILD_DIR / f"example09_kernel{shared_library_suffix()}",
        )
        library10 = build_native_kernel_library(
            manifest_path=EXAMPLE10_MANIFEST,
            output_path=BUILD_DIR / f"example10_kernel{shared_library_suffix()}",
        )

        result05 = run([
            "cargo",
            "run",
            "--offline",
            "--",
            "run",
            "6",
            "--native-kernel-manifest",
            str(EXAMPLE05_MANIFEST),
            "--native-kernel-library",
            str(library05),
        ], cwd=RUST_DIR)
        artifact05 = json.loads(result05.stdout)
        assert artifact05["outputs"]["public"]["result"] == 30
        assert artifact05["outputs"]["ui"]["ind_result"] == 30

        result06 = run([
            "cargo",
            "run",
            "--offline",
            "--",
            "run",
            "true",
            "--example",
            "06",
            "--native-kernel-manifest",
            str(EXAMPLE06_MANIFEST),
            "--native-kernel-library",
            str(library06),
        ], cwd=RUST_DIR)
        artifact06 = json.loads(result06.stdout)
        assert artifact06["outputs"]["public"]["result"] is True
        assert artifact06["outputs"]["ui"]["bool_result"] is True

        result07 = run([
            "cargo",
            "run",
            "--offline",
            "--",
            "run",
            "hello world",
            "--example",
            "07",
            "--native-kernel-manifest",
            str(EXAMPLE07_MANIFEST),
            "--native-kernel-library",
            str(library07),
        ], cwd=RUST_DIR)
        artifact07 = json.loads(result07.stdout)
        assert artifact07["outputs"]["public"]["result_text"] == "hello world"
        assert artifact07["outputs"]["ui"]["str_result"] == "hello world"

        result08 = run([
            "cargo",
            "run",
            "--offline",
            "--",
            "run",
            "fault",
            "--example",
            "08",
            "--native-kernel-manifest",
            str(EXAMPLE08_MANIFEST),
            "--native-kernel-library",
            str(library08),
        ], cwd=RUST_DIR)
        artifact08 = json.loads(result08.stdout)
        assert artifact08["outputs"]["public"]["result_mode"] == "fault"
        assert artifact08["outputs"]["ui"]["mode_result"] == "fault"

        result09 = run([
            "cargo",
            "run",
            "--offline",
            "--",
            "run",
            "C:/FROG/from_rust_bridge.txt",
            "--example",
            "09",
            "--native-kernel-manifest",
            str(EXAMPLE09_MANIFEST),
            "--native-kernel-library",
            str(library09),
        ], cwd=RUST_DIR)
        artifact09 = json.loads(result09.stdout)
        assert artifact09["outputs"]["public"]["result_path"] == "C:/FROG/from_rust_bridge.txt"
        assert artifact09["outputs"]["ui"]["path_result"] == "C:/FROG/from_rust_bridge.txt"

        result10 = run([
            "cargo",
            "run",
            "--offline",
            "--",
            "run",
            "true",
            "--example",
            "10",
            "--native-kernel-manifest",
            str(EXAMPLE10_MANIFEST),
            "--native-kernel-library",
            str(library10),
        ], cwd=RUST_DIR)
        artifact10 = json.loads(result10.stdout)
        assert artifact10["outputs"]["public"]["pressed"] is True
        assert artifact10["outputs"]["ui"]["trigger_button"] is False
        assert artifact10["outputs"]["ui"]["pressed_indicator"] is True

        print("Rust dynamic native kernel bridge check: ok")
        return 0
    except (RuntimeError, AssertionError, json.JSONDecodeError) as exc:
        print(f"Rust dynamic native kernel bridge check: FAILED: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
