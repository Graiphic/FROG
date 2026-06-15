from __future__ import annotations

import argparse
import json
from pathlib import Path

try:
    from .execute_contract import execute_contract
    from .native_kernel import load_native_bool_kernel_bridge, load_native_enum_kernel_bridge, load_native_kernel_bridge, load_native_string_kernel_bridge
    from .runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path
    from ..contract_executor import execute_contract_case, load_json
    from .ui_runtime import (
        build_runtime,
        ButtonRuntimeCore,
        BooleanRuntimeCore,
        EnumRuntimeCore,
        PathRuntimeCore,
        StringRuntimeCore,
        default_example10_contract_path,
        default_example10_wfrog_path,
        default_example11_contract_path,
        default_example11_wfrog_path,
        default_example12_contract_path,
        default_example12_wfrog_path,
        default_example13_contract_path,
        default_example13_wfrog_path,
        default_example14_contract_path,
        default_example14_wfrog_path,
        default_example15_contract_path,
        default_example15_wfrog_path,
        default_example06_contract_path,
        default_example06_wfrog_path,
        default_example07_contract_path,
        default_example07_wfrog_path,
        default_example08_contract_path,
        default_example08_wfrog_path,
        default_example09_contract_path,
        default_example09_wfrog_path,
        is_example06_contract,
        is_example07_contract,
        is_example08_contract,
        is_example09_contract,
        is_example10_contract,
        is_example11_contract,
        is_example12_contract,
        is_example13_contract,
        is_example14_contract,
        is_example15_contract,
        parse_bool_input,
        wants_example06,
        wants_example07,
        wants_example08,
        wants_example09,
        wants_example10,
        wants_example11,
        wants_example12,
        wants_example13,
        wants_example14,
        wants_example15,
    )
except ImportError:  # pragma: no cover
    import sys

    sys.path.insert(0, str(Path(__file__).resolve().parent))
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from execute_contract import execute_contract
    from native_kernel import load_native_bool_kernel_bridge, load_native_enum_kernel_bridge, load_native_kernel_bridge, load_native_string_kernel_bridge
    from runtime_core import Slice05RuntimeCore, default_contract_path, default_wfrog_path
    from contract_executor import execute_contract_case, load_json
    from ui_runtime import (
        build_runtime,
        ButtonRuntimeCore,
        BooleanRuntimeCore,
        EnumRuntimeCore,
        PathRuntimeCore,
        StringRuntimeCore,
        default_example10_contract_path,
        default_example10_wfrog_path,
        default_example11_contract_path,
        default_example11_wfrog_path,
        default_example12_contract_path,
        default_example12_wfrog_path,
        default_example13_contract_path,
        default_example13_wfrog_path,
        default_example14_contract_path,
        default_example14_wfrog_path,
        default_example15_contract_path,
        default_example15_wfrog_path,
        default_example06_contract_path,
        default_example06_wfrog_path,
        default_example07_contract_path,
        default_example07_wfrog_path,
        default_example08_contract_path,
        default_example08_wfrog_path,
        default_example09_contract_path,
        default_example09_wfrog_path,
        is_example06_contract,
        is_example07_contract,
        is_example08_contract,
        is_example09_contract,
        is_example10_contract,
        is_example11_contract,
        is_example12_contract,
        is_example13_contract,
        is_example14_contract,
        is_example15_contract,
        parse_bool_input,
        wants_example06,
        wants_example07,
        wants_example08,
        wants_example09,
        wants_example10,
        wants_example11,
        wants_example12,
        wants_example13,
        wants_example14,
        wants_example15,
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


def execute_example07_contract(
    input_value: str | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = StringRuntimeCore(
        contract_path=contract_path or default_example07_contract_path(),
        wfrog_path=wfrog_path or default_example07_wfrog_path(),
    )
    text = "hello world" if input_value is None else str(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_string_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, text)
    return runtime.execute(text)


def execute_example08_contract(
    input_value: str | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = EnumRuntimeCore(
        contract_path=contract_path or default_example08_contract_path(),
        wfrog_path=wfrog_path or default_example08_wfrog_path(),
    )
    mode = "run" if input_value is None else str(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_enum_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, mode)
    return runtime.execute(mode)


def execute_example09_contract(
    input_value: str | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = PathRuntimeCore(
        contract_path=contract_path or default_example09_contract_path(),
        wfrog_path=wfrog_path or default_example09_wfrog_path(),
    )
    value = "C:/FROG/hello_world.txt" if input_value is None else str(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_string_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, value)
    return runtime.execute(value)


def execute_example10_contract(
    input_value: str | bool | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = ButtonRuntimeCore(
        contract_path=contract_path or default_example10_contract_path(),
        wfrog_path=wfrog_path or default_example10_wfrog_path(),
    )
    pressed = parse_bool_input(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_bool_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, pressed)
    return runtime.execute(pressed)


def execute_example11_contract(
    input_value: str | bool | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = ButtonRuntimeCore(
        contract_path=contract_path or default_example11_contract_path(),
        wfrog_path=wfrog_path or default_example11_wfrog_path(),
    )
    value = parse_bool_input(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_bool_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, value)
    return runtime.execute(value)


def execute_example12_contract(
    input_value: str | bool | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = ButtonRuntimeCore(
        contract_path=contract_path or default_example12_contract_path(),
        wfrog_path=wfrog_path or default_example12_wfrog_path(),
    )
    value = parse_bool_input(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_bool_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, value)
    return runtime.execute(value)


def execute_example13_contract(
    input_value: str | bool | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = ButtonRuntimeCore(
        contract_path=contract_path or default_example13_contract_path(),
        wfrog_path=wfrog_path or default_example13_wfrog_path(),
    )
    value = parse_bool_input(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_bool_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, value)
    return runtime.execute(value)


def execute_example14_contract(
    input_value: str | bool | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = ButtonRuntimeCore(
        contract_path=contract_path or default_example14_contract_path(),
        wfrog_path=wfrog_path or default_example14_wfrog_path(),
    )
    value = parse_bool_input(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_bool_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, value)
    return runtime.execute(value)


def execute_example15_contract(
    input_value: str | bool | None = None,
    *,
    contract_path: Path | None = None,
    wfrog_path: Path | None = None,
    native_kernel_manifest: Path | None = None,
    native_kernel_library: Path | None = None,
) -> dict[str, object]:
    runtime = ButtonRuntimeCore(
        contract_path=contract_path or default_example15_contract_path(),
        wfrog_path=wfrog_path or default_example15_wfrog_path(),
    )
    value = parse_bool_input(input_value)
    if native_kernel_manifest is not None and native_kernel_library is not None:
        bridge = load_native_bool_kernel_bridge(native_kernel_manifest, native_kernel_library)
        return runtime.execute_with_native_kernel_bridge(bridge, value)
    return runtime.execute(value)


def execute_contract_runtime(args: argparse.Namespace) -> dict[str, object]:
    """Compatibility entry point used by the Python runtime parity tests."""
    if wants_example06(getattr(args, "example", None)) or is_example06_contract(getattr(args, "contract", None)):
        return execute_example06_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example07(getattr(args, "example", None)) or is_example07_contract(getattr(args, "contract", None)):
        return execute_example07_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example08(getattr(args, "example", None)) or is_example08_contract(getattr(args, "contract", None)):
        return execute_example08_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example09(getattr(args, "example", None)) or is_example09_contract(getattr(args, "contract", None)):
        return execute_example09_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example10(getattr(args, "example", None)) or is_example10_contract(getattr(args, "contract", None)):
        return execute_example10_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example11(getattr(args, "example", None)) or is_example11_contract(getattr(args, "contract", None)):
        return execute_example11_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example12(getattr(args, "example", None)) or is_example12_contract(getattr(args, "contract", None)):
        return execute_example12_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example13(getattr(args, "example", None)) or is_example13_contract(getattr(args, "contract", None)):
        return execute_example13_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example14(getattr(args, "example", None)) or is_example14_contract(getattr(args, "contract", None)):
        return execute_example14_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if wants_example15(getattr(args, "example", None)) or is_example15_contract(getattr(args, "contract", None)):
        return execute_example15_contract(
            getattr(args, "input_value", None),
            contract_path=getattr(args, "contract", None),
            wfrog_path=getattr(args, "wfrog", None),
            native_kernel_manifest=getattr(args, "native_kernel_manifest", None),
            native_kernel_library=getattr(args, "native_kernel_library", None),
        )
    if getattr(args, "native_kernel_manifest", None) is not None and getattr(args, "native_kernel_library", None) is not None:
        bridge = load_native_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        runtime = Slice05RuntimeCore(
            contract_path=getattr(args, "contract", None) or default_contract_path(),
            wfrog_path=getattr(args, "wfrog", None) or default_wfrog_path(),
        )
        return runtime.execute_with_native_kernel_bridge(
            bridge,
            control_value=int(getattr(args, "input_value", None) or 3),
        )
    return execute_contract(
        int(getattr(args, "input_value", None) or 3),
        contract_path=getattr(args, "contract", None) or default_contract_path(),
        wfrog_path=getattr(args, "wfrog", None) or default_wfrog_path(),
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
        artifact = execute_contract_runtime(args)
        print(json.dumps(artifact, indent=2))
        return 0

    native_bridge = None
    if args.native_kernel_manifest is not None and args.native_kernel_library is not None:
        if wants_example06(args.example) or is_example06_contract(args.contract):
            native_bridge = load_native_bool_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example07(args.example) or is_example07_contract(args.contract):
            native_bridge = load_native_string_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example08(args.example) or is_example08_contract(args.contract):
            native_bridge = load_native_enum_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example09(args.example) or is_example09_contract(args.contract):
            native_bridge = load_native_string_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example10(args.example) or is_example10_contract(args.contract):
            native_bridge = load_native_bool_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example11(args.example) or is_example11_contract(args.contract):
            native_bridge = load_native_bool_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example12(args.example) or is_example12_contract(args.contract):
            native_bridge = load_native_bool_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example13(args.example) or is_example13_contract(args.contract):
            native_bridge = load_native_bool_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example14(args.example) or is_example14_contract(args.contract):
            native_bridge = load_native_bool_kernel_bridge(args.native_kernel_manifest, args.native_kernel_library)
        elif wants_example15(args.example) or is_example15_contract(args.contract):
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
