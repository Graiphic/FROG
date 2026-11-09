#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct DiscreteMathTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct DiscreteMathTypeResult { binding::TypeResolution resolution; bool complete=false; };
[[nodiscard]] bool isDiscreteMathFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> discreteMathInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> discreteMathOutputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] DiscreteMathTypeResult resolveDiscreteMathPort(std::string_view catalogId,std::string_view portId,
    std::span<const DiscreteMathTypeEvidence> inputs={});
}
