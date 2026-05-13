#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "json.hpp"
#include "runtime.hpp"
#include "ui.hpp"

extern "C" void frog_example08_run(std::uint16_t mode_value, frog::runtime::FrogRunResult* out_result);

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

std::filesystem::path native_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "08_enum_value_roundtrip" / "native_kernel_manifest.json";
}

std::shared_ptr<const frog::runtime::NativeEnumKernelBridge> make_bridge() {
    return std::make_shared<const frog::runtime::NativeEnumKernelBridge>(
        frog::runtime::make_linked_native_enum_kernel_bridge(native_manifest_path(), &frog_example08_run));
}

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

void test_direct_llvm_enum_kernel_bridge_call() {
    const auto bridge = make_bridge();
    assert(bridge->manifest().entry_symbol == "frog_example08_run");
    assert(bridge->manifest().abi == "frog_enum_u16_to_result_status_outptr");
    assert(bridge->manifest().source_artifact == "Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/kernel.ll");

    const auto result = bridge->run(2);
    assert(result.ok);
    assert(result.result_numeric_value == 2);
    assert(result.error_code == 0);

    const auto invalid = bridge->run(3);
    assert(!invalid.ok);
    assert(invalid.error_code == 1);
}

void test_runtime_uses_llvm_enum_kernel_bridge() {
    frog::runtime::Slice08EnumRuntimeCore runtime(contract_path(), wfrog_path());
    const auto artifact = runtime.execute_with_native_kernel_bridge(*make_bridge(), "fault");
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("result_mode").as_string() == "fault");
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("mode_result").as_string() == "fault");
}

void test_browser_ui_runtime_uses_llvm_enum_kernel_bridge() {
    frog::runtime::EnumBrowserUiRuntime runtime(contract_path(), wfrog_path(), make_bridge());
    const auto artifact = runtime.run_once("idle");
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("result_mode").as_string() == "idle");

    const auto html = runtime.render_html();
    assert_contains(html, "data-execution-path='native_kernel_bridge'");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='llvm'");
    assert_contains(html, "Example 08");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "native kernel bridge");
    assert_contains(html, "LLVM native enum kernel artifact");
    assert_contains(html, "data-asset-route='/asset/enum_rectangular_ring_svg'");
}

} // namespace

int main() {
    test_direct_llvm_enum_kernel_bridge_call();
    test_runtime_uses_llvm_enum_kernel_bridge();
    test_browser_ui_runtime_uses_llvm_enum_kernel_bridge();
    std::cout << "slice08 LLVM-produced native enum kernel bridge passed" << std::endl;
    return 0;
}
