#pragma once

#include "frog_native_engine/NumericValueLexeme.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

inline constexpr std::size_t kMaximumFrogDocumentBytes = numeric_value::maximumDecimalNumberBytes;
inline constexpr std::size_t kMaximumFrogJsonStringBytes = 16U * 1024U * 1024U;
inline constexpr std::size_t kMaximumFrogJsonNestingDepth = 128U;
// Dense authored arrays share this bounded document budget with metadata.
inline constexpr std::size_t kMaximumFrogJsonValueCount = 1'000'000U;

namespace source {

enum class JsonNumberPolicy : std::uint8_t {
    FiniteDouble,
    PreserveLexeme
};

// JSON storage shared by public source and the legacy authoring adapter.
// It has no dependency on canvas, widgets, or executable graph projections.
struct JsonValue {
    enum class Type : std::uint8_t {
        Null,
        Boolean,
        Number,
        String,
        Object,
        Array
    };

    Type type = Type::Null;
    bool boolean = false;
    // Convenience projection for the legacy FiniteDouble reader only. Under
    // PreserveLexeme every Number has quiet_NaN here, even when it would fit.
    double number = 0.0;
    // Decoded UTF-8 for String; exact, authoritative JSON lexeme for Number.
    std::string string;
    std::map<std::string, JsonValue> object;
    std::vector<JsonValue> array;

    // Half-open token ranges in the exact input supplied to parseJson. For an
    // object member, memberBegin starts at its quoted key; for other values it
    // equals sourceBegin. They exclude surrounding whitespace and commas.
    // Fabricated values have no source range. These are preservation metadata,
    // never semantic identity, and only apply to the original parsed bytes.
    std::size_t sourceBegin = std::string_view::npos;
    std::size_t sourceEnd = std::string_view::npos;
    std::size_t memberBegin = std::string_view::npos;

    [[nodiscard]] const JsonValue *member(std::string_view name) const noexcept;
};

// Both policies enforce the same strict JSON grammar, Unicode and resource
// limits. PreserveLexeme does not require JSON numbers to fit in binary64.
// A successful parse establishes JSON readability, not FROG source validity.
[[nodiscard]] std::optional<JsonValue> parseJson(
    std::string_view source,
    std::string *error = nullptr,
    JsonNumberPolicy policy = JsonNumberPolicy::FiniteDouble);

// Same strict parser and limits, with its actual failure cursor. The offset
// is npos on success; no repair or incomplete JSON value is returned.
[[nodiscard]] std::optional<JsonValue> parseJson(
    std::string_view source, std::string *error,
    JsonNumberPolicy policy, std::size_t &failureOffset);

} // namespace source
} // namespace frog::native_engine
