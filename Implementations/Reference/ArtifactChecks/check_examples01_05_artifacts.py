#!/usr/bin/env python3
"""Compatibility entry point for the Examples 01-10 artifact preflight."""

from __future__ import annotations

try:
    from .check_examples01_10_artifacts import main
except ImportError:  # pragma: no cover
    from check_examples01_10_artifacts import main  # type: ignore


if __name__ == "__main__":
    raise SystemExit(main())
