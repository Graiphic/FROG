// Authoring suggestions only. A connected data operand (even unresolved) always
// takes precedence; auxiliary indices/sizes do not determine the element type.
struct ArrayAuthoringDefault {
    std::string_view source;
    TypeRef type;
};
std::optional<ArrayAuthoringDefault> arrayAuthoringDefault(
    const FunctionBindingDefinition &definition, std::span<const std::string> wiredInputs) {
    auto id = std::string_view(definition.catalogId);
    if (id == "string.conversion.array_to_spreadsheet_string") id = "array.conversion.array_to_spreadsheet_string";
    if (id == "cluster.conversion.array_to_cluster") id = "array.conversion.array_to_cluster";
    if (id == "cluster.conversion.cluster_to_array") id = "array.conversion.cluster_to_array";
    if (!id.starts_with("array.") || id == "array.build") return {};
    const auto kind = arrayPaletteKind(id);
    ArrayAuthoringDefault hint{"array", TypeRef::array(TypeRef::primitive("f32"), 1)};
    if (kind == ArrayPaletteKind::Interleave || kind == ArrayPaletteKind::BuildMatrix) {
        if (!wiredInputs.empty()) return {};
        if (kind == ArrayPaletteKind::Interleave) hint.source = "array_a";
        else hint = {"element1", TypeRef::primitive("f32")};
    } else if (kind == ArrayPaletteKind::Initialize) {
        hint = {"element", TypeRef::primitive("f32")};
    } else if (id.starts_with("array.matrix.")) {
        hint = {"matrix", TypeRef::array(TypeRef::primitive("f32"), 2)};
    } else if (id == "array.transpose_2d") {
        hint.type = TypeRef::array(TypeRef::primitive("f32"), 2);
    } else if (id.starts_with("array.conversion.") &&
            id != "array.conversion.array_to_cluster" && id != "array.conversion.array_to_spreadsheet_string") {
        // Preserve fixed Boolean/U8 conversions and the existing explicit
        // Spreadsheet String To Array type selector (DBL 2D). Cluster To Array
        // needs a real cluster schema; an Array hint must not invent its fields.
        return {};
    }
    const auto *source = findFunctionBindingPort(definition, hint.source);
    if (!source || source->direction != FunctionPortDirection::Input) return {};
    for (const auto &name : wiredInputs) {
        if (name == hint.source || (id == "array.search_1d" && name == "value")) return {};
        if (arrayEditKind(id) != ArrayEditKind::None) {
            const auto port = parseArrayEditPort(name);
            if (port && port->kind == ArrayEditPortKind::Value) return {};
        }
        if (id.starts_with("array.matrix.")) {
            const auto port = arrayPaletteSplitPort(name);
            if (port && named(port->first, {"elements", "diagonal_values", "subset"})) return {};
        }
    }
    return hint;
}
