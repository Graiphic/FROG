#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
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
           "12_button_switch_when_released.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path wfrog_path() {
    return repo_root() / "Examples" / "12_button_switch_when_released" / "ui" / "button_panel.wfrog";
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

void test_switch_when_released_runtime_core_consumes_source_and_default_assets() {
    frog::runtime::Slice10ButtonRuntimeCore runtime(contract_path(), wfrog_path());

    assert(runtime.contract.source_ref.example_id == "12_button_switch_when_released");
    assert(runtime.unit.kind == "button_switch_when_released_ui_unit");
    assert(runtime.unit.public_interface.inputs.front().id == "trigger_value");
    assert(runtime.unit.public_interface.inputs.front().binding_origin.value() == "widget.trigger_button.value");
    assert(runtime.unit.public_interface.outputs.front().id == "switched");
    assert(runtime.panel.panel_id == "main_panel");
    assert(runtime.panel.title == "Button Switch When Released");
    assert(runtime.package.kind == "widget_realization_package");

    assert(runtime.widgets.at("trigger_button").class_ref == "frog.widgets.button");
    assert(runtime.widgets.at("switched_indicator").class_ref == "frog.widgets.boolean_indicator");
    assert(runtime.widgets.at("trigger_button").asset_id == "button_rectangular_svg");
    assert(runtime.widgets.at("switched_indicator").asset_id == "boolean_circular_svg");
    assert(runtime.widgets.at("trigger_button").properties.at("binding.public_input_id").as_string() == "trigger_value");
    assert(runtime.widgets.at("switched_indicator").properties.at("binding.public_output_id").as_string() == "switched");
    assert(runtime.widgets.at("trigger_button").properties.at("behavior.mechanical_action").as_string() == "switch_when_released");
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.false_text").as_string() == "OFF");
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.true_text").as_string() == "ON");
    assert(runtime.widgets.at("trigger_button").properties.at("state_text.style.font_weight").as_string() == "400");
    assert(runtime.widgets.at("trigger_button").properties.at("style.face.border_width").as_string() == "1px");
    assert(runtime.widgets.at("switched_indicator").properties.at("interaction.read_only").as_bool());
    assert(runtime.widgets.at("switched_indicator").properties.at("state_text.style.font_weight").as_string() == "400");
    assert(std::filesystem::exists(runtime.asset_map.at("button_rectangular_svg")));
    assert(std::filesystem::exists(runtime.asset_map.at("boolean_circular_svg")));
    assert(runtime.asset_map.at("button_rectangular_svg").string().find("Libraries") != std::string::npos);
    assert(runtime.asset_map.at("button_rectangular_svg").string().find("Realizations") != std::string::npos);
    assert(runtime.asset_map.at("button_rectangular_svg").string().find("Default") != std::string::npos);

    const auto wfrog = read_text(wfrog_path());
    assert_contains(wfrog, "Libraries/Realizations/Default/button.default.wfrog");
    assert_contains(wfrog, "Libraries/Realizations/Default/boolean.default.wfrog");
    assert_contains(wfrog, "assets/button/templates/button_rectangular.svg");
    assert_contains(wfrog, "assets/boolean/templates/boolean_circular.svg");

    const auto button_svg = read_text(runtime.asset_map.at("button_rectangular_svg"));
    assert_contains(button_svg, "data-frog-template=\"frog.realizations.default.button.rectangular\"");
    assert_contains(button_svg, "data-frog-target-class=\"frog.widgets.button\"");
    assert_contains(button_svg, "data-frog-part=\"face\"");
    assert_contains(button_svg, "data-frog-part=\"state_face\"");
    assert_contains(button_svg, "data-frog-part=\"state_text\"");
    assert_contains(button_svg, "data-frog-anchor=\"face.bounds\"");
    assert_contains(button_svg, "data-frog-anchor=\"state_text.center\"");
    assert_contains(button_svg, "data-frog-bind=\"style.face.*\"");
    assert_contains(button_svg, "data-frog-bind=\"style.state_face.*\"");
}

void test_headless_switch_when_released_roundtrip() {
    frog::runtime::Slice10ButtonRuntimeCore runtime(contract_path(), wfrog_path());

    auto artifact = runtime.execution_artifact();
    assert(!artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("switched_indicator").as_bool());
    assert(artifact.as_object().at("execution_summary").as_object().at("mode").as_string() == "button_switch_when_released");

    artifact = runtime.execute(true);
    assert(artifact.as_object().at("execution_summary").as_object().at("trigger_pressed").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("switched_indicator").as_bool());

    artifact = runtime.execute(std::nullopt);
    assert(!artifact.as_object().at("execution_summary").as_object().at("trigger_pressed").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("switched_indicator").as_bool());

    artifact = runtime.execute(false);
    assert(artifact.as_object().at("execution_summary").as_object().at("trigger_pressed").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("switched_indicator").as_bool());
}

void test_switch_when_released_browser_ui_surface() {
    frog::runtime::ButtonBrowserUiRuntime runtime(contract_path(), wfrog_path());
    std::string html = runtime.render_html();

    assert_contains(html, "Button Switch When Released");
    assert_contains(html, "Example 12 - .frog switch_when_released Button value");
    assert_contains(html, "button switch contract executor");
    assert_contains(html, "none for Example 12");
    assert_contains(html, "data-execution-path='cpp_button_switch_when_released_contract_executor'");
    assert_contains(html, "data-widget-id='trigger_button'");
    assert_contains(html, "data-widget-id='switched_indicator'");
    assert_contains(html, "data-class-ref='frog.widgets.button'");
    assert_contains(html, "data-class-ref='frog.widgets.boolean_indicator'");
    assert_contains(html, "data-asset-route='/asset/button_rectangular_svg'");
    assert_contains(html, "data-asset-route='/asset/boolean_circular_svg'");
    assert_contains(html, "data-frog-template=\"frog.realizations.default.button.rectangular\"");
    assert_contains(html, "data-frog-asset-consumed='true'");
    assert_contains(html, "data-frog-visual-law='wfrog-realization-state-map'");
    assert_contains(html, "data-frog-mechanical-action='switch_when_released'");
    assert_contains(html, "data-frog-state-text-false='OFF'");
    assert_contains(html, "data-frog-state-text-true='ON'");
    assert_contains(html, "data-frog-part='state_text' data-svg-anchor='state_text.center'");
    assert_contains(html, "data-frog-part='face' data-frog-event='pressed' data-frog-public-input-id='trigger_value'");
    assert_contains(html, "data-frog-host-overlay='input' data-frog-align-to-part='face'");
    assert_contains(html, "name='trigger_value' value='true'");
    assert_contains(html, "aria-pressed='false'");
    assert_contains(html, "mechanicalAction === \"switch_when_released\"");
    assert_contains(html, "overlay.addEventListener(\"pointerup\", releaseToggle);");
    assert_contains(html, "setPressed(!(buttonWidget.dataset.currentValue === \"true\"));");
    assert_contains(html, "fetch(\"/event\"");
    assert_contains(html, ">OFF</span>");
    assert_contains(html, ">FALSE</span>");
    assert_not_contains(html, "fallback");
    assert_not_contains(html, "type='checkbox'");
    assert_not_contains(html, "type='submit'");

    runtime.run_once(true);
    html = runtime.render_html();
    assert_contains(html, "data-frog-visual-state='true'");
    assert_contains(html, "aria-pressed='true'");
    assert_contains(html, ">ON</span>");
    assert_contains(html, ">TRUE</span>");

    runtime.run_once(false);
    html = runtime.render_html();
    assert_contains(html, "data-frog-visual-state='false'");
    assert_contains(html, "aria-pressed='false'");
    assert_contains(html, ">OFF</span>");
    assert_contains(html, ">FALSE</span>");
}

} // namespace

int main() {
    test_switch_when_released_runtime_core_consumes_source_and_default_assets();
    test_headless_switch_when_released_roundtrip();
    test_switch_when_released_browser_ui_surface();
    std::cout << "slice12 Button switch_when_released C++ runtime acceptance passed" << std::endl;
    return 0;
}
