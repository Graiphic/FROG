#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "json.hpp"
#include "runtime.hpp"
#include "ui.hpp"

extern "C" void frog_example06_run(std::uint8_t input_value, frog::runtime::FrogBoolRunResult* out_result);

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
           "06_boolean_value_roundtrip" / "native_kernel_manifest.json";
}

std::string canonical_json(const frog::json::Value& value) {
    return frog::json::stringify(value, true, 2);
}

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

const frog::json::Object& acceptance_root() {
    static const frog::json::Value acceptance = load_json(
        repo_root() / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example06_boolean_value_roundtrip.acceptance.json");
    return acceptance.as_object();
}

std::shared_ptr<const frog::runtime::NativeBoolKernelBridge> make_bridge() {
    return std::make_shared<const frog::runtime::NativeBoolKernelBridge>(
        frog::runtime::make_linked_native_bool_kernel_bridge(native_manifest_path(), &frog_example06_run));
}

void test_direct_llvm_bool_kernel_bridge_call() {
    const auto bridge = make_bridge();
    assert(bridge->manifest().entry_symbol == "frog_example06_run");
    assert(bridge->manifest().abi == "frog_bool_to_result_status_outptr");
    assert(bridge->manifest().source_artifact == "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/kernel.ll");

    const auto false_result = bridge->run(false);
    assert(false_result.ok);
    assert(!false_result.result);
    assert(false_result.error_code == 0);

    const auto true_result = bridge->run(true);
    assert(true_result.ok);
    assert(true_result.result);
    assert(true_result.error_code == 0);
}

void test_runtime_uses_llvm_bool_kernel_bridge() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();

    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());
    const auto bridge = make_bridge();

    for (const auto& case_value : root.at("cases").as_array()) {
        const auto& current_case = case_value.as_object();
        const bool input_value = current_case.at("input_value").as_bool();
        const bool expected_result = current_case.at("expected_result").as_bool();
        const auto snapshot_path = resolve_repo_path(current_case.at("snapshot_path").as_string());
        const auto expected = load_json(snapshot_path);

        frog::runtime::Slice06BooleanRuntimeCore runtime(contract_path, wfrog_path);
        const auto actual = runtime.execute_with_native_kernel_bridge(*bridge, input_value);

        assert(actual.as_object().at("outputs").as_object().at("public").as_object().at("result").as_bool() == expected_result);
        assert(canonical_json(actual) == canonical_json(expected));
    }
}

void test_browser_ui_runtime_uses_llvm_bool_kernel_bridge() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();

    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());

    frog::runtime::BooleanBrowserUiRuntime runtime(contract_path, wfrog_path, make_bridge());
    const auto actual = runtime.run_once(true);
    assert(actual.as_object().at("outputs").as_object().at("public").as_object().at("result").as_bool());

    const auto html = runtime.render_html();
    assert_contains(html, "data-execution-path='native_kernel_bridge'");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='llvm'");
    assert_contains(html, "Example 06");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "native kernel bridge");
    assert_contains(html, "LLVM native bool kernel artifact");
    assert_contains(html, "data-asset-route='/asset/boolean_rectangular_svg'");
    assert_contains(html, "data-asset-route='/asset/boolean_circular_svg'");
    assert_contains(html, "--boolean-pressed-inset:1px;");
}

} // namespace

int main() {
    test_direct_llvm_bool_kernel_bridge_call();
    test_runtime_uses_llvm_bool_kernel_bridge();
    test_browser_ui_runtime_uses_llvm_bool_kernel_bridge();
    std::cout << "slice06 LLVM-produced native bool kernel bridge passed" << std::endl;
    return 0;
}
