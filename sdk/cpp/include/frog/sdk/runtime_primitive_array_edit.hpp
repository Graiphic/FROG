#pragma once
#include "frog/sdk/runtime_compound_value.hpp"
#include <bit>

namespace frog::runtime::numeric_aggregate { struct PreparedIncrementArrayWriter; }

namespace frog::sdk::runtime {
// Optional typed editing/view API: keep its template/code dependencies out of
// ordinary generic Array consumers. The data allocation/receipt is unchanged.
inline std::span<const float> f32Elements(const RuntimeIpeValue &value)noexcept {
    return value.numericElements<float>();
}
#if defined(_MSC_VER)
#define FROG_ARRAY_EDIT_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define FROG_ARRAY_EDIT_NOINLINE __attribute__((noinline))
#else
#define FROG_ARRAY_EDIT_NOINLINE
#endif
// Activation-owned recycle authority. Only the last envelope/storage lease's
// deleter can return an allocation here. A source, view lease, fanout or capture
// therefore excludes that allocation from editing, without a use_count guess.
class RuntimePrimitiveArrayPool final {
    friend class RuntimePrimitiveArrayEdit;
    using Storage=RuntimeIpeValue::AppendStorage;
    struct State {
        std::mutex mutex;std::vector<std::unique_ptr<Storage>> free;
        std::atomic<std::size_t> cachedBytes{0};
        std::size_t allocations=0,reuses=0;
        State(){free.reserve(2);}
        static std::size_t bytes(const Storage &s)noexcept{return s.physicalBytes()+s.slots.capacity()*sizeof(RuntimeValue);}
        void recycle(Storage *raw)noexcept {
            std::unique_ptr<Storage> value(raw);
            try{std::lock_guard lock(mutex);if(free.size()<2){const auto charge=bytes(*value);free.push_back(std::move(value));cachedBytes.fetch_add(charge,std::memory_order_relaxed);}}catch(...){}
        }
    };
    std::shared_ptr<State> state_=std::make_shared<State>();
public:
    FROG_ARRAY_EDIT_NOINLINE RuntimePrimitiveArrayPool()=default;
    FROG_ARRAY_EDIT_NOINLINE ~RuntimePrimitiveArrayPool()=default;
    struct Statistics {std::size_t allocations=0,reuses=0;};
    Statistics statistics()const{std::lock_guard lock(state_->mutex);return {state_->allocations,state_->reuses};}
    std::size_t retainedBytes()const noexcept{return state_->cachedBytes.load(std::memory_order_relaxed);}
    void clear(){std::lock_guard lock(state_->mutex);state_->free.clear();state_->cachedBytes.store(0,std::memory_order_relaxed);}
};
// A transactional unpublished destination, never an edit of a live value.
// Cancellation discards the draft. Seal checks finite payloads once and gives
// readers only const views; the mutable spans must not outlive this draft.
class RuntimePrimitiveArrayEdit final {
    friend struct frog::runtime::numeric_aggregate::PreparedIncrementArrayWriter;
    using Storage=RuntimeIpeValue::AppendStorage;
    std::shared_ptr<RuntimePrimitiveArrayPool::State> pool_;
    std::shared_ptr<const RuntimeIpeValue::AppendMetadata> metadata_;
    std::unique_ptr<Storage> draft_;
    std::array<std::uint32_t,16> shape_{};
    std::size_t rank_=0;
    std::uint32_t length_=0;
    RuntimePrimitiveArrayEdit()=default;
public:
    FROG_ARRAY_EDIT_NOINLINE static std::unique_ptr<RuntimePrimitiveArrayEdit> begin(const RuntimeIpeValue &source,
        const ValueType &type,const RuntimeValueOwner &owner,std::size_t availableBytes,RuntimePrimitiveArrayPool &pool) {
        if(!runtimeValueOwnerValid(owner)||!source.sealedArrayFor(type,&owner)||!source.packedCells())return {};
        return beginDraft(source.appendMetadata_,source.shape(),source.elementCount(),source.appendStorage_->kind,availableBytes,pool);
    }
    FROG_ARRAY_EDIT_NOINLINE static std::unique_ptr<RuntimePrimitiveArrayEdit> begin(const ValueType &outputType,
        std::span<const std::uint32_t> outputShape,const RuntimeValueOwner &owner,std::size_t availableBytes,
        RuntimePrimitiveArrayPool &pool) {
        using K=compound_detail::PackedNumericKind;
        if(outputType.category!=ValueCategory::Array||outputShape.empty()||outputShape.size()>16||
            outputType.rank!=outputShape.size()||!runtimeValueOwnerValid(owner))return {};
        const auto kind=compound_detail::packedNumericKind(outputType.elementType);
        const auto n=array_detail::elementCount(outputShape);
        if(kind==K::None||!n)return {};
        const auto element=valueTypeFromCanonicalName(outputType.elementType);
        const auto type=compound_detail::arrayTypeName(element,outputShape.size());
        if(outputType!=valueTypeFromCanonicalName(type))return {};
        auto metadata=std::make_shared<const RuntimeIpeValue::AppendMetadata>(RuntimeIpeValue::AppendMetadata{
            type,outputType,element,owner});
        return beginDraft(std::move(metadata),outputShape,*n,kind,availableBytes,pool);
    }
    template<class T> std::span<T> numeric()noexcept {
        auto *cells=draft_?std::get_if<std::vector<T>>(&draft_->numeric):nullptr;
        return cells?std::span<T>(*cells):std::span<T>{};
    }
    std::span<float> f32()noexcept{return numeric<float>();}
    std::span<double> f64()noexcept{return numeric<double>();}
    FROG_ARRAY_EDIT_NOINLINE std::optional<RuntimeValue> seal() {
        if(!draft_)return {};
        // Branch-free integer reductions allow the compiler to vectorize the
        // complete finite check without changing any floating-point rule.
        std::uint32_t invalid32=0;std::uint64_t invalid64=0;
        for(const auto x:f32())invalid32|=(std::bit_cast<std::uint32_t>(x)&UINT32_C(0x7f800000))==UINT32_C(0x7f800000);
        for(const auto x:f64())invalid64|=(std::bit_cast<std::uint64_t>(x)&UINT64_C(0x7ff0000000000000))==UINT64_C(0x7ff0000000000000);
        if(invalid32||invalid64)return {};
        return sealWrittenFinite();
    }
private:
    static std::unique_ptr<RuntimePrimitiveArrayEdit> beginDraft(
        std::shared_ptr<const RuntimeIpeValue::AppendMetadata> metadata,std::span<const std::uint32_t> shape,
        std::size_t n,compound_detail::PackedNumericKind kind,std::size_t availableBytes,RuntimePrimitiveArrayPool &pool) {
        const auto overhead=64+metadata->type.size()*3;
        if(overhead>availableBytes||n>(availableBytes-overhead)/64)return {};
        auto edit=std::unique_ptr<RuntimePrimitiveArrayEdit>(new RuntimePrimitiveArrayEdit);
        edit->pool_=pool.state_;edit->metadata_=std::move(metadata);edit->length_=static_cast<std::uint32_t>(n);
        std::copy(shape.begin(),shape.end(),edit->shape_.begin());
        edit->rank_=shape.size();
        {std::lock_guard lock(edit->pool_->mutex);
            auto &free=edit->pool_->free;
            const auto found=std::find_if(free.begin(),free.end(),[&](const auto &s){return s->capacity()==n&&s->kind==kind;});
            if(found!=free.end()){edit->draft_=std::move(*found);free.erase(found);edit->pool_->cachedBytes.fetch_sub(RuntimePrimitiveArrayPool::State::bytes(*edit->draft_),std::memory_order_relaxed);++edit->pool_->reuses;}
            else {
                // Retire one obsolete size/representation rather than leaving
                // two stale buffers permanently excluding the current size.
                if(free.size()==2){edit->pool_->cachedBytes.fetch_sub(RuntimePrimitiveArrayPool::State::bytes(*free.back()),std::memory_order_relaxed);free.pop_back();}
                edit->draft_=std::make_unique<Storage>(n,kind);++edit->pool_->allocations;
            }
            if(edit->pool_->cachedBytes.load(std::memory_order_relaxed)>availableBytes-overhead-n*64){free.clear();edit->pool_->cachedBytes.store(0,std::memory_order_relaxed);}
        }
        // No reader remains when a recycled allocation is acquired. Invalidate
        // its optional compatibility projection before publishing a new value.
        edit->draft_->genericUsed=0;edit->draft_->used=n;
        return edit;
    }
    // Only the Increment producer, which validates every stored lane in its
    // write loop, can use this after completing the entire admitted draft.
    FROG_ARRAY_EDIT_NOINLINE std::optional<RuntimeValue> sealWrittenFinite() {
        draft_->finitePrefix.store(length_,std::memory_order_release);
        const std::weak_ptr<RuntimePrimitiveArrayPool::State> weak=pool_;
        std::shared_ptr<Storage> storage(draft_.release(),[weak](Storage *value)noexcept{
            if(auto state=weak.lock())state->recycle(value);else delete value;
        });
        return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{new RuntimeIpeValue(
            metadata_,std::move(storage),std::span(shape_).first(rank_),length_,0,0)}};
    }
};
#undef FROG_ARRAY_EDIT_NOINLINE
}
