from __future__ import annotations

from Implementations.Reference.Runtime.python.cli import execute_example06_contract
from Implementations.Reference.Runtime.python.ui_runtime import (
    BooleanBrowserUiRuntime,
    build_runtime,
    default_example06_contract_path,
    default_example06_wfrog_path,
)


def test_python_example06_headless_matches_boolean_runtime_surface() -> None:
    artifact = execute_example06_contract(True)

    assert artifact["contract_ref"]["source_ref"]["example_id"] == "06_boolean_value_roundtrip"
    assert artifact["outputs"]["public"]["result"] is True
    assert artifact["outputs"]["ui"]["bool_input"] is True
    assert artifact["outputs"]["ui"]["bool_result"] is True


def test_python_example06_browser_ui_consumes_wfrog_realization() -> None:
    runtime = BooleanBrowserUiRuntime(
        contract_path=default_example06_contract_path(),
        wfrog_path=default_example06_wfrog_path(),
        open_browser=False,
    )
    html = runtime.render_html()

    assert "Python reference runtime" in html
    assert "boolean contract executor" in html
    assert "data-runtime-language=\"python\"" in html
    assert "data-execution-path=\"python_boolean_contract_executor\"" in html
    assert "/asset/boolean_rectangular_svg" in html
    assert "/asset/boolean_circular_svg" in html
    assert "data-frog-visual-law='wfrog-realization-state-map'" in html
    assert "data-svg-anchor='caption.anchor'" in html
    assert "left:0%;top:-22.2222%;transform:translateY(-50%);text-align:left;" in html
    assert "data-realization-crop-part='placement_bounds'" in html
    assert "data-frog-part=\"state_face\"" in html
    assert "--boolean-focus-color:#00ADEF;" in html
    assert "--boolean-focus-width:1px;" in html
    assert "data-frog-part=\"focus_ring\"" in html
    assert "outline:2px solid #2563eb" not in html
    assert "Current runtime snapshot" not in html

    runtime.runtime.execute(False)
    html = runtime.render_html()
    assert "data-toggle-target='true' aria-pressed='false'" in html
    assert "name='input_value' value='false' data-frog-boolean-state-input='bool_input'" in html
    assert "--boolean-fill:#ffffff;" in html


def test_python_runtime_builder_dispatches_example06() -> None:
    runtime = build_runtime(example="06", open_browser=False)

    assert isinstance(runtime, BooleanBrowserUiRuntime)
