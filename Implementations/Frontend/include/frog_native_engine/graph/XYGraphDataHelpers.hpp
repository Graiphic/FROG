#pragma once
#include "XYGraph.hpp"
namespace frog::native_engine::xy::data_detail {
bool finiteRect(Rect);
Coordinate logOrigin(const AxisRange&);
Coordinate unprojected(const Axis&,const AxisRange&,double) noexcept;
std::optional<double> timeUnit(std::string_view);
std::optional<std::int64_t> timeOrigin(std::string_view);
std::optional<int> timeZone(std::string_view);
}
