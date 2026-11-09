#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace frog::native_engine {

struct DiagramNode;

namespace document_internal {

[[nodiscard]] std::string serializeDisabledStructureProperties(
    const DiagramNode &node,
    const std::function<std::string(std::string_view)> &serializeRegion,
    int indent);

} // namespace document_internal
} // namespace frog::native_engine
