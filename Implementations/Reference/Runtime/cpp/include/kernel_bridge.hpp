#pragma once

#include <cstdint>
#include <array>
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
using FrogNativeEnumKernelFunction = void (*)(std::uint16_t mode_value, FrogRunResult* out_result);
using FrogNativeBoolKernelFunction = void (*)(std::uint8_t input_value, FrogBoolRunResult* out_result);

struct FrogStringRunResult {
    std::uint8_t ok;
    std::uint16_t error_code;
    std::uint32_t result_len;
    std::array<std::uint8_t, 256> result_buffer;
};

static_assert(sizeof(FrogStringRunResult) == 264, "FrogStringRunResult ABI layout must remain {u8, u16, u32, u8[256]} with natural padding.");
static_assert(alignof(FrogStringRunResult) == 4, "FrogStringRunResult ABI alignment must remain 4 bytes.");

using FrogNativeStringKernelFunction = void (*)(const std::uint8_t* input_ptr, std::uint32_t input_len, FrogStringRunResult* out_result);

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

struct NativeStringKernelResult {
    bool ok;
    std::string result;
    std::uint16_t error_code;
    std::string diagnostic;
};

struct NativeEnumKernelResult {
    bool ok;
    std::uint16_t result_numeric_value;
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

class NativeStringKernelBridge {
public:
    NativeStringKernelBridge(NativeKernelManifest manifest, FrogNativeStringKernelFunction entry_point);

    NativeStringKernelResult run(const std::string& input_value) const;
    const NativeKernelManifest& manifest() const;

private:
    NativeKernelManifest manifest_;
    FrogNativeStringKernelFunction entry_point_;
};

NativeStringKernelBridge make_linked_native_string_kernel_bridge(
    const std::filesystem::path& manifest_path,
    FrogNativeStringKernelFunction entry_point);

class NativeEnumKernelBridge {
public:
    NativeEnumKernelBridge(NativeKernelManifest manifest, FrogNativeEnumKernelFunction entry_point);

    NativeEnumKernelResult run(std::uint16_t mode_value) const;
    const NativeKernelManifest& manifest() const;

private:
    NativeKernelManifest manifest_;
    FrogNativeEnumKernelFunction entry_point_;
};

NativeEnumKernelBridge make_linked_native_enum_kernel_bridge(
    const std::filesystem::path& manifest_path,
    FrogNativeEnumKernelFunction entry_point);

} // namespace frog::runtime
