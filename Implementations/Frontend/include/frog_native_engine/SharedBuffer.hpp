#pragma once

#include <memory>
#include <vector>
#include <initializer_list>
#include <utility>
#include <limits>
#include <string>
#include <type_traits>
#include <optional>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

namespace frog::native_engine {

// Reads never expose mutable storage. Retained documents/history/workers must
// retain a SharedBuffer owner: their version is immutable while shared. A
// same-key edit may reuse exclusively owned storage after preparing its value.
template<class T> class SharedBuffer {
public:
    using value_type = T;
    using const_iterator = typename std::vector<T>::const_iterator;
    SharedBuffer() = default;
    SharedBuffer(const SharedBuffer &)=default;
    SharedBuffer &operator=(const SharedBuffer &)=default;
    SharedBuffer(SharedBuffer &&other) noexcept
        :values_(std::move(other.values_)),order_(std::move(other.order_)),
         retainedBytes_(std::exchange(other.retainedBytes_,0)){}
    SharedBuffer &operator=(SharedBuffer &&other) noexcept {
        if(this!=&other) {
            values_=std::move(other.values_);order_=std::move(other.order_);
            retainedBytes_=std::exchange(other.retainedBytes_,0);
        }
        return *this;
    }
    SharedBuffer(std::vector<T> values) { assignVector(std::move(values)); }
    SharedBuffer(std::initializer_list<T> values) : SharedBuffer(std::vector<T>(values)) {}
    SharedBuffer &operator=(std::vector<T> values) { assignVector(std::move(values)); return *this; }
    SharedBuffer &operator=(std::initializer_list<T> values) { return *this = std::vector<T>(values); }
    void assign(std::size_t count, const T &value) { assignVector(std::vector<T>(count, value)); }
    void clear() noexcept { values_.reset(); order_.reset(); retainedBytes_=0; }
    void swap(SharedBuffer &other) noexcept { values_.swap(other.values_);order_.swap(other.order_); std::swap(retainedBytes_,other.retainedBytes_); }
    template<class Edit> void update(Edit &&edit) {
        auto prepared=values();
        std::forward<Edit>(edit)(prepared);
        assignVector(std::move(prepared));
    }
    [[nodiscard]] bool uniqueStorage() const noexcept { return values_ && values_.use_count()==1; }
    // A same-key replacement preserves the prepared lookup index. All throwing
    // work precedes the swap; a shared version is copied before publication.
    // General/key-changing edits continue to use update(), which always copies.
    void replaceAt(std::size_t index, T prepared) {
        static_assert(std::is_nothrow_swappable_v<T>, "SharedBuffer replacement requires a nonthrowing value swap.");
        if(index>=size())throw std::out_of_range("SharedBuffer replacement index is outside the value.");
        if constexpr(requires(const T &value){sharedBufferElementKey(value);}) {
            if(sharedBufferElementKey(values()[index])!=sharedBufferElementKey(prepared))
                throw std::invalid_argument("SharedBuffer replacement must preserve the indexed key.");
        }
        const auto maximum=(std::numeric_limits<std::size_t>::max)();
        const auto oldHeap=elementHeapBytes(values()[index]);
        const auto newHeap=elementHeapBytes(prepared);
        if(uniqueStorage() && retainedBytes_!=maximum && oldHeap<=retainedBytes_ &&
            newHeap<=maximum-(retainedBytes_-oldHeap)) {
            const auto bytes=retainedBytes_-oldHeap+newHeap;
            using std::swap;swap((*values_)[index],prepared);
            retainedBytes_=bytes;
            return;
        }
        // Also recount the exceptional saturated-cache case: subtraction from
        // a saturated charge cannot establish the new exact charge.
        auto replacement=std::make_shared<std::vector<T>>(values());
        // Keep payload and index as one accounted retained version. Copy the
        // existing permutation once on detachment, without sorting its keys.
        auto replacementOrder=order_ ?
            std::make_shared<const std::vector<std::size_t>>(*order_) : nullptr;
        using std::swap;swap((*replacement)[index],prepared);
        const auto bytes=retainedSize(*replacement,replacementOrder);
        values_=std::move(replacement);order_=std::move(replacementOrder);retainedBytes_=bytes;
    }
    [[nodiscard]] const std::vector<T> &values() const noexcept {
        static const std::vector<T> empty;
        return values_ ? *values_ : empty;
    }
    operator const std::vector<T> &() const noexcept { return values(); }
    [[nodiscard]] std::size_t size() const noexcept { return values().size(); }
    [[nodiscard]] std::size_t capacity() const noexcept { return values().capacity(); }
    [[nodiscard]] bool empty() const noexcept { return values().empty(); }
    [[nodiscard]] const T *data() const noexcept { return values().data(); }
    [[nodiscard]] const T &operator[](std::size_t i) const noexcept { return values()[i]; }
    [[nodiscard]] const T &front() const noexcept { return values().front(); }
    [[nodiscard]] const T &back() const noexcept { return values().back(); }
    [[nodiscard]] const_iterator begin() const noexcept { return values().begin(); }
    [[nodiscard]] const_iterator end() const noexcept { return values().end(); }
    // The key/index, if this element type has one, is prepared together with
    // the immutable value, never lazily during a read/paint callback.
    template<class Key> [[nodiscard]] const_iterator findByKey(const Key &key) const {
        if(order_) {
            const auto found=std::lower_bound(order_->begin(),order_->end(),key,
                [&](std::size_t i,const auto &k){return sharedBufferElementKey(values()[i])<k;});
            return found!=order_->end() && sharedBufferElementKey(values()[*found])==key
                ? begin()+*found:end();
        }
        const auto found=std::lower_bound(begin(),end(),key,
            [](const auto &v,const auto &k){return sharedBufferElementKey(v)<k;});
        return found!=end() && sharedBufferElementKey(*found)==key ? found:end();
    }
    [[nodiscard]] const void *storageIdentity() const noexcept { return values_.get(); }
    [[nodiscard]] std::size_t retainedBytes() const noexcept {
        return retainedBytes_;
    }
    friend bool operator==(const SharedBuffer &a, const SharedBuffer &b) { return a.values_ == b.values_ || a.values() == b.values(); }
    friend bool operator==(const SharedBuffer &a, const std::vector<T> &b) { return a.values() == b; }
private:
    static std::size_t elementHeapBytes(const T &value) noexcept {
        if constexpr(std::is_same_v<T,std::string>) {
            const auto data=reinterpret_cast<std::uintptr_t>(value.data());
            const auto object=reinterpret_cast<std::uintptr_t>(&value);
            return data<object || data-object>=sizeof(value) ? value.capacity()+1U : 0U;
        } else if constexpr(requires{sharedBufferElementHeapBytes(value);}) {
            static_assert(noexcept(sharedBufferElementHeapBytes(value)), "SharedBuffer heap accounting must not throw.");
            return sharedBufferElementHeapBytes(value);
        } else return 0U;
    }
    static std::size_t retainedSize(const std::vector<T> &values,
        const std::shared_ptr<const std::vector<std::size_t>> &order) noexcept {
        const auto maximum=(std::numeric_limits<std::size_t>::max)();
        std::size_t bytes=sizeof(std::vector<T>)+32U;
        const auto add=[&](std::size_t n){bytes=n>maximum-bytes?maximum:bytes+n;};
        if(values.capacity()>(maximum-bytes)/sizeof(T))bytes=maximum;
        else add(values.capacity()*sizeof(T));
        if constexpr(std::is_same_v<T,std::string> ||
            requires(const T &value){sharedBufferElementHeapBytes(value);})
            for(const auto &value:values)add(elementHeapBytes(value));
        if(order) {
            add(sizeof(std::vector<std::size_t>)+32U);
            if(order->capacity()>(maximum-bytes)/sizeof(std::size_t))bytes=maximum;
            else add(order->capacity()*sizeof(std::size_t));
        }
        return bytes;
    }
    void assignVector(std::vector<T> values) {
        if(values.empty()) {clear();return;}
        auto prepared=std::make_shared<std::vector<T>>(std::move(values));
        std::shared_ptr<const std::vector<std::size_t>> order;
        if constexpr(requires(const T &value){sharedBufferElementKey(value);}) {
            const auto before=[](const T &a,const T &b){return sharedBufferElementKey(a)<sharedBufferElementKey(b);};
            if(!std::is_sorted(prepared->begin(),prepared->end(),before)) {
                std::vector<std::size_t> sorted(prepared->size());
                for(std::size_t i=0;i<sorted.size();++i)sorted[i]=i;
                std::sort(sorted.begin(),sorted.end(),[&](auto a,auto b){
                    const auto &x=sharedBufferElementKey((*prepared)[a]);const auto &y=sharedBufferElementKey((*prepared)[b]);
                    return x<y || (!(y<x)&&a<b);
                });
                order=std::make_shared<const std::vector<std::size_t>>(std::move(sorted));
            }
        }
        const auto bytes=retainedSize(*prepared,order);
        values_=std::move(prepared);order_=std::move(order);retainedBytes_=bytes;
    }
    std::shared_ptr<std::vector<T>> values_;
    std::shared_ptr<const std::vector<std::size_t>> order_;
    std::size_t retainedBytes_=0;
};

// Optional persisted text uses the same immutable owner as other retained
// payloads. Empty text and absent text remain distinct in the document codec.
class SharedOptionalString {
public:
    SharedOptionalString()=default;
    SharedOptionalString(std::nullopt_t) noexcept {}
    SharedOptionalString(std::string value){assign(std::move(value));}
    SharedOptionalString(const char *value):SharedOptionalString(std::string(value)){}
    SharedOptionalString(std::optional<std::string> value){if(value)assign(std::move(*value));}
    SharedOptionalString &operator=(std::string value){assign(std::move(value));return *this;}
    SharedOptionalString &operator=(const char *value){return *this=std::string(value);}
    SharedOptionalString &operator=(std::optional<std::string> value){if(value)assign(std::move(*value));else reset();return *this;}
    SharedOptionalString &operator=(std::nullopt_t) noexcept {reset();return *this;}
    [[nodiscard]] bool has_value() const noexcept {return !text_.empty();}
    explicit operator bool() const noexcept {return has_value();}
    [[nodiscard]] const std::string &operator*() const noexcept {return text_.front();}
    [[nodiscard]] const std::string *operator->() const noexcept {return &text_.front();}
    [[nodiscard]] const std::string &value() const {if(!has_value())throw std::bad_optional_access{};return **this;}
    void reset() noexcept {text_.clear();}
    void swap(SharedOptionalString &other) noexcept {text_.swap(other.text_);}
    [[nodiscard]] const void *storageIdentity() const noexcept {return text_.storageIdentity();}
    [[nodiscard]] std::size_t retainedBytes() const noexcept {return text_.retainedBytes();}
    friend bool operator==(const SharedOptionalString &a,const SharedOptionalString &b){return a.text_==b.text_;}
    friend bool operator==(const SharedOptionalString &a,const std::optional<std::string> &b){return bool(a)==bool(b)&&(!a||*a==*b);}
    friend bool operator==(const SharedOptionalString &a,const std::string &b){return a&&*a==b;}
    friend bool operator==(const SharedOptionalString &a,const char *b){return a&&*a==b;}
    friend bool operator==(const SharedOptionalString &a,std::nullopt_t) noexcept{return !a;}
private:
    void assign(std::string value){std::vector<std::string> prepared;prepared.push_back(std::move(value));text_=std::move(prepared);}
    SharedBuffer<std::string> text_;
};

} // namespace frog::native_engine
