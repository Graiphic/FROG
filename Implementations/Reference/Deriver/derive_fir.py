#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

try:
    from .fir_deriver import DerivationError, canonical_json_bytes, derive_fir_from_source, dump_json, load_json
except ImportError:  # pragma: no cover
    from fir_deriver import DerivationError, canonical_json_bytes, derive_fir_from_source, dump_json, load_json  # type: ignore

ROOT = Path(__file__).resolve().parents[3]

def repo_relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix() if path.is_absolute() else path.as_posix()

def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Derive FIR from a supported canonical .frog source.")
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--expected", type=Path, default=None)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--print", action="store_true", dest="print_json")
    args = parser.parse_args(argv)

    source_path = args.source if args.source.is_absolute() else ROOT / args.source
    try:
        generated = derive_fir_from_source(load_json(source_path), repo_relative(source_path))
        if args.output:
            output = args.output if args.output.is_absolute() else ROOT / args.output
            dump_json(generated, output)
            print(f"wrote generated FIR: {output}")
        if args.print_json:
            json.dump(generated, sys.stdout, indent=2)
            sys.stdout.write("\n")
        if args.check:
            if args.expected is None:
                raise DerivationError("--check requires --expected")
            expected_path = args.expected if args.expected.is_absolute() else ROOT / args.expected
            expected = load_json(expected_path)
            if canonical_json_bytes(generated) != canonical_json_bytes(expected):
                print("FIR derivation check: FAILED", file=sys.stderr)
                print(f"source:   {source_path}", file=sys.stderr)
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
    raise SystemExit(main())
