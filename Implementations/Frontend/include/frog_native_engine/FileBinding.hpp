#pragma once

#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogDraftData.hpp"

#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {

struct FileTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct FileTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};
[[nodiscard]] bool isFileFunction(std::string_view catalogId) noexcept;
[[nodiscard]] FileTypeResult resolveFilePort(const DiagramNode &node,
    std::string_view portId, std::span<const FileTypeEvidence> evidence = {});
// Scalar lexical defaults only. Absence means a required input, an unknown port
// or error_in (the shared runtime supplies its canonical no-error cluster).
[[nodiscard]] std::optional<std::string> fileOptionalInputDefault(
    const DiagramNode &node, std::string_view portId,
    std::span<const std::string> connectedInputPorts = {});

} // namespace frog::native_engine
