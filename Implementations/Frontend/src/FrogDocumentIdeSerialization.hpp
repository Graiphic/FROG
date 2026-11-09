#pragma once

#include <iosfwd>

#include "frog_native_engine/FrogWidgetData.hpp"
namespace frog::native_engine {
struct FrogEditorWindows;

namespace document_internal {
bool hasIdePreferences(FrogWidgetDataView canvas) noexcept;
void appendIdePreferences(std::ostringstream &output, FrogWidgetDataView canvas,
    const FrogEditorWindows &windows, int indent, bool comma = true);
} // namespace document_internal
} // namespace frog::native_engine
