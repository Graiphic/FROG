#include "kernel_bridge.hpp"

#include <stdexcept>
#include <utility>

#include "json.hpp"

namespace frog::runtime {

namespace {

using frog::json::Array;
using frog::json::Object;
using frog::json::Value;

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

const Object& as_object(const Value& value, const std::string& message) {
    require(value.is_object(), message);
    return value.as_object();
}

const Array& as_array(const Value& value, const std::string& message) {
    require(value.is_array(), message);
    return value.as_array();
}

std::string string_field(const Object& object, const std::string& key) {
    const auto it = object.find(key);
    require(it != object.end() && it->second.is_string(), "Expected string field: " + key);
    return it->second.as_string();
}

std::uint16_t u16_field(const Object& object, const std::string& key) {
    const auto it = object.find(key);
    require(it != object.end() && it->second.is_number(), "Expected integer field: " + key);
    const auto value = it->second.as_i64();
    require(value >= 0 && value <= 65535, "Expected u16 field: " + key);
    return static_cast<std::uint16_t>(value);
}

} // namespace

NativeKernelManifest load_native_kernel_manifest(const std::filesystem::path& manifest_path) {
    const auto root = as_object(frog::json::parse_file(manifest_path), "Expected native kernel manifest object.");
    require(string_field(root, "artifact_kind") == "frog_native_kernel_manifest", "Unexpected native kernel manifest artifact_kind.");

    const auto& kernel = as_object(root.at("kernel"), "Expected kernel object.");
    const auto& backend = as_object(kernel.at("backend"), "Expected backend object.");
    const auto& entry = as_object(kernel.at("entry"), "Expected entry object.");
    const auto& io = as_object(kernel.at("io"), "Expected io object.");
    const auto& inputs = as_array(io.at("inputs"), "Expected io.inputs array.");
    const auto& outputs = as_array(io.at("outputs"), "Expected io.outputs array.");
    require(inputs.size() == 1, "Native bridge expects exactly one input.");
    require(outputs.size() == 1, "Native bridge expects exactly one output.");

    const auto& input = as_object(inputs.front(), "Expected input object.");
    const auto& output = as_object(outputs.front(), "Expected output object.");
    const auto& error_model = as_object(kernel.at("error_model"), "Expected error_model object.");

    NativeKernelManifest manifest;
    manifest.manifest_path = manifest_path;
    manifest.kernel_id = string_field(kernel, "id");
    manifest.source_lowered_unit = string_field(kernel, "source_lowered_unit");
    manifest.backend_family = string_field(backend, "family");
    manifest.backend_artifact_kind = string_field(backend, "artifact_kind");
    manifest.source_artifact = string_field(backend, "source_artifact");
    manifest.entry_symbol = string_field(entry, "symbol");
    manifest.abi = string_field(entry, "abi");
    manifest.input_id = string_field(input, "id");
    manifest.input_type = string_field(input, "type");
    manifest.output_id = string_field(output, "id");
    manifest.output_type = string_field(output, "type");
    manifest.overflow_model = string_field(error_model, "overflow");

    require(manifest.entry_symbol == "frog_example05_run", "Unexpected native kernel entry symbol.");
    require(manifest.abi == "frog_u16_to_result_status", "Unexpected native kernel ABI.");
    require(manifest.input_id == "input_value" && manifest.input_type == "u16", "Unexpected native kernel input surface.");
    require(manifest.output_id == "result" && manifest.output_type == "u16", "Unexpected native kernel output surface.");
    require(manifest.overflow_model == "reject_execution_on_u16_overflow", "Unexpected native kernel overflow model.");

    if (const auto it = error_model.find("error_codes"); it != error_model.end()) {
        for (const auto& item : as_array(it->second, "Expected error_codes array.")) {
            const auto& entry_object = as_object(item, "Expected error code object.");
            const auto code = u16_field(entry_object, "code");
            if (const auto diagnostic_it = entry_object.find("diagnostic"); diagnostic_it != entry_object.end() && diagnostic_it->second.is_string()) {
                manifest.diagnostics_by_error_code[code] = diagnostic_it->second.as_string();
            }
        }
    }

    return manifest;
}

NativeKernelBridge::NativeKernelBridge(NativeKernelManifest manifest, FrogNativeKernelFunction entry_point)
    : manifest_(std::move(manifest)), entry_point_(entry_point) {
    require(entry_point_ != nullptr, "Native kernel bridge requires a non-null entry point.");
}

NativeKernelResult NativeKernelBridge::run(std::uint16_t input_value) const {
    const auto raw = entry_point_(input_value);
    NativeKernelResult result;
    result.ok = raw.ok != 0;
    result.result = raw.result;
    result.error_code = raw.error_code;

    if (!result.ok || result.error_code != 0) {
        const auto diagnostic = manifest_.diagnostics_by_error_code.find(result.error_code);
        result.diagnostic = diagnostic == manifest_.diagnostics_by_error_code.end()
            ? "native kernel returned an unmapped error_code."
            : diagnostic->second;
        result.ok = false;
    }

    return result;
}

const NativeKernelManifest& NativeKernelBridge::manifest() const {
    return manifest_;
}

NativeKernelBridge make_linked_native_kernel_bridge(
    const std::filesystem::path& manifest_path,
    FrogNativeKernelFunction entry_point) {
    return NativeKernelBridge(load_native_kernel_manifest(manifest_path), entry_point);
}

} // namespace frog::runtime
