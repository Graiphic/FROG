#!/usr/bin/env python3
"""Emit/check an LLVM module from a supported lowered FROG artifact.

This is the generic-by-kind LLVM entry point for the non-normative reference
workspace.

Example:

    python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
      --lowering Examples/01_pure_addition/main.lowering.json \
      --expected Implementations/Reference/LLVM/examples/01_pure_addition/module.ll \
      --check
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

try:
    from .llvm_lowering_emitter import (
        LLVMEmissionError,
        emit_llvm_from_lowering_path,
        normalize_text,
        write_text,
    )
except ImportError:  # pragma: no cover
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from llvm_lowering_emitter import (  # type: ignore
        LLVMEmissionError,
        emit_llvm_from_lowering_path,
        normalize_text,
        write_text,
    )


ROOT = Path(__file__).resolve().parents[4]


def repo_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path


def check_expected(generated: str, expected_path: Path) -> bool:
    expected = expected_path.read_text(encoding="utf-8")
    return normalize_text(generated) == normalize_text(expected)


def run_build(example_dir: Path) -> int:
    build_script = example_dir / "build.sh"
    if not build_script.is_file():
        raise LLVMEmissionError(f"missing build script: {build_script}")

    result = subprocess.run(
        ["bash", str(build_script)],
        cwd=example_dir,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    return result.returncode


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Emit/check LLVM module from a supported lowering artifact.")
    parser.add_argument("--lowering", type=Path, required=True)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--expected", type=Path, default=None)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--print", action="store_true", dest="print_module")
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--example-dir", type=Path, default=None, help="Directory containing build.sh for --build.")
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv or [])

    lowering_path = repo_path(args.lowering)
    output_path = repo_path(args.output) if args.output is not None else None
    expected_path = repo_path(args.expected) if args.expected is not None else None
    example_dir = repo_path(args.example_dir) if args.example_dir is not None else None

    try:
        generated = emit_llvm_from_lowering_path(lowering_path)

        if output_path is not None:
            write_text(output_path, generated)
            print(f"wrote LLVM module: {output_path}")

        if args.print_module:
            sys.stdout.write(normalize_text(generated))

        if args.check:
            if expected_path is None:
                raise LLVMEmissionError("--check requires --expected")
            if not check_expected(generated, expected_path):
                print("LLVM module emission check: FAILED", file=sys.stderr)
                print(f"lowering: {lowering_path}", file=sys.stderr)
                print(f"expected: {expected_path}", file=sys.stderr)
                return 1
            print("LLVM module emission check: ok")
            print(f"lowering: {lowering_path.relative_to(ROOT)}")
            print(f"expected: {expected_path.relative_to(ROOT)}")

        if args.build:
            if example_dir is None:
                raise LLVMEmissionError("--build requires --example-dir")
            code = run_build(example_dir)
            if code != 0:
                print(f"LLVM build check: FAILED (exit {code})", file=sys.stderr)
                return code
            print("LLVM build check: ok")

        return 0

    except LLVMEmissionError as exc:
        print(f"LLVM module emission error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
