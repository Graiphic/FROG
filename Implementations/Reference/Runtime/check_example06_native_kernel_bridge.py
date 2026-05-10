#!/usr/bin/env python3
"""Validate the Example 06 native Boolean kernel bridge publication surface."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[3]
MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json"
LOWERING = ROOT / "Examples/06_boolean_value_roundtrip/main.lowering.json"
KERNEL_LL = ROOT / "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/kernel.ll"
MODULE_LL = ROOT / "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/module.ll"


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
    module_text = MODULE_LL.read_text(encoding="utf-8")

    require(manifest.get("artifact_kind") == "frog_native_kernel_manifest", "unexpected manifest artifact_kind")
    require(manifest.get("artifact_governance_ref", {}).get("path") == "Versioning/Readme.md", "missing governance ref")

    kernel = manifest.get("kernel")
    require(isinstance(kernel, dict), "manifest.kernel must be an object")
    require(kernel.get("id") == "example06_boolean_roundtrip_kernel", "unexpected kernel id")
    require(kernel.get("source_lowered_unit") == "Examples/06_boolean_value_roundtrip/main.lowering.json", "unexpected source lowering")

    backend = kernel.get("backend")
    require(isinstance(backend, dict), "manifest.kernel.backend must be an object")
    require(backend.get("family") == "llvm", "backend family should be llvm metadata")
    require(backend.get("artifact_kind") == "native_object_or_library", "unexpected backend artifact kind")
    require(backend.get("source_artifact") == "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/kernel.ll", "unexpected source artifact")

    entry = kernel.get("entry")
    require(isinstance(entry, dict), "manifest.kernel.entry must be an object")
    require(entry.get("symbol") == "frog_example06_run", "unexpected entry symbol")
    require(entry.get("abi") == "frog_bool_to_result_status_outptr", "unexpected ABI name")

    layout = kernel.get("result_status_layout")
    require(isinstance(layout, dict), "manifest.kernel.result_status_layout must be an object")
    require(layout.get("carrier") == "out_parameter", "native bridge ABI must use an out parameter carrier")
    require(layout.get("c_signature") == "void frog_example06_run(uint8_t input_value, FrogBoolRunResult* out_result)", "unexpected C signature")

    io = kernel.get("io")
    require(isinstance(io, dict), "manifest.kernel.io must be an object")
    require(io.get("inputs") == [{"id": "input_value", "type": "bool"}], "unexpected input surface")
    require(io.get("outputs") == [{"id": "result", "type": "bool"}], "unexpected output surface")

    error_model = kernel.get("error_model")
    require(isinstance(error_model, dict), "manifest.kernel.error_model must be an object")
    require(error_model.get("overflow") == "not_applicable", "unexpected bool overflow model")
    require(error_model.get("error_codes") == [{"code": 0, "meaning": "no_error"}], "unexpected bool error codes")

    units = lowering.get("lowered_units")
    require(isinstance(units, list) and len(units) == 1, "lowering must contain exactly one lowered unit")
    unit = units[0]
    require(unit.get("kind") == "boolean_value_roundtrip_kernel_with_ui_bindings", "unexpected lowered unit kind")
    execution_kernel = unit.get("execution_kernel")
    require(isinstance(execution_kernel, dict), "lowered unit execution_kernel must be an object")
    require(execution_kernel == {"operation": "copy", "dst": "result", "type": "bool", "src": "input_value", "final_publication": execution_kernel.get("final_publication")}, "unexpected bool execution kernel surface")

    require("%FrogBoolRunResult = type { i8, i8, i16 }" in kernel_text, "kernel.ll must declare FrogBoolRunResult layout")
    require("define void @frog_example06_run(i8 %input_value, ptr %out_result)" in kernel_text, "kernel.ll must export out-parameter frog_example06_run")
    require("getelementptr inbounds %FrogBoolRunResult, ptr %out_result" in kernel_text, "kernel.ll must write through the out result pointer")
    require("store i8 %result" in kernel_text, "kernel.ll must encode bool result")
    require("store i16 0" in kernel_text, "kernel.ll must encode success error_code 0")
    require("ret void" in kernel_text, "kernel.ll must return void for the out-parameter ABI")

    require("define i1 @frog_example06_copy_bool" in module_text, "module.ll must contain the bool proof function")
    require("public_output=%s" in module_text, "module.ll must print the public output")

    print("Example 06 native bool kernel bridge publication check: ok")
    print(f"manifest: {MANIFEST.relative_to(ROOT)}")
    print(f"kernel: {KERNEL_LL.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(f"Example 06 native bool kernel bridge publication check: FAILED: {exc}", file=sys.stderr)
        raise SystemExit(1)
