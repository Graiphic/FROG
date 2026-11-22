#include "FrogDocumentArrayStorage.hpp"
#include "FrogDocumentEncoding.hpp"
#include "FrogDocumentJsonValue.hpp"
#include "FrogDocumentZstdCodec.hpp"
#include "frog_native_engine/ArrayCapacity.hpp"
#include "frog_native_engine/BindingType.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <type_traits>
#include <vector>

namespace frog::native_engine::document_internal {
namespace {
constexpr std::size_t chunkBytes = 64U * 1024U;
constexpr std::string_view textProfile = "numeric-cell-text-le32-v1";
constexpr std::string_view elementsProfile = "numeric-elements-json-le32-v1";
using Json = source::JsonValue;
bool isNumericWordArrayProfile(std::string_view profile) noexcept {
    return profile==numericWordArrayProfile || profile==exactNarrowNumericWordArrayProfile;
}
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
    static constexpr auto table = [] {
        std::array<std::uint32_t, 256> values{};
        for (std::uint32_t i = 0; i < values.size(); ++i) {
            auto entry = i;
            for (int bit = 0; bit < 8; ++bit)
                entry = (entry >> 1U) ^ (0xedb88320U & (0U - (entry & 1U)));
            values[i] = entry;
        }
        return values;
    }();
    std::uint32_t crc = 0xffffffffU;
    for (const auto byte : bytes) crc = (crc >> 8U) ^ table[(crc ^ byte) & 0xffU];
    std::ostringstream out;
    out << std::hex << std::setfill('0') << std::setw(8) << (crc ^ 0xffffffffU);
    return out.str();
}
bool exactU32(const Json *number,std::uint32_t &out) {
    if(!number||number->type!=Json::Type::Number)return false;
    const auto end=number->string.data()+number->string.size();
    const auto parsed=std::from_chars(number->string.data(),end,out);
    return parsed.ec==std::errc{}&&parsed.ptr==end;
}
bool u32(const Json &object, std::string_view key, std::uint32_t &out) {
    return exactU32(object.member(key),out);
}
std::string_view numericElementType(const binding::TypeParseResult &parsed) {
    const auto *value = &parsed.type;
    if (value->kind == binding::TypeKind::Array && value->children.size() == 1U)
        value = &value->children.front();
    if (!parsed.valid() || value->kind != binding::TypeKind::Primitive || !value->numeric()) return {};
    return value->name;
}
template<std::size_t Width> auto littleEndian(std::uint64_t value) {
    std::array<std::uint8_t, Width> bytes{};
    for (std::size_t i=0;i<Width;++i) bytes[i]=static_cast<std::uint8_t>(value>>(i*8U));
    return bytes;
}
bool readChunk(const Json &chunk,std::size_t expected,std::vector<std::uint8_t> &raw,std::string &error) {
    const auto fail = [&](std::string reason) {error="FROG array storage: "+std::move(reason);return false;};
    std::uint32_t decoded=0;
    if (!u32(chunk,"decoded_bytes",decoded) || decoded!=expected) return fail("invalid chunk decoded size.");
    const auto data=jsonStringField(chunk,"data");
    if (!data || data->size()>(chunkBytes+1024U)*2U) return fail("invalid base64 chunk size.");
    auto encoded=base64Decode(*data);
    if (base64Encode(encoded)!=*data) return fail("invalid base64 encoding.");
    const auto codec=jsonStringField(chunk,"codec").value_or("");
    if (codec=="raw") {
        if (encoded.size()!=decoded) return fail("raw chunk size mismatch.");
        raw=std::move(encoded);
    } else if (codec=="zstd") {
        raw.resize(decoded);
        switch (decodeDocumentZstdChunk(encoded,raw)) {
        case DocumentZstdDecodeResult::InvalidFrameSize: return fail("invalid Zstandard frame size.");
        case DocumentZstdDecodeResult::DecoderUnavailable: return fail("could not allocate the bounded Zstandard decoder.");
        case DocumentZstdDecodeResult::CorruptChunk: return fail("corrupt Zstandard chunk.");
        case DocumentZstdDecodeResult::Ok: break;
        }
    } else return fail("unsupported codec '"+codec+"'.");
    if (checksum(raw)!=jsonStringField(chunk,"crc32").value_or("")) return fail("chunk CRC32 mismatch.");
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
    return !numericElementType(binding::parseType(type)).empty();
}
template<class Emit> std::string encodeChunks(std::size_t count, std::size_t byteCount,
    std::string_view profile, std::string_view scalar, FrogDocumentCompressFn compress, Emit emit) {
    std::vector<std::uint8_t> bytes;
    bytes.reserve((std::min)(chunkBytes, byteCount));
    std::ostringstream out;
    out << "{\"encoding\":\"" << profile << "\",\"count\":" << count << ",\"decoded_bytes\":" << byteCount;
    if (!scalar.empty()) out << ",\"element_type\":\"" << scalar << '"';
    out << ",\"chunks\":[";
    bool firstChunk = true;
    const auto flush = [&] {
        const auto raw = std::span<const std::uint8_t>(bytes);
        std::vector<std::uint8_t> compressed;
        const bool useCompressed = compress && compress(raw, compressed);
        if (!firstChunk) out << ',';
        firstChunk = false;
        out << "{\"codec\":\"" << (useCompressed ? "zstd" : "raw") << "\",\"decoded_bytes\":" << raw.size()
            << ",\"crc32\":\"" << checksum(raw) << "\",\"data\":\""
            << base64Encode(useCompressed ? compressed : bytes) << "\"}";
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
    emit(append);
    if (!bytes.empty()) flush();
    out << "]}";
    return std::move(out).str();
}
template<class Cell> std::optional<std::string> encodeCells(std::size_t count,Cell cellAt,
    ArrayStorageWriteContext &context, std::string_view profile, std::size_t readableBytes,
    bool required = false) {
    const auto mode = context.mode;
    if (mode == FrogArrayStorageMode::Readable || count == 0) return std::nullopt;
    if (count > (required ? kMaximumArrayElements : context.remainingCells)) return std::nullopt;
    if (!required && mode == FrogArrayStorageMode::Automatic && readableBytes < 4096U) return std::nullopt;
    std::size_t byteCount = 0;
    for (std::size_t index=0;index<count;++index) {
        const auto cell=cellAt(index);
        if ((index&16383U)==0) source::pollJsonReadCheckpoint();
        if (!validUtf8(cell) || cell.size() > kMaximumFrogJsonStringBytes || cell.size() + 4U > context.remainingBytes - byteCount)
            return std::nullopt;
        byteCount += cell.size() + 4U;
    }
    auto encoded = encodeChunks(count,byteCount,profile,{},context.compress,[&](const auto &append) {
        for (std::size_t index=0;index<count;++index) {
            if ((index&16383U)==0) source::pollJsonReadCheckpoint();
            const auto cell=cellAt(index);
            append(littleEndian<4>(cell.size()));
            append({reinterpret_cast<const std::uint8_t *>(cell.data()),cell.size()});
        }
    });
    if (!required && mode == FrogArrayStorageMode::Automatic && encoded.size() >= readableBytes) return std::nullopt;
    context.remainingBytes -= byteCount;
    if (!required) context.remainingCells -= count;
    return encoded;
}
std::optional<std::string> encodeNumericWords(const DiagramArrayValues &cells,
    std::string_view scalar, ArrayStorageWriteContext &context) {
    const auto width=DiagramArrayValues::numericWordBytes(scalar);
    if (!width || !cells.hasNumericWords(scalar)) return std::nullopt;
    if (!arrayAllocationFitsBudget(cells.size(),width,0,context.remainingNumericWordsBytes))
        throw std::runtime_error("FROG array storage: numeric words exceed the aggregate decoded byte budget; required=" +
            std::to_string(std::uint64_t(cells.size())*width) + "; remaining=" +
            std::to_string(context.remainingNumericWordsBytes) + ".");
    std::size_t byteCount=cells.size()*width;
    cells.visitNumericSpellings([&](std::size_t,std::string_view spelling) {
        if (spelling.size()>kMaximumFrogJsonStringBytes || !validUtf8(spelling) ||
            spelling.size()+8U>context.remainingNumericWordsBytes-byteCount)
            throw std::runtime_error("FROG array storage: numeric spelling exceeds the compact byte budget or contains invalid text.");
        byteCount+=spelling.size()+8U;
    });
    const auto profile=width<4 ? exactNarrowNumericWordArrayProfile : numericWordArrayProfile;
    auto encoded=encodeChunks(cells.size(),byteCount,profile,scalar,context.compress,[&](const auto &append) {
        static constexpr std::array<std::uint8_t,chunkBytes> zeros{};
        cells.visitNumericWords([&](std::size_t count,auto page) {
            source::pollJsonReadCheckpoint();
            if (page.empty()) append(std::span(zeros).first(count*width));
            else if constexpr (std::endian::native==std::endian::little) {
                const auto *data=reinterpret_cast<const std::uint8_t *>(page.data());
                append(std::span(data,count*width));
            } else {
                for (auto word:page.first(count)) append(littleEndian<sizeof(word)>(word));
            }
        });
        cells.visitNumericSpellings([&](std::size_t index,std::string_view spelling) {
            source::pollJsonReadCheckpoint();
            append(littleEndian<4>(index));
            append(littleEndian<4>(spelling.size()));
            append({reinterpret_cast<const std::uint8_t *>(spelling.data()),spelling.size()});
        });
    });
    context.remainingNumericWordsBytes-=byteCount;
    return encoded;
}
std::optional<std::string> encodeArrayStorage(std::span<const std::string> cells,
    ArrayStorageWriteContext &context,std::string_view profile,std::size_t readableBytes) {
    return encodeCells(cells.size(),[&](std::size_t i){return std::string_view{cells[i]};},context,profile,readableBytes);
}
std::optional<std::string> encodeDiagramArrayStorage(const DiagramArrayValues &cells,
    ArrayStorageWriteContext &context, bool compactable, std::string_view type) {
    const auto count = cells.size();
    constexpr std::size_t largeArrayCells = 4096;
    const bool required = compactable && count != 0 && context.mode != FrogArrayStorageMode::Readable &&
        (count >= largeArrayCells || count > context.readableCells ||
            context.mode == FrogArrayStorageMode::CompactStandalone);
    if (required) {
        const auto parsed=binding::parseType(type);
        if (auto encoded=encodeNumericWords(cells,numericElementType(parsed),context)) return encoded;
        auto encoded = encodeCells(count, [&](std::size_t index) { return cells[index]; }, context,
            textProfile, 0, true);
        if (!encoded)
            throw std::runtime_error("FROG array storage: numeric cells exceed the compact byte budget or contain invalid text.");
        return encoded;
    }
    if (count > context.readableCells)
        throw std::runtime_error("FROG array storage: readable cells exceed the document JSON value budget.");
    context.readableCells -= count;
    return std::nullopt;
}

bool decodeDiagramArrayStorage(const Json &configuration, DiagramArrayValues &out,
    std::string_view type, std::size_t &remainingBytes, std::string &error,
    std::size_t *remainingNumericWordsBytes) {
    const auto fail = [&](std::string_view reason) {
        error = "FROG array storage: " + std::string(reason);
        return false;
    };
    const auto *envelope = objectField(configuration, "values");
    if (!envelope || envelope->type != Json::Type::Object) return fail("invalid numeric Array envelope.");
    const auto profile = jsonStringField(*envelope, "encoding").value_or("");
    if (profile == elementsProfile) return fail("encoding does not match the array value field.");
    const bool words = isNumericWordArrayProfile(profile);
    if (!words && profile != textProfile) return fail("unsupported encoding '" + profile + "'.");
    const auto parsedType = binding::parseType(type);
    const auto elementType = numericElementType(parsedType);
    if (elementType.empty()) return fail("encoding does not match the Array element type.");
    const auto exactWidth=DiagramArrayValues::numericWordBytes(elementType);
    const bool legacyNarrow=profile==numericWordArrayProfile && exactWidth && exactWidth<4;
    const bool signedLane=elementType.starts_with('i');
    const auto width = words && !legacyNarrow ? exactWidth : 4U;
    if (words && (!width || (profile==exactNarrowNumericWordArrayProfile && exactWidth>=4) ||
        jsonStringField(*envelope,"element_type").value_or("")!=elementType))
        return fail("encoding does not match the exact Array element type.");
    auto &budget = words && remainingNumericWordsBytes ? *remainingNumericWordsBytes : remainingBytes;
    std::uint32_t count = 0, byteCount = 0;
    if (!u32(*envelope, "count", count) || !u32(*envelope, "decoded_bytes", byteCount) ||
        !count || byteCount > budget || byteCount < std::uint64_t(count) * width)
        return fail("invalid count or decoded size exceeds the document resource budget.");
    const auto *rawShape=configuration.member("shape");
    std::uint32_t rank=0;
    if(!u32(configuration,"rank",rank)||!rank||rank>16||
       (rawShape&&(rawShape->type!=Json::Type::Array||rawShape->array.size()>rank)))
        return fail("numeric Array rank is invalid.");
    std::vector<std::uint32_t> shape;
    if(rawShape)for(const auto &extent:rawShape->array) {
        std::uint32_t size=0;
        if(!exactU32(&extent,size))return fail("numeric Array has an invalid extent.");
        shape.push_back(size);
    }
    if((!shape.empty()&&(shape.size()!=rank||checkedArrayElementCount(shape)!=count))||
       (shape.empty()&&rank!=1))return fail("numeric Array shape and count disagree.");

    const auto *chunks = objectField(*envelope, "chunks");
    if (!chunks || chunks->type != Json::Type::Array ||
        chunks->array.size() != (std::uint64_t(byteCount) + chunkBytes - 1) / chunkBytes)
        return fail("invalid chunk count.");
    DiagramArrayValues prepared;
    const bool typed = prepared.resizeNumeric(count, elementType);
    if (!typed && count > kMaximumFrogJsonValueCount)
        return fail("the numeric representation exceeds the text cell budget.");
    std::vector<std::string> textCells;
    if (!typed) textCells.reserve(count);
    std::uint32_t index = 0, length = 0;
    unsigned lengthBytes = 0;
    std::size_t bytesRead = 0;
    std::string cell;
    std::vector<std::uint8_t> raw;
    const auto wordsBytes=static_cast<std::size_t>(count)*width;
    std::array<std::uint8_t,8> spellingHeader{};
    std::optional<std::uint32_t> previousSpelling;
    for (const auto &chunk : chunks->array) {
        source::pollJsonReadCheckpoint();
        if (!readChunk(chunk,(std::min)(chunkBytes,std::size_t(byteCount)-bytesRead),raw,error)) return false;
        std::size_t cursor = 0;
        if (words && bytesRead < wordsBytes) {
            cursor=(std::min)(raw.size(),wordsBytes-bytesRead);
            if (legacyNarrow) {
                // v1 wrote narrow integer cells in four-byte lanes. Preserve
                // old out-of-range authoring text so execution can refuse it.
                for (std::size_t offset=0;offset<cursor;offset+=4) {
                    std::uint32_t word=0;
                    for (unsigned byte=0;byte<4;++byte) word|=std::uint32_t(raw[offset+byte])<<(8*byte);
                    if (word) prepared.replaceAt((bytesRead+offset)/4,signedLane ?
                        std::to_string(std::bit_cast<std::int32_t>(word)) : std::to_string(word));
                }
            } else if (!prepared.loadNumericWords(bytesRead/width,std::span(raw).first(cursor))) {
                return fail("invalid numeric word block.");
            }
        }
        bytesRead += raw.size();
        while (cursor < raw.size()) {
            if (!words && index == count) return fail("trailing bytes after the last cell.");
            const auto headerSize=words ? 8U : 4U;
            while (lengthBytes < headerSize && cursor < raw.size()) spellingHeader[lengthBytes++]=raw[cursor++];
            if (lengthBytes != headerSize) break;
            if (words) {
                index=0;
                for (unsigned byte=0;byte<4;++byte) index|=std::uint32_t(spellingHeader[byte])<<(byte*8U);
                if (index>=count || (previousSpelling && index<=*previousSpelling))
                    return fail("invalid or unordered numeric spelling index.");
            }
            for (unsigned byte=0;byte<4;++byte) length|=std::uint32_t(spellingHeader[headerSize-4U+byte])<<(byte*8U);
            if (length > kMaximumFrogJsonStringBytes) return fail("invalid cell length.");
            const auto take = (std::min)(std::size_t(length) - cell.size(), raw.size() - cursor);
            cell.append(reinterpret_cast<const char *>(raw.data() + cursor), take);
            cursor += take;
            if (cell.size() != length) break;
            if (!validUtf8(cell)) return fail("invalid UTF-8 cell text.");
            if (words) {
                if (legacyNarrow&&DiagramArrayValues::numericWordFromToken(signedLane?"i32":"u32",cell,true))
                    return fail("numeric spelling conflicts with its word.");
                if (!prepared.loadNumericSpelling(index,std::move(cell))) return fail("numeric spelling conflicts with its word.");
                previousSpelling=index;
            } else if (typed) {
                if (cell != "0") prepared.replaceAt(index, std::move(cell));
            } else textCells.push_back(std::move(cell));
            if (!words) ++index;
            cell.clear();
            length = 0;
            lengthBytes = 0;
        }
    }
    if ((!words && index != count) || lengthBytes != 0) return fail("truncated cell data.");
    if (!typed) prepared = std::move(textCells);
    budget -= byteCount;
    out.swap(prepared);
    return true;
}
bool isPackedExecutionArrayEnvelope(const Json &configuration,std::string_view type) {
    const auto *payload=configuration.member("values");
    std::uint32_t rank=0;
    if(!payload||!isNumericWordArrayProfile(jsonStringField(*payload,"encoding").value_or(""))||
        !u32(configuration,"rank",rank)||!rank||rank>16)
        return false;
    const auto schema=binding::parseType(type);
    const auto element=numericElementType(schema);
    return DiagramArrayValues::numericWordBytes(element)!=0&&
        (schema.type.kind==binding::TypeKind::Primitive||schema.type.rank==rank);
}

bool decodeComposedPackedNumericArray(const Json &value,const binding::TypeRef &type,
    DiagramArrayValues &cells,std::vector<std::uint32_t> &extents,
    std::vector<std::pair<std::size_t,std::uint64_t>> &spellings,
    std::size_t &remainingWords,std::string &error) {
    const auto *shape=value.member("shape"),*payload=value.member(composedNumericArrayField);
    const auto fail=[&](std::string reason){error=std::move(reason);return false;};
    const auto element=type.children.size()==1&&type.children[0].kind==binding::TypeKind::Primitive?
        std::string_view(type.children[0].name):std::string_view{};
    if(!payload||!shape||shape->type!=Json::Type::Array||shape->array.size()!=type.rank||
       type.kind!=binding::TypeKind::Array||type.rank<1||type.rank>16||value.object.size()!=2||
       !DiagramArrayValues::numericWordBytes(element))
        return fail("Packed numeric literals require explicit extents and an exact primitive numeric Array schema.");
    std::vector<std::uint32_t> decodedExtents;
    decodedExtents.reserve(type.rank);
    for (const auto &extent:shape->array) {
        std::uint32_t size=0;
        if(!exactU32(&extent,size))return fail("Packed numeric literals require exact Array extents.");
        decodedExtents.push_back(size);
    }
    Json configuration;
    configuration.type=Json::Type::Object;
    Json rank;rank.type=Json::Type::Number;rank.number=type.rank;rank.string=std::to_string(type.rank);
    configuration.object={{"rank",std::move(rank)},{"shape",*shape},{"values",*payload}};
    if(!isPackedExecutionArrayEnvelope(configuration,type.canonical()))return fail("Invalid packed numeric Array encoding.");
    auto unusedTextBudget=kMaximumFrogDocumentBytes;
    auto availableWords=remainingWords;
    DiagramArrayValues decoded;
    if(!decodeDiagramArrayStorage(configuration,decoded,type.canonical(),unusedTextBudget,error,&availableWords))return false;
    const bool single=element=="f32",real=single||element=="f64";
    const auto finiteWord=[&](std::uint64_t bits) {
        return !real||(single?std::isfinite(std::bit_cast<float>(static_cast<std::uint32_t>(bits))):
            std::isfinite(std::bit_cast<double>(bits)));
    };
    bool finite=true;
    if(real)decoded.visitNumericWords([&](std::size_t count,auto words){
        source::pollJsonReadCheckpoint();
        if(finite&&!words.empty())finite=std::all_of(words.begin(),words.begin()+count,finiteWord);
    });
    if(!finite)return fail("A packed Array literal contains a non-finite cell.");
    std::vector<std::pair<std::size_t,std::uint64_t>> decodedSpellings;
    decoded.visitNumericSpellings([&](std::size_t index,std::string_view spelling){
        if(!finite)return;
        source::pollJsonReadCheckpoint();
        auto token=source::parseJson(spelling,nullptr,source::JsonNumberPolicy::PreserveLexeme);
        const auto number=token&&(token->type==Json::Type::Number||token->type==Json::Type::String)?
            DiagramArrayValues::numericWordFromToken(element,token->string):std::nullopt;
        if(!number||!finiteWord(*number)) {error="Packed Array cell "+std::to_string(index)+" is outside its exact numeric representation.";finite=false;return;}
        decodedSpellings.emplace_back(index,*number);
    });
    if(!finite)return false;
    cells.swap(decoded);
    extents=std::move(decodedExtents);
    spellings=std::move(decodedSpellings);
    remainingWords=availableWords;
    return true;
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
        // The derived encoding must retain the source document's storage policy.
        if(!root.member("storage")) {
            auto storage=source::parseJson("{\"version\":1,\"array_mode\":\"automatic\"}");root.object.emplace("storage",std::move(*storage));
        }
    }
    return changed;
}
bool readArrayStorageMode(const Json &root, FrogArrayStorageMode &mode, std::string &error) {
    const auto *storage = objectField(root, "storage");
    if (!storage) {
        mode = FrogArrayStorageMode::Automatic;
        return true;
    }
    const auto value = jsonStringField(*storage, "array_mode").value_or("");
    // PreserveLexeme deliberately leaves the binary64 member as NaN.
    // Read this small version ordinal without casting that sentinel, keeping
    // the existing integral JSON spellings 1, 1.0 and 1e0 compatible.
    const auto *version = storage->member("version");
    double versionNumber = std::numeric_limits<double>::quiet_NaN();
    if (version && version->type == Json::Type::Number) {
        versionNumber = version->number;
        if (!std::isfinite(versionNumber)) {
            const auto end = version->string.data() + version->string.size();
            const auto parsed = std::from_chars(version->string.data(), end, versionNumber);
            if (parsed.ec != std::errc{} || parsed.ptr != end)
                versionNumber = std::numeric_limits<double>::quiet_NaN();
        }
    }
    if (versionNumber != 1.0) { error = "Unsupported FROG storage version."; return false; }
    if (value == "readable") mode = FrogArrayStorageMode::Readable;
    else if (value == "automatic") mode = FrogArrayStorageMode::Automatic;
    else if (value == "compact_standalone") mode = FrogArrayStorageMode::CompactStandalone;
    else { error = "Unsupported FROG array storage mode '" + value + "'."; return false; }
    return true;
}
static bool inflateArrayStorageImpl(Json &root, std::string &error, bool allowCompact, bool preserveDiagramCells,
    std::size_t *remainingDecodedBytes, bool *keptPackedArrays) {
    if(keptPackedArrays)*keptPackedArrays=false;
    std::size_t remainingBytes = kMaximumFrogDocumentBytes;
    std::size_t remainingWords = maximumDecodedNumericWordsBytes;
    const auto existing=valueCount(root);
    if(existing>kMaximumFrogJsonValueCount){error="FROG array storage: aggregate JSON value budget exceeded.";return false;}
    std::size_t remainingCells = kMaximumFrogJsonValueCount-existing;
    std::vector<Json *> pending{&root};
    while (!pending.empty()) {
        auto *value = pending.back(); pending.pop_back();
        if (value->type == Json::Type::Object) {
            Json *retainedConfiguration=nullptr;
            if (!preserveDiagramCells) {
                const auto *configuration=objectField(*value,"array_constant");
                const auto *envelope=configuration ? objectField(*configuration,"values") : nullptr;
                if (envelope && envelope->type==Json::Type::Object &&
                    isNumericWordArrayProfile(jsonStringField(*envelope,"encoding").value_or(""))) {
                    if (!allowCompact) {error="FROG compact arrays require private draft_revision 3.";return false;}
                    const auto type=jsonStringField(*value,"value_type").value_or("");
                    if(keptPackedArrays&&isPackedExecutionArrayEnvelope(*configuration,type)) {
                        *keptPackedArrays=true;
                        retainedConfiguration=&value->object.at("array_constant");
                    } else {
                    const auto count=jsonUnsignedField(*envelope,"count");
                    const auto available=remainingCells+valueCount(*envelope)-1U;
                    if (count && *count>available) {error=executionArrayStorageBudgetError;return false;}
                    DiagramArrayValues cells;
                    if (!decodeDiagramArrayStorage(*configuration,cells,type,remainingBytes,error,&remainingWords)) return false;
                    Json decoded;
                    decoded.type=Json::Type::Array;
                    decoded.array.reserve(cells.size());
                    std::size_t i=0;
                    for (auto cell:cells) {
                        if ((i++&16383U)==0) source::pollJsonReadCheckpoint();
                        Json entry;
                        entry.type=Json::Type::String;
                        entry.string=std::move(cell);
                        decoded.array.push_back(std::move(entry));
                    }
                    remainingCells=available-cells.size();
                    value->object.at("array_constant").object.at("values")=std::move(decoded);
                    }
                }
            }
            for (auto &[key, child] : value->object) {
                if(&child==retainedConfiguration)continue;
                const bool arrayField = (key == "values" && objectField(*value, "rank") && objectField(*value, "shape")) ||
                    (key == "elements" && objectField(*value, "dimensions.rank"));
                if (arrayField && child.type == Json::Type::Object) {
                    if (!allowCompact) { error = "FROG compact arrays require private draft_revision 3."; return false; }
                    if (preserveDiagramCells && key == "values") continue;
                    std::uint32_t count = 0;
                    std::uint32_t bytes = 0;
                    if (key == "values" && u32(child, "count", count) && count > kMaximumFrogJsonValueCount &&
                        u32(child, "decoded_bytes", bytes) && std::uint64_t(count) * 4 <= bytes &&
                        bytes <= remainingBytes && jsonStringField(child, "encoding") == textProfile) {
                        error = executionArrayStorageBudgetError;
                        return false;
                    }
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
    if (remainingDecodedBytes) *remainingDecodedBytes = remainingBytes;
    return true;
}
bool inflateArrayStorage(Json &root,std::string &error,bool allowCompact,bool preserveDiagramCells,std::size_t *remainingDecodedBytes) {
    return inflateArrayStorageImpl(root,error,allowCompact,preserveDiagramCells,remainingDecodedBytes,nullptr);
}
bool inflateExecutionArrayStorage(Json &root,std::string &error,bool allowCompact,bool &keptPackedArrays) {
    return inflateArrayStorageImpl(root,error,allowCompact,false,nullptr,&keptPackedArrays);
}
} // namespace frog::native_engine::document_internal
