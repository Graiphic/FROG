#include "FunctionPolymorphism.hpp"
#include "frog_native_engine/FunctionPolymorphicResolution.hpp"
#include "frog_native_engine/IndexArrayBinding.hpp"
#include "frog_native_engine/ArrayEditSemantics.hpp"
#include "frog_native_engine/BuildArrayBinding.hpp"
#include "frog_native_engine/ReshapeArraySemantics.hpp"
#include "frog_native_engine/ArrayPaletteSemantics.hpp"
#include "frog_native_engine/StringPaletteSemantics.hpp"
#include "frog_native_engine/BinarySerializationBinding.hpp"
#include "frog_native_engine/ComparisonBinding.hpp"
#include "frog_native_engine/ErrorPaletteBinding.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include <bit>
#include <algorithm>
#include <array>
#include <map>

namespace frog::native_engine::binding {
namespace {
BindingResult known(TypeRef type) {
    return {{type.known() ? ResolutionStatus::Compatible : ResolutionStatus::Unresolved,
        std::move(type), {}, {}}, true, {}};
}
BindingResult invalid(std::string message) {
    return {{ResolutionStatus::Incompatible, {}, "binding.polymorphic_operands", std::move(message)}, false, {}};
}
BindingResult unresolved() {
    return {{ResolutionStatus::Unresolved, {}, "binding.type_unresolved",
        "Connect a type-defining input to specialize this function."}, false, {}};
}
bool named(std::string_view value, std::initializer_list<std::string_view> names) {
    return std::find(names.begin(), names.end(), value) != names.end();
}
TypeResolution commonType(const TypeRef &a,const TypeRef &b,bool lift=false) {
    return commonPolymorphicType(a,b,lift);
}

TypeRef dropDimension(const TypeRef &array) {
    return array.rank > 1 ? TypeRef::array(array.children.front(), array.rank - 1) : array.children.front();
}
bool realNumber(const TypeRef &type) {
    return type.numeric() && !type.name.starts_with("c");
}
using LeafMap = std::function<TypeRef(const TypeRef &)>;
TypeRef mapLeaves(const TypeRef &type,const LeafMap &leaf) {
    return mapPolymorphicLeaves(type,leaf);
}
#include "ArrayAuthoringDefaults.inc"
}

static BindingResult resolvePolymorphicFunctionImpl(const DiagramNode &node,
    const FunctionBindingDefinition &definition, std::string_view port,
    const FunctionInputResolver &input, std::span<const std::string> wiredInputs,
    std::optional<std::string_view> formatLiteral) {
    if (hasSharedPolymorphicResolution(definition.catalogId))
        return resolveSharedPolymorphicFunction({node.comparisonAggregates},definition,port,input,wiredInputs);
    std::string_view id = definition.catalogId;
    // These are the same NI primitives exposed from two different palettes.
    if(id=="path.to_string") id="string.conversion.path_to_string";
    if(id=="path.from_string") id="string.conversion.string_to_path";
    if(id=="path.empty_string_or_path") id="string.empty_string_or_path";
    if(id=="string.conversion.spreadsheet_string_to_array") id="array.conversion.spreadsheet_string_to_array";
    if(id=="string.conversion.array_to_spreadsheet_string") id="array.conversion.array_to_spreadsheet_string";
    const auto wired = [&](std::string_view name) {
        return std::find(wiredInputs.begin(), wiredInputs.end(), name) != wiredInputs.end();
    };
    if(id=="variant.get_attribute") {
        if(!wired("name")&&wired("default_value"))
            return invalid("Wire name when requesting a typed default from Get Variant Attribute.");
        if(port=="found")return known(wired("name")?TypeRef::primitive("bool"):TypeRef::array(TypeRef::primitive("string")));
        if(port=="value")return !wired("name")?known(TypeRef::array(TypeRef::primitive("variant"))):
            wired("default_value")?input("default_value"):known(TypeRef::primitive("variant"));
        if(port=="default_value")return wired("default_value")?input(port):known(TypeRef::primitive("variant"));
        if(const auto *declared=findFunctionBindingPort(definition,port)) {
            if(declared->direction==FunctionPortDirection::Input&&wired(port))return input(port);
            const auto type=parseType(declared->defaultValueType);
            if(type.valid())return known(type.type);
        }
        return invalid("Unknown Get Variant Attribute connector.");
    }
    // Validate all connected inputs, including auxiliary indices, even when an
    // output's type depends on only one of them. Missing required values are a
    // readiness concern, not a reason to invent a different output type.
    const auto arrayPort = parseArrayEditPort(port);
    const bool arrayNavigationPort = arrayEditKind(id) != ArrayEditKind::None && arrayPort &&
        (arrayPort->kind == ArrayEditPortKind::Array || arrayPort->kind == ArrayEditPortKind::Index ||
         arrayPort->kind == ArrayEditPortKind::Length);
    for (const auto &name : wiredInputs) {
        if (arrayNavigationPort && name != "array" && name != port) continue;
        // Bundle's schema rail and output are independent of replacement
        // validity. Each replacement is validated at its own input below.
        if (isClusterBundleFunction(id) && name != "cluster" && name != port) continue;
        auto value = input(name);
        if (value.resolution.status == ResolutionStatus::Incompatible) return value;
    }
    const auto *declared = findFunctionBindingPort(definition, port);
    if (!declared && functionBindingInputPortOrdinal(definition, port)) {
        for (const auto &candidate : definition.ports)
            if (candidate.direction == FunctionPortDirection::Input) { declared = &candidate; break; }
    }
    if (!declared) return invalid("Unknown function port.");
    if ((id.starts_with("numeric.") || id=="boolean.number_to_array" || id=="boolean.to_integer") &&
        wiredInputs.empty() && !isBooleanArrayToNumber(id)) {
        // Match the palette's declared authoring type across Numeric/Boolean
        // aliases, including Boolean To Integer and Number To Boolean Array.
        // Keep arrays, complex numbers and fixed conversion ports as declared.
        // Never use this hint to replace evidence from a connected operand.
        auto hint = known(parseType(declared->defaultValueType).type);
        hint.complete = std::none_of(definition.ports.begin(), definition.ports.end(), [](const auto &p) {
            return p.direction == FunctionPortDirection::Input && p.requirement == FunctionPortRequirement::Required;
        });
        return hint;
    }
    const auto result = [&](const BindingResult &source, TypeRef type) {
        auto value = known(std::move(type));
        value.complete = source.complete;
        return value;
    };
#include "ArrayPalettePolymorphism.inc"
#include "ClusterFunctionPolymorphism.inc"
#include "BinarySerializationPolymorphism.inc"
#include "StringPalettePolymorphism.inc"
#include "ErrorPalettePolymorphism.inc"
#include "NumericPalettePolymorphism.inc"
    if (id == "timestamp.to_timestamp") {
        auto value = input("number");
        if (!value.resolution.accepted()) return value;
        const auto converted = mapLeaves(value.resolution.type, [](const TypeRef &leaf) {
            return realNumber(leaf) ? TypeRef::primitive("timestamp") : TypeRef{};
        });
        if (!converted.known()) return invalid("To Timestamp requires real numeric leaves.");
        return declared->direction == FunctionPortDirection::Input ? value : result(value, converted);
    }
    if (id == "timestamp.get_date_time_string" || id == "timestamp.to_date_time" ||
        id == "timestamp.format_date_time") {
        auto time = input("time_stamp");
        if (time.resolution.accepted() &&
            time.resolution.type != TypeRef::primitive("timestamp") &&
            !realNumber(time.resolution.type))
            return invalid("Date/time functions require a scalar Timestamp or real number.");
        if (time.resolution.status == ResolutionStatus::Incompatible) return time;
        return declared->direction == FunctionPortDirection::Input
            ? input(port) : known(parseType(declared->defaultValueType).type);
    }
    if(id=="path.get_system_directory") {
        if(declared->direction==FunctionPortDirection::Output) return known(TypeRef::primitive("path"));
        auto value=input(port);
        return port=="directory_type" && value.resolution.accepted()?result(value,TypeRef::primitive("i32")):value;
    }
    if(id=="path.compose" || id=="path.replace_file_extension") {
        const auto source=id=="path.compose"?"name_or_relative_path":"value";
        if((declared->direction==FunctionPortDirection::Input && port!=source) || port=="previous_extension")
            return declared->direction==FunctionPortDirection::Input?input(port):known(TypeRef::primitive("string"));
        auto value=input(source);
        if(!value.resolution.accepted()) return value;
        if(value.resolution.type.kind!=TypeKind::Primitive || !named(value.resolution.type.name,{"path","string"}))
            return invalid("This Path overload requires a scalar Path or String, not an array or cluster.");
        return declared->direction==FunctionPortDirection::Input || id=="path.replace_file_extension"?
            value:result(value,TypeRef::primitive("path"));
    }
    if (id == "array.reshape") {
        const unsigned count = reshapeArrayDimensionCount(node);
        const auto dimension = reshapeArrayDimensionIndex(port);
        if (port != "array" && port != "result" && (!dimension || *dimension >= count))
            return invalid("Reshape Array terminal does not exist for this output rank.");
        for (const auto &name : wiredInputs) {
            if (name == "array") continue;
            const auto index = reshapeArrayDimensionIndex(name);
            if (!index || *index >= count) return invalid("A Reshape Array wire references a removed size terminal.");
        }
        auto array = input("array");
        bool sizesComplete = true, requestedComplete = false;
        for (unsigned d=0; d<count; ++d) {
            auto size = input(reshapeArrayDimensionId(d));
            if (size.resolution.status == ResolutionStatus::Incompatible) return size;
            if (size.resolution.accepted() && !realNumber(size.resolution.type))
                return invalid("Reshape Array sizes require real numeric scalars, converted to I32.");
            const bool complete = wired(reshapeArrayDimensionId(d)) && size.complete && size.resolution.accepted();
            sizesComplete = sizesComplete && complete;
            if (dimension && *dimension == d) requestedComplete = complete;
        }
        if (dimension) {
            auto value = known(TypeRef::primitive("i32")); value.complete = requestedComplete; return value;
        }
        if (!array.resolution.accepted() || port == "array") return array;
        const auto &type = array.resolution.type;
        if (type.kind != TypeKind::Array || type.children.size() != 1)
            return invalid("Reshape Array requires an array input.");
        auto value = result(array, TypeRef::array(type.children.front(), count));
        value.complete = value.complete && sizesComplete;
        return value;
    }
    if (const auto kind = arrayEditKind(id); kind != ArrayEditKind::None) {
        const auto requested = parseArrayEditPort(port);
        const unsigned groups = arrayEditGroupCount(node);
        if (!requested || !arrayEditPortAllowed(kind, *requested, 16, groups))
            return invalid("Array operation terminal does not exist for this group count.");
        auto array = input("array");
        const auto &data = kind == ArrayEditKind::Subset ? indexArrayDataType(array.resolution.type) : array.resolution.type;
        if (array.resolution.accepted() && (data.kind != TypeKind::Array || data.children.size() != 1))
            return invalid("This operation requires an array input.");
        const auto rank = data.kind == TypeKind::Array ? static_cast<unsigned>(data.rank) : 16U;
        std::vector<unsigned> masks(groups, 0U);
        for (const auto &name : wiredInputs) {
            const auto p = parseArrayEditPort(name);
            if (!p || p->output() || !arrayEditPortAllowed(kind, *p, rank, groups))
                return invalid("An array operation wire references a removed dimension or group.");
            if (p->kind == ArrayEditPortKind::Index) masks[p->group] |= 1U << p->dimension;
        }
        if (!arrayEditPortAllowed(kind, *requested, rank, groups))
            return invalid("Array operation terminal does not exist for this rank.");
        if (kind == ArrayEditKind::Delete && std::popcount(masks[0]) > 1)
            return invalid("Delete From Array can select only one dimension at a time.");
        // A bad replacement must not poison the source/index terminals: users
        // can change the selected slice while its old value wire is broken.
        if (requested->kind == ArrayEditPortKind::Index || requested->kind == ArrayEditPortKind::Length)
            return known(TypeRef::primitive("i32"));
        if (requested->kind == ArrayEditPortKind::Array) return array;
        if (!array.resolution.accepted()) {
            if (requested->kind == ArrayEditPortKind::Index || requested->kind == ArrayEditPortKind::Length)
                return known(TypeRef::primitive("i32"));
            return array;
        }
        const auto reducedType = [&](unsigned remaining) {
            return remaining ? TypeRef::array(data.children.front(), remaining) : data.children.front();
        };
        bool complete = array.complete;
        unsigned mask = 0U;
        TypeRef requestedValue;
        for (unsigned group = 0; group < groups; ++group) {
            if (masks[group]) mask = masks[group];
            const auto selected = mask ? static_cast<unsigned>(std::popcount(mask)) : 1U;
            const auto sliceRank = rank - (kind == ArrayEditKind::Replace ? selected : 1U);
            const auto expected = reducedType(kind == ArrayEditKind::Replace && !sliceRank ? rank : sliceRank);
            if (kind != ArrayEditKind::Replace && kind != ArrayEditKind::Insert) break;
            const auto name = arrayEditValueId(group);
            auto valueType = expected;
            complete = complete && wired(name);
            if (wired(name)) {
                auto value = input(name);
                if (!value.resolution.accepted()) return value;
                const auto &replacement = value.resolution.type;
                const auto replacementRank = replacement.kind == TypeKind::Array ? replacement.rank : 0U;
                const auto expectedRank = expected.kind == TypeKind::Array ? expected.rank : 0U;
                // An unwired origin selects dimension zero at index zero.
                // Partial origins retain exactly the unindexed dimensions;
                // a full origin accepts either one element or a full-rank block.
                const bool acceptedRank = kind == ArrayEditKind::Insert
                    ? replacementRank == rank - 1U || replacementRank == rank
                    : replacementRank == expectedRank || (selected == rank && replacementRank == 0);
                if (!acceptedRank)
                    return invalid("Replacement/insertion rank does not match the selected slice or full array block.");
                const auto &leaf = replacementRank ? replacement.children.front() : replacement;
                const auto compatible = connect(leaf, data.children.front(), ConnectionPolicy::NumericImplicit);
                if (!compatible.accepted()) return {compatible, false, {}};
                valueType = replacement;
                complete = complete && value.complete;
            }
            if (group == requested->group) requestedValue = std::move(valueType);
        }
        switch (requested->kind) {
        case ArrayEditPortKind::Index: case ArrayEditPortKind::Length: return known(TypeRef::primitive("i32"));
        case ArrayEditPortKind::Array: return array;
        case ArrayEditPortKind::Value: return known(std::move(requestedValue));
        case ArrayEditPortKind::Deleted: return result(array, wired("length") ? data : reducedType(rank - 1));
        case ArrayEditPortKind::Result: {
            auto output = array; output.complete = complete; return output;
        }
        }
    }
    if (id == "string.concatenate") {
        for (const auto &name : wiredInputs) {
            auto value = input(name);
            if (!value.resolution.accepted()) return value;
            const auto &t = value.resolution.type;
            if (t != TypeRef::primitive("string") &&
                !(t.kind == TypeKind::Array && t.rank == 1 && t.children.front() == TypeRef::primitive("string")))
                return invalid("Concatenate Strings accepts a String or one-dimensional String array.");
        }
        return declared->direction == FunctionPortDirection::Input ? input(port) : known(TypeRef::primitive("string"));
    }
    if (named(id, {"boolean.to_integer", "numeric.conversion.boolean_to_integer"})) {
        auto value = input("value");
        if (!value.resolution.accepted()) return value;
        const auto output = isErrorCluster(value.resolution.type) ? TypeRef::primitive("i16") :
            mapLeaves(value.resolution.type, [](const TypeRef &t) {
                return t == TypeRef::primitive("bool") ? TypeRef::primitive("i16") : TypeRef{};
            });
        if (!output.known()) return invalid("Boolean To Integer requires Boolean leaves.");
        return declared->direction == FunctionPortDirection::Input ? value : result(value, output);
    }
    if (isBooleanArrayToNumber(id)) {
        auto value = input("value");
        if (value.resolution.accepted() && value.resolution.type != TypeRef::array(TypeRef::primitive("bool")))
            return invalid("Boolean Array To Number requires a one-dimensional Boolean array.");
        auto output = parseType(node.valueType).type;
        if (!output.integer() && output.name != "fxp") output = TypeRef::primitive("u32");
        auto resolved = known(declared->direction == FunctionPortDirection::Input
            ? TypeRef::array(TypeRef::primitive("bool")) : output);
        resolved.complete=value.complete && value.resolution.accepted();
        return resolved;
    }
    if (named(id, {"boolean.number_to_array", "numeric.conversion.number_to_boolean_array"})) {
        return declared->direction == FunctionPortDirection::Input ? input(port) :
            known(parseType(findFunctionBindingPort(definition, "result")->defaultValueType).type);
    }
    if (id.starts_with("numeric.") && !id.starts_with("numeric.conversion."))
        return resolveSharedPolymorphicFunction({},definition,port,input,wiredInputs);
    if (id.starts_with("numeric.conversion.") || id.starts_with("string.conversion.")) {
        if (named(id, {"string.conversion.scan_from_string", "string.conversion.scan_value", "string.conversion.unflatten_from_json", "string.conversion.spreadsheet_string_to_array"})) {
            const bool json = id.ends_with("unflatten_from_json"), spreadsheet = id.ends_with("spreadsheet_string_to_array");
            auto targetType = input(json ? "type" : spreadsheet ? "array_type" : "default_value");
            if (!targetType.resolution.accepted())
                return declared->direction == FunctionPortDirection::Input ? input(port) : targetType;
            const auto &t = targetType.resolution.type;
            if (spreadsheet && (t.kind != TypeKind::Array || !(t.children.front().numeric() || t.children.front().name == "string")))
                return invalid("Spreadsheet type must be a numeric or String array.");
            if (json && !mapLeaves(t, [](const TypeRef &leaf) -> TypeRef {
                    return named(leaf.name,{"bool","string"}) || (realNumber(leaf) && leaf.name != "fxp") ? leaf : TypeRef{};
                }).known()) return invalid("Unsupported JSON target schema.");
            if (!json && !spreadsheet && (t.kind == TypeKind::Array || t.kind == TypeKind::Cluster ||
                !(t.numeric() || t.kind == TypeKind::Enum || named(t.name,{"bool","string","path","timestamp"}))))
                return invalid("Scanning requires a supported scalar default type.");
            return declared->direction == FunctionPortDirection::Input ? input(port) : targetType;
        }
        auto value = input("value");
        if (!value.resolution.accepted()) return value;
        const auto *output = findFunctionBindingPort(definition, "result");
        if (!output) return invalid("Conversion has no result contract.");
        const auto target = parseType(output->defaultValueType).type;
        if (id.ends_with("format_into_string") || id == "string.conversion.format_value") {
            const auto &t = value.resolution.type;
            const bool valid = t.numeric() || (id.ends_with("format_into_string") &&
                (t.kind == TypeKind::Enum || named(t.name, {"bool", "string", "path", "timestamp"})));
            if (!valid) return invalid("This formatting operation requires supported scalar values, not arrays or clusters.");
            return declared->direction == FunctionPortDirection::Input ? value : result(value, TypeRef::primitive("string"));
        }
        if (named(id, {"string.conversion.array_to_spreadsheet_string", "string.conversion.flatten_to_json"})) {
            const bool spreadsheet = id == "string.conversion.array_to_spreadsheet_string";
            if (spreadsheet && value.resolution.type.kind != TypeKind::Array)
                return invalid("Spreadsheet formatting requires an array.");
            const auto allowed = mapLeaves(value.resolution.type, [&](const TypeRef &t) -> TypeRef {
                if (t.name == "string" || (t.numeric() && !t.name.starts_with("c") && t.name != "fxp") ||
                    (!spreadsheet && t.name == "bool")) return t;
                return {};
            });
            if (!allowed.known() || (spreadsheet && value.resolution.type.children.front().kind == TypeKind::Cluster))
                return invalid("The input contains a type not supported by this text serialization.");
            return declared->direction == FunctionPortDirection::Input ? value : result(value, TypeRef::primitive("string"));
        }
        const bool toNumeric = id.starts_with("numeric.conversion.to_");
        const bool numberToString = id.find("number_to_") != std::string::npos && id.ends_with("_string");
        const bool stringToNumber = id.ends_with("_string_to_number");
        const auto converted = mapLeaves(value.resolution.type, [&](const TypeRef &leaf) -> TypeRef {
            if (toNumeric && (leaf.numeric() || leaf.kind == TypeKind::Enum ||
                    (named(id, {"numeric.conversion.to_dbl", "numeric.conversion.to_i64", "numeric.conversion.to_u32",
                        "numeric.conversion.to_u64"}) && leaf.name == "timestamp"))) {
                if (id == "numeric.conversion.to_fxp" && leaf.name.starts_with("c")) return {};
                return TypeRef(target);
            }
            if (numberToString && leaf.numeric()) return TypeRef::primitive("string");
            if (stringToNumber && leaf.name == "string") return TypeRef(target);
            if (id == "string.conversion.path_to_string" && leaf.name == "path") return TypeRef(target);
            if (id == "string.conversion.string_to_path" && leaf.name == "string") return TypeRef(target);
            return {};
        });
        if (!converted.known()) return invalid("This conversion does not accept the connected element type.");
        return declared->direction == FunctionPortDirection::Input ? input(port) : result(value, converted);
    }
    if (id == "array.initialize") {
        auto element = input("element");
        if (!element.resolution.accepted())
            return declared->direction == FunctionPortDirection::Input ? input(port) : element;
        if (element.resolution.type.kind == TypeKind::Array)
            return invalid("Initialize Array requires a scalar or cluster element, not a direct array element.");
        if (declared->direction == FunctionPortDirection::Input) return input(port);
        return result(element, TypeRef::array(element.resolution.type));
    }
    if (id == "array.build") {
        const auto count=buildArrayInputCount(node);
        const auto requested=buildArrayInputIndex(port);
        if(port!="result" && (!requested || *requested>=count))
            return invalid("Build Array terminal does not exist for this input count.");
        std::vector<bool> connected(count,false);
        for(const auto &name:wiredInputs) {
            const auto index=buildArrayInputIndex(name);
            if(!index || *index>=count) return invalid("A Build Array wire references a removed input.");
            connected[*index]=true;
        }
        std::vector<int> ranks(count,-1);
        std::vector<BindingResult> values(count);
        TypeResolution common;
        bool hasType=false, complete=true;
        for(unsigned i=0;i<count;++i) {
            complete=complete && connected[i];
            if(!connected[i]) continue;
            values[i]=input(buildArrayInputId(i));
            const auto &value=values[i];
            if(value.resolution.status==ResolutionStatus::Incompatible) return value;
            complete=complete && value.complete;
            if(!value.resolution.accepted()) { complete=false; continue; }
            const auto &t=value.resolution.type;
            ranks[i]=t.kind==TypeKind::Array ? static_cast<int>(t.rank):0;
            const auto &leaf=ranks[i] ? t.children.front():t;
            common=hasType ? commonType(common.type,leaf):TypeResolution{ResolutionStatus::Compatible,leaf,{},{}};
            if(!common.accepted()) return {common,false,{}};
            hasType=true;
        }
        if(!hasType) {
            // Authoring default only: an untouched Build Array proposes DBL
            // constants/controls. Never use this hint as connected type evidence
            // or hide a wired source whose type has not been resolved yet.
            if(!wiredInputs.empty()) return unresolved();
            auto hint=known(requested ? TypeRef::primitive("f64") :
                TypeRef::array(TypeRef::primitive("f64"),1));
            hint.complete=false;
            return hint;
        }
        const auto mode=buildArrayMode(node.buildArrayConcatenate,ranks);
        if(mode.invalidRanks) return invalid("Build Array operands may differ by at most one dimension.");
        if(mode.outputRank>16) return invalid("The resulting array exceeds the supported rank of 16. Concatenate equal-rank arrays to retain their rank.");
        if(requested) {
            // Unwired concatenate slots propose an element/slice; wired slots
            // retain their own rank while sharing the promoted element schema.
            const unsigned rank=ranks[*requested]>=0 ? static_cast<unsigned>(ranks[*requested]):
                mode.concatenate ? mode.outputRank-1:mode.maximumRank;
            auto requestedInput=known(rank ? TypeRef::array(common.type,rank):common.type);
            requestedInput.complete=connected[*requested] && values[*requested].complete;
            return requestedInput;
        }
        common.type=TypeRef::array(std::move(common.type),mode.outputRank);
        return {common,complete,{}};
    }
    if (id == "array.interleave_1d") {
        auto a = input("array_a"), b = input("array_b");
        if (a.resolution.status == ResolutionStatus::Incompatible) return a;
        if (b.resolution.status == ResolutionStatus::Incompatible) return b;
        if (!a.resolution.accepted() && !b.resolution.accepted()) return unresolved();
        const auto &left = a.resolution.accepted() ? a.resolution.type : b.resolution.type;
        const auto &right = b.resolution.accepted() ? b.resolution.type : left;
        const bool arrays = left.kind == TypeKind::Array && right.kind == TypeKind::Array;
        if (id == "array.interleave_1d" && (!arrays || left.rank != 1 || right.rank != 1))
            return invalid("Interleave 1D Arrays requires one-dimensional arrays.");
        const auto leftRank = left.kind == TypeKind::Array ? left.rank : 0U;
        const auto rightRank = right.kind == TypeKind::Array ? right.rank : 0U;
        if ((std::max)(leftRank, rightRank) - (std::min)(leftRank, rightRank) > 1U)
            return invalid("Build Array operands may differ by at most one dimension.");
        auto common = commonType(leftRank ? left.children.front() : left,
            rightRank ? right.children.front() : right);
        if (!common.accepted()) return {common, false, {}};
        const auto rank = id == "array.interleave_1d" ? 1U :
            leftRank == rightRank ? leftRank + 1U : (std::max)(leftRank, rightRank);
        if (rank > 16U) return invalid("The resulting array exceeds the supported rank of 16.");
        if (declared->direction == FunctionPortDirection::Input) return input(port);
        common.type = TypeRef::array(std::move(common.type), rank);
        return {common, a.complete && b.complete, {}};
    }
    if (id.starts_with("array.") && !id.starts_with("array.conversion.")) {
        auto array = input("array");
        if (!array.resolution.accepted())
            return declared->direction == FunctionPortDirection::Input ? input(port) : array;
        TypeRef type = array.resolution.type;
        // The documented waveform overload acts on Y. Array Subset keeps the
        // waveform envelope; Index/Size/Max & Min expose its data instead.
        if (type.kind == TypeKind::Cluster && named(id,
                {"array.index", "array.size", "array.max_min", "array.subset"})) {
            const auto field = std::find(type.fieldIds.begin(), type.fieldIds.end(), "Y");
            if (field != type.fieldIds.end() &&
                std::count_if(type.children.begin(), type.children.end(), [](const auto &child) {
                    return child.kind == TypeKind::Array;
                }) == 1)
            {
                auto data = type.children[static_cast<std::size_t>(field - type.fieldIds.begin())];
                type = std::move(data);
            }
        }
        if (type.kind != TypeKind::Array || type.children.size() != 1)
            return invalid("This array operation requires an array input.");
        if ((id.ends_with("_1d") || id == "array.decimate_1d") && type.rank != 1)
            return invalid("This operation requires a one-dimensional array.");
        if (id == "array.transpose_2d" && type.rank != 2)
            return invalid("Transpose 2D Array requires two dimensions.");
        if (named(id, {"array.interpolate_1d", "array.threshold_1d"})) {
            const auto &element = type.children.front();
            const bool point = element.kind == TypeKind::Cluster && element.children.size() == 2 &&
                std::all_of(element.children.begin(), element.children.end(), realNumber);
            if (!realNumber(element) && !point)
                return invalid("Interpolation requires real numbers or two-real-number point clusters.");
            return declared->direction == FunctionPortDirection::Input ? input(port) :
                result(array, TypeRef::primitive("f64"));
        }
        if (named(id, {"array.replace_subset", "array.insert", "array.search_1d"})) {
            const auto otherName = id == "array.search_1d" ? "value" : "subset";
            if (wired(otherName)) {
                auto other = input(otherName);
                if (!other.resolution.accepted()) return other;
                const auto &replacement = other.resolution.type;
                const auto expected = id == "array.search_1d" ? type.children.front()
                    : replacement.kind == TypeKind::Array && replacement.rank == type.rank ? type : dropDimension(type);
                const auto compatible = commonType(expected, replacement);
                if (!compatible.accepted()) return {compatible, false, {}};
            }
        }
        if (id == "array.index") {
            const auto requested = parseIndexArrayPort(port);
            const auto validPort = [&](const IndexArrayPort &p) {
                return p.output < indexArrayOutputCount(node) &&
                    (p.isOutput || p.dimension < type.rank);
            };
            if (port != "array" && (!requested || !validPort(*requested)))
                return invalid("Index Array terminal does not exist for this rank/output count.");
            std::vector<unsigned> indexed(indexArrayOutputCount(node), 0U);
            for (const auto &name : wiredInputs) {
                if (name == "array") continue;
                const auto index = parseIndexArrayPort(name);
                if (!index || index->isOutput || !validPort(*index))
                    return invalid("An Index Array wire references a removed dimension or output group.");
                indexed[index->output] |= 1U << index->dimension;
            }
            if (declared->direction == FunctionPortDirection::Input) {
                if(port=="array")return array;
                const auto value=input(port);
                if(value.resolution.accepted()&&!realNumber(value.resolution.type))
                    return invalid("Array indices require real numeric scalars.");
                auto expected=known(TypeRef::primitive("i32"));
                expected.complete=!wired(port)||value.complete;
                return expected;
            }
            // An entirely unwired group follows the previous slice; the first
            // group indexes dimension zero by default (also for a 1D array).
            unsigned mask = 1U;
            for (unsigned group = 0; group <= requested->output; ++group)
                if (indexed[group]) mask = indexed[group];
            unsigned removed = 0;
            for (; mask; mask >>= 1) removed += mask & 1U;
            return result(array, removed >= type.rank ? type.children.front() :
                TypeRef::array(type.children.front(), type.rank - removed));
        }
        if (declared->direction == FunctionPortDirection::Input) return input(port);
        if (id == "array.size" || (id == "array.max_min" && port.ends_with("_index")))
            return result(array, type.rank == 1 ? TypeRef::primitive("i32") : TypeRef::array(TypeRef::primitive("i32")));
        if (id == "array.max_min") return result(array, type.children.front());
        if (id == "array.search_1d") return result(array, TypeRef::primitive("i32"));
        return array;
    }
    return invalid("The declared polymorphic resolver has no implementation for " + definition.catalogId + ".");
}

BindingResult resolvePolymorphicFunction(const DiagramNode &node,
    const FunctionBindingDefinition &definition, std::string_view port,
    const FunctionInputResolver &input, std::span<const std::string> wiredInputs,
    std::optional<std::string_view> formatLiteral) {
    const auto hint = arrayAuthoringDefault(definition, wiredInputs);
    if (!hint) return resolvePolymorphicFunctionImpl(node, definition, port, input, wiredInputs, formatLiteral);
    bool usedHint = false;
    const FunctionInputResolver hintedInput = [&](std::string_view name) {
        auto value = input(name);
        if (name == hint->source && value.resolution.status == ResolutionStatus::Unresolved) {
            usedHint = true;
            value = known(hint->type);
            value.complete = false;
        }
        return value;
    };
    auto value = resolvePolymorphicFunctionImpl(node, definition, port, hintedInput, wiredInputs, formatLiteral);
    if (usedHint) {
        // A useful type in the editor is not evidence that a required runtime
        // value exists. Keep the suggested source SGL even for DBL algorithms.
        if (port == hint->source && value.resolution.accepted()) value = known(hint->type);
    }
    // Some conversions have a fixed output type and need not query their
    // source to determine it. That source is still missing at runtime.
    value.complete = false;
    return value;
}
}
