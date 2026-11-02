#pragma once

#include <string>
#include <string_view>

namespace frog::native_engine {

struct DiagramNode;

namespace document_internal {

[[nodiscard]] std::string serializeWhileLoopStructureProperties(
    const DiagramNode &node,
    std::string_view bodyDiagram,
    int indent);

} // namespace document_internal
} // namespace frog::native_engine
