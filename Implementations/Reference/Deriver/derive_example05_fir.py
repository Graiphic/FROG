#!/usr/bin/env python3
from __future__ import annotations

import sys
from pathlib import Path

try:
    from .derive_fir import main as generic_main
except ImportError:  # pragma: no cover
    from derive_fir import main as generic_main  # type: ignore

ROOT = Path(__file__).resolve().parents[3]
DEFAULT_SOURCE = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.frog"
DEFAULT_EXPECTED = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.fir.json"

def main() -> int:
    argv = sys.argv[1:]
    if "--source" not in argv:
        argv.extend(["--source", str(DEFAULT_SOURCE)])
    if "--expected" not in argv:
        argv.extend(["--expected", str(DEFAULT_EXPECTED)])
    return generic_main(argv)

if __name__ == "__main__":
    raise SystemExit(main())
