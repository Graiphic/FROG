#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>

#include "json.hpp"

namespace frog::runtime {

frog::json::Value execute_contract(
    std::uint16_t input_value,
    std::optional<std::filesystem::path> contract_path = std::nullopt,
    std::optional<std::filesystem::path> wfrog_path = std::nullopt);

frog::json::Value execute_boolean_contract(
    bool input_value,
    std::filesystem::path contract_path,
    std::filesystem::path wfrog_path);

frog::json::Value execute_reference_contract_case(
    const frog::json::Value& contract,
    const frog::json::Value& case_value,
    const frog::json::Value* wfrog = nullptr);

frog::json::Value execute_reference_acceptance(
    const frog::json::Value& acceptance,
    const frog::json::Value& contract,
    const frog::json::Value* wfrog = nullptr);

} // namespace frog::runtime
