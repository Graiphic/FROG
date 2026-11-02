#pragma once
#include "frog_native_engine/BindingType.hpp"

namespace frog::native_engine {
// A semaphore controls admission; it does not carry a notification schema.
inline constexpr std::string_view kSemaphoreReferenceClass="frog.sync.semaphore";
[[nodiscard]] inline bool isSemaphoreReference(const binding::TypeRef &type) noexcept {
    return type.kind==binding::TypeKind::Reference && type.name==kSemaphoreReferenceClass && type.children.empty();
}
[[nodiscard]] inline binding::TypeRef semaphoreReferenceType() {
    return binding::TypeRef::reference(std::string(kSemaphoreReferenceClass));
}
} // namespace frog::native_engine
