#pragma once

#include "frog_native_engine/FrogDraftData.hpp"

#include <string>

namespace frog::native_engine::document_internal {

[[nodiscard]] std::string serializeCaseStructureProperties(
    const DiagramNode &node,
    int indent);

[[nodiscard]] std::string serializeCaseStructureSelectorTerminal(
    const DiagramNode &node,
    int indent);

} // namespace frog::native_engine::document_internal
