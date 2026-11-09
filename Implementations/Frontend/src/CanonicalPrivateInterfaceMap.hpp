#pragma once
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace frog::native_engine::private_interface_map {
enum class IssueKind { Invalid, UnknownField, Ambiguous };
struct Issue {IssueKind kind;std::string path,message;};
// Source-only validation of the presentation fields already read/written by
// the private document codec. Neither consumer may infer executable bindings
// from a visual map. The complete original map stays in source provenance.
inline std::optional<Issue> validate(const source::JsonValue &map,const std::string &path) {
    using Json=source::JsonValue;
    if(map.type!=Json::Type::Object)return Issue{IssueKind::Invalid,path,"The retained private interface map must be an object."};
    constexpr std::string_view names[]={"binding_map_profile","layout_profile_id","layout_pattern_id","layoutPatternId","visual_transforms","visualTransforms"};
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
    return {};
}
inline std::string_view diagnosticCode(IssueKind kind,bool composed) noexcept {
    if(kind==IssueKind::UnknownField)return composed?"migration.composed_field_unavailable":"migration.interface_map_field_unavailable";
    if(kind==IssueKind::Ambiguous)return composed?"migration.composed_interface_map_ambiguous":"migration.interface_map_ambiguous";
    return composed?"migration.composed_interface_map_invalid":"migration.interface_map_invalid";
}
} // namespace frog::native_engine::private_interface_map
