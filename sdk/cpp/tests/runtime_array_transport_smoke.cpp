#include "frog/sdk/runtime_array_value.hpp"
#include "frog/sdk/runtime_contract_codec.hpp"
#include "frog/sdk/typed_event_packet.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
// Deterministic one-allocation fault, isolated to this transport test process.
// A real payload-size-or-larger allocation fails once BEFORE malloc. Allocator
// alignment padding/capacity is not the payload's exact byte count. Small
// owner/diagnostic allocations remain live; no production allocator is changed.
namespace allocation_fault {
std::atomic<std::size_t> minimumSize{0},requestedFailedSize{0};
std::atomic<unsigned> failureCount{0};
struct State {std::size_t armedMinimum,requested;unsigned failures;};
void arm(std::size_t minimum) noexcept {requestedFailedSize=0;failureCount=0;minimumSize=minimum;}
void clear() noexcept {minimumSize=0;requestedFailedSize=0;failureCount=0;}
State state() noexcept {return {minimumSize.load(),requestedFailedSize.load(),failureCount.load()};}
}
void *operator new(std::size_t size) {
    auto wanted=allocation_fault::minimumSize.load();
    if(wanted&&size>=wanted&&allocation_fault::minimumSize.compare_exchange_strong(wanted,0)) {
        allocation_fault::requestedFailedSize=size;
        ++allocation_fault::failureCount;
        throw std::bad_alloc{};
    }
    if(void *result=std::malloc(size?size:1))return result;throw std::bad_alloc{};
}
void operator delete(void *value) noexcept {std::free(value);}
void operator delete(void *value,std::size_t) noexcept {std::free(value);}
namespace rt=frog::sdk::runtime;namespace ev=frog::sdk::events;
namespace {
unsigned checks=0;std::string phase;
void require(bool okay,std::string message){++checks;if(!okay)throw std::runtime_error(phase+": "+message);}
rt::RuntimeValueOwner owner(std::uint64_t epoch=1){return {"owner",std::string(64,'a'),epoch,2,3};}
std::vector<std::uint8_t> hex(std::string_view text) {
    const auto digit=[](char ch)->unsigned{if(ch>='0'&&ch<='9')return ch-'0';if(ch>='a'&&ch<='f')return ch-'a'+10;throw std::runtime_error("Bad golden hex");};
    std::vector<std::uint8_t> result;for(std::size_t i=0;i<text.size();i+=2)result.push_back(static_cast<std::uint8_t>(digit(text[i])*16+digit(text[i+1])));return result;
}
rt::ProgramArtifact legacy(bool record) {
    rt::ProgramArtifact a;a.artifactId={"legacy"};a.sourceDocumentId={"source"};a.sourceDocumentUri="untitled:/legacy.frog";a.revision={1,2,3};
    if(record){a.schemaVersion=rt::kTypedArtifactSchemaVersion;a.valueOwner=owner();const auto value=rt::makeRuntimeRecord({{"x",rt::valueTypeFromCanonicalName("i64"),std::int64_t{7}}});require(value.has_value(),"Existing sealed Record factory failed");rt::RuntimeNode n;n.id={"record"};n.kind=rt::NodeKind::Constant;n.operationId="frog.core.constant";n.ports.push_back({{"value"},"value",rt::PortDirection::Output,rt::valueTypeFromCanonicalName(std::get<6>(*value)->canonicalType())});n.literalValue=*value;a.nodes.push_back(std::move(n));}return a;
}
rt::RuntimeF64Array array(std::span<const double> values,const rt::RuntimeValueOwner &authority=owner()){
    const auto result=rt::makeRuntimeF64Array(values,authority,rt::kMaximumF64ArrayPayloadBytes);require(result.accepted(),"Real packed factory refused valid payload");return result.value;
}
rt::ProgramArtifact artifact(rt::RuntimeF64Array value,unsigned copies=1) {
    rt::ProgramArtifact a;a.schemaVersion=rt::kPackedF64ArrayArtifactSchemaVersion;a.artifactId={"packed-array"};a.sourceDocumentId={"source"};a.sourceDocumentUri="untitled:/packed-array.frog";a.revision={1,2,3};a.valueOwner=value->owner();
    for(unsigned i=0;i<copies;++i){rt::RuntimeNode n;n.id={"array-"+std::to_string(i)};n.kind=rt::NodeKind::Constant;n.operationId="frog.core.constant";n.ports.push_back({{"value"},"value",rt::PortDirection::Output,rt::f64ArrayValueType()});n.literalValue=rt::RuntimeValue{value};a.nodes.push_back(std::move(n));}return a;
}
bool code(const std::vector<rt::Diagnostic> &errors,std::string_view wanted){return std::any_of(errors.begin(),errors.end(),[&](const auto &d){return d.code==wanted&&d.blocksRun;});}
void legacyBytes() {
    phase="ART1/ART2/tag6 byte compatibility";
    const auto first=hex("46524f47415254310100000001000000060000006c656761637906000000736f7572636515000000756e7469746c65643a2f6c65676163792e66726f6701000000000000000200000000000000030000000000000000000000000000000000000000000000"),second=hex("46524f47415254320100000002000000060000006c656761637906000000736f7572636515000000756e7469746c65643a2f6c65676163792e66726f67010000000000000002000000000000000300000000000000050000006f776e6572400000006161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616101000000000000000200000000000000030000000000000001000000060000007265636f7264021200000066726f672e636f72652e636f6e7374616e7400000000010000000500000076616c75650500000076616c75650110000000636c75737465723c2278223a6936343e0a00000000000000000000010610000000636c75737465723c2278223a6936343e01000000010000007803000000693634024000000000000002070000000000000000000000000000000000000000000000");
    for(bool record:{false,true}){const auto a=legacy(record);const auto &golden=record?second:first;require(rt::encodeProgramArtifact(a)==golden,"Existing ART1/2 bytes changed against independent fixed layout");const auto d=rt::decodeProgramArtifact(golden);require(d.artifact&&d.diagnostics.empty()&&rt::encodeProgramArtifact(*d.artifact)==golden,"Existing ART1/2 golden no longer decodes and re-encodes exactly");if(record)require(rt::runtimeValuesEqual(*a.nodes[0].literalValue,*d.artifact->nodes[0].literalValue),"Tag6 Record semantics changed");}
    require(std::variant_size_v<rt::RuntimeValue> == 8,"RuntimeValue alternative count changed unexpectedly");static_assert(std::is_same_v<std::variant_alternative_t<6,rt::RuntimeValue>,std::shared_ptr<const rt::CompoundRuntimeValue>>);static_assert(std::is_same_v<std::variant_alternative_t<7,rt::RuntimeValue>,rt::RuntimeF64Array>);
}
void factories() {
    phase="Factory bounds/ownership/no mutable alias";
    const auto empty=array({});require(empty->shape()==std::array<std::uint32_t,1>{0}&&empty->elements().empty()&&empty->payloadBytes()==0,"Empty is not a valid rank1 value");
    std::vector<double> values{-0.0,1.25,32};const auto a=array(values);values[1]=99;
    require(a->elements()[1]==1.25&&std::signbit(a->elements()[0])&&a->payloadBytes()==24,"Factory retained producer alias or lost -0");
    require(a->validForOwner(owner())&&!a->validForOwner(owner(2)),"Exact owner authority was lost");
    auto other=array(std::vector<double>{0.0,1.25,32},owner(2));require(rt::runtimeValuesEqual(rt::RuntimeValue{a},rt::RuntimeValue{other}),"Content equality incorrectly depends on owner or distinguishes +/-0");require(!rt::runtimeValueValidForOwner(rt::RuntimeValue{a},rt::f64ArrayValueType(),owner(2)),"Content equality granted foreign authority");
    require(rt::runtimeValueMatches(rt::RuntimeValue{a},rt::f64ArrayValueType())&&rt::runtimeValueRepresentable(rt::RuntimeValue{a},rt::f64ArrayValueType()),"Exact SDK descriptor was not admitted");
    for(const auto &wrong:{rt::valueTypeFromCanonicalName("array<f64>"),rt::valueTypeFromCanonicalName("array:i32"),rt::ValueType{"array:f64",rt::ValueCategory::Array,64,1,"f64"},rt::ValueType{"array:f64",rt::ValueCategory::Array,0,2,"f64"}})require(!rt::runtimeValueMatches(rt::RuntimeValue{a},wrong),"Wrong spelling/element/rank/width was silently coerced");
    require(!rt::runtimeValueMatches(rt::RuntimeValue{rt::RuntimeF64Array{}},rt::f64ArrayValueType()),"Null Array handle matched");
    require(!rt::makeRuntimeRecord({{"illegal",rt::f64ArrayValueType(),rt::RuntimeValue{a}}}),"Array was admitted inside Record");
    ev::PacketError error=ev::PacketError::None;require(!ev::FieldValue::scalar("array:f64",rt::RuntimeValue{a},&error)&&error==ev::PacketError::InvalidValue,"Array was admitted as an unbudgeted Event scalar");
    for(const auto invalid:{std::numeric_limits<double>::infinity(),-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}){const std::array<double,1> bad{invalid};const auto r=rt::makeRuntimeF64Array(bad,owner(),8);require(!r.accepted()&&!r.value&&r.failure==rt::F64ArrayFailure::NonFinite,"Factory accepted nonfinite or returned a partial array");}
    const auto noOwner=rt::makeRuntimeF64Array(values,{},24),shortBudget=rt::makeRuntimeF64Array(values,owner(),23),invalidBudget=rt::makeRuntimeF64Array(values,owner(),0);
    require(noOwner.failure==rt::F64ArrayFailure::OwnerInvalid&&!noOwner.value,"Factory accepted absent owner");require(shortBudget.failure==rt::F64ArrayFailure::PayloadLimit&&!shortBudget.value&&invalidBudget.failure==rt::F64ArrayFailure::PayloadLimit,"Factory allocated outside the explicit available payload");
    std::vector<double> maximum(1'000'001,1.5);maximum.front()=-0.0;maximum.back()=9.25;
    const auto full=rt::makeRuntimeF64Array(maximum,owner(),maximum.size()*sizeof(double));require(full.accepted()&&full.value->elements().size()==1'000'001&&full.value->payloadBytes()==8'000'008,"Packed Array retained the former1M element cap");
    require(rt::runtimeValueRepresentable(rt::RuntimeValue{full.value},rt::f64ArrayValueType()),"1M packed value was recursively traversed/rejected");
    const std::array<std::uint32_t,2> boundary{UINT32_MAX,1},overflow{UINT32_MAX,2};
    require(rt::kMaximumF64ArrayElements==UINT32_MAX&&rt::array_detail::elementCount(boundary)==std::optional<std::size_t>{UINT32_MAX}&&
        !rt::array_detail::elementCount(overflow),"Packed Array limit arithmetic lost the exact U32 maximum or overflow refusal");
    std::vector<double> fail(5000,1.0);const auto payload=fail.size()*sizeof(double);allocation_fault::arm(payload);const auto failed=rt::makeRuntimeF64Array(fail,owner(),rt::kMaximumF64ArrayPayloadBytes);const auto fault=allocation_fault::state();
    const bool allocationRejected=!failed.value&&failed.failure==rt::F64ArrayFailure::ResourceFailure&&fault.armedMinimum==0&&fault.failures==1&&fault.requested>=payload;
    if(!allocationRejected)std::cerr<<"Factory allocation injection: payload="<<payload<<" armed-minimum="<<fault.armedMinimum<<" failed-request="<<fault.requested<<" failures="<<fault.failures<<" failure-status="<<unsigned(failed.failure)<<" partial="<<bool(failed.value)<<'\n';
    require(allocationRejected,"Factory allocation failure escaped or produced a partial value");allocation_fault::clear();
}
std::size_t tagOffset(const std::vector<std::uint8_t> &bytes,std::uint32_t count) {
    const std::array<std::uint8_t,6> signature{7,1,static_cast<std::uint8_t>(count),static_cast<std::uint8_t>(count>>8),static_cast<std::uint8_t>(count>>16),static_cast<std::uint8_t>(count>>24)};
    const auto found=std::search(bytes.begin(),bytes.end(),signature.begin(),signature.end());require(found!=bytes.end()&&std::search(found+1,bytes.end(),signature.begin(),signature.end())==bytes.end(),"Negative fixture did not identify one actual tag7 boundary");return static_cast<std::size_t>(found-bytes.begin());
}
void rejects(std::vector<std::uint8_t> bytes,std::string_view reason){const auto d=rt::decodeProgramArtifact(bytes);require(!d.artifact&&!d.diagnostics.empty(),"Malformed packed codec input accepted: "+std::string(reason));}
void codec() {
    phase="Real schema3 roundtrip/guards before allocation";
    const auto a=array(std::vector<double>{-0.0,3.25});auto source=artifact(a);const auto bytes=rt::encodeProgramArtifact(source);require(!bytes.empty()&&std::string(bytes.begin(),bytes.begin()+8)=="FROGART3","Packed artifact did not opt into separate magic/schema3");const auto decoded=rt::decodeProgramArtifact(bytes);require(decoded.artifact&&decoded.diagnostics.empty(),"Actual packed artifact failed roundtrip");
    const auto actual=std::get<7>(*decoded.artifact->nodes[0].literalValue);require(actual&&actual!=a&&rt::runtimeValuesEqual(rt::RuntimeValue{a},rt::RuntimeValue{actual})&&std::bit_cast<std::uint64_t>(actual->elements()[0])==std::bit_cast<std::uint64_t>(-0.0)&&actual->validForOwner(*source.valueOwner),"Codec lost packed ownership, content or the negative-zero bits");require(rt::encodeProgramArtifact(*decoded.artifact)==bytes,"Schema3 roundtrip bytes drifted");
    rt::RuntimeCapabilities caps;caps.valueTypes={"array:f64"};require(code(rt::validateRuntimeCapabilities(source,caps),"capability.packed_f64_arrays.unavailable"),"Schema3 was enabled by listing type without opt-in");caps.supportsPackedF64Arrays=true;require(rt::validateRuntimeCapabilities(source,caps).empty()&&!caps.supportsTypedEventValues,"Array opt-in accidentally required/enabled Event schema2");
    auto wrong=source;wrong.valueOwner=owner(2);require(code(rt::validateProgramArtifact(wrong),"contract.array.owner_invalid")&&rt::encodeProgramArtifact(wrong).empty(),"Foreign artifact owner reused an old sealed Array");
    for(unsigned schema:{1U,2U}){wrong=source;wrong.schemaVersion=schema;if(schema==1)wrong.valueOwner.reset();require(code(rt::validateProgramArtifact(wrong),"contract.array.schema_invalid")&&rt::encodeProgramArtifact(wrong).empty(),"Tag7 carrier was encoded under old schema");}
    auto record=legacy(true);record.schemaVersion=rt::kPackedF64ArrayArtifactSchemaVersion;require(!rt::validateProgramArtifact(record).empty()&&rt::encodeProgramArtifact(record).empty(),"Schema3 opened existing Record transport");
    wrong=source;wrong.nodes[0].selectedFields={"field"};require(!rt::validateProgramArtifact(wrong).empty(),"Schema3 admitted Record selections");wrong=source;wrong.uiBindings.push_back({{"event-input"},{{"array-0"},{"value"}},rt::UiBindingDirection::ControlToRuntime,rt::f64ArrayValueType(),{},true});require(code(rt::validateProgramArtifact(wrong),"contract.event_input.invalid"),"Schema3 admitted suppliedByEvent");
    const auto at=tagOffset(bytes,2);auto bad=bytes;bad[at+1]=2;rejects(std::move(bad),"rank2");bad=bytes;bad[at]=8;rejects(std::move(bad),"unknown tag");bad=bytes;bad.resize(at+6+8);rejects(std::move(bad),"missing packed element");
    bad=bytes;bad[7]='2';rejects(std::move(bad),"magic/schema mismatch");bad=bytes;bad[7]='2';bad[12]=2;rejects(std::move(bad),"tag7 in ART2");bad=bytes;bad[7]='1';bad[12]=1;rejects(std::move(bad),"tag7 in ART1");
    const auto raw=[&](){rt::codec_detail::Writer writer(false,true,&*source.valueOwner);writer.runtimeValue(rt::RuntimeValue{a});return std::move(writer).finish();}();
    for(bool typed:{false,true}){rt::codec_detail::Reader old(raw);require(old.typed(typed),"Old envelope mode was unexpectedly invalid");old.arrayOwner(*source.valueOwner);rt::RuntimeValue unchanged;require(!old.runtimeValue(unchanged)&&std::holds_alternative<std::monostate>(unchanged),"Actual old-schema value reader admitted tag7");}
    bad=bytes;const auto inf=std::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());for(unsigned i=0;i<8;++i)bad[at+6+i]=static_cast<std::uint8_t>(inf>>(i*8));rejects(std::move(bad),"nonfinite packed bits");
    bad=bytes;const std::uint32_t excess=UINT32_MAX;for(unsigned i=0;i<4;++i)bad[at+2+i]=static_cast<std::uint8_t>(excess>>(i*8));const auto oversizedPayload=static_cast<std::size_t>(excess)*8;allocation_fault::arm(oversizedPayload);rejects(std::move(bad),"U32-max count with truncated payload before allocation");const auto oversizedFault=allocation_fault::state();
    const bool refusedBeforeAllocation=oversizedFault.armedMinimum==oversizedPayload&&oversizedFault.failures==0&&oversizedFault.requested==0;
    if(!refusedBeforeAllocation)std::cerr<<"Oversized count guard: payload="<<oversizedPayload<<" armed-minimum="<<oversizedFault.armedMinimum<<" failed-request="<<oversizedFault.requested<<" failures="<<oversizedFault.failures<<'\n';
    require(refusedBeforeAllocation,"Oversized count allocated before refusal");allocation_fault::clear();
    std::vector<double> fail(5000,1.0);const auto failBytes=rt::encodeProgramArtifact(artifact(array(fail)));const auto payload=fail.size()*sizeof(double);allocation_fault::arm(payload);const auto noAllocation=rt::decodeProgramArtifact(failBytes);const auto decoderFault=allocation_fault::state();
    const bool allocationRejected=!noAllocation.artifact&&!noAllocation.diagnostics.empty()&&decoderFault.armedMinimum==0&&decoderFault.failures==1&&decoderFault.requested>=payload;
    if(!allocationRejected)std::cerr<<"Decoder allocation injection: payload="<<payload<<" armed-minimum="<<decoderFault.armedMinimum<<" failed-request="<<decoderFault.requested<<" failures="<<decoderFault.failures<<" partial-artifact="<<bool(noAllocation.artifact)<<" diagnostics="<<noAllocation.diagnostics.size()<<'\n';
    require(allocationRejected,"Decoder bad_alloc escaped or returned a partial artifact");allocation_fault::clear();
}
void aggregate() {
    phase="Large packed Arrays and allocation failure without a fixed payload quota";
    std::vector<double> values(2'100'000,2.5);values[0]=-0.0;const auto full=array(values);const auto source=artifact(full,2);const auto bytes=rt::encodeProgramArtifact(source);require(!bytes.empty()&&bytes.size()>32U*1024U*1024U,"Array payload remained limited by the former32MiB typed envelope quota");const auto decoded=rt::decodeProgramArtifact(bytes);require(decoded.artifact&&decoded.artifact->nodes.size()==2,"Large Array decoder did not admit two actual arrays");for(const auto &n:decoded.artifact->nodes){const auto &a=std::get<7>(*n.literalValue);require(a->payloadBytes()==16'800'000&&a->elements().size()==2'100'000&&std::signbit(a->elements()[0]),"Large packed codec consumed recursive elements or lost bits");}
    // Direct Reader is the same real decoder used above. Three actual blocks
    // prove that actual allocation failure, rather than a16MB quota, is atomic.
    const auto valueBytes=[&](){rt::codec_detail::Writer w(false,true,&*source.valueOwner);w.runtimeValue(rt::RuntimeValue{full});return std::move(w).finish();}();require(!valueBytes.empty(),"Real packed Writer failed");std::vector<std::uint8_t> three;three.reserve(valueBytes.size()*3);for(unsigned i=0;i<3;++i)three.insert(three.end(),valueBytes.begin(),valueBytes.end());rt::codec_detail::Reader reader(three);require(reader.typed(true,true),"Packed reader envelope rejected bounded fixture");reader.arrayOwner(*source.valueOwner);rt::RuntimeValue first,second,third;require(reader.runtimeValue(first)&&reader.runtimeValue(second),"Reader did not commit two exact8M packed values");allocation_fault::arm(8'000'000);const bool thirdRead=reader.runtimeValue(third);const auto thirdFault=allocation_fault::state();
    const bool thirdRejected=!thirdRead&&std::holds_alternative<std::monostate>(third)&&thirdFault.armedMinimum==0&&thirdFault.failures==1&&thirdFault.requested>=8'000'000;
    if(!thirdRejected)std::cerr<<"Third packed payload guard: armed-minimum="<<thirdFault.armedMinimum<<" failed-request="<<thirdFault.requested<<" failures="<<thirdFault.failures<<" partial-alternative="<<third.index()<<" read="<<thirdRead<<'\n';
    require(thirdRejected,"Actual Array allocation failure escaped or published a partial value");allocation_fault::clear();
}
void genericArray() {
    phase="Generic Array8192 and atomic allocation failure";
    std::vector<rt::RuntimeValue> elements(8192,rt::RuntimeValue{std::uint64_t{9007199254740993ULL}});
    elements.back()=std::uint64_t{UINT64_MAX};
    const auto authority=owner();
    const auto value=rt::makeRuntimeArray(rt::valueTypeFromCanonicalName("u64"),{64,128},std::move(elements),authority);
    require(value&&rt::runtimeValueValidForOwner(*value,rt::valueTypeFromCanonicalName("array<u64,rank=2>"),authority),"Generic Array8192 retained its recursive4096 cap");
    rt::codec_detail::Writer writer(true,false,&authority);writer.runtimeValue(*value);const auto bytes=std::move(writer).finish();
    rt::codec_detail::Reader reader(bytes);reader.typed(true);reader.arrayOwner(authority);rt::RuntimeValue decoded;
    require(!bytes.empty()&&reader.runtimeValue(decoded)&&reader.finished()&&rt::runtimeValuesEqual(*value,decoded),"Generic Array8192 lost rank, row-major values or exact u64 data");
    rt::codec_detail::Reader failed(bytes);failed.typed(true);failed.arrayOwner(authority);rt::RuntimeValue unchanged;
    const auto payload=std::size_t{8192}*sizeof(rt::RuntimeValue);allocation_fault::arm(payload);
    const bool accepted=failed.runtimeValue(unchanged);const auto failure=allocation_fault::state();allocation_fault::clear();
    require(!accepted&&std::holds_alternative<std::monostate>(unchanged)&&failure.failures==1&&failure.requested>=payload&&
        failed.error().find("Not enough memory")!=std::string_view::npos,"Generic Array allocation failure escaped or published partial state");
}
}
int main(){try{legacyBytes();factories();codec();aggregate();genericArray();std::cout<<"SDK_PACKED_F64_ARRAY_PASS "<<checks<<" checks; schema3/tag7 U32 capacity, actual arrays above1M and32MiB, generic Array8192, exact owner, allocation failure, -0 codec, sealed-copy and ART1/2/tag6 preservation; no Runtime/Source/operator/UI capability claimed\n";return 0;}catch(const std::exception &e){allocation_fault::clear();std::cerr<<e.what()<<'\n';return 1;}}
