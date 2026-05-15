from __future__ import annotations

import json
import urllib.parse
import urllib.request

from Implementations.Reference.Runtime.python.cli import execute_example11_contract
from Implementations.Reference.Runtime.python.ui_runtime import (
    ButtonBrowserUiRuntime,
    ButtonRuntimeCore,
    build_runtime,
    default_example11_contract_path,
    default_example11_wfrog_path,
)


def test_python_example11_headless_button_switch_when_pressed() -> None:
    runtime = ButtonRuntimeCore(
        contract_path=default_example11_contract_path(),
        wfrog_path=default_example11_wfrog_path(),
    )

    artifact = runtime.execution_artifact()
    assert artifact["contract_ref"]["source_ref"]["example_id"] == "11_button_switch_when_pressed"
    assert artifact["outputs"]["public"]["switched"] is False
    assert artifact["outputs"]["ui"]["trigger_button"] is False
    assert artifact["outputs"]["ui"]["switched_indicator"] is False

    artifact = runtime.execute(True)
    assert artifact["execution_summary"]["mode"] == "button_switch_when_pressed"
    assert artifact["execution_summary"]["trigger_pressed"] is True
    assert artifact["outputs"]["public"]["switched"] is True
    assert artifact["outputs"]["ui"]["trigger_button"] is True
    assert artifact["outputs"]["ui"]["switched_indicator"] is True

    artifact = runtime.execute(None)
    assert artifact["execution_summary"]["trigger_pressed"] is False
    assert artifact["outputs"]["public"]["switched"] is True
    assert artifact["outputs"]["ui"]["trigger_button"] is True
    assert artifact["outputs"]["ui"]["switched_indicator"] is True

    artifact = runtime.execute(False)
    assert artifact["execution_summary"]["trigger_pressed"] is True
    assert artifact["outputs"]["public"]["switched"] is False
    assert artifact["outputs"]["ui"]["trigger_button"] is False
    assert artifact["outputs"]["ui"]["switched_indicator"] is False


def test_python_example11_cli_contract_entrypoint() -> None:
    artifact = execute_example11_contract(True)

    assert artifact["outputs"]["public"]["switched"] is True
    assert artifact["outputs"]["ui"]["switched_indicator"] is True
    assert artifact["outputs"]["ui"]["trigger_button"] is True


def test_python_example11_browser_ui_consumes_default_svg_and_source_styles() -> None:
    runtime = ButtonBrowserUiRuntime(
        contract_path=default_example11_contract_path(),
        wfrog_path=default_example11_wfrog_path(),
        open_browser=False,
    )
    html = runtime.render_html()

    assert "Button Switch When Pressed" in html
    assert "Example 11 - .frog switch_when_pressed Button value" in html
    assert "Python reference runtime" in html
    assert "button switch contract executor" in html
    assert 'data-execution-path="python_button_switch_when_pressed_contract_executor"' in html
    assert "data-widget-id='trigger_button'" in html
    assert "data-widget-id='switched_indicator'" in html
    assert "data-widget-id='pressed_indicator'" not in html
    assert "data-class-ref='frog.widgets.button'" in html
    assert "data-class-ref='frog.widgets.boolean_indicator'" in html
    assert "data-asset-route='/asset/button_rectangular_svg'" in html
    assert "data-asset-route='/asset/boolean_circular_svg'" in html
    assert "data-frog-template=\"frog.realizations.default.button.rectangular\"" in html
    assert "data-frog-asset-consumed='true'" in html
    assert "data-frog-visual-law='wfrog-realization-state-map'" in html
    assert "data-frog-mechanical-action='switch_when_pressed'" in html
    assert "data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='trigger_value'" in html
    assert "name='trigger_value' value='true'" in html
    assert "--frog-button-face-stroke-width:1px;" in html
    assert "--frog-button-state-text-font-weight:400;" in html
    assert "--boolean-text-font-weight:400;" in html
    assert ".boolean-indicator[data-class-ref='frog.widgets.boolean_indicator']" in html
    assert 'mechanicalAction === "switch_when_pressed"' in html
    assert 'setPressed(!(buttonWidget.dataset.currentValue === "true"));' in html
    assert "fetch(\"/event\"" in html
    assert "pointerdown" in html
    assert ">OFF</span>" in html
    assert ">FALSE</span>" in html
    assert "fallback" not in html
    assert "type='checkbox'" not in html
    assert "type='submit'" not in html
    assert "Current runtime snapshot" not in html
    assert "<pre>" not in html

    runtime.run_once(True)
    html = runtime.render_html()
    assert "data-frog-visual-state='true'" in html
    assert "aria-pressed='true'" in html
    assert ">ON</span>" in html
    assert ">TRUE</span>" in html


def test_python_example11_event_endpoint_toggles_stored_value() -> None:
    runtime = ButtonBrowserUiRuntime(
        contract_path=default_example11_contract_path(),
        wfrog_path=default_example11_wfrog_path(),
        open_browser=False,
    )
    httpd, thread = runtime.serve_in_thread()
    host, port = httpd.server_address
    try:
        body = urllib.parse.urlencode({"trigger_value": "true"}).encode("utf-8")
        request = urllib.request.Request(
            f"http://{host}:{port}/event",
            data=body,
            headers={"Content-Type": "application/x-www-form-urlencoded;charset=UTF-8"},
            method="POST",
        )
        with urllib.request.urlopen(request, timeout=5) as response:
            artifact = json.loads(response.read().decode("utf-8"))
        assert artifact["outputs"]["public"]["switched"] is True
        assert artifact["outputs"]["ui"]["trigger_button"] is True
        assert artifact["outputs"]["ui"]["switched_indicator"] is True

        body = urllib.parse.urlencode({"trigger_value": "false"}).encode("utf-8")
        request = urllib.request.Request(
            f"http://{host}:{port}/event",
            data=body,
            headers={"Content-Type": "application/x-www-form-urlencoded;charset=UTF-8"},
            method="POST",
        )
        with urllib.request.urlopen(request, timeout=5) as response:
            artifact = json.loads(response.read().decode("utf-8"))
        assert artifact["outputs"]["public"]["switched"] is False
        assert artifact["outputs"]["ui"]["trigger_button"] is False
        assert artifact["outputs"]["ui"]["switched_indicator"] is False
    finally:
        httpd.shutdown()
        httpd.server_close()
        thread.join(timeout=5)


def test_python_runtime_builder_dispatches_example11() -> None:
    runtime = build_runtime(example="11", open_browser=False)

    assert isinstance(runtime, ButtonBrowserUiRuntime)
