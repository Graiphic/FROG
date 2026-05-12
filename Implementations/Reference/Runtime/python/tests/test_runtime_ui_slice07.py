from __future__ import annotations

from Implementations.Reference.Runtime.python.cli import execute_example07_contract
from Implementations.Reference.Runtime.python.ui_runtime import (
    StringBrowserUiRuntime,
    build_runtime,
    default_example07_contract_path,
    default_example07_wfrog_path,
)


def test_python_example07_headless_matches_string_runtime_surface() -> None:
    artifact = execute_example07_contract("hello world")

    assert artifact["contract_ref"]["source_ref"]["example_id"] == "07_string_value_roundtrip"
    assert artifact["outputs"]["public"]["result_text"] == "hello world"
    assert artifact["outputs"]["ui"]["str_input"] == "hello world"
    assert artifact["outputs"]["ui"]["str_result"] == "hello world"


def test_python_example07_browser_ui_consumes_wfrog_realization() -> None:
    runtime = StringBrowserUiRuntime(
        contract_path=default_example07_contract_path(),
        wfrog_path=default_example07_wfrog_path(),
        open_browser=False,
    )
    html = runtime.render_html()

    assert "Python reference runtime" in html
    assert "string contract executor" in html
    assert "data-runtime-language=\"python\"" in html
    assert "data-execution-path=\"python_string_contract_executor\"" in html
    assert "/asset/string_rectangular_svg" in html
    assert "data-frog-visual-law='wfrog-realization-state-map'" in html
    assert "--frog-string-text-region-fill-hover:#eef6ff;" in html
    assert "--frog-string-text-region-stroke-hover:#2563eb;" in html
    assert ".string-control:hover .string-skin svg" in html
    assert "string-card" not in html
    assert "Current runtime snapshot" not in html
    assert "<pre>" not in html
    assert "outline:2px solid #2563eb" not in html

    runtime.runtime.execute("edited hello")
    html = runtime.render_html()
    assert "edited hello" in html


def test_python_runtime_builder_dispatches_example07() -> None:
    runtime = build_runtime(example="07", open_browser=False)

    assert isinstance(runtime, StringBrowserUiRuntime)
