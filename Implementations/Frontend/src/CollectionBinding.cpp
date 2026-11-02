#include "frog_native_engine/CollectionBinding.hpp"
#include <algorithm>
#include <array>

namespace frog::native_engine {
namespace {
using Type=binding::TypeRef;
using Kind=binding::TypeKind;
using Status=binding::ResolutionStatus;
constexpr std::array<std::string_view,17> operations{
    "collection.map.build","collection.map.insert","collection.map.remove","collection.map.look","collection.map.extrema",
    "collection.size","collection.empty","collection.set.build","collection.set.insert","collection.set.remove",
    "collection.set.contains","collection.set.extrema","collection.set.union","collection.set.intersection",
    "collection.set.cartesian_product","collection.set.difference","collection.set.symmetric_difference"};
struct Signature {
    std::vector<CollectionPort> ports;
    std::string code,message;
    bool complete=true;
    void fail(std::string text) {if(code.empty()){code="binding.collection_schema";message=std::move(text);}}
    void add(std::string id,Type type,bool input,bool required=true,std::size_t row=0) {
        ports.push_back({id,id,input?FunctionPortDirection::Input:FunctionPortDirection::Output,std::move(type),required,row});
    }
};
Signature describe(const DiagramNode &node,const CollectionInputResolver &resolve) {
    Signature s;const auto op=collectionFunctionId(node.type);
    const auto input=[&](std::string_view id,bool required=true) {
        auto value=resolve?resolve(id):binding::BindingResult{};
        if(value.resolution.status==Status::Incompatible)s.fail(value.resolution.message);
        if(required&&(!value.resolution.type.known()||!value.complete))s.complete=false;
        return value.resolution.type;
    };
    const auto invariant=[&](Type &current,const Type &candidate,std::string_view why) {
        if(!candidate.known())return;
        if(current.known()&&!collectionTypesEqual(current,candidate))s.fail(std::string(why));else current=candidate;
    };
    const auto compare=[&](const Type &t) {if(t.known()){std::string why;if(!collectionComparableType(t,&why))s.fail(why);}};
    const auto boolean=Type::primitive("bool");
    if(op=="collection.map.build"||op=="collection.set.build") {
        const bool map=op=="collection.map.build";const auto rows=collectionBuildRows(node);Type key,value;
        for(std::size_t i=0;i<rows.size();++i) {
            if(map)invariant(key,input(collectionRowPort(rows[i],"key")),"Every Map row must preserve exactly the same full key schema.");
            invariant(value,input(collectionRowPort(rows[i],map?"value":"element")),"Every Collection row must preserve exactly the same full value schema.");
        }
        if(rows.empty()&&node.collectionBuild) {
            const auto &witness=node.collectionBuild->typeWitness;
            if(map&&witness.kind==Kind::Map&&witness.children.size()==2){key=witness.children[0];value=witness.children[1];}
            else if(!map&&witness.kind==Kind::Set&&witness.children.size()==1)value=witness.children[0];
        }
        compare(map?key:value);const auto result=map?(key.known()&&value.known()?Type::map(key,value):Type{}):(value.known()?Type::set(value):Type{});
        for(std::size_t i=0;i<rows.size();++i) {
            if(map)s.add(collectionRowPort(rows[i],"key"),key,true,true,i);
            s.add(collectionRowPort(rows[i],map?"value":"element"),value,true,true,i);
        }
        s.add(map?"map":"set",result,false);if(!result.known())s.complete=false;return s;
    }
    if(op=="collection.size"||op=="collection.empty") {
        const auto collection=input("collection");
        if(collection.known()&&collection.kind!=Kind::Map&&collection.kind!=Kind::Set)s.fail("Collection Size and Empty Collection? require a real Map or Set.");
        if(collection.kind==Kind::Map&&collection.children.size()==2)compare(collection.children[0]);
        if(collection.kind==Kind::Set&&collection.children.size()==1)compare(collection.children[0]);
        s.add("collection",collection,true);s.add(op=="collection.size"?"size":"empty",op=="collection.size"?Type::primitive("i32"):boolean,false);return s;
    }
    if(op.starts_with("collection.map.")) {
        const auto map=input("map");Type key,value;
        if(map.known()&&(map.kind!=Kind::Map||map.children.size()!=2))s.fail("This operation requires a real invariant Map<K,V>.");
        if(map.kind==Kind::Map&&map.children.size()==2){key=map.children[0];value=map.children[1];compare(key);}
        s.add("map",map,true);
        if(op=="collection.map.extrema") {s.add("max_key",key,false);s.add("min_key",key,false);return s;}
        const auto actualKey=input("key");if(key.known()&&actualKey.known()&&!collectionTypesEqual(key,actualKey))s.fail("The Map key connector requires the exact complete K schema.");
        s.add("key",key,true);
        if(op=="collection.map.insert") {
            const auto actualValue=input("value");if(value.known()&&actualValue.known()&&!collectionTypesEqual(value,actualValue))s.fail("The Map value connector requires the exact complete V schema.");
            s.add("value",value,true);s.add("map_out",map,false);s.add("key_already_included",boolean,false);s.add("value_unchanged",boolean,false);
        }else if(op=="collection.map.remove") {s.add("map_out",map,false);s.add("key_not_found",boolean,false);s.add("value",value,false);}
        else if(op=="collection.map.look") {
            const auto fallback=input("default",false);if(fallback.known()&&value.known()&&!collectionTypesEqual(fallback,value))s.fail("The optional default must retain the exact complete V schema.");
            s.add("default",value,true,false);s.add("value",value,false);s.add("key_not_found",boolean,false);
        }
        return s;
    }
    if(op.starts_with("collection.set.")) {
        const bool binary=op=="collection.set.union"||op=="collection.set.intersection"||op=="collection.set.cartesian_product"||op=="collection.set.difference"||op=="collection.set.symmetric_difference";
        const auto set=input(binary?"set_a":"set");Type element;
        if(set.known()&&(set.kind!=Kind::Set||set.children.size()!=1))s.fail("This operation requires a real invariant Set<T>.");
        if(set.kind==Kind::Set&&set.children.size()==1){element=set.children.front();compare(element);}
        s.add(binary?"set_a":"set",set,true);
        if(binary) {
            auto other=input("set_b");Type result=set;
            if(other.known()&&(other.kind!=Kind::Set||other.children.size()!=1))s.fail("The second operand requires a real Set<T>.");
            if(other.kind==Kind::Set&&other.children.size()==1)compare(other.children.front());
            if(op=="collection.set.cartesian_product") {
                result={};if(element.known()&&other.kind==Kind::Set&&other.children.size()==1&&other.children.front().known()) {
                    auto pair=Type::cluster({"first","second"},{element,other.children.front()});pair.fieldNames={"first","second"};result=Type::set(std::move(pair));
                }
            }else {
                if(set.known()&&other.known()&&!collectionTypesEqual(set,other))s.fail("Set algebra requires exactly the same complete element schema.");
                // A missing operand still prevents execution, but the other
                // operand supplies its exact authoring model for Create
                // Control/Constant and connection previews. Cartesian product
                // deliberately keeps its two independent element schemas.
                if(set.kind==Kind::Set&&set.children.size()==1&&!other.known())other=set;
                if(!set.known()&&other.kind==Kind::Set&&other.children.size()==1) {
                    s.ports.front().type=other;result=other;
                }
            }
            s.add("set_b",other,true);
            s.add("set",result,false);return s;
        }
        if(op=="collection.set.extrema") {s.add("max",element,false);s.add("min",element,false);return s;}
        const auto actualElement=input("element");if(element.known()&&actualElement.known()&&!collectionTypesEqual(element,actualElement))s.fail("The Set element connector requires the exact complete T schema.");
        s.add("element",element,true);
        if(op=="collection.set.insert") {s.add("set_out",set,false);s.add("already_included",boolean,false);}
        else if(op=="collection.set.remove") {s.add("set_out",set,false);s.add("not_included",boolean,false);}
        else if(op=="collection.set.contains")s.add("included",boolean,false);
    }
    return s;
}
}
std::string_view collectionFunctionId(std::string_view id)noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);else if(id.starts_with("frog."))id.remove_prefix(5);return id;
}
bool isCollectionFunction(std::string_view id)noexcept {
    id=collectionFunctionId(id);return std::find(operations.begin(),operations.end(),id)!=operations.end();
}
std::vector<CollectionPort> collectionPorts(const DiagramNode &node,const CollectionInputResolver &resolve) {
    if(!isCollectionFunction(node.type))return {};return describe(node,resolve).ports;
}
binding::BindingResult resolveCollectionPort(const DiagramNode &node,std::string_view port,const CollectionInputResolver &resolve) {
    if(const auto *state=collectionConstantState(node)) {
        if((collectionFunctionId(node.type)=="collection.map.constant")!=(state->kind==CollectionKind::Map))return {{Status::Incompatible,{},"binding.collection_kind","Collection constant identity and stored kind disagree."},false,{}};
        const auto type=collectionType(*state);
        if(port!=(state->kind==CollectionKind::Map?"map":"set")&&port!="value")return {{Status::Incompatible,{},"binding.collection_port","Unknown Collection constant connector."},false,{}};
        const auto value=collectionValue(*state);
        return {{value?Status::Compatible:Status::Unresolved,type,value?"":"binding.collection_unconfigured",value?"":"The Collection constant requires its complete type models."},bool(value),{}};
    }
    if(!isCollectionFunction(node.type))return {{Status::Incompatible,{},"binding.collection_operation","No Collection operation is declared."},false,{}};
    auto signature=describe(node,resolve);const auto found=std::find_if(signature.ports.begin(),signature.ports.end(),[&](const auto &p){return p.id==port;});
    if(found==signature.ports.end())return {{Status::Incompatible,{},"binding.collection_port","Unknown Collection connector identity."},false,{}};
    if(!signature.code.empty())return {{Status::Incompatible,found->type,std::move(signature.code),std::move(signature.message)},false,{}};
    const auto known=found->type.known();return {{known?Status::Compatible:Status::Unresolved,found->type,known?"":"binding.collection_unresolved",known?"":"Collection types require their connected complete schemas."},signature.complete,{}};
}
binding::TypeResolution checkCollectionConnection(const DiagramNode &node,std::string_view port,const Type &source,const CollectionInputResolver &resolve) {
    const auto candidate=[&](std::string_view id) {return id==port?binding::BindingResult{{Status::Compatible,source,{},{}},true,{}}:(resolve?resolve(id):binding::BindingResult{});};
    const auto actual=resolveCollectionPort(node,port,candidate);
    if(actual.resolution.status==Status::Incompatible)return actual.resolution;
    if(!source.known()||!actual.resolution.type.known())return {Status::Unresolved,{},"binding.collection_unresolved","The Collection connector has not acquired its complete type."};
    if(!collectionTypesEqual(source,actual.resolution.type))return {Status::Incompatible,actual.resolution.type,"binding.collection_exact","Collection connectors preserve their complete invariant schemas; no implicit numeric or nominal conversion is performed."};
    return {Status::Compatible,actual.resolution.type,{},{}};
}
} // namespace frog::native_engine
