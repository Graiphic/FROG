from __future__ import annotations

import json
import sys
from pathlib import Path

from Implementations.Reference.common import BackendContract
from Implementations.Reference.Runtime.reference_runtime import create_runtime_for_family


DEFAULT_CONTRACT_PATH = (
    Path(__file__).resolve().parents[1]
    / "ContractEmitter"
    / "examples"
    / "05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json"
)


def _parse_control_value(argv: list[str]) -> int:
    if len(argv) >= 2:
        try:
            value = int(argv[1])
        except ValueError as exc:
            raise SystemExit("control_value must be an integer.") from exc
    else:
        value = 3

    if not (0 <= value <= 65535):
        raise SystemExit("control_value must remain within the u16 domain.")
    return value


def _parse_contract_path(argv: list[str]) -> Path:
    if len(argv) >= 3:
        return Path(argv[2]).resolve()
    return DEFAULT_CONTRACT_PATH


def main(argv: list[str]) -> int:
    control_value = _parse_control_value(argv)
    contract_path = _parse_contract_path(argv)

    if not contract_path.exists():
        raise SystemExit(f"Contract file not found: {contract_path}")

    artifact = json.loads(contract_path.read_text(encoding="utf-8"))
    contract = BackendContract(artifact=artifact)

    runtime = create_runtime_for_family("reference_host_runtime_ui_binding")
    result = runtime.execute(contract, {"input_value": control_value})

    print(json.dumps(result.artifact, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
