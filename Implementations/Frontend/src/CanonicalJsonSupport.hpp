#pragma once
#include "frog_native_engine/FrogJson.hpp"
#include <charconv>
#include <array>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace frog::native_engine::canonical_json {
using Json=source::JsonValue;
inline Json object(){Json v;v.type=Json::Type::Object;return v;}
inline Json array(){Json v;v.type=Json::Type::Array;return v;}
inline Json string(std::string text){Json v;v.type=Json::Type::String;v.string=std::move(text);return v;}
inline Json boolean(bool value){Json v;v.type=Json::Type::Boolean;v.boolean=value;return v;}
inline std::string quote(std::string_view text){
    static constexpr char hex[]="0123456789abcdef";std::string out="\"";
    for(const unsigned char c:text){if(c=='\"'||c=='\\'){out+='\\';out+=char(c);}
        else if(c<32){out+="\\u00";out+=hex[c>>4];out+=hex[c&15];}else out+=char(c);}
    return out+'\"';
}
inline void append(std::string &out,const Json &v){
    switch(v.type){
    case Json::Type::Null:out+="null";break;
    case Json::Type::Boolean:out+=v.boolean?"true":"false";break;
    case Json::Type::String:out+=quote(v.string);break;
    case Json::Type::Number:out+=v.string;break;
    case Json::Type::Array:{out+='[';bool first=true;for(const auto &x:v.array){if(!first)out+=',';first=false;append(out,x);}out+=']';break;}
    case Json::Type::Object:{out+='{';bool first=true;for(const auto &[key,x]:v.object){if(!first)out+=',';first=false;out+=quote(key);out+=':';append(out,x);}out+='}';break;}
    }
}
inline std::string encode(const Json &v){std::string out;append(out,v);return out;}
inline std::string pointer(std::string_view parent,std::string_view key){
    std::string out(parent);out+='/';for(char c:key){if(c=='~')out+="~0";else if(c=='/')out+="~1";else out+=c;}return out;
}
inline std::string encodedId(std::string_view prefix,std::string_view id){
    static constexpr char hex[]="0123456789abcdef";std::string out(prefix);out+=':';
    for(unsigned char c:id){out+=hex[c>>4];out+=hex[c&15];}return out;
}
inline bool exact(const Json &a,const Json &b){
    if(a.type!=b.type)return false;
    if(a.type==Json::Type::Object){if(a.object.size()!=b.object.size())return false;
        for(const auto &[key,v]:a.object){const auto *other=b.member(key);if(!other||!exact(v,*other))return false;}return true;}
    if(a.type==Json::Type::Array){if(a.array.size()!=b.array.size())return false;
        for(std::size_t i=0;i<a.array.size();++i)if(!exact(a.array[i],b.array[i]))return false;return true;}
    if(a.type==Json::Type::String||a.type==Json::Type::Number)return a.string==b.string;
    return a.type!=Json::Type::Boolean||a.boolean==b.boolean;
}
inline bool typedScalar(const Json &v,std::string_view type){
    if(type=="bool")return v.type==Json::Type::Boolean;
    if(v.type!=Json::Type::Number)return false;
    if(type=="i32"){std::int32_t n=0;const auto r=std::from_chars(v.string.data(),v.string.data()+v.string.size(),n);return r.ec==std::errc{}&&r.ptr==v.string.data()+v.string.size();}
    if(type=="u32"){std::uint32_t n=0;const auto r=std::from_chars(v.string.data(),v.string.data()+v.string.size(),n);return r.ec==std::errc{}&&r.ptr==v.string.data()+v.string.size();}
    if(type=="i64"){std::int64_t n=0;const auto r=std::from_chars(v.string.data(),v.string.data()+v.string.size(),n);return r.ec==std::errc{}&&r.ptr==v.string.data()+v.string.size();}
    if(type=="u64"){std::uint64_t n=0;const auto r=std::from_chars(v.string.data(),v.string.data()+v.string.size(),n);return r.ec==std::errc{}&&r.ptr==v.string.data()+v.string.size();}
    if(type=="f64"){
        double n=0;const auto r=std::from_chars(v.string.data(),v.string.data()+v.string.size(),n);
        if(r.ec!=std::errc{}||r.ptr!=v.string.data()+v.string.size()||!std::isfinite(n))return false;
        if(v.string.find_first_of(".eE")==std::string::npos&&v.string!="-0"){
            std::array<char,512> buffer{};const auto written=std::to_chars(buffer.data(),buffer.data()+buffer.size(),n,std::chars_format::fixed,0);
            return written.ec==std::errc{}&&std::string_view(buffer.data(),std::size_t(written.ptr-buffer.data()))==v.string;
        }
        return true;
    }
    return false;
}
}
