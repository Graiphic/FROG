// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/graph/WaveformGraph.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include "frog_native_engine/TimestampSourceProjection.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <limits>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace frog::native_engine::graph {
namespace {
using Type=binding::TypeRef;using Kind=binding::TypeKind;using Json=source::JsonValue;
enum class Form {Invalid,Y,Rows,TimedY,TimedRows,Plots,TimedPlots,TimedArray,Waveform,Waveforms};
bool real(const Type &t) {
    constexpr std::string_view names[]{"i8","u8","i16","u16","i32","u32","i64","u64","f32","f64"};
    return t.kind==Kind::Primitive && t.children.empty() && std::find(std::begin(names),std::end(names),t.name)!=std::end(names);
}
bool record(const Type &t,std::size_t count) {
    if(t.kind!=Kind::Cluster || t.children.size()!=count || t.fieldIds.size()!=count)return false;
    std::set<std::string_view> ids;for(const auto &id:t.fieldIds)if(id.empty() || !ids.insert(id).second)return false;return true;
}
bool array(const Type &t,std::size_t rank) {return t.kind==Kind::Array && t.rank==rank && t.children.size()==1;}
bool ys(const Type &t,std::size_t rank) {return array(t,rank) && real(t.children[0]);}
bool plot(const Type &t) {return record(t,1) && ys(t.children[0],1);}
bool plots(const Type &t) {return array(t,1) && plot(t.children[0]);}
bool timed(const Type &t) {return record(t,3) && real(t.children[0]) && real(t.children[1]) && ys(t.children[2],1);}
std::size_t index(const Type &t,std::string_view id) {return std::size_t(std::find(t.fieldIds.begin(),t.fieldIds.end(),id)-t.fieldIds.begin());}
bool waveform(const Type &t) {
    if(!record(t,3))return false;const auto start=index(t,"t0"),step=index(t,"dt"),data=index(t,"Y");
    return start<3 && step<3 && data<3 && t.children[start]==Type::primitive("timestamp") &&
        t.children[step]==Type::primitive("f64") && ys(t.children[data],1);
}
Form form(const Type &t) {
    if(!t.known())return Form::Invalid;
    if(ys(t,1))return Form::Y;if(ys(t,2))return Form::Rows;
    if(plots(t))return Form::Plots;
    if(array(t,1) && timed(t.children[0]))return Form::TimedArray;
    if(waveform(t))return Form::Waveform;
    if(array(t,1) && waveform(t.children[0]))return Form::Waveforms;
    if(record(t,3) && real(t.children[0]) && real(t.children[1])) {
        if(ys(t.children[2],1))return Form::TimedY;
        if(ys(t.children[2],2))return Form::TimedRows;
        if(plots(t.children[2]))return Form::TimedPlots;
    }
    return Form::Invalid;
}
const Json &field(const Json &value,std::string_view id) {
    const auto *found=value.member(id);if(!found)throw std::runtime_error("WFG_FIELD: missing declared field '"+std::string(id)+"'.");return *found;
}
void fields(const Json &value,const Type &type) {
    if(value.type!=Json::Type::Object || value.object.size()!=type.fieldIds.size())throw std::runtime_error("WFG_RECORD: exactly the declared stable fields are required.");
    for(const auto &id:type.fieldIds)(void)field(value,id);
}
std::string token(const Json &value) {
    if(value.type!=Json::Type::Number)throw std::runtime_error("WFG_NUMBER: a typed JSON number is required.");
    if(!value.string.empty())return value.string;
    if(!std::isfinite(value.number))throw std::runtime_error("WFG_NUMBER: nonfinite samples require explicit NaN/Inf/-Inf markers.");
    std::array<char,64> buffer{};const auto parsed=std::to_chars(buffer.data(),buffer.data()+buffer.size(),value.number);
    if(parsed.ec!=std::errc{})throw std::runtime_error("WFG_NUMBER: cannot encode the numeric projection.");return {buffer.data(),parsed.ptr};
}
template<class Integer> Integer integer(const Json &value,std::string_view type) {
    const auto raw=token(value);const auto exact=numeric_value::canonicalWideIntegerLexeme(raw,type);
    if(!exact)throw std::runtime_error("WFG_NUMBER: integer is outside its exact declared representation.");
    Integer result{};const auto parsed=std::from_chars(exact->data(),exact->data()+exact->size(),result);
    if(parsed.ec!=std::errc{} || parsed.ptr!=exact->data()+exact->size())throw std::runtime_error("WFG_NUMBER: integer is outside its exact declared representation.");return result;
}
double scalar(const Type &type,const Json &value,bool gaps) {
    if(type.integer()) {
        const auto width=unsigned(std::stoi(type.name.substr(1)));
        if(type.name.front()=='u') {
            const auto n=integer<std::uint64_t>(value,"u64");
            if(width<64 && n>((std::uint64_t{1}<<width)-1))throw std::runtime_error("WFG_NUMBER: unsigned sample exceeds "+type.name);
            return static_cast<double>(n);
        }
        const auto n=integer<std::int64_t>(value,"i64");
        if(width<64 && (n<-(std::int64_t{1}<<(width-1)) || n>((std::int64_t{1}<<(width-1))-1)))throw std::runtime_error("WFG_NUMBER: signed sample exceeds "+type.name);
        return static_cast<double>(n);
    }
    if(value.type==Json::Type::String && gaps) {
        if(value.string=="NaN")return std::numeric_limits<double>::quiet_NaN();
        if(value.string=="Inf")return std::numeric_limits<double>::infinity();
        if(value.string=="-Inf")return -std::numeric_limits<double>::infinity();
    }
    const auto raw=token(value);
    if(type.name=="f32") {
        float n{};const auto parsed=std::from_chars(raw.data(),raw.data()+raw.size(),n);
        if(parsed.ec!=std::errc{} || parsed.ptr!=raw.data()+raw.size() || !std::isfinite(n))throw std::runtime_error("WFG_NUMBER: value exceeds f32.");return double(n);
    }
    double n{};const auto parsed=std::from_chars(raw.data(),raw.data()+raw.size(),n);
    if(parsed.ec!=std::errc{} || parsed.ptr!=raw.data()+raw.size() || !std::isfinite(n))throw std::runtime_error("WFG_NUMBER: value exceeds f64.");return n;
}
double timestamp(const Json &value) {
    if(value.type!=Json::Type::Object || value.object.size()!=2)throw std::runtime_error("WFG_TIMESTAMP: signed seconds and unsigned fraction are required.");
    // The existing signed64.64 Source/OwnedValue codec carries these two
    // integer parts as exact strings. Numeric JSON parts remain accepted;
    // this exception belongs only to Timestamp, never ordinary Y samples.
    const auto read=[](const Json &part,auto &result) {
        const auto raw=part.type==Json::Type::String?part.string:token(part);
        const auto parsed=std::from_chars(raw.data(),raw.data()+raw.size(),result);
        if(parsed.ec!=std::errc{} || parsed.ptr!=raw.data()+raw.size())throw std::runtime_error("WFG_TIMESTAMP: a part is outside its exact signed64.64 representation.");
    };
    timestamp_source::Parts parts{};read(field(value,"seconds"),parts.seconds);read(field(value,"fraction"),parts.fraction);
    return static_cast<double>(parts.seconds)+std::ldexp(static_cast<double>(parts.fraction),-64);
}
struct ArrayView {std::span<const Json> cells;std::array<std::size_t,2> shape{};bool nested=false;};
ArrayView elements(const Json &value,const Type &type) {
    ArrayView result;const Json *data=&value;
    if(value.type==Json::Type::Object) {
        if(value.object.size()!=2)throw std::runtime_error("WFG_ARRAY: only shape and elements are accepted.");
        const auto &shape=field(value,"shape");data=&field(value,"elements");
        if(shape.type!=Json::Type::Array || shape.array.size()!=type.rank || data->type!=Json::Type::Array)throw std::runtime_error("WFG_ARRAY: the array must preserve its declared rank.");
        std::size_t count=1;
        for(std::size_t axis=0;axis<type.rank;++axis) {
            const auto n=integer<std::uint64_t>(shape.array[axis],"u64");
            if(n>maximumSamples || (n && count>maximumSamples/n))throw std::runtime_error("WFG_SAMPLE_LIMIT: array shape exceeds one million elements.");
            result.shape[axis]=static_cast<std::size_t>(n);count*=static_cast<std::size_t>(n);
        }
        if(count!=data->array.size())throw std::runtime_error("WFG_ARRAY: shape and element count disagree.");
    } else {
        if(value.type!=Json::Type::Array)throw std::runtime_error("WFG_ARRAY: an Array is required.");
        result.shape[0]=value.array.size();
        if(type.rank==2) {
            result.nested=true;
            result.shape[1]=value.array.empty()?0:value.array.front().array.size();
            for(const auto &row:value.array)if(row.type!=Json::Type::Array || row.array.size()!=result.shape[1])throw std::runtime_error("WFG_ARRAY: numeric rows must form a rectangular matrix.");
            if(result.shape[0]>maximumSeries || (result.shape[1] && result.shape[0]>maximumSamples/result.shape[1]))throw std::runtime_error("WFG_SAMPLE_LIMIT: matrix exceeds graph quotas.");
        }
    }
    if(type.extent && (type.rank==1?result.shape[0]:result.shape[0]*result.shape[1])!=*type.extent)throw std::runtime_error("WFG_ARRAY: value does not match its declared extent.");
    result.cells=data->array;return result;
}
void append(Dataset &out,std::span<const Json> values,const Type &leaf,double origin,double step,std::size_t &total) {
    if(out.series.size()>=maximumSeries)throw std::runtime_error("WFG_SERIES_LIMIT: at most 64 series.");
    if(values.size()>maximumSamples-total)throw std::runtime_error("WFG_SAMPLE_LIMIT: at most one million samples.");
    if(!std::isfinite(origin) || !std::isfinite(step) || step<=0)throw std::runtime_error("WFG_X_DOMAIN: finite origin and positive finite sample interval are required.");
    Series series;series.id="series_"+std::to_string(out.series.size());series.name="Plot "+std::to_string(out.series.size());
    series.origin=origin;series.step=step;series.y.reserve(values.size());
    for(const auto &sample:values)series.y.push_back(scalar(leaf,sample,true));
    total+=values.size();out.series.push_back(std::move(series));
}
void numericArrays(Dataset &out,const Type &type,const Json &value,double origin,double step,std::size_t &total) {
    const auto data=elements(value,type);
    if(type.rank==1)append(out,data.cells,type.children[0],origin,step,total);
    else for(std::size_t row=0;row<data.shape[0];++row) {
        const auto cells=data.nested?std::span<const Json>(data.cells[row].array):data.cells.subspan(row*data.shape[1],data.shape[1]);
        append(out,cells,type.children[0],origin,step,total);
    }
}
void plotArrays(Dataset &out,const Type &type,const Json &value,double origin,double step,std::size_t &total) {
    const auto data=elements(value,type);if(data.cells.size()>maximumSeries-out.series.size())throw std::runtime_error("WFG_SERIES_LIMIT: at most 64 series.");
    const auto &item=type.children[0];for(const auto &entry:data.cells){fields(entry,item);numericArrays(out,item.children[0],field(entry,item.fieldIds[0]),origin,step,total);}
}
void timedRecord(Dataset &out,const Type &type,const Json &value,bool wave,std::size_t &total) {
    fields(value,type);const auto start=wave?index(type,"t0"):0,interval=wave?index(type,"dt"):1,data=wave?index(type,"Y"):2;
    const double origin=wave?timestamp(field(value,type.fieldIds[start])):scalar(type.children[start],field(value,type.fieldIds[start]),false);
    const double step=scalar(type.children[interval],field(value,type.fieldIds[interval]),false);
    if(!std::isfinite(origin) || !std::isfinite(step) || step<=0)throw std::runtime_error("WFG_X_DOMAIN: finite origin and positive finite sample interval are required.");
    if(plots(type.children[data]))plotArrays(out,type.children[data],field(value,type.fieldIds[data]),origin,step,total);
    else numericArrays(out,type.children[data],field(value,type.fieldIds[data]),origin,step,total);
}
Json arrayValue() {Json value;value.type=Json::Type::Array;return value;}
Json objectValue() {Json value;value.type=Json::Type::Object;return value;}
Json numberValue(double n) {
    Json value;
    if(!std::isfinite(n)){value.type=Json::Type::String;value.string=std::isnan(n)?"NaN":n<0?"-Inf":"Inf";return value;}
    value.type=Json::Type::Number;value.number=n;std::array<char,64> buffer{};const auto result=std::to_chars(buffer.data(),buffer.data()+buffer.size(),n);value.string.assign(buffer.data(),result.ptr);return value;
}
Json yValue(const Series &series) {auto result=arrayValue();for(double y:series.y)result.array.push_back(numberValue(y));return result;}
Json legacyTimedValue(const Type &type,const Series &series) {
    if(!timed(type))throw std::runtime_error("WFG_LEGACY_TYPE: timed legacy data requires a faithful numeric timing tuple.");
    auto value=objectValue();value.object.emplace(type.fieldIds[0],numberValue(series.origin));value.object.emplace(type.fieldIds[1],numberValue(series.step));value.object.emplace(type.fieldIds[2],yValue(series));return value;
}
}
std::string validateType(const binding::TypeRef &type) {
    return form(type)==Form::Invalid?"WFG_TYPE: expected real numeric Y arrays (rank 1/2), positional timing/plot clusters, or t0:timestamp/dt:f64/Y waveforms; scalar, Boolean, String, Complex and higher ranks are unsupported.":std::string{};
}
std::shared_ptr<const Dataset> decodeTypedValue(const Type &type,const Json &value,std::string *error) {
    try {
        if(const auto issue=validateType(type);!issue.empty())throw std::runtime_error(issue);
        auto out=std::make_shared<Dataset>();std::size_t total=0;const auto shape=form(type);
        if(shape==Form::Y || shape==Form::Rows)numericArrays(*out,type,value,0,1,total);
        else if(shape==Form::Plots)plotArrays(*out,type,value,0,1,total);
        else if(shape==Form::TimedArray || shape==Form::Waveforms) {
            const auto data=elements(value,type);if(data.cells.size()>maximumSeries)throw std::runtime_error("WFG_SERIES_LIMIT: at most 64 series.");
            for(const auto &entry:data.cells)timedRecord(*out,type.children[0],entry,shape==Form::Waveforms,total);
        } else timedRecord(*out,type,value,shape==Form::Waveform,total);
        if(const auto issue=validate(*out);!issue.empty())throw std::runtime_error(issue);
        out->datatype=type;out->exactSource=std::make_shared<const Json>(value);if(error)error->clear();return out;
    } catch(const std::exception &failure){if(error)*error=failure.what();return {};}
}
Json emptyValue(const Type &type) {
    if(const auto issue=validateType(type);!issue.empty())throw std::runtime_error(issue);
    const auto empty=[&](const auto &self,const Type &t)->Json {
        if(t.kind==Kind::Array) {
            auto value=arrayValue();const auto count=t.extent.value_or(0);
            if(count>(real(t.children[0])?maximumSamples:maximumSeries))throw std::runtime_error("WFG_EMPTY_EXTENT: declared extent exceeds graph quotas.");
            for(std::size_t i=0;i<count;++i)value.array.push_back(self(self,t.children[0]));
            if(t.rank==2) {auto object=objectValue(),shape=arrayValue();shape.array={numberValue(count?1:0),numberValue(double(count))};object.object.emplace("shape",std::move(shape));object.object.emplace("elements",std::move(value));return object;}
            return value;
        }
        if(t.kind==Kind::Cluster) {auto value=objectValue();for(std::size_t i=0;i<t.children.size();++i) {
            const bool interval=(waveform(t) && t.fieldIds[i]=="dt") || (record(t,3) && real(t.children[0]) && real(t.children[1]) && i==1);
            value.object.emplace(t.fieldIds[i],interval?numberValue(1):self(self,t.children[i]));}return value;}
        if(t.name=="timestamp")return *timestamp_source::json(0);
        return numberValue(0);
    };
    return empty(empty,type);
}
Type legacyDataType(const Dataset &data) {
    auto y=Type::array(Type::primitive("f64"));
    if(data.series.empty() || (data.series.size()==1 && data.series[0].origin==0 && data.series[0].step==1))return y;
    auto timed=Type::cluster({"x0","dx","Y"},{Type::primitive("f64"),Type::primitive("f64"),std::move(y)});
    return data.series.size()==1?timed:Type::array(std::move(timed));
}
Json initialValueSource(const State &state) {
    if(state.typedInitialData)return *state.typedInitialData;
    if(state.initialData.exactSource && state.initialData.datatype && binding::fullTypeEqual(*state.initialData.datatype,state.datatype))return *state.initialData.exactSource;
    const auto shape=form(state.datatype);const auto &series=state.initialData.series;
    if(series.empty())return emptyValue(state.datatype);
    if(shape==Form::Y && series.size()==1 && series[0].origin==0 && series[0].step==1)return yValue(series[0]);
    if(shape==Form::TimedY && series.size()==1)return legacyTimedValue(state.datatype,series[0]);
    if(shape==Form::TimedArray){auto value=arrayValue();for(const auto &item:series)value.array.push_back(legacyTimedValue(state.datatype.children[0],item));return value;}
    throw std::runtime_error("WFG_LEGACY_TYPE: the declared type cannot preserve the complete legacy samples and timing.");
}
bool setTypedInitialData(State &state,const Type &type,const Json &value,std::string *error) {
    const auto data=decodeTypedValue(type,value,error);if(!data)return false;
    try {
        auto preparedType=type;auto preparedData=*data;auto preparedSource=data->exactSource;
        static_assert(std::is_nothrow_move_assignable_v<Type> && std::is_nothrow_move_assignable_v<Dataset>);
        state.datatype=std::move(preparedType);state.initialData=std::move(preparedData);
        state.typedInitialData=std::move(preparedSource);state.preservedSource.reset();return true;
    } catch(const std::exception &failure){if(error)*error=failure.what();return false;}
}
bool replaceTypedData(State &state,const Type &type,const Json &value,std::string *error) {
    if(state.preservedSource){if(error)*error="WFG_UNSUPPORTED_SOURCE: a runtime publication cannot replace preserved source.";return false;}
    if(!binding::fullTypeEqual(type,state.datatype)){if(error)*error="WFG_RUNTIME_TYPE: publication must preserve the complete declared datatype.";return false;}
    const auto data=decodeTypedValue(type,value,error);if(!data)return false;
    state.current=data;++state.dataRevision;state.diagnostic.clear();return true;
}
}
