#pragma once

#include <iosfwd>
#include <string>

namespace frog::native_engine {
struct SceneObject;
struct StaticVisual;
}

namespace frog::native_engine::document_internal {

// Private-draft emission only. Field order, whitespace and media fallbacks
// remain identical to the original FrogDocumentSerialization.cpp helpers.
std::string staticVisualMediaType(const StaticVisual &visual);

void appendStaticVisual(
    std::ostringstream &output,
    const SceneObject &object,
    const StaticVisual &visual,
    int indent,
    bool comma);

} // namespace frog::native_engine::document_internal
