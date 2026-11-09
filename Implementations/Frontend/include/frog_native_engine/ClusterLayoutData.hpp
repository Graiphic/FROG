#pragma once
#include "frog_native_engine/WidgetInstance.hpp"
namespace frog::native_engine {
inline std::string_view clusterAutoSizingName(ClusterAutoSizing mode) noexcept {
    switch (mode) {
    case ClusterAutoSizing::SizeToFit: return "size_to_fit";
    case ClusterAutoSizing::ArrangeHorizontally: return "horizontal";
    case ClusterAutoSizing::ArrangeVertically: return "vertical";
    default: return "none";
    }
}

inline ClusterAutoSizing parseClusterAutoSizing(std::string_view name) noexcept {
    if (name == "size_to_fit") return ClusterAutoSizing::SizeToFit;
    if (name == "horizontal") return ClusterAutoSizing::ArrangeHorizontally;
    if (name == "vertical") return ClusterAutoSizing::ArrangeVertically;
    return ClusterAutoSizing::None;
}

}
