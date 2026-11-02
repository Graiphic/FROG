#pragma once
#include "BindingType.hpp"
#include <algorithm>
#include <utility>

namespace frog::native_engine {
// Execution-resource classes share TypeRef's nominal reference boundary. They
// are neither object references nor Synchronization Queue resources.
inline constexpr std::string_view kUserEventReferenceClass="frog.events.user";
inline constexpr std::string_view kEventRegistrationReferenceClass="frog.events.registration";
// Data-field paths use '/' as hierarchy and '#' for repeated port rows. Escape
// authored field IDs injectively so neither character can rebind another field.
[[nodiscard]] inline std::string eventPayloadFieldComponent(std::string_view id) {
    std::string result;
    for(const char c:id) {
        if(c=='%')result+="%25";
        else if(c=='/')result+="%2F";
        else if(c=='#')result+="%23";
        else result+=c;
    }
    return result;
}
[[nodiscard]] inline bool isUserEventReference(const binding::TypeRef &type) noexcept {
    return type.kind==binding::TypeKind::Reference && type.name==kUserEventReferenceClass;
}
[[nodiscard]] inline const binding::TypeRef *userEventDataType(const binding::TypeRef &type) noexcept {
    return isUserEventReference(type)&&type.children.size()==1?&type.children.front():nullptr;
}
[[nodiscard]] inline binding::TypeRef userEventReferenceType(binding::TypeRef data) {
    return binding::TypeRef::reference(std::string(kUserEventReferenceClass),std::move(data));
}
[[nodiscard]] inline bool isEventRegistrationReference(const binding::TypeRef &type) noexcept {
    return type.kind==binding::TypeKind::Reference && type.name==kEventRegistrationReferenceClass;
}
[[nodiscard]] inline const binding::TypeRef *eventRegistrationSchema(const binding::TypeRef &type) noexcept {
    return isEventRegistrationReference(type)&&type.children.size()==1?&type.children.front():nullptr;
}
[[nodiscard]] inline binding::TypeRef eventRegistrationReferenceType(binding::TypeRef schema) {
    return binding::TypeRef::reference(std::string(kEventRegistrationReferenceClass),std::move(schema));
}
struct EventRegistrationSource {
    // Stable row identity, prefixed by field identity for clusters of registrations.
    std::string id, eventKind;
    binding::TypeRef sourceType;
};
[[nodiscard]] inline bool isEventSourceReferenceCollection(const binding::TypeRef &type) {
    if(!type.known())return false;
    if(type.kind==binding::TypeKind::Reference)return true;
    if(type.kind==binding::TypeKind::Array)return isEventSourceReferenceCollection(type.children.front());
    return type.kind==binding::TypeKind::Cluster && !type.children.empty() &&
        std::all_of(type.children.begin(),type.children.end(),[](const auto &child){return isEventSourceReferenceCollection(child);});
}
[[nodiscard]] inline bool isEventRegistrationCollection(const binding::TypeRef &type) {
    if(const auto *schema=eventRegistrationSchema(type)) {
        if(!type.known() || schema->kind!=binding::TypeKind::Cluster)return false;
        for(std::size_t index=0;index<schema->fieldIds.size();++index) {
            const auto split=schema->fieldIds[index].find(':');
            if(split==std::string::npos || split==0 || split+1==schema->fieldIds[index].size() ||
                !isEventSourceReferenceCollection(schema->children[index]))return false;
        }
        return true;
    }
    return type.kind==binding::TypeKind::Cluster && type.known() &&
        std::all_of(type.children.begin(),type.children.end(),[](const auto &child){return isEventRegistrationCollection(child);});
}
[[nodiscard]] inline std::vector<EventRegistrationSource> eventRegistrationSources(const binding::TypeRef &type) {
    std::vector<EventRegistrationSource> result;
    if(!isEventRegistrationCollection(type))return result;
    struct Pending {const binding::TypeRef *type;std::string path;};
    std::vector<Pending> pending{{&type,{}}};
    while(!pending.empty()) {
        auto item=std::move(pending.back());pending.pop_back();
        if(const auto *schema=eventRegistrationSchema(*item.type)) {
            for(std::size_t index=0;index<schema->children.size();++index) {
                const auto &field=schema->fieldIds[index];const auto split=field.find(':');
                if(split==std::string::npos || split==0 || split+1==field.size())continue;
                result.push_back({item.path+field.substr(0,split),field.substr(split+1),schema->children[index]});
            }
        } else for(std::size_t index=item.type->children.size();index>0;--index)
            pending.push_back({&item.type->children[index-1],item.path+eventPayloadFieldComponent(item.type->fieldIds[index-1])+"/"});
    }
    return result;
}
} // namespace frog::native_engine
