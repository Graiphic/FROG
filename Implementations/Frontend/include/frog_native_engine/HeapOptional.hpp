#pragma once
#include <memory>
#include <optional>
#include <utility>

namespace frog::native_engine {

// A value-semantic optional for large, infrequently present records. Copies
// own independent values, exactly as an optional<T>; empty records occupy one
// pointer rather than reserving sizeof(T) in every diagram node.
template<class T> class HeapOptional {
public:
    HeapOptional() noexcept = default;
    HeapOptional(std::nullopt_t) noexcept {}
    HeapOptional(const T &value) : value_(std::make_unique<T>(value)) {}
    HeapOptional(T &&value) : value_(std::make_unique<T>(std::move(value))) {}
    HeapOptional(const std::optional<T> &value) {
        if (value) value_ = std::make_unique<T>(*value);
    }
    HeapOptional(std::optional<T> &&value) {
        if (value) value_ = std::make_unique<T>(std::move(*value));
    }
    HeapOptional(const HeapOptional &other) {
        if (other.value_) value_ = std::make_unique<T>(*other.value_);
    }
    HeapOptional(HeapOptional &&) noexcept = default;
    HeapOptional &operator=(const HeapOptional &other) {
        if (this != &other) {
            auto replacement = other.value_ ? std::make_unique<T>(*other.value_) : nullptr;
            value_ = std::move(replacement);
        }
        return *this;
    }
    HeapOptional &operator=(HeapOptional &&) noexcept = default;
    HeapOptional &operator=(std::nullopt_t) noexcept { reset(); return *this; }
    HeapOptional &operator=(const T &value) { value_ = std::make_unique<T>(value); return *this; }
    HeapOptional &operator=(T &&value) { value_ = std::make_unique<T>(std::move(value)); return *this; }
    HeapOptional &operator=(const std::optional<T> &value) {
        value_ = value ? std::make_unique<T>(*value) : nullptr; return *this;
    }
    HeapOptional &operator=(std::optional<T> &&value) {
        value_ = value ? std::make_unique<T>(std::move(*value)) : nullptr; return *this;
    }
    template<class... Args> T &emplace(Args &&...args) {
        value_ = std::make_unique<T>(std::forward<Args>(args)...); return *value_;
    }
    void reset() noexcept { value_.reset(); }
    void swap(HeapOptional &other) noexcept { value_.swap(other.value_); }
    [[nodiscard]] bool has_value() const noexcept { return value_ != nullptr; }
    explicit operator bool() const noexcept { return has_value(); }
    T &operator*() noexcept { return *value_; }
    const T &operator*() const noexcept { return *value_; }
    T *operator->() noexcept { return value_.get(); }
    const T *operator->() const noexcept { return value_.get(); }
    T &value() { if (!value_) throw std::bad_optional_access(); return *value_; }
    const T &value() const { if (!value_) throw std::bad_optional_access(); return *value_; }
    template<class U> T value_or(U &&fallback) const {
        return value_ ? *value_ : T(std::forward<U>(fallback));
    }
private:
    std::unique_ptr<T> value_;
};
} // namespace frog::native_engine
