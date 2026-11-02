#pragma once
// Private IEEE complex value carrier. RuntimeValue's alternatives remain
// unchanged: this value occupies its existing immutable CompoundRuntimeValue.
#include "frog/sdk/runtime_contract.hpp"
#include <bit>
#include <cmath>
#include <variant>

namespace frog::sdk::runtime {
struct RuntimeComplexParts { double real=0, imaginary=0; };
class ImmutableComplexValue;
[[nodiscard]] inline std::optional<RuntimeValue> makeRuntimeComplex(
    const ValueType &,double,double) noexcept;

// Neither a mutable array nor a textual encoding can impersonate a complex
// value. The two precision-specific physical alternatives are sealed here.
class ImmutableComplexValue final : public CompoundRuntimeValue {
    struct Components32 {float real,imaginary;};
    struct Components64 {double real,imaginary;};
    friend std::optional<RuntimeValue> makeRuntimeComplex(const ValueType &,double,double) noexcept;
    ImmutableComplexValue(float real,float imaginary):parts_(Components32{real,imaginary}){}
    ImmutableComplexValue(double real,double imaginary):parts_(Components64{real,imaginary}){}
public:
    std::string_view canonicalType() const noexcept override {
        return std::holds_alternative<Components32>(parts_)?"c32":"c64";
    }
    bool isReference() const noexcept override {return false;}
    bool valid() const noexcept override {
        // Both sealed alternatives carry every IEEE bit pattern, including
        // NaN and infinities. Finite-domain algorithms validate separately.
        return std::holds_alternative<Components32>(parts_) ||
            std::holds_alternative<Components64>(parts_);
    }
    bool validForOwner(const RuntimeValueOwner &owner) const noexcept override {
        return runtimeValueOwnerValid(owner)&&valid();
    }
    std::span<const RuntimeNamedField> fields() const noexcept override {return {};}
    [[nodiscard]] RuntimeComplexParts components() const noexcept {
        if(const auto *p=std::get_if<Components32>(&parts_))return {p->real,p->imaginary};
        const auto &p=std::get<Components64>(parts_);return {p.real,p.imaginary};
    }
    [[nodiscard]] double real() const noexcept {return components().real;}
    [[nodiscard]] double imaginary() const noexcept {return components().imaginary;}
    [[nodiscard]] std::uint64_t realBits() const noexcept {
        if(const auto *p=std::get_if<Components32>(&parts_))return std::bit_cast<std::uint32_t>(p->real);
        return std::bit_cast<std::uint64_t>(std::get<Components64>(parts_).real);
    }
    [[nodiscard]] std::uint64_t imaginaryBits() const noexcept {
        if(const auto *p=std::get_if<Components32>(&parts_))return std::bit_cast<std::uint32_t>(p->imaginary);
        return std::bit_cast<std::uint64_t>(std::get<Components64>(parts_).imaginary);
    }
private:
    const std::variant<Components32,Components64> parts_;
};

[[nodiscard]] inline const ImmutableComplexValue *runtimeComplex(const RuntimeValue &value) noexcept {
    const auto *p=std::get_if<std::shared_ptr<const CompoundRuntimeValue>>(&value);
    return p&&*p?dynamic_cast<const ImmutableComplexValue *>(p->get()):nullptr;
}
[[nodiscard]] inline std::optional<RuntimeComplexParts> readRuntimeComplex(
    const RuntimeValue &value,const ValueType &type) noexcept {
    try {
        const auto *p=runtimeComplex(value);
        if(!p||!p->valid()||p->canonicalType()!=type.canonicalName||
            type!=valueTypeFromCanonicalName(type.canonicalName)||type.category!=ValueCategory::Complex)return {};
        return p->components();
    }catch(...){return {};}
}

// The factory seals components already rounded to the requested precision.
// Arithmetic/conversion owns rounding; a decoder must not silently round a
// malformed CSG payload. In particular F32 integer conversion remains direct.
[[nodiscard]] inline std::optional<RuntimeValue> makeRuntimeComplex(
    const ValueType &type,double real,double imaginary) noexcept {
    try {
        if(type!=valueTypeFromCanonicalName(type.canonicalName))return {};
        std::shared_ptr<const CompoundRuntimeValue> result;
        if(type.canonicalName=="c32") {
            const float re=static_cast<float>(real),im=static_cast<float>(imaginary);
            const auto exact=[](double input,float stored) {
                return (std::isnan(input)&&std::isnan(stored)) || static_cast<double>(stored)==input;
            };
            if(!exact(real,re)||!exact(imaginary,im))return {};
            result.reset(new ImmutableComplexValue(re,im));
        } else if(type.canonicalName=="c64")result.reset(new ImmutableComplexValue(real,imaginary));
        else return {};
        return RuntimeValue{std::move(result)};
    }catch(...){return {};}
}
} // namespace frog::sdk::runtime
