from __future__ import annotations

from pathlib import Path

from Implementations.Reference.Runtime.python.ui_runtime import (
    ButtonBrowserUiRuntime,
    ButtonRuntimeCore,
    default_example13_contract_path,
    default_example13_wfrog_path,
    default_example14_contract_path,
    default_example14_wfrog_path,
    default_example15_contract_path,
    default_example15_wfrog_path,
)


def _specs() -> list[dict[str, object]]:
    return [
        {
            "number": 13,
            "contract": default_example13_contract_path(),
            "wfrog": default_example13_wfrog_path(),
            "example_id": "13_button_latch_when_pressed",
            "unit_kind": "button_latch_when_pressed_ui_unit",
            "action": "latch_when_pressed",
            "title": "Button Latch When Pressed",
            "execution_path": "python_button_latch_when_pressed_contract_executor",
            "requires_release_binding": False,
        },
        {
            "number": 14,
            "contract": default_example14_contract_path(),
            "wfrog": default_example14_wfrog_path(),
            "example_id": "14_button_latch_when_released",
            "unit_kind": "button_latch_when_released_ui_unit",
            "action": "latch_when_released",
            "title": "Button Latch When Released",
            "execution_path": "python_button_latch_when_released_contract_executor",
            "requires_release_binding": True,
        },
        {
            "number": 15,
            "contract": default_example15_contract_path(),
            "wfrog": default_example15_wfrog_path(),
            "example_id": "15_button_latch_until_released",
            "unit_kind": "button_latch_until_released_ui_unit",
            "action": "latch_until_released",
            "title": "Button Latch Until Released",
            "execution_path": "python_button_latch_until_released_contract_executor",
            "requires_release_binding": True,
        },
    ]


def _public(artifact: dict[str, object]) -> bool:
    return bool(artifact["outputs"]["public"]["latched"])  # type: ignore[index]


def _ui(artifact: dict[str, object], widget_id: str) -> bool:
    return bool(artifact["outputs"]["ui"][widget_id])  # type: ignore[index]


def _summary(artifact: dict[str, object], key: str) -> bool:
    return bool(artifact["execution_summary"][key])  # type: ignore[index]


def _core(spec: dict[str, object]) -> ButtonRuntimeCore:
    return ButtonRuntimeCore(contract_path=spec["contract"], wfrog_path=spec["wfrog"])


def _html(spec: dict[str, object]) -> str:
    return ButtonBrowserUiRuntime(contract_path=spec["contract"], wfrog_path=spec["wfrog"], open_browser=False).render_html()


def test_python_latch_examples_consume_source_and_default_assets() -> None:
    for spec in _specs():
        runtime = _core(spec)
        assert runtime.contract["source_ref"]["example_id"] == spec["example_id"]
        assert runtime.unit["kind"] == spec["unit_kind"]
        assert runtime.unit["public_io"]["inputs"][0]["id"] == "trigger_value"
        assert runtime.unit["public_io"]["outputs"][0]["id"] == "latched"
        assert runtime.panel["title"] == spec["title"]
        assert runtime.widgets["trigger_button"]["class_ref"] == "frog.widgets.button"
        assert runtime.widgets["latched_indicator"]["class_ref"] == "frog.widgets.boolean_indicator"
        assert runtime.widgets["trigger_button"]["asset_id"] == "button_rectangular_svg"
        assert runtime.widgets["latched_indicator"]["asset_id"] == "boolean_circular_svg"
        assert runtime.widgets["trigger_button"]["properties"]["behavior.mechanical_action"] == spec["action"]
        assert runtime.widgets["trigger_button"]["properties"]["behavior.latch_reset_policy"] == "reset_on_natural_value_consumption"
        assert runtime.widgets["trigger_button"]["properties"]["behavior.output_pulse.duration_ms"] == 220
        assert runtime.widgets["trigger_button"]["properties"]["style.face.fill_color.false"] == "#e2e8f0"
        assert runtime.widgets["trigger_button"]["properties"]["style.face.fill_color.true"] == "#cbd5e1"
        assert runtime.widgets["trigger_button"]["properties"]["style.face.fill_color.hover_false"] == "#f1f5f9"
        assert runtime.widgets["trigger_button"]["properties"]["style.face.fill_color.hover_true"] == "#dbeafe"
        assert runtime.widgets["trigger_button"]["properties"]["style.face.border_width"] == "1px"
        assert runtime.widgets["trigger_button"]["properties"]["style.pressed.inset"] == "0px"
        assert runtime.widgets["trigger_button"]["properties"]["style.pressed.apply_when_value_true"] is True
        assert runtime.widgets["trigger_button"]["properties"]["style.pressed.apply_while_active"] is False
        assert runtime.widgets["trigger_button"]["properties"]["style.hover.apply_when_value_false_only"] is False
        assert runtime.widgets["latched_indicator"]["properties"]["interaction.read_only"] is True
        assert "Libraries" in str(runtime.asset_map["button_rectangular_svg"])
        assert "Default" in str(runtime.asset_map["button_rectangular_svg"])

        wfrog = Path(spec["wfrog"]).read_text(encoding="utf-8")
        assert "Libraries/Realizations/Default/button.default.wfrog" in wfrog
        assert "Libraries/Realizations/Default/boolean.default.wfrog" in wfrog
        assert "assets/button/templates/button_rectangular.svg" in wfrog
        assert "assets/boolean/templates/boolean_circular.svg" in wfrog
        assert "button_latch_reset_on_value_consumption" in wfrog
        if spec["requires_release_binding"]:
            assert "button_release_binding" in wfrog
        else:
            assert "button_release_binding" not in wfrog


def test_python_latch_examples_follow_press_release_read_contract() -> None:
    for spec in _specs():
        runtime = _core(spec)
        artifact = runtime.execution_artifact()
        assert _public(artifact) is False
        assert _ui(artifact, "trigger_button") is False
        assert _ui(artifact, "latched_indicator") is False

        if spec["action"] == "latch_when_pressed":
            artifact = runtime.press_control()
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is True
            assert _ui(artifact, "latched_indicator") is True

            artifact = runtime.read_program_value()
            assert _summary(artifact, "program_read_value") is True
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is False
            assert _ui(artifact, "latched_indicator") is True

            artifact = runtime.read_program_value()
            assert _summary(artifact, "program_read_value") is False
            assert _public(artifact) is False
            assert _ui(artifact, "latched_indicator") is False
        elif spec["action"] == "latch_when_released":
            artifact = runtime.press_control()
            assert _public(artifact) is False
            assert _ui(artifact, "trigger_button") is False
            assert _ui(artifact, "latched_indicator") is False

            artifact = runtime.release_control()
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is True
            assert _ui(artifact, "latched_indicator") is True

            artifact = runtime.read_program_value()
            assert _summary(artifact, "program_read_value") is True
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is False
            assert _ui(artifact, "latched_indicator") is True
        else:
            artifact = runtime.press_control()
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is True
            assert _ui(artifact, "latched_indicator") is True

            artifact = runtime.read_program_value()
            assert _summary(artifact, "program_read_value") is True
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is True
            assert _ui(artifact, "latched_indicator") is True

            artifact = runtime.release_control()
            assert _public(artifact) is False
            assert _ui(artifact, "trigger_button") is False
            assert _ui(artifact, "latched_indicator") is False


def test_python_latch_browser_surface_is_svg_and_source_owned() -> None:
    for spec in _specs():
        html = _html(spec)
        assert spec["title"] in html
        assert f"data-execution-path=\"{spec['execution_path']}\"" in html
        assert "data-widget-id='trigger_button'" in html
        assert "data-widget-id='latched_indicator'" in html
        assert "data-class-ref='frog.widgets.button'" in html
        assert "data-class-ref='frog.widgets.boolean_indicator'" in html
        assert "data-asset-route='/asset/button_rectangular_svg'" in html
        assert "data-asset-route='/asset/boolean_circular_svg'" in html
        assert "data-frog-asset-consumed='true'" in html
        assert "data-frog-visual-law='wfrog-realization-state-map'" in html
        assert f"data-frog-mechanical-action='{spec['action']}'" in html
        assert "data-frog-output-pulse-duration-ms='220'" in html
        assert "data-frog-pressed-applies-when-value-true='true'" in html
        assert "data-frog-pressed-applies-while-active='false'" in html
        assert "data-frog-hover-applies-when-value-false-only='false'" in html
        assert "data-frog-state-text-false='OFF'" in html
        assert "data-frog-state-text-true='ON'" in html
        assert "data-frog-button-face-fill-false='#e2e8f0'" in html
        assert "data-frog-button-face-fill-true='#cbd5e1'" in html
        assert "data-frog-button-face-hover-fill-false='#f1f5f9'" in html
        assert "data-frog-button-face-hover-fill-true='#dbeafe'" in html
        assert "--frog-button-pressed-inset:0px;" in html
        assert "publishEvent(\"press\")" in html
        assert "publishEvent(\"release\")" in html
        assert "publishEvent(\"read\")" in html
        assert "const latchPulseVisible = latchAction" in html
        assert "applyButton(latchPulseVisible ? true : buttonValue" in html
        assert "buttonWidget.style.setProperty(\"--frog-button-face-fill\", buttonProperty(\"frogButtonFaceFill\", value));" in html
        assert "program-read-action" in html
        assert ">OFF</span>" in html
        assert ">FALSE</span>" in html
        assert "fallback" not in html
        assert "type='checkbox'" not in html
        assert "type='submit'" not in html


def test_python_latch_browser_event_semantics() -> None:
    for spec in _specs():
        runtime = ButtonBrowserUiRuntime(contract_path=spec["contract"], wfrog_path=spec["wfrog"], open_browser=False)

        if spec["action"] == "latch_when_pressed":
            artifact = runtime.apply_event("press")
            assert _summary(artifact, "program_read_performed") is True
            assert _summary(artifact, "program_read_value") is True
            assert _summary(artifact, "button_physical_pressed") is True
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is False
            assert _ui(artifact, "latched_indicator") is True

            artifact = runtime.apply_event("release")
            assert _summary(artifact, "program_read_value") is False
            assert _summary(artifact, "button_physical_pressed") is False
            assert _public(artifact) is False
            assert _ui(artifact, "latched_indicator") is False
        elif spec["action"] == "latch_when_released":
            artifact = runtime.apply_event("press")
            assert _summary(artifact, "program_read_value") is False
            assert _summary(artifact, "button_physical_pressed") is True
            assert _public(artifact) is False
            assert _ui(artifact, "latched_indicator") is False

            artifact = runtime.apply_event("release")
            assert _summary(artifact, "program_read_value") is True
            assert _summary(artifact, "button_physical_pressed") is False
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is False
            assert _ui(artifact, "latched_indicator") is True
        else:
            artifact = runtime.apply_event("press")
            assert _summary(artifact, "program_read_value") is True
            assert _summary(artifact, "button_physical_pressed") is True
            assert _public(artifact) is True
            assert _ui(artifact, "trigger_button") is True
            assert _ui(artifact, "latched_indicator") is True

            artifact = runtime.apply_event("release")
            assert _summary(artifact, "program_read_value") is False
            assert _summary(artifact, "button_physical_pressed") is False
            assert _public(artifact) is False
            assert _ui(artifact, "trigger_button") is False
            assert _ui(artifact, "latched_indicator") is False
