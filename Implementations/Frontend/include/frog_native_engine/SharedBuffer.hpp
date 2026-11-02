#pragma once

#include <memory>
#include <vector>
#include <initializer_list>
#include <utility>

namespace frog::native_engine {

// Pixels are immutable once installed. Copies of a document/history snapshot
// share them; replacing pixels installs a new buffer, never mutates old history.
template<class T> class SharedBuffer {
public:
    using value_type = T;
    using const_iterator = typename std::vector<T>::const_iterator;
    SharedBuffer() = default;
    SharedBuffer(std::vector<T> values) { assignVector(std::move(values)); }
    SharedBuffer(std::initializer_list<T> values) : SharedBuffer(std::vector<T>(values)) {}
    SharedBuffer &operator=(std::vector<T> values) { assignVector(std::move(values)); return *this; }
    SharedBuffer &operator=(std::initializer_list<T> values) { return *this = std::vector<T>(values); }
    void assign(std::size_t count, const T &value) { assignVector(std::vector<T>(count, value)); }
    void clear() noexcept { values_.reset(); }
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
    [[nodiscard]] const_iterator begin() const noexcept { return values().begin(); }
    [[nodiscard]] const_iterator end() const noexcept { return values().end(); }
    [[nodiscard]] const void *storageIdentity() const noexcept { return values_.get(); }
    [[nodiscard]] std::size_t retainedBytes() const noexcept {
        return values_ ? sizeof(std::vector<T>) + 32U + capacity() * sizeof(T) : 0U;
    }
    friend bool operator==(const SharedBuffer &a, const SharedBuffer &b) { return a.values_ == b.values_ || a.values() == b.values(); }
    friend bool operator==(const SharedBuffer &a, const std::vector<T> &b) { return a.values() == b; }
private:
    void assignVector(std::vector<T> values) {
        values_ = values.empty() ? nullptr : std::make_shared<const std::vector<T>>(std::move(values));
    }
    std::shared_ptr<const std::vector<T>> values_;
};

} // namespace frog::native_engine
