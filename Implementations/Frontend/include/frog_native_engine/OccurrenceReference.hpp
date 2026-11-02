#pragma once
#include "frog_native_engine/BindingType.hpp"

namespace frog::native_engine {
// A signal identity, never a message container or a persisted runtime address.
inline constexpr std::string_view kOccurrenceReferenceClass="frog.sync.occurrence";
[[nodiscard]] inline bool isOccurrenceReference(const binding::TypeRef &type) noexcept {
    return type.kind==binding::TypeKind::Reference && type.name==kOccurrenceReferenceClass && type.children.empty();
}
[[nodiscard]] inline binding::TypeRef occurrenceReferenceType() {
    return binding::TypeRef::reference(std::string(kOccurrenceReferenceClass));
}
} // namespace frog::native_engine
