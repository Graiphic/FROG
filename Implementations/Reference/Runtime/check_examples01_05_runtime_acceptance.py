#!/usr/bin/env python3
"""Compatibility entry point for the Examples 01-10 runtime acceptance check."""

from __future__ import annotations

try:
    from .check_examples01_10_runtime_acceptance import main
except ImportError:  # pragma: no cover
    from check_examples01_10_runtime_acceptance import main  # type: ignore


if __name__ == "__main__":
    raise SystemExit(main())
