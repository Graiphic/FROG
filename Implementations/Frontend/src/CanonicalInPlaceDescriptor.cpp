#include "frog_native_engine/CanonicalInPlaceDescriptor.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <charconv>
#include <set>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using namespace canonical_json;
void require(bool good,const char *why) {if(!good)throw std::invalid_argument(why);}
bool fields(const Json &value,std::initializer_list<std::string_view> allowed) {
    if(value.type!=Json::Type::Object)return false;
    for(const auto &[name,_]:value.object)if(std::find(allowed.begin(),allowed.end(),name)==allowed.end())return false;
    return true;
}
std::string id(const Json *value) {
    require(value&&value->type==Json::Type::String&&!value->string.empty()&&value->string.size()<=1024&&
        value->string.find_first_of(":@\0",0,3)==std::string::npos,"IPE identities must be explicit, bounded and free of reserved separators.");
    return value->string;
}
bool flag(const Json *value) {
    if(!value)return false;require(value->type==Json::Type::Boolean,"IPE flags must be Boolean.");return value->boolean;
}
std::uint32_t ordinal(const Json *value) {
    if(!value)return 0;require(value->type==Json::Type::Number,"IPE split dimension must be an exact unsigned ordinal.");
    std::uint32_t result{};const auto r=std::from_chars(value->string.data(),value->string.data()+value->string.size(),result);
    require(r.ec==std::errc{}&&r.ptr==value->string.data()+value->string.size()&&result<16,"IPE split dimension exceeds the supported rank.");return result;
}
}
std::optional<std::vector<DiagramInPlacePair>> canonicalInPlaceReadPairs(const Json &raw,bool authored,std::string *diagnostic) {
    try {
        require(raw.type==Json::Type::Array&&raw.array.size()<=32,"IPE needs a bounded explicit pair array (maximum32).");
        std::vector<DiagramInPlacePair> result;std::set<std::string> ids;
        constexpr std::string_view families[]{"array_index","array_split","cluster","waveform","variant","generic","dvr","variant_attribute","map"};
        for(const auto &entry:raw.array) {
            require(authored?fields(entry,{"id","kind","y","split_dimension","mark_as_modifier","allow_parallel_read_only","accesses","next_access_serial"}):
                fields(entry,{"id","kind","split_dimension","mark_as_modifier","allow_parallel_read_only","accesses"}),"An IPE pair contains an unknown semantic field.");
            DiagramInPlacePair pair;pair.id=id(entry.member("id"));pair.kind=id(entry.member("kind"));pair.accesses.clear();
            require(ids.insert(pair.id).second,"An IPE pair ID is duplicated.");
            require(std::find(std::begin(families),std::end(families),pair.kind)!=std::end(families),"Unknown IPE family.");
            pair.splitDimension=ordinal(entry.member("split_dimension"));pair.markAsModifier=flag(entry.member("mark_as_modifier"));pair.allowParallelReadOnly=flag(entry.member("allow_parallel_read_only"));
            require(!pair.allowParallelReadOnly||pair.kind=="dvr","Only a DVR pair has a parallel read-only policy.");
            const auto *accesses=entry.member("accesses");require(accesses&&accesses->type==Json::Type::Array&&!accesses->array.empty()&&accesses->array.size()<=32,"IPE accesses must number1..32.");
            require((pair.kind!="generic"&&pair.kind!="variant"&&pair.kind!="dvr"&&pair.kind!="map")||accesses->array.size()==1,"This IPE family has exactly one access.");
            require(pair.kind!="array_split"||accesses->array.size()>=2,"Array Split needs at least two partitions.");
            std::set<std::string> accessIds;
            for(const auto &access:accesses->array) {
                require(fields(access,{"id","field_path"}),"An IPE access contains an unknown semantic field.");
                DiagramInPlaceAccess a;a.id=id(access.member("id"));require(accessIds.insert(a.id).second,"An IPE access ID is duplicated.");
                const auto *path=access.member("field_path");require(path&&path->type==Json::Type::Array&&path->array.size()<=32,"An IPE field selection needs its bounded stable path.");
                for(const auto &part:path->array) {
                    require(part.type==Json::Type::String&&!part.string.empty()&&part.string.size()<=1024&&part.string.find('\0')==std::string::npos,"An IPE field path contains no actual stable ID.");
                    a.fieldPath.push_back(part.string);
                }
                if(pair.kind=="cluster")require(!a.fieldPath.empty(),"A Cluster access needs an explicit stable field path; ordinal retargeting is not allowed.");
                if(pair.kind!="cluster"&&pair.kind!="waveform")require(a.fieldPath.empty(),"This IPE family does not select Record fields.");
                pair.accesses.push_back(std::move(a));
            }
            if(pair.kind=="cluster"||pair.kind=="waveform")for(std::size_t a=0;a<pair.accesses.size();++a)for(std::size_t b=a+1;b<pair.accesses.size();++b) {
                const auto path=[&](std::size_t at){return pair.kind=="waveform"&&pair.accesses[at].fieldPath.empty()?std::vector<std::string>{"Y"}:pair.accesses[at].fieldPath;};
                const auto left=path(a),right=path(b);const auto count=(std::min)(left.size(),right.size());
                require(!count||!std::equal(left.begin(),left.begin()+count,right.begin()),"IPE field selections overlap.");
            }
            result.push_back(std::move(pair));
        }
        return result;
    }catch(const std::exception &error){if(diagnostic)*diagnostic=error.what();return {};}
}
source::JsonValue canonicalInPlaceConfiguration(std::span<const DiagramInPlacePair> pairs) {
    auto config=object(),list=array();config.object["profile"]=string(std::string(canonicalInPlaceProfile));
    for(const auto &pair:pairs) {
        auto item=object(),accesses=array();Json dimension;dimension.type=Json::Type::Number;dimension.string=std::to_string(pair.splitDimension);
        item.object={{"id",string(pair.id)},{"kind",string(pair.kind)},{"split_dimension",std::move(dimension)},
            {"mark_as_modifier",boolean(pair.markAsModifier)},{"allow_parallel_read_only",boolean(pair.allowParallelReadOnly)}};
        for(const auto &access:pair.accesses){auto a=object(),path=array();for(const auto &part:access.fieldPath)path.array.push_back(string(part));a.object={{"id",string(access.id)},{"field_path",std::move(path)}};accesses.array.push_back(std::move(a));}
        item.object["accesses"]=std::move(accesses);list.array.push_back(std::move(item));
    }
    config.object["pairs"]=std::move(list);return config;
}
std::optional<std::vector<DiagramInPlacePair>> canonicalInPlaceReadConfiguration(const Json &raw,std::string *diagnostic) {
    if(!fields(raw,{"profile","pairs"})||!raw.member("profile")||raw.member("profile")->type!=Json::Type::String||raw.member("profile")->string!=canonicalInPlaceProfile||!raw.member("pairs")) {
        if(diagnostic)*diagnostic="An IPE requires the explicit closed local frog.studio.ipe@1 profile.";return {};
    }
    return canonicalInPlaceReadPairs(*raw.member("pairs"),false,diagnostic);
}
}
