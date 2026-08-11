#pragma once

#include "frog/sdk/runtime_contract.hpp"

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
inline constexpr std::uint32_t kMaximumCollectionSize = 1'000'000U;
inline constexpr std::uint32_t kMaximumStringSize = 16U * 1024U * 1024U;

class Writer final {
public:
    void bytes(std::span<const std::uint8_t> value) {
        data_.insert(data_.end(), value.begin(), value.end());
    }

    template <typename Unsigned>
    void unsignedInteger(Unsigned value) {
        static_assert(std::is_unsigned_v<Unsigned>);
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

    void runtimeValue(const RuntimeValue &value) {
        enumeration(static_cast<std::uint8_t>(value.index()));
        switch (value.index()) {
        case 0U: break;
        case 1U: boolean(std::get<bool>(value)); break;
        case 2U:
            unsignedInteger(static_cast<std::uint64_t>(std::get<std::int64_t>(value)));
            break;
        case 3U: unsignedInteger(std::get<std::uint64_t>(value)); break;
        case 4U:
            unsignedInteger(std::bit_cast<std::uint64_t>(std::get<double>(value)));
            break;
        case 5U: string(std::get<std::string>(value)); break;
        default: break;
        }
    }

    [[nodiscard]] std::vector<std::uint8_t> finish() && {
        return std::move(data_);
    }

private:
    std::vector<std::uint8_t> data_;
};

class Reader final {
public:
    explicit Reader(std::span<const std::uint8_t> data) : data_(data) {}

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
            enumeration(type.category, static_cast<std::uint8_t>(ValueCategory::Array)) &&
            unsignedInteger(type.bitWidth) && unsignedInteger(type.rank) &&
            string(type.elementType);
    }

    [[nodiscard]] bool runtimeValue(RuntimeValue &value) {
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
            std::string decoded;
            if (!string(decoded)) return false;
            value = std::move(decoded);
            return true;
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
        return true;
    }

    [[nodiscard]] bool fail(std::string_view message) {
        if (valid_) error_ = std::string(message);
        valid_ = false;
        return false;
    }

    std::span<const std::uint8_t> data_;
    std::size_t cursor_ = 0U;
    bool valid_ = true;
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
    codec_detail::Writer writer;
    writer.bytes(codec_detail::kMagic);
    writer.unsignedInteger(artifact.protocol.major);
    writer.unsignedInteger(artifact.protocol.minor);
    writer.unsignedInteger(artifact.schemaVersion);
    writer.stableId(artifact.artifactId);
    writer.stableId(artifact.sourceDocumentId);
    writer.string(artifact.sourceDocumentUri);
    writer.unsignedInteger(artifact.revision.semantic);
    writer.unsignedInteger(artifact.revision.interfaceContract);
    writer.unsignedInteger(artifact.revision.topology);

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

    if (!reader.bytes(magic) || magic != codec_detail::kMagic ||
        !reader.unsignedInteger(artifact.protocol.major) ||
        !reader.unsignedInteger(artifact.protocol.minor) ||
        !reader.unsignedInteger(artifact.schemaVersion) ||
        !reader.stableId(artifact.artifactId) ||
        !reader.stableId(artifact.sourceDocumentId) ||
        !reader.string(artifact.sourceDocumentUri) ||
        !reader.unsignedInteger(artifact.revision.semantic) ||
        !reader.unsignedInteger(artifact.revision.interfaceContract) ||
        !reader.unsignedInteger(artifact.revision.topology)) return fail();

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
    if (result.diagnostics.empty()) result.artifact = std::move(artifact);
    return result;
}

} // namespace frog::sdk::runtime
