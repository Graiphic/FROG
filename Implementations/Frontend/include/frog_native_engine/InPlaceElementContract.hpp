#pragma once
// Persisted IPE identities and access policies shared by source adaptation and
// FIR. No document, canvas, widget, renderer or executable resource dependency.
#include <cstdint>
#include <string>
#include <vector>

namespace frog::native_engine {
// The vector is execution order, independent of coordinates and titles.
// This Studio authoring model does not advertise runtime support.
// One authored access/replacement pair, never two independent nodes.
struct DiagramInPlaceAccess {
    std::string id = "element_0";
    std::vector<std::string> fieldPath;
    bool operator==(const DiagramInPlaceAccess &) const = default;
};
struct DiagramInPlacePair {
    std::string id;
    std::string kind = "generic";
    double y = 48.0;
    std::vector<DiagramInPlaceAccess> accesses{DiagramInPlaceAccess{}};
    std::uint32_t splitDimension = 0;
    bool markAsModifier = false;
    bool allowParallelReadOnly = false;
    // Allocation identities never reuse removed channels, including wires
    // retained as obsolete endpoints after a historical row reduction.
    std::uint64_t nextAccessSerial = 1;
    bool operator==(const DiagramInPlacePair &) const = default;
};
} // namespace frog::native_engine
