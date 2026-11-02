#pragma once
#include <vector>

namespace frog::native_engine::xy {
// Immutable measurements captured by the host. This DTO contains neither a
// font handle nor rendering logic and is also used by generated native hosts.
struct LayoutFontProfile {
    double fontHeight=0,fallbackAdvance=0;
    std::vector<double> advances;
};
}
