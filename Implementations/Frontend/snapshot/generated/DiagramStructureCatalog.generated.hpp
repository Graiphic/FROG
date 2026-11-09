// Generated Studio-private boundary catalog; not an executable FROG contract.
#pragma once
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include <array>

namespace frog::native_engine::generated::structures {
inline constexpr std::array<DiagramStructureCatalogEntry, 9> catalog{{
    { DiagramStructureFamily::Conditional, "frog.studio.structures.case_structure", "case", "Conditional Structure" },
    { DiagramStructureFamily::ForLoop, "frog.studio.structures.for_loop", "for_loop", "For Loop" },
    { DiagramStructureFamily::WhileLoop, "frog.studio.structures.while_loop", "while_loop", "While Loop" },
    { DiagramStructureFamily::TimedLoop, "frog.studio.structures.timed_loop", "timed_loop", "Timed Loop" },
    { DiagramStructureFamily::Event, "frog.studio.structures.event_structure", "event_structure", "Event Structure" },
    { DiagramStructureFamily::Disabled, "frog.studio.structures.disabled_structure", "disabled_structure", "Diagram Disable Structure" },
    { DiagramStructureFamily::ConditionalDisable, "frog.studio.structures.conditional_disable_structure", "conditional_disable_structure", "Conditional Disable Structure" },
    { DiagramStructureFamily::Sequence, "frog.studio.structures.sequence_structure", "sequence_structure", "Sequence Structure" },
    { DiagramStructureFamily::InPlace, "frog.studio.structures.in_place_element_structure", "in_place_element_structure", "In Place Element Structure" },
}};
} // namespace frog::native_engine::generated::structures
