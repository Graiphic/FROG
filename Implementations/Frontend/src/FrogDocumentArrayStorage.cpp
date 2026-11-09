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
std::size_t valueCount(const Json &root) {
    if(root.type!=Json::Type::Object&&root.type!=Json::Type::Array)return 1;
    std::size_t count=1;std::vector<const Json *> pending{&root};
    const auto append=[&](const Json &v){if(++count>kMaximumFrogJsonValueCount)return false;
        if(v.type==Json::Type::Object||v.type==Json::Type::Array)pending.push_back(&v);return true;};
    while(!pending.empty()){const auto *v=pending.back();pending.pop_back();
        for(const auto &[key,child]:v->object)if(!append(child))return count;
        for(const auto &child:v->array)if(!append(child))return count;
    }
    return count;
}
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
    std::size_t cursor = 0, decodedValues=0;
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
        const auto values=valueCount(value);
        if(values>remainingCells-decodedValues)return fail("aggregate decoded JSON value budget exceeded; limit="+std::to_string(kMaximumFrogJsonValueCount)+".");
        decodedValues+=values;result.array.push_back(std::move(value));
    }
    if (cursor != bytes.size()) return fail("trailing bytes after the last cell.");
    remainingBytes -= byteCount;
    remainingCells -= decodedValues;
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
template<class Cell> std::optional<std::string> encodeCells(std::size_t count,Cell cellAt,
    ArrayStorageWriteContext &context, std::string_view profile, std::size_t readableBytes) {
    const auto mode = context.mode;
    if (mode == FrogArrayStorageMode::Readable || count == 0) return std::nullopt;
    if (count > context.remainingCells) return std::nullopt;
    std::size_t byteCount = 0;
    for (std::size_t index=0;index<count;++index) {
        const auto cell=cellAt(index);
        if((index&16383U)==0)source::pollJsonReadCheckpoint();
        if (!validUtf8(cell) || cell.size() > kMaximumFrogJsonStringBytes || cell.size() + 4U > kMaximumFrogDocumentBytes - byteCount)
            return std::nullopt; // Keep unsupported authoring data readable.
        byteCount += cell.size() + 4U;
    }
    if (byteCount > context.remainingBytes) return std::nullopt;
    if (mode == FrogArrayStorageMode::Automatic && readableBytes < 4096U) return std::nullopt;
    // Preserve the byte stream and its fixed chunk boundaries without staging
    // a second complete copy of all cell lengths and payloads.
    std::vector<std::uint8_t> bytes;
    bytes.reserve((std::min)(chunkBytes, byteCount));
    std::ostringstream out;
    out << "{\"encoding\":\"" << profile << "\",\"count\":" << count
        << ",\"decoded_bytes\":" << byteCount << ",\"chunks\":[";
    bool firstChunk = true;
    const auto flush = [&] {
        const auto raw = std::span<const std::uint8_t>(bytes);
        std::vector<std::uint8_t> compressed;
        const bool useCompressed = context.compress && context.compress(raw, compressed);
        if (!useCompressed) compressed.assign(raw.begin(), raw.end());
        if (!firstChunk) out << ',';
        firstChunk = false;
        out << "{\"codec\":\"" << (useCompressed ? "zstd" : "raw") << "\",\"decoded_bytes\":" << raw.size()
            << ",\"crc32\":\"" << checksum(raw) << "\",\"data\":\"" << base64Encode(compressed) << "\"}";
        bytes.clear();
    };
    const auto append = [&](std::span<const std::uint8_t> source) {
        while (!source.empty()) {
            const auto count = (std::min)(chunkBytes - bytes.size(), source.size());
            bytes.insert(bytes.end(), source.begin(), source.begin() + count);
            source = source.subspan(count);
            if (bytes.size() == chunkBytes) flush();
        }
    };
    for (std::size_t index=0;index<count;++index) {
        const auto cell=cellAt(index);
        if((index&16383U)==0)source::pollJsonReadCheckpoint();
        std::array<std::uint8_t, 4> length{};
        const auto size = static_cast<std::uint32_t>(cell.size());
        for (unsigned int byte = 0; byte < length.size(); ++byte)
            length[byte] = static_cast<std::uint8_t>(size >> (byte * 8U));
        append(length);
        append({reinterpret_cast<const std::uint8_t *>(cell.data()), cell.size()});
    }
    if (!bytes.empty()) flush();
    out << "]}";
    auto encoded = out.str();
    if (mode == FrogArrayStorageMode::Automatic && encoded.size() >= readableBytes) return std::nullopt;
    context.remainingBytes -= byteCount;
    context.remainingCells -= count;
    return encoded;
}
std::optional<std::string> encodeArrayStorage(std::span<const std::string> cells,
    ArrayStorageWriteContext &context,std::string_view profile,std::size_t readableBytes) {
    return encodeCells(cells.size(),[&](std::size_t i){return std::string_view{cells[i]};},context,profile,readableBytes);
}
std::size_t boundedDocumentJsonValueCount(const Json &root){return valueCount(root);}
bool compactDraftArrayProvenance(Json &root,std::string &error) {
    ArrayStorageWriteContext context;context.mode=FrogArrayStorageMode::CompactStandalone;context.compress=compressDocumentZstdChunk;
    bool changed=false;std::vector<Json *> pending{&root};
    while(!pending.empty()) {
        auto *node=pending.back();pending.pop_back();
        const auto *type=node->member("value_type"),*configuration=node->member("array_constant");
        if(type&&type->type==Json::Type::String&&compactNumericArrayType(type->string)&&configuration) {
            const auto *values=configuration->member("values");
            if(values&&values->type==Json::Type::Array&&values->array.size()>=4096&&
                std::all_of(values->array.begin(),values->array.end(),[](const Json &v){return v.type==Json::Type::String;})) {
                const auto encoded=encodeCells(values->array.size(),[&](std::size_t i){return std::string_view{values->array[i].string};},context,textProfile,0);
                if(!encoded){error="The retained Array cannot use its existing exact text storage profile.";return false;}
                auto payload=source::parseJson(*encoded,&error,source::JsonNumberPolicy::PreserveLexeme);
                if(!payload)return false;
                node->object.at("array_constant").object.at("values")=std::move(*payload);changed=true;
            }
        }
        if(node->type==Json::Type::Object)for(auto &[key,child]:node->object)
            if(child.type==Json::Type::Object||child.type==Json::Type::Array)pending.push_back(&child);
        // Do not detach an unchanged million-cell scalar vector merely to
        // discover that it has no nested containers.
        const auto &view=std::as_const(*node);
        if(view.type==Json::Type::Array&&std::any_of(view.array.begin(),view.array.end(),[](const Json &v){return v.type==Json::Type::Object||v.type==Json::Type::Array;}))
            for(auto &child:node->array)if(child.type==Json::Type::Object||child.type==Json::Type::Array)pending.push_back(&child);
    }
    if(changed) {
        auto revision=source::parseJson("3");const auto *previous=root.member("draft_revision");
        if(!previous||(previous->type==Json::Type::Number&&previous->string=="2"))root.object.insert_or_assign("draft_revision",std::move(*revision));
        // The declaration describes the existing encoding, not a new storage
        // mode selected by the user. Readable remains their persisted choice.
        if(!root.member("storage")) {
            auto storage=source::parseJson("{\"version\":1,\"array_mode\":\"readable\"}");root.object.emplace("storage",std::move(*storage));
        }
    }
    return changed;
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
    const auto existing=valueCount(root);
    if(existing>kMaximumFrogJsonValueCount){error="FROG array storage: aggregate JSON value budget exceeded.";return false;}
    std::size_t remainingCells = kMaximumFrogJsonValueCount-existing;
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
                    // Replacement removes the encoded envelope's descendants;
                    // retain one slot for the new decoded Array container.
                    remainingCells+=valueCount(child)-1U;
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
