#pragma once
// Private pure kernels shared by real32/real64 binding and the local typed ABI.
// Complex kernels remain in TrigonometricBinding. No editor, Runtime or SDK.
#include <array>
#include <cstdint>
#include <string_view>
namespace frog::native_engine {
enum class TrigonometricRealOperation { Unknown, Sin, Cos, Tan, Csc, Sec, Cot, Acsc, Asec, Acot, Asin, Acos, Atan, Atan2, SinCos, Sinc };
enum class TrigonometricCAbi { ReturnF64, StatusOutF64Pair };
struct TrigonometricReal64Contract { std::string_view catalogId,operationId,symbol; std::uint32_t arity; TrigonometricCAbi callAbi; };
inline constexpr std::string_view trigonometricReal64SemanticVersion="frog.trigonometric.ieee.v1";
inline constexpr std::string_view trigonometricReal64AbiProfile="frog.private.trigonometric.real64-ieee.c.v1.win64";
inline constexpr std::array<TrigonometricReal64Contract,15> trigonometricReal64Contracts{{
    {"math.trigonometric.sin","frog.studio.math.trigonometric.sin","frog_studio_trigonometric_sin_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.cos","frog.studio.math.trigonometric.cos","frog_studio_trigonometric_cos_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.tan","frog.studio.math.trigonometric.tan","frog_studio_trigonometric_tan_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.csc","frog.studio.math.trigonometric.csc","frog_studio_trigonometric_csc_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.sec","frog.studio.math.trigonometric.sec","frog_studio_trigonometric_sec_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.cot","frog.studio.math.trigonometric.cot","frog_studio_trigonometric_cot_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.acsc","frog.studio.math.trigonometric.acsc","frog_studio_trigonometric_acsc_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.asec","frog.studio.math.trigonometric.asec","frog_studio_trigonometric_asec_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.acot","frog.studio.math.trigonometric.acot","frog_studio_trigonometric_acot_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.asin","frog.studio.math.trigonometric.asin","frog_studio_trigonometric_asin_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.acos","frog.studio.math.trigonometric.acos","frog_studio_trigonometric_acos_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.atan","frog.studio.math.trigonometric.atan","frog_studio_trigonometric_atan_f64_v1",1,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.atan2","frog.studio.math.trigonometric.atan2","frog_studio_trigonometric_atan2_f64_v1",2,TrigonometricCAbi::ReturnF64},
    {"math.trigonometric.sincos","frog.studio.math.trigonometric.sincos","frog_studio_trigonometric_sincos_f64_v1",1,TrigonometricCAbi::StatusOutF64Pair},
    {"math.trigonometric.sinc","frog.studio.math.trigonometric.sinc","frog_studio_trigonometric_sinc_f64_v1",1,TrigonometricCAbi::ReturnF64},
}};
[[nodiscard]] inline const TrigonometricReal64Contract *canonicalTrigonometricReal64Contract(std::string_view id) noexcept {
    for(const auto &entry:trigonometricReal64Contracts)if(entry.catalogId==id||entry.operationId==id)return &entry;
    return nullptr;
}
[[nodiscard]] double evaluateTrigonometricReal64(TrigonometricRealOperation,double);
[[nodiscard]] float evaluateTrigonometricReal32(TrigonometricRealOperation,float);
[[nodiscard]] double trigonometricAtan2Real64(double y,double x) noexcept;
[[nodiscard]] float trigonometricAtan2Real32(float y,float x) noexcept;
struct TrigonometricReal64Pair { double sin,cos; };
struct TrigonometricReal32Pair { float sin,cos; };
[[nodiscard]] TrigonometricReal64Pair trigonometricSinCosReal64(double) noexcept;
[[nodiscard]] TrigonometricReal32Pair trigonometricSinCosReal32(float) noexcept;
}
