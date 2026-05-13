#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include "kernel_bridge.hpp"
#include "runtime.hpp"

namespace frog::runtime {

class BrowserUiRuntime {
public:
    BrowserUiRuntime(
        std::optional<std::filesystem::path> contract_path = std::nullopt,
        std::optional<std::filesystem::path> wfrog_path = std::nullopt,
        std::shared_ptr<const NativeKernelBridge> native_kernel_bridge = nullptr);

    frog::json::Value run_once(std::uint16_t input_value);
    std::string render_html() const;
    void serve(const std::string& host = "127.0.0.1", std::uint16_t port = 0, bool open_browser = true);

    Slice05RuntimeCore core;
    std::shared_ptr<const NativeKernelBridge> native_kernel_bridge;
    std::optional<std::string> last_error;
};

class BooleanBrowserUiRuntime {
public:
    BooleanBrowserUiRuntime(
        std::filesystem::path contract_path,
        std::filesystem::path wfrog_path,
        std::shared_ptr<const NativeBoolKernelBridge> native_kernel_bridge = nullptr);

    frog::json::Value run_once(bool input_value);
    std::string render_html() const;
    void serve(const std::string& host = "127.0.0.1", std::uint16_t port = 0, bool open_browser = true);

    Slice06BooleanRuntimeCore core;
    std::shared_ptr<const NativeBoolKernelBridge> native_kernel_bridge;
    std::optional<std::string> last_error;
};

class StringBrowserUiRuntime {
public:
    StringBrowserUiRuntime(
        std::filesystem::path contract_path,
        std::filesystem::path wfrog_path,
        std::shared_ptr<const NativeStringKernelBridge> native_kernel_bridge = nullptr);

    frog::json::Value run_once(const std::string& input_value);
    std::string render_html() const;
    void serve(const std::string& host = "127.0.0.1", std::uint16_t port = 0, bool open_browser = true);

    Slice07StringRuntimeCore core;
    std::shared_ptr<const NativeStringKernelBridge> native_kernel_bridge;
    std::optional<std::string> last_error;
};

class EnumBrowserUiRuntime {
public:
    EnumBrowserUiRuntime(
        std::filesystem::path contract_path,
        std::filesystem::path wfrog_path,
        std::shared_ptr<const NativeEnumKernelBridge> native_kernel_bridge = nullptr);

    frog::json::Value run_once(const std::string& input_value);
    std::string render_html() const;
    void serve(const std::string& host = "127.0.0.1", std::uint16_t port = 0, bool open_browser = true);

    Slice08EnumRuntimeCore core;
    std::shared_ptr<const NativeEnumKernelBridge> native_kernel_bridge;
    std::optional<std::string> last_error;
};

} // namespace frog::runtime
