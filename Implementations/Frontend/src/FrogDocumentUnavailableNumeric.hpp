#pragma once
// Inspect existing private widget/constant identities before materialization.
// Unknown numeric data is archived by the existing recovery authority, never
// decoded as CDB. No new state grammar, value decoder or execution path.
#include "frog_native_engine/UnavailableNumericRepresentation.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include "FrogJsonInternal.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace frog::native_engine::document_internal {
struct UnavailableNumericFact {std::string ownerPath,pointer,spelling,canonicalIdentity;};
inline std::vector<UnavailableNumericFact> unavailableNumericFacts(const source::JsonValue &root,bool widgetTransport=false) {
    using Json=source::JsonValue;
    const auto text=[](const Json *v)->std::string_view{return v&&v->type==Json::Type::String?v->string:std::string_view{};};
    const auto part=[](std::string path,std::string_view key){path+='/';for(const auto c:key){if(c=='~')path+="~0";else if(c=='/')path+="~1";else path+=c;}return path;};
    const auto encodedCandidate=[](std::string_view bytes){
        // A legal JSON escape can spell a class key or unavailable type.
        // Such candidate transports need the real parser before filtering.
        if(bytes.find("\\u")!=bytes.npos)return true;
        if(bytes.find("\"classId\"")==bytes.npos&&bytes.find("\"class_ref\"")==bytes.npos)return false;
        for(auto i=bytes.find_first_of("cC");i!=bytes.npos;i=bytes.find_first_of("cC",i+1))
            if(!unavailableComplexNumericIdentity(bytes.substr(i,3)).empty()||!unavailableComplexNumericIdentity(bytes.substr(i,16)).empty())return true;
        return false;
    };
    struct Frame {const Json *value;std::string path,owner,embedded;bool widgetOwner=false,numericArrayOwner=false;};
    std::vector<Frame> pending;
    if(widgetTransport)pending.push_back({&root,{},"/",{},true,false});
    // A Numeric-looking vendor JSON object or string elsewhere in metadata
    // is not a declaration. Begin only at real authoring ownership roots.
    if(const auto *panel=root.member("frontPanel"))if(const auto *widgets=panel->member("widgets");widgets&&widgets->type==Json::Type::Array)
        for(std::size_t i=0;i<widgets->array.size();++i){const auto path="/frontPanel/widgets/"+std::to_string(i);pending.push_back({&widgets->array[i],path,path,{},true,false});}
    for(const auto key:{"diagram","blockDiagram"})if(const auto *diagram=root.member(key))if(const auto *nodes=diagram->member("nodes");nodes&&nodes->type==Json::Type::Array)
        for(std::size_t i=0;i<nodes->array.size();++i){const auto path="/"+std::string(key)+"/nodes/"+std::to_string(i);pending.push_back({&nodes->array[i],path,path,{},false,false});}
    std::vector<std::unique_ptr<Json>> decoded;
    struct Cleanup {std::vector<std::unique_ptr<Json>> &trees;~Cleanup(){for(auto &tree:trees)source::internal::clearJsonTree(*tree);}} cleanup{decoded};
    std::vector<UnavailableNumericFact> facts;std::size_t bytes=0,nodes=0;
    while(!pending.empty()) {
        auto frame=std::move(pending.back());pending.pop_back();const auto &value=*frame.value;
        if(++nodes>1'000'000)throw std::runtime_error("Unavailable Numeric inspection exceeds its bounded document budget.");
        if(value.type==Json::Type::Array){for(std::size_t i=0;i<value.array.size();++i){
            const auto &child=value.array[i];
            if(frame.numericArrayOwner&&frame.path.ends_with("/array_constant/values")&&child.type==Json::Type::String&&encodedCandidate(child.string)){
                const auto where=frame.path+"/"+std::to_string(i);
                if(child.string.size()>64U*1024U*1024U-bytes)throw std::runtime_error("Unavailable Numeric cell evidence exceeds its document budget.");
                bytes+=child.string.size();auto parsed=source::internal::parseAuthoringJsonCandidate(child.string);if(!parsed)continue;
                decoded.push_back(std::make_unique<Json>(std::move(*parsed)));pending.push_back({decoded.back().get(),where,frame.owner,frame.embedded.empty()?where:frame.embedded,frame.widgetOwner,frame.numericArrayOwner});
            }else if(child.type==Json::Type::Object||child.type==Json::Type::Array)pending.push_back({&child,frame.path+"/"+std::to_string(i),frame.owner,frame.embedded,frame.widgetOwner,frame.numericArrayOwner});
        }continue;}
        if(value.type!=Json::Type::Object)continue;
        auto klass=text(value.member("classId"));if(klass.empty())klass=text(value.member("class_ref"));
        const auto kind=text(value.member("kind"));const auto id=text(value.member("id"));
        const bool widget=klass.starts_with("frog.widgets.")||klass.starts_with("frog.studio.widgets.");
        const bool node=!id.empty()&&(kind=="primitive"||kind=="structure"||kind=="widget_value"||kind=="frog_call");
        if(widget&&frame.owner.empty()){frame.owner=frame.path;frame.widgetOwner=true;}
        if(node&&!frame.widgetOwner){frame.owner=frame.path;}
        const bool numericWidget=klass=="frog.widgets.numeric"||klass=="frog.widgets.numeric_control"||klass=="frog.widgets.numeric_indicator";
        const auto nodeType=text(value.member("type"));
        const bool numericConstant=kind=="primitive"&&(((nodeType.starts_with("frog.studio.numeric.")||nodeType.starts_with("numeric."))&&nodeType.find("constant")!=nodeType.npos)||nodeType=="numeric");
        if(node&&!frame.widgetOwner){const auto declared=text(value.member("value_type"));frame.numericArrayOwner=nodeType.find("array_constant")!=nodeType.npos&&declared!="string"&&declared!="path"&&declared!="bool";}
        const auto add=[&](const Json *candidate,const std::string &where){const auto spelling=text(candidate),identity=unavailableComplexNumericIdentity(spelling);
            if(!identity.empty()&&!frame.owner.empty())facts.push_back({frame.owner,frame.embedded.empty()?where:frame.embedded,std::string(spelling),std::string(identity)});};
        if(numericWidget){add(value.member("valueType"),part(frame.path,"valueType"));add(value.member("value_type"),part(frame.path,"value_type"));
            auto declared=text(value.member("valueType"));if(declared.empty())declared=text(value.member("value_type"));
            // An explicit supported Complex declaration remains authoritative;
            // an obsolete presentation property alone is not an opaque type.
            if(declared!="c32"&&declared!="c64")if(const auto *props=value.member("props"))for(const auto key:{"representation.kind","data_type.representation","data_type.named_numeric_size"})
                add(props->member(key),part(part(frame.path,"props"),key));}
        if(numericConstant)add(value.member("value_type"),part(frame.path,"value_type"));
        for(const auto &[key,child]:value.object){
            const auto where=part(frame.path,key);
            if(child.type!=Json::Type::String){if(child.type==Json::Type::Object||child.type==Json::Type::Array)pending.push_back({&child,where,frame.owner,frame.embedded,frame.widgetOwner,frame.numericArrayOwner});continue;}
            // Only existing widget-owned default/cell transport strings are
            // nested JSON. Ordinary String/comment/source text is never scanned.
            const bool encoded=key=="data.default"||((key=="value"||key=="widget")&&text(value.member("kind"))=="widget");
            if(!encoded||!encodedCandidate(child.string))continue;
            if(child.string.size()>64U*1024U*1024U-bytes)throw std::runtime_error("Unavailable Numeric embedded evidence exceeds its document budget.");
            bytes+=child.string.size();auto parsed=source::internal::parseAuthoringJsonCandidate(child.string);if(!parsed)continue;
            decoded.push_back(std::make_unique<Json>(std::move(*parsed)));
            pending.push_back({decoded.back().get(),where,frame.owner,frame.embedded.empty()?where:frame.embedded,frame.widgetOwner,frame.numericArrayOwner});
        }
    }
    return facts;
}
}
