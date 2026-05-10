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
    assert "--boolean-caption-left:50%;" in html
    assert "--boolean-inner-width:40px;" in html
    assert "--boolean-inner-height:40px;" in html
    assert "Current runtime snapshot" not in html

    runtime.runtime.execute(False)
    html = runtime.render_html()
    assert "value='true'" in html
    assert "--boolean-fill:#ef4444;" in html


def test_python_runtime_builder_dispatches_example06() -> None:
    runtime = build_runtime(example="06", open_browser=False)

    assert isinstance(runtime, BooleanBrowserUiRuntime)
