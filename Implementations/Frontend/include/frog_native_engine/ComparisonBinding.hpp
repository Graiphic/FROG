#pragma once
#include <string_view>
namespace frog::native_engine {
[[nodiscard]] inline bool comparisonHasMode(std::string_view type) noexcept {
    if (type.starts_with("frog.studio.")) type.remove_prefix(12);
    return type == "comparison.equal" || type == "comparison.not_equal" ||
        type == "comparison.greater" || type == "comparison.less" ||
        type == "comparison.greater_or_equal" || type == "comparison.less_or_equal";
}
}
