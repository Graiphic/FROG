#include "FrogDocumentArrayStorage.hpp"
#include "FrogDocumentEncoding.hpp"
#include "FrogDocumentJsonValue.hpp"
#include "FrogDocumentZstdCodec.hpp"
#include "frog_native_engine/BindingType.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <vector>

namespace frog::native_engine::document_internal {
namespace {
constexpr std::size_t chunkBytes = 64U * 1024U;
constexpr std::string_view textProfile = "numeric-cell-text-le32-v1";
constexpr std::string_view elementsProfile = "numeric-elements-json-le32-v1";
using Json = source::JsonValue;
bool validUtf8(std::string_view text) {
    for (std::size_t index = 0; index < text.size();) {
        const auto first = static_cast<unsigned char>(text[index++]);
        if (first < 0x80U) continue;
        const unsigned int extra = first >= 0xc2U && first <= 0xdfU ? 1U :
            first >= 0xe0U && first <= 0xefU ? 2U : first >= 0xf0U && first <= 0xf4U ? 3U : 0U;
        if (extra == 0U || extra > text.size() - index) return false;
        for (unsigned int part = 0; part < extra; ++part) {
            const auto byte = static_cast<unsigned char>(text[index++]);
            if (byte < 0x80U || byte > 0xbfU || (part == 0U &&
                ((first == 0xe0U && byte < 0xa0U) || (first == 0xedU && byte >= 0xa0U) ||
                 (first == 0xf0U && byte < 0x90U) || (first == 0xf4U && byte > 0x8fU)))) return false;
        }
    }
    return true;
}
std::string checksum(std::span<const std::uint8_t> bytes) {
    std::uint32_t crc = 0xffffffffU;
    for (const auto byte : bytes) {
        crc ^= byte;
        for (int bit = 0; bit < 8; ++bit)
            crc = (crc >> 1U) ^ (0xedb88320U & (0U - (crc & 1U)));
    }
    std::ostringstream out;
    out << std::hex << std::setfill('0') << std::setw(8) << (crc ^ 0xffffffffU);
    return out.str();
}
void appendU32(std::vector<std::uint8_t> &out, std::uint32_t value) {
    for (int shift = 0; shift < 32; shift += 8) out.push_back(static_cast<std::uint8_t>(value >> shift));
}
bool u32(const Json &object, std::string_view key, std::uint32_t &out) {
    const auto result = jsonUnsignedField(object, key);
    if (!result) return false;
    out = *result;
    return true;
}
bool decode(const Json &envelope, Json &result, std::string &error,
    std::size_t &remainingBytes, std::size_t &remainingCells,
    std::string_view expectedProfile, std::optional<std::uint32_t> expectedRank) {
    const auto fail = [&](std::string_view why) { error = "FROG array storage: " + std::string(why); return false; };
    const auto profile = jsonStringField(envelope, "encoding").value_or("");
    if (profile != textProfile && profile != elementsProfile) return fail("unsupported encoding '" + profile + "'.");
    if (profile != expectedProfile) return fail("encoding does not match the array value field.");
    std::uint32_t count = 0, byteCount = 0;
    if (!u32(envelope, "count", count) || !u32(envelope, "decoded_bytes", byteCount) ||
        count == 0U || count > remainingCells || byteCount > remainingBytes || byteCount < static_cast<std::uint64_t>(count) * 4U)
        return fail("invalid count or decoded size exceeds the document resource budget.");
    const auto *chunks = objectField(envelope, "chunks");
    if (!chunks || chunks->type != Json::Type::Array ||
        chunks->array.size() != (static_cast<std::size_t>(byteCount) + chunkBytes - 1U) / chunkBytes)
        return fail("invalid chunk count.");
    std::vector<std::uint8_t> bytes;
    bytes.reserve(byteCount);
    for (const auto &chunk : chunks->array) {
        std::uint32_t decoded = 0;
        if (!u32(chunk, "decoded_bytes", decoded) || decoded != (std::min)(chunkBytes, static_cast<std::size_t>(byteCount) - bytes.size()))
            return fail("invalid chunk decoded size.");
        const auto data = jsonStringField(chunk, "data");
        if (!data || data->size() > (chunkBytes + 1024U) * 2U) return fail("invalid base64 chunk size.");
        const auto encoded = base64Decode(*data);
        if (base64Encode(encoded) != *data) return fail("invalid base64 encoding.");
        const auto codec = jsonStringField(chunk, "codec").value_or("");
        std::vector<std::uint8_t> raw(decoded);
        if (codec == "raw") {
            if (encoded.size() != decoded) return fail("raw chunk size mismatch.");
            raw = encoded;
        } else if (codec == "zstd") {
            switch (decodeDocumentZstdChunk(encoded, raw)) {
            case DocumentZstdDecodeResult::InvalidFrameSize:
                return fail("invalid Zstandard frame size.");
            case DocumentZstdDecodeResult::DecoderUnavailable:
                return fail("could not allocate the bounded Zstandard decoder.");
            case DocumentZstdDecodeResult::CorruptChunk:
                return fail("corrupt Zstandard chunk.");
            case DocumentZstdDecodeResult::Ok:
                break;
            }
        } else return fail("unsupported codec '" + codec + "'.");
        if (checksum(raw) != jsonStringField(chunk, "crc32").value_or("")) return fail("chunk CRC32 mismatch.");
        bytes.insert(bytes.end(), raw.begin(), raw.end());
    }
    result.type = Json::Type::Array;
    result.array.reserve(count);
    std::size_t cursor = 0;
    for (std::uint32_t i = 0; i < count; ++i) {
        if (bytes.size() - cursor < 4U) return fail("truncated cell length.");
        std::uint32_t length = 0;
        for (int shift = 0; shift < 32; shift += 8) length |= static_cast<std::uint32_t>(bytes[cursor++]) << shift;
        if (length > kMaximumFrogJsonStringBytes || length > bytes.size() - cursor) return fail("invalid cell length.");
        std::string cell(reinterpret_cast<const char *>(bytes.data() + cursor), length);
        cursor += length;
        Json value;
        if (profile == elementsProfile) {
            auto parsed = source::parseJson(cell, &error);
            if (!parsed || parsed->type != Json::Type::Object || !objectField(*parsed, "indices") ||
                !objectField(*parsed, "value") || objectField(*parsed, "value")->type != Json::Type::Number)
                return fail("invalid numeric element record.");
            const auto *indices = objectField(*parsed, "indices");
            if (indices->type != Json::Type::Array || indices->array.empty() || indices->array.size() > 16U ||
                (expectedRank && indices->array.size() != *expectedRank)) return fail("invalid numeric element indices/rank.");
            for (const auto &index : indices->array)
                if (index.type != Json::Type::Number || !std::isfinite(index.number) || index.number < 0.0 ||
                    index.number > (std::numeric_limits<std::uint32_t>::max)() || index.number != std::trunc(index.number))
                    return fail("invalid numeric element index.");
            for (const auto flag : {"defined", "enabled"})
                if (const auto *field = objectField(*parsed, flag); field && field->type != Json::Type::Boolean)
                    return fail("invalid numeric element flags.");
            if (objectField(*parsed, "kind")) return fail("unexpected numeric element kind.");
            value = std::move(*parsed);
        } else {
            // Numeric authoring text is intentionally exact, including signed
            // zero, high precision, empty cells and partially edited values.
            if (!validUtf8(cell)) return fail("invalid UTF-8 cell text.");
            value.type = Json::Type::String;
            value.string = std::move(cell);
        }
        result.array.push_back(std::move(value));
    }
    if (cursor != bytes.size()) return fail("trailing bytes after the last cell.");
    remainingBytes -= byteCount;
    remainingCells -= count;
    return true;
}
} // namespace

std::string_view arrayStorageModeToken(FrogArrayStorageMode mode) noexcept {
    switch (mode) {
    case FrogArrayStorageMode::Automatic: return "automatic";
    case FrogArrayStorageMode::CompactStandalone: return "compact_standalone";
    default: return "readable";
    }
}
bool compactNumericArrayType(std::string_view type) {
    const auto parsed = binding::parseType(type);
    const auto *value = &parsed.type;
    if (value->kind == binding::TypeKind::Array && value->children.size() == 1U) value = &value->children.front();
    return parsed.valid() && value->kind == binding::TypeKind::Primitive && value->numeric();
}
std::optional<std::string> encodeArrayStorage(std::span<const std::string> cells,
    ArrayStorageWriteContext &context, std::string_view profile, std::size_t readableBytes) {
    const auto mode = context.mode;
    if (mode == FrogArrayStorageMode::Readable || cells.empty()) return std::nullopt;
    if (cells.size() > context.remainingCells) return std::nullopt;
    std::size_t byteCount = 0;
    for (const auto &cell : cells) {
        if (!validUtf8(cell) || cell.size() > kMaximumFrogJsonStringBytes || cell.size() + 4U > kMaximumFrogDocumentBytes - byteCount)
            return std::nullopt; // Keep unsupported authoring data readable.
        byteCount += cell.size() + 4U;
    }
    if (byteCount > context.remainingBytes) return std::nullopt;
    if (mode == FrogArrayStorageMode::Automatic && readableBytes < 4096U) return std::nullopt;
    std::vector<std::uint8_t> bytes;
    bytes.reserve(byteCount);
    for (const auto &cell : cells) {
        appendU32(bytes, static_cast<std::uint32_t>(cell.size()));
        bytes.insert(bytes.end(), cell.begin(), cell.end());
    }
    std::ostringstream out;
    out << "{\"encoding\":\"" << profile << "\",\"count\":" << cells.size()
        << ",\"decoded_bytes\":" << bytes.size() << ",\"chunks\":[";
    for (std::size_t offset = 0; offset < bytes.size(); offset += chunkBytes) {
        const auto raw = std::span(bytes).subspan(offset, (std::min)(chunkBytes, bytes.size() - offset));
        std::vector<std::uint8_t> compressed;
        const bool useCompressed = context.compress && context.compress(raw, compressed);
        if (!useCompressed) compressed.assign(raw.begin(), raw.end());
        if (offset != 0) out << ',';
        out << "{\"codec\":\"" << (useCompressed ? "zstd" : "raw") << "\",\"decoded_bytes\":" << raw.size()
            << ",\"crc32\":\"" << checksum(raw) << "\",\"data\":\"" << base64Encode(compressed) << "\"}";
    }
    out << "]}";
    auto encoded = out.str();
    if (mode == FrogArrayStorageMode::Automatic && encoded.size() >= readableBytes) return std::nullopt;
    context.remainingBytes -= byteCount;
    context.remainingCells -= cells.size();
    return encoded;
}
bool readArrayStorageMode(const Json &root, FrogArrayStorageMode &mode, std::string &error) {
    const auto *storage = objectField(root, "storage");
    if (!storage) return true;
    const auto value = jsonStringField(*storage, "array_mode").value_or("");
    if (jsonUnsignedField(*storage, "version") != 1U) { error = "Unsupported FROG storage version."; return false; }
    if (value == "readable") mode = FrogArrayStorageMode::Readable;
    else if (value == "automatic") mode = FrogArrayStorageMode::Automatic;
    else if (value == "compact_standalone") mode = FrogArrayStorageMode::CompactStandalone;
    else { error = "Unsupported FROG array storage mode '" + value + "'."; return false; }
    return true;
}
bool inflateArrayStorage(Json &root, std::string &error, bool allowCompact) {
    std::size_t remainingBytes = kMaximumFrogDocumentBytes;
    std::size_t remainingCells = kMaximumFrogJsonValueCount;
    std::vector<Json *> pending{&root};
    while (!pending.empty()) {
        auto *value = pending.back(); pending.pop_back();
        if (value->type == Json::Type::Object) {
            for (auto &[key, child] : value->object) {
                const bool arrayField = (key == "values" && objectField(*value, "rank") && objectField(*value, "shape")) ||
                    (key == "elements" && objectField(*value, "dimensions.rank"));
                if (arrayField && child.type == Json::Type::Object) {
                    if (!allowCompact) { error = "FROG compact arrays require private draft_revision 3."; return false; }
                    Json decoded;
                    if (!decode(child, decoded, error, remainingBytes, remainingCells,
                        key == "values" ? textProfile : elementsProfile,
                        key == "elements" ? jsonUnsignedField(*value, "dimensions.rank") : std::nullopt)) return false;
                    child = std::move(decoded);
                }
                pending.push_back(&child);
            }
        } else if (value->type == Json::Type::Array)
            for (auto &child : value->array) pending.push_back(&child);
    }
    return true;
}
} // namespace frog::native_engine::document_internal
