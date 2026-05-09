#!/usr/bin/env python3
"""Validate the Example 05 native-kernel bridge publication surface.

This check is intentionally narrow. It verifies the manifest and ABI artifact
needed by the compiler-agnostic runtime bridge direction.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json"
LOWERING = ROOT / "Examples/05_bounded_ui_accumulator/main.lowering.json"
KERNEL_LL = ROOT / "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll"
EXPECTED_DIAGNOSTIC = "final_state must remain in the u16 domain."


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise AssertionError(f"missing file: {path.relative_to(ROOT)}") from exc
    if not isinstance(data, dict):
        raise AssertionError(f"expected JSON object: {path.relative_to(ROOT)}")
    return data


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> int:
    manifest = load_json(MANIFEST)
    lowering = load_json(LOWERING)
    kernel_text = KERNEL_LL.read_text(encoding="utf-8")

    require(manifest.get("artifact_kind") == "frog_native_kernel_manifest", "unexpected manifest artifact_kind")
    require(manifest.get("artifact_governance_ref", {}).get("path") == "Versioning/Readme.md", "missing governance ref")

    kernel = manifest.get("kernel")
    require(isinstance(kernel, dict), "manifest.kernel must be an object")
    require(kernel.get("id") == "example05_accumulator_kernel", "unexpected kernel id")
    require(kernel.get("source_lowered_unit") == "Examples/05_bounded_ui_accumulator/main.lowering.json", "unexpected source lowering")

    backend = kernel.get("backend")
    require(isinstance(backend, dict), "manifest.kernel.backend must be an object")
    require(backend.get("family") == "llvm", "backend family should be llvm metadata for the first artifact")
    require(backend.get("artifact_kind") == "native_object_or_library", "unexpected backend artifact kind")
    require(backend.get("source_artifact") == "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll", "unexpected source artifact")

    entry = kernel.get("entry")
    require(isinstance(entry, dict), "manifest.kernel.entry must be an object")
    require(entry.get("symbol") == "frog_example05_run", "unexpected entry symbol")
    require(entry.get("abi") == "frog_u16_to_result_status_outptr", "unexpected ABI name")

    layout = kernel.get("result_status_layout")
    require(isinstance(layout, dict), "manifest.kernel.result_status_layout must be an object")
    require(layout.get("carrier") == "out_parameter", "native bridge ABI must use an out parameter carrier")
    require(layout.get("c_signature") == "void frog_example05_run(uint16_t input_value, FrogRunResult* out_result)", "unexpected C signature")

    io = kernel.get("io")
    require(isinstance(io, dict), "manifest.kernel.io must be an object")
    require(io.get("inputs") == [{"id": "input_value", "type": "u16"}], "unexpected input surface")
    require(io.get("outputs") == [{"id": "result", "type": "u16"}], "unexpected output surface")

    error_model = kernel.get("error_model")
    require(isinstance(error_model, dict), "manifest.kernel.error_model must be an object")
    require(error_model.get("overflow") == "reject_execution_on_u16_overflow", "unexpected overflow model")
    require(any(item.get("code") == 1 and item.get("diagnostic") == EXPECTED_DIAGNOSTIC for item in error_model.get("error_codes", [])), "missing overflow diagnostic mapping")

    units = lowering.get("lowered_units")
    require(isinstance(units, list) and len(units) == 1, "lowering must contain exactly one lowered unit")
    unit = units[0]
    require(unit.get("kind") == "bounded_accumulator_kernel_with_ui_bindings", "unexpected lowered unit kind")
    execution_kernel = unit.get("execution_kernel")
    require(isinstance(execution_kernel, dict), "lowered unit execution_kernel must be an object")
    require(execution_kernel.get("state_type") == "u16", "native bridge expects u16 state")
    require(execution_kernel.get("initial_state") == 0, "native bridge expects initial_state 0")
    require(execution_kernel.get("iteration_count") == 5, "native bridge expects five iterations")

    require("%FrogRunResult = type { i8, i16, i16 }" in kernel_text, "kernel.ll must declare FrogRunResult layout")
    require("define void @frog_example05_run(i16 %input_value, ptr %out_result)" in kernel_text, "kernel.ll must export out-parameter frog_example05_run")
    require("getelementptr inbounds %FrogRunResult, ptr %out_result" in kernel_text, "kernel.ll must write through the out result pointer")
    require("store i8 1" in kernel_text, "kernel.ll must encode success ok=1")
    require("store i16 1" in kernel_text, "kernel.ll must encode overflow error_code 1")
    require("ret void" in kernel_text, "kernel.ll must return void for the out-parameter ABI")

    print("Example 05 native kernel bridge publication check: ok")
    print(f"manifest: {MANIFEST.relative_to(ROOT)}")
    print(f"kernel: {KERNEL_LL.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"Example 05 native kernel bridge publication check: FAILED: {exc}", file=sys.stderr)
        raise SystemExit(1)
