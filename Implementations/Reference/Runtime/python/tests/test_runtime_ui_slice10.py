from __future__ import annotations

import json
import urllib.parse
import urllib.request

from Implementations.Reference.Runtime.python.cli import execute_example10_contract
from Implementations.Reference.Runtime.python.ui_runtime import (
    ButtonBrowserUiRuntime,
    ButtonRuntimeCore,
    build_runtime,
    default_example10_contract_path,
    default_example10_wfrog_path,
)


def test_python_example10_headless_button_press_to_boolean() -> None:
    runtime = ButtonRuntimeCore(
        contract_path=default_example10_contract_path(),
        wfrog_path=default_example10_wfrog_path(),
    )

    artifact = runtime.execute(False)
    assert artifact["contract_ref"]["source_ref"]["example_id"] == "10_button_press_to_boolean"
    assert artifact["outputs"]["public"]["pressed"] is False
    assert artifact["outputs"]["ui"]["trigger_button"] is False
    assert artifact["outputs"]["ui"]["pressed_indicator"] is False

    artifact = runtime.execute(True)
    assert artifact["execution_summary"]["trigger_pressed"] is True
    assert artifact["outputs"]["public"]["pressed"] is True
    assert artifact["outputs"]["ui"]["trigger_button"] is False
    assert artifact["outputs"]["ui"]["pressed_indicator"] is True
    button = next(widget for widget in artifact["ui_runtime"]["widgets"] if widget["widget_id"] == "trigger_button")
    assert button["class_ref"] == "frog.widgets.button"
    assert button["runtime"]["event.pressed"] is True
    assert button["runtime"]["value"] is False


def test_python_example10_cli_contract_entrypoint() -> None:
    artifact = execute_example10_contract(True)

    assert artifact["outputs"]["public"]["pressed"] is True
    assert artifact["outputs"]["ui"]["pressed_indicator"] is True
    assert artifact["outputs"]["ui"]["trigger_button"] is False


def test_python_example10_browser_ui_consumes_default_svg_and_source_styles() -> None:
    runtime = ButtonBrowserUiRuntime(
        contract_path=default_example10_contract_path(),
        wfrog_path=default_example10_wfrog_path(),
        open_browser=False,
    )
    html = runtime.render_html()

    assert "Button Press to Boolean" in html
    assert "Python reference runtime" in html
    assert "button contract executor" in html
    assert 'data-runtime-language="python"' in html
    assert 'data-execution-path="python_button_contract_executor"' in html
    assert "data-class-ref='frog.widgets.button'" in html
    assert "data-class-ref='frog.widgets.boolean_indicator'" in html
    assert "data-asset-route='/asset/button_rectangular_svg'" in html
    assert "data-asset-route='/asset/boolean_circular_svg'" in html
    assert "data-frog-template=\"frog.realizations.default.button.rectangular\"" in html
    assert "class='button-skin'" in html
    assert "data-frog-asset-consumed='true'" in html
    assert "data-frog-part='caption' data-svg-anchor='caption.anchor'" in html
    assert "data-frog-part='state_text' data-svg-anchor='state_text.center'" in html
    assert "class='button-press-overlay' type='button'" in html
    assert "data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='trigger_pressed'" in html
    assert "data-frog-host-overlay='input' data-frog-align-to-part='face'" in html
    assert "--frog-button-face-stroke-width:1px;" in html
    assert "--frog-button-state-text-font-weight:400;" in html
    assert "--frog-button-caption-font-size:18px;" in html
    assert "--frog-button-caption-font-weight:600;" in html
    assert "--boolean-caption-font-size:18px;" in html
    assert "--boolean-text-font-size:12px;" in html
    assert "--boolean-text-font-weight:400;" in html
    assert "--boolean-inner-border-width:0px;" in html
    assert "fetch(\"/event\"" in html
    assert "pointerdown" in html
    assert "pointerup" in html
    assert ">OFF</span>" in html
    assert ">FALSE</span>" in html
    assert "font-size:14px" not in html
    assert "top:49px" not in html
    assert "type='submit'" not in html
    assert "missing-skin" not in html
    assert "fallback" not in html
    assert "Current runtime snapshot" not in html
    assert "<pre>" not in html

    runtime.run_once(True)
    html = runtime.render_html()
    assert "data-frog-visual-state='false'" in html
    assert ">OFF</span>" in html
    assert ">TRUE</span>" in html


def test_python_example10_event_endpoint_is_momentary() -> None:
    runtime = ButtonBrowserUiRuntime(
        contract_path=default_example10_contract_path(),
        wfrog_path=default_example10_wfrog_path(),
        open_browser=False,
    )
    httpd, thread = runtime.serve_in_thread()
    host, port = httpd.server_address
    try:
        body = urllib.parse.urlencode({"trigger_pressed": "true"}).encode("utf-8")
        request = urllib.request.Request(
            f"http://{host}:{port}/event",
            data=body,
            headers={"Content-Type": "application/x-www-form-urlencoded;charset=UTF-8"},
            method="POST",
        )
        with urllib.request.urlopen(request, timeout=5) as response:
            artifact = json.loads(response.read().decode("utf-8"))
        assert artifact["outputs"]["public"]["pressed"] is True
        assert artifact["outputs"]["ui"]["trigger_button"] is False
        assert artifact["outputs"]["ui"]["pressed_indicator"] is True

        body = urllib.parse.urlencode({"trigger_pressed": "false"}).encode("utf-8")
        request = urllib.request.Request(
            f"http://{host}:{port}/event",
            data=body,
            headers={"Content-Type": "application/x-www-form-urlencoded;charset=UTF-8"},
            method="POST",
        )
        with urllib.request.urlopen(request, timeout=5) as response:
            artifact = json.loads(response.read().decode("utf-8"))
        assert artifact["outputs"]["public"]["pressed"] is False
        assert artifact["outputs"]["ui"]["pressed_indicator"] is False
    finally:
        httpd.shutdown()
        httpd.server_close()
        thread.join(timeout=5)


def test_python_runtime_builder_dispatches_example10() -> None:
    runtime = build_runtime(example="10", open_browser=False)

    assert isinstance(runtime, ButtonBrowserUiRuntime)
