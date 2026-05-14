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
           "07_string_value_roundtrip.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path wfrog_path() {
    return repo_root() / "Examples" / "07_string_value_roundtrip" / "ui" / "string_panel.wfrog";
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

void test_string_runtime_core_consumes_wfrog_assets() {
    frog::runtime::Slice07StringRuntimeCore runtime(contract_path(), wfrog_path());
    assert(runtime.unit.kind == "string_value_roundtrip_ui_unit");
    assert(runtime.panel.panel_id == "main_panel");
    assert(runtime.widgets.at("str_input").class_ref == "frog.widgets.string_control");
    assert(runtime.widgets.at("str_result").class_ref == "frog.widgets.string_indicator");
    assert(runtime.widgets.at("str_input").asset_id == "string_rectangular_svg");
    assert(runtime.widgets.at("str_result").asset_id == "string_rectangular_svg");
    assert(std::filesystem::exists(runtime.asset_map.at("string_rectangular_svg")));

    const auto string_svg = read_text(runtime.asset_map.at("string_rectangular_svg"));
    assert_contains(string_svg, "data-frog-template=\"frog.realizations.default.string.rectangular\"");
    assert_contains(string_svg, "data-frog-part=\"text_region\"");
    assert_contains(string_svg, "data-frog-part=\"text_value\"");
    assert_contains(string_svg, "data-frog-anchor=\"text_region.left_center\"");
    assert_contains(string_svg, "x=\"28\"");
    assert_contains(string_svg, "y=\"64\">String</text>");
    assert_contains(string_svg, "style.text_region.fill_color.hover");
}

void test_headless_string_roundtrip() {
    frog::runtime::Slice07StringRuntimeCore runtime(contract_path(), wfrog_path());
    const auto artifact = runtime.execute("hello world");
    const auto& root = artifact.as_object();
    assert(root.at("outputs").as_object().at("public").as_object().at("result_text").as_string() == "hello world");
    assert(root.at("outputs").as_object().at("ui").as_object().at("str_result").as_string() == "hello world");
}

void test_string_browser_ui_surface() {
    frog::runtime::StringBrowserUiRuntime runtime(contract_path(), wfrog_path());
    std::string html = runtime.render_html();

    assert_contains(html, "String Hello World Roundtrip");
    assert_contains(html, "class='front-panel'");
    assert_contains(html, "style='width:560px;height:170px;'");
    assert_contains(html, "data-panel-id='main_panel'");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "string contract executor");
    assert_contains(html, "none for Example 07");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='none'");
    assert_contains(html, "data-execution-path='cpp_string_contract_executor'");
    assert_contains(html, "data-widget-id='str_input'");
    assert_contains(html, "data-widget-id='str_result'");
    assert_contains(html, "data-class-ref='frog.widgets.string_control'");
    assert_contains(html, "data-class-ref='frog.widgets.string_indicator'");
    assert_contains(html, "data-asset-route='/asset/string_rectangular_svg'");
    assert_contains(html, "class='string-skin'");
    assert_contains(html, "data-frog-visual-law='wfrog-realization-state-map'");
    assert_contains(html, "--frog-string-text-region-fill-hover:#eef6ff;");
    assert_contains(html, "--frog-string-text-region-stroke-hover:#2563eb;");
    assert_contains(html, "--frog-string-caption-color:#111827;");
    assert_contains(html, "--frog-string-caption-font-size:14px;");
    assert_contains(html, "--frog-string-caption-font-weight:600;");
    assert_contains(html, "font-size:var(--frog-string-caption-font-size)");
    assert_contains(html, ".string-control:hover .string-skin svg");
    assert_contains(html, "name='input_text' type='text'");
    assert_contains(html, "hello world");
    assert_contains(html, "String input");
    assert_contains(html, "String result");
    assert_not_contains(html, "string-card");
    assert_not_contains(html, "fallback");
    assert_not_contains(html, "Current runtime snapshot");
    assert_not_contains(html, "<pre>");
    assert_not_contains(html, "outline:2px solid #2563eb");
    assert_not_contains(html, ".string-caption-overlay{position:absolute;transform:translateY(-50%);font-size:14px;font-weight:600;");

    runtime.run_once("hello FROG");
    html = runtime.render_html();
    assert_contains(html, "hello FROG");
}

} // namespace

int main() {
    test_string_runtime_core_consumes_wfrog_assets();
    test_headless_string_roundtrip();
    test_string_browser_ui_surface();
    std::cout << "slice07 String C++ runtime acceptance passed" << std::endl;
    return 0;
}
