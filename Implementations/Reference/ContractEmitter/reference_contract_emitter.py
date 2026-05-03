from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

from .emit_backend_contract import (
    ContractEmissionError,
    emit_contract_to_path,
    emit_reference_host_runtime_contract,
    load_lowering_from_path,
)


def find_repo_root(start: Path) -> Path:
    current = start.resolve()
    for candidate in [current, *current.parents]:
        if (candidate / "Examples").is_dir() and (candidate / "Implementations").is_dir():
            return candidate
    raise ContractEmissionError("Unable to locate repository root from current path.")


def default_paths() -> tuple[Path, Path, Path]:
    repo_root = find_repo_root(Path(__file__).resolve())
    lowering = repo_root / "Examples" / "05_bounded_ui_accumulator" / "main.lowering.json"
    expected = (
        repo_root
        / "Implementations"
        / "Reference"
        / "ContractEmitter"
        / "examples"
        / "05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json"
    )
    output = expected
    return lowering, expected, output


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ContractEmissionError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ContractEmissionError(f"invalid JSON in {path}: {exc}") from exc

    if not isinstance(data, dict):
        raise ContractEmissionError(f"{path} must contain a JSON object")
    return data


def emit_contract_in_memory(lowering_path: Path, ui_package_path: str | None) -> dict[str, Any]:
    lowering = load_lowering_from_path(lowering_path)
    return emit_reference_host_runtime_contract(
        lowering,
        ui_package_path=ui_package_path,
        lowering_path=lowering_path,
    )


def main(argv: list[str] | None = None) -> int:
    default_lowering, default_expected, default_output = default_paths()

    parser = argparse.ArgumentParser(
        description="Emit or check the reference_host_runtime_ui_binding contract for Example 05."
    )
    parser.add_argument("--lowering", type=Path, default=default_lowering)
    parser.add_argument("--output", type=Path, default=default_output)
    parser.add_argument("--expected", type=Path, default=default_expected)
    parser.add_argument("--ui-package-path", type=str, default=None)
    parser.add_argument("--check", action="store_true", help="Compare emitted contract against the published expected artifact.")
    parser.add_argument("--print", action="store_true", dest="print_json", help="Print emitted contract JSON to stdout.")
    parser.add_argument("--no-write", action="store_true", help="Do not write --output when not checking.")
    args = parser.parse_args(argv)

    try:
        if args.check:
            emitted = emit_contract_in_memory(args.lowering, args.ui_package_path)
            expected = load_json(args.expected)
            if canonical_json_bytes(emitted) != canonical_json_bytes(expected):
                print("Contract emission check: FAILED", file=sys.stderr)
                print(f"lowering: {args.lowering}", file=sys.stderr)
                print(f"expected: {args.expected}", file=sys.stderr)
                return 1
            print("Contract emission check: ok")
            print(f"lowering: {args.lowering}")
            print(f"expected: {args.expected}")
            if args.print_json:
                json.dump(emitted, sys.stdout, indent=2)
                sys.stdout.write("\n")
            return 0

        if args.no_write:
            emitted = emit_contract_in_memory(args.lowering, args.ui_package_path)
            if args.print_json:
                json.dump(emitted, sys.stdout, indent=2)
                sys.stdout.write("\n")
            return 0

        emit_contract_to_path(args.lowering, args.output, ui_package_path=args.ui_package_path)
        print(args.output)
        return 0

    except ContractEmissionError as exc:
        print(f"Contract emission error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
