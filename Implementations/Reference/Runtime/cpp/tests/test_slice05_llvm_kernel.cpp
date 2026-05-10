#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "json.hpp"
#include "runtime.hpp"
#include "ui.hpp"

extern "C" void frog_example05_run(std::uint16_t input_value, frog::runtime::FrogRunResult* out_result);

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

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

const frog::json::Object& acceptance_root() {
    static const frog::json::Value acceptance = load_json(
        repo_root() / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example05_runtime_family.acceptance.json");
    return acceptance.as_object();
}

std::shared_ptr<const frog::runtime::NativeKernelBridge> make_bridge() {
    return std::make_shared<const frog::runtime::NativeKernelBridge>(
        frog::runtime::make_linked_native_kernel_bridge(native_manifest_path(), &frog_example05_run));
}

void test_direct_llvm_kernel_bridge_call() {
    const auto& root = acceptance_root();
    const auto& overflow = root.at("overflow").as_object();

    const auto bridge = make_bridge();
    assert(bridge->manifest().entry_symbol == "frog_example05_run");
    assert(bridge->manifest().abi == "frog_u16_to_result_status_outptr");
    assert(bridge->manifest().source_artifact == "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll");

    const auto ok = bridge->run(3);
    assert(ok.ok);
    assert(ok.result == 15);
    assert(ok.error_code == 0);

    const auto rejected = bridge->run(static_cast<std::uint16_t>(overflow.at("input_value").as_i64()));
    assert(!rejected.ok);
    assert(rejected.error_code == 1);
    assert(rejected.diagnostic == overflow.at("expected_error").as_string());
}

void test_runtime_uses_llvm_kernel_bridge() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto& headless = root.at("headless").as_object();
    const auto& overflow = root.at("overflow").as_object();

    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());
    const auto snapshot_path = resolve_repo_path(refs.at("snapshot_path").as_string());
    const auto expected = load_json(snapshot_path);

    const auto bridge = make_bridge();

    frog::runtime::Slice05RuntimeCore runtime(contract_path, wfrog_path);
    const auto actual = runtime.execute_with_native_kernel_bridge(
        *bridge,
        static_cast<std::uint16_t>(headless.at("input_value").as_i64()));
    assert(canonical_json(actual) == canonical_json(expected));

    bool rejected = false;
    try {
        static_cast<void>(runtime.execute_with_native_kernel_bridge(
            *bridge,
            static_cast<std::uint16_t>(overflow.at("input_value").as_i64())));
    } catch (const std::exception& error) {
        rejected = true;
        assert(std::string(error.what()) == overflow.at("expected_error").as_string());
    }
    assert(rejected);
}

void test_browser_ui_runtime_uses_llvm_kernel_bridge() {
    const auto& root = acceptance_root();
    const auto& refs = root.at("artifact_refs").as_object();
    const auto& headless = root.at("headless").as_object();

    const auto contract_path = resolve_repo_path(refs.at("contract_path").as_string());
    const auto wfrog_path = resolve_repo_path(refs.at("wfrog_path").as_string());
    const auto snapshot_path = resolve_repo_path(refs.at("snapshot_path").as_string());
    const auto expected = load_json(snapshot_path);

    frog::runtime::BrowserUiRuntime runtime(contract_path, wfrog_path, make_bridge());
    const auto actual = runtime.run_once(static_cast<std::uint16_t>(headless.at("input_value").as_i64()));
    assert(canonical_json(actual) == canonical_json(expected));

    const auto html = runtime.render_html();
    assert_contains(html, "data-execution-path='native_kernel_bridge'");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='llvm'");
    assert_contains(html, "Example 05");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "native kernel bridge");
    assert_contains(html, "LLVM native kernel artifact");
    assert_contains(html, "style='width:500px;height:170px;'");
    assert_contains(html, "left:20px;top:24px;width:220px;height:88px;");
    assert_contains(html, "left:260px;top:24px;width:220px;height:88px;");
}

} // namespace

int main() {
    test_direct_llvm_kernel_bridge_call();
    test_runtime_uses_llvm_kernel_bridge();
    test_browser_ui_runtime_uses_llvm_kernel_bridge();
    std::cout << "slice05 LLVM-produced native kernel bridge passed" << std::endl;
    return 0;
}
