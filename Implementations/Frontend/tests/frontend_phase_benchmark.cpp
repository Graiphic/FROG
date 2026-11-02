// SPDX-License-Identifier: Apache-2.0
// Finite production-frontend benchmark. No execution/backend surrogate.
#include <frog/frontend.h>
#include "frog_native_engine/CanonicalExecutionIr.hpp"
#include "frog_native_engine/CanonicalEventExecutionIr.hpp"
#include "frog_native_engine/CanonicalFrontendMetrics.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
namespace ne=frog::native_engine;
namespace cj=ne::canonical_json;
namespace fs=std::filesystem;
using Json=ne::source::JsonValue;
using Clock=std::chrono::steady_clock;
using Ns=std::uint64_t;
void require(bool value,std::string message){if(!value)throw std::runtime_error(std::move(message));}
Ns elapsed(Clock::time_point begin){return static_cast<Ns>(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-begin).count());}
Json number(Ns v){Json out;out.type=Json::Type::Number;out.string=std::to_string(v);return out;}
frog_frontend_utf8 span(std::string_view value){return {value.data(),value.size()};}
std::string_view span(frog_frontend_utf8 value){require(value.data||!value.size,"Malformed result span.");return {value.data?value.data:"",value.size};}
struct Context {
    frog_frontend_context *value=nullptr;
    Context(){frog_frontend_config c{};c.struct_size=sizeof(c);require(frog_frontend_context_create(FROG_FRONTEND_API_VERSION,&c,&value)==FROG_FRONTEND_OK,"Cannot create real API context.");}
    ~Context(){frog_frontend_context_destroy(value);}
};
struct Result {frog_frontend_result *value=nullptr;~Result(){frog_frontend_result_destroy(value);}};
frog_frontend_result_view view(const Result &result){frog_frontend_result_view v{};v.struct_size=sizeof(v);require(frog_frontend_result_get(result.value,&v)==FROG_FRONTEND_OK,"Cannot get real result.");return v;}
Json json(std::string_view value){std::string error;auto j=ne::source::parseJson(value,&error,ne::source::JsonNumberPolicy::PreserveLexeme);require(bool(j),error);return std::move(*j);}
std::shared_ptr<const std::string> read(const fs::path &path){require(fs::file_size(path)<=8U*1024U*1024U,"Benchmark fixture exceeds the bounded corpus.");std::ifstream file(path,std::ios::binary);require(bool(file),"Cannot read benchmark fixture.");return std::make_shared<const std::string>(std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>{});}
bool isEvent(const Json &root){
    const auto *diagram=root.member("diagram");if(!diagram)return false;
    if(const auto *profile=diagram->member("profile");profile&&profile->string=="frog.studio.composed-program@1")return false;
    const auto *nodes=diagram->member("nodes");if(!nodes)return false;
    return std::any_of(nodes->array.begin(),nodes->array.end(),[](const auto &node){const auto *type=node.member("structure_type");return type&&type->string=="event_structure";});
}
std::string why(const std::vector<ne::CanonicalIrDiagnostic> &diagnostics){std::string out;for(const auto &d:diagnostics)out+=d.code+"@"+d.location+": "+d.message+"; ";return out;}
struct Observation {
    Ns captureOwned=0,captureCopy=0,directWall=0,directWithoutMetrics=0,apiWall=0,eventReaderWall=0;
    ne::CanonicalFrontendMetrics metrics,eventReaderMetrics;
    std::size_t firBytes=0;
};
Observation observe(const std::shared_ptr<const std::string> &owner,Context &context){
    Observation result;std::string error;
    std::optional<ne::FrogSourceDocument> source;
    {
        const ne::CanonicalFrontendPhaseScope capture(&result.metrics,ne::CanonicalFrontendPhase::Capture);
        source=ne::FrogSourceDocument::readOwned(owner,&error);
    }
    result.captureOwned=result.metrics.captureNanoseconds;
    require(bool(source),error);require(source->originalUtf8().data()==owner->data(),"readOwned copied the captured Source buffer.");
    {
        const auto begin=Clock::now();auto copied=ne::FrogSourceDocument::read(*owner,&error);result.captureCopy=elapsed(begin);
        require(bool(copied),error);require(copied->originalUtf8()==*owner,"The legacy copied read changed Source bytes.");
        require(copied->originalUtf8().data()!=owner->data(),"Copied-reader comparison did not own a distinct source capture.");
    }
    ne::CanonicalIrReadOptions options;options.sourceUri="memory:frontend-phase-benchmark";options.metrics=&result.metrics;
    const std::string documentId="benchmark:document";
    std::vector<ne::CanonicalIrDiagnostic> diagnostics;
    std::optional<ne::CanonicalExecutionIr> regular;
    std::optional<ne::CanonicalEventExecutionIr> event;
    const bool eventProfile=isEvent(source->root());
    {
        const auto begin=Clock::now();
        if(eventProfile)event=ne::CanonicalEventExecutionIr::derive(*source,documentId,options,&diagnostics);
        else regular=ne::CanonicalExecutionIr::derive(*source,documentId,options,&diagnostics);
        result.directWall=elapsed(begin);
    }
    require(bool(regular)||bool(event),"Instrumented production derive refused: "+why(diagnostics));
    require(!result.metrics.phaseActive,"An instrumented phase retained ownership after returning.");
    require(result.metrics.semanticCalls&&result.metrics.firConstructionCalls,"Actual admission or FIR construction boundary was not measured.");
    const auto artifact=regular?regular->originalUtf8():event->originalUtf8();result.firBytes=artifact.size();
    options.metrics=nullptr;
    {
        const auto begin=Clock::now();
        if(eventProfile){auto unmeasured=ne::CanonicalEventExecutionIr::derive(*source,documentId,options,&diagnostics);result.directWithoutMetrics=elapsed(begin);require(bool(unmeasured),why(diagnostics));require(unmeasured->originalUtf8()==artifact,"Instrumentation changed Event FIR.");}
        else {auto unmeasured=ne::CanonicalExecutionIr::derive(*source,documentId,options,&diagnostics);result.directWithoutMetrics=elapsed(begin);require(bool(unmeasured),why(diagnostics));require(unmeasured->originalUtf8()==artifact,"Instrumentation changed FIR.");}
    }
    if(eventProfile){
        // Event::derive does not call read. Measure its real sealed reader as a
        // separate operation; do not pretend the API performed this extra call.
        options.metrics=&result.eventReaderMetrics;const auto begin=Clock::now();
        auto sealed=ne::CanonicalEventExecutionIr::read(artifact,*source,options,&diagnostics);result.eventReaderWall=elapsed(begin);
        require(bool(sealed),"Event reader refused its real production artifact: "+why(diagnostics));
        require(sealed->originalUtf8()==artifact,"Event reader changed the supplied artifact.");
        require(result.eventReaderMetrics.sealingCalls==1&&!result.eventReaderMetrics.semanticCalls&&!result.eventReaderMetrics.firConstructionCalls,
            "Event sealing double-counted its mandatory reference rederivation.");
    }else require(result.metrics.sealingCalls==1,"The production derive did not measure its real sealed reader.");
    frog_frontend_request request{};request.struct_size=sizeof(request);request.source_bytes=span(*owner);request.source_uri=span(options.sourceUri);
    request.source_document_id=span("source:frontend-phase-benchmark");request.fir_document_id=span(documentId);request.source_format_id=span("frog.source");request.source_format_version=span("0.1");request.fir_format_version=span("0.1");
    Result api;
    const auto begin=Clock::now();const auto status=frog_frontend_derive(context.value,&request,&api.value);result.apiWall=elapsed(begin);
    require(status==FROG_FRONTEND_OK,"The real API derive returned an API error.");const auto actual=view(api);
    require(actual.outcome==FROG_FRONTEND_SUCCESS,"The real API derive rejected the same fixture: "+std::string(span(actual.diagnostics_json)));
    require(span(actual.fir_json)==artifact,"Direct and API production derivation emitted different FIR for the same exact inputs.");
    require(!actual.resolved_source_json.size,"A benchmark without dependency projection returned a redundant Source copy.");
    const auto actualFir=json(span(actual.fir_json));const auto *actualIdentity=actualFir.member("document_id");
    require(actualIdentity&&actualIdentity->type==Json::Type::String&&actualIdentity->string==documentId,"The API replaced the explicit benchmark FIR identity.");
    require(source->originalUtf8().data()==owner->data()&&source->originalUtf8()==*owner,"The pipeline changed or recopied its retained authored Source.");
    return result;
}
Json distribution(std::vector<Ns> values){
    require(!values.empty(),"An empty timing series is not a measurement.");std::sort(values.begin(),values.end());
    long double mean=0;for(auto value:values)mean+=value;mean/=values.size();long double variance=0;for(auto value:values){const auto difference=value-mean;variance+=difference*difference;}variance/=values.size();
    const auto middle=values.size()/2;
    const auto median=values.size()%2?values[middle]:values[middle-1]+(values[middle]-values[middle-1])/2;
    auto out=cj::object(),samples=cj::array();for(auto value:values)samples.array.push_back(number(value));
    out.object={{"unit",cj::string("ns")},{"minimum",number(values.front())},{"median",number(median)},
        {"maximum",number(values.back())},{"mean",number(static_cast<Ns>(mean))},{"standard_deviation",number(static_cast<Ns>(std::sqrt(variance)))},{"samples_sorted",std::move(samples)}};return out;
}
template<class Extract> Json series(const std::vector<Observation> &values,Extract extract){std::vector<Ns> times;for(const auto &value:values)times.push_back(extract(value));return distribution(std::move(times));}
Json benchmark(const fs::path &directory,unsigned repetitions){
    Context context;Result description;require(frog_frontend_describe(context.value,&description.value)==FROG_FRONTEND_OK,"Describe failed.");
    const auto described=view(description);require(described.outcome==FROG_FRONTEND_SUCCESS,"Describe was not successful.");
    auto report=cj::object(),cases=cj::array();
    for(const auto name:{"pure-add.frog","record-exact.frog","nominal-nested.frog","event-values.frog","pane-unbundle.frog","event-product.frog"}) {
        const auto owner=read(directory/name);
        for(unsigned warmup=0;warmup<2;++warmup)(void)observe(owner,context);
        std::vector<Observation> observations;for(unsigned repetition=0;repetition<repetitions;++repetition)observations.push_back(observe(owner,context));
        auto item=cj::object();item.object={{"fixture",cj::string(name)},{"source_bytes",number(owner->size())},{"fir_bytes",number(observations.front().firBytes)},
            {"capture_read_owned",series(observations,[](const auto &v){return v.captureOwned;})},
            {"capture_read_copy",series(observations,[](const auto &v){return v.captureCopy;})},
            {"direct_product_instrumented",series(observations,[](const auto &v){return v.directWall;})},
            {"direct_product_uninstrumented",series(observations,[](const auto &v){return v.directWithoutMetrics;})},
            {"admission_or_semantics",series(observations,[](const auto &v){return v.metrics.semanticNanoseconds;})},
            {"fir_construction",series(observations,[](const auto &v){return v.metrics.firConstructionNanoseconds;})},
            {"sealing_including_reference_rederivation",series(observations,[](const auto &v){return v.metrics.sealingNanoseconds;})},
            {"unclassified_direct_work",series(observations,[](const auto &v){const auto known=v.metrics.semanticNanoseconds+v.metrics.firConstructionNanoseconds+v.metrics.sealingNanoseconds;return v.directWall>known?v.directWall-known:0;})},
            {"api_end_to_end",series(observations,[](const auto &v){return v.apiWall;})},
            {"event_admission_includes_body_record_assembly",cj::boolean(observations.front().metrics.eventAdmissionIncludesAssembly)},
            {"event_explicit_reader_wall",series(observations,[](const auto &v){return v.eventReaderWall;})},
            {"event_explicit_reader_sealing",series(observations,[](const auto &v){return v.eventReaderMetrics.sealingNanoseconds;})}};
        cases.array.push_back(std::move(item));
    }
    report.object={{"schema",cj::string("frog.frontend.phase-benchmark")},{"version",cj::string("1")},
        {"clock",cj::string("std::chrono::steady_clock")},{"pointer_bytes",number(sizeof(void*))},
        {"warmups_per_fixture",number(2)},{"repetitions_per_fixture",number(repetitions)},
        {"frontend_descriptor",json(span(described.descriptor_json))},{"cases",std::move(cases)},
        {"memory_measured",cj::boolean(false)},{"source_read_owned_identity_verified",cj::boolean(true)},
        {"instrumentation_fir_parity_verified",cj::boolean(true)},{"api_direct_fir_parity_verified",cj::boolean(true)},
        {"notes",cj::string("File IO excluded. Direct phases are exclusive; sealing absorbs reference rederivation. API timings are actual total calls, not attributed internal phases. Event admission includes interleaved body-record assembly; Event derive has no reader, so its explicit read is separate. Final result-copy time is not called FIR generation. Sequential paired timings retain scheduler/cache/order noise; no gain is inferred automatically.")}};
#ifdef _MSC_FULL_VER
    report.object["msvc_full_ver"]=number(_MSC_FULL_VER);
#endif
#ifdef __clang_version__
    report.object["clang_version"]=cj::string(__clang_version__);
#endif
    return report;
}
} // namespace

int main(int argc,char **argv){
    try{
        require(argc>=2&&argc<=4,"Usage: frog-frontend-phase-benchmark FIXTURE_DIRECTORY [OUTPUT_JSON] [REPETITIONS]");
        unsigned repetitions=7;if(argc==4){const auto value=std::stoul(argv[3]);require(value>=3&&value<=101,"Repetitions must be3..101.");repetitions=static_cast<unsigned>(value);}
        const auto report=cj::encode(benchmark(fs::u8path(argv[1]),repetitions));
        if(argc>=3){std::ofstream out(fs::u8path(argv[2]),std::ios::binary);require(bool(out),"Cannot create the explicit benchmark report.");out<<report;require(bool(out),"Cannot persist the complete benchmark report.");}
        std::cout<<report<<'\n';return 0;
    }catch(const std::exception &error){std::cerr<<"FRONTEND_PHASE_BENCHMARK_FAIL: "<<error.what()<<'\n';return 1;}
}
