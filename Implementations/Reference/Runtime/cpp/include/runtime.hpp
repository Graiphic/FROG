#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "contract.hpp"
#include "kernel_bridge.hpp"

namespace frog::runtime {

struct WidgetState {
    std::string widget_id;
    std::string class_ref;
    std::string role;
    frog::json::Value layout;
    frog::json::Object properties;
    std::optional<std::string> asset_id;
    std::filesystem::path asset_path;
    std::vector<std::string> supported_members;
};

class Slice05RuntimeCore {
public:
    Slice05RuntimeCore(
        std::filesystem::path contract_path = default_contract_path(),
        std::filesystem::path wfrog_path = default_wfrog_path());

    void apply_contract_property_writes();
    void set_control_value(std::uint16_t value);
    std::uint16_t control_value() const;
    void invoke_method(const std::string& widget_id, const std::string& method_name);
    frog::json::Value execute(std::optional<std::uint16_t> control_value = std::nullopt);
    frog::json::Value execute_with_native_kernel_bridge(
        const NativeKernelBridge& bridge,
        std::optional<std::uint16_t> control_value = std::nullopt);
    frog::json::Value execution_artifact() const;

    std::filesystem::path contract_path;
    std::filesystem::path wfrog_path;
    BackendContract contract;
    WfrogPackage package;
    ContractUnit unit;
    FrontPanel panel;
    std::map<std::string, WidgetState> widgets;
    std::map<std::string, std::filesystem::path> asset_map;
    std::vector<frog::json::Value> applied_widget_references;
    std::uint16_t last_final_state = 0;

private:
    static std::uint16_t checked_u16(std::uint32_t value, const std::string& label);
    ContractUnit load_and_validate() const;
    std::map<std::string, WidgetState> build_widgets() const;
    void reset_to_default_style(const std::string& widget_id);
};

class Slice06BooleanRuntimeCore {
public:
    Slice06BooleanRuntimeCore(std::filesystem::path contract_path, std::filesystem::path wfrog_path);

    void set_control_value(bool value);
    bool control_value() const;
    frog::json::Value execute(std::optional<bool> control_value = std::nullopt);
    frog::json::Value execute_with_native_kernel_bridge(
        const NativeBoolKernelBridge& bridge,
        std::optional<bool> control_value = std::nullopt);
    frog::json::Value execution_artifact() const;

    std::filesystem::path contract_path;
    std::filesystem::path wfrog_path;
    BackendContract contract;
    WfrogPackage package;
    ContractUnit unit;
    FrontPanel panel;
    std::map<std::string, WidgetState> widgets;
    std::map<std::string, std::filesystem::path> asset_map;
    bool last_result = false;

private:
    ContractUnit load_and_validate() const;
    std::map<std::string, WidgetState> build_widgets() const;
};

} // namespace frog::runtime
