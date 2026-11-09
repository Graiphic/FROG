from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any, Dict

try:
    from .runtime_core import (
        Slice05RuntimeCore,
        default_contract_path,
        default_wfrog_path,
        execute_slice05_contract,
    )
except ImportError:  # pragma: no cover
    from runtime_core import (
        Slice05RuntimeCore,
        default_contract_path,
        default_wfrog_path,
        execute_slice05_contract,
    )


def load_runtime(
    *,
    contract_path: str | Path | None = None,
    wfrog_path: str | Path | None = None,
) -> Slice05RuntimeCore:
    return Slice05RuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)


def execute_contract(
    control_value: int = 3,
    *,
    contract_path: str | Path | None = None,
    wfrog_path: str | Path | None = None,
) -> Dict[str, Any]:
    return execute_slice05_contract(
        control_value=control_value,
        contract_path=contract_path,
        wfrog_path=wfrog_path,
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Execute the Example 05 runtime contract in headless mode.")
    parser.add_argument("input_value", nargs="?", type=int, default=3)
    parser.add_argument("--contract", type=Path, default=default_contract_path())
    parser.add_argument("--wfrog", type=Path, default=default_wfrog_path())
    args = parser.parse_args()

    artifact = execute_contract(
        args.input_value,
        contract_path=args.contract,
        wfrog_path=args.wfrog,
    )
    print(json.dumps(artifact, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
