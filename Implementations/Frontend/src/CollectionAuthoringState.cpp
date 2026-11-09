#include "frog_native_engine/CollectionAuthoringState.hpp"
#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include "frog/sdk/collection_order.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <set>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using Type=binding::TypeRef;
using Kind=binding::TypeKind;
using Json=source::JsonValue;
namespace order=sdk::collection_order;
using namespace canonical_json;
constexpr std::size_t maximumBytes=16'000'000,maximumNodes=65'536;
const Json &member(const Json &v,std::string_view name) {
    const auto *p=v.member(name);if(!p)throw std::runtime_error("Collection data is missing field "+std::string(name));return *p;
}
void exactMembers(const Json &v,std::initializer_list<std::string_view> names) {
    if(v.type!=Json::Type::Object||v.object.size()!=names.size())throw std::runtime_error("Collection data has an unexpected object shape.");
    for(auto name:names)(void)member(v,name);
}
Json number(std::string token) {Json v;v.type=Json::Type::Number;v.string=std::move(token);return v;}
template<class T>T integer(const Json &v,bool stringAllowed=false) {
    if(v.type!=Json::Type::Number&&!(stringAllowed&&v.type==Json::Type::String))throw std::runtime_error("An exact integer token is required.");
    T n{};const auto r=std::from_chars(v.string.data(),v.string.data()+v.string.size(),n);
    if(r.ec!=std::errc{}||r.ptr!=v.string.data()+v.string.size())throw std::runtime_error("An integer is outside its exact declared representation.");return n;
}
double realNumber(const Json &v) {
    if(v.type!=Json::Type::Number)throw std::runtime_error("An exact real numeric token is required.");
    double n{};const auto r=std::from_chars(v.string.data(),v.string.data()+v.string.size(),n);
    if(r.ec!=std::errc{}||r.ptr!=v.string.data()+v.string.size()||!std::isfinite(n))throw std::runtime_error("A Collection real value must be finite and representable.");return n;
}
std::string realToken(double n) {
    char bytes[128];const auto r=std::to_chars(bytes,bytes+sizeof bytes,n,std::chars_format::general,std::numeric_limits<double>::max_digits10);
    if(r.ec!=std::errc{})throw std::runtime_error("The real value cannot be encoded.");return {bytes,r.ptr};
}
const Type &representation(const Type &type) {return type.kind==Kind::Enum&&type.children.size()==1?type.children.front():type;}
order::Kind orderedKind(const Type &original) {
    const auto &t=representation(original);
    if(t.kind==Kind::Cluster)return order::Kind::Record;
    if(t.kind==Kind::Array)return order::Kind::Array;
    if(t.kind!=Kind::Primitive)return order::Kind::Unsupported;
    if(t.name=="bool")return order::Kind::Boolean;
    if(t.name=="i8"||t.name=="i16"||t.name=="i32"||t.name=="i64")return order::Kind::Signed;
    if(t.name=="u8"||t.name=="u16"||t.name=="u32"||t.name=="u64")return order::Kind::Unsigned;
    if(t.name=="f32"||t.name=="f64")return order::Kind::Real;
    if(t.name=="string"||t.name=="path")return order::Kind::Text;
    if(t.name=="timestamp")return order::Kind::Timestamp;
    return order::Kind::Unsupported;
}
struct TypeView {
    const Type *type;
    order::Kind kind()const{return orderedKind(*type);}
    std::size_t childCount()const{return representation(*type).children.size();}
    TypeView child(std::size_t i)const{return {&representation(*type).children.at(i)};}
    std::size_t rank()const{return representation(*type).rank;}
};
struct ValueView {
    const Type *type;const Json *value;
    TypeView schema()const{return {type};}
    order::Kind kind()const{return orderedKind(*type);}
    bool sameSchema(const ValueView &other)const{return type==other.type||collectionTypesEqual(*type,*other.type);}
    bool boolean()const{return value->boolean;}
    std::int64_t signedInteger()const{return integer<std::int64_t>(*value);}
    std::uint64_t unsignedInteger()const{return integer<std::uint64_t>(*value);}
    double real()const{return realNumber(*value);}
    std::string_view text()const{return value->string;}
    std::size_t childCount()const{return kind()==order::Kind::Array?member(*value,"elements").array.size():type->children.size();}
    ValueView child(std::size_t i)const {
        if(kind()==order::Kind::Array)return {&type->children.front(),&member(*value,"elements").array.at(i)};
        return {&type->children.at(i),&member(*value,type->fieldIds.at(i))};
    }
    std::size_t rank()const{return type->rank;}
    std::size_t extent(std::size_t d)const{return integer<std::uint32_t>(member(*value,"shape").array.at(d));}
    std::int64_t seconds()const{return integer<std::int64_t>(member(*value,"seconds"),true);}
    std::uint64_t fraction()const{return integer<std::uint64_t>(member(*value,"fraction"),true);}
};
std::string orderError(order::Error error) {
    switch(error) {
    case order::Error::None:return {};
    case order::Error::Unsupported:return "The key/element schema has no declared Collection ordering policy.";
    case order::Error::NonFinite:return "Non-finite real values cannot be Collection keys or Set elements.";
    case order::Error::TypeMismatch:return "Collection ordering requires exactly the same complete schema.";
    case order::Error::DepthLimit:return "Collection ordering exceeds its depth bound.";
    case order::Error::ValueLimit:return "Collection ordering exceeds its value bound.";
    case order::Error::WorkLimit:return "Collection normalization exceeds its work bound.";
    case order::Error::InvalidShape:return "Collection ordering encountered an invalid shape.";
    case order::Error::Cancelled:return "Collection normalization was cancelled.";
    case order::Error::AllocationFailure:return "Collection normalization could not reserve memory.";
    }return "Collection ordering failed.";
}
struct Budget {
    std::size_t nodes=0,bytes=0;
    void enter(unsigned depth,std::size_t n=0) {
        if(depth>32||++nodes>maximumNodes||n>maximumBytes-bytes)throw std::runtime_error("Collection data exceeds its depth, size or value budget.");bytes+=n;
    }
};
void schema(const Type &t,unsigned depth=0,std::size_t *counter=nullptr) {
    std::size_t local=0;if(!counter)counter=&local;
    if(depth>32||++*counter>4096||!t.known())throw std::runtime_error("A Collection type must be complete and bounded.");
    if(t.kind==Kind::Array) {
        if(t.rank<1||t.rank>16||t.children.size()!=1)throw std::runtime_error("An Array requires its element schema and rank.");
    }else if(t.kind==Kind::Cluster) {
        std::set<std::string> ids;
        if(t.fieldIds.size()!=t.children.size()||(!t.fieldNames.empty()&&t.fieldNames.size()!=t.fieldIds.size()))throw std::runtime_error("Cluster schema and field identities disagree.");
        for(const auto &id:t.fieldIds)if(id.empty()||!ids.insert(id).second)throw std::runtime_error("Cluster fields require distinct stable identities.");
    }else if(t.kind==Kind::Map) {
        if(t.children.size()!=2)throw std::runtime_error("Map requires full K and V schemas.");
        if(const auto e=order::comparableSchema(TypeView{&t.children[0]});e!=order::Error::None)throw std::runtime_error(orderError(e));
    }else if(t.kind==Kind::Set) {
        if(t.children.size()!=1)throw std::runtime_error("Set requires its full element schema.");
        if(const auto e=order::comparableSchema(TypeView{&t.children[0]});e!=order::Error::None)throw std::runtime_error(orderError(e));
    }else if(t.kind==Kind::Enum) {
        if(t.children.size()!=1||!t.children.front().integer()||t.choices.empty())throw std::runtime_error("Enum data requires a complete domain and integer representation.");
    }else if(t.kind==Kind::Reference) {
        if(t.name.empty()||t.children.size()>1)throw std::runtime_error("A reference requires a typed nominal class.");
    }else if(t.kind==Kind::Primitive) {
        if(orderedKind(t)==order::Kind::Unsupported&&t.name!="path"&&t.name!="variant")throw std::runtime_error("This value schema has no declared Collection authoring carrier: "+t.canonical());
    }else throw std::runtime_error("This aggregate has no declared Collection authoring carrier: "+t.canonical());
    for(const auto &child:t.children)schema(child,depth+1,counter);
}
Json normalized(const Type &type,const Json &value,Budget &budget,unsigned depth=0) {
    budget.enter(depth,value.string.size());
    if(type.kind==Kind::Map||type.kind==Kind::Set) {
        const bool map=type.kind==Kind::Map;exactMembers(value,{map?"entries":"elements"});
        const auto &items=member(value,map?"entries":"elements");
        if(items.type!=Json::Type::Array||items.array.size()>kMaximumCollectionEntries)throw std::runtime_error("Collection entries exceed their bound.");
        auto result=object(),values=array();values.array.reserve(items.array.size());
        for(const auto &item:items.array) {
            if(map) {
                exactMembers(item,{"key","value"});auto entry=object();
                entry.object.emplace("key",normalized(type.children[0],member(item,"key"),budget,depth+1));
                entry.object.emplace("value",normalized(type.children[1],member(item,"value"),budget,depth+1));values.array.push_back(std::move(entry));
            }else values.array.push_back(normalized(type.children[0],item,budget,depth+1));
        }
        std::vector<ValueView> keys;keys.reserve(values.array.size());
        for(const auto &item:values.array)keys.push_back({&type.children[0],map?&member(item,"key"):&item});
        const auto indices=order::normalize(std::span<const ValueView>{keys},map?order::DuplicatePolicy::KeepLast:order::DuplicatePolicy::KeepFirst);
        if(!indices.accepted())throw std::runtime_error(orderError(indices.error));
        auto sorted=array();sorted.array.reserve(indices.indices.size());for(auto i:indices.indices)sorted.array.push_back(std::move(values.array[i]));
        result.object.emplace(map?"entries":"elements",std::move(sorted));return result;
    }
    if(type.kind==Kind::Array) {
        exactMembers(value,{"shape","elements"});const auto &shape=member(value,"shape"),&items=member(value,"elements");
        if(shape.type!=Json::Type::Array||shape.array.size()!=type.rank||items.type!=Json::Type::Array||items.array.size()>kMaximumCollectionEntries)throw std::runtime_error("An Array requires its exact rank, shape and row-major contents.");
        auto result=object(),extents=array(),elements=array();std::size_t count=1;
        for(const auto &extent:shape.array) {const auto n=integer<std::uint32_t>(extent);if(n>kMaximumCollectionEntries||(n&&count>kMaximumCollectionEntries/n))throw std::runtime_error("An Array shape exceeds its value bound.");count*=n;extents.array.push_back(number(std::to_string(n)));}
        if(count!=items.array.size())throw std::runtime_error("Array shape and element count disagree.");
        for(const auto &item:items.array)elements.array.push_back(normalized(type.children.front(),item,budget,depth+1));
        result.object={{"shape",std::move(extents)},{"elements",std::move(elements)}};return result;
    }
    if(type.kind==Kind::Cluster) {
        if(value.type!=Json::Type::Object||value.object.size()!=type.fieldIds.size())throw std::runtime_error("Cluster values require exactly the stable declared fields.");
        auto result=object();for(std::size_t i=0;i<type.children.size();++i){budget.enter(depth,type.fieldIds[i].size());result.object.emplace(type.fieldIds[i],normalized(type.children[i],member(value,type.fieldIds[i]),budget,depth+1));}return result;
    }
    if(type.kind==Kind::Enum) {
        auto result=normalized(type.children.front(),value,budget,depth+1);
        const auto n=integer<std::uint64_t>(result);
        if(std::none_of(type.choices.begin(),type.choices.end(),[&](const auto &choice){return choice.value==n;}))throw std::runtime_error("The Enum value is outside its declared domain.");return result;
    }
    if(type.kind==Kind::Reference) {
        exactMembers(value,{"frog_studio_object_reference"});
        std::string why;const auto cfg=application_control_source::readConfiguration(member(value,"frog_studio_object_reference"),&why);
        if(!cfg||cfg->kind!=ApplicationControlKind::Reference)throw std::runtime_error(why.empty()?"A Collection reference must remain a symbolic object reference.":why);
        const auto actual=Type::reference(cfg->expectedClass,cfg->valueType.known()?std::optional{cfg->valueType}:std::nullopt);
        if(!collectionTypesEqual(type,actual))throw std::runtime_error("The symbolic reference lost its complete nominal schema.");
        auto result=object();result.object.emplace("frog_studio_object_reference",application_control_source::writeConfiguration(*cfg));return result;
    }
    if(type.name=="variant") {
        exactMembers(value,{"value_schema","value","attributes"});std::string why;
        const auto payload=application_control_source::readType(member(value,"value_schema"),&why);
        if(!payload)throw std::runtime_error(why);schema(*payload);
        const auto &attributes=member(value,"attributes");if(attributes.type!=Json::Type::Object||attributes.object.size()>64)throw std::runtime_error("Variant attributes require bounded named Variant values.");
        auto result=object(),attrs=object();for(const auto &[name,item]:attributes.object){budget.enter(depth,name.size());attrs.object.emplace(name,normalized(type,item,budget,depth+1));}
        result.object={{"value_schema",application_control_source::writeType(*payload)},{"value",normalized(*payload,member(value,"value"),budget,depth+1)},{"attributes",std::move(attrs)}};return result;
    }
    if(type.name=="timestamp") {
        exactMembers(value,{"seconds","fraction"});auto result=object();result.object={{"seconds",string(std::to_string(integer<std::int64_t>(member(value,"seconds"),true)))},{"fraction",string(std::to_string(integer<std::uint64_t>(member(value,"fraction"),true)))}};return result;
    }
    if(type.name=="bool") {if(value.type!=Json::Type::Boolean)throw std::runtime_error("A Boolean value is required.");return boolean(value.boolean);}
    if(type.name=="string"||type.name=="path") {if(value.type!=Json::Type::String||value.string.size()>1'000'000)throw std::runtime_error("A bounded String/Path value is required.");return string(value.string);}
    if(type.integer()) {
        if(type.name.starts_with('u')) {const auto n=integer<std::uint64_t>(value);const unsigned bits=type.name=="u8"?8:type.name=="u16"?16:type.name=="u32"?32:64;if(bits<64&&n>((std::uint64_t{1}<<bits)-1))throw std::runtime_error("An unsigned value exceeds its declared width.");return number(std::to_string(n));}
        const auto n=integer<std::int64_t>(value);const unsigned bits=type.name=="i8"?8:type.name=="i16"?16:type.name=="i32"?32:64;
        if(bits<64&&(n<-(std::int64_t{1}<<(bits-1))||n>((std::int64_t{1}<<(bits-1))-1)))throw std::runtime_error("A signed value exceeds its declared width.");return number(std::to_string(n));
    }
    if(type.name=="f32"||type.name=="f64") {
        auto n=realNumber(value);
        if(type.name=="f32") {float f{};const auto r=std::from_chars(value.string.data(),value.string.data()+value.string.size(),f);
            if(r.ec!=std::errc{}||r.ptr!=value.string.data()+value.string.size()||!std::isfinite(f))throw std::runtime_error("A real value exceeds f32 range.");n=static_cast<double>(f);}
        return number(realToken(n));
    }
    throw std::runtime_error("The Collection value has no declared authoring carrier.");
}
Json emptyValue(CollectionKind kind) {auto value=object();value.object.emplace(kind==CollectionKind::Map?"entries":"elements",array());return value;}
bool rowValid(std::string_view row) {return !row.empty()&&row.size()<=128&&row.find_first_of(":@\0",0,3)==std::string::npos;}
template<class F>auto guarded(std::string *error,F operation)->decltype(operation()) {
    try{return operation();}catch(const std::exception &e){if(error)*error=e.what();return {};}
}
} // namespace

struct CollectionDataStorage {
    Type type;
    Json value;
    std::string encoded;
    std::size_t count=0,bytes=0;
};
struct CollectionAuthoringCodec {
    static std::size_t schemaRetained(const Type &type) {
        std::size_t bytes=type.name.capacity()+type.children.capacity()*sizeof(Type)+type.choices.capacity()*sizeof(binding::ChoiceItemMetadata);
        for(const auto &child:type.children)bytes+=schemaRetained(child);
        for(const auto *strings:{&type.fieldIds,&type.fieldNames}) {bytes+=strings->capacity()*sizeof(std::string);for(const auto &text:*strings)bytes+=text.capacity();}
        for(const auto &choice:type.choices)bytes+=choice.id.capacity()+choice.label.capacity();return bytes;
    }
    static std::size_t retained(const Json &value) {
        std::size_t bytes=sizeof(Json)+value.string.capacity()+value.array.capacity()*sizeof(Json);
        for(const auto &child:value.array)bytes+=retained(child)-sizeof(Json);
        for(const auto &[key,child]:value.object)bytes+=sizeof(std::pair<const std::string,Json>)+4*sizeof(void*)+key.capacity()+retained(child)-sizeof(Json);
        return bytes;
    }
    static CollectionData store(Type type,Json value) {
        auto storage=std::make_shared<CollectionDataStorage>();storage->type=std::move(type);storage->value=std::move(value);
        storage->encoded=encode(storage->value);storage->count=member(storage->value,storage->type.kind==Kind::Map?"entries":"elements").array.size();
        storage->bytes=sizeof(CollectionDataStorage)+storage->encoded.capacity()+retained(storage->value)+schemaRetained(storage->type);
        return CollectionData{std::move(storage)};
    }
    static const CollectionDataStorage *storage(const CollectionData &data) {return data.storage_.get();}
};
std::size_t CollectionData::size()const noexcept{return storage_?storage_->count:0;}
std::size_t CollectionData::retainedBytes()const noexcept{return storage_?storage_->bytes:0;}
bool CollectionData::operator==(const CollectionData &other)const {
    if(storage_==other.storage_)return true;if(empty()&&other.empty())return true;
    return storage_&&other.storage_&&collectionTypesEqual(storage_->type,other.storage_->type)&&storage_->encoded==other.storage_->encoded;
}
bool collectionTypesEqual(const Type &a,const Type &b) {return binding::fullTypeEqual(a,b);}
bool CollectionAuthoringState::operator==(const CollectionAuthoringState &other)const {
    return kind==other.kind&&comparisonPolicy==other.comparisonPolicy&&collectionTypesEqual(keyType,other.keyType)&&collectionTypesEqual(valueType,other.valueType)&&data==other.data&&
        keyPresentation==other.keyPresentation&&valuePresentation==other.valuePresentation&&visibleIndex==other.visibleIndex&&visibleRows==other.visibleRows;
}
bool CollectionBuildState::operator==(const CollectionBuildState &other)const {
    return rows==other.rows&&nextRowSerial==other.nextRowSerial&&collectionTypesEqual(typeWitness,other.typeWitness);
}
Type collectionType(const CollectionAuthoringState &state) {
    if(!state.valueType.known()||(state.kind==CollectionKind::Map&&!state.keyType.known()))return {};
    return state.kind==CollectionKind::Map?Type::map(state.keyType,state.valueType):Type::set(state.valueType);
}
std::optional<CollectionTypedValue> collectionValue(const CollectionAuthoringState &state) {
    auto type=collectionType(state);if(!type.known()||state.comparisonPolicy!=kCollectionComparisonPolicy)return {};
    if(state.kind==CollectionKind::Set&&state.keyType.known())return {};
    try{schema(type);}catch(...){return {};}
    if(const auto *stored=CollectionAuthoringCodec::storage(state.data);stored&&!collectionTypesEqual(stored->type,type))return {};
    return CollectionTypedValue{std::move(type),state.data};
}
bool collectionComparableType(const Type &type,std::string *error) {
    try {schema(type);const auto e=order::comparableSchema(TypeView{&type});if(e!=order::Error::None)throw std::runtime_error(orderError(e));return true;}
    catch(const std::exception &e){if(error)*error=e.what();return false;}
}
std::optional<CollectionTypedValue> readCollectionData(const Type &type,const Json &value,std::string *error) {
    return guarded(error,[&]()->std::optional<CollectionTypedValue>{
        if(type.kind!=Kind::Map&&type.kind!=Kind::Set)throw std::runtime_error("Collection data requires a real Map or Set schema.");
        schema(type);Budget budget;auto data=normalized(type,value,budget);
        return CollectionTypedValue{type,CollectionAuthoringCodec::store(type,std::move(data))};
    });
}
Json writeCollectionData(const CollectionTypedValue &value) {
    schema(value.type);
    if(value.type.kind!=Kind::Map&&value.type.kind!=Kind::Set)throw std::invalid_argument("Collection data requires Map/Set.");
    if(const auto *stored=CollectionAuthoringCodec::storage(value.data)) {
        if(!collectionTypesEqual(stored->type,value.type))throw std::invalid_argument("Collection data cannot be reinterpreted under another schema.");return stored->value;
    }
    return emptyValue(value.type.kind==Kind::Map?CollectionKind::Map:CollectionKind::Set);
}
std::vector<CollectionPreviewEntry> collectionPreviewEntries(const CollectionAuthoringState &state,std::size_t rows,std::size_t characters) {
    std::vector<CollectionPreviewEntry> result;const auto *storage=CollectionAuthoringCodec::storage(state.data);
    if(!storage||!rows||!characters)return result;rows=(std::min)(rows,std::size_t{64});characters=(std::min)(characters,std::size_t{4096});
    if((storage->type.kind==Kind::Map)!=(state.kind==CollectionKind::Map))return result;
    const auto &values=member(storage->value,state.kind==CollectionKind::Map?"entries":"elements").array;
    const auto render=[&](const Json &root) {
        std::string output;bool truncated=false;const auto append=[&](std::string_view text){
            const auto available=characters-output.size();auto count=(std::min)(text.size(),available);
            if(count<text.size()){truncated=true;while(count&&(static_cast<unsigned char>(text[count])&0xc0)==0x80)--count;}
            output.append(text.substr(0,count));
        };
        const auto visit=[&](auto &&self,const Json &value)->void {
            if(truncated||output.size()>=characters)return;
            switch(value.type) {
            case Json::Type::Null:append("null");break;
            case Json::Type::Boolean:append(value.boolean?"true":"false");break;
            case Json::Type::Number:append(value.string);break;
            case Json::Type::String: {auto end=(std::min)(value.string.size(),characters-output.size());while(end&&end<value.string.size()&&(static_cast<unsigned char>(value.string[end])&0xc0)==0x80)--end;append(quote(std::string_view(value.string).substr(0,end)));break;}
            case Json::Type::Array: {append("[");bool first=true;for(const auto &child:value.array){if(truncated||output.size()>=characters)break;if(!first)append(", ");first=false;self(self,child);}if(!truncated)append("]");break;}
            case Json::Type::Object: {append("{");bool first=true;for(const auto &[name,child]:value.object){if(truncated||output.size()>=characters)break;if(!first)append(", ");first=false;auto end=(std::min)(name.size(),characters-output.size());while(end&&end<name.size()&&(static_cast<unsigned char>(name[end])&0xc0)==0x80)--end;append(quote(std::string_view(name).substr(0,end)));if(!truncated)append(": ");self(self,child);}if(!truncated)append("}");break;}
            }
        };visit(visit,root);
        if(truncated||output.size()>=characters)output+="...";
        return output;
    };
    for(std::size_t i=0;i<(std::min)(rows,values.size());++i)result.push_back(state.kind==CollectionKind::Map?CollectionPreviewEntry{render(member(values[i],"key")),render(member(values[i],"value"))}:CollectionPreviewEntry{{},render(values[i])});
    return result;
}
const source::JsonValue *collectionEntrySourceValue(const CollectionAuthoringState &state,bool key,std::size_t index) noexcept {
    if(key&&state.kind!=CollectionKind::Map)return nullptr;
    const auto *storage=CollectionAuthoringCodec::storage(state.data);
    if(!storage||(storage->type.kind==Kind::Map)!=(state.kind==CollectionKind::Map))return nullptr;
    const auto *items=storage->value.member(state.kind==CollectionKind::Map?"entries":"elements");
    if(!items||index>=items->array.size())return nullptr;
    const auto &entry=items->array[index];
    return state.kind==CollectionKind::Map?entry.member(key?"key":"value"):&entry;
}
std::optional<CollectionAuthoringState> makeCollectionState(const CollectionTypedValue &value,std::string *error) {
    return guarded(error,[&]()->std::optional<CollectionAuthoringState>{
        (void)writeCollectionData(value);CollectionAuthoringState state;state.kind=value.type.kind==Kind::Map?CollectionKind::Map:CollectionKind::Set;
        if(state.kind==CollectionKind::Map)state.keyType=value.type.children[0];state.valueType=value.type.children.back();state.data=value.data;return state;
    });
}
std::optional<CollectionAuthoringState> makeEmptyCollectionState(const Type &type,std::string *error) {
    const auto value=readCollectionData(type,emptyValue(type.kind==Kind::Map?CollectionKind::Map:CollectionKind::Set),error);
    return value?makeCollectionState(*value,error):std::nullopt;
}
bool assignCollectionData(CollectionAuthoringState &target,const CollectionTypedValue &value,std::string *error) {
    if(!collectionTypesEqual(collectionType(target),value.type)||target.comparisonPolicy!=kCollectionComparisonPolicy) {if(error)*error="Paste Data requires exactly the target's complete configured Collection schema.";return false;}
    const auto next=makeCollectionState(value,error);if(!next)return false;target.data=next->data;
    target.visibleIndex=target.data.empty()?0:(std::min)(target.visibleIndex,target.data.size()-1);return true;
}
bool configureCollectionTypeModels(CollectionAuthoringState &target,const Type &key,const Type &value,std::string *error) {
    try {
        if(target.kind==CollectionKind::Set&&key.known())throw std::runtime_error("Set has only one element type model.");
        if(key.known()&&!collectionComparableType(key,error))return false;
        if(value.known()){schema(value);if(target.kind==CollectionKind::Set&&!collectionComparableType(value,error))return false;}
        auto proposed=target;proposed.keyType=key;proposed.valueType=value;
        if(!collectionTypesEqual(target.keyType,key))proposed.keyPresentation.clear();
        if(!collectionTypesEqual(target.valueType,value))proposed.valuePresentation.clear();
        const auto oldType=collectionType(target),newType=collectionType(proposed);
        if(!target.data.empty()&&!collectionTypesEqual(oldType,newType))throw std::runtime_error("Changing Collection models would reinterpret existing entries; empty or replace the data explicitly first.");
        if(target.data.empty()) {
            proposed.visibleIndex=0;
            proposed.data={};if(newType.known()){const auto empty=makeEmptyCollectionState(newType,error);if(!empty)return false;proposed.data=empty->data;}
        }
        if(proposed==target)return false;target=std::move(proposed);return true;
    }catch(const std::exception &e){if(error)*error=e.what();return false;}
}
std::optional<CollectionAuthoringState> readCollectionAuthoringState(const Json &v,std::string *error) {
    return guarded(error,[&]()->std::optional<CollectionAuthoringState>{
        if(v.type!=Json::Type::Object||member(v,"profile").type!=Json::Type::String||member(v,"profile").string!=kCollectionAuthoringProfile||member(v,"comparison_policy").type!=Json::Type::String||member(v,"comparison_policy").string!=kCollectionComparisonPolicy)throw std::runtime_error("Unsupported Collection authoring or ordering profile.");
        const auto &kind=member(v,"kind");if(kind.type!=Json::Type::String||(kind.string!="map"&&kind.string!="set"))throw std::runtime_error("Unknown Collection kind.");
        CollectionAuthoringState state;state.kind=kind.string=="map"?CollectionKind::Map:CollectionKind::Set;
        for(const auto &[key,item]:v.object)if(key!="profile"&&key!="kind"&&key!="comparison_policy"&&key!="data"&&key!="key_type"&&key!="value_type"&&key!="key_presentation"&&key!="value_presentation"&&key!="visible_index"&&key!="visible_rows")throw std::runtime_error("Unknown Collection authoring field.");
        const auto presentation=[&](std::string_view name) {
            const auto *p=v.member(name);if(!p)return std::string{};
            if(p->type!=Json::Type::String||p->string.size()>8U*1024U*1024U)throw std::runtime_error("Collection model presentation exceeds its widget codec bound.");
            return p->string;
        };
        state.keyPresentation=presentation("key_presentation");state.valuePresentation=presentation("value_presentation");
        if(state.kind!=CollectionKind::Map&&!state.keyPresentation.empty())throw std::runtime_error("Set has no key presentation.");
        if(const auto *t=v.member("key_type")) {if(state.kind!=CollectionKind::Map)throw std::runtime_error("Set has no key model.");const auto parsed=application_control_source::readType(*t,error);if(!parsed)throw std::runtime_error("Invalid complete key schema.");state.keyType=*parsed;}
        if(const auto *t=v.member("value_type")) {const auto parsed=application_control_source::readType(*t,error);if(!parsed)throw std::runtime_error("Invalid complete value schema.");state.valueType=*parsed;}
        if(state.keyType.known()&&!collectionComparableType(state.keyType,error))throw std::runtime_error(error?*error:"Unsupported key type.");
        if(state.valueType.known()) {schema(state.valueType);if(state.kind==CollectionKind::Set&&!collectionComparableType(state.valueType,error))throw std::runtime_error(error?*error:"Unsupported Set element type.");}
        const auto type=collectionType(state);if(type.known()) {const auto value=readCollectionData(type,member(v,"data"),error);if(!value)throw std::runtime_error(error?*error:"Invalid Collection data.");state.data=value->data;}
        else {const auto &data=member(v,"data");exactMembers(data,{state.kind==CollectionKind::Map?"entries":"elements"});const auto &items=member(data,state.kind==CollectionKind::Map?"entries":"elements");if(items.type!=Json::Type::Array||!items.array.empty())throw std::runtime_error("An unconfigured Collection cannot contain values.");}
        if(const auto *index=v.member("visible_index")) {
            if(index->type!=Json::Type::Number)throw std::runtime_error("Collection visible index must be a nonnegative integer.");
            state.visibleIndex=integer<std::size_t>(*index);
        }
        if(const auto *rows=v.member("visible_rows")) {
            // Map and Set share a viewport count, never a value cardinality.
            // Missing historical metadata still means one visible model row.
            if(rows->type!=Json::Type::Number)
                throw std::runtime_error("Collection visible rows require an exact positive integer.");
            state.visibleRows=integer<std::size_t>(*rows);
            if(state.visibleRows==0||state.visibleRows>kMaximumCollectionEntries)
                throw std::runtime_error("Collection visible rows exceed their presentation bound.");
        }
        state.visibleIndex=state.data.empty()?0:(std::min)(state.visibleIndex,state.data.size()-1);
        return state;
    });
}
Json writeCollectionAuthoringState(const CollectionAuthoringState &state) {
    if(state.comparisonPolicy!=kCollectionComparisonPolicy)throw std::invalid_argument("Unsupported Collection ordering profile.");
    if(state.visibleRows==0||state.visibleRows>kMaximumCollectionEntries)
        throw std::invalid_argument("Collection visible rows require a bounded positive presentation count.");
    if(state.kind==CollectionKind::Set&&state.keyType.known())throw std::invalid_argument("Set has no key model.");
    if(state.keyType.known()) {std::string why;if(!collectionComparableType(state.keyType,&why))throw std::invalid_argument(why);}
    if(state.valueType.known()) {schema(state.valueType);if(state.kind==CollectionKind::Set){std::string why;if(!collectionComparableType(state.valueType,&why))throw std::invalid_argument(why);}}
    auto out=object();out.object={{"profile",string(std::string(kCollectionAuthoringProfile))},{"comparison_policy",string(state.comparisonPolicy)},{"kind",string(state.kind==CollectionKind::Map?"map":"set")}};
    if(state.kind==CollectionKind::Map&&state.keyType.known())out.object.emplace("key_type",application_control_source::writeType(state.keyType));
    if(state.valueType.known())out.object.emplace("value_type",application_control_source::writeType(state.valueType));
    if(state.keyPresentation.size()>8U*1024U*1024U||state.valuePresentation.size()>8U*1024U*1024U)throw std::invalid_argument("Collection model presentation exceeds its widget codec bound.");
    if(state.kind==CollectionKind::Map&&!state.keyPresentation.empty())out.object.emplace("key_presentation",string(state.keyPresentation));
    if(!state.valuePresentation.empty())out.object.emplace("value_presentation",string(state.valuePresentation));
    if(state.visibleIndex&&!state.data.empty())out.object.emplace("visible_index",number(std::to_string((std::min)(state.visibleIndex,state.data.size()-1))));
    if(state.visibleRows!=1)out.object.emplace("visible_rows",number(std::to_string(state.visibleRows)));
    const auto typed=collectionValue(state);if(!typed&&(!state.data.empty()||collectionType(state).known()))throw std::invalid_argument("Unconfigured or mismatched Collection data is not publishable.");
    out.object.emplace("data",typed?writeCollectionData(*typed):emptyValue(state.kind));return out;
}
std::optional<CollectionBuildState> readCollectionBuildState(const Json &v,std::string *error) {
    return guarded(error,[&]()->std::optional<CollectionBuildState>{
        if(v.type!=Json::Type::Object||member(v,"profile").type!=Json::Type::String||member(v,"profile").string!=kCollectionAuthoringProfile)throw std::runtime_error("Unsupported Collection Build profile.");
        for(const auto &[key,item]:v.object)if(key!="profile"&&key!="rows"&&key!="next_row_serial"&&key!="type_witness")throw std::runtime_error("Unknown Collection Build field.");
        const auto &rows=member(v,"rows");if(rows.type!=Json::Type::Array||rows.array.size()>kMaximumCollectionBuildRows)throw std::runtime_error("Collection Build row count exceeds its bound.");
        CollectionBuildState state;state.rows.clear();state.nextRowSerial=integer<std::uint64_t>(member(v,"next_row_serial"),true);std::set<std::string> unique;
        for(const auto &row:rows.array) {
            if(row.type!=Json::Type::String||!rowValid(row.string)||!unique.insert(row.string).second)throw std::runtime_error("Collection Build rows require distinct stable identities.");state.rows.push_back(row.string);
            if(row.string.starts_with("row_")){std::uint64_t n{};const auto p=std::from_chars(row.string.data()+4,row.string.data()+row.string.size(),n);if(p.ec==std::errc{}&&p.ptr==row.string.data()+row.string.size())state.nextRowSerial=(std::max)(state.nextRowSerial,n==UINT64_MAX?n:n+1);}
        }
        if(const auto *t=v.member("type_witness")){const auto parsed=application_control_source::readType(*t,error);if(!parsed||(parsed->kind!=Kind::Map&&parsed->kind!=Kind::Set))throw std::runtime_error("A Build type witness must be a complete Map or Set.");schema(*parsed);state.typeWitness=*parsed;}
        if(state.rows.empty()&&!state.typeWitness.known())throw std::runtime_error("An empty Build requires its complete retained type witness.");return state;
    });
}
Json writeCollectionBuildState(const CollectionBuildState &state) {
    auto out=object(),rows=array();for(const auto &row:state.rows)rows.array.push_back(string(row));
    out.object={{"profile",string(std::string(kCollectionAuthoringProfile))},{"rows",std::move(rows)},{"next_row_serial",string(std::to_string(state.nextRowSerial))}};
    if(state.typeWitness.known())out.object.emplace("type_witness",application_control_source::writeType(state.typeWitness));
    std::string why;if(!readCollectionBuildState(out,&why))throw std::invalid_argument(why);return out;
}
} // namespace frog::native_engine
