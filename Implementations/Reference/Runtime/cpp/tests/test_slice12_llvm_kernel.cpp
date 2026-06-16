#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "json.hpp"
#include "runtime.hpp"
#include "ui.hpp"

extern "C" void frog_example12_run(std::uint8_t trigger_value, frog::runtime::FrogBoolRunResult* out_result);

namespace {

std::filesystem::path repo_root() {
    return frog::runtime::find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
}

std::filesystem::path contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "12_button_switch_when_released.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path wfrog_path() {
    return repo_root() / "Examples" / "12_button_switch_when_released" / "ui" / "button_panel.wfrog";
}

std::filesystem::path native_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "12_button_switch_when_released" / "native_kernel_manifest.json";
}

std::shared_ptr<const frog::runtime::NativeBoolKernelBridge> make_bridge() {
    return std::make_shared<const frog::runtime::NativeBoolKernelBridge>(
        frog::runtime::make_linked_native_bool_kernel_bridge(native_manifest_path(), &frog_example12_run));
}

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

void test_direct_llvm_button_switch_release_kernel_bridge_call() {
    const auto bridge = make_bridge();
    assert(bridge->manifest().entry_symbol == "frog_example12_run");
    assert(bridge->manifest().abi == "frog_bool_to_result_status_outptr");
    assert(bridge->manifest().source_artifact == "Implementations/Reference/LLVM/examples/12_button_switch_when_released/kernel.ll");
    assert(bridge->manifest().source_lowered_unit == "Examples/12_button_switch_when_released/main.lowering.json");
    assert(bridge->manifest().input_id == "trigger_value");
    assert(bridge->manifest().output_id == "switched");

    const auto false_result = bridge->run(false);
    assert(false_result.ok);
    assert(!false_result.result);
    assert(false_result.error_code == 0);

    const auto true_result = bridge->run(true);
    assert(true_result.ok);
    assert(true_result.result);
    assert(true_result.error_code == 0);
}

void test_runtime_uses_llvm_button_switch_release_kernel_bridge() {
    frog::runtime::Slice10ButtonRuntimeCore runtime(contract_path(), wfrog_path());
    const auto bridge = make_bridge();

    auto artifact = runtime.execution_artifact();
    assert(!artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("switched_indicator").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());

    artifact = runtime.press_control();
    assert(artifact.as_object().at("execution_summary").as_object().at("trigger_pressed").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("ui").as_object().at("switched_indicator").as_bool());

    artifact = runtime.read_program_value_with_native_kernel_bridge(*bridge);
    assert(artifact.as_object().at("execution_summary").as_object().at("program_read_performed").as_bool());
    assert(!artifact.as_object().at("execution_summary").as_object().at("program_read_value").as_bool());
    assert(!artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());

    artifact = runtime.execute_with_native_kernel_bridge(*bridge, false);
    assert(!artifact.as_object().at("execution_summary").as_object().at("trigger_pressed").as_bool());
    assert(artifact.as_object().at("execution_summary").as_object().at("program_read_performed").as_bool());
    assert(artifact.as_object().at("execution_summary").as_object().at("program_read_value").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("trigger_button").as_bool());
    assert(artifact.as_object().at("outputs").as_object().at("ui").as_object().at("switched_indicator").as_bool());
}

void test_browser_ui_runtime_uses_llvm_button_switch_release_kernel_bridge() {
    frog::runtime::ButtonBrowserUiRuntime runtime(contract_path(), wfrog_path(), make_bridge());
    runtime.run_once(true);
    const auto artifact = runtime.run_once(false);
    assert(artifact.as_object().at("outputs").as_object().at("public").as_object().at("switched").as_bool());

    const auto html = runtime.render_html();
    assert_contains(html, "data-execution-path='native_kernel_bridge'");
    assert_contains(html, "data-runtime-language='cpp'");
    assert_contains(html, "data-compiler-backend='llvm'");
    assert_contains(html, "Example 12");
    assert_contains(html, "C++ reference runtime");
    assert_contains(html, "native kernel bridge");
    assert_contains(html, "LLVM native Button bool kernel artifact");
    assert_contains(html, "data-asset-route='/asset/button_rectangular_svg'");
    assert_contains(html, "data-asset-route='/asset/boolean_circular_svg'");
    assert_contains(html, "class='button-press-overlay' type='button'");
    assert_contains(html, "data-frog-part='button_face' data-frog-event='pressed' data-frog-public-input-id='trigger_value'");
    assert_contains(html, "data-frog-host-overlay='input' data-frog-align-to-part='button_face'");
    assert_contains(html, "fetch(\"/event\"");
    assert_contains(html, "pointerup");
    assert_contains(html, "switch_when_released");
}

} // namespace

int main() {
    test_direct_llvm_button_switch_release_kernel_bridge_call();
    test_runtime_uses_llvm_button_switch_release_kernel_bridge();
    test_browser_ui_runtime_uses_llvm_button_switch_release_kernel_bridge();
    std::cout << "slice12 LLVM-produced native Button switch-when-released bool kernel bridge passed" << std::endl;
    return 0;
}
