#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine::binding {

enum class TypeKind { Unknown, Primitive, Array, Enum, Cluster, WaveformGraph, Reference, Matrix, Map, Set };
enum class ChoiceKind { None, Enum, Ring, Dropdown };
struct ChoiceItemMetadata {
    std::string id;
    std::string label;
    std::uint32_t value = 0;
    bool disabled = false;
    bool visible = true;
    bool operator==(const ChoiceItemMetadata &) const = default;
};

// Value semantics: no UI handles, mutable caches, or layout-dependent identity.
struct TypeRef {
    TypeKind kind = TypeKind::Unknown;
    std::string name;
    std::size_t rank = 0U;
    std::optional<std::size_t> extent;
    std::vector<TypeRef> children;
    std::vector<std::string> fieldIds;
    // Authoring names follow the source schema. They do not replace stable
    // field IDs in structural compatibility or the canonical type encoding.
    std::vector<std::string> fieldNames;
    // Enum labels, values and order form its definition. Choice IDs,
    // visibility and disabled state remain presentation metadata, as do Ring
    // and Dropdown choices. Definitions travel through Cluster functions.
    ChoiceKind choiceKind = ChoiceKind::None;
    std::vector<ChoiceItemMetadata> choices;

    bool operator==(const TypeRef &other) const {
        if (kind!=other.kind || name!=other.name || rank!=other.rank ||
            extent!=other.extent || children!=other.children || fieldIds!=other.fieldIds) return false;
        if (kind!=TypeKind::Enum) return true;
        if (choices.size()!=other.choices.size()) return false;
        for (std::size_t index=0; index<choices.size(); ++index)
            if (choices[index].label!=other.choices[index].label ||
                choices[index].value!=other.choices[index].value) return false;
        return true;
    }
    [[nodiscard]] bool known() const;
    [[nodiscard]] bool numeric() const;
    [[nodiscard]] bool integer() const;
    [[nodiscard]] std::string canonical() const;
    [[nodiscard]] static TypeRef reference(std::string classId, std::optional<TypeRef> valueType = {});
    [[nodiscard]] static TypeRef primitive(std::string_view name);
    [[nodiscard]] static TypeRef array(TypeRef element, std::size_t rank = 1U);
    // Distinct private Matrix profile; no implicit Array conversion or public
    // FROG promotion. This first value transport admits f64 elements only.
    [[nodiscard]] static TypeRef matrix(TypeRef element);
    // Local nominal Map value; keys and values keep their complete schemas.
    [[nodiscard]] static TypeRef map(TypeRef key, TypeRef value);
    [[nodiscard]] static TypeRef set(TypeRef element);
    [[nodiscard]] static TypeRef enumeration(std::string domain, TypeRef representation);
    [[nodiscard]] static TypeRef cluster(std::vector<std::string> ids, std::vector<TypeRef> fields);
};

struct TypeParseResult {
    TypeRef type;
    std::string error;
    bool legacy = false;
    [[nodiscard]] bool valid() const { return error.empty(); }
};

// Legacy spellings are accepted only at this boundary; canonical() never emits them.
[[nodiscard]] TypeParseResult parseType(std::string_view text);
[[nodiscard]] TypeRef withArrayRank(TypeRef type, std::size_t rank);
// Complete authoring schema identity, including nominal/presentation metadata.
[[nodiscard]] bool fullTypeEqual(const TypeRef &,const TypeRef &);

// Error is a structural interpretation of an ordinary cluster, not a distinct
// widget/type kind. Field IDs and display names remain authored identities;
// only the ordered Boolean, signed I32, String triplet carries this meaning.
[[nodiscard]] bool isErrorCluster(const TypeRef &type);
// Class upcasts only; a strict value schema is invariant, never numerically coerced.
[[nodiscard]] bool referenceClassAssignable(std::string_view source, std::string_view target) noexcept;
// Native object classes shared by reference providers and authoring catalogues.
// A declared class is not evidence that a host can create a live object of it.
[[nodiscard]] std::span<const std::string_view> nativeObjectReferenceClasses() noexcept;

enum class ConstraintKind { Concrete, Any, Numeric, Integer, Cluster, CaseSelector, SameAs,
    NumericTree, NumericConvertibleTree, LogicalTree, StringTree, PathTree,
    FileOrPath, TextFileValue, Spreadsheet, SpreadsheetRow, BinaryFileValue };
struct TypeExpr {
    ConstraintKind kind = ConstraintKind::Concrete;
    TypeRef concrete;
    std::string variable;
    std::size_t minimumRank = 0U;
    std::size_t maximumRank = 0U; // zero means no upper bound
    [[nodiscard]] static TypeExpr concreteType(TypeRef type);
};
struct TypeExprParseResult {
    TypeExpr expression;
    std::string error;
    [[nodiscard]] bool valid() const { return error.empty(); }
};
[[nodiscard]] TypeExprParseResult parseTypeExpr(std::string_view text);

enum class ResolutionStatus { Compatible, Coercion, Unresolved, Incompatible };
enum class ConnectionPolicy { Exact, NumericImplicit };
struct TypeResolution {
    ResolutionStatus status = ResolutionStatus::Unresolved;
    TypeRef type;
    std::string code;
    std::string message;
    [[nodiscard]] bool accepted() const {
        return status == ResolutionStatus::Compatible || status == ResolutionStatus::Coercion;
    }
};

[[nodiscard]] TypeResolution connect(const TypeRef &source, const TypeRef &target,
    ConnectionPolicy policy = ConnectionPolicy::Exact);
[[nodiscard]] TypeResolution constrain(const TypeRef &source, const TypeExpr &expression);
[[nodiscard]] TypeResolution unify(std::span<const TypeRef> sources);
[[nodiscard]] bool acceptsCaseSelector(const TypeRef &type);

} // namespace frog::native_engine::binding
