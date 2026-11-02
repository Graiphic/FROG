#pragma once
#include <string_view>
namespace frog::native_engine::widget_numeric_detail {
struct NumericRepresentationMapping {std::string_view kind,valueType,namedSize;};
const NumericRepresentationMapping *numericRepresentationMapping(std::string_view) noexcept;
bool numericValueTypeIsIntegral(std::string_view) noexcept;
}
