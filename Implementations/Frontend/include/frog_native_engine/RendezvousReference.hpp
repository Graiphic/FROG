#pragma once
#include "frog_native_engine/BindingType.hpp"

namespace frog::native_engine {
// A rendezvous synchronizes arrivals; its reference carries no message schema.
inline constexpr std::string_view kRendezvousReferenceClass="frog.sync.rendezvous";
[[nodiscard]] inline bool isRendezvousReference(const binding::TypeRef &type) noexcept {
    return type.kind==binding::TypeKind::Reference && type.name==kRendezvousReferenceClass && type.children.empty();
}
[[nodiscard]] inline binding::TypeRef rendezvousReferenceType() {
    return binding::TypeRef::reference(std::string(kRendezvousReferenceClass));
}
} // namespace frog::native_engine
