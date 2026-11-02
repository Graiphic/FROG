#include "frog_native_engine/FunctionPolymorphicResolution.hpp"
#include "frog_native_engine/ComparisonBinding.hpp"
#include <algorithm>
#include <array>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <map>

namespace frog::native_engine::binding {
namespace {
BindingResult known(TypeRef type) {
    return {{type.known()?ResolutionStatus::Compatible:ResolutionStatus::Unresolved,
        std::move(type),{},{}},true,{}};
}
BindingResult invalid(std::string message) {
    return {{ResolutionStatus::Incompatible,{},"binding.polymorphic_operands",std::move(message)},false,{}};
}
BindingResult unresolved() {
    return {{ResolutionStatus::Unresolved,{},"binding.type_unresolved",
        "Connect a type-defining input to specialize this function."},false,{}};
}
bool named(std::string_view value,std::initializer_list<std::string_view> names) {
    return std::find(names.begin(),names.end(),value)!=names.end();
}
bool realNumber(const TypeRef &type) {return type.numeric()&&!type.name.starts_with("c");}
using LeafMap=std::function<TypeRef(const TypeRef &)>;
TypeRef mapLeaves(const TypeRef &type,const LeafMap &leaf) {return mapPolymorphicLeaves(type,leaf);}
TypeResolution commonType(const TypeRef &a,const TypeRef &b,bool lift=false) {
    return commonPolymorphicType(a,b,lift);
}
TypeRef logicType(const TypeRef &type,unsigned depth=0) {
    if(depth>32)return {};
    // logical_tree admits Error.status at every depth. Stop at that record
    // before recursing into its diagnostic String field.
    if(isErrorCluster(type))return TypeRef::primitive("bool");
    if(type.kind==TypeKind::Array||type.kind==TypeKind::Cluster) {
        if(type.children.empty()||(type.kind==TypeKind::Array&&type.children.front().kind==TypeKind::Array))return {};
        TypeRef result=type;
        for(auto &child:result.children){child=logicType(child,depth+1);if(!child.known())return {};}
        return result;
    }
    if(type==TypeRef::primitive("bool")||type.integer())return type;
    if(realNumber(type))return TypeRef::primitive("i32");
    return {};
}
}
// Preserve rank, enum identity and cluster schema. Numeric representations may
// promote; unrelated scalar types and different schemas never become DBL.
TypeResolution commonPolymorphicType(const TypeRef &a, const TypeRef &b, bool lift) {
    if (!a.known() || !b.known()) return {ResolutionStatus::Unresolved, {}, {}, {}};
    if (a == b) return {ResolutionStatus::Compatible, a, {}, {}};
    if (a.kind==TypeKind::Enum && b.kind==TypeKind::Enum) {
        auto checked=connect(a,b);
        if (checked.accepted()) checked.type=a.choiceKind==ChoiceKind::Enum?a:b;
        return checked;
    }
    if (a.kind == TypeKind::Array || b.kind == TypeKind::Array) {
        if (a.kind == TypeKind::Array && b.kind == TypeKind::Array && a.rank != b.rank)
            return {ResolutionStatus::Incompatible, {}, {}, "Array ranks differ."};
        if (!lift && (a.kind != TypeKind::Array || b.kind != TypeKind::Array))
            return {ResolutionStatus::Incompatible, {}, {}, "Array and scalar shapes differ."};
        auto child = commonPolymorphicType(a.kind == TypeKind::Array ? a.children.front() : a,
            b.kind == TypeKind::Array ? b.children.front() : b, lift);
        if (child.accepted()) child.type = TypeRef::array(std::move(child.type),
            a.kind == TypeKind::Array ? a.rank : b.rank);
        return child;
    }
    if (a.kind == TypeKind::Cluster && b.kind == TypeKind::Cluster &&
        a.fieldIds == b.fieldIds && a.children.size() == b.children.size()) {
        TypeRef result = a;
        for (std::size_t i = 0; i < result.children.size(); ++i) {
            auto child = commonPolymorphicType(a.children[i], b.children[i], lift);
            if (!child.accepted()) return child;
            result.children[i] = std::move(child.type);
        }
        return {ResolutionStatus::Compatible, std::move(result), {}, {}};
    }
    if (lift && (a.kind == TypeKind::Cluster || b.kind == TypeKind::Cluster)) {
        const auto &scalar = a.kind == TypeKind::Cluster ? b : a;
        if (scalar.kind != TypeKind::Primitive)
            return {ResolutionStatus::Incompatible, {}, {}, "Only a scalar can lift over cluster fields."};
        TypeRef result = a.kind == TypeKind::Cluster ? a : b;
        for (auto &field : result.children) {
            auto child = commonPolymorphicType(field, scalar, true);
            if (!child.accepted()) return child;
            field = std::move(child.type);
        }
        return {ResolutionStatus::Compatible, std::move(result), {}, {}};
    }
    if (a.numeric() && b.numeric()) {
        const std::array<std::string_view, 2> operands{ a.name, b.name };
        const auto promoted = resolveNumericAddValueTypes(operands);
        if (promoted.resolved()) return {ResolutionStatus::Compatible,
            parseType(promoted.resolvedValueType).type, {}, {}};
    }
    return {ResolutionStatus::Incompatible, {}, {}, "Operands have incompatible data types or schemas."};
}
TypeRef mapPolymorphicLeaves(const TypeRef &type, const std::function<TypeRef(const TypeRef &)> &leaf) {
    TypeRef result = type;
    std::vector<TypeRef *> pending{&result};
    while(!pending.empty()) {
        auto &current=*pending.back();pending.pop_back();
        if(current.kind==TypeKind::Array || current.kind==TypeKind::Cluster) {
            if(current.children.empty() || (current.kind==TypeKind::Array && current.children.front().kind==TypeKind::Array)) return {};
            for(auto &child:current.children) pending.push_back(&child);
        } else {
            current=leaf(current);if(!current.known()) return {};
        }
    }
    return result;
}
bool hasSharedPolymorphicResolution(std::string_view id) noexcept {
    return id.starts_with("comparison.")||id=="structures.select"||
        named(id,{"boolean.and","boolean.or","boolean.xor","boolean.not","boolean.nand","boolean.nor",
            "boolean.xnor","boolean.implies","boolean.and_array_elements","boolean.or_array_elements"});
}
BindingResult resolveSharedPolymorphicFunction(const FunctionPolymorphicOptions &node,
    const FunctionBindingDefinition &definition,std::string_view port,
    const FunctionInputResolver &input,std::span<const std::string> wiredInputs) {
    const auto id=std::string_view(definition.catalogId);
    const auto wired=[&](std::string_view name){return std::find(wiredInputs.begin(),wiredInputs.end(),name)!=wiredInputs.end();};
    for(const auto &name:wiredInputs) {
        auto value=input(name);if(value.resolution.status==ResolutionStatus::Incompatible)return value;
    }
    const auto *declared=findFunctionBindingPort(definition,port);
    if(!declared&&functionBindingInputPortOrdinal(definition,port)) {
        for(const auto &candidate:definition.ports)if(candidate.direction==FunctionPortDirection::Input){declared=&candidate;break;}
    }
    if(!declared)return invalid("Unknown function port.");
    if(id.starts_with("numeric.")&&wiredInputs.empty()) {
        auto hint=known(parseType(declared->defaultValueType).type);
        hint.complete=std::none_of(definition.ports.begin(),definition.ports.end(),[](const auto &p){
            return p.direction==FunctionPortDirection::Input&&p.requirement==FunctionPortRequirement::Required;});
        return hint;
    }
    const auto result=[&](const BindingResult &source,TypeRef type){auto value=known(std::move(type));value.complete=source.complete;return value;};
#include "ComparisonPolymorphism.inc"
    if (named(id, {"boolean.and_array_elements", "boolean.or_array_elements"})) {
        if (declared->direction == FunctionPortDirection::Input && !wired(port)) {
            // A 1D authoring hint enables creation without constraining wired nD arrays.
            auto hint = known(TypeRef::array(TypeRef::primitive("bool"), 1));
            hint.complete = false; // The required input still needs a real value.
            return hint;
        }
        return declared->direction == FunctionPortDirection::Input ? input(port) : known(TypeRef::primitive("bool"));
    }
    if (id == "structures.select") {
        auto selector = input("select");
        if (selector.resolution.accepted() && selector.resolution.type != TypeRef::primitive("bool") &&
                !isErrorCluster(selector.resolution.type)) return invalid("Select requires a Boolean or error-cluster selector.");
        auto a = input("true"), b = input("false");
        if (!a.resolution.accepted() && !b.resolution.accepted())
            return declared->direction == FunctionPortDirection::Input ? input(port) : unresolved();
        auto common = commonType(a.resolution.accepted() ? a.resolution.type : b.resolution.type,
            b.resolution.accepted() ? b.resolution.type : a.resolution.type);
        if (!common.accepted()) return {common, false, {}};
        if (declared->direction == FunctionPortDirection::Input) {
            auto value=input(port);
            // The other data branch establishes the editor type of an empty
            // Select input. This is a type suggestion, not an input value:
            // execution remains incomplete until that branch is connected.
            if (port!="select" && !wired(port) &&
                value.resolution.status==ResolutionStatus::Unresolved)
                return {common,false,{}};
            return value;
        }
        return {common, a.complete && b.complete, {}};
    }
    const bool booleanOperator = named(id, {"boolean.and", "boolean.or", "boolean.xor", "boolean.not",
        "boolean.nand", "boolean.nor", "boolean.xnor", "boolean.implies"});
    const bool numericOperator = id.starts_with("numeric.") && !id.starts_with("numeric.conversion.");
    if (booleanOperator || numericOperator) {
        const bool reduction = id.ends_with("_array_elements");
        const bool binary = named(id, {"numeric.subtract", "numeric.multiply", "numeric.divide"}) ||
            (booleanOperator && id != "boolean.not");
        const auto unaryType = [&](const TypeRef &value) {
            if (booleanOperator) return logicType(value);
            return mapLeaves(value, [&](const TypeRef &leaf) -> TypeRef {
                if (leaf.name == "timestamp" && id.starts_with("numeric.round_")) return leaf;
                if (!leaf.numeric()) return {};
                if (reduction && (leaf.name == "fxp" || leaf.name == "cfx")) return {};
                if (named(id, {"numeric.negate", "numeric.absolute_value"}) &&
                    (leaf.name.starts_with("u") || leaf.name == "biguint")) return {};
                if (id == "numeric.absolute_value" && leaf.name.starts_with("c"))
                    return TypeRef::primitive(leaf.name == "cfx" ? "fxp" : "f" + leaf.name.substr(1));
                if (named(id, {"numeric.square_root", "numeric.reciprocal", "numeric.divide"}) && leaf.integer())
                    return TypeRef::primitive("f64");
                return leaf;
            });
        };
        if (!binary) {
            if (booleanOperator && !wired("in")) {
                auto hint = known(TypeRef::primitive("bool"));
                hint.complete = false; // An editor type suggestion, not an input value.
                return hint;
            }
            auto value = input(reduction ? "value" : "in");
            if (!value.resolution.accepted()) return value;
            TypeRef operand = value.resolution.type;
            if (reduction) {
                if (operand.kind != TypeKind::Array) return invalid("This reduction requires an array.");
                auto element = operand.children.front(); operand = std::move(element);
            }
            auto output = unaryType(operand);
            if (!output.known()) return invalid("The input contains a type not accepted by this operation.");
            return declared->direction == FunctionPortDirection::Input ? input(port) : result(value, std::move(output));
        }
        // Timestamp subtraction is directional; do not run it through symmetric
        // numeric promotion (timestamp - timestamp is a duration).
        auto a = input("a"), b = input("b");
        if (id == "numeric.subtract" && a.resolution.accepted() && b.resolution.accepted() &&
            (a.resolution.type.name == "timestamp" || b.resolution.type.name == "timestamp")) {
            if (a.resolution.type.name != "timestamp" ||
                (b.resolution.type.name != "timestamp" && !realNumber(b.resolution.type)))
                return invalid("A timestamp can only be subtracted from a timestamp.");
            return declared->direction == FunctionPortDirection::Input ? input(port) :
                known(TypeRef::primitive(b.resolution.type.name == "timestamp" ? "f64" : "timestamp"));
        }
        std::map<std::size_t, TypeRef> operands;
        for (const auto &name : wiredInputs) {
            const auto ordinal = functionBindingInputPortOrdinal(definition, name);
            if (!ordinal) continue;
            auto value = input(name);
            if (!value.resolution.accepted()) return value;
            // Divide promotes after combining leaves, not before: I32/F32 is
            // F32, but I32/I32 is DBL.
            auto operand = booleanOperator ? logicType(value.resolution.type) :
                mapLeaves(value.resolution.type, [](const TypeRef &leaf) { return leaf.numeric() ? leaf : TypeRef{}; });
            if (!operand.known()) return invalid("The input contains an unsupported arithmetic/logical type.");
            if (!operands.emplace(*ordinal, std::move(operand)).second) return invalid("Duplicate input source.");
        }
        if (operands.empty()) {
            if (booleanOperator) {
                auto hint = known(TypeRef::primitive("bool"));
                hint.complete = false;
                return hint;
            }
            return declared->direction == FunctionPortDirection::Input ? input(port) : unresolved();
        }
        TypeResolution common{ResolutionStatus::Compatible, operands.begin()->second, {}, {}};
        for (auto it = std::next(operands.begin()); it != operands.end(); ++it) {
            common = commonType(common.type, it->second, true);
            if (!common.accepted()) return {common, false, {}};
        }
        if (declared->direction == FunctionPortDirection::Input && !wired(port)) {
            // Suggest the connected operands' shape for authoring/help. Keep
            // connected inputs' own types and commonType(..., true) above so
            // scalar/array broadcasting remains legal in either input order.
            return BindingResult{common, false, {}};
        }
        if (id == "numeric.divide") common.type = unaryType(common.type);
        return declared->direction == FunctionPortDirection::Input ? input(port) : BindingResult{common, true, {}};
    }
    return invalid("The declared polymorphic resolver has no implementation for "+definition.catalogId+".");
}
}
