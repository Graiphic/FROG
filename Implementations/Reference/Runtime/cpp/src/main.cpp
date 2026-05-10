#include <cstdint>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "contract.hpp"
#include "execute.hpp"
#include "json.hpp"
#include "ui.hpp"

#ifndef FROG_RUNTIME_CPP_SOURCE_DIR
#define FROG_RUNTIME_CPP_SOURCE_DIR "."
#endif

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

std::filesystem::path example06_contract_path() {
    return repo_root() / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "06_boolean_value_roundtrip.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path example06_wfrog_path() {
    return repo_root() / "Examples" / "06_boolean_value_roundtrip" / "ui" / "boolean_panel.wfrog";
}

bool wants_example06(const std::string& value) {
    return value == "06" || value == "6" || value == "example06" || value == "06_boolean_value_roundtrip";
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
                } else if (args[index] == "--host") {
                    host = require_value(args, index, "--host");
                } else if (args[index] == "--port") {
                    port = static_cast<std::uint16_t>(std::stoul(require_value(args, index, "--port")));
                } else if (args[index] == "--no-open-browser") {
                    open_browser = false;
                } else {
                    throw std::runtime_error("Unknown ui argument: " + args[index]);
                }
            }

            if (example.has_value() && wants_example06(*example)) {
                frog::runtime::BooleanBrowserUiRuntime runtime(
                    contract_path.value_or(example06_contract_path()),
                    wfrog_path.value_or(example06_wfrog_path()));
                runtime.serve(host, port, open_browser);
                return 0;
            }

            const auto effective_contract = contract_path.value_or(frog::runtime::default_contract_path());
            const auto contract = frog::runtime::load_contract_from_path(effective_contract);
            if (contract.source_ref.example_id == "06_boolean_value_roundtrip") {
                frog::runtime::BooleanBrowserUiRuntime runtime(
                    effective_contract,
                    wfrog_path.value_or(example06_wfrog_path()));
                runtime.serve(host, port, open_browser);
            } else {
                frog::runtime::BrowserUiRuntime runtime(effective_contract, wfrog_path);
                runtime.serve(host, port, open_browser);
            }
            return 0;
        }

        std::optional<std::filesystem::path> contract_path;
        std::optional<std::filesystem::path> wfrog_path;
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
            } else {
                throw std::runtime_error("Unknown argument: " + args[index]);
            }
        }

        if (example.has_value() && wants_example06(*example)) {
            const auto artifact = frog::runtime::execute_boolean_contract(
                parse_bool_input(input_value_text.value_or("true")),
                contract_path.value_or(example06_contract_path()),
                wfrog_path.value_or(example06_wfrog_path()));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        const auto effective_contract = contract_path.value_or(frog::runtime::default_contract_path());
        const auto contract = frog::runtime::load_contract_from_path(effective_contract);
        if (contract.source_ref.example_id == "06_boolean_value_roundtrip") {
            const auto artifact = frog::runtime::execute_boolean_contract(
                parse_bool_input(input_value_text.value_or("true")),
                effective_contract,
                wfrog_path.value_or(example06_wfrog_path()));
            std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
            return 0;
        }

        const auto input_value = static_cast<std::uint16_t>(std::stoul(input_value_text.value_or("3")));
        const auto artifact = frog::runtime::execute_contract(input_value, effective_contract, wfrog_path);
        std::cout << frog::json::stringify(artifact, true, 2) << std::endl;
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
}
