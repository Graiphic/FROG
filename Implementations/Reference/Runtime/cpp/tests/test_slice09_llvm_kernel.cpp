#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "json.hpp"
#include "runtime.hpp"
#include "ui.hpp"

extern "C" void frog_example09_run(const std::uint8_t* input_ptr, std::uint32_t input_len, frog::runtime::FrogStringRunResult* out_result);

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

std::filesystem::path native_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "09_path_value_roundtrip" / "native_kernel_manifest.json";
}

std::shared_ptr<const frog::runtime::NativeStringKernelBridge> make_bridge() {
    return std::make_shared<const frog::runtime::NativeStringKernelBridge>(
        frog::runtime::make_linked_native_string_kernel_bridge(native_manifest_path(), &frog_example09_run));
}

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

void test_direct_llvm_path_kernel_bridge_call() {
    const auto bridge = make_bridge();
    assert(bridge->manifest().entry_symbol == "frog_example09_run");
    assert(bridge->manifest().abi == "frog_string_utf8_256_to_result_status_outptr");
    assert(bridge->manifest().source_artifact == "Implementations/Reference/LLVM/examples/09_path_value_roundtrip/kernel.ll");
    assert(bridge->manifest().input_id == "input_path");
    assert(bridge->manifest().output_id == "result_path");

    const auto result = bridge->run("C:/FROG/hello_world.txt");
    assert(result.ok);
    assert(result.result == "C:/FROG/hello_world.txt");
    assert(result.error_code == 0);
}

void test_runtime_uses_llvm_path_kernel_bridge() {
    frog::runtime::Slice09PathRuntimeCore runtime(contract_path(), wfrog_path());
    const auto artifact = runtime.execute_with_native_kernel_bridge(*make_bridge(), "C:/FROG/from_llvm.txt");
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("result_path").as_string() == "C:/FROG/from_llvm.txt");
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("path_result").as_string() == "C:/FROG/from_llvm.txt");
}

void test_browser_ui_runtime_uses_llvm_path_kernel_bridge() {
    frog::runtime::PathBrowserUiRuntime runtime(contract_path(), wfrog_path(), make_bridge());
    const auto artifact = runtime.run_once("C:/FROG/hello_world.txt");
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("result_path").as_string() == "C:/FROG/hello_world.txt");

    const auto html = runtime.render_html();
    assert_contains(html, "data-execution-path='native_kernel_bridge'");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='llvm'");
    assert_contains(html, "Example 09");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "native kernel bridge");
    assert_contains(html, "LLVM native path kernel artifact");
    assert_contains(html, "data-asset-route='/asset/path_rectangular_field_svg'");
}

} // namespace

int main() {
    test_direct_llvm_path_kernel_bridge_call();
    test_runtime_uses_llvm_path_kernel_bridge();
    test_browser_ui_runtime_uses_llvm_path_kernel_bridge();
    std::cout << "slice09 LLVM-produced native path kernel bridge passed" << std::endl;
    return 0;
}
