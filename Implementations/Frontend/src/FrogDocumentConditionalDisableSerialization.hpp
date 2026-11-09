#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace frog::native_engine {
struct DiagramNode;
}

namespace frog::native_engine::document_internal {

[[nodiscard]] std::string serializeConditionalDisableProperties(
    const DiagramNode &node,
    const std::function<std::string(std::string_view)> &serializeRegion,
    int indent);

} // namespace frog::native_engine::document_internal
