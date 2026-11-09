#pragma once
#include <frog/bridge.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace frog::bridge::json {
enum class Kind {Null,Boolean,Number,String,Array,Object};
// Source convenience API, not a third-party binary ABI. Every handle shares
// an immutable complete tree. No Frontend, Runtime or UI type is exposed.
class Value {
public:
    Value(const Value&) noexcept=default;
    Value(Value&&) noexcept=default;
    Value& operator=(const Value&) noexcept=default;
    Value& operator=(Value&&) noexcept=default;
    [[nodiscard]] Kind kind() const;
    [[nodiscard]] bool boolean() const;
    [[nodiscard]] std::string_view string() const;
    [[nodiscard]] std::string_view numberLexeme() const;
    // Exact token span from the admitted input, excluding surrounding space.
    // Shares the same immutable lifetime as this handle; never re-encoded.
    [[nodiscard]] std::string_view originalUtf8() const;
    [[nodiscard]] std::optional<Value> member(std::string_view name) const;
    [[nodiscard]] std::vector<Value> elements() const;
    [[nodiscard]] std::vector<std::pair<std::string,Value>> members() const;
private:
    struct Impl;
    std::shared_ptr<const Impl> impl_;
    explicit Value(std::shared_ptr<const Impl>);
    [[nodiscard]] const Impl &data() const;
    friend std::optional<Value> parse(std::string_view,Error&,std::size_t);
    friend std::string encode(const Value&);
};
// maxBytes is a caller ceiling, capped at 16 MiB. The same strict Frontend
// parser enforces Unicode, duplicate-key, nesting and value-count admission.
[[nodiscard]] std::optional<Value> parse(std::string_view,Error&,std::size_t maxBytes=16U*1024U*1024U);
[[nodiscard]] std::string encode(const Value&);
// Strict UTF-8 string quoting, bounded to 16 MiB of encoded output.
[[nodiscard]] std::string quote(std::string_view);
// Exact bytes, including embedded NUL; no text interpretation or trust claim.
[[nodiscard]] std::string sha256(std::string_view);
}
