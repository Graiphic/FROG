#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "contract.hpp"
#include "execute.hpp"
#include "json.hpp"
#include "ui.hpp"

extern "C" void frog_example05_run(std::uint16_t input_value, frog::runtime::FrogRunResult* out_result) {
    std::uint32_t state = 0;
    for (std::uint32_t index = 0; index < 5; ++index) {
        state += input_value;
        if (state > 65535u) {
            *out_result = frog::runtime::FrogRunResult{0, 0, 1};
            return;
        }
    }
    *out_result = frog::runtime::FrogRunResult{1, static_cast<std::uint16_t>(state), 0};
}

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

std::filesystem::path native_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "05_bounded_ui_accumulator" / "native_kernel_manifest.json";
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

std::int64_t object_i64(const frog::json::Object& object, const std::string& key) {
    const auto it = object.find(key);
    assert(it != object.end());
    assert(it->second.is_number());
    return it->second.as_i64();
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

void test_native_kernel_bridge() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto& headless = root.at("headless").as_object();
    const auto& overflow = root.at("overflow").as_object();
    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());
    const auto snapshot_path = resolve_repo_path(refs.at("snapshot_path").as_string());

    const auto bridge = frog::runtime::make_linked_native_kernel_bridge(native_manifest_path(), &frog_example05_run);
    assert(bridge.manifest().entry_symbol == "frog_example05_run");
    assert(bridge.manifest().abi == "frog_u16_to_result_status_outptr");
    assert(bridge.manifest().backend_family == "llvm");

    frog::runtime::Slice05RuntimeCore runtime(contract_path, wfrog_path);
    const auto expected = load_json(snapshot_path);
    const auto actual = runtime.execute_with_native_kernel_bridge(
        bridge,
        static_cast<std::uint16_t>(headless.at("input_value").as_i64()));
    assert(canonical_json(actual) == canonical_json(expected));

    bool rejected = false;
    try {
        static_cast<void>(runtime.execute_with_native_kernel_bridge(
            bridge,
            static_cast<std::uint16_t>(overflow.at("input_value").as_i64())));
    } catch (const std::exception& error) {
        rejected = true;
        assert(std::string(error.what()) == overflow.at("expected_error").as_string());
    }
    assert(rejected);
}

void test_frog_widget_geometry_matches_svg_skin_size() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto contract = load_json(contract_path);
    const auto& source_ref = contract.as_object().at("source_ref").as_object();
    const auto source_path = resolve_repo_path(source_ref.at("path").as_string());
    const auto panel = frog::runtime::load_front_panel_from_frog_source_path(source_path);
    const auto& panel_layout = panel.layout.as_object();
    assert(object_i64(panel_layout, "width") == 500);
    assert(object_i64(panel_layout, "height") == 170);
    assert(panel.widgets.size() == 2);

    const auto numeric_svg = read_text(
        repo_root() / "Libraries" / "Realizations" / "Default" / "assets" / "numeric" / "templates" / "numeric_rectangular.svg");
    assert_contains(numeric_svg, "viewBox=\"0 0 380 150\"");
    assert_contains(numeric_svg, "id=\"caption_text\"");
    assert_contains(numeric_svg, "id=\"frame\"");
    assert_contains(numeric_svg, "width=\"222\"");
    assert_contains(numeric_svg, "id=\"value_face\"");
    assert_contains(numeric_svg, "id=\"text_value\"");
    assert_contains(numeric_svg, "id=\"increment_up\"");
    assert_contains(numeric_svg, "id=\"increment_down\"");
    assert_contains(numeric_svg, "data-frog-states=\"normal pressed\"");
    assert_contains(numeric_svg, "data-frog-state-law=\"normal-pressed-color-only\"");
    assert_contains(numeric_svg, "data-frog-bind=\"style.increment_button.*\"");
    assert_contains(numeric_svg, "id=\"unit_label\"");
    assert_contains(numeric_svg, "id=\"radix_badge\"");
    assert(numeric_svg.find(">DBL<") == std::string::npos);

    const auto& control = panel.widgets.at(0);
    const auto& control_layout = control.layout.as_object();
    assert(control.instance_id == "ctrl_input");
    assert(object_i64(control_layout, "x") == 20);
    assert(object_i64(control_layout, "y") == 24);
    assert(object_i64(control_layout, "width") == 220);
    assert(object_i64(control_layout, "height") == 88);

    const auto& indicator = panel.widgets.at(1);
    const auto& indicator_layout = indicator.layout.as_object();
    assert(indicator.instance_id == "ind_result");
    assert(object_i64(indicator_layout, "x") == 260);
    assert(object_i64(indicator_layout, "y") == 24);
    assert(object_i64(indicator_layout, "width") == 220);
    assert(object_i64(indicator_layout, "height") == 88);
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
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "contract executor");
    assert_contains(html, "none in runtime path");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='none'");
    assert_contains(html, "style='width:500px;height:170px;'");

    assert_contains(html, "data-widget-id='ctrl_input'");
    assert_contains(html, "data-widget-id='ind_result'");
    assert_contains(html, "data-class-ref='frog.widgets.numeric_control'");
    assert_contains(html, "data-class-ref='frog.widgets.numeric_indicator'");
    assert_contains(html, "data-frog-visual-law='wfrog-realization-state-map'");
    assert_contains(html, "data-asset-route='/asset/numeric_rectangular_svg'");

    assert_contains(html, "left:20px;top:24px;width:220px;height:88px;");
    assert_contains(html, "left:260px;top:24px;width:220px;height:88px;");
    assert_contains(html, "class='numeric-skin'");
    assert_contains(html, "<svg");
    assert_contains(html, "data-frog-part='caption'");
    assert_contains(html, "data-svg-anchor='caption.anchor'");
    assert_contains(html, "data-frog-part='text_value'");
    assert_contains(html, "data-svg-anchor='text_value.center'");
    assert_contains(html, "data-frog-part='increment_up'");
    assert_contains(html, "data-frog-method='increment'");
    assert_contains(html, "data-frog-part='increment_down'");
    assert_contains(html, "data-frog-method='decrement'");
    assert_contains(html, "data-frog-button-state-law='normal-pressed'");
    assert_contains(html, "--frog-numeric-frame-stroke:transparent");
    assert_contains(html, "--frog-numeric-frame-stroke-width:2px");
    assert_contains(html, "--frog-numeric-step-fill:#5B9BD5");
    assert_contains(html, "--frog-numeric-step-fill-pressed:#2B4F7B");
    assert_contains(html, "--frog-numeric-caption-color:#111827;");
    assert_contains(html, "--frog-numeric-caption-font-size:12px;");
    assert_contains(html, "--frog-numeric-caption-font-weight:400;");
    assert_contains(html, "--frog-numeric-text-color:#111827;");
    assert_contains(html, "--frog-numeric-text-font-size:11px;");
    assert_contains(html, "--frog-numeric-text-font-weight:700;");
    assert_contains(html, "font-size:var(--frog-numeric-caption-font-size)");
    assert_contains(html, "color:var(--frog-numeric-text-color)");
    assert_not_contains(html, "font-size:12px;line-height:1;");
    assert_not_contains(html, "Current runtime snapshot");
    assert_not_contains(html, "<pre>");
    assert_contains(html, "Input");
    assert_contains(html, "Accumulated result");

    runtime.core.execute(static_cast<std::uint16_t>(root.at("headless").as_object().at("input_value").as_i64()));
    assert(canonical_json(runtime.core.execution_artifact()) == canonical_json(expected));
    assert(std::filesystem::exists(runtime.core.asset_map.at("numeric_rectangular_svg")));
}

} // namespace

int main() {
    test_headless_snapshot();
    test_overflow_rejection();
    test_native_kernel_bridge();
    test_frog_widget_geometry_matches_svg_skin_size();
    test_ui_surface();
    std::cout << "slice05 runtime acceptance passed" << std::endl;
    return 0;
}
