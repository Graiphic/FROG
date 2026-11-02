#pragma once
#include <string_view>

namespace frog::runtime::native_cpu {struct ScalarProvider;}
namespace frog::native_engine {
// Local scalar signature only: x:F64,n:F64 -> result:F64. No public ABI alias,
// implicit promotion, integer arithmetic-shift overload or aggregate lifting.
inline constexpr std::string_view canonicalScalePowerTwoId="frog.studio.numeric.scale_power_two.real64.v1";
inline constexpr std::string_view canonicalScalePowerTwoProfile="frog.studio.numeric.scale-power-two-even@1";
[[nodiscard]] inline std::string_view canonicalScalePowerTwoMigrationIdentity(std::string_view id) noexcept {
    if(id==canonicalScalePowerTwoId)return id;
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return id=="numeric.scale_power_two"?canonicalScalePowerTwoId:std::string_view{};
}
[[nodiscard]] runtime::native_cpu::ScalarProvider canonicalScalePowerTwoNativeProvider();
}
