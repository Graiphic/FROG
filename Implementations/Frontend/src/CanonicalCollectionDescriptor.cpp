#include "frog_native_engine/CanonicalCollectionDescriptor.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <array>
#include <set>

namespace frog::native_engine {
namespace {
using Type=binding::TypeRef;
using Kind=binding::TypeKind;
using Json=source::JsonValue;
constexpr std::array<std::string_view,17> operations{
    "frog.collection.map.build","frog.collection.map.insert","frog.collection.map.remove",
    "frog.collection.map.look","frog.collection.map.extrema","frog.collection.size","frog.collection.empty",
    "frog.collection.set.build","frog.collection.set.insert","frog.collection.set.remove",
    "frog.collection.set.contains","frog.collection.set.extrema","frog.collection.set.union",
    "frog.collection.set.intersection","frog.collection.set.cartesian_product",
    "frog.collection.set.difference","frog.collection.set.symmetric_difference"};
}
std::string_view canonicalCollectionOperationId(std::string_view authored) noexcept {
    if(authored.starts_with("frog.studio."))authored.remove_prefix(12);
    for(const auto id:operations)if(authored==id||authored==id.substr(5))return id;
    return {};
}
Json canonicalCollectionConfiguration(std::span<const std::string> rows) {
    using namespace canonical_json;
    auto value=object(),items=array();
    for(const auto &row:rows)items.array.push_back(string(row));
    value.object={{"profile",string(std::string(canonicalCollectionProfile))},
        {"comparison_policy",string(std::string(canonicalCollectionOrder))},{"rows",std::move(items)}};
    return value;
}
std::optional<CanonicalCollectionDescriptor> canonicalCollectionDescribe(std::string_view operation,
    const Json &configuration,std::span<const CanonicalCollectionPort> actual,std::string *error) {
    const auto fail=[&](std::string why)->std::optional<CanonicalCollectionDescriptor>{if(error)*error=std::move(why);return {};};
    const auto op=canonicalCollectionOperationId(operation);
    if(op.empty()||op!=operation)return fail("No native Collection operation contract is declared for this identity.");
    const auto *profile=configuration.member("profile"),*policy=configuration.member("comparison_policy"),*rows=configuration.member("rows");
    if(configuration.type!=Json::Type::Object||configuration.object.size()!=3||
       !profile||profile->type!=Json::Type::String||profile->string!=canonicalCollectionProfile||
       !policy||policy->type!=Json::Type::String||policy->string!=canonicalCollectionOrder||
       !rows||rows->type!=Json::Type::Array||rows->array.size()>32)
        return fail("Collection requires its exact local profile, comparator revision and bounded logical rows.");
    CanonicalCollectionDescriptor result;result.operation=op;
    std::set<std::string> unique;
    for(const auto &row:rows->array) {
        if(row.type!=Json::Type::String||row.string.empty()||row.string.size()>128||
            row.string.find_first_of(":@")!=std::string::npos||!unique.insert(row.string).second)
            return fail("Build rows require distinct stable identities without reserved port separators.");
        result.rows.push_back(row.string);
    }
    const bool mapBuild=op=="frog.collection.map.build",setBuild=op=="frog.collection.set.build";
    if(!mapBuild&&!setBuild&&!result.rows.empty())return fail("Only Build Map and Build Set declare variable rows.");
    std::set<std::pair<std::string,bool>> portIds;
    for(const auto &p:actual)if(!p.type.known()||!portIds.emplace(p.id,p.input).second)
        return fail("Collection ports require distinct identities and complete exact nominal schemas.");
    const auto find=[&](std::string_view id,bool input)->const Type* {
        const auto p=std::find_if(actual.begin(),actual.end(),[&](const auto &p){return p.id==id&&p.input==input;});
        return p==actual.end()?nullptr:&p->type;
    };
    const auto add=[&](std::string id,Type type,bool input,bool required=true) {
        result.ports.push_back({std::move(id),std::move(type),input,required});
    };
    const auto boolean=Type::primitive("bool");
    if(op.starts_with("frog.collection.map.")) {
        const auto *map=find("map",!mapBuild);
        if(!map||map->kind!=Kind::Map||map->children.size()!=2)
            return fail("The operation requires one complete invariant Map<K,V> schema.");
        const auto key=map->children[0],value=map->children[1];
        if(mapBuild) {
            for(const auto &row:result.rows){add("row:"+row+":key",key,true);add("row:"+row+":value",value,true);}
            add("map",*map,false);
        } else {
            add("map",*map,true);
            if(op=="frog.collection.map.extrema"){add("max_key",key,false);add("min_key",key,false);}
            else {
                add("key",key,true);
                if(op=="frog.collection.map.insert") {
                    add("value",value,true);add("map_out",*map,false);
                    add("key_already_included",boolean,false);add("value_unchanged",boolean,false);
                }else if(op=="frog.collection.map.remove") {
                    add("map_out",*map,false);add("key_not_found",boolean,false);add("value",value,false);
                }else if(op=="frog.collection.map.look") {
                    add("default",value,true,false);add("value",value,false);add("key_not_found",boolean,false);
                }else return fail("The Map operation has no admitted signature.");
            }
        }
    }else if(op=="frog.collection.size"||op=="frog.collection.empty") {
        const auto *collection=find("collection",true);
        if(!collection||(collection->kind!=Kind::Map&&collection->kind!=Kind::Set))
            return fail("Collection Size and Empty Collection? require a real Map or Set.");
        add("collection",*collection,true);
        add(op=="frog.collection.size"?"size":"empty",op=="frog.collection.size"?Type::primitive("i32"):boolean,false);
    }else {
        const bool binary=op=="frog.collection.set.union"||op=="frog.collection.set.intersection"||
            op=="frog.collection.set.cartesian_product"||op=="frog.collection.set.difference"||op=="frog.collection.set.symmetric_difference";
        const auto *set=find(binary?"set_a":"set",!setBuild);
        if(!set||set->kind!=Kind::Set||set->children.size()!=1)
            return fail("The operation requires one complete invariant Set<T> schema.");
        const auto element=set->children[0];
        if(setBuild) {
            for(const auto &row:result.rows)add("row:"+row+":element",element,true);
            add("set",*set,false);
        }else if(binary) {
            const auto *other=find("set_b",true);
            if(!other||other->kind!=Kind::Set||other->children.size()!=1)
                return fail("The second set requires its own complete element schema.");
            add("set_a",*set,true);add("set_b",*other,true);
            if(op=="frog.collection.set.cartesian_product") {
                auto pair=Type::cluster({"first","second"},{element,other->children.front()});
                pair.fieldNames={"first","second"};add("set",Type::set(std::move(pair)),false);
            }else {
                if(!canonicalObjectReferenceFullTypeEqual(*set,*other))return fail("Set algebra requires exactly the same complete element schema.");
                add("set",*set,false);
            }
        }else {
            add("set",*set,true);
            if(op=="frog.collection.set.extrema"){add("max",element,false);add("min",element,false);}
            else {
                add("element",element,true);
                if(op=="frog.collection.set.insert") {add("set_out",*set,false);add("already_included",boolean,false);}
                else if(op=="frog.collection.set.remove") {add("set_out",*set,false);add("not_included",boolean,false);}
                else if(op=="frog.collection.set.contains")add("included",boolean,false);
                else return fail("The Set operation has no admitted signature.");
            }
        }
    }
    if(result.ports.size()!=actual.size())return fail("The actual Collection signature has missing or additional connectors.");
    for(const auto &expected:result.ports) {
        const auto *type=find(expected.id,expected.input);
        if(!type||!canonicalObjectReferenceFullTypeEqual(*type,expected.type))
            return fail("Collection connector "+expected.id+" lost its complete invariant schema.");
    }
    return result;
}
}
