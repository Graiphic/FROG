#pragma once

#include "frog/sdk/runtime_contract.hpp"
#include "frog/sdk/runtime_array_value.hpp"
#include "frog/sdk/collection_order.hpp"
#include <cmath>
#include <limits>

namespace frog::sdk::runtime {

// A reference provider is a capability backed by an actual owner registry.
// The SDK never fabricates one from a class name or persists its live handle.
class RuntimeReferenceAuthority {
public:
    virtual ~RuntimeReferenceAuthority() = default;
    virtual std::string_view canonicalType() const noexcept = 0;
    virtual bool validForOwner(const RuntimeValueOwner &) const noexcept = 0;
    // Local C++ authority contract, not a public foreign ABI. Registries may
    // compare sealed target identities; addresses are never an ordering key.
    virtual bool equivalentTo(const RuntimeReferenceAuthority &other) const noexcept { return this==&other; }
};

std::optional<RuntimeValue> makeRuntimeRecord(std::vector<RuntimeNamedField>);
std::optional<RuntimeValue> makeRuntimeReference(std::shared_ptr<const RuntimeReferenceAuthority>, const RuntimeValueOwner &);

// Immutable local containers used by frog.studio.ipe@1. Existing SDK
// RuntimeValue alternatives and public artifact schema numbers are unchanged.
class RuntimeIpeValue;
inline collection_order::Error runtimeCollectionComparableType(const ValueType &,collection_order::Limits) noexcept;
inline collection_order::Result compareRuntimeCollectionValues(const RuntimeValue &,const RuntimeValue &,const ValueType &,const RuntimeValueOwner &,collection_order::Limits,collection_order::Cancelled,void *) noexcept;
inline collection_order::NormalizationResult normalizeRuntimeCollectionValues(std::span<const RuntimeValue>,const ValueType &,const RuntimeValueOwner &,collection_order::DuplicatePolicy,collection_order::Limits,collection_order::Cancelled,void *) noexcept;
namespace compound_detail {
inline bool validateIpe(const RuntimeIpeValue &,const RuntimeValueOwner *,unsigned,std::size_t &,std::size_t &) noexcept;
}
class RuntimeIpeValue final : public CompoundRuntimeValue {
public:
    enum class Kind {Array,Variant,Map,Timestamp,Set};
    RuntimeIpeValue(Kind kind,std::string type,RuntimeValueOwner owner,ValueType element,
        ValueType key,std::vector<std::uint32_t> shape,std::vector<RuntimeValue> values,
        std::vector<RuntimeNamedField> attributes,std::vector<std::pair<RuntimeValue,RuntimeValue>> entries,
        std::int64_t seconds=0,std::uint64_t fraction=0,std::string payloadSchemaJson={})
        :kind_(kind),type_(std::move(type)),owner_(std::move(owner)),element_(std::move(element)),key_(std::move(key)),
        shape_(std::move(shape)),values_(std::move(values)),attributes_(std::move(attributes)),entries_(std::move(entries)),
        seconds_(seconds),fraction_(fraction),payloadSchemaJson_(std::move(payloadSchemaJson)){}
    Kind kind()const noexcept{return kind_;}
    std::string_view canonicalType()const noexcept override{return type_;}
    bool isReference()const noexcept override{return false;}
    bool valid()const noexcept override{std::size_t n=0,b=0;return compound_detail::validateIpe(*this,nullptr,0,n,b);}
    bool validForOwner(const RuntimeValueOwner &owner)const noexcept override{std::size_t n=0,b=0;return compound_detail::validateIpe(*this,&owner,0,n,b);}
    std::span<const RuntimeNamedField> fields()const noexcept override{return attributes_;}
    const RuntimeValueOwner &owner()const noexcept{return owner_;}
    const ValueType &elementType()const noexcept{return element_;}
    const ValueType &payloadType()const noexcept{return element_;}
    const ValueType &valueType()const noexcept{return element_;}
    const ValueType &keyType()const noexcept{return key_;}
    const std::string &payloadSchemaJson()const noexcept{return payloadSchemaJson_;}
    std::span<const std::uint32_t> shape()const noexcept{return shape_;}
    std::span<const RuntimeValue> elements()const noexcept{return values_;}
    const RuntimeValue &payload()const{return values_.at(0);}
    std::span<const RuntimeNamedField> attributes()const noexcept{return attributes_;}
    std::span<const std::pair<RuntimeValue,RuntimeValue>> entries()const noexcept{return entries_;}
    std::int64_t seconds()const noexcept{return seconds_;}
    std::uint64_t fraction()const noexcept{return fraction_;}
private:
    const Kind kind_;const std::string type_;const RuntimeValueOwner owner_;
    const ValueType element_,key_;const std::vector<std::uint32_t> shape_;
    const std::vector<RuntimeValue> values_;const std::vector<RuntimeNamedField> attributes_;
    const std::vector<std::pair<RuntimeValue,RuntimeValue>> entries_;
    const std::int64_t seconds_;const std::uint64_t fraction_;const std::string payloadSchemaJson_;
};
inline const RuntimeIpeValue *runtimeIpeValue(const RuntimeValue &value)noexcept {
    const auto *p=std::get_if<std::shared_ptr<const CompoundRuntimeValue>>(&value);
    return p&&*p?dynamic_cast<const RuntimeIpeValue *>(p->get()):nullptr;
}
inline const RuntimeIpeValue *runtimeArray(const RuntimeValue &v)noexcept{const auto *p=runtimeIpeValue(v);return p&&p->kind()==RuntimeIpeValue::Kind::Array?p:nullptr;}
inline const RuntimeIpeValue *runtimeVariant(const RuntimeValue &v)noexcept{const auto *p=runtimeIpeValue(v);return p&&p->kind()==RuntimeIpeValue::Kind::Variant?p:nullptr;}
inline const RuntimeIpeValue *runtimeMap(const RuntimeValue &v)noexcept{const auto *p=runtimeIpeValue(v);return p&&p->kind()==RuntimeIpeValue::Kind::Map?p:nullptr;}
inline const RuntimeIpeValue *runtimeSet(const RuntimeValue &v)noexcept{const auto *p=runtimeIpeValue(v);return p&&p->kind()==RuntimeIpeValue::Kind::Set?p:nullptr;}
inline const RuntimeIpeValue *runtimeTimestamp(const RuntimeValue &v)noexcept{const auto *p=runtimeIpeValue(v);return p&&p->kind()==RuntimeIpeValue::Kind::Timestamp?p:nullptr;}

namespace compound_detail {
class ImmutableRecord;
class BorrowedReference;
inline bool validate(const RuntimeValue &, const ValueType &, const RuntimeValueOwner *,
    unsigned, std::size_t &, std::size_t &) noexcept;

class ImmutableRecord final : public CompoundRuntimeValue {
private:
    friend std::optional<RuntimeValue> frog::sdk::runtime::makeRuntimeRecord(std::vector<RuntimeNamedField>);
    ImmutableRecord(std::string type, std::vector<RuntimeNamedField> fields)
        : type_(std::move(type)), fields_(std::move(fields)) {}
public:
    std::string_view canonicalType() const noexcept override { return type_; }
    bool isReference() const noexcept override { return false; }
    bool valid() const noexcept override {
        std::size_t nodes = 0, bytes = 0;
        for (const auto &field : fields_)
            if (!validate(field.value, field.type, nullptr, 1, nodes, bytes)) return false;
        return nodes < 4096;
    }
    bool validForOwner(const RuntimeValueOwner &owner) const noexcept override {
        if (!runtimeValueOwnerValid(owner)) return false;
        std::size_t nodes = 0, bytes = 0;
        for (const auto &field : fields_)
            if (!validate(field.value, field.type, &owner, 1, nodes, bytes)) return false;
        return nodes < 4096;
    }
    std::span<const RuntimeNamedField> fields() const noexcept override { return fields_; }
private:
    const std::string type_;
    const std::vector<RuntimeNamedField> fields_;
};

class BorrowedReference final : public CompoundRuntimeValue {
private:
    friend std::optional<RuntimeValue> frog::sdk::runtime::makeRuntimeReference(std::shared_ptr<const RuntimeReferenceAuthority>, const RuntimeValueOwner &);
    BorrowedReference(std::string type, std::shared_ptr<const RuntimeReferenceAuthority> authority,
        RuntimeValueOwner owner) : type_(std::move(type)), authority_(std::move(authority)), owner_(std::move(owner)) {}
public:
    std::string_view canonicalType() const noexcept override { return type_; }
    bool isReference() const noexcept override { return true; }
    bool valid() const noexcept override { return validForOwner(owner_); }
    bool validForOwner(const RuntimeValueOwner &owner) const noexcept override {
        return owner == owner_ && authority_ && authority_->canonicalType() == type_ &&
            authority_->validForOwner(owner);
    }
    std::span<const RuntimeNamedField> fields() const noexcept override { return {}; }
    const std::shared_ptr<const RuntimeReferenceAuthority> &authority()const noexcept{return authority_;}
private:
    const std::string type_;
    const std::shared_ptr<const RuntimeReferenceAuthority> authority_;
    const RuntimeValueOwner owner_;
};

inline bool validate(const RuntimeValue &value, const ValueType &type, const RuntimeValueOwner *owner,
    unsigned depth, std::size_t &nodes, std::size_t &bytes) noexcept {
    if (depth > 32 || ++nodes > 4096 || std::holds_alternative<std::monostate>(value)) return false;
    try {
        if (type != valueTypeFromCanonicalName(type.canonicalName) || !type.known()) return false;
    } catch (...) { return false; }
    if (const auto *array = std::get_if<RuntimeF64Array>(&value)) {
        if(depth != 0 || !*array || !array_detail::descriptor(type) ||
            !(owner ? (*array)->validForOwner(*owner) : (*array)->validForOwner((*array)->owner()))) return false;
        const auto payload=(*array)->payloadBytes();
        if(payload > kMaximumF64ArrayPayloadBytes-bytes)return false;
        bytes+=payload;return true; // packed terminal, not recursive RuntimeValues
    }
    if (const auto *carrier = std::get_if<std::shared_ptr<const CompoundRuntimeValue>>(&value)) {
        if (!*carrier || (*carrier)->canonicalType() != type.canonicalName) return false;
        if (const auto *record = dynamic_cast<const ImmutableRecord *>(carrier->get())) {
            if (type.category != ValueCategory::Cluster) return false;
            for (const auto &field : record->fields())
                if (!validate(field.value, field.type, owner, depth + 1, nodes, bytes)) return false;
            return true;
        }
        if(const auto *local=dynamic_cast<const RuntimeIpeValue *>(carrier->get()))
            return validateIpe(*local,owner,depth,nodes,bytes);
        const auto *reference = dynamic_cast<const BorrowedReference *>(carrier->get());
        return reference && type.category == ValueCategory::ObjectReference &&
            (owner ? reference->validForOwner(*owner) : reference->valid());
    }
    if (!runtimeValueMatches(value, type)) return false;
    if (const auto *number = std::get_if<std::int64_t>(&value)) {
        if (type.bitWidth == 64) return true;
        if (type.bitWidth != 8 && type.bitWidth != 16 && type.bitWidth != 32) return false;
        const auto limit = std::int64_t{1} << (type.bitWidth - 1);
        return *number >= -limit && *number < limit;
    }
    if (const auto *number = std::get_if<std::uint64_t>(&value)) {
        if (type.bitWidth == 64) return true;
        return (type.bitWidth == 8 || type.bitWidth == 16 || type.bitWidth == 32) &&
            *number < (std::uint64_t{1} << type.bitWidth);
    }
    if (const auto *number = std::get_if<double>(&value))
        return std::isfinite(*number) && (type.bitWidth == 64 ||
            (type.bitWidth == 32 && static_cast<double>(static_cast<float>(*number)) == *number));
    if (const auto *text = std::get_if<std::string>(&value)) {
        if (text->size() > 1'000'000 || text->size() > 16'000'000 - bytes) return false;
        bytes += text->size();
    }
    return true;
}
inline bool validateIpe(const RuntimeIpeValue &v,const RuntimeValueOwner *owner,unsigned depth,
    std::size_t &nodes,std::size_t &bytes)noexcept {
    try {
        if(depth>32||nodes>4096||bytes>16'000'000||v.payloadSchemaJson().size()>65536)return false;
        bytes+=v.payloadSchemaJson().size();if(bytes>16'000'000)return false;
        const auto type=valueTypeFromCanonicalName(v.canonicalType());
        if(!type.known())return false;
        if(v.kind()!=RuntimeIpeValue::Kind::Timestamp &&
            (!runtimeValueOwnerValid(v.owner())||(owner&&v.owner()!=*owner)))return false;
        const auto child=[&](const RuntimeValue &value,const ValueType &t){return validate(value,t,owner?owner:&v.owner(),depth+1,nodes,bytes);};
        if(v.kind()==RuntimeIpeValue::Kind::Timestamp)return type.canonicalName=="timestamp"&&v.elements().empty()&&v.attributes().empty()&&v.entries().empty();
        if(v.kind()==RuntimeIpeValue::Kind::Array){
            if(v.shape().empty()||v.shape().size()>16||type.category!=ValueCategory::Array||
                type.rank!=v.shape().size()||type.elementType!=v.elementType().canonicalName||!v.attributes().empty()||!v.entries().empty())return false;
            const auto count=array_detail::elementCount(v.shape());
            if(!count||*count!=v.elements().size())return false;
            // The recursive bound applies to each element's value tree, not
            // to the number of independently typed Array elements.
            for(const auto &value:v.elements()) {
                std::size_t elementNodes=0,elementBytes=0;
                if(!validate(value,v.elementType(),owner?owner:&v.owner(),depth+1,elementNodes,elementBytes))return false;
            }
            return true;
        }
        if(v.kind()==RuntimeIpeValue::Kind::Variant){
            if(type.canonicalName!="variant"||v.elements().size()!=1||!v.shape().empty()||!v.entries().empty()||!child(v.payload(),v.payloadType()))return false;
            std::unordered_set<std::string> names;
            for(const auto &a:v.attributes())if(!structural_detail::identity(a.id)||!names.insert(a.id).second||a.type.canonicalName!="variant"||!child(a.value,a.type))return false;
            return true;
        }
        if(v.kind()==RuntimeIpeValue::Kind::Set){
            if(type.category!=ValueCategory::Set||type.elementType!=v.elementType().canonicalName||!v.shape().empty()||!v.entries().empty()||!v.attributes().empty()||v.elements().size()>4096||runtimeCollectionComparableType(v.elementType(),{})!=collection_order::Error::None)return false;
            for(std::size_t i=0;i<v.elements().size();++i){if(!child(v.elements()[i],v.elementType()))return false;if(i){const auto c=compareRuntimeCollectionValues(v.elements()[i-1],v.elements()[i],v.elementType(),owner?*owner:v.owner(),{},nullptr,nullptr);if(!c.accepted()||c.order!=collection_order::Order::Less)return false;}}
            return true;
        }
        if(v.kind()!=RuntimeIpeValue::Kind::Map||type.category!=ValueCategory::Map||!v.elements().empty()||!v.shape().empty()||!v.attributes().empty()||
            v.canonicalType()!="map<"+v.keyType().canonicalName+","+v.valueType().canonicalName+">"||v.entries().size()>4096)return false;
        if(runtimeCollectionComparableType(v.keyType(),{})!=collection_order::Error::None)return false;
        for(std::size_t i=0;i<v.entries().size();++i){const auto &entry=v.entries()[i];if(!child(entry.first,v.keyType())||!child(entry.second,v.valueType()))return false;
            if(i){const auto c=compareRuntimeCollectionValues(v.entries()[i-1].first,entry.first,v.keyType(),owner?*owner:v.owner(),{},nullptr,nullptr);if(!c.accepted()||c.order!=collection_order::Order::Less)return false;}
        }
        return true;
    }catch(...){return false;}
}
} // namespace compound_detail

[[nodiscard]] inline bool runtimeValueValidForOwner(const RuntimeValue &value, const ValueType &type,
    const RuntimeValueOwner &owner) noexcept {
    if (!runtimeValueOwnerValid(owner)) return false;
    std::size_t nodes = 0, bytes = 0;
    return compound_detail::validate(value, type, &owner, 0, nodes, bytes);
}
[[nodiscard]] inline bool runtimeValueRepresentable(const RuntimeValue &value, const ValueType &type) noexcept {
    std::size_t nodes = 0, bytes = 0;
    return compound_detail::validate(value, type, nullptr, 0, nodes, bytes);
}

[[nodiscard]] inline std::optional<RuntimeValue> makeRuntimeRecord(std::vector<RuntimeNamedField> fields) {
    if (fields.size() > 64) return {};
    std::unordered_set<std::string> ids;
    std::string type = "cluster<";
    std::size_t nodes = 1, bytes = 0;
    for (const auto &field : fields) {
        if (!structural_detail::identity(field.id) || !ids.insert(field.id).second ||
            !compound_detail::validate(field.value, field.type, nullptr, 1, nodes, bytes)) return {};
        if (type.size() > 8) type += ',';
        type += structural_detail::quoted(field.id) + ":" + field.type.canonicalName;
        if (type.size() >= 65536) return {};
    }
    type += '>';
    // Only these sealed immutable carriers are admitted by the recursive
    // validator. Caller-defined mutable CompoundRuntimeValue implementations
    // cannot enter a body by returning true from a virtual valid() method.
    return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{new compound_detail::ImmutableRecord(std::move(type), std::move(fields))}};
}

[[nodiscard]] inline std::optional<RuntimeValue> makeRuntimeReference(
    std::shared_ptr<const RuntimeReferenceAuthority> authority, const RuntimeValueOwner &owner) {
    if (!authority || !runtimeValueOwnerValid(owner) || !authority->validForOwner(owner)) return {};
    const auto type = authority->canonicalType();
    if (!structural_detail::TypeSyntax(type).reference()) return {};
    return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{new compound_detail::BorrowedReference(std::string(type), std::move(authority), owner)}};
}

inline std::optional<RuntimeValue> sealRuntimeIpeValue(std::shared_ptr<const RuntimeIpeValue> value) {
    if(!value||!value->valid())return {};return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{std::move(value)}};
}
inline std::optional<RuntimeValue> makeRuntimeArray(ValueType element,std::vector<std::uint32_t> shape,
    std::vector<RuntimeValue> values,const RuntimeValueOwner &owner) {
    const auto type="array<"+element.canonicalName+(shape.size()==1?std::string{}:",rank="+std::to_string(shape.size()))+">";
    return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Array,type,owner,std::move(element),ValueType{},
        std::move(shape),std::move(values),std::vector<RuntimeNamedField>{},std::vector<std::pair<RuntimeValue,RuntimeValue>>{}));
}
inline std::optional<RuntimeValue> makeRuntimeVariant(ValueType payloadType,RuntimeValue payload,
    std::vector<RuntimeNamedField> attributes,const RuntimeValueOwner &owner,std::string payloadSchemaJson={}) {
    return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Variant,"variant",owner,std::move(payloadType),ValueType{},
        std::vector<std::uint32_t>{},std::vector<RuntimeValue>{std::move(payload)},std::move(attributes),std::vector<std::pair<RuntimeValue,RuntimeValue>>{},0,0,std::move(payloadSchemaJson)));
}
inline std::optional<RuntimeValue> makeRuntimeMap(ValueType key,ValueType value,
    std::vector<std::pair<RuntimeValue,RuntimeValue>> entries,const RuntimeValueOwner &owner) {
    std::vector<RuntimeValue> keys;keys.reserve(entries.size());for(const auto &entry:entries)keys.push_back(entry.first);
    const auto normalized=normalizeRuntimeCollectionValues(keys,key,owner,collection_order::DuplicatePolicy::KeepLast,{},nullptr,nullptr);
    if(!normalized.accepted())return {};
    std::vector<std::pair<RuntimeValue,RuntimeValue>> sorted;sorted.reserve(normalized.indices.size());for(const auto i:normalized.indices)sorted.push_back(std::move(entries[i]));
    const auto type="map<"+key.canonicalName+","+value.canonicalName+">";
    return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Map,type,owner,std::move(value),std::move(key),
        std::vector<std::uint32_t>{},std::vector<RuntimeValue>{},std::vector<RuntimeNamedField>{},std::move(sorted)));
}
inline std::optional<RuntimeValue> makeRuntimeSet(ValueType element,std::vector<RuntimeValue> values,const RuntimeValueOwner &owner) {
    const auto normalized=normalizeRuntimeCollectionValues(values,element,owner,collection_order::DuplicatePolicy::KeepFirst,{},nullptr,nullptr);
    if(!normalized.accepted())return {};
    std::vector<RuntimeValue> sorted;sorted.reserve(normalized.indices.size());for(const auto i:normalized.indices)sorted.push_back(std::move(values[i]));
    const auto type="set<"+element.canonicalName+">";
    return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Set,type,owner,std::move(element),ValueType{},std::vector<std::uint32_t>{},std::move(sorted),std::vector<RuntimeNamedField>{},std::vector<std::pair<RuntimeValue,RuntimeValue>>{}));
}
inline std::optional<RuntimeValue> makeRuntimeTimestamp(std::int64_t seconds,std::uint64_t fraction) {
    return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Timestamp,"timestamp",RuntimeValueOwner{},ValueType{},ValueType{},
        std::vector<std::uint32_t>{},std::vector<RuntimeValue>{},std::vector<RuntimeNamedField>{},std::vector<std::pair<RuntimeValue,RuntimeValue>>{},seconds,fraction));
}
inline std::shared_ptr<const RuntimeReferenceAuthority> runtimeReferenceAuthority(const RuntimeValue &value)noexcept {
    const auto *p=std::get_if<std::shared_ptr<const CompoundRuntimeValue>>(&value);
    const auto *ref=p&&*p?dynamic_cast<const compound_detail::BorrowedReference *>(p->get()):nullptr;
    return ref?ref->authority():nullptr;
}

// Content equality is explicit. The scalar variant keeps its legacy identity
// semantics; immutable records round-trip with independent allocations.
[[nodiscard]] inline bool runtimeValuesEqual(const RuntimeValue &left, const RuntimeValue &right,
    unsigned depth = 0) noexcept {
    if (depth > 32 || left.index() != right.index()) return false;
    if (left.index() == 7) {
        const auto &a=std::get<7>(left), &b=std::get<7>(right);
        return depth == 0 && a && b && a->validForOwner(a->owner()) && b->validForOwner(b->owner()) &&
            a->shape() == b->shape() && std::equal(a->elements().begin(),a->elements().end(),b->elements().begin());
    }
    if (left.index() != 6) return left == right;
    const auto &a = std::get<6>(left), &b = std::get<6>(right);
    if (!a || !b || a->canonicalType() != b->canonicalType() || !a->valid() || !b->valid()) return false;
    if (a->isReference() || b->isReference()) return a == b;
    const auto *ia=dynamic_cast<const RuntimeIpeValue *>(a.get()),*ib=dynamic_cast<const RuntimeIpeValue *>(b.get());
    if(ia||ib){
        if(!ia||!ib||ia->kind()!=ib->kind()||ia->elementType()!=ib->elementType()||ia->keyType()!=ib->keyType()||
            !std::equal(ia->shape().begin(),ia->shape().end(),ib->shape().begin(),ib->shape().end())||
            ia->elements().size()!=ib->elements().size()||ia->entries().size()!=ib->entries().size()||
            ia->seconds()!=ib->seconds()||ia->fraction()!=ib->fraction()||ia->payloadSchemaJson()!=ib->payloadSchemaJson())return false;
        for(std::size_t i=0;i<ia->elements().size();++i)if(!runtimeValuesEqual(ia->elements()[i],ib->elements()[i],depth+1))return false;
        for(std::size_t i=0;i<ia->entries().size();++i)if(!runtimeValuesEqual(ia->entries()[i].first,ib->entries()[i].first,depth+1)||!runtimeValuesEqual(ia->entries()[i].second,ib->entries()[i].second,depth+1))return false;
    }
    const auto af = a->fields(), bf = b->fields();
    if (af.size() != bf.size()) return false;
    for (std::size_t index = 0; index < af.size(); ++index)
        if (af[index].id != bf[index].id || af[index].type != bf[index].type ||
            !runtimeValuesEqual(af[index].value, bf[index].value, depth + 1)) return false;
    return true;
}

} // namespace frog::sdk::runtime
#include "frog/sdk/runtime_collection_order.hpp"
