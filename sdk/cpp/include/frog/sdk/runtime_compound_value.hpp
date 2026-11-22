#pragma once

#include "frog/sdk/runtime_contract.hpp"
#include "frog/sdk/runtime_value_metrics.hpp"
#include "frog/sdk/runtime_array_value.hpp"
#include "frog/sdk/runtime_complex_value.hpp"
#include "frog/sdk/collection_order.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <limits>
#include <mutex>
#include <atomic>
#include <type_traits>

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
std::optional<RuntimeValue> makeRuntimeNullReference(std::string, const RuntimeValueOwner &);
std::optional<RuntimeValue> makeRuntimeWidgetReferenceSymbol(std::string, std::string, const RuntimeValueOwner &);

// Immutable local containers used by frog.studio.ipe@1. Existing SDK
// RuntimeValue alternatives and public artifact schema numbers are unchanged.
class RuntimeIpeValue;
class RuntimePrimitiveArrayPool;
class RuntimePrimitiveArrayEdit;
std::optional<RuntimeValue> makeRuntimeArray(ValueType,std::vector<std::uint32_t>,
    std::vector<RuntimeValue>,const RuntimeValueOwner &);
// Optional private-host reservation for an already inventoried sealed prefix.
// The checked factory calculates the actual shared/detached allocation under
// its storage lock, before allocating or extending any tail.
struct PrimitiveArrayAppendReservation {
    std::size_t availableAdditionalBytes=0,retainedStorageBytes=0;
};
// Append preserves sealed prefixes. It never writes an element visible from
// an earlier value; a divergent branch detaches before extending its prefix.
std::optional<RuntimeValue> appendRuntimePrimitiveArray(const RuntimeIpeValue &,
    std::span<const RuntimeValue>, const RuntimeValueOwner &, std::size_t, const PrimitiveArrayAppendReservation * = nullptr);
inline collection_order::Error runtimeCollectionComparableType(const ValueType &,collection_order::Limits) noexcept;
inline collection_order::Result compareRuntimeCollectionValues(const RuntimeValue &,const RuntimeValue &,const ValueType &,const RuntimeValueOwner &,collection_order::Limits,collection_order::Cancelled,void *) noexcept;
inline collection_order::NormalizationResult normalizeRuntimeCollectionValues(std::span<const RuntimeValue>,const ValueType &,const RuntimeValueOwner &,collection_order::DuplicatePolicy,collection_order::Limits,collection_order::Cancelled,void *) noexcept;
namespace compound_detail {
inline bool validateIpe(const RuntimeIpeValue &,const RuntimeValueOwner *,unsigned,std::size_t &,std::size_t &) noexcept;

enum class PackedNumericKind { None, I8, U8, I16, U16, I32, U32, I64, U64, F32, F64 };

inline PackedNumericKind packedNumericKind(std::string_view name) noexcept {
    using K=PackedNumericKind;
    if(name=="i8")return K::I8;
    if(name=="u8")return K::U8;
    if(name=="i16")return K::I16;
    if(name=="u16")return K::U16;
    if(name=="i32")return K::I32;
    if(name=="u32")return K::U32;
    if(name=="i64")return K::I64;
    if(name=="u64")return K::U64;
    if(name=="f32")return K::F32;
    if(name=="f64")return K::F64;
    return K::None;
}

template<class Function>
decltype(auto) visitPackedNumericKind(PackedNumericKind kind,Function &&function) {
    using K=PackedNumericKind;
    switch(kind) {
    case K::I8: return function.template operator()<std::int8_t>();
    case K::U8: return function.template operator()<std::uint8_t>();
    case K::I16: return function.template operator()<std::int16_t>();
    case K::U16: return function.template operator()<std::uint16_t>();
    case K::I32: return function.template operator()<std::int32_t>();
    case K::U32: return function.template operator()<std::uint32_t>();
    case K::I64: return function.template operator()<std::int64_t>();
    case K::U64: return function.template operator()<std::uint64_t>();
    case K::F32: return function.template operator()<float>();
    case K::F64: return function.template operator()<double>();
    default: return function.template operator()<void>();
    }
}

inline std::size_t packedNumericWidth(PackedNumericKind kind) noexcept {
    return visitPackedNumericKind(kind,[]<class T>() -> std::size_t {
        if constexpr(std::is_void_v<T>)return 0;
        else return sizeof(T);
    });
}

template<class T>
using NumericScalarCarrier=std::conditional_t<std::is_floating_point_v<T>,double,
    std::conditional_t<std::is_signed_v<T>,std::int64_t,std::uint64_t>>;

template<class T>
using PackedNumericWord=std::conditional_t<sizeof(T)==1,std::uint8_t,
    std::conditional_t<sizeof(T)==2,std::uint16_t,
    std::conditional_t<sizeof(T)==4,std::uint32_t,std::uint64_t>>>;

inline std::string arrayTypeName(const ValueType &element,std::size_t rank) {
    return "array<"+element.canonicalName+(rank==1?std::string{}:",rank="+std::to_string(rank))+">";
}
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
        seconds_(seconds),fraction_(fraction),payloadSchemaJson_(std::move(payloadSchemaJson)),elementView_(values_){}
    RuntimeIpeValue(const RuntimeIpeValue &other)
        :kind_(other.kind_),type_(other.type_),owner_(other.owner_),element_(other.element_),key_(other.key_),
        shape_(other.shape_),values_(other.values_),attributes_(other.attributes_),entries_(other.entries_),
        seconds_(other.seconds_),fraction_(other.fraction_),payloadSchemaJson_(other.payloadSchemaJson_),
        appendStorage_(other.appendStorage_),appendMetadata_(other.appendMetadata_),prefixShape_(other.prefixShape_),prefixRank_(other.prefixRank_),
        prefixSize_(other.prefixSize_),prefixCopies_(other.prefixCopies_),
        storageAllocations_(other.storageAllocations_),elementView_(appendStorage_?other.elementView_:std::span<const RuntimeValue>(values_)){}
    Kind kind()const noexcept{return kind_;}
    std::string_view canonicalType()const noexcept override{return appendMetadata_?appendMetadata_->type:type_;}
    bool isReference()const noexcept override{return false;}
    bool valid()const noexcept override{std::size_t n=0,b=0;return compound_detail::validateIpe(*this,nullptr,0,n,b);}
    bool validForOwner(const RuntimeValueOwner &owner)const noexcept override{std::size_t n=0,b=0;return compound_detail::validateIpe(*this,&owner,0,n,b);}
    std::span<const RuntimeNamedField> fields()const noexcept override{return attributes_;}
    const RuntimeValueOwner &owner()const noexcept{return appendMetadata_?appendMetadata_->owner:owner_;}
    const ValueType &elementType()const noexcept{return appendMetadata_?appendMetadata_->element:element_;}
    const ValueType &payloadType()const noexcept{return elementType();}
    // An empty Variant has no synthetic Boolean/zero payload. Its attributes
    // remain ordinary named Variants and payload() deliberately stays checked.
    bool isEmptyVariant()const noexcept{return kind_==Kind::Variant&&values_.empty()&&element_==ValueType{}&&payloadSchemaJson_.empty();}
    const ValueType &valueType()const noexcept{return elementType();}
    const ValueType &keyType()const noexcept{return key_;}
    const std::string &payloadSchemaJson()const noexcept{return payloadSchemaJson_;}
    std::span<const std::uint32_t> shape()const noexcept{return appendStorage_?std::span(prefixShape_).first(prefixRank_):std::span<const std::uint32_t>(shape_);}
    std::size_t elementCount()const noexcept{return appendStorage_?prefixSize_:values_.size();}
    bool packedF64()const noexcept{return appendStorage_&&appendStorage_->kind==compound_detail::PackedNumericKind::F64;}
    bool packedF32()const noexcept{return appendStorage_&&appendStorage_->kind==compound_detail::PackedNumericKind::F32;}
    bool packedNumeric()const noexcept{return packedF64()||packedF32();}
    bool packedI32()const noexcept{return appendStorage_&&appendStorage_->kind==compound_detail::PackedNumericKind::I32;}
    bool packedCells()const noexcept{return appendStorage_&&appendStorage_->kind!=compound_detail::PackedNumericKind::None;}
    template<class T> std::span<const T> numericElements()const noexcept {
        const auto *cells=appendStorage_?std::get_if<std::vector<T>>(&appendStorage_->numeric):nullptr;
        return cells?std::span<const T>(cells->data(),prefixSize_):std::span<const T>{};
    }
    RuntimeValue numericElement(std::size_t index)const {
        if(index>=elementCount())return {};
        if(!packedCells())return elementView_[index];
        return compound_detail::visitPackedNumericKind(appendStorage_->kind,[&]<class T>() -> RuntimeValue {
            if constexpr(std::is_void_v<T>)return {};
            else return static_cast<compound_detail::NumericScalarCarrier<T>>(numericElements<T>()[index]);
        });
    }
    std::span<const std::int32_t> i32Elements()const noexcept{return numericElements<std::int32_t>();}
    bool finiteNumericElements()const noexcept{return packedNumeric()&&prefixSize_<=appendStorage_->finitePrefix.load(std::memory_order_acquire);}
    const float *f32Data()const noexcept{return numericElements<float>().data();}
    std::span<const double> f64Elements()const noexcept{return numericElements<double>();}
    // Compatibility projection for consumers requiring generic cells. It is
    // allocated once per backing buffer, never on the append/charge path.
    // Every returned prefix stays immutable; failures propagate to the checked
    // operation boundary rather than returning a fabricated empty Array.
    std::span<const RuntimeValue> elements()const {
        if(!packedCells())return elementView_;
        std::lock_guard lock(appendStorage_->mutex);
        if(!appendStorage_->genericReady.load(std::memory_order_relaxed)) {
            std::vector<RuntimeValue> slots(appendStorage_->capacity());
            appendStorage_->slots=std::move(slots);
            appendStorage_->genericReady.store(true,std::memory_order_release);
            if(validation_metrics::active)++validation_metrics::active->materializations;
        }
        compound_detail::visitPackedNumericKind(appendStorage_->kind,[&]<class T>() {
            if constexpr(!std::is_void_v<T>) {
                const auto cells=numericElements<T>();
                for(auto i=appendStorage_->genericUsed;i<prefixSize_;++i)
                    appendStorage_->slots[i]=static_cast<compound_detail::NumericScalarCarrier<T>>(cells[i]);
            }
        });
        if(validation_metrics::active)validation_metrics::active->materializedElements+=prefixSize_>appendStorage_->genericUsed?prefixSize_-appendStorage_->genericUsed:0;
        appendStorage_->genericUsed=(std::max)(appendStorage_->genericUsed,prefixSize_);
        return {appendStorage_->slots.data(),prefixSize_};
    }
    bool sealedArrayFor(const ValueType &type,const RuntimeValueOwner *owner)const noexcept {
        return appendMetadata_&&type==appendMetadata_->physical&&(!owner||*owner==appendMetadata_->owner);
    }
    // Receipt can only be installed by the checked Array factories below.
    bool sealedPrimitiveElements()const noexcept{return bool(appendStorage_);}
    // Retains the actual allocation, including its generation/lifetime. A
    // consumer may compare shared ownership, never cache a borrowed address.
    std::shared_ptr<const void> arrayStorageLease()const noexcept{return appendStorage_;}
    std::size_t arrayStorageCapacity()const noexcept{return appendStorage_?appendStorage_->capacity():values_.capacity();}
    std::size_t arrayStorageBytes()const noexcept {
        if(!appendStorage_)return values_.capacity()*sizeof(RuntimeValue);
        const auto genericCapacity=!packedCells()||appendStorage_->genericReady.load(std::memory_order_acquire)?
            appendStorage_->slots.capacity():0;
        return appendStorage_->physicalBytes()+genericCapacity*sizeof(RuntimeValue);
    }
    // Budget the optional generic compatibility view before it can be asked
    // for. Its later materialization cannot increase a retained receipt's
    // charge. This remains distinct from currently allocated physical bytes.
    std::size_t arrayStorageBudgetBytes()const noexcept {
        if(!appendStorage_)return values_.capacity()*sizeof(RuntimeValue);
        const auto genericCapacity=packedCells()?
            (std::max)(appendStorage_->capacity(),appendStorage_->genericReady.load(std::memory_order_acquire)?
                appendStorage_->slots.capacity():0):appendStorage_->slots.capacity();
        return appendStorage_->physicalBytes()+genericCapacity*sizeof(RuntimeValue);
    }
    std::size_t arrayPrefixCopies()const noexcept{return prefixCopies_;}
    std::size_t arrayStorageAllocations()const noexcept{return storageAllocations_;}
    const RuntimeValue &payload()const{return values_.at(0);}
    std::span<const RuntimeNamedField> attributes()const noexcept{return attributes_;}
    std::span<const std::pair<RuntimeValue,RuntimeValue>> entries()const noexcept{return entries_;}
    std::int64_t seconds()const noexcept{return seconds_;}
    std::uint64_t fraction()const noexcept{return fraction_;}
private:
    struct AppendStorage {
        explicit AppendStorage(std::size_t capacity,compound_detail::PackedNumericKind kind)
            :kind(kind),fixedCapacity(capacity),slots(kind==compound_detail::PackedNumericKind::None?capacity:0) {
            compound_detail::visitPackedNumericKind(kind,[&]<class T>() {
                if constexpr(!std::is_void_v<T>)numeric.emplace<std::vector<T>>(capacity);
            });
        }
        // Fixed allocation: neither vector size nor data changes after creation.
        // Readers use their immutable prefix and never read the writer's tail.
        const compound_detail::PackedNumericKind kind;
        const std::size_t fixedCapacity;
        std::vector<RuntimeValue> slots;
        std::variant<std::monostate,std::vector<std::int8_t>,std::vector<std::uint8_t>,
            std::vector<std::int16_t>,std::vector<std::uint16_t>,std::vector<std::int32_t>,
            std::vector<std::uint32_t>,std::vector<std::int64_t>,std::vector<std::uint64_t>,
            std::vector<float>,std::vector<double>> numeric;
        std::atomic<bool> genericReady{false};
        std::atomic<std::size_t> finitePrefix{0};
        std::size_t genericUsed=0;
        std::mutex mutex;
        std::size_t used=0;
        std::size_t capacity()const noexcept{return kind==compound_detail::PackedNumericKind::None?slots.size():fixedCapacity;}
        std::size_t physicalBytes()const noexcept{return fixedCapacity*compound_detail::packedNumericWidth(kind);}
    };
    struct AppendMetadata {
        const std::string type;const ValueType physical,element;const RuntimeValueOwner owner;
    };
    friend std::optional<RuntimeValue> makeRuntimeArray(ValueType,std::vector<std::uint32_t>,
        std::vector<RuntimeValue>,const RuntimeValueOwner &);
    template<class T,class ReadCell>
    friend std::optional<RuntimeValue> readRuntimePackedNumericCells(const ValueType &,
        std::span<const std::uint32_t>,const RuntimeValueOwner &,ReadCell);
    friend class RuntimePrimitiveArrayPool;
    friend class RuntimePrimitiveArrayEdit;
    friend std::optional<RuntimeValue> appendRuntimePrimitiveArray(const RuntimeIpeValue &,
        std::span<const RuntimeValue>, const RuntimeValueOwner &, std::size_t, const PrimitiveArrayAppendReservation *);
    RuntimeIpeValue(std::shared_ptr<const AppendMetadata> metadata,std::shared_ptr<AppendStorage> storage,
        std::span<const std::uint32_t> shape,std::uint32_t length,std::size_t copies,std::size_t allocations)
        :kind_(Kind::Array),seconds_(0),fraction_(0),appendStorage_(std::move(storage)),appendMetadata_(std::move(metadata)),prefixRank_(shape.size()),
        prefixSize_(length),prefixCopies_(copies),storageAllocations_(allocations),
        elementView_(packedCells()?std::span<const RuntimeValue>{}:std::span<const RuntimeValue>(appendStorage_->slots.data(),length)) {
        std::copy(shape.begin(),shape.end(),prefixShape_.begin());
    }
    const Kind kind_;const std::string type_;const RuntimeValueOwner owner_;
    const ValueType element_,key_;const std::vector<std::uint32_t> shape_;
    const std::vector<RuntimeValue> values_;const std::vector<RuntimeNamedField> attributes_;
    const std::vector<std::pair<RuntimeValue,RuntimeValue>> entries_;
    const std::int64_t seconds_;const std::uint64_t fraction_;const std::string payloadSchemaJson_;
    const std::shared_ptr<AppendStorage> appendStorage_;
    const std::shared_ptr<const AppendMetadata> appendMetadata_;
    std::array<std::uint32_t,16> prefixShape_{};
    const std::size_t prefixRank_=0;
    const std::size_t prefixSize_=0,prefixCopies_=0,storageAllocations_=0;
    const std::span<const RuntimeValue> elementView_;
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
// Primitive descriptors have a fixed, complete layout. Compare every field;
// other schemas retain the canonical parser, including nominal/composite ones.
inline bool canonicalDescriptor(const ValueType &type) {
    ValueCategory category=ValueCategory::Unknown;unsigned bits=0;
    const auto name=std::string_view(type.canonicalName);
    if(name=="bool"){category=ValueCategory::Boolean;bits=1;}
    else if(name=="f32"||name=="f64"){category=ValueCategory::FloatingPoint;bits=name=="f32"?32:64;}
    else if(name=="i4"||name=="i8"||name=="i16"||name=="i32"||name=="i64"||name=="u4"||name=="u8"||name=="u16"||name=="u32"||name=="u64") {
        category=name.front()=='i'?ValueCategory::SignedInteger:ValueCategory::UnsignedInteger;
        bits=name.size()==2?unsigned(name[1]-'0'):name[1]=='1'?16:name[1]=='3'?32:64;
    }
    if(category!=ValueCategory::Unknown)return type.category==category&&type.bitWidth==bits&&type.rank==0&&type.elementType.empty();
    if(validation_metrics::active)++validation_metrics::active->types;
    return type==valueTypeFromCanonicalName(name)&&type.known();
}
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

// An intrinsic NULL is data, not an authority for a live registry/resource.
// Sealed separately so a provider cannot make a live handle serializable by
// overriding a virtual marker. The existing Reference carrier is reused.
class IntrinsicNullReferenceAuthority final : public RuntimeReferenceAuthority {
    friend std::optional<RuntimeValue> frog::sdk::runtime::makeRuntimeNullReference(std::string,const RuntimeValueOwner &);
    IntrinsicNullReferenceAuthority(std::string type,RuntimeValueOwner owner):type_(std::move(type)),owner_(std::move(owner)){}
public:
    std::string_view canonicalType()const noexcept override{return type_;}
    bool validForOwner(const RuntimeValueOwner &owner)const noexcept override{return owner==owner_&&runtimeValueOwnerValid(owner_);}
    bool equivalentTo(const RuntimeReferenceAuthority &other)const noexcept override {
        const auto *null=dynamic_cast<const IntrinsicNullReferenceAuthority*>(&other);
        return null&&type_==null->type_&&owner_==null->owner_;
    }
    const RuntimeValueOwner &owner()const noexcept{return owner_;}
private:
    const std::string type_;const RuntimeValueOwner owner_;
};
// A local Source identity is inert until the actual instance registry resolves
// it. Only this sealed data carrier can be encoded with a widget identity.
class WidgetReferenceSymbol final : public RuntimeReferenceAuthority {
    friend std::optional<RuntimeValue> frog::sdk::runtime::makeRuntimeWidgetReferenceSymbol(
        std::string, std::string, const RuntimeValueOwner &);
    WidgetReferenceSymbol(std::string type, std::string id, RuntimeValueOwner owner)
        :type_(std::move(type)), id_(std::move(id)), owner_(std::move(owner)) {}
public:
    std::string_view canonicalType() const noexcept override { return type_; }
    bool validForOwner(const RuntimeValueOwner &owner) const noexcept override {
        return owner==owner_ && runtimeValueOwnerValid(owner_);
    }
    bool equivalentTo(const RuntimeReferenceAuthority &other) const noexcept override {
        const auto *symbol=dynamic_cast<const WidgetReferenceSymbol*>(&other);
        return symbol && symbol->owner_==owner_ && symbol->type_==type_ && symbol->id_==id_;
    }
    std::string_view id() const noexcept { return id_; }
private:
    const std::string type_,id_;
    const RuntimeValueOwner owner_;
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
    if(validation_metrics::active)++validation_metrics::active->values;
    if (depth > 32 || ++nodes > 4096 || std::holds_alternative<std::monostate>(value)) return false;
    try {
        if(const auto *array=runtimeArray(value);array&&array->sealedPrimitiveElements())return array->sealedArrayFor(type,owner);
        if (!canonicalDescriptor(type)) return false;
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
        if(const auto *complex=runtimeComplex(value)) {
            if(type.category!=ValueCategory::Complex || !complex->valid() ||
                (owner && !complex->validForOwner(*owner)))return false;
            const std::size_t payload=type.canonicalName=="c32"?8:16;
            if(payload>16'000'000-bytes)return false;bytes+=payload;return true;
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
        if (type.bitWidth != 4 && type.bitWidth != 8 && type.bitWidth != 16 && type.bitWidth != 32) return false;
        const auto limit = std::int64_t{1} << (type.bitWidth - 1);
        return *number >= -limit && *number < limit;
    }
    if (const auto *number = std::get_if<std::uint64_t>(&value)) {
        if (type.bitWidth == 64) return true;
        return (type.bitWidth == 4 || type.bitWidth == 8 || type.bitWidth == 16 || type.bitWidth == 32) &&
            *number < (std::uint64_t{1} << type.bitWidth);
    }
    if (const auto *number = std::get_if<double>(&value))
        // IEEE specials are physical f32/f64 values. Semantic/literal profiles
        // still decide whether to admit them; aggregate transport must not
        // erase the NaN/Inf gaps of an XY series. Finite f32 stays exact.
        return type.bitWidth == 64 || (type.bitWidth == 32 &&
            (std::isnan(*number) || static_cast<double>(static_cast<float>(*number)) == *number));
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
        // Only the checked factory can install these immutable metadata and
        // storage receipts. Values and owner were checked before first sealing.
        if(v.sealedPrimitiveElements())return !owner||v.owner()==*owner;
        bytes+=v.payloadSchemaJson().size();if(bytes>16'000'000)return false;
        if(validation_metrics::active)++validation_metrics::active->types;
        const auto type=valueTypeFromCanonicalName(v.canonicalType());
        if(!type.known())return false;
        if(v.kind()!=RuntimeIpeValue::Kind::Timestamp &&
            ((validation_metrics::active?++validation_metrics::active->owners:0),
             (!runtimeValueOwnerValid(v.owner())||(owner&&v.owner()!=*owner))))return false;
        const auto child=[&](const RuntimeValue &value,const ValueType &t){return validate(value,t,owner?owner:&v.owner(),depth+1,nodes,bytes);};
        if(v.kind()==RuntimeIpeValue::Kind::Timestamp)return type.canonicalName=="timestamp"&&v.elements().empty()&&v.attributes().empty()&&v.entries().empty();
        if(v.kind()==RuntimeIpeValue::Kind::Array){
            if(v.shape().empty()||v.shape().size()>16||type.category!=ValueCategory::Array||
                type.rank!=v.shape().size()||type.elementType!=v.elementType().canonicalName||!v.attributes().empty()||!v.entries().empty())return false;
            const auto count=array_detail::elementCount(v.shape());
            if(!count||*count!=v.elements().size())return false;
            // Complete first admission of the base and new leaves took place
            // in the only factory allowed to create this immutable receipt.
            if(v.sealedPrimitiveElements())return true;
            // The recursive bound applies to each element's value tree, not
            // to the number of independently typed Array elements.
            for(const auto &value:v.elements()) {
                std::size_t elementNodes=0,elementBytes=0;
                if(!validate(value,v.elementType(),owner?owner:&v.owner(),depth+1,elementNodes,elementBytes))return false;
            }
            return true;
        }
        if(v.kind()==RuntimeIpeValue::Kind::Variant){
            if(type.canonicalName!="variant"||!v.shape().empty()||!v.entries().empty())return false;
            if(v.isEmptyVariant()) {
                if(v.keyType()!=ValueType{})return false;
            }else if(v.elements().size()!=1||!child(v.payload(),v.payloadType()))return false;
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
    if(const auto *array=runtimeArray(value);array&&array->sealedPrimitiveElements())return array->sealedArrayFor(type,&owner);
    if(validation_metrics::active)++validation_metrics::active->owners;
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
[[nodiscard]] inline std::optional<RuntimeValue> makeRuntimeNullReference(std::string type,const RuntimeValueOwner &owner) {
    if(type.size()>65536||!structural_detail::TypeSyntax(type).reference()||!runtimeValueOwnerValid(owner))return {};
    return makeRuntimeReference(std::shared_ptr<const RuntimeReferenceAuthority>{new compound_detail::IntrinsicNullReferenceAuthority(std::move(type),owner)},owner);
}
[[nodiscard]] inline std::optional<RuntimeValue> makeRuntimeWidgetReferenceSymbol(
    std::string type, std::string widgetId, const RuntimeValueOwner &owner) {
    if(type.size()>65536 || !structural_detail::TypeSyntax(type).reference() ||
        widgetId.size()>1024 || !structural_detail::identity(widgetId) || !runtimeValueOwnerValid(owner))return {};
    return makeRuntimeReference(std::shared_ptr<const RuntimeReferenceAuthority>{
        new compound_detail::WidgetReferenceSymbol(std::move(type),std::move(widgetId),owner)},owner);
}

inline std::optional<RuntimeValue> sealRuntimeIpeValue(std::shared_ptr<const RuntimeIpeValue> value) {
    if(!value||!value->valid())return {};return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{std::move(value)}};
}

// Checked typed construction reads directly into the sealed buffer. Shape
// and owner are admitted before allocation, and no draft is published on failure.
template<class T,class ReadCell>
std::optional<RuntimeValue> readRuntimePackedNumericCells(const ValueType &element,
    std::span<const std::uint32_t> shape,const RuntimeValueOwner &owner,ReadCell readCell) {
    const auto kind=compound_detail::packedNumericKind(element.canonicalName);
    const bool matching=compound_detail::visitPackedNumericKind(kind,[]<class Cell>() {
        return std::is_same_v<Cell,T>;
    });
    if(!matching||shape.empty()||shape.size()>16||
        !compound_detail::canonicalDescriptor(element)||!runtimeValueOwnerValid(owner))return {};
    const auto count=array_detail::elementCount(shape);
    if(!count||*count>(std::numeric_limits<std::size_t>::max)()/sizeof(T))return {};
    auto storage=std::make_shared<RuntimeIpeValue::AppendStorage>(*count,kind);
    auto &cells=std::get<std::vector<T>>(storage->numeric);
    std::size_t finitePrefix=*count;
    for(std::uint32_t i=0;i<*count;++i) {
        const auto cell=readCell(i);
        if(!cell)return {};
        cells[i]=*cell;
        if constexpr(std::is_floating_point_v<T>)
            if(finitePrefix==*count&&!std::isfinite(*cell))finitePrefix=i;
    }
    storage->used=*count;
    storage->finitePrefix.store(finitePrefix,std::memory_order_relaxed);
    const auto type=compound_detail::arrayTypeName(element,shape.size());
    auto metadata=std::make_shared<const RuntimeIpeValue::AppendMetadata>(RuntimeIpeValue::AppendMetadata{
        type,valueTypeFromCanonicalName(type),element,owner});
    return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{new RuntimeIpeValue(
        std::move(metadata),std::move(storage),shape,static_cast<std::uint32_t>(*count),0,*count?1U:0U)}};
}

template<class T>
std::optional<RuntimeValue> makeRuntimeArrayFromNumeric(const ValueType &element,
    std::span<const std::uint32_t> shape,std::span<const T> cells,const RuntimeValueOwner &owner) {
    const auto count=array_detail::elementCount(shape);
    if(!count||*count!=cells.size())return {};
    return readRuntimePackedNumericCells<T>(element,shape,owner,[&](std::uint32_t i) {
        return std::optional<T>{cells[i]};
    });
}

namespace compound_detail {
template<class T,class NextWord>
std::optional<RuntimeValue> readRuntimePackedNumericWords(const ValueType &element,
    std::span<const std::uint32_t> shape,const RuntimeValueOwner &owner,NextWord &nextWord) {
    return readRuntimePackedNumericCells<T>(element,shape,owner,[&](std::uint32_t i) -> std::optional<T> {
        const auto word=nextWord(i);
        if(!word)return {};
        if constexpr(sizeof(T)<sizeof(std::uint64_t))
            if(*word>>(sizeof(T)*8))return {};
        return std::bit_cast<T>(static_cast<PackedNumericWord<T>>(*word));
    });
}

template<class T>
std::optional<RuntimeValue> readGenericNumericCells(const ValueType &element,
    std::span<const std::uint32_t> shape,const RuntimeValueOwner &owner,std::span<const RuntimeValue> values) {
    return readRuntimePackedNumericCells<T>(element,shape,owner,[&](std::uint32_t i) {
        return std::optional<T>{static_cast<T>(std::get<NumericScalarCarrier<T>>(values[i]))};
    });
}
}

// Import rank and raw physical words without widening through generic cells.
// Bits outside the declared element width indicate a malformed producer.
template<class NextWord>
std::optional<RuntimeValue> readRuntimePackedNumericArray(const ValueType &element,
    std::vector<std::uint32_t> shape,const RuntimeValueOwner &owner,NextWord nextWord) {
    const auto kind=compound_detail::packedNumericKind(element.canonicalName);
    return compound_detail::visitPackedNumericKind(kind,[&]<class T>() -> std::optional<RuntimeValue> {
        if constexpr(std::is_void_v<T>)return {};
        else return compound_detail::readRuntimePackedNumericWords<T>(element,shape,owner,nextWord);
    });
}

// Allocation and validation are cold relative to shared Array transfers. Keep
// this factory out of callers to avoid duplicating its checked construction.
#if defined(_MSC_VER)
__declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((noinline))
#endif
inline std::optional<RuntimeValue> makeRuntimeArray(ValueType element,std::vector<std::uint32_t> shape,
    std::vector<RuntimeValue> values,const RuntimeValueOwner &owner) {
    const auto type=compound_detail::arrayTypeName(element,shape.size());
    const auto numeric=compound_detail::packedNumericKind(element.canonicalName);
    const bool primitive=element.category==ValueCategory::Boolean||element.category==ValueCategory::SignedInteger||
        element.category==ValueCategory::UnsignedInteger||element.category==ValueCategory::FloatingPoint;
    if(primitive&&(shape.size()==1||numeric!=compound_detail::PackedNumericKind::None)) {
        const auto count=array_detail::elementCount(shape);
        if(shape.empty()||shape.size()>16||!count||*count!=values.size()||
            !runtimeValueOwnerValid(owner)||!compound_detail::canonicalDescriptor(element))return {};
        for(const auto &value:values){std::size_t nodes=0,bytes=0;if(!compound_detail::validate(value,element,&owner,1,nodes,bytes))return {};}
        // Preserve non-finite F32 compatibility payloads exactly, including NaN
        // payloads that the old double carrier can represent beyond F32 bits.
        const bool legacyF32=element.canonicalName=="f32"&&
            std::any_of(values.begin(),values.end(),[](const auto &v){return !std::isfinite(std::get<double>(v));});
        if(numeric!=compound_detail::PackedNumericKind::None&&!legacyF32)
            return compound_detail::visitPackedNumericKind(numeric,[&]<class T>() -> std::optional<RuntimeValue> {
                if constexpr(std::is_void_v<T>)return {};
                else return compound_detail::readGenericNumericCells<T>(element,shape,owner,values);
            });
        auto storage=std::make_shared<RuntimeIpeValue::AppendStorage>(0,compound_detail::PackedNumericKind::None);
        storage->slots=std::move(values);
        storage->used=*count;
        auto metadata=std::make_shared<const RuntimeIpeValue::AppendMetadata>(RuntimeIpeValue::AppendMetadata{
            type,valueTypeFromCanonicalName(type),element,owner});
        return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{new RuntimeIpeValue(
            std::move(metadata),std::move(storage),shape,static_cast<std::uint32_t>(*count),0,*count?1U:0U)}};
    }
    return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Array,type,owner,std::move(element),ValueType{},
        std::move(shape),std::move(values),std::vector<RuntimeNamedField>{},std::vector<std::pair<RuntimeValue,RuntimeValue>>{}));
}
// Read a rank-one IEEE Array directly into its immutable backing storage.
// The reader returns an exact double carrier or an empty optional on failure.
// F32 producers with non-finite carriers still use the legacy generic factory.
template<class ReadCell>
std::optional<RuntimeValue> readRuntimePackedRealArray(const ValueType &element,std::uint32_t length,
    const RuntimeValueOwner &owner,ReadCell readCell) {
    const std::array shape{length};
    if(element.canonicalName=="f64")return readRuntimePackedNumericCells<double>(element,shape,owner,readCell);
    if(element.canonicalName!="f32")return {};
    return readRuntimePackedNumericCells<float>(element,shape,owner,[&](std::uint32_t i) -> std::optional<float> {
        const auto value=readCell(i);
        if(!value||!std::isfinite(*value))return {};
        const auto cell=static_cast<float>(*value);
        return static_cast<double>(cell)==*value?std::optional{cell}:std::nullopt;
    });
}
template<class ReadCell>
std::optional<RuntimeValue> readRuntimePackedI32Array(const ValueType &element,std::uint32_t length,
    const RuntimeValueOwner &owner,ReadCell readCell) {
    const std::array shape{length};
    return readRuntimePackedNumericCells<std::int32_t>(element,shape,owner,[&](std::uint32_t i) -> std::optional<std::int32_t> {
        const auto cell=readCell(i);
        if(!cell||*cell<INT32_MIN||*cell>INT32_MAX)return {};
        return static_cast<std::int32_t>(*cell);
    });
}
inline std::optional<RuntimeValue> appendRuntimePrimitiveArray(const RuntimeIpeValue &base,
    std::span<const RuntimeValue> suffix,const RuntimeValueOwner &owner,std::size_t availableBytes,const PrimitiveArrayAppendReservation *reservation) {
    const auto category=base.elementType().category;
    const bool primitive=category==ValueCategory::Boolean||category==ValueCategory::SignedInteger||
        category==ValueCategory::UnsignedInteger||category==ValueCategory::FloatingPoint;
    if(!primitive||(!base.sealedPrimitiveElements()&&!compound_detail::canonicalDescriptor(base.elementType()))||
        base.kind()!=RuntimeIpeValue::Kind::Array||base.shape().size()!=1||
        !base.validForOwner(owner))return {};
    const auto prefixSize=base.elementCount();
    // Retain the existing owned scalar charge and both physical/logical bounds.
    const auto limit=(std::min)(availableBytes/64,std::size_t{UINT32_MAX});
    if(prefixSize>limit||suffix.size()>limit-prefixSize)return {};
    for(const auto &value:suffix) {
        std::size_t nodes=0,bytes=0;
        if(!compound_detail::validate(value,base.elementType(),&owner,1,nodes,bytes))return {};
    }
    const auto length=prefixSize+suffix.size();
    if(length>UINT32_MAX)return {};
    auto storage=base.appendStorage_;
    std::unique_lock<std::mutex> lock;
    if(storage)lock=std::unique_lock(storage->mutex);
    auto copies=base.prefixCopies_,allocations=base.storageAllocations_;
    using K=compound_detail::PackedNumericKind;
    auto kind=compound_detail::packedNumericKind(base.elementType().canonicalName);
    if(kind==K::F32&&(!base.finiteNumericElements()||
        !std::all_of(suffix.begin(),suffix.end(),[](const auto &v){return std::isfinite(std::get<double>(v));})))
        kind=K::None;
    const bool detach=!storage||!storage->capacity()||storage->used!=prefixSize||length>storage->capacity()||storage->kind!=kind;
    const auto reserveStorage=[&](std::size_t capacity,bool shared) {
        if(!reservation)return true;
        const auto cellBytes=sizeof(RuntimeValue)+compound_detail::packedNumericWidth(kind);
        const auto cells=(std::max)(length*64,capacity*cellBytes);
        const auto credit=shared?(std::min)(reservation->retainedStorageBytes,
            (std::max)(prefixSize*64,base.arrayStorageBudgetBytes())):0;
        return cells<=credit||cells-credit<=reservation->availableAdditionalBytes;
    };
    if(!detach&&!reserveStorage(storage->capacity(),true))return {};
    if(detach) {
        std::size_t capacity=storage&&storage->used==prefixSize?storage->capacity():prefixSize;
        capacity=(std::max)(std::size_t{1},capacity);
        while(capacity<length)capacity=capacity>limit/2?limit:capacity*2;
        capacity=(std::min)(capacity,limit);
        if(!reserveStorage(capacity,false))return {};
        auto next=std::make_shared<RuntimeIpeValue::AppendStorage>(capacity,kind);
        // The base mutex is held. Read the immutable prefix without asking
        // for the compatibility projection, which would take the same mutex.
        compound_detail::visitPackedNumericKind(kind,[&]<class T>() {
            if constexpr(std::is_void_v<T>) {
                for(std::size_t i=0;i<prefixSize;++i)next->slots[i]=base.numericElement(i);
            } else {
                auto &destination=std::get<std::vector<T>>(next->numeric);
                if(base.packedCells()) {
                    const auto cells=base.numericElements<T>();
                    std::copy(cells.begin(),cells.end(),destination.begin());
                } else {
                    using Carrier=compound_detail::NumericScalarCarrier<T>;
                    for(std::size_t i=0;i<prefixSize;++i)
                        destination[i]=static_cast<T>(std::get<Carrier>(base.numericElement(i)));
                }
            }
        });
        if(base.packedNumeric())next->finitePrefix.store((std::min)(prefixSize,storage?storage->finitePrefix.load(std::memory_order_acquire):std::size_t{}),std::memory_order_relaxed);
        copies+=prefixSize;++allocations;
        if(lock.owns_lock())lock.unlock();
        storage=std::move(next);
        lock=std::unique_lock(storage->mutex);
    }
    // Allocate all potentially throwing metadata before extending the tail.
    // Only validated primitive variants are assigned below (no allocations).
    auto metadata=base.appendMetadata_;
    if(!metadata)metadata=std::make_shared<const RuntimeIpeValue::AppendMetadata>(RuntimeIpeValue::AppendMetadata{
        std::string(base.canonicalType()),valueTypeFromCanonicalName(base.canonicalType()),base.elementType(),base.owner()});
    const std::array shape{static_cast<std::uint32_t>(length)};
    std::shared_ptr<const CompoundRuntimeValue> result{new RuntimeIpeValue(std::move(metadata),storage,
        shape,shape[0],copies,allocations)};
    if(validation_metrics::active)++validation_metrics::active->arrayEnvelopes;
    compound_detail::visitPackedNumericKind(kind,[&]<class T>() {
        if constexpr(std::is_void_v<T>) {
            std::copy(suffix.begin(),suffix.end(),storage->slots.begin()+prefixSize);
        } else {
            using Carrier=compound_detail::NumericScalarCarrier<T>;
            auto &destination=std::get<std::vector<T>>(storage->numeric);
            for(std::size_t i=0;i<suffix.size();++i)destination[prefixSize+i]=static_cast<T>(std::get<Carrier>(suffix[i]));
        }
    });
    if(base.elementType().category==ValueCategory::FloatingPoint&&storage->finitePrefix.load(std::memory_order_relaxed)==prefixSize){
        auto finite=prefixSize;for(const auto &value:suffix){if(!std::isfinite(std::get<double>(value)))break;++finite;}
        storage->finitePrefix.store(finite,std::memory_order_release);
    }
    storage->used=length;
    return RuntimeValue{std::move(result)};
}
inline std::optional<RuntimeValue> makeRuntimeVariant(ValueType payloadType,RuntimeValue payload,
    std::vector<RuntimeNamedField> attributes,const RuntimeValueOwner &owner,std::string payloadSchemaJson={}) {
    return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Variant,"variant",owner,std::move(payloadType),ValueType{},
        std::vector<std::uint32_t>{},std::vector<RuntimeValue>{std::move(payload)},std::move(attributes),std::vector<std::pair<RuntimeValue,RuntimeValue>>{},0,0,std::move(payloadSchemaJson)));
}
inline std::optional<RuntimeValue> makeRuntimeEmptyVariant(
    std::vector<RuntimeNamedField> attributes,const RuntimeValueOwner &owner) {
    return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Variant,"variant",owner,ValueType{},ValueType{},
        std::vector<std::uint32_t>{},std::vector<RuntimeValue>{},std::move(attributes),std::vector<std::pair<RuntimeValue,RuntimeValue>>{}));
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
inline const RuntimeValueOwner *runtimeNullReferenceOwner(const RuntimeValue &value)noexcept {
    const auto authority=runtimeReferenceAuthority(value);
    const auto *null=dynamic_cast<const compound_detail::IntrinsicNullReferenceAuthority*>(authority.get());
    return null?&null->owner():nullptr;
}
inline std::string_view runtimeWidgetReferenceSymbol(const RuntimeValue &value) noexcept {
    const auto authority=runtimeReferenceAuthority(value);
    const auto *symbol=dynamic_cast<const compound_detail::WidgetReferenceSymbol*>(authority.get());
    return symbol?symbol->id():std::string_view{};
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
    if (a->isReference() || b->isReference()) {
        if(a==b)return true;
        const auto *x=runtimeNullReferenceOwner(left),*y=runtimeNullReferenceOwner(right);
        return x&&y&&*x==*y;
    }
    if(const auto *ac=runtimeComplex(left),*bc=runtimeComplex(right);ac||bc)
        return ac&&bc&&ac->real()==bc->real()&&ac->imaginary()==bc->imaginary();
    const auto *ia=dynamic_cast<const RuntimeIpeValue *>(a.get()),*ib=dynamic_cast<const RuntimeIpeValue *>(b.get());
    if(ia||ib){
        if(!ia||!ib||ia->kind()!=ib->kind()||ia->isEmptyVariant()!=ib->isEmptyVariant()||ia->elementType()!=ib->elementType()||ia->keyType()!=ib->keyType()||
            !std::equal(ia->shape().begin(),ia->shape().end(),ib->shape().begin(),ib->shape().end())||
            ia->elementCount()!=ib->elementCount()||ia->entries().size()!=ib->entries().size()||
            ia->seconds()!=ib->seconds()||ia->fraction()!=ib->fraction()||ia->payloadSchemaJson()!=ib->payloadSchemaJson())return false;
        if(ia->packedCells()||ib->packedCells()) {
            // Equality is noexcept and never needs a generic allocation.
            // Ordinary IEEE == preserves the existing NaN/signed-zero rule.
            if(ia->packedCells()&&ib->packedCells()) {
                const auto kind=compound_detail::packedNumericKind(ia->elementType().canonicalName);
                if(!compound_detail::visitPackedNumericKind(kind,[&]<class T>() {
                    if constexpr(std::is_void_v<T>)return false;
                    else return std::ranges::equal(ia->numericElements<T>(),ib->numericElements<T>());
                }))return false;
            } else for(std::size_t i=0;i<ia->elementCount();++i)
                if(ia->numericElement(i)!=ib->numericElement(i))return false;
        }else for(std::size_t i=0;i<ia->elementCount();++i)if(!runtimeValuesEqual(ia->elements()[i],ib->elements()[i],depth+1))return false;
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
