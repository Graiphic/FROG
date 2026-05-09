#include <cstdint>
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

std::filesystem::path default_manifest_path() {
    return repo_root() / "Implementations" / "Reference" / "LLVM" / "examples" /
           "05_bounded_ui_accumulator" / "native_kernel_manifest.json";
}

std::uint16_t parse_u16_value(const std::string& raw, const std::string& label) {
    const auto parsed = std::stoul(raw);
    if (parsed > 65535ul) {
        throw std::runtime_error(label + " must remain in the u16 domain.");
    }
    return static_cast<std::uint16_t>(parsed);
}

std::shared_ptr<const frog::runtime::NativeKernelBridge> make_bridge(const std::filesystem::path& manifest_path) {
    return std::make_shared<const frog::runtime::NativeKernelBridge>(
        frog::runtime::make_linked_native_kernel_bridge(manifest_path, &frog_example05_run));
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
            std::filesystem::path manifest_path = default_manifest_path();
            std::string host = "127.0.0.1";
            std::uint16_t port = 0;
            bool open_browser = true;

            for (std::size_t index = 0; index < args.size(); ++index) {
                if (args[index] == "--contract") {
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

            frog::runtime::BrowserUiRuntime runtime(contract_path, wfrog_path, make_bridge(manifest_path));
            runtime.serve(host, port, open_browser);
            return 0;
        }

        std::optional<std::filesystem::path> contract_path;
        std::optional<std::filesystem::path> wfrog_path;
        std::filesystem::path manifest_path = default_manifest_path();
        std::uint16_t input_value = 3;

        if (!args.empty() && args.front() == "run") {
            args.erase(args.begin());
        }
        if (!args.empty() && !args.front().starts_with("--")) {
            input_value = parse_u16_value(args.front(), "input_value");
            args.erase(args.begin());
        }

        for (std::size_t index = 0; index < args.size(); ++index) {
            if (args[index] == "--contract") {
                contract_path = require_value(args, index, "--contract");
            } else if (args[index] == "--wfrog") {
                wfrog_path = require_value(args, index, "--wfrog");
            } else if (args[index] == "--manifest") {
                manifest_path = require_value(args, index, "--manifest");
            } else {
                throw std::runtime_error("Unknown argument: " + args[index]);
            }
        }

        frog::runtime::Slice05RuntimeCore runtime(
            contract_path.value_or(frog::runtime::default_contract_path()),
            wfrog_path.value_or(frog::runtime::default_wfrog_path()));
        const auto artifact = runtime.execute_with_native_kernel_bridge(*make_bridge(manifest_path), input_value);
        std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
}
