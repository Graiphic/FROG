#!/usr/bin/env python3
"""Derive the published Example 05 FIR from its canonical .frog source.

This tool is intentionally narrow. It is the first reproducible .frog -> FIR
reference step for the frozen Example 05 corridor. It does not implement a
general FROG compiler.

Run from the repository root:

    python Implementations/Reference/Deriver/derive_example05_fir.py --check

or:

    python Implementations/Reference/Deriver/derive_example05_fir.py \
      --source Examples/05_bounded_ui_accumulator/main.frog \
      --output build/generated/main.fir.json
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

try:
    from .fir_deriver import (
        DerivationError,
        canonical_json_bytes,
        derive_fir_from_source,
        dump_json,
        load_json,
    )
except ImportError:  # pragma: no cover
    from fir_deriver import (  # type: ignore
        DerivationError,
        canonical_json_bytes,
        derive_fir_from_source,
        dump_json,
        load_json,
    )


ROOT = Path(__file__).resolve().parents[3]

DEFAULT_SOURCE = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.frog"
DEFAULT_EXPECTED_FIR = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.fir.json"


def repo_relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix() if path.is_absolute() else path.as_posix()


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Derive Example 05 FIR from canonical .frog source.")
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE, help="Path to the Example 05 .frog source.")
    parser.add_argument("--output", type=Path, default=None, help="Optional path where generated FIR should be written.")
    parser.add_argument("--expected", type=Path, default=DEFAULT_EXPECTED_FIR, help="Expected FIR artifact used by --check.")
    parser.add_argument("--check", action="store_true", help="Compare generated FIR against the expected published FIR.")
    parser.add_argument("--print", action="store_true", dest="print_json", help="Print generated FIR to stdout.")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    source_path = args.source if args.source.is_absolute() else ROOT / args.source
    expected_path = args.expected if args.expected.is_absolute() else ROOT / args.expected

    try:
        source = load_json(source_path)
        generated = derive_fir_from_source(source, repo_relative(source_path))

        if args.output:
            out_path = args.output if args.output.is_absolute() else ROOT / args.output
            dump_json(generated, out_path)
            print(f"wrote generated FIR: {out_path}")

        if args.print_json:
            json.dump(generated, sys.stdout, indent=2)
            sys.stdout.write("\n")

        if args.check:
            expected = load_json(expected_path)
            if canonical_json_bytes(generated) != canonical_json_bytes(expected):
                print("FIR derivation check: FAILED", file=sys.stderr)
                print(f"expected: {expected_path}", file=sys.stderr)
                return 1
            print("FIR derivation check: ok")
            print(f"source:   {source_path.relative_to(ROOT)}")
            print(f"expected: {expected_path.relative_to(ROOT)}")

        return 0

    except DerivationError as exc:
        print(f"FIR derivation error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
