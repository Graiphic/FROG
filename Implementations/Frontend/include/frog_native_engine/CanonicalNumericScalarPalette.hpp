#pragma once
// Integration proposal for the draft-only adapter. Explicit public Source
// never calls this migration helper and keeps the unchanged Core contracts.
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/CanonicalScalarSelection.hpp"
#include "frog_native_engine/CanonicalScalePowerTwo.hpp"
#include "frog/semantics/numeric_scalar_palette_contracts.hpp"
#include "frog/semantics/complex_arithmetic_ports.hpp"
#include <functional>
#include <vector>

namespace frog::native_engine {
namespace numeric_scalar_palette_detail {
namespace palette=frog::runtime::numeric_aggregate::scalar_palette;
inline std::string_view catalog(std::string_view id) noexcept {
    // These are the actual palette persistence identities, not new aliases in
    // the public FROG operation registry.
    if(id=="frog.core.sub")return "numeric.subtract";
    if(id=="frog.core.mul")return "numeric.multiply";
    if(id=="frog.core.div")return "numeric.divide";
    if(id=="frog.core.abs")return "numeric.absolute_value";
    if(id=="frog.core.neg")return "numeric.negate";
    if(id=="frog.math.round")return "numeric.round_nearest";
    if(id=="frog.math.floor")return "numeric.round_negative_infinity";
    if(id=="frog.math.ceil")return "numeric.round_positive_infinity";
    if(id=="frog.math.square")return "numeric.square";
    if(id=="frog.math.sqrt")return "numeric.square_root";
    if(id=="frog.math.reciprocal")return "numeric.reciprocal";
    if(id=="frog.math.sign")return "numeric.sign";
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return id;
}
inline bool plainScalar(const binding::TypeRef &type) {
    return type.kind==binding::TypeKind::Primitive&&
        (type.name=="bool"||type.name=="c32"||type.name=="c64"||runtime::studio_numeric::supportedPhysicalType(type.name))&&
        binding::fullTypeEqual(type,binding::TypeRef::primitive(type.name));
}
inline std::vector<runtime::numeric_aggregate::SourcePort> ports(
    std::span<const CanonicalNumericAggregatePort> source) {
    std::vector<runtime::numeric_aggregate::SourcePort> result;result.reserve(source.size());
    for(const auto &p:source) {
        if(!plainScalar(p.type))return {};
        const auto physical=sdk::runtime::valueTypeFromCanonicalName(p.type.canonical());
        result.push_back({p.id,physical,p.input,p.connected,(runtime::complex_numeric::arithmetic::complexType(p.type.name)?runtime::complex_numeric::arithmetic::plainSchema(physical):palette::scalarSchema(physical))});
    }
    return result;
}
}
inline std::string_view canonicalNumericScalarPaletteMigrationIdentity(std::string_view authored,
    bool includeLower=true,bool includeUpper=false) noexcept {
    namespace p=numeric_scalar_palette_detail;
    if(p::palette::supports(authored))return authored;
    const auto id=p::catalog(authored);
    for(const auto &spec:p::palette::specs)if(spec.catalog==id &&
        (spec.kind!=p::palette::Kind::InRange || (spec.includeLower==includeLower&&spec.includeUpper==includeUpper)))return spec.id;
    return {};
}
inline bool canonicalNumericScalarPaletteSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr) noexcept {
    try {
        namespace p=numeric_scalar_palette_detail;using Kind=binding::TypeKind;
        const bool add=id==canonicalNumericAggregateAddId;const auto *spec=p::palette::find(id);if(!spec&&!add)return false;
        const bool scalarOnly=!add&&(spec->kind==p::palette::Kind::LogicalShift||spec->kind==p::palette::Kind::Rotate||
            spec->kind==p::palette::Kind::CarryLeft||spec->kind==p::palette::Kind::CarryRight);
        const auto refuse=[&](std::string_view why,std::string_view port={}){if(message)*message=why;if(at)*at=port;return false;};
        // Pure Source-schema validation only. It calls neither the Runtime
        // value walker nor canonicalRecordDataType/WorkBudget/LLVM. The one
        // Runtime walker is used only after lowering prepares exact rd::Type.
        std::size_t visited=0;
        std::function<bool(std::span<const CanonicalNumericAggregatePort>,unsigned)> check;
        check=[&](std::span<const CanonicalNumericAggregatePort> current,unsigned depth){
            if(depth>32||++visited>4096)return refuse("Numeric Source schema exceeds its bounded depth or node count.");
            const binding::TypeRef *shape=nullptr;
            for(const auto &port:current){if(!port.type.known())return refuse("Numeric Source has an unknown full type.",port.id);
                if(port.input&&port.type.kind==Kind::Array){shape=&port.type;break;}}
            if(!shape)for(const auto &port:current)if(port.input&&port.type.kind==Kind::Cluster){shape=&port.type;break;}
            if(!shape){const auto runtimePorts=p::ports(current);if(runtimePorts.size()!=current.size())return refuse("Numeric requires complete plain physical numeric/declared Boolean leaves.");
                if(runtime::complex_numeric::arithmetic::spec(id)&&std::any_of(runtimePorts.begin(),runtimePorts.end(),[](const auto &p){return p.input&&runtime::complex_numeric::arithmetic::complexType(p.type.canonicalName);}))return bool(runtime::complex_numeric::arithmetic::describePorts(id,std::span<const runtime::numeric_aggregate::SourcePort>(runtimePorts),message));
                return add?canonicalNumericAggregateSignatureValid(id,current,message,at):p::palette::validateSignature(id,std::span<const runtime::numeric_aggregate::SourcePort>(runtimePorts),message,at);}
            if(scalarOnly)return refuse("This actual Numeric contract accepts scalars only.");
            if(shape->children.empty()||(shape->kind==Kind::Array&&shape->children.front().kind==Kind::Array))
                return refuse("The actual Numeric authoring contract rejects empty Clusters and direct Array-of-Array schemas.");
            for(const auto &port:current){
                if(!port.input){if(port.type.kind!=shape->kind)return refuse("Numeric output must retain its aggregate structure.",port.id);
                    if(shape->kind==Kind::Array&&port.type.rank!=shape->rank)return refuse("Numeric output Array rank differs.",port.id);
                    if(shape->kind==Kind::Cluster&&(port.type.fieldIds!=shape->fieldIds||port.type.children.size()!=shape->children.size()||port.type.fieldNames!=shape->fieldNames))
                        return refuse("Numeric output Cluster must retain its stable fields and presentation names.",port.id);
                }else if(port.type.kind==shape->kind){
                    if(shape->kind==Kind::Array&&port.type.rank!=shape->rank)return refuse("Numeric input Array ranks differ.",port.id);
                    if(shape->kind==Kind::Cluster&&(port.type.fieldIds!=shape->fieldIds||port.type.children.size()!=shape->children.size()))
                        return refuse("Numeric input Cluster fields differ.",port.id);
                }else if(shape->kind==Kind::Cluster&&port.type.kind!=Kind::Primitive)
                    return refuse("Only a primitive scalar can broadcast over Cluster fields.",port.id);
            }
            const auto fields=shape->kind==Kind::Array?1U:shape->children.size();
            for(std::size_t field=0;field<fields;++field){std::vector<CanonicalNumericAggregatePort> child;child.reserve(current.size());
                for(const auto &port:current){auto next=port;if(port.type.kind==shape->kind)next.type=port.type.children.at(field);child.push_back(std::move(next));}
                if(!check(child,depth+1))return false;
            }
            return true;
        };
        return check(ports,0);
    }catch(...){if(message)*message="Numeric palette signature validation failed without admitting a partial schema.";return false;}
}
inline bool canonicalNumericScalarPaletteShouldUse(std::string_view authored,
    std::span<const CanonicalNumericAggregatePort> ports,bool includeLower=true,bool includeUpper=false) noexcept {
    namespace p=numeric_scalar_palette_detail;
    try {
        const auto id=canonicalNumericScalarPaletteMigrationIdentity(authored,includeLower,includeUpper);
        if(id.empty()||!canonicalNumericScalarPaletteSignatureValid(id,ports))return false;
        if(p::palette::supports(authored))return true;
        const auto catalog=p::catalog(authored);std::string exact;std::size_t inputs=0;
        bool uniform=true;
        for(const auto &port:ports)if(port.input&&port.type.name!="bool") {
            if(exact.empty())exact=port.type.name;else if(exact!=port.type.name)uniform=false;++inputs;
        }
        if(catalog=="numeric.scale_power_two") {
            // Preserve the existing IEEE F64 provider only for its EXACT actual
            // x/n/result signature, not merely the first operand's type.
            return !std::all_of(ports.begin(),ports.end(),[](const auto &port){return port.type==binding::TypeRef::primitive("f64");});
        }
        if(catalog=="numeric.max_min"&&uniform) {
            const auto *existing=canonicalScalarSelectionAuthoredContract("frog.studio.numeric.max_min");
            if(existing&&canonicalScalarSelectionTypeAccepted(*existing,exact))return false;
        }
        if(catalog=="numeric.absolute_value"&&uniform&&(exact=="i8"||exact=="i16"||exact=="f32"))
            return false; // The existing exact-width Abs/coercion owner remains.
        if(uniform) {
            const auto legacy=canonicalScalarMigrationIdentityForType("frog.studio."+std::string(catalog),exact);
            const auto *contract=canonicalScalarNativeContract(legacy);
            if(contract && (inputs==contract->inputCount ||
                (contract->maximumInputCount && inputs>=contract->inputCount&&inputs<=contract->maximumInputCount)))return false;
        }
        return true;
    }catch(...){return false;}
}
}
