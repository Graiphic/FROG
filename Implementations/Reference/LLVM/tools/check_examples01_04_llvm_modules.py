#!/usr/bin/env python3
"""Compatibility wrapper for the Examples 01-04 LLVM module emitter."""

from __future__ import annotations

try:
    from .emit_examples01_04_llvm_modules import main
except ImportError:  # pragma: no cover
    from emit_examples01_04_llvm_modules import main  # type: ignore


if __name__ == "__main__":
    raise SystemExit(main())
