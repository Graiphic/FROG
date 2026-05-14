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
           "08_enum_value_roundtrip.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path wfrog_path() {
    return repo_root() / "Examples" / "08_enum_value_roundtrip" / "ui" / "enum_panel.wfrog";
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

void test_enum_runtime_core_consumes_frog_instance_and_wfrog_assets() {
    frog::runtime::Slice08EnumRuntimeCore runtime(contract_path(), wfrog_path());
    assert(runtime.unit.kind == "enum_value_roundtrip_ui_unit");
    assert(runtime.panel.panel_id == "main_panel");
    assert(runtime.package.kind == "widget_realization_package");
    assert(runtime.package.front_panels.empty());
    assert(runtime.widgets.at("mode_input").class_ref == "frog.widgets.enum_control");
    assert(runtime.widgets.at("mode_result").class_ref == "frog.widgets.enum_indicator");
    assert(runtime.widgets.at("mode_input").layout.as_object().at("y").as_i64() == 0);
    assert(runtime.widgets.at("mode_result").layout.as_object().at("y").as_i64() == 0);
    assert(runtime.widgets.at("mode_input").asset_id == "enum_rectangular_ring_svg");
    assert(runtime.widgets.at("mode_result").asset_id == "enum_rectangular_ring_svg");
    assert(std::filesystem::exists(runtime.asset_map.at("enum_rectangular_ring_svg")));

    const auto enum_svg = read_text(runtime.asset_map.at("enum_rectangular_ring_svg"));
    assert_contains(enum_svg, "data-frog-template=\"frog.realizations.default.enum.rectangular_ring\"");
    assert_contains(enum_svg, "data-frog-part=\"value_display\"");
    assert_contains(enum_svg, "data-frog-part=\"selector_face\"");
    assert_contains(enum_svg, "--frog-enum-frame-display");
    assert_contains(enum_svg, "--frog-enum-selector-display");
    assert_contains(enum_svg, "data-frog-anchor=\"value_display.left_center\"");
}

void test_headless_enum_roundtrip() {
    frog::runtime::Slice08EnumRuntimeCore runtime(contract_path(), wfrog_path());
    const auto artifact = runtime.execute("fault");
    const auto& root = artifact.as_object();
    assert(root.at("outputs").as_object().at("public").as_object().at("result_mode").as_string() == "fault");
    assert(root.at("outputs").as_object().at("ui").as_object().at("mode_result").as_string() == "fault");
    assert(root.at("execution_summary").as_object().at("result_text").as_string() == "Fault");
    assert(root.at("execution_summary").as_object().at("result_numeric_value").as_i64() == 2);
}

void test_enum_browser_ui_surface() {
    frog::runtime::EnumBrowserUiRuntime runtime(contract_path(), wfrog_path());
    std::string html = runtime.render_html();

    assert_contains(html, "Enum Mode Roundtrip");
    assert_contains(html, "class='front-panel'");
    assert_contains(html, "style='width:620px;height:180px;'");
    assert_contains(html, "data-panel-id='main_panel'");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "enum contract executor");
    assert_contains(html, "none for Example 08");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='none'");
    assert_contains(html, "data-execution-path='cpp_enum_contract_executor'");
    assert_contains(html, "data-widget-id='mode_input'");
    assert_contains(html, "data-widget-id='mode_result'");
    assert_contains(html, "data-class-ref='frog.widgets.enum_control'");
    assert_contains(html, "data-class-ref='frog.widgets.enum_indicator'");
    assert_contains(html, "data-asset-route='/asset/enum_rectangular_ring_svg'");
    assert_contains(html, "class='enum-skin'");
    assert_contains(html, "data-frog-visual-law='wfrog-realization-state-map'");
    assert_contains(html, "data-frog-selector-visible='true'");
    assert_contains(html, "data-frog-selector-visible='false'");
    assert_contains(html, "name='mode_value'");
    assert_contains(html, "class='enum-value-display-overlay enum-display-button'");
    assert_contains(html, "class='enum-selector-overlay enum-selector-button'");
    assert_contains(html, "class='enum-select-state'");
    assert_contains(html, "class='enum-dropdown'");
    assert_contains(html, "class='enum-dropdown-option'");
    assert_contains(html, "--frog-enum-text-padding-inline:8px");
    assert_contains(html, "--frog-enum-selector-stroke-width:1px");
    assert_contains(html, "--frog-enum-selector-radius:1px");
    assert_contains(html, "--frog-enum-selector-symbol-width:10px");
    assert_contains(html, "--frog-enum-selector-symbol-height:7px");
    assert_contains(html, "--frog-enum-dropdown-fill:#ffffff");
    assert_contains(html, "--frog-enum-dropdown-border:#64748b");
    assert_contains(html, "--frog-enum-dropdown-option-hover-fill:#2563eb");
    assert_contains(html, "--frog-enum-dropdown-option-selected-fill:#1d4ed8");
    assert_contains(html, "--frog-enum-dropdown-option-font-style:normal");
    assert_contains(html, "border-width:var(--frog-enum-selector-stroke-width)");
    assert_contains(html, "padding:0 var(--frog-enum-text-padding-inline)");
    assert_contains(html, "data-svg-anchor='value_display.left_center'");
    assert_contains(html, "id='mode_input_display'");
    assert_contains(html, "onchange=\"frogUpdateEnumDisplay(this,'mode_input_display')\"");
    assert_contains(html, "function frogUpdateEnumDisplay");
    assert_contains(html, "onclick=\"frogToggleEnumDropdown('mode_input_dropdown','mode_input_display')\"");
    assert_contains(html, "function frogToggleEnumDropdown");
    assert_contains(html, "function frogSelectEnumOption");
    assert_contains(html, "<option value='idle'");
    assert_contains(html, "<option value='run' selected");
    assert_contains(html, "<option value='fault'");
    assert_contains(html, "Mode input");
    assert_contains(html, "Mode result");
    assert_not_contains(html, "enum-card");
    assert_not_contains(html, "enum-select-hitbox");
    assert_not_contains(html, "fallback");
    assert_not_contains(html, "Current runtime snapshot");
    assert_not_contains(html, "<pre>");

    runtime.run_once("idle");
    html = runtime.render_html();
    assert_contains(html, "<option value='idle' selected");
}

} // namespace

int main() {
    test_enum_runtime_core_consumes_frog_instance_and_wfrog_assets();
    test_headless_enum_roundtrip();
    test_enum_browser_ui_surface();
    std::cout << "slice08 Enum C++ runtime acceptance passed" << std::endl;
    return 0;
}
