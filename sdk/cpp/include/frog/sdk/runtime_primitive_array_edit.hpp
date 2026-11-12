#pragma once
#include "frog/sdk/runtime_compound_value.hpp"
#include <bit>

namespace frog::sdk::runtime {
// Optional typed editing/view API: keep its template/code dependencies out of
// ordinary generic Array consumers. The data allocation/receipt is unchanged.
inline std::span<const float> f32Elements(const RuntimeIpeValue &value)noexcept {
    return value.packedF32()?std::span<const float>(value.f32Data(),value.elementCount()):std::span<const float>{};
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
        static std::size_t bytes(const Storage &s)noexcept{return s.numbers.capacity()*sizeof(double)+s.singleCapacity*sizeof(float)+s.slots.capacity()*sizeof(RuntimeValue);}
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
    using Storage=RuntimeIpeValue::AppendStorage;
    std::shared_ptr<RuntimePrimitiveArrayPool::State> pool_;
    std::shared_ptr<const RuntimeIpeValue::AppendMetadata> metadata_;
    std::unique_ptr<Storage> draft_;
    std::uint32_t length_=0;
    RuntimePrimitiveArrayEdit()=default;
public:
    FROG_ARRAY_EDIT_NOINLINE static std::unique_ptr<RuntimePrimitiveArrayEdit> begin(const RuntimeIpeValue &source,
        const ValueType &type,const RuntimeValueOwner &owner,std::size_t availableBytes,RuntimePrimitiveArrayPool &pool) {
        if(!runtimeValueOwnerValid(owner)||!source.sealedArrayFor(type,&owner)||!source.packedNumeric()||source.shape().size()!=1)return {};
        const auto n=source.elementCount(),overhead=64+type.canonicalName.size()*3;
        if(overhead>availableBytes||n>(availableBytes-overhead)/64)return {};
        auto edit=std::unique_ptr<RuntimePrimitiveArrayEdit>(new RuntimePrimitiveArrayEdit);
        edit->pool_=pool.state_;edit->metadata_=source.appendMetadata_;edit->length_=static_cast<std::uint32_t>(n);
        {std::lock_guard lock(edit->pool_->mutex);
            auto &free=edit->pool_->free;
            const auto found=std::find_if(free.begin(),free.end(),[&](const auto &s){return s->capacity()==n&&s->single==source.packedF32();});
            if(found!=free.end()){edit->draft_=std::move(*found);free.erase(found);edit->pool_->cachedBytes.fetch_sub(RuntimePrimitiveArrayPool::State::bytes(*edit->draft_),std::memory_order_relaxed);++edit->pool_->reuses;}
            else {
                // Retire one obsolete size/representation rather than leaving
                // two stale buffers permanently excluding the current size.
                if(free.size()==2){edit->pool_->cachedBytes.fetch_sub(RuntimePrimitiveArrayPool::State::bytes(*free.back()),std::memory_order_relaxed);free.pop_back();}
                edit->draft_=std::make_unique<Storage>(n,source.packedF64(),source.packedF32());++edit->pool_->allocations;
            }
            if(edit->pool_->cachedBytes.load(std::memory_order_relaxed)>availableBytes-overhead-n*64){free.clear();edit->pool_->cachedBytes.store(0,std::memory_order_relaxed);}
        }
        // No reader remains when a recycled allocation is acquired. Invalidate
        // its optional compatibility projection before publishing a new value.
        edit->draft_->genericUsed=0;edit->draft_->used=n;
        return edit;
    }
    std::span<float> f32()noexcept{return draft_?std::span<float>(draft_->singles.get(),draft_->singleCapacity):std::span<float>{};}
    std::span<double> f64()noexcept{return draft_?std::span<double>(draft_->numbers):std::span<double>{};}
    FROG_ARRAY_EDIT_NOINLINE std::optional<RuntimeValue> seal() {
        if(!draft_)return {};
        // Branch-free integer reductions allow the compiler to vectorize the
        // complete finite check without changing any floating-point rule.
        std::uint32_t invalid32=0;std::uint64_t invalid64=0;
        for(const auto x:std::span<const float>(draft_->singles.get(),draft_->singleCapacity))invalid32|=(std::bit_cast<std::uint32_t>(x)&UINT32_C(0x7f800000))==UINT32_C(0x7f800000);
        for(const auto x:draft_->numbers)invalid64|=(std::bit_cast<std::uint64_t>(x)&UINT64_C(0x7ff0000000000000))==UINT64_C(0x7ff0000000000000);
        if(invalid32||invalid64)return {};
        draft_->finitePrefix.store(length_,std::memory_order_release);
        const std::weak_ptr<RuntimePrimitiveArrayPool::State> weak=pool_;
        std::shared_ptr<Storage> storage(draft_.release(),[weak](Storage *value)noexcept{
            if(auto state=weak.lock())state->recycle(value);else delete value;
        });
        return RuntimeValue{std::shared_ptr<const CompoundRuntimeValue>{new RuntimeIpeValue(metadata_,std::move(storage),length_,0,0)}};
    }
};
#undef FROG_ARRAY_EDIT_NOINLINE
}
