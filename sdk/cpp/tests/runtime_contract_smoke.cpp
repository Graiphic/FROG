#include "frog/sdk/runtime_contract.hpp"
#include "frog/sdk/runtime_contract_codec.hpp"
#include "frog/sdk/runtime_array_value.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>
#include <stdexcept>

int main() {
    using namespace frog::sdk::runtime;
    const auto require=[](bool condition,const char *message){if(!condition)throw std::runtime_error(message);};
    const auto nan=std::numeric_limits<double>::quiet_NaN();
    const auto inf=std::numeric_limits<double>::infinity();
    for(const auto name:{"f32","f64"}) {
        const auto type=valueTypeFromCanonicalName(name);
        require(runtimeValueRepresentable(RuntimeValue{nan},type),"IEEE NaN lost its physical real carrier");
        require(runtimeValueRepresentable(RuntimeValue{inf},type) && runtimeValueRepresentable(RuntimeValue{-inf},type),"IEEE infinity lost its sign/real carrier");
    }
    require(!runtimeValueRepresentable(RuntimeValue{0.1},valueTypeFromCanonicalName("f32")),"Finite F64 was silently narrowed to F32");
    require(!runtimeValueRepresentable(RuntimeValue{nan},valueTypeFromCanonicalName("i64")),"An IEEE real impersonated an integer");
    const RuntimeValueOwner ieeeOwner{"xy-ieee",std::string(64,'a'),1,2,3};
    const auto array=makeRuntimeArray(valueTypeFromCanonicalName("f64"),{3},{RuntimeValue{nan},RuntimeValue{inf},RuntimeValue{-inf}},ieeeOwner);
    const auto arrayType=valueTypeFromCanonicalName("array<f64>");
    require(array && runtimeValueValidForOwner(*array,arrayType,ieeeOwner),"Owned XY IEEE Array was rejected");
    auto foreignOwner=ieeeOwner;++foreignOwner.epoch;
    require(!runtimeValueValidForOwner(*array,arrayType,foreignOwner),"IEEE transport weakened Array ownership");
    for(const auto name:{"c32","c64"}) {
        const auto type=valueTypeFromCanonicalName(name);const auto value=makeRuntimeComplex(type,nan,-inf);
        const auto parts=value?readRuntimeComplex(*value,type):std::nullopt;
        require(parts && std::isnan(parts->real) && std::isinf(parts->imaginary) && parts->imaginary<0,"Complex IEEE coordinates were lost");
        require(runtimeValueRepresentable(*value,type),"Complex IEEE aggregate is not representable");
    }
    require(!makeRuntimeComplex(valueTypeFromCanonicalName("c32"),0.1,0),"Finite complex components were silently narrowed");
    require(!makeRuntimeComplex(valueTypeFromCanonicalName("c32"),(std::numeric_limits<double>::max)(),0),"Finite complex overflow became infinity implicitly");

    // Additive empty-Variant capability: no fabricated Boolean payload;
    // existing typed tag9 bytes, exact I64 and attribute ownership stay intact.
    const auto variantType=valueTypeFromCanonicalName("variant");
    const auto emptyVariant=makeRuntimeEmptyVariant({},ieeeOwner);
    require(emptyVariant&&runtimeVariant(*emptyVariant)->isEmptyVariant()&&
        runtimeVariant(*emptyVariant)->elements().empty()&&!runtimeVariant(*emptyVariant)->payloadType().known()&&
        runtimeVariant(*emptyVariant)->payloadSchemaJson().empty(),"An empty Variant fabricated a typed payload");
    bool checkedPayload=false;try{(void)runtimeVariant(*emptyVariant)->payload();}catch(const std::out_of_range&){checkedPayload=true;}
    require(checkedPayload,"Empty Variant payload access must remain checked");
    require(runtimeValueValidForOwner(*emptyVariant,variantType,ieeeOwner)&&
        !runtimeValueValidForOwner(*emptyVariant,variantType,foreignOwner),"Empty Variant ownership was relaxed");
    const auto i64Type=valueTypeFromCanonicalName("i64");
    constexpr std::int64_t exactInteger=9007199254740993LL;
    const std::string retainedSchema="exact-i64-test-schema";
    const auto typedVariant=makeRuntimeVariant(i64Type,RuntimeValue{exactInteger},{},ieeeOwner,retainedSchema);
    const auto falseVariant=makeRuntimeVariant(valueTypeFromCanonicalName("bool"),RuntimeValue{false},{},ieeeOwner);
    require(typedVariant&&falseVariant&&!runtimeVariant(*typedVariant)->isEmptyVariant()&&
        !runtimeValuesEqual(*emptyVariant,*falseVariant),"Empty and Boolean False Variants became equivalent");
    const auto attributedEmpty=makeRuntimeEmptyVariant({{"exact",variantType,*typedVariant},{"empty",variantType,*emptyVariant}},ieeeOwner);
    require(attributedEmpty&&runtimeVariant(*attributedEmpty)->isEmptyVariant()&&runtimeVariant(*attributedEmpty)->attributes().size()==2,
        "Empty Variant attributes were dropped or converted into payload data");
    require(!makeRuntimeEmptyVariant({{"foreign",variantType,*typedVariant}},foreignOwner),"An empty Variant accepted a foreign-owned attribute");
    require(!makeRuntimeEmptyVariant({{"duplicate",variantType,*emptyVariant},{"duplicate",variantType,*emptyVariant}},ieeeOwner),
        "Empty Variant duplicate attribute identities were accepted");
    require(!makeRuntimeEmptyVariant({{"not-variant",valueTypeFromCanonicalName("bool"),false}},ieeeOwner),
        "Empty Variant attributes lost their Variant type contract");
    for(const auto &value:{*emptyVariant,*attributedEmpty,*typedVariant,*falseVariant}) {
        codec_detail::Writer writer(true,false,&ieeeOwner);writer.runtimeValue(value);
        const auto encodedValue=std::move(writer).finish();require(!encodedValue.empty(),"A valid empty/typed Variant did not encode");
        const bool isEmpty=runtimeVariant(value)->isEmptyVariant();require(encodedValue.front()==(isEmpty?14:9),"Empty Variant did not use its distinct additive codec tag");
        codec_detail::Reader reader(encodedValue);require(reader.typed(true),"Typed Variant reader initialization failed");reader.arrayOwner(ieeeOwner);
        RuntimeValue roundtrip;require(reader.runtimeValue(roundtrip)&&reader.finished()&&runtimeValuesEqual(roundtrip,value),"Variant binary roundtrip lost empty/typed data or attributes");
        require(runtimeValueValidForOwner(roundtrip,variantType,ieeeOwner),"Decoded Variant lost the declared receiving owner");
    }
    codec_detail::Writer typedWriter(true,false,&ieeeOwner);typedWriter.runtimeValue(*typedVariant);
    auto typedBytes=std::move(typedWriter).finish();
    codec_detail::Writer legacyLayout(true,false,&ieeeOwner);legacyLayout.enumeration(std::uint8_t{9});
    legacyLayout.valueType(i64Type);legacyLayout.string(retainedSchema);legacyLayout.runtimeValue(RuntimeValue{exactInteger},1);
    legacyLayout.unsignedInteger(std::uint32_t{0});
    require(typedBytes==std::move(legacyLayout).finish(),"The existing typed Variant tag9 wire layout changed");
    codec_detail::Writer foreignWriter(true,false,&foreignOwner);foreignWriter.runtimeValue(*attributedEmpty);
    require(std::move(foreignWriter).finish().empty(),"The Variant writer accepted a mismatched source owner");
    const std::array<std::uint8_t,1> unknownTag{255};codec_detail::Reader unknownReader(unknownTag);require(unknownReader.typed(true),"Unknown-tag fixture could not initialize");unknownReader.arrayOwner(ieeeOwner);
    RuntimeValue unknownValue;require(!unknownReader.runtimeValue(unknownValue),"An unknown typed value tag was silently admitted");
    const auto malformedVariant=[&](ValueType payload,std::vector<RuntimeValue> values,std::vector<std::uint32_t> shape,std::string schema){
        return sealRuntimeIpeValue(std::make_shared<const RuntimeIpeValue>(RuntimeIpeValue::Kind::Variant,"variant",ieeeOwner,
            std::move(payload),ValueType{},std::move(shape),std::move(values),std::vector<RuntimeNamedField>{},
            std::vector<std::pair<RuntimeValue,RuntimeValue>>{},0,0,std::move(schema)));
    };
    require(!malformedVariant(valueTypeFromCanonicalName("bool"),{}, {},{}),"A zero-payload Variant claimed a known payload type");
    require(!malformedVariant({}, {},{},"payload-schema-without-payload"),"An empty Variant retained a fictitious payload schema");
    require(!malformedVariant({}, {},{0},{}),"An empty Variant acquired an Array shape");
    require(!malformedVariant({}, {RuntimeValue{false}},{},{}),"A typed Variant payload had no declared type");

    for(const auto spelling:{"ref<\"frog.sync.queue\",f64>","ref<\"frog.sync.queue\",string>","ref<\"frog.sync.queue\",cluster<\"x\":f64,\"name\":string>>"}) {
        const auto null=makeRuntimeNullReference(spelling,ieeeOwner);
        require(null&&runtimeNullReferenceOwner(*null),"A typed NULL reference acquired a live authority");
        require(!makeRuntimeNullReference(spelling,{}),"NULL reference accepted an invalid owner");
        const auto record=makeRuntimeRecord({{"queue",valueTypeFromCanonicalName(spelling),*null},{"large",i64Type,RuntimeValue{exactInteger}}});
        require(bool(record),"A Queue NULL could not be a real record field");
        codec_detail::Writer writer(true,false,&ieeeOwner);writer.runtimeValue(*record);const auto bytes=std::move(writer).finish();
        require(!bytes.empty(),"A record with intrinsic NULL cannot be encoded");
        codec_detail::Reader reader(bytes);require(reader.typed(true),"NULL reader profile failed");reader.arrayOwner(ieeeOwner);
        RuntimeValue decodedNull;require(reader.runtimeValue(decodedNull)&&reader.finished()&&runtimeValuesEqual(*record,decodedNull),"NULL record roundtrip lost type, exact I64 or ownership");
        codec_detail::Writer foreign(true,false,&foreignOwner);foreign.runtimeValue(*null);require(std::move(foreign).finish().empty(),"NULL writer accepted a foreign owner");
        codec_detail::Writer old;old.runtimeValue(*null);require(std::move(old).finish().empty(),"NULL reference entered the legacy scalar profile");
        auto truncatedNull=bytes;truncatedNull.pop_back();codec_detail::Reader shortReader(truncatedNull);require(shortReader.typed(true),"Truncated NULL fixture profile failed");shortReader.arrayOwner(ieeeOwner);RuntimeValue invalidNull;require(!shortReader.runtimeValue(invalidNull),"Truncated NULL record was accepted");
    }
    require(!makeRuntimeNullReference("f64",ieeeOwner)&&!makeRuntimeNullReference(std::string(65537,'x'),ieeeOwner),"NULL factory admitted a non-reference or oversized type");
    struct LiveReference final:RuntimeReferenceAuthority {
        std::string_view canonicalType()const noexcept override{return "ref<\"frog.sync.queue\",f64>";}
        bool validForOwner(const RuntimeValueOwner &owner)const noexcept override{return runtimeValueOwnerValid(owner);}
    };
    const auto live=makeRuntimeReference(std::make_shared<LiveReference>(),ieeeOwner);require(bool(live),"Live negative fixture did not seal");
    codec_detail::Writer liveWriter(true,false,&ieeeOwner);liveWriter.runtimeValue(*live);require(std::move(liveWriter).finish().empty(),"A live reference handle was serialized");
    std::cout<<"INTRINSIC_NULL_CODEC_PASS types=3 nested_record exact_i64 owner foreign truncated legacy active_ref_refused\n";

    ProgramArtifact artifact;
    artifact.artifactId = ArtifactId{ "artifact-1" };
    artifact.sourceDocumentId = DocumentId{ "document-1" };
    artifact.revision = ArtifactRevision{ 2U, 3U, 4U };

    RuntimeNode control;
    control.id = NodeId{ "control" };
    control.kind = NodeKind::Control;
    control.operationId = "frog.ui.numeric.control";
    control.ports.push_back(RuntimePort{
        PortId{ "value" }, "value", PortDirection::Output,
        valueTypeFromCanonicalName("f64") });
    artifact.nodes.push_back(control);

    RuntimeNode indicator;
    indicator.id = NodeId{ "indicator" };
    indicator.kind = NodeKind::Indicator;
    indicator.operationId = "frog.ui.numeric.indicator";
    indicator.ports.push_back(RuntimePort{
        PortId{ "value" }, "value", PortDirection::Input,
        valueTypeFromCanonicalName("f64") });
    artifact.nodes.push_back(indicator);

    artifact.edges.push_back(RuntimeEdge{
        EdgeId{ "edge" },
        RuntimeEndpoint{ NodeId{ "control" }, PortId{ "value" } },
        RuntimeEndpoint{ NodeId{ "indicator" }, PortId{ "value" } }
    });
    artifact.uiBindings.push_back(UiBinding{
        WidgetId{ "control-widget" },
        RuntimeEndpoint{ NodeId{ "control" }, PortId{ "value" } },
        UiBindingDirection::ControlToRuntime,
        valueTypeFromCanonicalName("f64"),
        RuntimeValue{ 1.5 }
    });
    artifact.uiBindings.push_back(UiBinding{
        WidgetId{ "indicator-widget" },
        RuntimeEndpoint{ NodeId{ "indicator" }, PortId{ "value" } },
        UiBindingDirection::RuntimeToIndicator,
        valueTypeFromCanonicalName("f64"),
        RuntimeValue{ 0.0 }
    });

    assert(validateProgramArtifact(artifact).empty());

    RuntimeCapabilities capabilities;
    capabilities.targetId = "frog.runtime.test";
    capabilities.valueTypes = { "f64" };
    assert(validateRuntimeCapabilities(artifact, capabilities).empty());

    const auto encoded = encodeProgramArtifact(artifact);
    const auto decoded = decodeProgramArtifact(encoded);
    assert(decoded.diagnostics.empty());
    assert(decoded.artifact.has_value());
    assert(decoded.artifact->artifactId == artifact.artifactId);
    assert(decoded.artifact->revision == artifact.revision);
    assert(decoded.artifact->nodes == artifact.nodes);
    assert(decoded.artifact->edges == artifact.edges);
    assert(decoded.artifact->uiBindings == artifact.uiBindings);

    auto nullArtifact=artifact;nullArtifact.schemaVersion=kTypedArtifactSchemaVersion;nullArtifact.valueOwner=ieeeOwner;
    const auto nullType=valueTypeFromCanonicalName("ref<\"frog.sync.queue\",string>");
    const auto nullValue=makeRuntimeNullReference(nullType.canonicalName,ieeeOwner);require(bool(nullValue),"NULL artifact fixture failed");
    for(auto &node:nullArtifact.nodes)node.ports.front().valueType=nullType;
    for(auto &binding:nullArtifact.uiBindings){binding.valueType=nullType;binding.initialValue=*nullValue;}
    const auto nullEncoding=encodeProgramArtifact(nullArtifact);const auto nullDecoded=decodeProgramArtifact(nullEncoding);
    require(!nullEncoding.empty()&&nullDecoded.artifact&&runtimeValuesEqual(nullDecoded.artifact->uiBindings.front().initialValue,*nullValue),"A complete artifact lost its intrinsic NULL binding");

    auto truncated = encoded;
    truncated.pop_back();
    const auto rejected = decodeProgramArtifact(truncated);
    assert(!rejected.artifact.has_value());
    assert(!rejected.diagnostics.empty());
    assert(valueTypeFromCanonicalName("u8").bitWidth == 8U);
    assert(valueTypeFromCanonicalName("array:bool").rank == 1U);
    assert(runtimeValueMatches(RuntimeValue{ true }, valueTypeFromCanonicalName("bool")));
    assert(!runtimeValueMatches(RuntimeValue{ true }, valueTypeFromCanonicalName("f64")));

    artifact.edges.front().source.portId = PortId{ "missing" };
    const auto diagnostics = validateProgramArtifact(artifact);
    assert(!diagnostics.empty());
    assert(diagnostics.front().blocksRun);
    return 0;
}
