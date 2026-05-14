#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "json.hpp"
#include "runtime.hpp"
#include "ui.hpp"

namespace {

std::filesystem::path repo_root() {
    return frog::runtime::find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
}

std::filesystem::path contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "10_button_press_to_boolean.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path wfrog_path() {
    return repo_root() / "Examples" / "10_button_press_to_boolean" / "ui" / "button_panel.wfrog";
}

std::filesystem::path default_button_wfrog_path() {
    return repo_root() / "Libraries" / "Realizations" / "Default" / "button.default.wfrog";
}

std::string read_text(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    assert(input);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

void assert_not_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) == std::string::npos);
}

void test_button_runtime_core_consumes_frog_instance_and_wfrog_assets() {
    frog::runtime::Slice10ButtonRuntimeCore runtime(contract_path(), wfrog_path());
    assert(runtime.unit.kind == "button_press_to_boolean_ui_unit");
    assert(runtime.panel.panel_id == "main_panel");
    assert(runtime.package.kind == "widget_realization_package");
    assert(runtime.widgets.at("trigger_button").class_ref == "frog.widgets.button");
    assert(runtime.widgets.at("pressed_indicator").class_ref == "frog.widgets.boolean_indicator");
    assert(runtime.widgets.at("trigger_button").asset_id == "button_rectangular_svg");
    assert(runtime.widgets.at("pressed_indicator").asset_id == "boolean_circular_svg");
    assert(runtime.widgets.at("trigger_button").class_ref != "frog.widgets.boolean_control");
    assert(runtime.widgets.at("trigger_button").asset_id != "boolean_rectangular_svg");
    assert(runtime.widgets.at("trigger_button").properties.at("behavior.mechanical_action").as_string() == "switch_until_released");
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.false_text").as_string() == "OFF");
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.true_text").as_string() == "ON");
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.style.font_weight").as_string() == "400");
    assert(runtime.widgets.at("trigger_button").properties.at("style.face.border_width").as_string() == "1px");
    assert(runtime.widgets.at("pressed_indicator").class_ref == "frog.widgets.boolean_indicator");
    assert(runtime.widgets.at("pressed_indicator").properties.at("caption.style.font_size").as_string() == "18px");
    assert(runtime.widgets.at("pressed_indicator").properties.at("caption.style.font_weight").as_string() == "600");
    assert(runtime.widgets.at("pressed_indicator").properties.at("state_text.style.font_weight").as_string() == "400");
    assert(runtime.widgets.at("pressed_indicator").properties.at("style.inner.border_width").as_string() == "0px");
    assert(std::filesystem::exists(runtime.asset_map.at("button_rectangular_svg")));
    assert(std::filesystem::exists(runtime.asset_map.at("boolean_circular_svg")));
    assert(runtime.asset_map.at("button_rectangular_svg").string().find("Libraries") != std::string::npos);
    assert(runtime.asset_map.at("button_rectangular_svg").string().find("Realizations") != std::string::npos);
    assert(runtime.asset_map.at("button_rectangular_svg").string().find("Default") != std::string::npos);

    const auto button_svg = read_text(runtime.asset_map.at("button_rectangular_svg"));
    assert_contains(button_svg, "data-frog-template=\"frog.realizations.default.button.rectangular\"");
    assert_contains(button_svg, "data-frog-target-class=\"frog.widgets.button\"");
    assert_contains(button_svg, "data-frog-part=\"face\"");
    assert_contains(button_svg, "data-frog-part=\"state_face\"");
    assert_contains(button_svg, "data-frog-part=\"state_text\"");
    assert_contains(button_svg, "data-frog-anchor=\"caption.anchor\"");
    assert_contains(button_svg, "data-frog-anchor=\"face.bounds\"");
    assert_contains(button_svg, "data-frog-anchor=\"state_face.bounds\"");
    assert_contains(button_svg, "data-frog-anchor=\"state_text.center\"");
    assert_contains(button_svg, "data-frog-bind=\"style.face.*\"");
    assert_contains(button_svg, "data-frog-bind=\"style.state_face.*\"");
    assert_contains(button_svg, "--frog-button-caption-font-weight");
    assert_contains(button_svg, "--frog-button-face-stroke-width");
    assert_contains(button_svg, "--frog-button-state-face-stroke");

    const auto default_button_wfrog = read_text(default_button_wfrog_path());
    assert_contains(default_button_wfrog, "frog.realizations.default.button.face.bounds");
    assert_contains(default_button_wfrog, "frog.realizations.default.button.state_face.bounds");
    assert_contains(default_button_wfrog, "frog.realizations.default.button.state_text.center");
    assert_contains(default_button_wfrog, "\"host_overlay_alignment\": true");
    assert_contains(default_button_wfrog, "\"style.face.*\"");
    assert_contains(default_button_wfrog, "\"caption.style.font_weight\"");
}

void test_headless_button_roundtrip() {
    frog::runtime::Slice10ButtonRuntimeCore runtime(contract_path(), wfrog_path());

    auto artifact = runtime.execute(false);
    assert(!artifact.as_object().at("outputs").as_object().at("public").as_object().at("pressed").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("pressed_indicator").as_bool());

    artifact = runtime.execute(true);
    assert(artifact.as_object().at("execution_summary").as_object().at("trigger_pressed").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("pressed").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("pressed_indicator").as_bool());
    const auto& widgets = artifact.as_object().at("ui_runtime").as_object().at("widgets").as_array();
    bool saw_button_event = false;
    for (const auto& widget_value : widgets) {
        const auto& widget = widget_value.as_object();
        if (widget.at("widget_id").as_string() == "trigger_button") {
            const auto& runtime_fields = widget.at("runtime").as_object();
            assert(!runtime_fields.at("value").as_bool());
            assert(runtime_fields.at("event.pressed").as_bool());
            saw_button_event = true;
        }
    }
    assert(saw_button_event);
}

void test_button_browser_ui_surface() {
    frog::runtime::ButtonBrowserUiRuntime runtime(contract_path(), wfrog_path());
    std::string html = runtime.render_html();

    assert_contains(html, "Button Press to Boolean");
    assert_contains(html, "class='front-panel'");
    assert_contains(html, "style='width:420px;height:150px;'");
    assert_contains(html, "data-panel-id='main_panel'");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "button contract executor");
    assert_contains(html, "none for Example 10");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='none'");
    assert_contains(html, "data-execution-path='cpp_button_contract_executor'");
    assert_contains(html, "data-widget-id='trigger_button'");
    assert_contains(html, "data-widget-id='pressed_indicator'");
    assert_contains(html, "data-class-ref='frog.widgets.button'");
    assert_contains(html, "data-class-ref='frog.widgets.boolean_indicator'");
    assert_contains(html, "data-asset-route='/asset/button_rectangular_svg'");
    assert_contains(html, "data-asset-route='/asset/boolean_circular_svg'");
    assert_not_contains(html, "data-class-ref='frog.widgets.boolean_control'");
    assert_not_contains(html, "data-asset-route='/asset/boolean_rectangular_svg'");
    assert_contains(html, "data-frog-template=\"frog.realizations.default.button.rectangular\"");
    assert_contains(html, "class='button-skin'");
    assert_contains(html, "data-frog-asset-consumed='true'");
    assert_contains(html, "data-frog-visual-law='wfrog-realization-state-map'");
    assert_contains(html, "data-realization-variant='rectangular'");
    assert_contains(html, "data-frog-visual-state='false'");
    assert_contains(html, "data-frog-hover-state='hover_false'");
    assert_contains(html, "data-frog-pressed-state='pressed_false'");
    assert_contains(html, "data-frog-transition-state='transition_false_to_true'");
    assert_contains(html, "data-frog-state-text-false='OFF'");
    assert_contains(html, "data-frog-state-text-true='ON'");
    assert_contains(html, "data-frog-part='caption' data-svg-anchor='caption.anchor'");
    assert_contains(html, "data-frog-part='state_text' data-svg-anchor='state_text.center'");
    assert_contains(html, "class='button-press-overlay' type='button'");
    assert_contains(html, "data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='trigger_pressed'");
    assert_contains(html, "data-frog-host-overlay='input' data-frog-align-to-part='face'");
    assert_contains(html, "name='trigger_pressed' value='true'");
    assert_contains(html, "aria-pressed='false'");
    assert_contains(html, "data-frog-fill-false='#ef4444'");
    assert_contains(html, "data-frog-fill-true='#22c55e'");
    assert_contains(html, "data-frog-text-false='FALSE'");
    assert_contains(html, "data-frog-text-true='TRUE'");
    assert_contains(html, "--frog-button-face-fill:#e2e8f0;");
    assert_contains(html, "--frog-button-face-hover-fill:#dbeafe;");
    assert_contains(html, "--frog-button-face-pressed-fill:#bfdbfe;");
    assert_contains(html, "--frog-button-face-stroke:#334155;");
    assert_contains(html, "--frog-button-face-stroke-width:1px;");
    assert_contains(html, "--frog-button-state-face-fill:transparent;");
    assert_contains(html, "--frog-button-state-face-hover-fill:transparent;");
    assert_contains(html, "--frog-button-state-face-pressed-fill:transparent;");
    assert_contains(html, "--frog-button-state-face-stroke-width:0px;");
    assert_contains(html, "--frog-button-focus-color:#2563eb;");
    assert_contains(html, "--frog-button-focus-width:3px;");
    assert_contains(html, "--frog-button-pressed-inset:2px;");
    assert_contains(html, "--frog-button-caption-font-size:18px;");
    assert_contains(html, "--frog-button-caption-font-weight:600;");
    assert_contains(html, "--frog-button-caption-font-family:system-ui, Segoe UI, Arial, sans-serif;");
    assert_contains(html, "--frog-button-state-text-font-weight:400;");
    assert_contains(html, "--boolean-text-font-size:12px;");
    assert_contains(html, "--boolean-text-font-weight:400;");
    assert_contains(html, "--boolean-caption-font-size:18px;");
    assert_contains(html, "--boolean-caption-font-weight:600;");
    assert_contains(html, "--boolean-caption-font-family:system-ui, Segoe UI, Arial, sans-serif;");
    assert_contains(html, "--boolean-inner-border-width:0px;");
    assert_contains(html, "data-frog-part='state_text' data-svg-anchor='state_text.center' style='left:50.00%;top:62.50%;'");
    assert_contains(html, "font-size:var(--boolean-caption-font-size)");
    assert_contains(html, "border:var(--boolean-inner-border-width) solid var(--boolean-inner-border)");
    assert_contains(html, ".button-widget:has(.button-press-overlay:hover) .button-skin [data-frog-part='face']");
    assert_contains(html, ".button-widget:has(.button-press-overlay:active) .button-skin [data-frog-part='state_face']");
    assert_contains(html, "fetch(\"/event\"");
    assert_contains(html, "pointerdown");
    assert_contains(html, "pointerup");
    assert_contains(html, "mousedown");
    assert_contains(html, "touchstart");
    assert_contains(html, "buttonWidget.querySelector(\".button-state-overlay[data-frog-part='state_text']\")");
    assert_contains(html, "buttonStateText.textContent = buttonProperty(\"frogStateText\", value);");
    assert_not_contains(html, "font-size:14px");
    assert_not_contains(html, "top:49px");
    assert_contains(html, "setPressed(false)");
    assert_contains(html, ">OFF</span>");
    assert_contains(html, "aria-readonly='true'");
    assert_contains(html, "/state.json");
    assert_not_contains(html, "missing-skin");
    assert_not_contains(html, "fallback");
    assert_not_contains(html, "type='checkbox'");
    assert_not_contains(html, "type='submit'");
    assert_not_contains(html, "Current runtime snapshot");
    assert_not_contains(html, "<pre>");

    runtime.run_once(true);
    html = runtime.render_html();
    assert_contains(html, "data-frog-visual-state='true'");
    assert_contains(html, "data-frog-hover-state='hover_true'");
    assert_contains(html, "data-frog-pressed-state='pressed_true'");
    assert_contains(html, "data-frog-transition-state='transition_false_to_true'");
    assert_contains(html, "aria-pressed='false'");
    assert_contains(html, "--frog-button-face-fill:#e2e8f0;");
    assert_contains(html, "--frog-button-face-hover-fill:#dbeafe;");
    assert_contains(html, "--frog-button-face-pressed-fill:#bfdbfe;");
    assert_contains(html, "--frog-button-face-stroke:#334155;");
    assert_contains(html, "--frog-button-state-face-fill:transparent;");
    assert_contains(html, "--frog-button-state-face-hover-fill:transparent;");
    assert_contains(html, "--frog-button-state-face-pressed-fill:transparent;");
    assert_contains(html, ">OFF</span>");
    assert_contains(html, ">TRUE</span>");
}

} // namespace

int main() {
    test_button_runtime_core_consumes_frog_instance_and_wfrog_assets();
    test_headless_button_roundtrip();
    test_button_browser_ui_surface();
    std::cout << "slice10 Button C++ runtime acceptance passed" << std::endl;
    return 0;
}
