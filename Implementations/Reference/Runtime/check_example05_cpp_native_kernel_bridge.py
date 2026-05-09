#!/usr/bin/env python3
"""Build and test the Example 05 C++ bridge with the LLVM-produced kernel artifact.

This check is intentionally optional. It requires CMake, a C++ toolchain, and
clang. The standard C++ runtime build remains independent from LLVM/clang.
"""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILD_DIR = ROOT / "build" / "frog_runtime_cpp_native_kernel_bridge"


@dataclass(frozen=True)
class ConfigureCandidate:
    name: str
    args: list[str]


def require_tool(name: str) -> None:
    if shutil.which(name) is None:
        raise RuntimeError(f"required tool not found on PATH: {name}")


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


def run_result(command: list[str]) -> subprocess.CompletedProcess[str]:
    print("$ " + " ".join(command))
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    return result


def remove_build_dir(build_dir: Path) -> None:
    if not build_dir.exists():
        return
    shutil.rmtree(build_dir)


def prepare_build_dir() -> Path:
    if not BUILD_DIR.exists():
        return BUILD_DIR
    try:
        remove_build_dir(BUILD_DIR)
        return BUILD_DIR
    except PermissionError:
        return ROOT / "build" / f"frog_runtime_cpp_native_kernel_bridge_{os.getpid()}"


def configure_candidates() -> list[ConfigureCandidate]:
    candidates: list[ConfigureCandidate] = []
    compiler_arg = ["-DCMAKE_CXX_COMPILER=clang++"] if shutil.which("clang++") is not None else []

    if sys.platform == "win32":
        if shutil.which("ninja") is not None:
            candidates.append(ConfigureCandidate("Ninja", ["-G", "Ninja", *compiler_arg]))
        if shutil.which("mingw32-make") is not None:
            candidates.append(ConfigureCandidate("MinGW Makefiles", ["-G", "MinGW Makefiles", *compiler_arg]))
        candidates.append(ConfigureCandidate("default", compiler_arg))
    else:
        candidates.append(ConfigureCandidate("default", compiler_arg))

    return candidates


def cmake_configure_base_command(build_dir: Path) -> list[str]:
    return [
        "cmake",
        "-S",
        "Implementations/Reference/Runtime/cpp",
        "-B",
        str(build_dir.relative_to(ROOT)),
        "-DFROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE=ON",
    ]


def configure_build_dir(build_dir: Path) -> Path:
    errors: list[str] = []
    for candidate in configure_candidates():
        candidate_build_dir = build_dir
        try:
            remove_build_dir(candidate_build_dir)
        except PermissionError:
            candidate_build_dir = ROOT / "build" / f"frog_runtime_cpp_native_kernel_bridge_{os.getpid()}_{candidate.name.replace(' ', '_')}"

        command = cmake_configure_base_command(candidate_build_dir) + candidate.args
        result = run_result(command)
        if result.returncode == 0:
            print(f"CMake configure selected: {candidate.name}")
            return candidate_build_dir
        errors.append(f"{candidate.name}: exit {result.returncode}")

    joined = "; ".join(errors)
    raise RuntimeError(f"no usable CMake generator for native bridge check ({joined})")


def executable_candidates(build_dir: Path, name: str) -> list[Path]:
    suffix = ".exe" if sys.platform == "win32" else ""
    return [
        build_dir / f"{name}{suffix}",
        build_dir / "Debug" / f"{name}{suffix}",
        build_dir / "Release" / f"{name}{suffix}",
        build_dir / "RelWithDebInfo" / f"{name}{suffix}",
    ]


def executable_path(build_dir: Path, name: str) -> Path:
    for candidate in executable_candidates(build_dir, name):
        if candidate.exists():
            return candidate
    joined = ", ".join(str(path.relative_to(ROOT)) for path in executable_candidates(build_dir, name))
    raise RuntimeError(f"unable to locate built executable {name}; checked {joined}")


def check_native_runtime_headless(build_dir: Path) -> None:
    executable = executable_path(build_dir, "frog_reference_runtime_cpp_llvm_kernel")
    result = run([str(executable.relative_to(ROOT)), "3"])
    artifact = json.loads(result.stdout)
    assert artifact["status"] == "ok"
    assert artifact["execution_summary"]["final_state"] == 15
    assert artifact["outputs"]["public"]["result"] == 15
    assert artifact["outputs"]["ui"]["ctrl_input"] == 3
    assert artifact["outputs"]["ui"]["ind_result"] == 15
    assert artifact["ui_runtime"]["panel"]["layout"]["width"] == 500
    assert artifact["ui_runtime"]["widgets"][0]["layout"]["width"] == 220
    assert artifact["ui_runtime"]["widgets"][1]["layout"]["width"] == 220


def main() -> int:
    try:
        require_tool("cmake")
        require_tool("clang")

        build_dir = configure_build_dir(prepare_build_dir())

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
        run([
            "cmake",
            "--build",
            str(build_dir.relative_to(ROOT)),
            "--target",
            "frog_reference_runtime_cpp_llvm_kernel",
        ])
        check_native_runtime_headless(build_dir)

        print("Example 05 C++ native kernel bridge check: ok")
        return 0
    except (RuntimeError, AssertionError, json.JSONDecodeError) as exc:
        print(f"Example 05 C++ native kernel bridge check: FAILED: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
