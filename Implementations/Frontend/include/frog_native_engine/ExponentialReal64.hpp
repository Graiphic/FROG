#pragma once

namespace frog::native_engine {
// Graiphic implementation kernel shared by authoring's explicit evaluator
// and immutable compiled scalar providers. No graph, document or Runtime
// object is read; IEEE values are returned without an invented finite fault.
enum class ExponentialReal64Operation {
    Exp, Expm1, Pow10, Pow2, Power, Root, Ln, Log1p, Log10, Log2, LogBase
};
[[nodiscard]] bool exponentialReal64OddInteger(double value) noexcept;
// rootIndexIsOdd allows the authored evaluator to preserve exact integer
// parity before its historic numeric projection. Native callers are f64 only.
[[nodiscard]] double evaluateExponentialReal64(ExponentialReal64Operation,
    double x, double y, bool rootIndexIsOdd) noexcept;
}
