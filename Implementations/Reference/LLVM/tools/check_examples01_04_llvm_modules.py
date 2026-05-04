#!/usr/bin/env python3
"""Check/build LLVM modules for Examples 01-04.

This tool is intentionally narrow and non-normative. It verifies that the
published LLVM module files for Examples 01-04 are present and executable.
For Example 05, use tools/emit_llvm_module.py.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[4]

EXAMPLE_DIRS = {
    "01": ROOT / "Implementations/Reference/LLVM/examples/01_pure_addition",
    "02": ROOT / "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip",
    "03": ROOT / "Implementations/Reference/LLVM/examples/03_ui_property_write",
    "04": ROOT / "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay",
}


REQUIRED_MARKERS = {
    "01": ["define double @frog_example01_add", "fadd double %a, %b"],
    "02": ["define double @frog_example02_widget_add", "fadd double %ctrl_a, %ctrl_b"],
    "03": ["widget=ctrl_gain", "member=label.text"],
    "04": ["define double @frog_example04_step", "fadd double %state_current, %x"],
}


def check_module(key: str) -> None:
    path = EXAMPLE_DIRS[key] / "module.ll"
    if not path.is_file():
        raise RuntimeError(f"missing LLVM module: {path}")
    text = path.read_text(encoding="utf-8").replace("\r\n", "\n")
    for marker in REQUIRED_MARKERS[key]:
        if marker not in text:
            raise RuntimeError(f"missing marker in Example {key} module.ll: {marker}")
    print(f"LLVM module check ok: Example {key}")


def build_module(key: str) -> None:
    script = EXAMPLE_DIRS[key] / "build.sh"
    if not script.is_file():
        raise RuntimeError(f"missing build script: {script}")
    result = subprocess.run(["bash", str(script)], cwd=EXAMPLE_DIRS[key], text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        raise RuntimeError(f"LLVM build failed for Example {key}")
    print(f"LLVM build ok: Example {key}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Check/build LLVM modules for Examples 01-04.")
    parser.add_argument("--example", choices=sorted(EXAMPLE_DIRS), default=None)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--build", action="store_true")
    args = parser.parse_args()

    keys = [args.example] if args.example else sorted(EXAMPLE_DIRS)

    try:
        if args.check or not args.build:
            for key in keys:
                check_module(key)
        if args.build:
            for key in keys:
                build_module(key)
        return 0
    except RuntimeError as exc:
        print(f"LLVM examples01-04 error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
