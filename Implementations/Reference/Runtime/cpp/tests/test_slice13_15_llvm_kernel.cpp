#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "json.hpp"
#include "kernel_bridge.hpp"
#include "runtime.hpp"
#include "ui.hpp"

extern "C" void frog_example13_run(std::uint8_t trigger_value, frog::runtime::FrogBoolRunResult* out_result);
extern "C" void frog_example14_run(std::uint8_t trigger_value, frog::runtime::FrogBoolRunResult* out_result);
extern "C" void frog_example15_run(std::uint8_t trigger_value, frog::runtime::FrogBoolRunResult* out_result);

namespace {

using Entry = void (*)(std::uint8_t, frog::runtime::FrogBoolRunResult*);

struct LatchKernelSpec {
    int number;
    std::string example_id;
    std::string action;
    std::string unit_kind;
    std::string title;
    std::string symbol;
    Entry entry;
};

std::filesystem::path repo_root() {
    return frog::runtime::find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
}

std::filesystem::path contract_path(const LatchKernelSpec& spec) {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           (spec.example_id + ".reference_host_runtime_ui_binding.contract.json");
}

std::filesystem::path wfrog_path(const LatchKernelSpec& spec) {
    return repo_root() / "Examples" / spec.example_id / "ui" / "button_panel.wfrog";
}

std::filesystem::path native_manifest_path(const LatchKernelSpec& spec) {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" / spec.example_id / "native_kernel_manifest.json";
}

std::vector<LatchKernelSpec> specs() {
    return {
        {13, "13_button_latch_when_pressed", "latch_when_pressed", "button_latch_when_pressed", "Button Latch When Pressed", "frog_example13_run", &frog_example13_run},
        {14, "14_button_latch_when_released", "latch_when_released", "button_latch_when_released", "Button Latch When Released", "frog_example14_run", &frog_example14_run},
        {15, "15_button_latch_until_released", "latch_until_released", "button_latch_until_released", "Button Latch Until Released", "frog_example15_run", &frog_example15_run},
    };
}

void assert_contains(const std::string& haystack, const std::string& needle) {
    assert(haystack.find(needle) != std::string::npos);
}

bool public_output_value(const frog::json::Value& artifact, const std::string& id) {
    return artifact.as_object().at("outputs").as_object().at("public").as_object().at(id).as_bool();
}

bool ui_value(const frog::json::Value& artifact, const std::string& id) {
    return artifact.as_object().at("outputs").as_object().at("ui").as_object().at(id).as_bool();
}

bool summary_bool(const frog::json::Value& artifact, const std::string& id) {
    return artifact.as_object().at("execution_summary").as_object().at(id).as_bool();
}

void test_native_latch_kernel_bridge(const LatchKernelSpec& spec) {
    const auto bridge = std::make_shared<const frog::runtime::NativeBoolKernelBridge>(
        frog::runtime::make_linked_native_bool_kernel_bridge(native_manifest_path(spec), spec.entry));

    assert(bridge->manifest().entry_symbol == spec.symbol);
    assert(bridge->manifest().input_id == "trigger_value");
    assert(bridge->manifest().output_id == "latched");
    assert(bridge->manifest().source_artifact == "Implementations/Reference/LLVM/examples/" + spec.example_id + "/kernel.ll");
    assert(bridge->manifest().source_lowered_unit == "Examples/" + spec.example_id + "/main.lowering.json");
    assert(bridge->run(true).result);
    assert(!bridge->run(false).result);

    frog::runtime::Slice10ButtonRuntimeCore core(contract_path(spec), wfrog_path(spec));
    auto artifact = core.execution_artifact();
    assert(artifact.as_object().at("execution_summary").as_object().at("mode").as_string() == spec.unit_kind);
    assert(!public_output_value(artifact, "latched"));
    assert(!ui_value(artifact, "trigger_button"));
    assert(!ui_value(artifact, "latched_indicator"));

    if (spec.action == "latch_when_pressed") {
        artifact = core.press_control();
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = core.release_control();
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = core.read_program_value_with_native_kernel_bridge(*bridge);
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));
    } else if (spec.action == "latch_when_released") {
        artifact = core.press_control();
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "latched_indicator"));

        artifact = core.release_control();
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = core.read_program_value_with_native_kernel_bridge(*bridge);
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));
    } else {
        artifact = core.press_control();
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = core.release_control();
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = core.read_program_value_with_native_kernel_bridge(*bridge);
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "trigger_button"));
        assert(ui_value(artifact, "latched_indicator"));

        frog::runtime::Slice10ButtonRuntimeCore read_before_release(contract_path(spec), wfrog_path(spec));
        artifact = read_before_release.press_control();
        assert(public_output_value(artifact, "latched"));

        artifact = read_before_release.read_program_value_with_native_kernel_bridge(*bridge);
        assert(summary_bool(artifact, "program_read_performed"));
        assert(summary_bool(artifact, "program_read_value"));
        assert(public_output_value(artifact, "latched"));
        assert(ui_value(artifact, "latched_indicator"));

        artifact = read_before_release.release_control();
        assert(!public_output_value(artifact, "latched"));
        assert(!ui_value(artifact, "latched_indicator"));
    }

    frog::runtime::ButtonBrowserUiRuntime runtime(contract_path(spec), wfrog_path(spec), bridge);
    std::string html = runtime.render_html();
    assert_contains(html, "native kernel bridge");
    assert_contains(html, "LLVM native Button bool kernel artifact");
    assert_contains(html, "data-compiler-backend='llvm'");
    assert_contains(html, "data-execution-path='native_kernel_bridge'");
    assert_contains(html, spec.action);
}

} // namespace

int main() {
    for (const auto& spec : specs()) {
        test_native_latch_kernel_bridge(spec);
    }
    std::cout << "slice13-15 Button latch LLVM kernel bridge acceptance passed" << std::endl;
    return 0;
}
