#pragma once
#include "BindingType.hpp"
#include <utility>

namespace frog::native_engine {
// Private execution resource identity; unrelated to a reference TO a widget.
inline constexpr std::string_view kQueueReferenceClass="frog.sync.queue";
[[nodiscard]] inline bool isQueueReference(const binding::TypeRef &type) noexcept {
    return type.kind==binding::TypeKind::Reference && type.name==kQueueReferenceClass;
}
[[nodiscard]] inline const binding::TypeRef *queueReferenceElementType(const binding::TypeRef &type) noexcept {
    return isQueueReference(type)&&type.children.size()==1?&type.children.front():nullptr;
}
[[nodiscard]] inline binding::TypeRef queueReferenceType(binding::TypeRef element) {
    return binding::TypeRef::reference(std::string(kQueueReferenceClass),std::move(element));
}
} // namespace frog::native_engine
