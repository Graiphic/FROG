#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>

namespace frog::runtime {

struct FrogRunResult {
    std::uint8_t ok;
    std::uint16_t result;
    std::uint16_t error_code;
};

static_assert(sizeof(FrogRunResult) == 6, "FrogRunResult ABI layout must remain {u8, u16, u16} with natural padding.");
static_assert(alignof(FrogRunResult) == 2, "FrogRunResult ABI alignment must remain 2 bytes.");

struct FrogBoolRunResult {
    std::uint8_t ok;
    std::uint8_t result;
    std::uint16_t error_code;
};

static_assert(sizeof(FrogBoolRunResult) == 4, "FrogBoolRunResult ABI layout must remain {u8, u8, u16}.");
static_assert(alignof(FrogBoolRunResult) == 2, "FrogBoolRunResult ABI alignment must remain 2 bytes.");

using FrogNativeKernelFunction = void (*)(std::uint16_t input_value, FrogRunResult* out_result);
using FrogNativeBoolKernelFunction = void (*)(std::uint8_t input_value, FrogBoolRunResult* out_result);

struct NativeKernelManifest {
    std::filesystem::path manifest_path;
    std::string kernel_id;
    std::string source_lowered_unit;
    std::string backend_family;
    std::string backend_artifact_kind;
    std::string source_artifact;
    std::string entry_symbol;
    std::string abi;
    std::string input_id;
    std::string input_type;
    std::string output_id;
    std::string output_type;
    std::string overflow_model;
    std::map<std::uint16_t, std::string> diagnostics_by_error_code;
};

struct NativeKernelResult {
    bool ok;
    std::uint16_t result;
    std::uint16_t error_code;
    std::string diagnostic;
};

struct NativeBoolKernelResult {
    bool ok;
    bool result;
    std::uint16_t error_code;
    std::string diagnostic;
};

NativeKernelManifest load_native_kernel_manifest(const std::filesystem::path& manifest_path);

class NativeKernelBridge {
public:
    NativeKernelBridge(NativeKernelManifest manifest, FrogNativeKernelFunction entry_point);

    NativeKernelResult run(std::uint16_t input_value) const;
    const NativeKernelManifest& manifest() const;

private:
    NativeKernelManifest manifest_;
    FrogNativeKernelFunction entry_point_;
};

NativeKernelBridge make_linked_native_kernel_bridge(
    const std::filesystem::path& manifest_path,
    FrogNativeKernelFunction entry_point);

class NativeBoolKernelBridge {
public:
    NativeBoolKernelBridge(NativeKernelManifest manifest, FrogNativeBoolKernelFunction entry_point);

    NativeBoolKernelResult run(bool input_value) const;
    const NativeKernelManifest& manifest() const;

private:
    NativeKernelManifest manifest_;
    FrogNativeBoolKernelFunction entry_point_;
};

NativeBoolKernelBridge make_linked_native_bool_kernel_bridge(
    const std::filesystem::path& manifest_path,
    FrogNativeBoolKernelFunction entry_point);

} // namespace frog::runtime
