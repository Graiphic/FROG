#pragma once
// Read-only projection of actual widget values into the Source value codec.
// No execution, Runtime carrier, UI window or LLVM dependency.
#include "frog_native_engine/FrogJson.hpp"
#include <optional>
#include <string>
namespace frog::native_engine {
struct WidgetInstance;
[[nodiscard]] std::optional<source::JsonValue>
typedWidgetSourceValue(const WidgetInstance &, std::string *error = nullptr);
} // namespace frog::native_engine
