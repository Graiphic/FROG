// Included in resolvePolymorphicFunction: authoring only, no evaluation.
if (id.starts_with("numeric.") && definition.maturity == FunctionBindingMaturity::ReferenceVerified &&
    !id.starts_with("numeric.conversion.") &&
    (id.starts_with("numeric.data_manipulation.") || id.starts_with("numeric.complex.") ||
     id.starts_with("numeric.scaling.") || named(id,{"numeric.quotient_remainder","numeric.increment",
        "numeric.decrement","numeric.scale_power_two","numeric.max_min","numeric.in_range",
        "numeric.multiply_accumulate","numeric.random_range"}))) {
    const auto operand = [&](std::string_view name) {
        if (wired(name)) return input(name);
        const auto *p = findFunctionBindingPort(definition,name);
        auto hint = known(p ? parseType(p->defaultValueType).type : TypeRef{});
        hint.complete = false;
        return hint;
    };
    const auto width = [](const TypeRef &leaf) -> unsigned {
        if (named(leaf.name,{"i8","u8"})) return 8;
        if (named(leaf.name,{"i16","u16"})) return 16;
        if (named(leaf.name,{"i32","u32","f32"})) return 32;
        if (named(leaf.name,{"i64","u64","f64"})) return 64;
        return 0;
    };
    const auto realLeaf = [](const TypeRef &leaf) { return realNumber(leaf) ? leaf : TypeRef{}; };
    const auto integerLeaf = [&](const TypeRef &leaf) { return leaf.integer() && width(leaf) ? leaf : TypeRef{}; };
    // Common shape/type comes from connected operands only. Unwired DBL hints
    // must not silently promote an I16 input to DBL.
    const auto combine = [&](std::initializer_list<std::string_view> names, const LeafMap &map, bool lift=true) {
        TypeResolution common;
        bool complete = true;
        for (const auto name : names) {
            complete = complete && wired(name);
            if (!wired(name)) continue;
            auto source = operand(name);
            if (!source.resolution.accepted()) return source;
            auto t = mapLeaves(source.resolution.type,map);
            if (!t.known()) return invalid("Unsupported numeric operand representation.");
            if (!common.type.known()) common = {ResolutionStatus::Compatible,t,{},{}};
            else common = commonType(common.type,t,lift);
            if (!common.accepted()) return BindingResult{common,false,{}};
        }
        if (!common.type.known()) {
            auto t = mapLeaves(operand(*names.begin()).resolution.type,map);
            return BindingResult{{ResolutionStatus::Compatible,t,{},{}},false,{}};
        }
        return BindingResult{common,complete,{}};
    };
    const auto project = [&](BindingResult value) {
        if (!value.resolution.accepted()) return value;
        if (declared->direction == FunctionPortDirection::Input && wired(port)) return input(port);
        return value;
    };
    if (named(id,{"numeric.data_manipulation.adapt_to_type","numeric.data_manipulation.unflatten_from_byte_array",
            "numeric.data_manipulation.flatten_to_byte_array"})) {
        const bool unflatten = id.ends_with("unflatten_from_byte_array");
        const bool flatten = id.ends_with(".flatten_to_byte_array");
        for (const auto &name : wiredInputs) {
            if (name != "anything" && name != "type" && name != "x") continue;
            const auto serial = mapLeaves(input(name).resolution.type, [](const TypeRef &leaf) {
                return leaf.name == "refnum" ? TypeRef{} : leaf;
            });
            if (!serial.known()) return invalid("Opaque references cannot be flattened or reinterpreted.");
        }
        if (declared->direction == FunctionPortDirection::Input) return operand(port);
        if (port == "error_out" || port == "data_bytes" || port == "remainder")
            return known(parseType(declared->defaultValueType).type);
        if (!flatten || unflatten) return operand("type");
    }
    if (id == "numeric.data_manipulation.reinterpret_number") {
        auto source=operand("data_in"), target=operand("type");
        const auto &s=source.resolution.type;
        const auto &leaf=s.kind==TypeKind::Array ? s.children.front() : s;
        if ((s.kind==TypeKind::Array && s.rank!=1) || !width(leaf) ||
            !width(target.resolution.type) || width(leaf)!=width(target.resolution.type))
            return invalid("Reinterpret Number requires equal-width numeric scalar types (or a 1D source array).");
        if (declared->direction==FunctionPortDirection::Input) return operand(port);
        return result(source,s.kind==TypeKind::Array ? TypeRef::array(target.resolution.type) : target.resolution.type);
    }
    if (id.starts_with("numeric.scaling.")) {
        auto value=operand("voltage");
        const auto &t=value.resolution.type;
        if (!(realNumber(t) || (t.kind==TypeKind::Array && t.rank==1 && realNumber(t.children.front()))))
            return invalid("Sensor scaling accepts real scalar or 1D voltage data; waveform overloads are not available.");
        // Preserve actual voltage representation; only the measurement output is F64.
        if (declared->direction==FunctionPortDirection::Input) return operand(port);
        return result(value,mapLeaves(t,[](const TypeRef &) { return TypeRef::primitive("f64"); }));
    }
    if (id.starts_with("numeric.complex.")) {
        const bool fromComplex=named(id,{"numeric.complex.conjugate","numeric.complex.complex_to_polar","numeric.complex.complex_to_real_imaginary"});
        const auto complexLeaf=[](const TypeRef &leaf) { return leaf.numeric() && named(leaf.name,{"c32","c64","f32","f64"}) ? leaf : TypeRef{}; };
        auto value=fromComplex ? combine({"z"},complexLeaf) :
            (id.find("polar_to_")!=std::string_view::npos ? combine({"r","theta"},realLeaf) : combine({"re","im"},realLeaf));
        if (!value.resolution.accepted()) return value;
        if (declared->direction==FunctionPortDirection::Input) return project(value);
        const bool toComplex=named(id,{"numeric.complex.conjugate","numeric.complex.polar_to_complex","numeric.complex.real_imaginary_to_complex"});
        return result(value,mapLeaves(value.resolution.type,[&](const TypeRef &leaf) {
            const bool single=leaf.name=="f32" || leaf.name=="c32";
            return TypeRef::primitive(toComplex ? (single?"c32":"c64") : (single?"f32":"f64"));
        }));
    }
    if (named(id,{"numeric.data_manipulation.rotate_left_with_carry","numeric.data_manipulation.rotate_right_with_carry"})) {
        auto value=operand("value_in");
        if (!integerLeaf(value.resolution.type).known()) return invalid("Rotate with carry requires an 8/16/32/64-bit integer scalar.");
        if (port=="carry" || port=="carry_out") return known(TypeRef::primitive("bool"));
        return value;
    }
    if (named(id,{"numeric.data_manipulation.logical_shift","numeric.data_manipulation.rotate"})) {
        auto value=operand("x"), count=operand("y");
        if (!integerLeaf(value.resolution.type).known() || !realNumber(count.resolution.type))
            return invalid("Shift/rotate requires an integer scalar and a real scalar bit count.");
        return port=="y" ? count : value;
    }
    if (named(id,{"numeric.data_manipulation.split_number","numeric.data_manipulation.join_numbers",
            "numeric.data_manipulation.swap_bytes","numeric.data_manipulation.swap_words"})) {
        const bool join=id.ends_with("join_numbers"), split=id.ends_with("split_number");
        auto value=join ? combine({"hi","lo"},integerLeaf) : combine({"x"},integerLeaf);
        if (!value.resolution.accepted()) return value;
        const auto output=mapLeaves(value.resolution.type,[&](const TypeRef &leaf) {
            const unsigned bits=width(leaf);
            if (join && bits>32) return TypeRef{};
            return join || split ? TypeRef::primitive("u"+std::to_string(join?bits*2:(std::max)(8U,bits/2))) : leaf;
        });
        if (!output.known()) return invalid("Join Numbers accepts 8/16/32-bit halves only.");
        return declared->direction==FunctionPortDirection::Input ? project(value) : result(value,output);
    }
    if (id=="numeric.data_manipulation.mantissa_exponent") {
        auto value=combine({"number"},realLeaf);
        if (!value.resolution.accepted()) return value;
        // Result promotion must not retype its actual integer input. The
        // operation performs this numeric conversion inside its own kernel.
        if (declared->direction==FunctionPortDirection::Input) return operand(port);
        return result(value,mapLeaves(value.resolution.type,[](const TypeRef &leaf) {
            return leaf.integer()?TypeRef::primitive("f64"):leaf;
        }));
    }
    if (id=="numeric.scale_power_two") {
        auto value=combine({"x","n"},realLeaf);
        if (!value.resolution.accepted()) return value;
        if (port=="n") return operand(port);
        // The exponent affects shape, not the numeric representation of x.
        const std::function<TypeRef(const TypeRef &,const TypeRef &)> preserveX =
            [&](const TypeRef &x,const TypeRef &shape) -> TypeRef {
                if (x.kind==TypeKind::Primitive) return mapLeaves(shape,[&](const TypeRef &) {return x;});
                TypeRef output=shape;
                if (shape.kind==TypeKind::Array) {
                    output.children[0]=preserveX(x.kind==TypeKind::Array?x.children[0]:x,shape.children[0]);
                } else if (shape.kind==TypeKind::Cluster && x.kind==TypeKind::Cluster) {
                    for (std::size_t i=0;i<output.children.size();++i)
                        output.children[i]=preserveX(x.children[i],shape.children[i]);
                }
                return output;
            };
        return result(value,preserveX(operand("x").resolution.type,value.resolution.type));
    }
    if (id=="numeric.multiply_accumulate" || id=="numeric.random_range") {
        auto value=id=="numeric.random_range" ? combine({"lower_limit","upper_limit"},realLeaf,false) :
            combine({"x","y","initial_value"},realLeaf,false);
        if (!value.resolution.accepted()) return value;
        if (value.resolution.type.kind!=TypeKind::Primitive) return invalid("This operation accepts real scalar operands only.");
        if (port=="initialize") return operand(port);
        return project(value);
    }
    auto value=named(id,{"numeric.increment","numeric.decrement"}) ?
        combine({"in"},[](const TypeRef &leaf) { return leaf.numeric()?leaf:TypeRef{}; }) :
        (id=="numeric.in_range" ? combine({"upper_limit","x","lower_limit"},realLeaf) : combine({"x","y"},realLeaf));
    if (!value.resolution.accepted()) return value;
    if (port=="in_range") return result(value,mapLeaves(value.resolution.type,[](const TypeRef &) {return TypeRef::primitive("bool");}));
    return project(value);
}
