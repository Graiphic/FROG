#include "frog_native_engine/NativeCallScalarContract.hpp"
#include <array>
#include <charconv>
#include <limits>
namespace frog::native_engine {
namespace native_call_detail {
using K=NativeCallKind;using N=NativeCallNumericType;
bool scalarType(N type) noexcept {return type==N::I32||type==N::U32||type==N::I64||type==N::U64||type==N::F64;}
CanonicalNumber decodeCanonicalNumber(const source::JsonValue &item,bool unsignedInteger,bool signedInteger) {
    if(item.type!=source::JsonValue::Type::Number)throw Fault(7607,"Numeric default must be a JSON number.");
    const auto &text=item.string;
    auto parse=[&]<class T>()->T {T value{};const auto r=std::from_chars(text.data(),text.data()+text.size(),value);if(r.ec!=std::errc{} || r.ptr!=text.data()+text.size())throw Fault(7607,"Invalid canonical numeric default.");return value;};
    if(unsignedInteger)return parse.template operator()<std::uint64_t>();
    if(signedInteger)return parse.template operator()<std::int64_t>();
    return parse.template operator()<double>();
}
void scalarProfileDiagnostics(const NativeCallConfiguration &config,NativeCallValidation &result) {
    const auto unavailable=[&](bool supported,std::string code,std::string message,std::string id={}) {
        if(!supported)result.diagnostics.push_back({std::move(code),std::move(message),std::move(id),
            NativeCallDiagnosticCategory::Unavailable,NativeCallDiagnosticSeverity::Error});
    };
    unavailable(config.thread==NativeCallThread::Any,"scalar_thread_unavailable","The exact scalar service admits Any execution only; Ui needs its own real dispatcher.");
    unavailable(!config.library.specifyPathOnDiagram,"scalar_dynamic_path_unavailable","Dynamic path input/output ports are outside the exact scalar service.");
    unavailable(config.callbacks.reserve.empty()&&config.callbacks.unreserve.empty()&&config.callbacks.abort.empty(),
        "scalar_callbacks_unavailable","The exact scalar service does not admit lifecycle callback configurations.");
    unavailable(config.returnDefinition.type.kind==K::Void ||
        (config.returnDefinition.type.kind==K::Numeric&&scalarType(config.returnDefinition.type.numericType)),
        "scalar_return_unavailable","Return must be void or exact i32/u32/i64/u64/f64.","return");
    for(const auto &p:config.arguments)unavailable(p.type.kind==K::Numeric&&scalarType(p.type.numericType)&&
        p.pass==NativeCallPass::Value&&p.access==NativeCallAccess::ReadOnly,
        "scalar_argument_unavailable","Arguments must be exact i32/u32/i64/u64/f64 passed by value and ReadOnly.",p.id);
    // The count itself is not an arbitrary profile cap. The actual ABI count
    // representation and existing 64MiB native packing budget bound allocation.
    constexpr auto perArgument=sizeof(std::array<std::byte,16>)+2*sizeof(void*);
    unavailable(config.arguments.size()<=std::numeric_limits<unsigned>::max()&&
        config.arguments.size()<=(native_call_detail::allocationBudget-64)/perArgument,
        "scalar_pack_budget","Native scalar argument storage and ABI pointer tables exceed the existing 64MiB invocation budget.");
}
std::optional<NativeCallScalarValue> scalarCanonicalDefault(const NativeCallParameter &parameter) {
    if(!parameter.defaultCanonicalValue)return {};
    std::string error;
    const auto json=source::parseJson(*parameter.defaultCanonicalValue,&error,source::JsonNumberPolicy::PreserveLexeme);
    if(!json)throw Fault(7607,"Invalid canonical default: "+error);
    const auto type=parameter.type.numericType;
    const auto number=native_call_detail::decodeCanonicalNumber(*json,type==N::U32||type==N::U64,type!=N::F64);
    switch(type) {
    case N::I32:{const auto n=std::get<std::int64_t>(number);if(n<INT32_MIN||n>INT32_MAX)throw Fault(7607,"Integer input is outside native representation.");return static_cast<std::int32_t>(n);}
    case N::U32:{const auto n=std::get<std::uint64_t>(number);if(n>UINT32_MAX)throw Fault(7607,"Integer input is outside native representation.");return static_cast<std::uint32_t>(n);}
    case N::I64:return std::get<std::int64_t>(number);
    case N::U64:return std::get<std::uint64_t>(number);
    case N::F64:return std::get<double>(number);
    default:throw Fault(7606,"Default has no exact scalar representation.");
    }
}
}
bool nativeCallScalarConfigurationPreflight(const source::JsonValue &root,std::string *error) {
    if(error)error->clear();
    const auto refuse=[&](const char *reason){if(error)*error=reason;return false;};
    constexpr std::size_t maximumBytes=16U*1024U*1024U;
    // NativeCall configuration metadata keeps its own existing admission bound;
    // increasing the document budget for dense arrays must not expand this codec.
    constexpr std::size_t maximumValues=250'000U;
    struct Item {const source::JsonValue *value;bool adapted;};
    std::vector<Item> pending{{&root,false}};std::size_t visited=0,bytes=0;
    const auto charge=[&](std::size_t count){if(count>maximumBytes-bytes)return false;bytes+=count;return true;};
    while(!pending.empty()) {
        const auto item=pending.back();pending.pop_back();
        if(++visited>maximumValues)return refuse("NativeCall codec value count exceeds the existing bounded JSON contract.");
        const auto &value=*item.value;
        if((value.type==source::JsonValue::Type::String||value.type==source::JsonValue::Type::Number)&&!charge(value.string.size()))
            return refuse("NativeCall codec text exceeds the 16MiB metadata admission bound.");
        if(item.adapted&&value.type==source::JsonValue::Type::Object)if(const auto *canonical=value.member("canonical");canonical&&canonical->type==source::JsonValue::Type::String) {
            std::size_t depth=0;bool quoted=false,escaped=false;
            for(const char c:canonical->string) {
                if(quoted){if(escaped)escaped=false;else if(c=='\\')escaped=true;else if(c=='"')quoted=false;continue;}
                if(c=='"'){quoted=true;continue;}
                if(c=='<'&&++depth>32)return refuse("NativeCall adapted type text exceeds the codec's depth32 contract before type parsing.");
                if(c=='>'&&depth)--depth;
            }
        }
        const auto children=value.type==source::JsonValue::Type::Object?value.object.size():value.type==source::JsonValue::Type::Array?value.array.size():0U;
        if(children>maximumValues-visited||pending.size()>maximumValues-visited-children)
            return refuse("NativeCall codec tables exceed the existing bounded JSON value count.");
        if(value.type==source::JsonValue::Type::Object)for(const auto &[key,child]:value.object) {
            if(!charge(key.size()))return refuse("NativeCall codec identities exceed the 16MiB metadata admission bound.");
            pending.push_back({&child,item.adapted||key=="adapted_type"});
        }
        if(value.type==source::JsonValue::Type::Array)for(const auto &child:value.array)pending.push_back({&child,item.adapted});
    }
    return true;
}
NativeCallValidation nativeCallScalarSignatureValidate(const NativeCallConfiguration &config) {
    auto result=nativeCallValidate(config);
    native_call_detail::scalarProfileDiagnostics(config,result);
    return result;
}
std::optional<NativeCallScalarValue> nativeCallScalarDefaultValue(const NativeCallParameter &parameter,std::string *error) {
    if(error)error->clear();
    try {return native_call_detail::scalarCanonicalDefault(parameter);}
    catch(const std::exception &failure){if(error)*error=failure.what();return {};}
}
}
