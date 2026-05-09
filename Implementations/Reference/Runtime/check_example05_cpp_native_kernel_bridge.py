#!/usr/bin/env python3
"""Build and test the Example 05 C++ bridge with the LLVM-produced kernel artifact.

This check is intentionally optional. It requires CMake, a C++ toolchain, and
clang. The standard C++ runtime build remains independent from LLVM/clang.
"""

from __future__ import annotations

import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILD_DIR = ROOT / "build" / "frog_runtime_cpp_native_kernel_bridge"


def require_tool(name: str) -> None:
    if shutil.which(name) is None:
        raise RuntimeError(f"required tool not found on PATH: {name}")


def run(command: list[str]) -> None:
    print("$ " + " ".join(command))
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit {result.returncode}: {' '.join(command)}")


def prepare_build_dir() -> Path:
    if not BUILD_DIR.exists():
        return BUILD_DIR
    try:
        shutil.rmtree(BUILD_DIR)
        return BUILD_DIR
    except PermissionError:
        return ROOT / "build" / f"frog_runtime_cpp_native_kernel_bridge_{os.getpid()}"


def cmake_configure_command(build_dir: Path) -> list[str]:
    command = [
        "cmake",
        "-S",
        "Implementations/Reference/Runtime/cpp",
        "-B",
        str(build_dir.relative_to(ROOT)),
        "-DFROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE=ON",
    ]
    if sys.platform == "win32":
        if shutil.which("mingw32-make") is not None:
            command.extend(["-G", "MinGW Makefiles"])
        elif shutil.which("ninja") is not None:
            command.extend(["-G", "Ninja"])
        if shutil.which("clang++") is not None:
            command.append("-DCMAKE_CXX_COMPILER=clang++")
    return command


def main() -> int:
    try:
        require_tool("cmake")
        require_tool("clang")

        build_dir = prepare_build_dir()

        run(cmake_configure_command(build_dir))
        run([
            "cmake",
            "--build",
            str(build_dir.relative_to(ROOT)),
            "--target",
            "frog_reference_runtime_cpp_llvm_kernel_tests",
        ])
        run([
            "ctest",
            "--test-dir",
            str(build_dir.relative_to(ROOT)),
            "-R",
            "frog_reference_runtime_cpp_llvm_kernel_tests",
            "--output-on-failure",
        ])

        print("Example 05 C++ native kernel bridge check: ok")
        return 0
    except RuntimeError as exc:
        print(f"Example 05 C++ native kernel bridge check: FAILED: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
