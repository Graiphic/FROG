#pragma once
#include "BindingType.hpp"

namespace frog::native_engine::binding {
struct BindingResult {
    TypeResolution resolution;
    bool complete = true;
    std::vector<std::string> missingRegions;
};
}
