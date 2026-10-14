#pragma once
#include "frog/sdk/typed_event_packet.hpp"
#include "frog/sdk/runtime_compound_value.hpp"

namespace frog::sdk::events {
// Explicit owner-preserving adaptation into the existing RuntimeValue and
// ProgramSession transport. No pointer, UI label or presentation string is
// substituted for a real borrowed object reference or a structured value.
inline runtime::RuntimeValueOwner runtimeValueOwner(const SourceOwner &owner) {
    return {owner.id,owner.sourceDigest,owner.epoch,owner.sourceRevision,owner.artifactRevision};
}
inline SourceOwner eventSourceOwner(const runtime::RuntimeValueOwner &owner) {
    return {owner.id,owner.sourceDigest,owner.epoch,owner.sourceRevision,owner.artifactRevision};
}
namespace captured_detail {
class ReferenceAuthority final:public runtime::RuntimeReferenceAuthority {
public:
    ReferenceAuthority(FieldValue reference,SourceOwner owner)
        :reference_(std::move(reference)),owner_(std::move(owner)) {}
    std::string_view canonicalType() const noexcept override{return reference_.typeName();}
    bool validForOwner(const runtime::RuntimeValueOwner &owner) const noexcept override {
        // Compare directly without allocating on the provider's noexcept path.
        return owner.id==owner_.id && owner.sourceDigest==owner_.sourceDigest &&
            owner.epoch==owner_.epoch && owner.sourceRevision==owner_.sourceRevision &&
            owner.artifactRevision==owner_.artifactRevision &&
            reference_.validateReferences(owner_)==PacketError::None;
    }
private:
    const FieldValue reference_;
    const SourceOwner owner_;
};
inline std::optional<runtime::RuntimeValue> value(const FieldValue &field,const SourceOwner &owner) {
    if(field.kind()==FieldValue::Kind::Scalar) {
        const auto *actual=field.scalarValue();
        if(!actual || !runtime::runtimeValueValidForOwner(*actual,
            runtime::valueTypeFromCanonicalName(field.typeName()),runtimeValueOwner(owner)))return {};
        return *actual;
    }
    if(field.kind()==FieldValue::Kind::Reference)
        return runtime::makeRuntimeReference(std::make_shared<ReferenceAuthority>(field,owner),runtimeValueOwner(owner));
    std::vector<runtime::RuntimeNamedField> fields;fields.reserve(field.members().size());
    for(const auto &[id,child]:field.members()) {
        auto captured=value(child,owner);
        if(!captured)return {};
        fields.push_back({id,runtime::valueTypeFromCanonicalName(child.typeName()),std::move(*captured)});
    }
    auto result=runtime::makeRuntimeRecord(std::move(fields));
    if(!result || !runtime::runtimeValueValidForOwner(*result,
        runtime::valueTypeFromCanonicalName(field.typeName()),runtimeValueOwner(owner)))return {};
    return result;
}
}
// Admission checks the exact owner and the actual compiled region schema;
// captured packet values never choose the body, wire type or object owner.
inline std::optional<runtime::EventNotification> runtimeEventNotification(
    const TypedEventPacket &packet,const SourceOwner &expectedOwner,
    const runtime::EventPayloadSchema &schema,PacketError *error=nullptr) {
    if(const auto status=packet.validateFor(expectedOwner);status!=PacketError::None) {
        packet_detail::error(error,status);return {};
    }
    std::vector<std::pair<std::string,std::string>> declared;declared.reserve(schema.size());
    for(const auto &[id,type]:schema)declared.emplace_back(id,type.canonicalName);
    if(const auto status=packet.validateSchema(declared);status!=PacketError::None) {
        packet_detail::error(error,status);return {};
    }
    runtime::EventNotification notification{packet.identity(),packet.timeMilliseconds(),{}};
    for(const auto &[id,field]:packet.fields()) {
        auto captured=captured_detail::value(field,expectedOwner);
        if(!captured){packet_detail::error(error,PacketError::InvalidValue);return {};}
        notification.fields.emplace(id,std::move(*captured));
    }
    packet_detail::error(error,PacketError::None);return notification;
}
} // namespace frog::sdk::events
