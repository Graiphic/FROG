#pragma once
#include "frog/sdk/event_runtime_contract.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <set>
#include <span>

namespace frog::sdk::events {
// In-process typed transport. It is deliberately separate from the scalar
// ProgramArtifact v1 codec and cannot make an unimplemented backend runnable.
// There is no persistent pointer/token encoding or NI binary ABI here.
struct SourceOwner {
    std::string id, sourceDigest;
    std::uint64_t epoch=0, sourceRevision=0, artifactRevision=0;
    bool operator==(const SourceOwner &) const noexcept=default;
};
enum class PacketError { None, InvalidOwner, InvalidIdentity, InvalidType,
    InvalidValue, DuplicateField, Limit, ExpiredReference, WrongOwner };
class BorrowedObjectReference {
public:
    virtual ~BorrowedObjectReference()=default;
    virtual std::string_view typeName() const noexcept=0;
    // The host must reuse its actual reference registry and validate domain,
    // generation, class and strict value type. A name alone is not a reference.
    virtual PacketError validate(const SourceOwner &) const noexcept=0;
};
namespace packet_detail {
inline bool id(std::string_view s) noexcept {
    return !s.empty() && s.size()<=1024 && std::none_of(s.begin(),s.end(),[](unsigned char ch){return ch<32;});
}
inline bool field(std::string_view s) noexcept {
    return id(s);
}
inline std::string quoted(std::string_view s) {
    std::string result="\"";
    for(const char ch:s){if(ch=='\\' || ch=='"')result+='\\';result+=ch;}
    return result+'"';
}
// Canonical structural spelling, not a source-class catalogue or a substitute
// for the real provider's domain/class/value-schema validation.
using TypeSyntax = runtime::structural_detail::TypeSyntax;
inline bool owner(const SourceOwner &s) noexcept {
    return id(s.id) && s.epoch && s.sourceDigest.size()==64 &&
        std::all_of(s.sourceDigest.begin(),s.sourceDigest.end(),[](char ch) {
            return (ch>='0' && ch<='9') || (ch>='a' && ch<='f') || (ch>='A' && ch<='F');
        });
}
inline bool scalar(const runtime::RuntimeValue &v,const runtime::ValueType &t) noexcept {
    if(t.category==runtime::ValueCategory::Cluster || t.category==runtime::ValueCategory::ObjectReference)return false;
    if(!t.known() || std::holds_alternative<std::monostate>(v) || !runtime::runtimeValueMatches(v,t))return false;
    if(const auto *f=std::get_if<double>(&v))return std::isfinite(*f) &&
        (t.bitWidth==64 || (t.bitWidth==32 && static_cast<double>(static_cast<float>(*f))==*f));
    if(const auto *n=std::get_if<std::int64_t>(&v);n && t.bitWidth<64) {
        const auto limit=std::int64_t{1}<<(t.bitWidth-1);return *n>=-limit && *n<limit;
    }
    if(const auto *n=std::get_if<std::uint64_t>(&v);n && t.bitWidth<64)return *n<(std::uint64_t{1}<<t.bitWidth);
    if(const auto *s=std::get_if<std::string>(&v))return s->size()<=1'000'000;
    return true;
}
inline void error(PacketError *out,PacketError value) noexcept {if(out)*out=value;}
}
class FieldValue final {
public:
    enum class Kind { Scalar, Record, Reference };
    using Member=std::pair<std::string,FieldValue>;
    static std::optional<FieldValue> scalar(std::string type,runtime::RuntimeValue value,PacketError *error=nullptr) {
        if(!packet_detail::scalar(value,runtime::valueTypeFromCanonicalName(type))) {
            packet_detail::error(error,PacketError::InvalidValue);return {};
        }
        FieldValue result(Kind::Scalar,std::move(type));result.scalar_=std::move(value);
        if(const auto *s=std::get_if<std::string>(&result.scalar_))result.textBytes_=s->size();
        packet_detail::error(error,PacketError::None);return result;
    }
    static std::optional<FieldValue> record(std::vector<Member> fields,PacketError *error=nullptr) {
        if(fields.empty() || fields.size()>64){packet_detail::error(error,PacketError::Limit);return {};}
        FieldValue result(Kind::Record,"cluster<");std::set<std::string> ids;
        for(const auto &[id,value]:fields) {
            if(!packet_detail::field(id) || !ids.insert(id).second){packet_detail::error(error,PacketError::DuplicateField);return {};}
            const auto name=packet_detail::quoted(id);
            if(value.depth_>=32 || value.nodes_>4096-result.nodes_ || value.textBytes_>16'000'000-result.textBytes_ ||
                value.type_.size()+name.size()+3>65536-result.type_.size()) {
                packet_detail::error(error,PacketError::Limit);return {};
            }
            if(result.type_.size()>8)result.type_+=',';
            result.type_+=name+":"+value.type_;result.nodes_+=value.nodes_;result.textBytes_+=value.textBytes_;
            result.depth_=(std::max)(result.depth_,value.depth_+1);
        }
        result.type_+='>';result.fields_=std::move(fields);
        packet_detail::error(error,PacketError::None);return result;
    }
    static std::optional<FieldValue> reference(std::shared_ptr<const BorrowedObjectReference> reference,
        const SourceOwner &owner,PacketError *error=nullptr) {
        if(!reference || !packet_detail::owner(owner)) {packet_detail::error(error,PacketError::InvalidOwner);return {};}
        const auto status=reference->validate(owner);
        if(status!=PacketError::None){packet_detail::error(error,status);return {};}
        const auto type=reference->typeName();
        if(!packet_detail::TypeSyntax(type).reference()) {
            packet_detail::error(error,PacketError::InvalidType);return {};
        }
        FieldValue result(Kind::Reference,std::string(type));result.reference_=std::move(reference);
        packet_detail::error(error,PacketError::None);return result;
    }
    Kind kind() const noexcept{return kind_;}
    std::string_view typeName() const noexcept{return type_;}
    const runtime::RuntimeValue *scalarValue() const noexcept{return kind_==Kind::Scalar?&scalar_:nullptr;}
    const BorrowedObjectReference *referenceValue() const noexcept{return reference_.get();}
    std::span<const Member> members() const noexcept{return fields_;}
    const FieldValue *member(std::string_view id) const noexcept {
        for(const auto &[name,value]:fields_)if(name==id)return &value;return nullptr;
    }
    PacketError validateReferences(const SourceOwner &owner) const noexcept {
        if(reference_)return reference_->typeName()==type_?reference_->validate(owner):PacketError::InvalidType;
        for(const auto &[_,value]:fields_)if(const auto e=value.validateReferences(owner);e!=PacketError::None)return e;
        return PacketError::None;
    }
private:
    friend class TypedEventPacket;
    FieldValue(Kind kind,std::string type):kind_(kind),type_(std::move(type)){}
    Kind kind_;
    std::string type_;
    runtime::RuntimeValue scalar_;
    std::vector<Member> fields_;
    std::shared_ptr<const BorrowedObjectReference> reference_;
    std::size_t nodes_=1,textBytes_=0,depth_=1;
};
class TypedEventPacket final {
public:
    static std::optional<TypedEventPacket> capture(SourceOwner owner,runtime::EventIdentity identity,
        std::uint64_t timeMilliseconds,std::uint64_t sequence,std::vector<FieldValue::Member> fields,
        PacketError *error=nullptr) {
        if(!packet_detail::owner(owner)){packet_detail::error(error,PacketError::InvalidOwner);return {};}
        if(!sequence || !packet_detail::id(identity.kind) || identity.kind=="timeout" ||
            (identity.scope=="application"?!identity.source.empty():
                ((identity.scope!="control" && identity.scope!="pane" && identity.scope!="vi" && identity.scope!="dynamic") ||
                !packet_detail::id(identity.source)))) {
            packet_detail::error(error,PacketError::InvalidIdentity);return {};
        }
        if(fields.size()>256){packet_detail::error(error,PacketError::Limit);return {};}
        std::size_t nodes=0,bytes=0;std::set<std::string> ids;
        for(const auto &[name,value]:fields) {
            if(!packet_detail::field(name) || name=="source" || name=="type" || name=="time" || !ids.insert(name).second) {
                packet_detail::error(error,PacketError::DuplicateField);return {};
            }
            if(value.nodes_>16384-nodes || value.textBytes_>16'000'000-bytes) {
                packet_detail::error(error,PacketError::Limit);return {};
            }
            if(const auto status=value.validateReferences(owner);status!=PacketError::None) {
                packet_detail::error(error,status);return {};
            }
            nodes+=value.nodes_;bytes+=value.textBytes_;
        }
        packet_detail::error(error,PacketError::None);
        return TypedEventPacket(std::move(owner),std::move(identity),timeMilliseconds,sequence,std::move(fields));
    }
    const SourceOwner &owner() const noexcept{return owner_;}
    const runtime::EventIdentity &identity() const noexcept{return identity_;}
    std::uint64_t timeMilliseconds() const noexcept{return time_;}
    std::uint64_t sequence() const noexcept{return sequence_;}
    std::span<const FieldValue::Member> fields() const noexcept{return fields_;}
    const FieldValue *field(std::string_view id) const noexcept {
        for(const auto &[name,value]:fields_)if(name==id)return &value;return nullptr;
    }
    PacketError validateFor(const SourceOwner &owner) const noexcept {
        if(owner!=owner_)return PacketError::WrongOwner;
        for(const auto &[_,value]:fields_)if(const auto status=value.validateReferences(owner);status!=PacketError::None)return status;
        return PacketError::None;
    }
    // The compiler supplies this schema from the admitted authoritative Source
    // region. Neither UI labels nor the packet's values resolve its wire types.
    PacketError validateSchema(std::span<const std::pair<std::string,std::string>> schema) const noexcept {
        if(schema.size()!=fields_.size())return PacketError::InvalidType;
        for(std::size_t i=0;i<schema.size();++i) {
            const auto &[name,type]=schema[i];const auto *value=field(name);
            if(!value || value->typeName()!=type)return PacketError::InvalidType;
            for(std::size_t j=0;j<i;++j)if(schema[j].first==name)return PacketError::DuplicateField;
        }
        return PacketError::None;
    }
private:
    TypedEventPacket(SourceOwner owner,runtime::EventIdentity identity,std::uint64_t time,std::uint64_t sequence,
        std::vector<FieldValue::Member> fields):owner_(std::move(owner)),identity_(std::move(identity)),time_(time),sequence_(sequence),fields_(std::move(fields)){}
    SourceOwner owner_;
    runtime::EventIdentity identity_;
    std::uint64_t time_,sequence_;
    std::vector<FieldValue::Member> fields_;
};
} // namespace frog::sdk::events
