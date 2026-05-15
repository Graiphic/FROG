#!/usr/bin/env python3
"""Compatibility entry point for the Examples 01-10 full pipeline."""

from __future__ import annotations

try:
    from .check_examples01_10_full import main
except ImportError:  # pragma: no cover
    from check_examples01_10_full import main  # type: ignore


if __name__ == "__main__":
    raise SystemExit(main())
