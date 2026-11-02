#include "frog_native_engine/BindingType.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <utility>

namespace frog::native_engine::binding {
namespace {
TypeResolution mismatch(std::string code, std::string message) {
    return {ResolutionStatus::Incompatible, {}, std::move(code), std::move(message)};
}
std::string_view trimmed(std::string_view value) {
    const auto start = value.find_first_not_of(" \r\n\t");
    return start == std::string_view::npos ? std::string_view{} :
        value.substr(start, value.find_last_not_of(" \r\n\t") - start + 1U);
}
bool referenceId(std::string_view value) {
    if (value.empty() || (!std::isalpha(static_cast<unsigned char>(value.front())) && value.front() != '_')) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return std::isalnum(c) || c == '_' || c == '.' || c == '-';
    });
}
bool binaryFileValue(const TypeRef &type) {
    const TypeRef *value=&type;
    if(type.kind==TypeKind::Array) {
        if(type.rank<1 || type.rank>16 || type.children.size()!=1) return false;
        value=&type.children.front();
    }
    if(value->kind!=TypeKind::Primitive) return false;
    constexpr std::array<std::string_view,8> integers{"i8","u8","i16","u16","i32","u32","i64","u64"};
    return std::find(integers.begin(),integers.end(),value->name)!=integers.end() || value->name=="f32" || value->name=="f64" ||
        value->name=="c32" || value->name=="c64" || value->name=="bool";
}
bool acceptsTree(const TypeRef &type,ConstraintKind domain,std::string &reason) {
    struct Frame {const TypeRef *type;std::string path;};
    std::vector<Frame> pending{{&type,"Value"}};
    while(!pending.empty()) {
        auto frame=std::move(pending.back());pending.pop_back();const auto &value=*frame.type;
        if(domain==ConstraintKind::LogicalTree && isErrorCluster(value)) continue;
        if(value.kind==TypeKind::Array || value.kind==TypeKind::Cluster) {
            if(value.children.empty() || (value.kind==TypeKind::Array && value.children.front().kind==TypeKind::Array)) {
                reason="Unsupported aggregate shape at "+frame.path+".";return false;
            }
            for(std::size_t i=value.children.size();i-- >0;) {
                auto path=frame.path;
                if(value.kind==TypeKind::Array) path+="[array rank "+std::to_string(value.rank)+"]";
                else {
                    const auto &name=i<value.fieldNames.size()?value.fieldNames[i]:
                        i<value.fieldIds.size()?value.fieldIds[i]:std::string{};
                    path+="."+name+" [field "+std::to_string(i)+"]";
                }
                pending.push_back({&value.children[i],std::move(path)});
            }
            continue;
        }
        bool accepted=false;
        switch(domain) {
        case ConstraintKind::NumericTree: accepted=value.numeric();break;
        case ConstraintKind::NumericConvertibleTree: accepted=value.numeric() || value.kind==TypeKind::Enum || value.name=="timestamp";break;
        case ConstraintKind::LogicalTree: accepted=value.name=="bool" || (value.numeric() && !value.name.starts_with("c"));break;
        case ConstraintKind::StringTree: accepted=value.name=="string";break;
        case ConstraintKind::PathTree: accepted=value.name=="path";break;
        default: break;
        }
        if(!accepted) {
            reason="Unsupported type '"+value.canonical()+"' at "+frame.path+". "+
                ((domain==ConstraintKind::NumericTree || domain==ConstraintKind::NumericConvertibleTree)
                    ? "This numeric port requires compatible numeric elements throughout the aggregate."
                    : "Every element must satisfy this port's type constraint.");
            return false;
        }
    }
    return true;
}
}
bool acceptsCaseSelector(const TypeRef &type) {
    return type.known() && (type.integer() || type.kind == TypeKind::Enum || isErrorCluster(type) ||
        (type.kind == TypeKind::Primitive && (type.name == "bool" || type.name == "string")));
}
TypeExpr TypeExpr::concreteType(TypeRef type) {
    TypeExpr result; result.concrete = std::move(type); return result;
}
static TypeExprParseResult parseTypeExprImpl(std::string_view text, std::size_t depth) {
    text = trimmed(text);
    TypeExprParseResult result;
    if (text.size() > 65536U) return {{}, "Type expression exceeds parser limits."};
    if (text == "case_selector") { result.expression.kind = ConstraintKind::CaseSelector; return result; }
    if (text == "any") { result.expression.kind = ConstraintKind::Any; return result; }
    if (text.starts_with("same(") && text.ends_with(")")) {
        result.expression.kind = ConstraintKind::SameAs;
        result.expression.variable = text.substr(5U, text.size() - 6U);
        if (!referenceId(result.expression.variable)) result.error = "same() requires one valid port ID.";
        return result;
    }
    const auto rankAt = text.rfind(",rank");
    if (text.starts_with("array<") && text.ends_with('>') && rankAt != std::string_view::npos &&
        text.find('>', rankAt+7U) == text.size()-1U) {
        const bool minimum = text.substr(rankAt).starts_with(",rank>=");
        const bool exact = text.substr(rankAt).starts_with(",rank=");
        if (!minimum && !exact) return {{}, "Invalid array rank constraint."};
        const auto number = text.substr(rankAt + (minimum ? 7U : 6U), text.size()-rankAt-(minimum ? 8U : 7U));
        std::size_t rank = 0U;
        const auto parsed = std::from_chars(number.data(), number.data()+number.size(), rank);
        if (parsed.ec != std::errc{} || parsed.ptr != number.data()+number.size() || rank < 1U || rank > 16U)
            return {{}, "Array rank must be between 1 and 16."};
        const auto nested = parseTypeExprImpl(text.substr(6U, rankAt - 6U), depth + 1U);
        result = nested;
        if (nested.expression.minimumRank != 0U)
            result.error = "Nested array constraints require a separately registered expression form.";
        result.expression.minimumRank = rank;
        result.expression.maximumRank = exact ? rank : 0U;
        return result;
    }
    const auto colon = text.find(':');
    if (colon != std::string_view::npos) {
        const auto constraint = text.substr(colon + 1U);
        static constexpr std::pair<std::string_view, ConstraintKind> domains[]{
            {"numeric", ConstraintKind::Numeric}, {"integer", ConstraintKind::Integer},
            {"cluster", ConstraintKind::Cluster}, {"any", ConstraintKind::Any},
            {"numeric_tree", ConstraintKind::NumericTree}, {"numeric_convertible_tree", ConstraintKind::NumericConvertibleTree},
            {"logical_tree", ConstraintKind::LogicalTree}, {"string_tree", ConstraintKind::StringTree}, {"path_tree", ConstraintKind::PathTree},
            {"path_or_file", ConstraintKind::FileOrPath}, {"text", ConstraintKind::TextFileValue},
            {"spreadsheet", ConstraintKind::Spreadsheet}, {"spreadsheet_row", ConstraintKind::SpreadsheetRow},
            {"binary_array", ConstraintKind::BinaryFileValue}};
        for (const auto &[name, domain] : domains) if (constraint == name) {
            result.expression.kind = domain;
            result.expression.variable = text.substr(0U, colon);
            if (!referenceId(result.expression.variable)) result.error = "A type variable requires a valid name.";
            return result;
        }
    }
    const auto parsed = parseType(text);
    result.expression.concrete = parsed.type;
    result.error = parsed.error;
    if (result.error.empty() && !parsed.type.known()) result.error = "A concrete port expression requires a known type.";
    return result;
}
TypeExprParseResult parseTypeExpr(std::string_view text) { return parseTypeExprImpl(text, 0U); }
TypeResolution connect(const TypeRef &source, const TypeRef &target, ConnectionPolicy policy) {
    if (!source.known() || !target.known()) {
        return {ResolutionStatus::Unresolved, {}, "binding.type_unresolved", "A terminal type is not yet determined."};
    }
    if (source.kind == TypeKind::Reference || target.kind == TypeKind::Reference) {
        if (source.kind != TypeKind::Reference || target.kind != TypeKind::Reference)
            return mismatch("binding.reference_required", "Object references and data values are distinct.");
        if (!referenceClassAssignable(source.name, target.name))
            return mismatch("binding.reference_class", "Reference classes require a checked downcast.");
        if (!target.children.empty() && (source.children.empty() || source.children.front() != target.children.front()))
            return mismatch("binding.reference_schema", "A strict reference requires the exact complete value schema.");
        return {ResolutionStatus::Compatible, target, {}, {}};
    }
    if (source.kind==TypeKind::Enum && target.kind==TypeKind::Enum) {
        if (source.name!=target.name || source.children!=target.children)
            return mismatch("binding.enum_domain_mismatch", "Enum domain or representation differs.");
        // Parsed constraints carry only a domain and representation. Real
        // terminals additionally supply the ordered enumerator definition.
        if (source.choiceKind==ChoiceKind::Enum && target.choiceKind==ChoiceKind::Enum) {
            if (source.choices.size()!=target.choices.size())
                return mismatch("binding.enum_definition_mismatch", "Enum definitions differ: expected " +
                    std::to_string(target.choices.size()) + " enumerators, received " +
                    std::to_string(source.choices.size()) + ". Names, values and order must match.");
            for (std::size_t index=0; index<source.choices.size(); ++index) {
                const auto &received=source.choices[index], &expected=target.choices[index];
                if (received.label!=expected.label || received.value!=expected.value)
                    return mismatch("binding.enum_definition_mismatch", "Enum enumerator #" +
                        std::to_string(index+1) + " differs: expected '" + expected.label + "' (value " +
                        std::to_string(expected.value) + "), received '" + received.label + "' (value " +
                        std::to_string(received.value) + "). Names, values and order must match.");
            }
        }
        return {ResolutionStatus::Compatible, target, {}, {}};
    }
    if(source.kind==TypeKind::Map||target.kind==TypeKind::Map||source.kind==TypeKind::Set||target.kind==TypeKind::Set) {
        if(fullTypeEqual(source,target))return {ResolutionStatus::Compatible,target,{}, {}};
        return mismatch("binding.collection_schema","Collection values require exactly the same complete nominal schemas; no implicit coercion is performed.");
    }
    if (source == target) return {ResolutionStatus::Compatible, target, {}, {}};
    // Positional error roles accept authored IDs without renaming stored fields
    // or loosening field identity for other cluster schemas.
    if (isErrorCluster(source) && isErrorCluster(target))
        return {ResolutionStatus::Compatible, target, {}, {}};
    if (target.kind == TypeKind::WaveformGraph) {
        const auto leaf = source.kind == TypeKind::Array && source.children.size() == 1U ? &source.children.front() : nullptr;
        if (leaf && (source.rank == 1U || source.rank == 2U) && leaf->kind == TypeKind::Primitive &&
            (leaf->name == "f64" || leaf->name == "f32" || leaf->name == "i8" || leaf->name == "u8" ||
             leaf->name == "i16" || leaf->name == "u16" || leaf->name == "i32" || leaf->name == "u32"))
            return {ResolutionStatus::Coercion, target, source.rank == 1 ? "wfg.y_array" : "wfg.row_matrix",
                "Normalize to complete Waveform Graph series (2D: one series per row), without history."};
        return mismatch("wfg.adapter_required", "Waveform Graph expects a numeric 1D/2D array or frog.waveform_graph_value. Convert i64/u64 explicitly to f64; scalar input does not accumulate history.");
    }
    if (source.kind != target.kind) return mismatch("binding.kind_mismatch", "The value kinds differ.");
    if (source.kind == TypeKind::Map)
        return mismatch("binding.map_schema", "Map keys and values require the exact complete schemas.");
    if(source.kind==TypeKind::Set)return mismatch("binding.set_schema","Set elements require the exact complete schema.");
    if (source.kind == TypeKind::Array) {
        if (source.rank != target.rank) return mismatch("binding.rank_mismatch",
            "Expected rank " + std::to_string(target.rank) + ", received rank " + std::to_string(source.rank) + ".");
        if (source.extent != target.extent) return mismatch("binding.shape_mismatch", "Contractual array extents differ.");
        auto leaf = connect(source.children.front(), target.children.front(), policy);
        if (leaf.accepted()) leaf.type = target;
        return leaf;
    }
    if (source.kind == TypeKind::Cluster) {
        if (source.fieldIds != target.fieldIds) return mismatch("binding.cluster_fields_mismatch",
            "Cluster field IDs or logical order differ.");
        bool coercion = false;
        for (std::size_t i = 0U; i < source.children.size(); ++i) {
            // Cluster field conversion is explicit; aggregate identity is exact.
            auto field = connect(source.children[i], target.children[i]);
            if (!field.accepted()) {
                field.message = "Field " + source.fieldIds[i] + ": " + field.message;
                return field;
            }
            coercion = coercion || field.status == ResolutionStatus::Coercion;
        }
        return {coercion ? ResolutionStatus::Coercion : ResolutionStatus::Compatible, target, {}, {}};
    }
    if (policy == ConnectionPolicy::NumericImplicit && source.numeric() && target.numeric()) {
        return {ResolutionStatus::Coercion, target, "binding.numeric_coercion", "Numeric conversion is required."};
    }
    return mismatch("binding.type_mismatch", "Expected " + target.canonical() + ", received " + source.canonical() + ".");
}
TypeResolution constrain(const TypeRef &source, const TypeExpr &expression) {
    if (!source.known()) return {ResolutionStatus::Unresolved, {}, "binding.type_unresolved", "A source type is required."};
    const TypeRef *value = &source;
    if (expression.minimumRank > 0U) {
        if (source.kind != TypeKind::Array || source.rank < expression.minimumRank ||
            (expression.maximumRank != 0U && source.rank > expression.maximumRank))
            return mismatch("binding.array_required", "An array with the declared rank is required.");
        value = &source.children.front();
    }
    switch (expression.kind) {
    case ConstraintKind::Concrete: {
        auto result = connect(*value, expression.concrete);
        if (result.accepted()) result.type = source;
        return result;
    }
    case ConstraintKind::SameAs: return {ResolutionStatus::Unresolved, {}, "binding.reference_unresolved", "Resolve the referenced port first."};
    case ConstraintKind::Numeric: if (!value->numeric()) return mismatch("binding.numeric_required", "A numeric value is required."); break;
    case ConstraintKind::NumericTree:
    case ConstraintKind::NumericConvertibleTree:
    case ConstraintKind::LogicalTree:
    case ConstraintKind::StringTree:
    case ConstraintKind::PathTree: {
        std::string reason;
        if (!acceptsTree(*value,expression.kind,reason)) return mismatch("binding.leaf_domain",std::move(reason));
        break;
    }
    case ConstraintKind::Integer: if (!value->integer()) return mismatch("binding.integer_required", "An integer value is required."); break;
    case ConstraintKind::CaseSelector: if (!acceptsCaseSelector(source)) return mismatch("binding.selector_type", "Case accepts Boolean, integer, String, Enum, or Error Cluster selectors."); break;
    case ConstraintKind::Cluster: if (value->kind != TypeKind::Cluster) return mismatch("binding.cluster_required", "A cluster value is required."); break;
    case ConstraintKind::Any: break;
    case ConstraintKind::FileOrPath:
        if(*value!=TypeRef::primitive("path") && *value!=TypeRef::reference("file"))
            return mismatch("binding.file_or_path_required","A path or an open file reference is required.");
        break;
    case ConstraintKind::TextFileValue:
        if(*value!=TypeRef::primitive("string") && !(value->kind==TypeKind::Array && value->rank==1 &&
            value->children.size()==1 && value->children.front()==TypeRef::primitive("string")))
            return mismatch("binding.file_text_required","A String or a rank-one String array is required.");
        break;
    case ConstraintKind::Spreadsheet:
    case ConstraintKind::SpreadsheetRow:
        if(value->kind!=TypeKind::Array || value->rank<1 ||
            value->rank>(expression.kind==ConstraintKind::SpreadsheetRow?1U:2U) || value->children.size()!=1 ||
            (value->children.front()!=TypeRef::primitive("f64") && value->children.front()!=TypeRef::primitive("i64") &&
             value->children.front()!=TypeRef::primitive("string")))
            return mismatch("binding.file_spreadsheet_required","Spreadsheet data requires the supported array rank and DBL, I64 or String elements.");
        break;
    case ConstraintKind::BinaryFileValue:
        if(!binaryFileValue(*value))
            return mismatch("binding.file_binary_required","Binary file data requires a supported scalar or primitive array.");
        break;
    }
    return {ResolutionStatus::Compatible, source, {}, {}};
}
TypeResolution unify(std::span<const TypeRef> sources) {
    TypeResolution result;
    for (const auto &source : sources) {
        if (!source.known()) continue;
        if (!result.type.known()) { result = {ResolutionStatus::Compatible, source, {}, {}}; continue; }
        const auto comparison = connect(source, result.type);
        if (!comparison.accepted()) return comparison;
    }
    if (!result.type.known()) {
        result.code = "binding.type_unresolved";
        result.message = "No typed source determines this binding.";
    }
    return result;
}
} // namespace frog::native_engine::binding
