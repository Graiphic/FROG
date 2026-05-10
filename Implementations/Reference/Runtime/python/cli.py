from __future__ import annotations

import argparse
import json
from pathlib import Path

try:
    from .execute_contract import execute_contract
    from .native_kernel import load_native_bool_kernel_bridge, load_native_kernel_bridge
    from .runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path
    from ..contract_executor import execute_contract_case, load_json
    from .ui_runtime import (
        build_runtime,
        BooleanRuntimeCore,
        default_example06_contract_path,
        default_example06_wfrog_path,
        is_example06_contract,
        parse_bool_input,
        wants_example06,
    )
except ImportError:  # pragma: no cover
    import sys

    sys.path.append(str(Path(__file__).resolve().parents[1]))
    from execute_contract import execute_contract
    from native_kernel import load_native_bool_kernel_bridge, load_native_kernel_bridge
    from runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path
    from contract_executor import execute_contract_case, load_json
    from ui_runtime import (
        build_runtime,
        BooleanRuntimeCore,
        default_example06_contract_path,
        default_example06_wfrog_path,
        is_example06_contract,
        parse_bool_input,
        wants_example06,
    )


def execute_example06_contract(
    input_value: str | bool | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_bool_kernel_bridge(native_kernel_manifest, native_kernel_library)
        runtime = BooleanRuntimeCore(
            contract_path=contract_path or default_example06_contract_path(),
            wfrog_path=wfrog_path or default_example06_wfrog_path(),
        )
        return runtime.execute_with_native_kernel_bridge(bridge, parse_bool_input(input_value))

    contract = load_json(contract_path or default_example06_contract_path())
    wfrog = load_json(wfrog_path or default_example06_wfrog_path())
    return execute_contract_case(
        contract,
        {"input_value": parse_bool_input(input_value)},
        {"wfrog": wfrog},
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Python reference runtime family for published FROG examples.")
    subparsers = parser.add_subparsers(dest="command", required=False)

    run_parser = subparsers.add_parser("run", help="Execute the runtime in headless mode.")
    run_parser.add_argument("input_value", nargs="?", default=None)
    run_parser.add_argument("--example", type=str, default=None)
    run_parser.add_argument("--contract", type=Path, default=None)
    run_parser.add_argument("--wfrog", type=Path, default=None)
    run_parser.add_argument("--native-kernel-manifest", type=Path, default=None)
    run_parser.add_argument("--native-kernel-library", type=Path, default=None)

    ui_parser = subparsers.add_parser("ui", help="Serve the browser-based UI host.")
    ui_parser.add_argument("--example", type=str, default=None)
    ui_parser.add_argument("--contract", type=Path, default=None)
    ui_parser.add_argument("--wfrog", type=Path, default=None)
    ui_parser.add_argument("--host", type=str, default="127.0.0.1")
    ui_parser.add_argument("--port", type=int, default=0)
    ui_parser.add_argument("--no-open-browser", action="store_true")
    ui_parser.add_argument("--native-kernel-manifest", type=Path, default=None)
    ui_parser.add_argument("--native-kernel-library", type=Path, default=None)

    args = parser.parse_args()

    if args.command in {None, "run"}:
        if wants_example06(getattr(args, "example", None)) or is_example06_contract(getattr(args, "contract", None)):
            artifact = execute_example06_contract(
                getattr(args, "input_value", None),
                contract_path=getattr(args, "contract", None),
                wfrog_path=getattr(args, "wfrog", None),
                native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
                native_kernel_library=getattr(args, "native_kernel_library", None),
            )
        elif args.native_kernel_manifest is not None and args.native_kernel_library is not None:
            bridge = load_native_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
            runtime = Slice05RuntimeCore(
                contract_path=getattr(args, "contract", None) or default_contract_path(),
                wfrog_path=getattr(args, "wfrog", None) or default_wfrog_path(),
            )
            artifact = runtime.execute_with_native_kernel_bridge(
                bridge,
                control_value=int(getattr(args, "input_value", None) or 3),
            )
        else:
            artifact = execute_contract(
                int(getattr(args, "input_value", None) or 3),
                contract_path=getattr(args, "contract", None) or default_contract_path(),
                wfrog_path=getattr(args, "wfrog", None) or default_wfrog_path(),
            )
        print(json.dumps(artifact, indent=2))
        return 0

    native_bridge = None
    if args.native_kernel_manifest is not None and args.native_kernel_library is not None:
        if wants_example06(args.example) or is_example06_contract(args.contract):
            native_bridge = load_native_bool_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        else:
            native_bridge = load_native_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)

    runtime = build_runtime(
        example=args.example,
        contract_path=args.contract,
        wfrog_path=args.wfrog,
        host=args.host,
        port=args.port,
        open_browser=not args.no_open_browser,
        native_kernel_bridge=native_bridge,
    )
    runtime.serve()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
