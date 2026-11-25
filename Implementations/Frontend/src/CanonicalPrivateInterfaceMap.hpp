#pragma once
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <iterator>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>

namespace frog::native_engine::private_interface_map {
enum class IssueKind { Invalid, UnknownField, Ambiguous };
struct Issue {IssueKind kind;std::string path,message;};
inline std::optional<Issue> validatePortHistory(const source::JsonValue &map,const std::string &path) {
    using Json=source::JsonValue;
    const auto *owners=map.member("widget_port_owners");
    if(owners) {
        const auto ownerPath=canonical_json::pointer(path,"widget_port_owners");
        if(owners->type!=Json::Type::Object)
            return Issue{IssueKind::Invalid,ownerPath,"Map port owners must be keyed by port ID."};
        for(const auto &[id,owner]:owners->object) {
            if(id.empty() || (owner.type!=Json::Type::Null && (owner.type!=Json::Type::String || owner.string.empty())))
                return Issue{IssueKind::Invalid,canonical_json::pointer(ownerPath,id),"A Map port needs a nonempty ID and a widget ID or null for a reserved identity."};
        }
    }
    std::set<std::string_view> detachedIds;
    for(const auto key:{"detached_inputs","detached_outputs"}) {
        const auto *ports=map.member(key);
        if(!ports)continue;
        const auto listPath=canonical_json::pointer(path,key);
        if(ports->type!=Json::Type::Array)
            return Issue{IssueKind::Invalid,listPath,"Detached Map port declarations must be an array."};
        for(std::size_t index=0;index<ports->array.size();++index) {
            const auto &port=ports->array[index];
            const auto itemPath=canonical_json::pointer(listPath,std::to_string(index));
            if(port.type!=Json::Type::Object)
                return Issue{IssueKind::Invalid,itemPath,"A detached Map port must retain its declaration object."};
            for(const auto field:{"id","type"}) {
                const auto *value=port.member(field);
                if(!value || value->type!=Json::Type::String || value->string.empty())
                    return Issue{IssueKind::Invalid,canonical_json::pointer(itemPath,field),"A detached Map port must retain its ID and declared type."};
            }
            const auto &id=port.member("id")->string;
            if(!owners || !owners->member(id))
                return Issue{IssueKind::Invalid,canonical_json::pointer(itemPath,"id"),"A detached Map port must have a reserved owner entry."};
            if(!detachedIds.insert(id).second)
                return Issue{IssueKind::Ambiguous,canonical_json::pointer(itemPath,"id"),"The same Map port identity occurs in multiple detached declarations."};
            // The full original declaration (including any extension fields) is
            // retained in the source. It does not declare an executable port here.
        }
    }
    return {};
}
// Source-only validation of the presentation fields already read/written by
// the private document codec. Neither consumer may infer executable bindings
// from a visual map. The complete original map stays in source provenance.
inline std::optional<Issue> validate(const source::JsonValue &map,const std::string &path) {
    using Json=source::JsonValue;
    if(map.type!=Json::Type::Object)return Issue{IssueKind::Invalid,path,"The retained private interface map must be an object."};
    constexpr std::string_view names[]={"binding_map_profile","layout_profile_id","layout_pattern_id","layoutPatternId","visual_transforms","visualTransforms","widget_port_owners","detached_inputs","detached_outputs"};
    for(const auto &[key,_]:map.object)if(std::find(std::begin(names),std::end(names),key)==std::end(names))
        return Issue{IssueKind::UnknownField,canonical_json::pointer(path,key),"This private interface-map field has no presentation adaptation contract."};
    for(const auto key:{"binding_map_profile","layout_profile_id","layout_pattern_id","layoutPatternId"})
        if(const auto *value=map.member(key);value&&value->type!=Json::Type::String)
            return Issue{IssueKind::Invalid,canonical_json::pointer(path,key),"A private interface-map profile/pattern must remain a string."};
    for(const auto key:{"visual_transforms","visualTransforms"})if(const auto *value=map.member(key))
        if(value->type!=Json::Type::Array||std::any_of(value->array.begin(),value->array.end(),[](const auto &item){return item.type!=Json::Type::String;}))
            return Issue{IssueKind::Invalid,canonical_json::pointer(path,key),"Private interface-map visual transforms must remain an array of strings."};
    for(const auto &[preferred,alias]:{std::pair{"binding_map_profile","layout_profile_id"},std::pair{"layout_pattern_id","layoutPatternId"},std::pair{"visual_transforms","visualTransforms"}})
        if(const auto *a=map.member(preferred))if(const auto *b=map.member(alias);b&&!canonical_json::exact(*a,*b))
            return Issue{IssueKind::Ambiguous,path,"Conflicting private interface-map aliases need an explicit authoring decision."};
    return validatePortHistory(map,path);
}
inline std::string_view diagnosticCode(IssueKind kind,bool composed) noexcept {
    if(kind==IssueKind::UnknownField)return composed?"migration.composed_field_unavailable":"migration.interface_map_field_unavailable";
    if(kind==IssueKind::Ambiguous)return composed?"migration.composed_interface_map_ambiguous":"migration.interface_map_ambiguous";
    return composed?"migration.composed_interface_map_invalid":"migration.interface_map_invalid";
}
} // namespace frog::native_engine::private_interface_map
