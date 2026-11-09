    if (id.starts_with("comparison.")) {
        const bool binary = comparisonHasMode(id);
        const bool equality = named(id, {"comparison.equal", "comparison.not_equal"});
        const bool equalityZero = named(id, {"comparison.equal_zero", "comparison.not_equal_zero"});
        const bool character = named(id, {"comparison.decimal_digit", "comparison.hex_digit",
            "comparison.octal_digit", "comparison.white_space"});
        const auto booleanShape = [&](auto &&self, const TypeRef &type) -> TypeRef {
            if (type.kind == TypeKind::Array || type.kind == TypeKind::Cluster) {
                TypeRef shape = type;
                for (auto &child : shape.children) {
                    child = self(self, child);
                    if (!child.known()) return {};
                }
                return shape;
            }
            bool accepted = false;
            if (binary) {
                accepted = equality ? type.known() :
                    realNumber(type) || type.kind == TypeKind::Enum ||
                    named(type.name, {"bool", "string", "timestamp"});
            } else if (character) {
                accepted = realNumber(type) || type.name == "string";
            } else if (id == "comparison.empty_string_or_path") {
                accepted = named(type.name, {"string", "path", "variant"});
            } else if (id == "comparison.not_a_number_path_refnum") {
                // Refnums have no type in the current Studio binding algebra.
                // They must not silently become integers or numeric placeholders.
                accepted = type.numeric() || type.name == "path";
            } else {
                accepted = (equalityZero ? type.numeric() : realNumber(type)) ||
                    type.name == "timestamp";
            }
            return accepted ? TypeRef::primitive("bool") : TypeRef{};
        };
        // LabVIEW compares cluster fields by order and type, not by label.
        const auto alignFieldNames = [&](auto &&self, const TypeRef &left, TypeRef right) -> TypeRef {
            if (left.kind == right.kind && left.children.size() == right.children.size()) {
                if (left.kind == TypeKind::Cluster) right.fieldIds = left.fieldIds;
                for (std::size_t i = 0; i < right.children.size(); ++i)
                    right.children[i] = self(self, left.children[i], std::move(right.children[i]));
            }
            return right;
        };
        TypeRef operand;
        bool complete = true;
        for (const auto &name : wiredInputs) {
            if (name != "x" && !(binary && name == "y")) continue;
            auto source = input(name);
            if (!source.resolution.accepted()) return source;
            complete = complete && source.complete;
            const auto &type = source.resolution.type;
            if (id == "comparison.empty_array") {
                if (type.kind != TypeKind::Array)
                    return invalid("Empty Array requires an array of any rank.");
            } else if (!booleanShape(booleanShape, type).known()) {
                return invalid("This Comparison function does not accept the connected element type.");
            }
            if (!operand.known()) operand = type;
            else {
                const auto common = commonType(operand, alignFieldNames(alignFieldNames, operand, type), true);
                if (!common.accepted()) return {common, false, {}};
                operand = common.type;
            }
        }
        if (!operand.known()) return known(parseType(declared->defaultValueType).type);
        if (declared->direction == FunctionPortDirection::Input) {
            if (std::find(wiredInputs.begin(), wiredInputs.end(), port) != wiredInputs.end())
                return input(port); // Keep the wired scalar/array shape at each input.
            return known(operand);
        }
        const auto output = id == "comparison.empty_array" || (binary && node.comparisonAggregates)
            ? TypeRef::primitive("bool") : booleanShape(booleanShape, operand);
        return {TypeResolution{ResolutionStatus::Compatible, output, {}, {}}, complete, {}};
    }
