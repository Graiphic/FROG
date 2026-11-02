#pragma once
// Pure shared real kernels, no editor, Highlight values, Runtime or SDK.
// Existing real32/real64 formulas and domains are retained. An invalid enum
// throws invalid_argument; exported C functions bind only known enum values.
namespace frog::native_engine {
enum class HyperbolicRealOperation { Unknown, Sinh, Cosh, Tanh, Sech, Csch, Coth, Asinh, Acosh, Atanh, Asech, Acsch, Acoth };
[[nodiscard]] double evaluateHyperbolicReal64(HyperbolicRealOperation,double);
[[nodiscard]] float evaluateHyperbolicReal32(HyperbolicRealOperation,float);
}
