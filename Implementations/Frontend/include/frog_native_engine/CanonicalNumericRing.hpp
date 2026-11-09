#pragma once
// Pure Source metadata checks. A Ring is a physical numeric value plus UI
// choices, never a nominal Enum and never a new numeric realization.
#include "frog_native_engine/BindingType.hpp"
#include "frog/semantics/studio_numeric_promotion.hpp"
#include <cstdint>

namespace frog::native_engine {
inline bool canonicalNumericRingRepresentationSupported(std::string_view name) noexcept {
    const auto type=runtime::studio_numeric::physicalType(name);
    return runtime::studio_numeric::supportedPhysicalType(name)&&type.bits>=8;
}
inline bool canonicalNumericRingChoiceRepresentable(std::string_view name,std::uint32_t value) noexcept {
    if(!canonicalNumericRingRepresentationSupported(name))return false;
    const auto type=runtime::studio_numeric::physicalType(name);
    using Kind=runtime::studio_numeric::PhysicalType::Kind;
    if(type.kind==Kind::Real)return type.bits==64||static_cast<double>(static_cast<float>(value))==static_cast<double>(value);
    if(type.kind==Kind::Signed)return type.bits>32||static_cast<std::uint64_t>(value)<(std::uint64_t{1}<<(type.bits-1));
    return type.bits>=32||static_cast<std::uint64_t>(value)<(std::uint64_t{1}<<type.bits);
}
inline bool canonicalNumericRingMetadataValid(const binding::TypeRef &type) {
    if(type.kind!=binding::TypeKind::Primitive||type.choiceKind!=binding::ChoiceKind::Ring||
        !canonicalNumericRingRepresentationSupported(type.name))return false;
    // Only a disposable validation copy loses display metadata. The actual
    // Source/FIR descriptor, choices and stable IDs stay unchanged.
    auto physical=type;physical.choiceKind=binding::ChoiceKind::None;physical.choices.clear();
    if(!binding::fullTypeEqual(physical,binding::TypeRef::primitive(type.name)))return false;
    for(const auto &choice:type.choices)if(!canonicalNumericRingChoiceRepresentable(type.name,choice.value))return false;
    return true;
}
}
