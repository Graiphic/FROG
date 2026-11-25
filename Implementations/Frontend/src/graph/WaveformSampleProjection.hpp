#pragma once

#include "frog_native_engine/graph/WaveformGraph.hpp"
#include <cmath>

namespace frog::native_engine::graph {

inline bool sameSampleProjection(const Dataset &a, const Dataset &b) {
    const auto defaultEmpty = [](const Dataset &data) {
        return data.series.size() == 1 && data.series[0].y.empty() &&
            data.series[0].origin == 0 && data.series[0].step == 1;
    };
    // Empty legacy graphs can represent an empty Y array with no plot.
    if ((a.series.empty() && defaultEmpty(b)) || (b.series.empty() && defaultEmpty(a))) return true;
    if (a.series.size() != b.series.size()) return false;
    for (std::size_t i = 0; i < a.series.size(); ++i) {
        const auto &left = a.series[i], &right = b.series[i];
        if (left.origin != right.origin || left.step != right.step || left.y.size() != right.y.size()) return false;
        for (std::size_t j = 0; j < left.y.size(); ++j)
            if (left.y[j] != right.y[j] && !(std::isnan(left.y[j]) && std::isnan(right.y[j]))) return false;
    }
    return true;
}

} // namespace frog::native_engine::graph
