#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {
struct CanonicalComposedMathPort {
    std::string id;
    binding::TypeRef type;
    bool input=false,connected=true;
};
// Portable admission only. Implementations and their ABI remain owned by the
// existing Trigonometric/Hyperbolic providers, never by this Source adapter.
[[nodiscard]] std::string_view canonicalComposedMathMigrationIdentity(std::string_view) noexcept;
[[nodiscard]] bool canonicalComposedMathSignatureValid(std::string_view,
    std::span<const CanonicalComposedMathPort>,std::string *message=nullptr,std::string *port=nullptr);
}
