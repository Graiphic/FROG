from __future__ import annotations

import json
from pathlib import Path
import urllib.parse
import urllib.request

import pytest

from Implementations.Reference.Runtime.python.cli import execute_example10_contract
from Implementations.Reference.Runtime.python.ui_runtime import (
    ButtonBrowserUiRuntime,
    ButtonRuntimeCore,
    build_runtime,
    default_example10_contract_path,
    default_example10_wfrog_path,
    load_button_svg_geometry,
    render_button_widget,
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
    assert artifact["outputs"]["ui"]["trigger_button"] is True
    assert artifact["outputs"]["ui"]["pressed_indicator"] is True
    button = next(widget for widget in artifact["ui_runtime"]["widgets"] if widget["widget_id"] == "trigger_button")
    assert button["class_ref"] == "frog.widgets.button"
    assert button["runtime"]["event.pressed"] is True
    assert button["runtime"]["value"] is True


def test_python_example10_cli_contract_entrypoint() -> None:
    artifact = execute_example10_contract(True)

    assert artifact["outputs"]["public"]["pressed"] is True
    assert artifact["outputs"]["ui"]["pressed_indicator"] is True
    assert artifact["outputs"]["ui"]["trigger_button"] is True


def test_python_example10_browser_ui_consumes_default_svg_and_source_styles() -> None:
    runtime = ButtonBrowserUiRuntime(
        contract_path=default_example10_contract_path(),
        wfrog_path=default_example10_wfrog_path(),
        open_browser=False,
    )
    html = runtime.render_html()

    assert "Button Switch Until Released" in html
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
    assert "data-frog-mechanical-action='switch_until_released'" in html
    assert "data-frog-pressed-applies-when-value-true='true'" in html
    assert "data-frog-pressed-applies-while-active='false'" in html
    assert "data-frog-hover-applies-when-value-false-only='false'" in html
    assert "data-frog-part='caption' data-svg-anchor='caption.anchor'" in html
    assert "data-frog-part='state_text' data-svg-anchor='state_text.center'" in html
    assert "class='button-press-overlay' type='button'" in html
    assert "data-frog-part='button_face' data-frog-event='pressed' data-frog-public-input-id='trigger_pressed'" in html
    assert "data-frog-host-overlay='input' data-frog-align-to-part='button_face'" in html
    assert "--frog-button-face-fill:#e2e8f0;" in html
    assert "--frog-button-face-hover-fill:#f1f5f9;" in html
    assert "--frog-button-face-pressed-fill:#e2e8f0;" in html
    assert "--frog-button-face-stroke-width:1px;" in html
    assert "--frog-button-pressed-inset:0px;" in html
    assert "--frog-button-state-text-font-weight:400;" in html
    assert "--frog-button-caption-font-size:18px;" in html
    assert "--frog-button-caption-font-weight:600;" in html
    # Public circular Boolean defaults; do not certify the renderer's 18px fallback.
    assert "--boolean-caption-font-size:12px;" in html
    assert "--boolean-text-font-size:13px;" in html
    assert "--boolean-text-font-weight:400;" in html
    assert "--boolean-state-border-width:1px;" in html
    assert "fetch(\"/event\"" in html
    assert 'mechanicalAction !== "switch_until_released"' in html
    assert 'mechanicalAction !== "latch_until_released"' in html
    assert 'publishEvent("press")' in html
    assert 'publishEvent("release")' in html
    assert "pointerdown" in html
    assert "pointerup" in html
    assert ">OFF</span>" in html
    assert ">Off</span>" in html
    assert "font-size:14px" not in html
    assert "top:49px" not in html
    assert "type='submit'" not in html
    assert "missing-skin" not in html
    assert "fallback" not in html
    assert "Current runtime snapshot" not in html
    assert "<pre>" not in html

    runtime.run_once(True)
    html = runtime.render_html()
    assert "data-frog-visual-state='true'" in html
    assert ">ON</span>" in html
    assert ">On</span>" in html


def test_python_example10_event_endpoint_is_momentary() -> None:
    runtime = ButtonBrowserUiRuntime(
        contract_path=default_example10_contract_path(),
        wfrog_path=default_example10_wfrog_path(),
        open_browser=False,
    )
    httpd, thread = runtime.serve_in_thread()
    host, port = httpd.server_address
    try:
        body = urllib.parse.urlencode({"frog_event": "press", "trigger_pressed": "false"}).encode("utf-8")
        request = urllib.request.Request(
            f"http://{host}:{port}/event",
            data=body,
            headers={"Content-Type": "application/x-www-form-urlencoded;charset=UTF-8"},
            method="POST",
        )
        with urllib.request.urlopen(request, timeout=5) as response:
            artifact = json.loads(response.read().decode("utf-8"))
        assert artifact["outputs"]["public"]["pressed"] is True
        assert artifact["outputs"]["ui"]["trigger_button"] is True
        assert artifact["outputs"]["ui"]["pressed_indicator"] is True

        body = urllib.parse.urlencode({"frog_event": "release", "trigger_pressed": "true"}).encode("utf-8")
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


@pytest.mark.parametrize("part", ["button_face", "face"])
def test_button_face_geometry_and_overlay_follow_canonical_or_legacy_skin(tmp_path: Path, part: str) -> None:
    asset = tmp_path / "button.svg"
    asset.write_text(
        f'<svg viewBox="0 0 100 100"><rect id="{part}" data-frog-part="{part}" '
        'x="10" y="20" width="30" height="40" /></svg>', encoding="utf-8",
    )
    geometry = load_button_svg_geometry(asset)
    assert [geometry[f"face_{key}"] for key in ("x", "y", "width", "height")] == [10, 20, 30, 40]
    rendered = render_button_widget({
        "widget_id": "button", "class_ref": "frog.widgets.button", "role": "control",
        "layout": {}, "runtime": {},
    }, asset)
    assert f"data-frog-part='{part}' data-frog-event='pressed'" in rendered
    assert f"data-frog-host-overlay='input' data-frog-align-to-part='{part}'" in rendered
    assert "left:10%;top:20%;width:30%;height:40%;" in rendered


def test_button_face_canonical_states_win_and_legacy_only_documents_remain_readable() -> None:
    browser = build_runtime(example="10", open_browser=False)
    core = browser.runtime
    widget = next(w for w in core.execution_artifact()["ui_runtime"]["widgets"] if w["widget_id"] == "trigger_button")
    asset = core.asset_map["button_rectangular_svg"]
    source_runtime = widget["runtime"]
    assert source_runtime["style.button_face.fill_color.hover_false"] == "#f1f5f9"
    states = {
        "false": "#112233", "true": "#223344", "hover_false": "#334455",
        "hover_true": "#445566", "pressed_false": "#556677", "pressed_true": "#667788",
    }
    for prefix in ("style.button_face", "style.face"):
        runtime = {key: value for key, value in source_runtime.items() if not key.startswith(("style.button_face.", "style.face."))}
        runtime.update({f"{prefix}.fill_color.{state}": color for state, color in states.items()})
        if prefix == "style.button_face":
            runtime["style.face.fill_color.true"] = "#abcdef"
        for value in (False, True):
            runtime.update(value=value, pressed=value)
            rendered = render_button_widget({**widget, "runtime": runtime}, asset)
            state = "true" if value else "false"
            assert f"--frog-button-face-fill:{states[state]};" in rendered
            assert f"--frog-button-face-hover-fill:{states['hover_' + state]};" in rendered
            assert f"--frog-button-face-pressed-fill:{states['pressed_' + state]};" in rendered
            assert f"data-frog-button-face-fill-{state}='{states[state]}'" in rendered
        # Missing canonical states inherit the canonical base, not stale legacy values.
        if prefix == "style.button_face":
            del runtime["style.button_face.fill_color.true"]
            assert "--frog-button-face-fill:#112233;" in render_button_widget({**widget, "runtime": runtime}, asset)
    html = browser.render_html()
    marker_selector = ":is([data-frog-part='button_face'],[data-frog-part='face'])"
    assert f".button-press-overlay{marker_selector}[data-frog-host-overlay='input']" in html
    assert f".button-skin {marker_selector}{{fill:var(--frog-button-face-fill)" in html
    assert f".button-skin {marker_selector}{{fill:var(--frog-button-face-hover-fill)" in html
    assert f".button-skin {marker_selector}{{fill:var(--frog-button-face-pressed-fill)" in html
    assert "data-frog-template=\"frog.realizations.default.boolean.circular\"" in html
    assert "class='boolean-skin missing-skin'" not in html
