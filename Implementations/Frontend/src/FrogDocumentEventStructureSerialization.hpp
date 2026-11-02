#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

struct DiagramNode;

namespace document_internal {

[[nodiscard]] std::string serializeEventStructureProperties(
    const DiagramNode &node,
    const std::vector<std::string> &caseDiagrams,
    int indent);

} // namespace document_internal
} // namespace frog::native_engine
