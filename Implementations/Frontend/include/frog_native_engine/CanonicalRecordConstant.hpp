#pragma once
#include "BindingType.hpp"
#include "FrogJson.hpp"
#include "DocumentDiagnostic.hpp"
#include <optional>
#include <string>
#include <string_view>

namespace frog::native_engine {
inline constexpr std::string_view canonicalImmutableRecordProfile="frog.studio.immutable-record-constant@1";
struct CanonicalRecordDiagnostic {std::string code,message;DocumentDiagnosticDomain domain=DocumentDiagnosticDomain::Semantics;};
// Local immutable Record producer. This is not a Cluster Front Panel control,
// an Event payload adapter, a reference carrier or an Array capability.
[[nodiscard]] std::optional<binding::TypeRef> canonicalRecordTransportType(
    std::string_view,CanonicalRecordDiagnostic *diagnostic=nullptr);
[[nodiscard]] bool canonicalRecordLiteralValid(const binding::TypeRef &,
    const source::JsonValue &,CanonicalRecordDiagnostic *diagnostic=nullptr);
[[nodiscard]] bool canonicalRecordOutputIdentity(std::string_view,std::size_t maximumBytes=1024) noexcept;
}
