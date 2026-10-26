#pragma once

#include "frog/sdk/runtime_compound_value.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace frog::sdk::runtime {

struct ArtifactDecodeResult {
    std::optional<ProgramArtifact> artifact;
    std::vector<Diagnostic> diagnostics;
};

namespace codec_detail {

inline constexpr std::array<std::uint8_t, 8> kMagic{
    'F', 'R', 'O', 'G', 'A', 'R', 'T', '1'
};
inline constexpr std::array<std::uint8_t, 8> kTypedMagic{
    'F', 'R', 'O', 'G', 'A', 'R', 'T', '2'
};
inline constexpr std::array<std::uint8_t, 8> kPackedF64ArrayMagic{
    'F', 'R', 'O', 'G', 'A', 'R', 'T', '3'
};
inline constexpr std::uint32_t kMaximumCollectionSize = 1'000'000U;
inline constexpr std::uint32_t kMaximumStringSize = 16U * 1024U * 1024U;
inline constexpr std::size_t kMaximumTypedEnvelopeSize = 32U * 1024U * 1024U;

class Writer final {
public:
    explicit Writer(bool typed = false, bool packed = false, const RuntimeValueOwner *arrayOwner = nullptr)
        : typed_(typed || packed), packed_(packed), arrayOwner_(arrayOwner) {}
    void bytes(std::span<const std::uint8_t> value) {
        if (!valid_ || (typed_ && !arrayDepth_ && value.size() > kMaximumTypedEnvelopeSize - (data_.size()-arrayEnvelopeBytes_))) { valid_ = false; return; }
        data_.insert(data_.end(), value.begin(), value.end());
    }

    template <typename Unsigned>
    void unsignedInteger(Unsigned value) {
        static_assert(std::is_unsigned_v<Unsigned>);
        if (!valid_ || (typed_ && !arrayDepth_ && sizeof(Unsigned) > kMaximumTypedEnvelopeSize - (data_.size()-arrayEnvelopeBytes_))) { valid_ = false; return; }
        for (std::size_t index = 0; index < sizeof(Unsigned); ++index) {
            data_.push_back(static_cast<std::uint8_t>(value & 0xffU));
            value >>= 8U;
        }
    }

    template <typename Enum>
    void enumeration(Enum value) {
        unsignedInteger(static_cast<std::uint8_t>(value));
    }

    void boolean(bool value) { unsignedInteger<std::uint8_t>(value ? 1U : 0U); }

    void string(std::string_view value) {
        unsignedInteger(static_cast<std::uint32_t>(value.size()));
        bytes(std::span(
            reinterpret_cast<const std::uint8_t *>(value.data()), value.size()));
    }

    template <typename Id>
    void stableId(const Id &id) { string(id.value); }

    void valueType(const ValueType &type) {
        string(type.canonicalName);
        enumeration(type.category);
        unsignedInteger(type.bitWidth);
        unsignedInteger(type.rank);
        string(type.elementType);
    }

    void runtimeValue(const RuntimeValue &value, unsigned depth = 0) {
        if (typed_ && (depth > 32 || ++valueNodes_ > 4096)) { valid_ = false; return; }
        // Local typed IPE extension: new explicit tags leave every historical
        // encoding unchanged. Older readers reject these tags; a live opaque
        // reference is deliberately never serialized.
        std::uint8_t tag=static_cast<std::uint8_t>(value.index());
        if(runtimeArray(value))tag=8;else if(runtimeVariant(value))tag=9;
        else if(runtimeMap(value))tag=10;else if(runtimeTimestamp(value))tag=11;else if(runtimeSet(value))tag=12;
        enumeration(tag);
        switch (tag) {
        case 0U: break;
        case 1U: boolean(std::get<bool>(value)); break;
        case 2U:
            unsignedInteger(static_cast<std::uint64_t>(std::get<std::int64_t>(value)));
            break;
        case 3U: unsignedInteger(std::get<std::uint64_t>(value)); break;
        case 4U:
            unsignedInteger(std::bit_cast<std::uint64_t>(std::get<double>(value)));
            break;
        case 5U: {
            if(packed_) { valid_=false; return; }
            const auto &text = std::get<std::string>(value);
            if (typed_ && (text.size() > 1'000'000 || text.size() > 16'000'000 - valueBytes_)) { valid_ = false; return; }
            valueBytes_ += typed_ ? text.size() : 0;
            string(text); break;
        }
        case 6U: {
            const auto &record = std::get<6>(value);
            if (!typed_ || packed_ || !record || record->isReference() ||
                !runtimeValueRepresentable(value, valueTypeFromCanonicalName(record->canonicalType()))) { valid_ = false; return; }
            string(record->canonicalType());
            unsignedInteger(static_cast<std::uint32_t>(record->fields().size()));
            for (const auto &field : record->fields()) {
                string(field.id); valueType(field.type); runtimeValue(field.value, depth + 1);
            }
            break;
        }
        case 7U: {
            const auto &array=std::get<7>(value);
            if(!packed_ || depth != 0 || !array || !arrayOwner_ || !array->validForOwner(*arrayOwner_) ||
                array->payloadBytes()>kMaximumF64ArrayPayloadBytes-valueBytes_) {valid_=false;return;}
            valueBytes_+=array->payloadBytes();
            unsignedInteger<std::uint8_t>(1);
            unsignedInteger(static_cast<std::uint32_t>(array->elements().size()));
            const auto begin=data_.size();++arrayDepth_;
            for(double element:array->elements())unsignedInteger(std::bit_cast<std::uint64_t>(element));
            --arrayDepth_;if(!arrayDepth_)arrayEnvelopeBytes_+=data_.size()-begin;
            break;
        }
        case 8U: case 9U: case 10U: case 11U: case 12U: {
            const auto &carrier=*std::get<6>(value);
            if(!typed_||packed_||!runtimeValueRepresentable(value,valueTypeFromCanonicalName(carrier.canonicalType()))||
                (tag!=11&&(!arrayOwner_||!runtimeValueValidForOwner(value,valueTypeFromCanonicalName(carrier.canonicalType()),*arrayOwner_)))) {valid_=false;return;}
            if(tag==8) {
                const auto &a=*runtimeArray(value);valueType(a.elementType());unsignedInteger(static_cast<std::uint32_t>(a.shape().size()));
                for(const auto extent:a.shape())unsignedInteger(extent);
                unsignedInteger(static_cast<std::uint32_t>(a.elements().size()));
                const auto begin=data_.size(),outerNodes=valueNodes_,outerBytes=valueBytes_;++arrayDepth_;
                for(const auto &v:a.elements()){valueNodes_=0;valueBytes_=0;runtimeValue(v,depth+1);}
                --arrayDepth_;valueNodes_=outerNodes;valueBytes_=outerBytes;
                if(!arrayDepth_)arrayEnvelopeBytes_+=data_.size()-begin;
            }else if(tag==9) {
                const auto &v=*runtimeVariant(value);valueType(v.payloadType());string(v.payloadSchemaJson());runtimeValue(v.payload(),depth+1);
                unsignedInteger(static_cast<std::uint32_t>(v.attributes().size()));
                for(const auto &field:v.attributes()){string(field.id);valueType(field.type);runtimeValue(field.value,depth+1);}
            }else if(tag==10) {
                const auto &v=*runtimeMap(value);valueType(v.keyType());valueType(v.valueType());unsignedInteger(static_cast<std::uint32_t>(v.entries().size()));
                for(const auto &[key,item]:v.entries()){runtimeValue(key,depth+1);runtimeValue(item,depth+1);}
            }else if(tag==12) {
                const auto &v=*runtimeSet(value);valueType(v.elementType());unsignedInteger(static_cast<std::uint32_t>(v.elements().size()));
                for(const auto &element:v.elements())runtimeValue(element,depth+1);
            }else {const auto &v=*runtimeTimestamp(value);unsignedInteger(static_cast<std::uint64_t>(v.seconds()));unsignedInteger(v.fraction());}
            break;
        }
        default: valid_ = false; break;
        }
    }

    // Mixed private metadata may contain ordinary typed values and the
    // historical packed-F64 root carrier. Select only this root's existing
    // codec profile; owner, depth, node and byte counters remain cumulative.
    // The caller must seal/write the selected profile beside the value.
    [[nodiscard]] bool runtimeValueWithProfile(const RuntimeValue &value,bool packed) {
        if(!typed_) {valid_=false;return false;}
        const bool previous=packed_;packed_=packed;runtimeValue(value);packed_=previous;
        return valid_;
    }

    [[nodiscard]] std::vector<std::uint8_t> finish() && {
        return valid_ ? std::move(data_) : std::vector<std::uint8_t>{};
    }

private:
    std::vector<std::uint8_t> data_;
    bool typed_ = false, packed_ = false, valid_ = true;
    const RuntimeValueOwner *arrayOwner_ = nullptr;
    std::size_t valueNodes_ = 0, valueBytes_ = 0;
    std::size_t arrayEnvelopeBytes_ = 0;
    unsigned arrayDepth_ = 0;
};

class Reader final {
public:
    explicit Reader(std::span<const std::uint8_t> data) : data_(data) {}
    bool typed(bool enabled, bool packed = false) {
        typed_ = enabled || packed; packed_=packed;
        // Array payload bytes are charged separately while parsing; metadata
        // and non-Array values retain the existing aggregate envelope bound.
        return true;
    }

    void arrayOwner(const RuntimeValueOwner &owner) noexcept { arrayOwner_=&owner; }

    [[nodiscard]] bool bytes(std::span<std::uint8_t> target) {
        if (!reserve(target.size())) return false;
        std::copy_n(data_.data() + cursor_, target.size(), target.data());
        cursor_ += target.size();
        return true;
    }

    template <typename Unsigned>
    [[nodiscard]] bool unsignedInteger(Unsigned &value) {
        static_assert(std::is_unsigned_v<Unsigned>);
        if (!reserve(sizeof(Unsigned))) return false;
        value = 0U;
        for (std::size_t index = 0; index < sizeof(Unsigned); ++index) {
            value |= static_cast<Unsigned>(data_[cursor_++]) << (index * 8U);
        }
        return true;
    }

    template <typename Enum>
    [[nodiscard]] bool enumeration(Enum &value, std::uint8_t maximum) {
        std::uint8_t raw = 0U;
        if (!unsignedInteger(raw)) return false;
        if (raw > maximum) return fail("The artifact contains an invalid enumeration value.");
        value = static_cast<Enum>(raw);
        return true;
    }

    [[nodiscard]] bool boolean(bool &value) {
        std::uint8_t raw = 0U;
        if (!unsignedInteger(raw)) return false;
        if (raw > 1U) return fail("The artifact contains an invalid Boolean value.");
        value = raw != 0U;
        return true;
    }

    [[nodiscard]] bool string(std::string &value) {
        std::uint32_t size = 0U;
        if (!unsignedInteger(size)) return false;
        if (size > kMaximumStringSize || !reserve(size)) {
            return fail("The artifact contains an invalid string length.");
        }
        value.assign(reinterpret_cast<const char *>(data_.data() + cursor_), size);
        cursor_ += size;
        return true;
    }

    template <typename Id>
    [[nodiscard]] bool stableId(Id &id) { return string(id.value); }

    [[nodiscard]] bool count(std::uint32_t &value) {
        if (!unsignedInteger(value)) return false;
        if (value > kMaximumCollectionSize) {
            return fail("The artifact collection exceeds the contract limit.");
        }
        return true;
    }

    [[nodiscard]] bool valueType(ValueType &type) {
        return string(type.canonicalName) &&
            enumeration(type.category, static_cast<std::uint8_t>(typed_ ? ValueCategory::Set : ValueCategory::Array)) &&
            unsignedInteger(type.bitWidth) && unsignedInteger(type.rank) &&
            string(type.elementType);
    }

    [[nodiscard]] bool runtimeValue(RuntimeValue &value, unsigned depth = 0) {
        if (typed_ && (depth > 32 || ++valueNodes_ > 4096)) return fail("Typed Runtime values exceed their recursive limit.");
        std::uint8_t tag = 0U;
        if (!unsignedInteger(tag)) return false;
        switch (tag) {
        case 0U: value = std::monostate{}; return true;
        case 1U: {
            bool decoded = false;
            if (!boolean(decoded)) return false;
            value = decoded;
            return true;
        }
        case 2U: {
            std::uint64_t decoded = 0U;
            if (!unsignedInteger(decoded)) return false;
            value = static_cast<std::int64_t>(decoded);
            return true;
        }
        case 3U: {
            std::uint64_t decoded = 0U;
            if (!unsignedInteger(decoded)) return false;
            value = decoded;
            return true;
        }
        case 4U: {
            std::uint64_t decoded = 0U;
            if (!unsignedInteger(decoded)) return false;
            value = std::bit_cast<double>(decoded);
            return true;
        }
        case 5U: {
            if(packed_)return fail("String values are outside local schema3 packed Arrays.");
            std::string decoded;
            if (!string(decoded)) return false;
            if (typed_ && (decoded.size() > 1'000'000 || decoded.size() > 16'000'000 - valueBytes_))
                return fail("Typed Runtime strings exceed their aggregate limit.");
            valueBytes_ += typed_ ? decoded.size() : 0;
            value = std::move(decoded);
            return true;
        }
        case 6U: {
            if(packed_)return fail("Record values are outside local schema3 packed Arrays.");
            if (!typed_) return fail("Record values require artifact schema v2.");
            std::string canonical; std::uint32_t size = 0;
            if (!string(canonical) || canonical.size() > 65536 || !count(size) || size > 64)
                return fail("The record encoding has an invalid type or field count.");
            std::vector<RuntimeNamedField> fields;
            fields.reserve(size);
            for (std::uint32_t index = 0; index < size; ++index) {
                RuntimeNamedField field;
                if (!string(field.id) || !valueType(field.type) || !runtimeValue(field.value, depth + 1)) return false;
                fields.push_back(std::move(field));
            }
            auto record = makeRuntimeRecord(std::move(fields));
            if (!record || std::get<6>(*record)->canonicalType() != canonical)
                return fail("Record fields do not match their canonical type.");
            value = std::move(*record);
            return true;
        }
        case 7U: {
            if(!packed_ || depth != 0 || !arrayOwner_ || !runtimeValueOwnerValid(*arrayOwner_))
                return fail("Packed Array values require local schema3 and its exact owner.");
            std::uint8_t rank=0;std::uint32_t count=0;
            if(!unsignedInteger(rank) || !unsignedInteger(count) || rank != 1 || count>kMaximumF64ArrayElements)
                return fail("Packed Array rank/count exceeds the exact rank1 f64 profile.");
            if(count>(std::numeric_limits<std::size_t>::max)()/sizeof(double))return fail("Not enough memory to complete this operation.");
            const auto payload=static_cast<std::size_t>(count)*sizeof(double);
            const auto begin=cursor_;++arrayDepth_;
            if(payload>kMaximumF64ArrayPayloadBytes-valueBytes_ || !reserve(payload))
                return fail("Packed Array payload is truncated or exceeds addressable memory.");
            // Charge BEFORE allocation. The decoder alone can move its private
            // validated buffer into the sealed carrier, without an8MB copy.
            valueBytes_+=payload;
            try {
                std::vector<double> decoded;decoded.reserve(count);
                for(std::uint32_t index=0;index<count;++index) {
                    std::uint64_t bits=0;if(!unsignedInteger(bits))return false;
                    const auto element=std::bit_cast<double>(bits);
                    if(!std::isfinite(element))return fail("Packed Array requires finite exact f64 elements.");
                    decoded.push_back(element);
                }
                RuntimeValue complete{RuntimeF64Array{new ImmutableF64Array(*arrayOwner_,std::move(decoded))}};
                --arrayDepth_;if(!arrayDepth_)arrayEnvelopeBytes_+=cursor_-begin;
                value=std::move(complete);return true;
            }catch(const std::bad_alloc &) {return fail("Not enough memory to complete this operation; no partial Array value was produced.");}
            catch(const std::length_error &) {return fail("Not enough memory to complete this operation; no partial Array value was produced.");}
        }
        case 8U: case 9U: case 10U: case 11U: case 12U: {
            if(!typed_||packed_||(tag!=11&&(!arrayOwner_||!runtimeValueOwnerValid(*arrayOwner_))))return fail("Local IPE values require a typed envelope and its exact value owner.");
            std::optional<RuntimeValue> complete;
            if(tag==8) {
                ValueType element;std::uint32_t rank=0,size=0;
                if(!valueType(element)||!count(rank)||rank==0||rank>16)return fail("Invalid IPE Array rank.");
                std::vector<std::uint32_t> shape(rank);
                for(auto &extent:shape)if(!unsignedInteger(extent))return false;
                const auto product=array_detail::elementCount(shape);
                if(!unsignedInteger(size)||!product||size!=*product)return fail("IPE Array shape/value count differs or exceeds4294967295 elements.");
                if(size>(std::numeric_limits<std::size_t>::max)()/sizeof(RuntimeValue))return fail("Not enough memory to complete this operation.");
                const auto begin=cursor_,outerNodes=valueNodes_,outerBytes=valueBytes_;++arrayDepth_;
                // At least one tag byte is required per element. Refuse a
                // forged huge count before attempting its vector allocation.
                if(!reserve(size))return false;
                try {
                std::vector<RuntimeValue> elements;elements.reserve(size);
                for(std::uint32_t i=0;i<size;++i){valueNodes_=0;valueBytes_=0;RuntimeValue v;if(!runtimeValue(v,depth+1))return false;elements.push_back(std::move(v));}
                --arrayDepth_;valueNodes_=outerNodes;valueBytes_=outerBytes;
                if(!arrayDepth_)arrayEnvelopeBytes_+=cursor_-begin;
                complete=makeRuntimeArray(std::move(element),std::move(shape),std::move(elements),*arrayOwner_);
                }catch(const std::bad_alloc &){return fail("Not enough memory to complete this operation; no partial Array value was produced.");}
                catch(const std::length_error &){return fail("Not enough memory to complete this operation; no partial Array value was produced.");}
            }else if(tag==9) {
                ValueType payloadType;RuntimeValue payload;std::uint32_t size=0;std::string payloadSchema;
                if(!valueType(payloadType)||!string(payloadSchema)||payloadSchema.size()>65536||!runtimeValue(payload,depth+1)||!count(size)||size>4096)return fail("Invalid IPE Variant attribute count or schema.");
                std::vector<RuntimeNamedField> attributes;attributes.reserve(size);
                for(std::uint32_t i=0;i<size;++i){RuntimeNamedField field;if(!string(field.id)||!valueType(field.type)||!runtimeValue(field.value,depth+1))return false;attributes.push_back(std::move(field));}
                complete=makeRuntimeVariant(std::move(payloadType),std::move(payload),std::move(attributes),*arrayOwner_,std::move(payloadSchema));
            }else if(tag==10) {
                ValueType key,item;std::uint32_t size=0;if(!valueType(key)||!valueType(item)||!count(size)||size>4096)return fail("Invalid IPE Map entry count.");
                std::vector<std::pair<RuntimeValue,RuntimeValue>> entries;entries.reserve(size);
                for(std::uint32_t i=0;i<size;++i){RuntimeValue k,v;if(!runtimeValue(k,depth+1)||!runtimeValue(v,depth+1))return false;entries.emplace_back(std::move(k),std::move(v));}
                std::vector<RuntimeValue> keys;keys.reserve(entries.size());for(const auto &entry:entries)keys.push_back(entry.first);
                const auto normalized=normalizeRuntimeCollectionValues(keys,key,*arrayOwner_,collection_order::DuplicatePolicy::KeepLast);
                if(!normalized.accepted()||normalized.indices.size()!=entries.size())return fail("A sealed Map must already have unique sorted keys.");
                for(std::size_t i=0;i<normalized.indices.size();++i)if(normalized.indices[i]!=i)return fail("A sealed Map cannot be reordered by decoding.");
                complete=makeRuntimeMap(std::move(key),std::move(item),std::move(entries),*arrayOwner_);
            }else if(tag==12) {
                ValueType element;std::uint32_t size=0;if(!valueType(element)||!count(size)||size>4096)return fail("Invalid Set element count.");
                std::vector<RuntimeValue> elements;elements.reserve(size);
                for(std::uint32_t i=0;i<size;++i){RuntimeValue item;if(!runtimeValue(item,depth+1))return false;elements.push_back(std::move(item));}
                const auto normalized=normalizeRuntimeCollectionValues(elements,element,*arrayOwner_,collection_order::DuplicatePolicy::KeepFirst);
                if(!normalized.accepted()||normalized.indices.size()!=elements.size())return fail("A sealed Set must already contain unique sorted elements.");
                for(std::size_t i=0;i<normalized.indices.size();++i)if(normalized.indices[i]!=i)return fail("A sealed Set cannot be reordered by decoding.");
                complete=makeRuntimeSet(std::move(element),std::move(elements),*arrayOwner_);
            }else {std::uint64_t seconds=0,fraction=0;if(!unsignedInteger(seconds)||!unsignedInteger(fraction))return false;complete=makeRuntimeTimestamp(static_cast<std::int64_t>(seconds),fraction);}
            if(!complete)return fail("The sealed IPE value failed its exact carrier contract.");value=std::move(*complete);return true;
        }
        default:
            return fail("The artifact contains an invalid Runtime value tag.");
        }
    }

    [[nodiscard]] bool finished() const noexcept {
        return valid_ && cursor_ == data_.size();
    }
    [[nodiscard]] std::string_view error() const noexcept { return error_; }

private:
    [[nodiscard]] bool reserve(std::size_t size) {
        if (!valid_ || size > data_.size() - cursor_) {
            return fail("The artifact payload is truncated.");
        }
        if(typed_&&!arrayDepth_&&size>kMaximumTypedEnvelopeSize-(cursor_-arrayEnvelopeBytes_))
            return fail("Typed artifact metadata exceeds its aggregate envelope budget.");
        return true;
    }

    [[nodiscard]] bool fail(std::string_view message) {
        if (valid_) error_ = std::string(message);
        valid_ = false;
        return false;
    }

    std::span<const std::uint8_t> data_;
    std::size_t cursor_ = 0U;
    bool valid_ = true, typed_ = false, packed_ = false;
    const RuntimeValueOwner *arrayOwner_ = nullptr;
    std::size_t valueNodes_ = 0, valueBytes_ = 0;
    std::size_t arrayEnvelopeBytes_ = 0;
    unsigned arrayDepth_ = 0;
    std::string error_;
};

inline Diagnostic codecDiagnostic(std::string message) {
    return Diagnostic{
        DiagnosticSeverity::Error, DiagnosticStage::ContractIntake,
        "contract.codec.invalid", std::move(message),
        {}, {}, {}, true, true
    };
}

} // namespace codec_detail

[[nodiscard]] inline std::vector<std::uint8_t> encodeProgramArtifact(
    const ProgramArtifact &artifact) {
    const bool typed = artifact.schemaVersion == kTypedArtifactSchemaVersion;
    const bool packed = artifact.schemaVersion == kPackedF64ArrayArtifactSchemaVersion;
    const bool owned = typed || packed;
    if ((!owned && artifact.schemaVersion != kArtifactSchemaVersion) ||
        !validateProgramArtifact(artifact).empty()) return {};
    if (owned) {
        for (const auto &node : artifact.nodes) if (node.literalValue) {
            const auto output = std::find_if(node.ports.begin(), node.ports.end(), [](const auto &port) {
                return port.direction == PortDirection::Output;
            });
            if (output == node.ports.end() || !runtimeValueValidForOwner(*node.literalValue, output->valueType, *artifact.valueOwner)) return {};
        }
        for (const auto &binding : artifact.uiBindings)
            if (!std::holds_alternative<std::monostate>(binding.initialValue) &&
                !runtimeValueValidForOwner(binding.initialValue, binding.valueType, *artifact.valueOwner)) return {};
    }
    codec_detail::Writer writer(typed,packed,owned ? &*artifact.valueOwner : nullptr);
    writer.bytes(packed ? codec_detail::kPackedF64ArrayMagic : typed ? codec_detail::kTypedMagic : codec_detail::kMagic);
    writer.unsignedInteger(artifact.protocol.major);
    writer.unsignedInteger(artifact.protocol.minor);
    writer.unsignedInteger(artifact.schemaVersion);
    writer.stableId(artifact.artifactId);
    writer.stableId(artifact.sourceDocumentId);
    writer.string(artifact.sourceDocumentUri);
    writer.unsignedInteger(artifact.revision.semantic);
    writer.unsignedInteger(artifact.revision.interfaceContract);
    writer.unsignedInteger(artifact.revision.topology);
    if (owned) {
        const auto &owner = *artifact.valueOwner;
        writer.string(owner.id); writer.string(owner.sourceDigest);
        writer.unsignedInteger(owner.epoch); writer.unsignedInteger(owner.sourceRevision);
        writer.unsignedInteger(owner.artifactRevision);
    }

    writer.unsignedInteger(static_cast<std::uint32_t>(artifact.nodes.size()));
    for (const auto &node : artifact.nodes) {
        writer.stableId(node.id);
        writer.enumeration(node.kind);
        writer.string(node.operationId);
        writer.string(node.implementationId);
        writer.unsignedInteger(static_cast<std::uint32_t>(node.ports.size()));
        for (const auto &port : node.ports) {
            writer.stableId(port.id);
            writer.string(port.displayName);
            writer.enumeration(port.direction);
            writer.valueType(port.valueType);
            writer.enumeration(port.requirement);
            writer.enumeration(port.cardinality);
            writer.enumeration(port.connectionPolicy);
        }
        writer.boolean(node.literalValue.has_value());
        if (node.literalValue.has_value()) writer.runtimeValue(*node.literalValue);
        if (owned) {
            writer.unsignedInteger(static_cast<std::uint32_t>(node.selectedFields.size()));
            for (const auto &field : node.selectedFields) writer.string(field);
        }
    }

    writer.unsignedInteger(static_cast<std::uint32_t>(artifact.edges.size()));
    for (const auto &edge : artifact.edges) {
        writer.stableId(edge.id);
        writer.stableId(edge.source.nodeId);
        writer.stableId(edge.source.portId);
        writer.stableId(edge.sink.nodeId);
        writer.stableId(edge.sink.portId);
    }

    writer.unsignedInteger(static_cast<std::uint32_t>(artifact.uiBindings.size()));
    for (const auto &binding : artifact.uiBindings) {
        writer.stableId(binding.widgetId);
        writer.stableId(binding.endpoint.nodeId);
        writer.stableId(binding.endpoint.portId);
        writer.enumeration(binding.direction);
        writer.valueType(binding.valueType);
        writer.runtimeValue(binding.initialValue);
        if (owned) writer.boolean(binding.suppliedByEvent);
    }

    writer.unsignedInteger(static_cast<std::uint32_t>(artifact.sourceIdentities.size()));
    for (const auto &identity : artifact.sourceIdentities) {
        writer.stableId(identity.id);
        writer.string(identity.sourceUri);
        writer.unsignedInteger(identity.span.beginLine);
        writer.unsignedInteger(identity.span.beginColumn);
        writer.unsignedInteger(identity.span.endLine);
        writer.unsignedInteger(identity.span.endColumn);
        writer.stableId(identity.nodeId);
        writer.stableId(identity.portId);
    }
    return std::move(writer).finish();
}

[[nodiscard]] inline ArtifactDecodeResult decodeProgramArtifact(
    std::span<const std::uint8_t> payload) {
    codec_detail::Reader reader(payload);
    std::array<std::uint8_t, codec_detail::kMagic.size()> magic{};
    ProgramArtifact artifact;
    const auto fail = [&reader]() {
        ArtifactDecodeResult result;
        result.diagnostics.push_back(codec_detail::codecDiagnostic(
            reader.error().empty() ? "The artifact payload is invalid." :
                                     std::string(reader.error())));
        return result;
    };

    if (!reader.bytes(magic) || (magic != codec_detail::kMagic && magic != codec_detail::kTypedMagic && magic != codec_detail::kPackedF64ArrayMagic) ||
        !reader.unsignedInteger(artifact.protocol.major) ||
        !reader.unsignedInteger(artifact.protocol.minor) ||
        !reader.unsignedInteger(artifact.schemaVersion)) return fail();
    const bool typed = artifact.schemaVersion == kTypedArtifactSchemaVersion;
    const bool packed = artifact.schemaVersion == kPackedF64ArrayArtifactSchemaVersion;
    const bool owned = typed || packed;
    if ((packed ? codec_detail::kPackedF64ArrayMagic : typed ? codec_detail::kTypedMagic : codec_detail::kMagic) != magic ||
        (!owned && artifact.schemaVersion != kArtifactSchemaVersion)) return fail();
    // Admit each owned envelope before the first variable-length field.
    if (!reader.typed(owned,packed)) return fail();
    if (!reader.stableId(artifact.artifactId) ||
        !reader.stableId(artifact.sourceDocumentId) ||
        !reader.string(artifact.sourceDocumentUri) ||
        !reader.unsignedInteger(artifact.revision.semantic) ||
        !reader.unsignedInteger(artifact.revision.interfaceContract) ||
        !reader.unsignedInteger(artifact.revision.topology)) return fail();
    if (owned) {
        RuntimeValueOwner owner;
        if (!reader.string(owner.id) || !reader.string(owner.sourceDigest) ||
            !reader.unsignedInteger(owner.epoch) || !reader.unsignedInteger(owner.sourceRevision) ||
            !reader.unsignedInteger(owner.artifactRevision) || !runtimeValueOwnerValid(owner)) return fail();
        artifact.valueOwner = std::move(owner);
        reader.arrayOwner(*artifact.valueOwner);
    }

    std::uint32_t count = 0U;
    if (!reader.count(count)) return fail();
    artifact.nodes.reserve(count);
    for (std::uint32_t nodeIndex = 0U; nodeIndex < count; ++nodeIndex) {
        RuntimeNode node;
        if (!reader.stableId(node.id) ||
            !reader.enumeration(node.kind, static_cast<std::uint8_t>(NodeKind::Call)) ||
            !reader.string(node.operationId) || !reader.string(node.implementationId)) return fail();
        std::uint32_t portCount = 0U;
        if (!reader.count(portCount)) return fail();
        node.ports.reserve(portCount);
        for (std::uint32_t portIndex = 0U; portIndex < portCount; ++portIndex) {
            RuntimePort port;
            if (!reader.stableId(port.id) || !reader.string(port.displayName) ||
                !reader.enumeration(port.direction, static_cast<std::uint8_t>(PortDirection::Output)) ||
                !reader.valueType(port.valueType) ||
                !reader.enumeration(port.requirement, static_cast<std::uint8_t>(PortRequirement::Optional)) ||
                !reader.enumeration(port.cardinality, static_cast<std::uint8_t>(PortCardinality::Variadic)) ||
                !reader.enumeration(port.connectionPolicy, static_cast<std::uint8_t>(ConnectionPolicy::AnyExplicit))) return fail();
            node.ports.push_back(std::move(port));
        }
        bool hasLiteral = false;
        if (!reader.boolean(hasLiteral)) return fail();
        if (hasLiteral) {
            RuntimeValue value;
            if (!reader.runtimeValue(value)) return fail();
            node.literalValue = std::move(value);
        }
        if (owned) {
            std::uint32_t fieldCount = 0;
            if (!reader.count(fieldCount) || fieldCount > 64 || (packed && fieldCount != 0)) return fail();
            for (std::uint32_t index = 0; index < fieldCount; ++index) {
                std::string field; if (!reader.string(field)) return fail();
                node.selectedFields.push_back(std::move(field));
            }
        }
        artifact.nodes.push_back(std::move(node));
    }

    if (!reader.count(count)) return fail();
    artifact.edges.reserve(count);
    for (std::uint32_t index = 0U; index < count; ++index) {
        RuntimeEdge edge;
        if (!reader.stableId(edge.id) || !reader.stableId(edge.source.nodeId) ||
            !reader.stableId(edge.source.portId) || !reader.stableId(edge.sink.nodeId) ||
            !reader.stableId(edge.sink.portId)) return fail();
        artifact.edges.push_back(std::move(edge));
    }

    if (!reader.count(count)) return fail();
    artifact.uiBindings.reserve(count);
    for (std::uint32_t index = 0U; index < count; ++index) {
        UiBinding binding;
        if (!reader.stableId(binding.widgetId) ||
            !reader.stableId(binding.endpoint.nodeId) ||
            !reader.stableId(binding.endpoint.portId) ||
            !reader.enumeration(binding.direction, static_cast<std::uint8_t>(UiBindingDirection::RuntimeToIndicator)) ||
            !reader.valueType(binding.valueType) || !reader.runtimeValue(binding.initialValue)) return fail();
        if (owned && !reader.boolean(binding.suppliedByEvent)) return fail();
        artifact.uiBindings.push_back(std::move(binding));
    }

    if (!reader.count(count)) return fail();
    artifact.sourceIdentities.reserve(count);
    for (std::uint32_t index = 0U; index < count; ++index) {
        SourceIdentity identity;
        if (!reader.stableId(identity.id) || !reader.string(identity.sourceUri) ||
            !reader.unsignedInteger(identity.span.beginLine) ||
            !reader.unsignedInteger(identity.span.beginColumn) ||
            !reader.unsignedInteger(identity.span.endLine) ||
            !reader.unsignedInteger(identity.span.endColumn) ||
            !reader.stableId(identity.nodeId) || !reader.stableId(identity.portId)) return fail();
        artifact.sourceIdentities.push_back(std::move(identity));
    }

    if (!reader.finished()) {
        ArtifactDecodeResult result;
        result.diagnostics.push_back(codec_detail::codecDiagnostic(
            "The artifact payload contains trailing or malformed data."));
        return result;
    }

    ArtifactDecodeResult result;
    result.diagnostics = validateProgramArtifact(artifact);
    if (owned && result.diagnostics.empty()) {
        for (const auto &node : artifact.nodes) if (node.literalValue) {
            const auto output = std::find_if(node.ports.begin(), node.ports.end(), [](const auto &port) {
                return port.direction == PortDirection::Output;
            });
            if (output == node.ports.end() || !runtimeValueValidForOwner(*node.literalValue, output->valueType, *artifact.valueOwner))
                result.diagnostics.push_back(codec_detail::codecDiagnostic("Typed literal is not representable by its actual type/owner."));
        }
        for (const auto &binding : artifact.uiBindings)
            if (!std::holds_alternative<std::monostate>(binding.initialValue) &&
                !runtimeValueValidForOwner(binding.initialValue, binding.valueType, *artifact.valueOwner))
                result.diagnostics.push_back(codec_detail::codecDiagnostic("Typed binding is not representable by its actual type/owner."));
    }
    if (result.diagnostics.empty()) result.artifact = std::move(artifact);
    return result;
}

} // namespace frog::sdk::runtime
