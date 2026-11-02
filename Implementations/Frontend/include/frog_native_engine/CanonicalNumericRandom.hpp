#pragma once
// Pure Source admission/default metadata. No editor, Runtime Instance/value
// implementation, LLVM, WorkBudget or non-inline Runtime symbol is required.
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog/semantics/studio_numeric_promotion.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>

namespace frog::native_engine {
inline constexpr std::string_view canonicalNumericRandomProfile="frog.studio.numeric.random.instance-owned@1";
inline constexpr std::string_view canonicalNumericRandomZeroOneId="frog.studio.numeric.random_zero_one.instance-owned.v1";
inline constexpr std::string_view canonicalNumericRandomRangeId="frog.studio.numeric.random_range.instance-owned.v1";
inline std::string_view canonicalNumericRandomMigrationIdentity(std::string_view authored)noexcept {
    if(authored==canonicalNumericRandomZeroOneId||authored==canonicalNumericRandomRangeId)return authored;
    if(authored.starts_with("frog.studio."))authored.remove_prefix(12);
    if(authored=="numeric.random_zero_one")return canonicalNumericRandomZeroOneId;
    if(authored=="numeric.random_range")return canonicalNumericRandomRangeId;
    return {};
}
inline bool canonicalNumericRandomOperation(std::string_view id)noexcept {
    return id==canonicalNumericRandomZeroOneId||id==canonicalNumericRandomRangeId;
}
inline bool canonicalNumericRandomSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr)noexcept {
    const auto refuse=[&](std::string_view why,std::string_view port={}){if(message)*message=why;if(at)*at=port;return false;};
    try {
        const bool range=id==canonicalNumericRandomRangeId;
        if(!canonicalNumericRandomOperation(id)||ports.size()!=(range?3U:1U))
            return refuse("Random requires its exact private identity and complete declared connectors.");
        const CanonicalNumericAggregatePort *lower=nullptr,*upper=nullptr,*result=nullptr;
        for(const auto &p:ports) {
            if(p.type.kind!=binding::TypeKind::Primitive||!runtime::studio_numeric::supportedPhysicalType(p.type.name)||
               !binding::fullTypeEqual(p.type,binding::TypeRef::primitive(p.type.name)))
                return refuse("Random requires a complete plain physical real/integer scalar; nominal and unsupported representations are retained and refused.",p.id);
            if(!p.input&&p.id=="result"&&!result)result=&p;
            else if(range&&p.input&&p.id=="lower_limit"&&!lower)lower=&p;
            else if(range&&p.input&&p.id=="upper_limit"&&!upper)upper=&p;
            else return refuse("Random received an unknown or duplicate connector.",p.id);
        }
        if(!result||(range&&(!lower||!upper)))return refuse("Random lost a declared connector.");
        if(!range)return result->type==binding::TypeRef::primitive("f64")||refuse("Random Number (0-1) produces scalar DBL.","result");
        const auto promoted=runtime::studio_numeric::binaryPromotionType(lower->type.name,upper->type.name);
        return promoted==result->type.name||refuse("Random Range output must retain the actual input numeric promotion.","result");
    }catch(...){return refuse("Random schema validation failed without admitting a partial type.");}
}
// The existing GS evaluator obtains these defaults from its selected numeric
// type, with saturating conversion of the literal +/-100 for narrow types.
// This metadata is consumed once by the snapshot adapter, never by the UI.
inline std::optional<std::string> canonicalNumericRandomDefault(
    const binding::TypeRef &type,std::string_view port) {
    if((port!="lower_limit"&&port!="upper_limit")||type.kind!=binding::TypeKind::Primitive||
       !runtime::studio_numeric::supportedPhysicalType(type.name)||
       !binding::fullTypeEqual(type,binding::TypeRef::primitive(type.name)))return {};
    const auto physical=runtime::studio_numeric::physicalType(type.name);
    if(port=="lower_limit") {
        if(physical.kind!=runtime::studio_numeric::PhysicalType::Kind::Signed)return "0";
        return physical.bits<8?std::to_string(-(std::int64_t{1}<<(physical.bits-1))):std::string("-100");
    }
    if(physical.bits<8) {
        const auto maximum=(std::uint64_t{1}<<(physical.kind==runtime::studio_numeric::PhysicalType::Kind::Signed?physical.bits-1:physical.bits))-1;
        return std::to_string((std::min)(std::uint64_t{100},maximum));
    }
    return "100";
}
}
