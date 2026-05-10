from __future__ import annotations

import shutil
from pathlib import Path

import pytest

from Implementations.Reference.Runtime.build_native_kernel_library import build_native_kernel_library
from Implementations.Reference.Runtime.python.native_kernel import (
    load_native_bool_kernel_bridge,
    load_native_kernel_bridge,
)
from Implementations.Reference.Runtime.python.runtime_core import Slice05RuntimeCore, find_repo_root
from Implementations.Reference.Runtime.python.ui_runtime import (
    BrowserUiRuntime,
    BooleanBrowserUiRuntime,
    BooleanRuntimeCore,
    default_example06_contract_path,
    default_example06_wfrog_path,
)


ROOT = find_repo_root(Path(__file__).resolve())
EXAMPLE05_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json"
EXAMPLE06_MANIFEST = ROOT / "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json"


def _build_library(tmp_path: Path, example: str, manifest: Path) -> Path:
    if shutil.which("clang") is None:
        pytest.skip("clang is required for the dynamic native kernel bridge test")
    suffix = ".dll" if __import__("sys").platform == "win32" else ".dylib" if __import__("sys").platform == "darwin" else ".so"
    return build_native_kernel_library(
        manifest_path=manifest,
        output_path=tmp_path / f"example{example}_kernel{suffix}",
    )


def test_python_dynamic_native_kernel_bridge_executes_example05(tmp_path: Path) -> None:
    library = _build_library(tmp_path, "05", EXAMPLE05_MANIFEST)
    bridge = load_native_kernel_bridge(EXAMPLE05_MANIFEST, library)

    assert bridge.run(6).result == 30

    runtime = Slice05RuntimeCore()
    artifact = runtime.execute_with_native_kernel_bridge(bridge, control_value=6)

    assert artifact["outputs"]["public"]["result"] == 30
    assert artifact["outputs"]["ui"]["ind_result"] == 30

    ui = BrowserUiRuntime(native_kernel_bridge=bridge, open_browser=False)
    html = ui.render_html()
    assert "native kernel bridge" in html
    assert "LLVM native kernel artifact" in html


def test_python_dynamic_native_kernel_bridge_executes_example06(tmp_path: Path) -> None:
    library = _build_library(tmp_path, "06", EXAMPLE06_MANIFEST)
    bridge = load_native_bool_kernel_bridge(EXAMPLE06_MANIFEST, library)

    assert bridge.run(True).result is True
    assert bridge.run(False).result is False

    core = BooleanRuntimeCore()
    artifact = core.execute_with_native_kernel_bridge(bridge, True)

    assert artifact["outputs"]["public"]["result"] is True
    assert artifact["outputs"]["ui"]["bool_result"] is True

    ui = BooleanBrowserUiRuntime(
        contract_path=default_example06_contract_path(),
        wfrog_path=default_example06_wfrog_path(),
        native_kernel_bridge=bridge,
        open_browser=False,
    )
    html = ui.render_html()
    assert "native kernel bridge" in html
    assert "LLVM native bool kernel artifact" in html
    assert 'data-compiler-backend="llvm"' in html
    assert 'data-execution-path="native_kernel_bridge"' in html
