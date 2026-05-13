from __future__ import annotations

from Implementations.Reference.Runtime.python.cli import execute_example08_contract
from Implementations.Reference.Runtime.python.ui_runtime import (
    EnumBrowserUiRuntime,
    build_runtime,
    default_example08_contract_path,
    default_example08_wfrog_path,
)


def test_python_example08_headless_matches_enum_runtime_surface() -> None:
    artifact = execute_example08_contract("fault")

    assert artifact["contract_ref"]["source_ref"]["example_id"] == "08_enum_value_roundtrip"
    assert artifact["outputs"]["public"]["result_mode"] == "fault"
    assert artifact["outputs"]["ui"]["mode_input"] == "fault"
    assert artifact["outputs"]["ui"]["mode_result"] == "fault"
    assert artifact["execution_summary"]["result_text"] == "Fault"


def test_python_example08_browser_ui_consumes_frog_instance_and_wfrog_realization() -> None:
    runtime = EnumBrowserUiRuntime(
        contract_path=default_example08_contract_path(),
        wfrog_path=default_example08_wfrog_path(),
        open_browser=False,
    )
    html = runtime.render_html()

    assert "Python reference runtime" in html
    assert "enum contract executor" in html
    assert 'data-runtime-language="python"' in html
    assert 'data-execution-path="python_enum_contract_executor"' in html
    assert "/asset/enum_rectangular_ring_svg" in html
    assert "data-frog-visual-law='wfrog-realization-state-map'" in html
    assert "class='enum-value-display-overlay enum-display-button'" in html
    assert "class='enum-selector-overlay enum-selector-button'" in html
    assert "class='enum-dropdown'" in html
    assert "--frog-enum-selector-hover-fill:#60a5fa;" in html
    assert "--frog-enum-dropdown-option-hover-fill:#2563eb;" in html
    assert "enum-card" not in html
    assert "Current runtime snapshot" not in html
    assert "<pre>" not in html

    runtime.runtime.execute("idle")
    html = runtime.render_html()
    assert "Idle" in html


def test_python_runtime_builder_dispatches_example08() -> None:
    runtime = build_runtime(example="08", open_browser=False)

    assert isinstance(runtime, EnumBrowserUiRuntime)
