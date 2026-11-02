#include <frog/bridge_json.hpp>
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <new>
#include <stdexcept>

namespace frog::bridge::json {
namespace source=frog::native_engine::source;
namespace cj=frog::native_engine::canonical_json;
using Json=source::JsonValue;
namespace {
constexpr std::size_t maximumBytes=16U*1024U*1024U;
void require(const Json &value,Json::Type type) {
    if(value.type!=type)throw std::logic_error("The JSON accessor does not match the value kind.");
}
}
struct Value::Impl {
    const std::shared_ptr<const Json> root;
    const std::shared_ptr<const std::string> bytes;
    const Json *const node;
    Impl(std::shared_ptr<const Json> owner,std::shared_ptr<const std::string> text,const Json *value):root(std::move(owner)),bytes(std::move(text)),node(value){}
};
Value::Value(std::shared_ptr<const Impl> impl):impl_(std::move(impl)){}
const Value::Impl &Value::data() const {
    if(!impl_)throw std::logic_error("A moved-from JSON handle has no value.");
    return *impl_;
}
Kind Value::kind() const {
    switch(data().node->type) {
    case Json::Type::Null:return Kind::Null;
    case Json::Type::Boolean:return Kind::Boolean;
    case Json::Type::Number:return Kind::Number;
    case Json::Type::String:return Kind::String;
    case Json::Type::Array:return Kind::Array;
    case Json::Type::Object:return Kind::Object;
    }
    throw std::logic_error("The JSON owner returned an unknown value kind.");
}
bool Value::boolean() const {const auto &v=*data().node;require(v,Json::Type::Boolean);return v.boolean;}
std::string_view Value::string() const {const auto &v=*data().node;require(v,Json::Type::String);return v.string;}
std::string_view Value::numberLexeme() const {const auto &v=*data().node;require(v,Json::Type::Number);return v.string;}
std::string_view Value::originalUtf8() const {const auto &owner=data();const auto &v=*owner.node;if(v.sourceBegin==std::string_view::npos||v.sourceEnd<v.sourceBegin||v.sourceEnd>owner.bytes->size())throw std::logic_error("The shared parser did not retain an exact token span.");return std::string_view(*owner.bytes).substr(v.sourceBegin,v.sourceEnd-v.sourceBegin);}
std::optional<Value> Value::member(std::string_view name) const {
    const auto &owner=data();require(*owner.node,Json::Type::Object);const auto *child=owner.node->member(name);
    if(!child)return {};
    return Value(std::make_shared<const Impl>(owner.root,owner.bytes,child));
}
std::vector<Value> Value::elements() const {
    const auto &owner=data();require(*owner.node,Json::Type::Array);std::vector<Value> out;out.reserve(owner.node->array.size());
    for(const auto &child:owner.node->array)out.push_back(Value(std::make_shared<const Impl>(owner.root,owner.bytes,&child)));
    return out;
}
std::vector<std::pair<std::string,Value>> Value::members() const {
    const auto &owner=data();require(*owner.node,Json::Type::Object);std::vector<std::pair<std::string,Value>> out;out.reserve(owner.node->object.size());
    for(const auto &[key,child]:owner.node->object)out.emplace_back(key,Value(std::make_shared<const Impl>(owner.root,owner.bytes,&child)));
    return out;
}
std::optional<Value> parse(std::string_view bytes,Error &error,std::size_t maxBytes) {
    error={};
    if(bytes.size()>std::min(maxBytes,maximumBytes)) {
        error={"bridge.json.byte_limit","JSON input exceeds the effective byte ceiling.",{}};return {};
    }
    try {
        std::string reason;std::size_t offset=std::string_view::npos;
        auto parsed=source::parseJson(bytes,&reason,source::JsonNumberPolicy::PreserveLexeme,offset);
        if(!parsed) {
            error={"bridge.json.refused",std::move(reason),offset==std::string_view::npos?std::string{}:"@byte:"+std::to_string(offset)};return {};
        }
        auto root=std::make_shared<const Json>(std::move(*parsed));
        auto ownedBytes=std::make_shared<const std::string>(bytes);
        return Value(std::make_shared<const Value::Impl>(root,std::move(ownedBytes),root.get()));
    } catch(const std::bad_alloc &) {
        error={"bridge.json.resource_limit","Insufficient memory for the bounded JSON value.",{}};return {};
    } catch(const std::length_error &) {
        error={"bridge.json.resource_limit","The bounded JSON value exceeds storage limits.",{}};return {};
    }
}
std::string encode(const Value &value) {return cj::encode(*value.data().node);}
std::string quote(std::string_view text) {
    // Count the existing encoder's output before allocation. JSON quoting is
    // delegated to that owner; this loop establishes only its byte budget.
    std::size_t size=2;
    for(const unsigned char c:text) {
        const std::size_t add=c<32?6:c=='"'||c=='\\'?2:1;
        if(add>maximumBytes-size)throw std::length_error("Quoted JSON exceeds the 16-MiB byte ceiling.");
        size+=add;
    }
    auto result=cj::quote(text);std::string error;
    if(!source::parseJson(result,&error,source::JsonNumberPolicy::PreserveLexeme))
        throw std::invalid_argument("Invalid UTF-8 JSON string: "+error);
    return result;
}
std::string sha256(std::string_view bytes) {return frog::native_engine::hashing::sha256Hex(bytes);}
}
