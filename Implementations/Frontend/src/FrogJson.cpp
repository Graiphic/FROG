#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include "FrogJsonInternal.hpp"
#include "PortableText.hpp"

#include <charconv>
#include <cmath>
#include <limits>
#include <utility>

namespace frog::native_engine::source {
namespace {

// Called only when the parser's frames/references have expired. A later mutable
// access marks the containing branch again, so escaped references force a deep
// copy rather than violating snapshot isolation.
void sealParsedTree(JsonValue &root) {
    if(root.type!=JsonValue::Type::Object&&root.type!=JsonValue::Type::Array)return;
    std::vector<JsonValue *> pending{&root};
    while(!pending.empty()) {
        auto *v=pending.back();pending.pop_back();
        for(auto &[key,child]:v->object)if(child.type==JsonValue::Type::Object||child.type==JsonValue::Type::Array)pending.push_back(&child);
        for(auto &child:v->array)if(child.type==JsonValue::Type::Object||child.type==JsonValue::Type::Array)pending.push_back(&child);
        v->object.sealParsedContainer();v->array.sealParsedContainer();
    }
}

enum class JsonStringPolicy : std::uint8_t {
    StrictUtf8,
    LegacyByteValues
};

std::optional<std::uint32_t> parseJsonUnicodeEscape(
    std::string_view source,
    std::size_t index) {
    if (index + 4U > source.size()) {
        return std::nullopt;
    }

    std::uint32_t value = 0;
    for (std::size_t offset = 0; offset < 4U; ++offset) {
        const int digit = portable_internal::asciiHexDigit(source[index + offset]);
        if (digit < 0) {
            return std::nullopt;
        }
        value = (value << 4U) | static_cast<std::uint32_t>(digit);
    }
    return value;
}

class JsonParser {
public:
    JsonParser(
        std::string_view source,
        JsonNumberPolicy numberPolicy,
        JsonStringPolicy valueStringPolicy = JsonStringPolicy::StrictUtf8)
        : source_(source), numberPolicy_(numberPolicy),
          valueStringPolicy_(valueStringPolicy) {}

    [[nodiscard]] std::optional<JsonValue> parse() {
        if (source_.size() > kMaximumFrogDocumentBytes) {
            error_ = "FROG document exceeds the maximum supported size of 64 MiB.";
            return std::nullopt;
        }
        skipWhitespace();
        auto value = parseValue(0U);
        if (!value.has_value()) {
            return std::nullopt;
        }
        skipWhitespace();
        if (index_ != source_.size()) {
            error_ = "Unexpected trailing content.";
            return std::nullopt;
        }
        sealParsedTree(*value);
        return value;
    }

    [[nodiscard]] const std::string &error() const noexcept {
        return error_;
    }
    [[nodiscard]] std::size_t position() const noexcept { return index_; }

    [[nodiscard]] std::optional<JsonValue> parseAuthoring() {
        if (source_.size() > kMaximumFrogDocumentBytes) return std::nullopt;
        JsonValue root;
        struct Frame { JsonValue *value; bool afterValue = false; bool first = true; };
        std::vector<Frame> pending;
        const auto read = [&](JsonValue &value) {
            skipWhitespace();
            if (index_ == source_.size()) return false;
            const auto begin = index_;
            if (source_[index_] != '{' && source_[index_] != '[') {
                auto scalar = parseValue(0U);
                if (!scalar) return false;
                value = std::move(*scalar);
                return true;
            }
            if (valueCount_ >= kMaximumFrogJsonValueCount) return false;
            ++valueCount_;
            value.type = source_[index_++] == '{' ? JsonValue::Type::Object : JsonValue::Type::Array;
            value.sourceBegin = value.memberBegin = begin;
            pending.push_back({&value});
            return true;
        };
        const auto fail = [&]() -> std::optional<JsonValue> {
            internal::clearJsonTree(root);
            return std::nullopt;
        };
        if (!read(root)) return fail();
        while (!pending.empty()) {
            auto &frame = pending.back();
            auto &parent = *frame.value;
            const bool object = parent.type == JsonValue::Type::Object;
            const char close = object ? '}' : ']';
            skipWhitespace();
            if ((frame.first || frame.afterValue) && consume(close)) {
                parent.sourceEnd = index_;
                pending.pop_back();
                continue;
            }
            if (frame.afterValue && !consume(',')) return fail();
            skipWhitespace();
            // A closing token after a comma is never an empty container.
            if (index_ == source_.size() || source_[index_] == close) return fail();
            frame.first = false;
            frame.afterValue = true;
            if (object) {
                const auto memberBegin = index_;
                auto key = parseString(JsonStringPolicy::StrictUtf8);
                if (!key || !consume(':') || parent.object.contains(*key)) return fail();
                auto &child = parent.object.emplace(std::move(*key), JsonValue{}).first->second;
                if (!read(child)) return fail();
                child.memberBegin = memberBegin;
            } else {
                parent.array.emplace_back();
                if (!read(parent.array.back())) return fail();
            }
        }
        skipWhitespace();
        if (index_ != source_.size()) return fail();
        sealParsedTree(root);
        return std::move(root);
    }

private:
    void skipWhitespace() noexcept {
        while (index_ < source_.size() &&
            portable_internal::kJsonWhitespace.find(source_[index_]) != std::string_view::npos) {
            ++index_;
        }
    }

    [[nodiscard]] bool consume(char expected) noexcept {
        skipWhitespace();
        if (index_ >= source_.size() || source_[index_] != expected) {
            return false;
        }
        ++index_;
        return true;
    }

    [[nodiscard]] std::optional<JsonValue> parseValue(std::size_t nestingDepth) {
        skipWhitespace();
        const auto begin = index_;
        auto value = parseUnspannedValue(nestingDepth);
        if (value) {
            value->sourceBegin = begin;
            value->sourceEnd = index_;
            value->memberBegin = begin;
        }
        return value;
    }

    [[nodiscard]] std::optional<JsonValue> parseUnspannedValue(std::size_t nestingDepth) {
        if((valueCount_&1023U)==0)pollJsonReadCheckpoint();
        if (index_ >= source_.size()) {
            error_ = "Unexpected end of document.";
            return std::nullopt;
        }
        if (valueCount_ >= kMaximumFrogJsonValueCount) {
            error_ = "JSON value count exceeds the supported maximum of " +
                std::to_string(kMaximumFrogJsonValueCount) + ".";
            return std::nullopt;
        }
        ++valueCount_;

        const char ch = source_[index_];
        if (ch == '"') {
            auto value = parseString(valueStringPolicy_);
            if (!value.has_value()) {
                return std::nullopt;
            }
            JsonValue json;
            json.type = JsonValue::Type::String;
            json.string = std::move(value.value());
            return json;
        }
        if (ch == '{') {
            if (nestingDepth >= kMaximumFrogJsonNestingDepth) {
                error_ = "JSON nesting depth exceeds the supported maximum of 128.";
                return std::nullopt;
            }
            return parseObject(nestingDepth + 1U);
        }
        if (ch == '[') {
            if (nestingDepth >= kMaximumFrogJsonNestingDepth) {
                error_ = "JSON nesting depth exceeds the supported maximum of 128.";
                return std::nullopt;
            }
            return parseArray(nestingDepth + 1U);
        }
        if (ch == '-' || (ch >= '0' && ch <= '9')) {
            return parseNumber();
        }
        if (source_.substr(index_, 4) == "true") {
            index_ += 4;
            JsonValue value;
            value.type = JsonValue::Type::Boolean;
            value.boolean = true;
            return value;
        }
        if (source_.substr(index_, 5) == "false") {
            index_ += 5;
            JsonValue value;
            value.type = JsonValue::Type::Boolean;
            value.boolean = false;
            return value;
        }
        if (source_.substr(index_, 4) == "null") {
            index_ += 4;
            return JsonValue{};
        }

        error_ = "Unexpected JSON token.";
        return std::nullopt;
    }

    [[nodiscard]] bool appendStringByte(std::string &result, char value) {
        if (result.size() >= kMaximumFrogJsonStringBytes) {
            error_ = "JSON string exceeds the maximum supported length of 16 MiB.";
            return false;
        }
        result.push_back(value);
        return true;
    }

    [[nodiscard]] bool appendRawUtf8(std::string &result, unsigned char lead) {
        const std::size_t start = index_ - 1U;
        std::size_t length = 0;
        std::uint32_t codepoint = 0;
        std::uint32_t minimumCodepoint = 0;
        if (lead >= 0xC2U && lead <= 0xDFU) {
            length = 2U;
            codepoint = lead & 0x1FU;
            minimumCodepoint = 0x80U;
        } else if (lead >= 0xE0U && lead <= 0xEFU) {
            length = 3U;
            codepoint = lead & 0x0FU;
            minimumCodepoint = 0x800U;
        } else if (lead >= 0xF0U && lead <= 0xF4U) {
            length = 4U;
            codepoint = lead & 0x07U;
            minimumCodepoint = 0x10000U;
        } else {
            error_ = "Invalid UTF-8 in JSON string.";
            return false;
        }
        if (source_.size() - start < length) {
            error_ = "Truncated UTF-8 in JSON string.";
            return false;
        }
        for (std::size_t offset = 1U; offset < length; ++offset) {
            const auto continuation = static_cast<unsigned char>(source_[start + offset]);
            if ((continuation & 0xC0U) != 0x80U) {
                error_ = "Invalid UTF-8 continuation in JSON string.";
                return false;
            }
            codepoint = (codepoint << 6U) | (continuation & 0x3FU);
        }
        if (codepoint < minimumCodepoint || codepoint > 0x10FFFFU ||
            (codepoint >= 0xD800U && codepoint <= 0xDFFFU)) {
            error_ = "Invalid UTF-8 code point in JSON string.";
            return false;
        }
        if (length > kMaximumFrogJsonStringBytes - result.size()) {
            error_ = "JSON string exceeds the maximum supported length of 16 MiB.";
            return false;
        }
        result.append(source_.substr(start, length));
        index_ = start + length;
        return true;
    }

    [[nodiscard]] std::optional<std::string> parseString(JsonStringPolicy stringPolicy) {
        if (index_ >= source_.size() || source_[index_] != '"') {
            error_ = "Expected string.";
            return std::nullopt;
        }
        ++index_;

        std::string result;
        while (index_ < source_.size()) {
            const auto ch = static_cast<unsigned char>(source_[index_++]);
            if (ch == '"') {
                return result;
            }
            if (ch < 0x20U) {
                error_ = "Unescaped control character in JSON string.";
                return std::nullopt;
            }
            if (ch >= 0x80U) {
                const bool appended = stringPolicy == JsonStringPolicy::LegacyByteValues
                    ? appendStringByte(result, static_cast<char>(ch))
                    : appendRawUtf8(result, ch);
                if (!appended) {
                    return std::nullopt;
                }
                continue;
            }
            if (ch != '\\') {
                if (!appendStringByte(result, static_cast<char>(ch))) {
                    return std::nullopt;
                }
                continue;
            }
            if (index_ >= source_.size()) {
                error_ = "Unterminated JSON escape.";
                return std::nullopt;
            }
            const char escaped = source_[index_++];
            switch (escaped) {
            case '"':
            case '\\':
            case '/':
                if (!appendStringByte(result, escaped)) {
                    return std::nullopt;
                }
                break;
            case 'b':
                if (!appendStringByte(result, '\b')) {
                    return std::nullopt;
                }
                break;
            case 'f':
                if (!appendStringByte(result, '\f')) {
                    return std::nullopt;
                }
                break;
            case 'n':
                if (!appendStringByte(result, '\n')) {
                    return std::nullopt;
                }
                break;
            case 'r':
                if (!appendStringByte(result, '\r')) {
                    return std::nullopt;
                }
                break;
            case 't':
                if (!appendStringByte(result, '\t')) {
                    return std::nullopt;
                }
                break;
            case 'u':
            {
                const auto high = parseJsonUnicodeEscape(source_, index_);
                if (!high.has_value()) {
                    error_ = "Invalid unicode escape.";
                    return std::nullopt;
                }
                index_ += 4;

                std::uint32_t codepoint = high.value();
                if (codepoint >= 0xD800U && codepoint <= 0xDBFFU) {
                    if (index_ + 6U > source_.size() ||
                        source_[index_] != '\\' || source_[index_ + 1U] != 'u') {
                        error_ = "Missing unicode surrogate pair.";
                        return std::nullopt;
                    }
                    index_ += 2;
                    const auto low = parseJsonUnicodeEscape(source_, index_);
                    if (!low.has_value() || low.value() < 0xDC00U || low.value() > 0xDFFFU) {
                        error_ = "Invalid unicode surrogate pair.";
                        return std::nullopt;
                    }
                    index_ += 4;
                    codepoint = 0x10000U +
                        (((codepoint - 0xD800U) << 10U) | (low.value() - 0xDC00U));
                } else if (codepoint >= 0xDC00U && codepoint <= 0xDFFFU) {
                    error_ = "Unexpected unicode low surrogate.";
                    return std::nullopt;
                }

                portable_internal::appendUtf8CodePoint(result, codepoint);
                if (result.size() > kMaximumFrogJsonStringBytes) {
                    error_ = "JSON string exceeds the maximum supported length of 16 MiB.";
                    return std::nullopt;
                }
                break;
            }
            default:
                error_ = "Unsupported JSON escape.";
                return std::nullopt;
            }
        }

        error_ = "Unterminated string.";
        return std::nullopt;
    }

    [[nodiscard]] std::optional<JsonValue> parseNumber() {
        const auto start = index_;
        const auto invalid = [&]() -> std::optional<JsonValue> {
            error_ = "Invalid or unrepresentable JSON number.";
            return std::nullopt;
        };
        const auto consumed=numeric_value::scanDecimalNumber(source_.substr(start));
        if(!consumed)return invalid();
        index_+=*consumed;

        JsonValue value;
        value.type = JsonValue::Type::Number;
        value.string.assign(source_.substr(start, index_ - start));
        value.number = std::numeric_limits<double>::quiet_NaN();
        if (numberPolicy_ == JsonNumberPolicy::FiniteDouble) {
            double number = 0.0;
            const auto parsed = std::from_chars(
                value.string.data(), value.string.data() + value.string.size(), number);
            if (parsed.ec != std::errc{} ||
                parsed.ptr != value.string.data() + value.string.size() || !std::isfinite(number)) {
                return invalid();
            }
            value.number = number;
        }
        return value;
    }

    [[nodiscard]] std::optional<JsonValue> parseObject(std::size_t nestingDepth) {
        JsonValue object;
        object.type = JsonValue::Type::Object;
        if (!consume('{')) {
            error_ = "Expected object.";
            return std::nullopt;
        }
        if (consume('}')) {
            return object;
        }

        while (index_ < source_.size()) {
            skipWhitespace();
            const auto memberBegin = index_;
            auto key = parseString(JsonStringPolicy::StrictUtf8);
            if (!key.has_value()) {
                return std::nullopt;
            }
            if (!consume(':')) {
                error_ = "Expected ':' after object key.";
                return std::nullopt;
            }
            if (object.object.contains(key.value())) {
                error_ = "Duplicate JSON object member.";
                return std::nullopt;
            }
            auto value = parseValue(nestingDepth);
            if (!value.has_value()) {
                return std::nullopt;
            }
            value->memberBegin = memberBegin;
            object.object.emplace(std::move(key.value()), std::move(value.value()));
            if (consume('}')) {
                return object;
            }
            if (!consume(',')) {
                error_ = "Expected ',' or '}' in object.";
                return std::nullopt;
            }
            skipWhitespace();
            if (index_ < source_.size() && source_[index_] == '}') {
                error_ = "Trailing comma in JSON object.";
                return std::nullopt;
            }
        }

        error_ = "Unterminated object.";
        return std::nullopt;
    }

    [[nodiscard]] std::optional<JsonValue> parseArray(std::size_t nestingDepth) {
        JsonValue array;
        array.type = JsonValue::Type::Array;
        if (!consume('[')) {
            error_ = "Expected array.";
            return std::nullopt;
        }
        if (consume(']')) {
            return array;
        }

        // Count immediate slots without constructing values. Exact reservation
        // avoids the simultaneous old/new multi-million-value vectors created
        // by geometric growth. This scan never admits grammar or skips parsing.
        std::size_t slots=1,depth=0;bool quoted=false,escaped=false;
        for(std::size_t cursor=index_;cursor<source_.size();++cursor) {
            if((cursor&65535U)==0)pollJsonReadCheckpoint();
            const char ch=source_[cursor];
            if(quoted){if(escaped)escaped=false;else if(ch=='\\')escaped=true;else if(ch=='"')quoted=false;continue;}
            if(ch=='"'){quoted=true;continue;}
            if(ch=='['||ch=='{'){++depth;continue;}
            if(ch==']'||ch=='}'){if(depth==0)break;--depth;continue;}
            if(ch==','&&depth==0&&++slots>kMaximumFrogJsonValueCount)break;
        }
        if(slots>kMaximumFrogJsonValueCount-valueCount_) {
            error_="JSON value count exceeds the supported maximum of "+std::to_string(kMaximumFrogJsonValueCount)+".";
            return std::nullopt;
        }
        array.array.reserve(slots);

        while (index_ < source_.size()) {
            auto value = parseValue(nestingDepth);
            if (!value.has_value()) {
                return std::nullopt;
            }
            array.array.push_back(std::move(value.value()));
            if (consume(']')) {
                return array;
            }
            if (!consume(',')) {
                error_ = "Expected ',' or ']' in array.";
                return std::nullopt;
            }
            skipWhitespace();
            if (index_ < source_.size() && source_[index_] == ']') {
                error_ = "Trailing comma in JSON array.";
                return std::nullopt;
            }
        }

        error_ = "Unterminated array.";
        return std::nullopt;
    }

    std::string_view source_;
    JsonNumberPolicy numberPolicy_;
    JsonStringPolicy valueStringPolicy_;
    std::size_t index_ = 0;
    std::size_t valueCount_ = 0;
    std::string error_;
};

} // namespace

const JsonValue *JsonValue::member(std::string_view name) const noexcept {
    if (type != Type::Object) {
        return nullptr;
    }
    // The legacy map comparator is not transparent. Avoid allocating a key
    // inside this noexcept view operation; source sections have few members.
    for (const auto &[key, value] : object) {
        const auto order = std::string_view(key).compare(name);
        if (order == 0) return &value;
        if (order > 0) break;
    }
    return nullptr;
}

std::optional<JsonValue> parseJson(
    std::string_view source,
    std::string *error,
    JsonNumberPolicy policy) {
    JsonParser parser(source, policy);
    auto value = parser.parse();
    if (error != nullptr) {
        *error = parser.error();
    }
    return value;
}

std::optional<JsonValue> parseJson(std::string_view source,std::string *error,
    JsonNumberPolicy policy,std::size_t &failureOffset) {
    JsonParser parser(source,policy);auto value=parser.parse();
    failureOffset=value?std::string_view::npos:parser.position();
    if(error)*error=parser.error();return value;
}

namespace internal {

void clearJsonTree(JsonValue &value) {
    struct Frame { JsonValue *value; bool visited; };
    std::vector<Frame> pending{{&value, false}};
    while (!pending.empty()) {
        const auto frame = pending.back();
        pending.pop_back();
        if (frame.visited) {
            frame.value->object.clear();
            frame.value->array.clear();
            continue;
        }
        pending.push_back({frame.value, true});
        // Releasing a shared immutable branch must not clone it merely to
        // dismantle this owner's reference. Another owner retains that tree.
        if(frame.value->object.shared())frame.value->object.clear();
        if(frame.value->array.shared())frame.value->array.clear();
        for (auto &[key, child] : frame.value->object) pending.push_back({&child, false});
        for (auto &child : frame.value->array) pending.push_back({&child, false});
    }
}

std::optional<JsonValue> parseAuthoringJsonCandidate(std::string_view source) {
    JsonParser parser(source, JsonNumberPolicy::FiniteDouble);
    return parser.parseAuthoring();
}

std::optional<JsonValue> parseLegacyByteStringDraftCandidate(std::string_view source) {
    JsonParser parser(source, JsonNumberPolicy::FiniteDouble, JsonStringPolicy::LegacyByteValues);
    return parser.parse();
}

} // namespace internal

} // namespace frog::native_engine::source
