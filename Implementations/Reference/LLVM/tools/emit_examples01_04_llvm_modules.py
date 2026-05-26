#!/usr/bin/env python3
"""Emit/check LLVM modules for Examples 01-04.

Compatibility wrapper around the generic lowered-unit-kind LLVM emitter.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

try:
    from .llvm_lowering_emitter import LLVMEmissionError, emit_llvm_from_lowering_path, normalize_text, write_text
except ImportError:  # pragma: no cover
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from llvm_lowering_emitter import LLVMEmissionError, emit_llvm_from_lowering_path, normalize_text, write_text  # type: ignore


ROOT = Path(__file__).resolve().parents[4]

EXAMPLES = {
    "01": {
        "lowering": ROOT / "Examples/01_pure_addition/main.lowering.json",
        "module": ROOT / "Implementations/Reference/LLVM/examples/01_pure_addition/module.ll",
        "example_dir": ROOT / "Implementations/Reference/LLVM/examples/01_pure_addition",
    },
    "02": {
        "lowering": ROOT / "Examples/02_ui_value_roundtrip/main.lowering.json",
        "module": ROOT / "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll",
        "example_dir": ROOT / "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip",
    },
    "03": {
        "lowering": ROOT / "Examples/03_ui_property_write/main.lowering.json",
        "module": ROOT / "Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll",
        "example_dir": ROOT / "Implementations/Reference/LLVM/examples/03_ui_property_write",
    },
    "04": {
        "lowering": ROOT / "Examples/04_stateful_feedback_delay/main.lowering.json",
        "module": ROOT / "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll",
        "example_dir": ROOT / "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay",
    },
}


def check_module(key: str) -> None:
    spec = EXAMPLES[key]
    generated = emit_llvm_from_lowering_path(spec["lowering"])
    expected = normalize_text(spec["module"].read_text(encoding="utf-8"))
    if generated != expected:
        raise LLVMEmissionError(f"LLVM module emission mismatch for Example {key}")
    print(f"LLVM module emission check ok: Example {key}")


def write_module(key: str, output: Path | None) -> None:
    spec = EXAMPLES[key]
    generated = emit_llvm_from_lowering_path(spec["lowering"])
    out_path = output if output is not None else spec["module"]
    write_text(out_path, generated)
    print(out_path)


def build_module(key: str) -> None:
    spec = EXAMPLES[key]
    script = spec["example_dir"] / "build.sh"
    result = subprocess.run(["bash", str(script)], cwd=spec["example_dir"], text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        raise LLVMEmissionError(f"LLVM build failed for Example {key}")
    print(f"LLVM build ok: Example {key}")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Emit/check LLVM modules for Examples 01-04.")
    parser.add_argument("--example", choices=sorted(EXAMPLES), default=None)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--build", action="store_true")
    args = parser.parse_args(argv)

    keys = [args.example] if args.example else sorted(EXAMPLES)

    try:
        if args.output is not None and args.example is None:
            raise LLVMEmissionError("--output requires --example")
        if args.check:
            for key in keys:
                check_module(key)
        elif not args.build:
            for key in keys:
                write_module(key, args.output)

        if args.build:
            for key in keys:
                build_module(key)

        return 0
    except LLVMEmissionError as exc:
        print(f"LLVM examples01-04 emission error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
