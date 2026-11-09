from __future__ import annotations

import argparse
from pathlib import Path

try:
    from .ui_runtime import build_runtime
    from .runtime_core import default_contract_path, default_wfrog_path
except ImportError:  # pragma: no cover
    from ui_runtime import build_runtime
    from runtime_core import default_contract_path, default_wfrog_path


def main() -> int:
    parser = argparse.ArgumentParser(description="Run Example 05 through the browser-based Python runtime UI.")
    parser.add_argument("--contract", type=Path, default=default_contract_path())
    parser.add_argument("--wfrog", type=Path, default=default_wfrog_path())
    parser.add_argument("--host", type=str, default="127.0.0.1")
    parser.add_argument("--port", type=int, default=0)
    parser.add_argument("--no-open-browser", action="store_true")
    args = parser.parse_args()

    runtime = build_runtime(
        contract_path=args.contract,
        wfrog_path=args.wfrog,
        host=args.host,
        port=args.port,
        open_browser=not args.no_open_browser,
    )
    runtime.serve()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
