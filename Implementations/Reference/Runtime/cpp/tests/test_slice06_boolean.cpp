#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "execute.hpp"
#include "json.hpp"
#include "runtime.hpp"
#include "ui.hpp"

namespace {

std::filesystem::path repo_root() {
    return frog::runtime::find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
}

frog::json::Value load_json(const std::filesystem::path& path) {
    return frog::json::parse_file(path);
}

std::filesystem::path resolve_repo_path(const std::string& relative_path) {
    return repo_root() / relative_path;
}

std::string canonical_json(const frog::json::Value& value) {
    return frog::json::stringify(value, true, 2);
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

const frog::json::Object& acceptance_root() {
    static const frog::json::Value acceptance = load_json(
        repo_root() / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example06_boolean_value_roundtrip.acceptance.json");
    return acceptance.as_object();
}

void test_headless_snapshots() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());

    for (const auto& case_value : root.at("cases").as_array()) {
        const auto& current_case = case_value.as_object();
        const bool input_value = current_case.at("input_value").as_bool();
        const bool expected_result = current_case.at("expected_result").as_bool();
        const auto snapshot_path = resolve_repo_path(current_case.at("snapshot_path").as_string());
        const auto expected = load_json(snapshot_path);
        const auto actual = frog::runtime::execute_boolean_contract(input_value, contract_path, wfrog_path);

        assert(actual.as_object().at("outputs").as_object().at("public").as_object().at("result").as_bool() == expected_result);
        assert(canonical_json(actual) == canonical_json(expected));
    }
}

void test_boolean_runtime_core_consumes_wfrog_assets() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());

    frog::runtime::Slice06BooleanRuntimeCore runtime(contract_path, wfrog_path);
    assert(runtime.unit.kind == "boolean_value_roundtrip_ui_unit");
    assert(runtime.panel.panel_id == "main_panel");
    assert(runtime.widgets.at("bool_input").class_ref == "frog.widgets.boolean_control");
    assert(runtime.widgets.at("bool_result").class_ref == "frog.widgets.boolean_indicator");
    assert(runtime.widgets.at("bool_input").asset_id == "boolean_rectangular_svg");
    assert(runtime.widgets.at("bool_result").asset_id == "boolean_circular_svg");
    assert(std::filesystem::exists(runtime.asset_map.at("boolean_rectangular_svg")));
    assert(std::filesystem::exists(runtime.asset_map.at("boolean_circular_svg")));

    const auto rectangular_svg = read_text(runtime.asset_map.at("boolean_rectangular_svg"));
    const auto circular_svg = read_text(runtime.asset_map.at("boolean_circular_svg"));
    assert_contains(rectangular_svg, "viewBox=\"0 0 160 80\"");
    assert_contains(circular_svg, "viewBox=\"0 0 160 80\"");
    assert_contains(rectangular_svg, "data-frog-part=\"state_text\"");
    assert_contains(circular_svg, "data-frog-part=\"state_text\"");
    assert_contains(rectangular_svg, "stroke: transparent;");
    assert_contains(rectangular_svg, "fill: transparent;");
    assert_contains(rectangular_svg, "opacity: 0;");
    assert_contains(circular_svg, "stroke: transparent;");
    assert_contains(circular_svg, "fill: transparent;");
    assert_contains(circular_svg, "opacity: 0;");
}

void test_boolean_browser_ui_surface() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());

    frog::runtime::BooleanBrowserUiRuntime runtime(contract_path, wfrog_path);
    std::string html = runtime.render_html();

    assert_contains(html, "Boolean Value Roundtrip");
    assert_contains(html, "class='front-panel'");
    assert_contains(html, "style='width:420px;height:150px;'");
    assert_contains(html, "data-panel-id='main_panel'");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "boolean contract executor");
    assert_contains(html, "none for Example 06");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='none'");
    assert_contains(html, "data-execution-path='cpp_boolean_contract_executor'");
    assert_contains(html, "data-widget-id='bool_input'");
    assert_contains(html, "data-widget-id='bool_result'");
    assert_contains(html, "data-class-ref='frog.widgets.boolean_control'");
    assert_contains(html, "data-class-ref='frog.widgets.boolean_indicator'");
    assert_contains(html, "data-asset-route='/asset/boolean_rectangular_svg'");
    assert_contains(html, "data-asset-route='/asset/boolean_circular_svg'");
    assert_contains(html, "class='boolean-skin'");
    assert_contains(html, "class='boolean-state-face'");
    assert_contains(html, "data-frog-visual-law='wfrog-realization-state-map'");
    assert_contains(html, "data-realization-variant='rectangular'");
    assert_contains(html, "data-realization-variant='circular'");
    assert_contains(html, "data-frog-visual-state='true'");
    assert_contains(html, "data-frog-hover-state='hover_true'");
    assert_contains(html, "data-frog-pressed-state='pressed_true'");
    assert_contains(html, "data-frog-transition-state='transition_true_to_false'");
    assert_contains(html, "data-frog-frame-visible='false'");
    assert_contains(html, "data-frog-state-text-visible='false'");
    assert_contains(html, "--boolean-fill:#8bd86f;");
    assert_contains(html, "--boolean-fill:#22c55e;");
    assert_contains(html, "--boolean-border:transparent;");
    assert_contains(html, "--boolean-inner-border:transparent;");
    assert_contains(html, "--boolean-transition:120ms ease-out;");
    assert_contains(html, "--boolean-pressed-inset:1px;");
    assert_contains(html, "transform:translateY(var(--boolean-pressed-inset));");
    assert_contains(html, "name='input_value' value='false'");
    assert_contains(html, "data-toggle-target='false'");
    assert_contains(html, "aria-readonly='true'");
    assert_contains(html, "/state.json");
    assert_not_contains(html, "boolean-card");
    assert_not_contains(html, "boolean-face");
    assert_not_contains(html, "type='checkbox'");
    assert_not_contains(html, "Current runtime snapshot");
    assert_not_contains(html, "<pre>");

    runtime.run_once(false);
    html = runtime.render_html();
    assert_contains(html, "name='input_value' value='true'");
    assert_contains(html, "data-toggle-target='true'");
    assert_contains(html, "data-frog-visual-state='false'");
    assert_contains(html, "data-frog-hover-state='hover_false'");
    assert_contains(html, "data-frog-pressed-state='pressed_false'");
    assert_contains(html, "data-frog-transition-state='transition_false_to_true'");
    assert_contains(html, "--boolean-fill:#ffffff;");
    assert_contains(html, "--boolean-fill:#ef4444;");
    assert_contains(html, ">FALSE</span>");
}

} // namespace

int main() {
    test_headless_snapshots();
    test_boolean_runtime_core_consumes_wfrog_assets();
    test_boolean_browser_ui_surface();
    std::cout << "slice06 Boolean C++ runtime acceptance passed" << std::endl;
    return 0;
}
