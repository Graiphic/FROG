#pragma once
#include "BindingType.hpp"
#include <utility>

namespace frog::native_engine {
// Execution resource identity, deliberately disjoint from Queue and widgets.
inline constexpr std::string_view kNotifierReferenceClass="frog.sync.notifier";
inline constexpr std::string_view kNotifierMultipleProfile="frog.studio.notifier.multiple.v1";
[[nodiscard]] inline bool isNotifierReference(const binding::TypeRef &type) noexcept {
    return type.kind==binding::TypeKind::Reference && type.name==kNotifierReferenceClass;
}
[[nodiscard]] inline const binding::TypeRef *notifierReferenceElementType(const binding::TypeRef &type) noexcept {
    return isNotifierReference(type)&&type.children.size()==1?&type.children.front():nullptr;
}
[[nodiscard]] inline binding::TypeRef notifierReferenceType(binding::TypeRef element) {
    return binding::TypeRef::reference(std::string(kNotifierReferenceClass),std::move(element));
}
// FROG's explicit version-one representation preserves ragged/ranked messages.
[[nodiscard]] inline binding::TypeRef notifierMultipleNotificationType(binding::TypeRef element) {
    if(element.kind==binding::TypeKind::Array) {
        element=binding::TypeRef::cluster({"notification"},{std::move(element)});
        element.fieldNames={"notification"};
    }
    return binding::TypeRef::array(std::move(element),1);
}
} // namespace frog::native_engine
