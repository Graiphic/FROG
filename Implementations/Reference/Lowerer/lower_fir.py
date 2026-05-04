#!/usr/bin/env python3
"""Generic reference lowerer CLI for supported FIR examples."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

try:
    from .fir_lowerer import LoweringError, canonical_json_bytes, dump_json, load_json, lower_fir_artifact
except ImportError:  # pragma: no cover
    from fir_lowerer import LoweringError, canonical_json_bytes, dump_json, load_json, lower_fir_artifact  # type: ignore


ROOT = Path(__file__).resolve().parents[3]


def repo_relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix() if path.is_absolute() else path.as_posix()


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Lower a supported FIR artifact.")
    parser.add_argument("--fir", type=Path, required=True)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--expected", type=Path, default=None)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--print", action="store_true", dest="print_json")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    fir_path = args.fir if args.fir.is_absolute() else ROOT / args.fir

    try:
        fir = load_json(fir_path)
        generated = lower_fir_artifact(fir, repo_relative(fir_path))

        if args.output:
            output_path = args.output if args.output.is_absolute() else ROOT / args.output
            dump_json(generated, output_path)
            print(f"wrote generated lowering: {output_path}")

        if args.print_json:
            json.dump(generated, sys.stdout, indent=2)
            sys.stdout.write("\n")

        if args.check:
            if args.expected is None:
                raise LoweringError("--check requires --expected")
            expected_path = args.expected if args.expected.is_absolute() else ROOT / args.expected
            expected = load_json(expected_path)
            if canonical_json_bytes(generated) != canonical_json_bytes(expected):
                print("Lowering check: FAILED", file=sys.stderr)
                print(f"fir:      {fir_path}", file=sys.stderr)
                print(f"expected: {expected_path}", file=sys.stderr)
                return 1
            print("Lowering check: ok")
            print(f"fir:      {fir_path.relative_to(ROOT)}")
            print(f"expected: {expected_path.relative_to(ROOT)}")

        return 0
    except LoweringError as exc:
        print(f"Lowering error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
