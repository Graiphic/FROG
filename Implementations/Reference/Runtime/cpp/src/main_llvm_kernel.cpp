#include <cstdint>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "json.hpp"
#include "kernel_bridge.hpp"
#include "runtime.hpp"
#include "ui.hpp"

extern "C" void frog_example05_run(std::uint16_t input_value, frog::runtime::FrogRunResult* out_result);
extern "C" void frog_example06_run(std::uint8_t input_value, frog::runtime::FrogBoolRunResult* out_result);
extern "C" void frog_example07_run(const std::uint8_t* input_ptr, std::uint32_t input_len, frog::runtime::FrogStringRunResult* out_result);
extern "C" void frog_example08_run(std::uint16_t mode_value, frog::runtime::FrogRunResult* out_result);
extern "C" void frog_example09_run(const std::uint8_t* input_ptr, std::uint32_t input_len, frog::runtime::FrogStringRunResult* out_result);
extern "C" void frog_example10_run(std::uint8_t trigger_pressed, frog::runtime::FrogBoolRunResult* out_result);
extern "C" void frog_example11_run(std::uint8_t trigger_value, frog::runtime::FrogBoolRunResult* out_result);
extern "C" void frog_example12_run(std::uint8_t trigger_value, frog::runtime::FrogBoolRunResult* out_result);

namespace {

std::string require_value(const std::vector<std::string>& args, std::size_t& index, const std::string& option_name) {
    ++index;
    if (index >= args.size()) {
        throw std::runtime_error("Missing value for option " + option_name + ".");
    }
    return args[index];
}

std::filesystem::path repo_root() {
    return frog::runtime::find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
}

std::filesystem::path default_example05_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "05_bounded_ui_accumulator" / "native_kernel_manifest.json";
}

std::filesystem::path default_example06_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "06_boolean_value_roundtrip" / "native_kernel_manifest.json";
}

std::filesystem::path default_example07_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "07_string_value_roundtrip" / "native_kernel_manifest.json";
}

std::filesystem::path default_example08_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "08_enum_value_roundtrip" / "native_kernel_manifest.json";
}

std::filesystem::path default_example09_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "09_path_value_roundtrip" / "native_kernel_manifest.json";
}

std::filesystem::path default_example10_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "10_button_press_to_boolean" / "native_kernel_manifest.json";
}

std::filesystem::path default_example11_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "11_button_switch_when_pressed" / "native_kernel_manifest.json";
}

std::filesystem::path default_example12_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "12_button_switch_when_released" / "native_kernel_manifest.json";
}

std::filesystem::path example06_contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "06_boolean_value_roundtrip.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path example07_contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "07_string_value_roundtrip.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path example08_contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "08_enum_value_roundtrip.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path example09_contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "09_path_value_roundtrip.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path example10_contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "10_button_press_to_boolean.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path example11_contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "11_button_switch_when_pressed.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path example12_contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "12_button_switch_when_released.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path example06_wfrog_path() {
    return repo_root() / "Examples" / "06_boolean_value_roundtrip" / "ui" / "boolean_panel.wfrog";
}

std::filesystem::path example07_wfrog_path() {
    return repo_root() / "Examples" / "07_string_value_roundtrip" / "ui" / "string_panel.wfrog";
}

std::filesystem::path example08_wfrog_path() {
    return repo_root() / "Examples" / "08_enum_value_roundtrip" / "ui" / "enum_panel.wfrog";
}

std::filesystem::path example09_wfrog_path() {
    return repo_root() / "Examples" / "09_path_value_roundtrip" / "ui" / "path_panel.wfrog";
}

std::filesystem::path example10_wfrog_path() {
    return repo_root() / "Examples" / "10_button_press_to_boolean" / "ui" / "button_panel.wfrog";
}

std::filesystem::path example11_wfrog_path() {
    return repo_root() / "Examples" / "11_button_switch_when_pressed" / "ui" / "button_panel.wfrog";
}

std::filesystem::path example12_wfrog_path() {
    return repo_root() / "Examples" / "12_button_switch_when_released" / "ui" / "button_panel.wfrog";
}

bool wants_example06(const std::string& value) {
    return value == "06" || value == "6" || value == "example06" || value == "06_boolean_value_roundtrip";
}

bool wants_example07(const std::string& value) {
    return value == "07" || value == "7" || value == "example07" || value == "07_string_value_roundtrip";
}

bool wants_example08(const std::string& value) {
    return value == "08" || value == "8" || value == "example08" || value == "08_enum_value_roundtrip";
}

bool wants_example09(const std::string& value) {
    return value == "09" || value == "9" || value == "example09" || value == "09_path_value_roundtrip";
}

bool wants_example10(const std::string& value) {
    return value == "10" || value == "example10" || value == "10_button_press_to_boolean";
}

bool wants_example11(const std::string& value) {
    return value == "11" || value == "example11" || value == "11_button_switch_when_pressed";
}

bool wants_example12(const std::string& value) {
    return value == "12" || value == "example12" || value == "12_button_switch_when_released";
}

bool parse_bool_input(std::string value) {
    for (auto& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    if (value == "true" || value == "1" || value == "on") {
        return true;
    }
    if (value == "false" || value == "0" || value.empty()) {
        return false;
    }
    throw std::runtime_error("Boolean input must be true or false.");
}

std::uint16_t parse_u16_value(const std::string& raw, const std::string& label) {
    const auto parsed = std::stoul(raw);
    if (parsed > 65535ul) {
        throw std::runtime_error(label + " must remain in the u16 domain.");
    }
    return static_cast<std::uint16_t>(parsed);
}

std::shared_ptr<const frog::runtime::NativeKernelBridge> make_example05_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeKernelBridge>(
        frog::runtime::make_linked_native_kernel_bridge(manifest_path, &frog_example05_run));
}

std::shared_ptr<const frog::runtime::NativeBoolKernelBridge> make_example06_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeBoolKernelBridge>(
        frog::runtime::make_linked_native_bool_kernel_bridge(manifest_path, &frog_example06_run));
}

std::shared_ptr<const frog::runtime::NativeStringKernelBridge> make_example07_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeStringKernelBridge>(
        frog::runtime::make_linked_native_string_kernel_bridge(manifest_path, &frog_example07_run));
}

std::shared_ptr<const frog::runtime::NativeEnumKernelBridge> make_example08_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeEnumKernelBridge>(
        frog::runtime::make_linked_native_enum_kernel_bridge(manifest_path, &frog_example08_run));
}

std::shared_ptr<const frog::runtime::NativeStringKernelBridge> make_example09_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeStringKernelBridge>(
        frog::runtime::make_linked_native_string_kernel_bridge(manifest_path, &frog_example09_run));
}

std::shared_ptr<const frog::runtime::NativeBoolKernelBridge> make_example10_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeBoolKernelBridge>(
        frog::runtime::make_linked_native_bool_kernel_bridge(manifest_path, &frog_example10_run));
}

std::shared_ptr<const frog::runtime::NativeBoolKernelBridge> make_example11_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeBoolKernelBridge>(
        frog::runtime::make_linked_native_bool_kernel_bridge(manifest_path, &frog_example11_run));
}

std::shared_ptr<const frog::runtime::NativeBoolKernelBridge> make_example12_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeBoolKernelBridge>(
        frog::runtime::make_linked_native_bool_kernel_bridge(manifest_path, &frog_example12_run));
}

} // namespace

int main(int argc, char** argv) {
    try {
        std::vector<std::string> args;
        args.reserve(static_cast<std::size_t>(argc > 0 ? argc - 1 : 0));
        for (int index = 1; index < argc; ++index) {
            args.emplace_back(argv[index]);
        }

        if (!args.empty() && args.front() == "ui") {
            args.erase(args.begin());
            std::optional<std::filesystem::path> contract_path;
            std::optional<std::filesystem::path> wfrog_path;
            std::optional<std::filesystem::path> manifest_path;
            std::optional<std::string> example;
            std::string host = "127.0.0.1";
            std::uint16_t port = 0;
            bool open_browser = true;

            for (std::size_t index = 0; index < args.size(); ++index) {
                if (args[index] == "--example") {
                    example = require_value(args, index, "--example");
                } else if (args[index] == "--contract") {
                    contract_path = require_value(args, index, "--contract");
                } else if (args[index] == "--wfrog") {
                    wfrog_path = require_value(args, index, "--wfrog");
                } else if (args[index] == "--manifest") {
                    manifest_path = require_value(args, index, "--manifest");
                } else if (args[index] == "--host") {
                    host = require_value(args, index, "--host");
                } else if (args[index] == "--port") {
                    port = parse_u16_value(require_value(args, index, "--port"), "--port");
                } else if (args[index] == "--no-open-browser") {
                    open_browser = false;
                } else {
                    throw std::runtime_error("Unknown ui argument: " + args[index]);
                }
            }

            if (example.has_value() && wants_example06(*example)) {
                frog::runtime::BooleanBrowserUiRuntime runtime(
                    contract_path.value_or(example06_contract_path()),
                    wfrog_path.value_or(example06_wfrog_path()),
                    make_example06_bridge(manifest_path.value_or(default_example06_manifest_path())));
                runtime.serve(host, port, open_browser);
                return 0;
            }

            if (example.has_value() && wants_example07(*example)) {
                frog::runtime::StringBrowserUiRuntime runtime(
                    contract_path.value_or(example07_contract_path()),
                    wfrog_path.value_or(example07_wfrog_path()),
                    make_example07_bridge(manifest_path.value_or(default_example07_manifest_path())));
                runtime.serve(host, port, open_browser);
                return 0;
            }

            if (example.has_value() && wants_example08(*example)) {
                frog::runtime::EnumBrowserUiRuntime runtime(
                    contract_path.value_or(example08_contract_path()),
                    wfrog_path.value_or(example08_wfrog_path()),
                    make_example08_bridge(manifest_path.value_or(default_example08_manifest_path())));
                runtime.serve(host, port, open_browser);
                return 0;
            }

            if (example.has_value() && wants_example09(*example)) {
                frog::runtime::PathBrowserUiRuntime runtime(
                    contract_path.value_or(example09_contract_path()),
                    wfrog_path.value_or(example09_wfrog_path()),
                    make_example09_bridge(manifest_path.value_or(default_example09_manifest_path())));
                runtime.serve(host, port, open_browser);
                return 0;
            }

            if (example.has_value() && wants_example10(*example)) {
                frog::runtime::ButtonBrowserUiRuntime runtime(
                    contract_path.value_or(example10_contract_path()),
                    wfrog_path.value_or(example10_wfrog_path()),
                    make_example10_bridge(manifest_path.value_or(default_example10_manifest_path())));
                runtime.serve(host, port, open_browser);
                return 0;
            }

            if (example.has_value() && wants_example11(*example)) {
                frog::runtime::ButtonBrowserUiRuntime runtime(
                    contract_path.value_or(example11_contract_path()),
                    wfrog_path.value_or(example11_wfrog_path()),
                    make_example11_bridge(manifest_path.value_or(default_example11_manifest_path())));
                runtime.serve(host, port, open_browser);
                return 0;
            }

            if (example.has_value() && wants_example12(*example)) {
                frog::runtime::ButtonBrowserUiRuntime runtime(
                    contract_path.value_or(example12_contract_path()),
                    wfrog_path.value_or(example12_wfrog_path()),
                    make_example12_bridge(manifest_path.value_or(default_example12_manifest_path())));
                runtime.serve(host, port, open_browser);
                return 0;
            }

            frog::runtime::BrowserUiRuntime runtime(
                contract_path,
                wfrog_path,
                make_example05_bridge(manifest_path.value_or(default_example05_manifest_path())));
            runtime.serve(host, port, open_browser);
            return 0;
        }

        std::optional<std::filesystem::path> contract_path;
        std::optional<std::filesystem::path> wfrog_path;
        std::optional<std::filesystem::path> manifest_path;
        std::optional<std::string> example;
        std::optional<std::string> input_value_text;

        if (!args.empty() && args.front() == "run") {
            args.erase(args.begin());
        }
        if (!args.empty() && !args.front().starts_with("--")) {
            input_value_text = args.front();
            args.erase(args.begin());
        }

        for (std::size_t index = 0; index < args.size(); ++index) {
            if (args[index] == "--example") {
                example = require_value(args, index, "--example");
            } else if (args[index] == "--contract") {
                contract_path = require_value(args, index, "--contract");
            } else if (args[index] == "--wfrog") {
                wfrog_path = require_value(args, index, "--wfrog");
            } else if (args[index] == "--manifest") {
                manifest_path = require_value(args, index, "--manifest");
            } else {
                throw std::runtime_error("Unknown argument: " + args[index]);
            }
        }

        if (example.has_value() && wants_example06(*example)) {
            frog::runtime::Slice06BooleanRuntimeCore runtime(
                contract_path.value_or(example06_contract_path()),
                wfrog_path.value_or(example06_wfrog_path()));
            const auto artifact = runtime.execute_with_native_kernel_bridge(
                *make_example06_bridge(manifest_path.value_or(default_example06_manifest_path())),
                parse_bool_input(input_value_text.value_or("true")));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        if (example.has_value() && wants_example07(*example)) {
            frog::runtime::Slice07StringRuntimeCore runtime(
                contract_path.value_or(example07_contract_path()),
                wfrog_path.value_or(example07_wfrog_path()));
            const auto artifact = runtime.execute_with_native_kernel_bridge(
                *make_example07_bridge(manifest_path.value_or(default_example07_manifest_path())),
                input_value_text.value_or("hello world"));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        if (example.has_value() && wants_example08(*example)) {
            frog::runtime::Slice08EnumRuntimeCore runtime(
                contract_path.value_or(example08_contract_path()),
                wfrog_path.value_or(example08_wfrog_path()));
            const auto artifact = runtime.execute_with_native_kernel_bridge(
                *make_example08_bridge(manifest_path.value_or(default_example08_manifest_path())),
                input_value_text.value_or("run"));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        if (example.has_value() && wants_example09(*example)) {
            frog::runtime::Slice09PathRuntimeCore runtime(
                contract_path.value_or(example09_contract_path()),
                wfrog_path.value_or(example09_wfrog_path()));
            const auto artifact = runtime.execute_with_native_kernel_bridge(
                *make_example09_bridge(manifest_path.value_or(default_example09_manifest_path())),
                input_value_text.value_or("C:/FROG/hello_world.txt"));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        if (example.has_value() && wants_example10(*example)) {
            frog::runtime::Slice10ButtonRuntimeCore runtime(
                contract_path.value_or(example10_contract_path()),
                wfrog_path.value_or(example10_wfrog_path()));
            const auto artifact = runtime.execute_with_native_kernel_bridge(
                *make_example10_bridge(manifest_path.value_or(default_example10_manifest_path())),
                parse_bool_input(input_value_text.value_or("true")));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        if (example.has_value() && wants_example11(*example)) {
            frog::runtime::Slice10ButtonRuntimeCore runtime(
                contract_path.value_or(example11_contract_path()),
                wfrog_path.value_or(example11_wfrog_path()));
            const auto artifact = runtime.execute_with_native_kernel_bridge(
                *make_example11_bridge(manifest_path.value_or(default_example11_manifest_path())),
                parse_bool_input(input_value_text.value_or("true")));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        if (example.has_value() && wants_example12(*example)) {
            frog::runtime::Slice10ButtonRuntimeCore runtime(
                contract_path.value_or(example12_contract_path()),
                wfrog_path.value_or(example12_wfrog_path()));
            const auto artifact = runtime.execute_with_native_kernel_bridge(
                *make_example12_bridge(manifest_path.value_or(default_example12_manifest_path())),
                parse_bool_input(input_value_text.value_or("true")));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        const auto input_value = parse_u16_value(input_value_text.value_or("3"), "input_value");
        frog::runtime::Slice05RuntimeCore runtime(
            contract_path.value_or(frog::runtime::default_contract_path()),
            wfrog_path.value_or(frog::runtime::default_wfrog_path()));
        const auto artifact = runtime.execute_with_native_kernel_bridge(
            *make_example05_bridge(manifest_path.value_or(default_example05_manifest_path())),
            input_value);
        std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
}
