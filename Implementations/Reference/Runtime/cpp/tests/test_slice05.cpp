#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include "contract.hpp"
#include "execute.hpp"
#include "json.hpp"
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

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

const frog::json::Object& acceptance_root() {
    static const frog::json::Value acceptance = load_json(
        repo_root() / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example05_runtime_family.acceptance.json");
    return acceptance.as_object();
}

void test_headless_snapshot() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto& headless = root.at("headless").as_object();
    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());
    const auto snapshot_path = resolve_repo_path(refs.at("snapshot_path").as_string());

    const auto expected = load_json(snapshot_path);
    const auto actual = frog::runtime::execute_contract(
        static_cast<std::uint16_t>(headless.at("input_value").as_i64()),
        contract_path,
        wfrog_path);

    assert(canonical_json(actual) == canonical_json(expected));
}

void test_overflow_rejection() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto& overflow = root.at("overflow").as_object();
    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());

    bool rejected = false;
    try {
        static_cast<void>(frog::runtime::execute_contract(
            static_cast<std::uint16_t>(overflow.at("input_value").as_i64()),
            contract_path,
            wfrog_path));
    } catch (const std::exception& error) {
        rejected = true;
        assert(std::string(error.what()) == overflow.at("expected_error").as_string());
    }
    assert(rejected);
}

void test_ui_surface() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto& routes = root.at("ui").as_object().at("expected_routes").as_array();
    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());
    const auto snapshot_path = resolve_repo_path(refs.at("snapshot_path").as_string());
    const auto expected = load_json(snapshot_path);

    frog::runtime::BrowserUiRuntime runtime(contract_path, wfrog_path);
    const std::string html = runtime.render_html();
    for (const auto& route_value : routes) {
        const auto route = route_value.as_string();
        if (route == "/") {
            continue;
        }
        assert_contains(html, route);
    }

    assert_contains(html, "class='front-panel'");
    assert_contains(html, "data-panel-id='main_panel'");
    assert_contains(html, "data-coordinate-space='panel_pixels'");
    assert_contains(html, "style='width:460px;height:170px;'");

    assert_contains(html, "data-widget-id='ctrl_input'");
    assert_contains(html, "data-widget-id='ind_result'");
    assert_contains(html, "data-class-ref='frog.widgets.numeric_control'");
    assert_contains(html, "data-class-ref='frog.widgets.numeric_indicator'");
    assert_contains(html, "data-asset-route='/asset/numeric_control_svg'");
    assert_contains(html, "data-asset-route='/asset/numeric_indicator_svg'");

    assert_contains(html, "left:20px;top:24px;width:140px;height:32px;");
    assert_contains(html, "left:240px;top:24px;width:160px;height:32px;");
    assert_contains(html, "class='numeric-skin'");
    assert_contains(html, "data-svg-anchor='label_anchor'");
    assert_contains(html, "data-svg-anchor='value_anchor'");
    assert_contains(html, "data-svg-part='value_box'");
    assert_contains(html, "Input");
    assert_contains(html, "Accumulated result");

    runtime.core.execute(static_cast<std::uint16_t>(root.at("headless").as_object().at("input_value").as_i64()));
    assert(canonical_json(runtime.core.execution_artifact()) == canonical_json(expected));
    assert(std::filesystem::exists(runtime.core.asset_map.at("numeric_control_svg")));
    assert(std::filesystem::exists(runtime.core.asset_map.at("numeric_indicator_svg")));
}

} // namespace

int main() {
    test_headless_snapshot();
    test_overflow_rejection();
    test_ui_surface();
    std::cout << "slice05 runtime acceptance passed" << std::endl;
    return 0;
}
