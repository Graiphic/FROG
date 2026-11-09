#include "frog_native_engine/FrogSourceDocument.hpp"

#include <charconv>
#include <utility>
#include <vector>

namespace frog::native_engine {
namespace {

using source::JsonValue;

bool fail(std::string *error, std::string_view message) {
    if (error) error->assign(message);
    return false;
}

bool hasWritableAuthority(const JsonValue &root, std::string *error) {
    if (root.member("format") || root.member("draft_revision") || root.member("frontPanel"))
        return fail(error, "Mixed or private draft envelopes cannot use the public writer; preserve the original source.");
    const auto *version = root.member("spec_version");
    if (!version || !isCurrentPublicFrogSourceVersion(*version))
        return fail(error, "Only public source version 0.1 can be written or edited; preserve the original source.");
    return true;
}

std::optional<std::string> splice(
    std::string_view original, std::size_t begin, std::size_t end,
    std::string_view replacement, std::string *error) {
    if (begin > end || end > original.size()) {
        fail(error, "Source token range is invalid.");
        return std::nullopt;
    }
    const auto retainedBytes = original.size() - (end - begin);
    if (retainedBytes > kMaximumFrogDocumentBytes ||
        replacement.size() > kMaximumFrogDocumentBytes - retainedBytes) {
        fail(error, "Edited FROG source exceeds the maximum supported size of 64 MiB.");
        return std::nullopt;
    }
    std::string result;
    result.reserve(retainedBytes + replacement.size());
    result.append(original.substr(0U, begin));
    result.append(replacement);
    result.append(original.substr(end));
    return result;
}

std::optional<std::string> canonicalBytes(const FrogSourceDocument &document, std::string *error) {
    if (!hasWritableAuthority(document.root(), error)) return std::nullopt;
    const auto &version = *document.section("spec_version");
    return splice(document.originalUtf8(), version.sourceBegin, version.sourceEnd, R"("0.1")", error);
}

// No binary64 projection, normalization or implicit absent/null equivalence.
bool sameValue(const JsonValue &left, const JsonValue &right) {
    if (left.type != right.type) return false;
    switch (left.type) {
    case JsonValue::Type::Null: return true;
    case JsonValue::Type::Boolean: return left.boolean == right.boolean;
    case JsonValue::Type::Number:
    case JsonValue::Type::String: return left.string == right.string;
    case JsonValue::Type::Object: {
        if (left.object.size() != right.object.size()) return false;
        auto other = right.object.begin();
        for (const auto &[key, value] : left.object) {
            if (key != other->first || !sameValue(value, other->second)) return false;
            ++other;
        }
        return true;
    }
    case JsonValue::Type::Array:
        if (left.array.size() != right.array.size()) return false;
        for (std::size_t index = 0; index < left.array.size(); ++index)
            if (!sameValue(left.array[index], right.array[index])) return false;
        return true;
    }
    return false;
}

std::optional<std::vector<std::string>> decodePointer(std::string_view pointer, std::string *error) {
    if (pointer.empty() || pointer.front() != '/') {
        fail(error, "An edit must name a non-root RFC 6901 JSON Pointer beginning with '/'.");
        return std::nullopt;
    }
    if (pointer.size() > kMaximumFrogJsonStringBytes) {
        fail(error, "JSON Pointer exceeds the supported length of 16 MiB.");
        return std::nullopt;
    }
    std::vector<std::string> segments;
    std::size_t begin = 1U;
    while (true) {
        if (segments.size() >= kMaximumFrogJsonNestingDepth) {
            fail(error, "JSON Pointer exceeds the supported nesting depth.");
            return std::nullopt;
        }
        const auto separator = pointer.find('/', begin);
        const auto end = separator == std::string_view::npos ? pointer.size() : separator;
        std::string segment;
        segment.reserve(end - begin);
        for (std::size_t index = begin; index < end; ++index) {
            if (pointer[index] != '~') segment.push_back(pointer[index]);
            else {
                ++index;
                if (index >= end || (pointer[index] != '0' && pointer[index] != '1')) {
                    fail(error, "JSON Pointer contains an invalid '~' escape.");
                    return std::nullopt;
                }
                segment.push_back(pointer[index] == '0' ? '~' : '/');
            }
        }
        segments.push_back(std::move(segment));
        if (separator == std::string_view::npos) break;
        begin = separator + 1U;
    }
    if (segments.front() == "spec_version" || segments.front() == "format" ||
        segments.front() == "draft_revision" || segments.front() == "frontPanel") {
        fail(error, "Transactions cannot change source version or introduce a private draft envelope.");
        return std::nullopt;
    }
    return segments;
}

std::optional<std::size_t> arrayIndex(std::string_view segment) {
    if (segment.empty() || (segment.size() > 1U && segment.front() == '0')) return std::nullopt;
    // from_chars accepts a minus sign for signed types; require only digits
    // explicitly to keep JSON Pointer array syntax independent of that detail.
    for (const char digit : segment)
        if (digit < '0' || digit > '9') return std::nullopt;
    std::size_t result = 0;
    const auto parsed = std::from_chars(segment.data(), segment.data() + segment.size(), result);
    if (parsed.ec != std::errc{} || parsed.ptr != segment.data() + segment.size()) return std::nullopt;
    return result;
}

const JsonValue *child(const JsonValue &parent, const std::string &segment) {
    if (parent.type == JsonValue::Type::Object) {
        const auto found = parent.object.find(segment);
        return found == parent.object.end() ? nullptr : &found->second;
    }
    if (parent.type == JsonValue::Type::Array) {
        const auto index = arrayIndex(segment);
        if (index && *index < parent.array.size()) return &parent.array[*index];
    }
    return nullptr;
}

std::optional<std::string> quotedKey(std::string_view key, std::string *error) {
    std::string quoted = "\"";
    for (const unsigned char byte : key) {
        if (quoted.size() > kMaximumFrogDocumentBytes - 7U) {
            fail(error, "Escaped member key exceeds the supported document size.");
            return std::nullopt;
        }
        if (byte == '"' || byte == '\\') {
            quoted.push_back('\\');
            quoted.push_back(static_cast<char>(byte));
        } else if (byte < 0x20U) {
            constexpr std::string_view hex = "0123456789abcdef";
            quoted += "\\u00";
            quoted.push_back(hex[byte >> 4U]);
            quoted.push_back(hex[byte & 0x0fU]);
        } else quoted.push_back(static_cast<char>(byte));
    }
    quoted.push_back('"');
    return quoted;
}

std::pair<std::size_t, std::size_t> removalRange(const JsonValue &parent, const JsonValue &target) {
    const JsonValue *previous = nullptr;
    const JsonValue *next = nullptr;
    const auto visit = [&](const JsonValue &value) {
        if (value.memberBegin < target.memberBegin &&
            (!previous || value.memberBegin > previous->memberBegin)) previous = &value;
        if (value.memberBegin > target.memberBegin &&
            (!next || value.memberBegin < next->memberBegin)) next = &value;
    };
    if (parent.type == JsonValue::Type::Object)
        for (const auto &[key, value] : parent.object) visit(value);
    else
        for (const auto &value : parent.array) visit(value);
    // Object storage is sorted by key, so locate neighbours by source ranges,
    // not map iteration order. Remove exactly one adjoining separator.
    if (next) return { target.memberBegin, next->memberBegin };
    if (previous) return { previous->sourceEnd, target.sourceEnd };
    return { target.memberBegin, target.sourceEnd };
}

struct ParseBudget {
    std::size_t remaining = kMaximumFrogSourceTransactionParseBytes;

    bool consume(std::size_t bytes, std::string *error) {
        if (bytes > remaining)
            return fail(error, "Source transaction exceeds the cumulative parse budget of 256 MiB; use a smaller batch.");
        remaining -= bytes;
        return true;
    }

    std::optional<JsonValue> parse(std::string_view text, std::string *error) {
        if (!consume(text.size(), error)) return std::nullopt;
        return source::parseJson(text, error, source::JsonNumberPolicy::PreserveLexeme);
    }
};

std::optional<std::string> applyEdit(
    std::string_view original, const JsonValue &root, const FrogSourceEdit &edit,
    ParseBudget &budget, std::string *error) {
    if (edit.kind != FrogSourceEditKind::Set && edit.kind != FrogSourceEditKind::Remove) {
        fail(error, "Unknown source edit operation.");
        return std::nullopt;
    }
    if (!budget.consume(edit.pointer.size(), error)) return std::nullopt;
    const auto segments = decodePointer(edit.pointer, error);
    if (!segments) return std::nullopt;
    const JsonValue *parent = &root;
    for (std::size_t index = 0U; index + 1U < segments->size(); ++index) {
        parent = child(*parent, (*segments)[index]);
        if (!parent) {
            fail(error, "JSON Pointer parent does not exist; intermediate containers are never synthesized.");
            return std::nullopt;
        }
    }
    if (parent->type != JsonValue::Type::Object && parent->type != JsonValue::Type::Array) {
        fail(error, "JSON Pointer parent is not an object or array.");
        return std::nullopt;
    }
    const auto &key = segments->back();
    const auto *current = child(*parent, key);
    if (parent->type == JsonValue::Type::Array && !current && key != "-") {
        fail(error, "Array index must name an existing element; use '-' to append.");
        return std::nullopt;
    }
    if (edit.expectedJson) {
        auto expected = budget.parse(*edit.expectedJson, error);
        if (!expected) return std::nullopt;
        if (!current || !sameValue(*current, *expected)) {
            fail(error, "Source edit conflict: the current JSON value differs from the expected value.");
            return std::nullopt;
        }
    } else if (current) {
        fail(error, "Source edit conflict: the JSON Pointer was expected to be absent.");
        return std::nullopt;
    }

    if (edit.kind == FrogSourceEditKind::Remove) {
        if (!current || !edit.valueJson.empty()) {
            fail(error, "Remove requires an existing expected value and no replacement JSON.");
            return std::nullopt;
        }
        const auto [begin, end] = removalRange(*parent, *current);
        return splice(original, begin, end, {}, error);
    }

    // Parse replacement independently, then reparse the complete candidate to
    // enforce combined depth/value limits and strict UTF-8 for any new key.
    if (!budget.parse(edit.valueJson, error)) return std::nullopt;
    if (current) return splice(original, current->sourceBegin, current->sourceEnd, edit.valueJson, error);

    std::string insertion;
    const bool nonempty = parent->type == JsonValue::Type::Object
        ? !parent->object.empty() : !parent->array.empty();
    if (nonempty) insertion.push_back(',');
    if (parent->type == JsonValue::Type::Object) {
        auto quoted = quotedKey(key, error);
        if (!quoted) return std::nullopt;
        if (quoted->size() > kMaximumFrogDocumentBytes - insertion.size() - 1U) {
            fail(error, "Inserted JSON member exceeds the supported document size.");
            return std::nullopt;
        }
        insertion += *quoted;
        insertion.push_back(':');
    }
    if (edit.valueJson.size() > kMaximumFrogDocumentBytes - insertion.size()) {
        fail(error, "Inserted JSON member exceeds the supported document size.");
        return std::nullopt;
    }
    insertion += edit.valueJson;
    return splice(original, parent->sourceEnd - 1U, parent->sourceEnd - 1U, insertion, error);
}

} // namespace

std::optional<std::string> FrogSourceDocument::serializeCanonical(std::string *error) const {
    if (error) error->clear();
    auto output = canonicalBytes(*this, error);
    if (!output) return std::nullopt;
    if (!source::parseJson(*output, error, source::JsonNumberPolicy::PreserveLexeme)) return std::nullopt;
    return output;
}

std::optional<FrogSourceDocument> FrogSourceDocument::transact(
    std::span<const FrogSourceEdit> edits, std::string *error) const {
    if (error) error->clear();
    if (edits.size() > kMaximumFrogSourceTransactionEdits) {
        fail(error, "Source transaction exceeds the maximum of 256 edits.");
        return std::nullopt;
    }
    auto candidate = canonicalBytes(*this, error);
    if (!candidate) return std::nullopt;
    ParseBudget budget;
    auto tree = budget.parse(*candidate, error);
    if (!tree) return std::nullopt;
    for (std::size_t index = 0U; index < edits.size(); ++index) {
        std::string editError;
        auto next = applyEdit(*candidate, *tree, edits[index], budget, &editError);
        if (next) tree = budget.parse(*next, &editError);
        if (!next || !tree) {
            if (error) *error = "Source transaction edit " + std::to_string(index) + ": " + editError;
            return std::nullopt;
        }
        candidate = std::move(next);
    }
    if (!budget.consume(candidate->size(), error)) return std::nullopt;
    // No observer sees partial edits. The final read builds fresh immutable
    // storage and diagnostic views without claiming semantic validity.
    return read(*candidate, error);
}

} // namespace frog::native_engine
