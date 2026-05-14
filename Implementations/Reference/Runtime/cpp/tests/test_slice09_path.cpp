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
           "09_path_value_roundtrip.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path wfrog_path() {
    return repo_root() / "Examples" / "09_path_value_roundtrip" / "ui" / "path_panel.wfrog";
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

void test_path_runtime_core_consumes_frog_instance_and_wfrog_assets() {
    frog::runtime::Slice09PathRuntimeCore runtime(contract_path(), wfrog_path());
    assert(runtime.unit.kind == "path_value_roundtrip_ui_unit");
    assert(runtime.panel.panel_id == "main_panel");
    assert(runtime.package.kind == "widget_realization_package");
    assert(runtime.widgets.at("path_input").class_ref == "frog.widgets.path_control");
    assert(runtime.widgets.at("path_result").class_ref == "frog.widgets.path_indicator");
    assert(runtime.widgets.at("path_input_no_icon").class_ref == "frog.widgets.path_control");
    assert(runtime.widgets.at("path_result_no_icon").class_ref == "frog.widgets.path_indicator");
    assert(runtime.widgets.at("path_input").asset_id == "path_rectangular_field_svg");
    assert(runtime.widgets.at("path_result").asset_id == "path_rectangular_field_svg");
    assert(runtime.widgets.at("path_input_no_icon").asset_id == "path_rectangular_field_svg");
    assert(runtime.widgets.at("path_result_no_icon").asset_id == "path_rectangular_field_svg");
    assert(std::filesystem::exists(runtime.asset_map.at("path_rectangular_field_svg")));

    const auto path_svg = read_text(runtime.asset_map.at("path_rectangular_field_svg"));
    assert_contains(path_svg, "data-frog-template=\"frog.realizations.default.path.rectangular_field\"");
    assert_contains(path_svg, "data-frog-part=\"path_face\"");
    assert_contains(path_svg, "data-frog-part=\"path_display\"");
    assert_contains(path_svg, "data-frog-part=\"browse_button\"");
    assert_contains(path_svg, "data-frog-part=\"path_icon\"");
    assert_contains(path_svg, "data-frog-anchor=\"path_display.left_center\"");
}

void test_headless_path_roundtrip() {
    frog::runtime::Slice09PathRuntimeCore runtime(contract_path(), wfrog_path());
    const auto artifact = runtime.execute_all({
        {"input_path", "C:/FROG/top.txt"},
        {"input_path_no_icon", "C:/FROG/bottom.txt"},
    });
    const auto& root = artifact.as_object();
    assert(root.at("outputs").as_object().at("public").as_object().at("result_path").as_string() == "C:/FROG/top.txt");
    assert(root.at("outputs").as_object().at("ui").as_object().at("path_result").as_string() == "C:/FROG/top.txt");
    assert(root.at("outputs").as_object().at("ui").as_object().at("path_result_no_icon").as_string() == "C:/FROG/bottom.txt");
}

void test_path_browser_ui_surface() {
    frog::runtime::PathBrowserUiRuntime runtime(contract_path(), wfrog_path());
    std::string html = runtime.render_html();

    assert_contains(html, "Path Value Roundtrip");
    assert_contains(html, "class='front-panel'");
    assert_contains(html, "style='width:700px;height:300px;'");
    assert_contains(html, "data-panel-id='main_panel'");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "path contract executor");
    assert_contains(html, "none for Example 09");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='none'");
    assert_contains(html, "data-execution-path='cpp_path_contract_executor'");
    assert_contains(html, "data-widget-id='path_input'");
    assert_contains(html, "data-widget-id='path_result'");
    assert_contains(html, "data-widget-id='path_input_no_icon'");
    assert_contains(html, "data-widget-id='path_result_no_icon'");
    assert_contains(html, "data-class-ref='frog.widgets.path_control'");
    assert_contains(html, "data-class-ref='frog.widgets.path_indicator'");
    assert_contains(html, "data-asset-route='/asset/path_rectangular_field_svg'");
    assert_contains(html, "class='path-skin'");
    assert_contains(html, "data-frog-visual-law='wfrog-realization-state-map'");
    assert_contains(html, "name='input_path' type='text'");
    assert_contains(html, "type='file' class='path-file-picker'");
    assert_contains(html, "data-frog-part='browse_button'");
    assert_contains(html, "--frog-path-icon-display:none");
    assert_contains(html, "--frog-path-face-stroke-width:2px");
    assert_contains(html, "name='input_path_no_icon' type='text'");
    assert_contains(html, "data-frog-input-id='input_path'");
    assert_contains(html, "data-frog-input-id='input_path_no_icon'");
    assert_contains(html, "frogPathPicked");
    assert_not_contains(html, "frogPathValueChanged");
    assert_contains(html, "Path input");
    assert_contains(html, "Path result");
    assert_not_contains(html, "path-card");
    assert_not_contains(html, "fallback");
    assert_not_contains(html, "Current runtime snapshot");
    assert_not_contains(html, "<pre>");

    runtime.core.execute_all({
        {"input_path", "C:/FROG/from_cpp_top.txt"},
        {"input_path_no_icon", "C:/FROG/from_cpp_bottom.txt"},
    });
    html = runtime.render_html();
    assert_contains(html, "C:/FROG/from_cpp_top.txt");
    assert_contains(html, "C:/FROG/from_cpp_bottom.txt");
}

} // namespace

int main() {
    test_path_runtime_core_consumes_frog_instance_and_wfrog_assets();
    test_headless_path_roundtrip();
    test_path_browser_ui_surface();
    std::cout << "slice09 Path C++ runtime acceptance passed" << std::endl;
    return 0;
}
