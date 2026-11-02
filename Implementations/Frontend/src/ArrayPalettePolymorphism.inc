// Remaining NI Array palette authoring contracts. Included inside the shared
// resolver so the same rules serve preview, candidate wires and live endpoints.
    if (const auto kind=arrayPaletteKind(id); kind!=ArrayPaletteKind::None || id.starts_with("array.matrix.")) {
        const bool dynamic=kind!=ArrayPaletteKind::None;
        const auto requested=dynamic ? arrayPaletteSemanticPort(node,port):std::optional<ArrayPalettePort>{};
        if(dynamic && !requested) return invalid("This Array terminal does not exist for the configured count.");
        for(const auto &name:wiredInputs)
            if(dynamic && !arrayPaletteSemanticPort(node,name)) return invalid("A wire references a removed Array terminal.");
        const auto expectedNumber=[&](std::string_view name,bool vector=false) -> BindingResult {
            auto source=input(name); const auto &t=source.resolution.type;
            if(source.resolution.accepted() && !realNumber(t) &&
                !(vector && t.kind==TypeKind::Array && t.rank==1 && realNumber(t.children.front())))
                return invalid("Indices and sizes require real numeric scalars (or 1D numeric indices where documented).");
            auto expected=known(vector && t.kind==TypeKind::Array ? TypeRef::array(TypeRef::primitive("i32")):TypeRef::primitive("i32"));
            expected.complete=!wired(name) || source.complete; return expected;
        };
        if(kind==ArrayPaletteKind::Initialize) {
            bool complete=true;
            for(const auto &p:arrayPaletteSemanticPorts(node)) if(p.base=="size") {
                auto size=expectedNumber(p.id); if(!size.resolution.accepted()) return size;
                complete=complete && wired(p.id) && input(p.id).complete;
            }
            auto element=input("element");
            if(element.resolution.accepted() && element.resolution.type.kind==TypeKind::Array)
                return invalid("Initialize Array requires a scalar or cluster, not a direct array element.");
            if(requested->base=="size") return expectedNumber(port);
            if(!element.resolution.accepted() || port=="element") return element;
            auto value=result(element,TypeRef::array(element.resolution.type,arrayPaletteCount(node)));
            value.complete=value.complete && complete; return value;
        }
        if(kind==ArrayPaletteKind::Interleave || kind==ArrayPaletteKind::BuildMatrix) {
            TypeResolution common; bool hasType=false,complete=true;
            for(const auto &p:arrayPaletteSemanticPorts(node)) if(!p.output) {
                if(!wired(p.id)) { complete=false; continue; }
                auto value=input(p.id); complete=complete && value.complete && value.resolution.accepted();
                if(!value.resolution.accepted()) continue;
                const auto &t=value.resolution.type;
                if(kind==ArrayPaletteKind::Interleave && (t.kind!=TypeKind::Array || t.rank!=1))
                    return invalid("Interleave requires only one-dimensional arrays with compatible element types.");
                const auto &leaf=t.kind==TypeKind::Array ? t.children.front():t;
                if(kind==ArrayPaletteKind::BuildMatrix && (!leaf.numeric() || (t.kind==TypeKind::Array && t.rank>2)))
                    return invalid("Build Matrix accepts numeric scalars and numeric arrays of rank one or two.");
                common=hasType ? commonType(common.type,leaf):TypeResolution{ResolutionStatus::Compatible,leaf,{},{}};
                if(!common.accepted()) return {common,false,{}};
                hasType=true;
            }
            if(!hasType) {
                if(!wiredInputs.empty()) return unresolved();
                auto suggestion=input(kind==ArrayPaletteKind::Interleave ? "array_a":"element1");
                if(!suggestion.resolution.accepted() || !requested->output) return suggestion;
                const auto &type=suggestion.resolution.type;
                return result(suggestion,TypeRef::array(type.kind==TypeKind::Array ? type.children.front():type,
                    kind==ArrayPaletteKind::Interleave ? 1:2));
            }
            if(!requested->output) {
                auto t=input(port).resolution.type;
                const auto rank=kind==ArrayPaletteKind::Interleave ? 1U:t.kind==TypeKind::Array ? t.rank:0U;
                return known(rank ? TypeRef::array(common.type,rank):common.type);
            }
            return {TypeResolution{ResolutionStatus::Compatible,TypeRef::array(common.type,kind==ArrayPaletteKind::Interleave?1:2),{},{}},complete,{}};
        }
        if(kind==ArrayPaletteKind::Decimate) {
            auto value=input("array");
            if(value.resolution.accepted() && (value.resolution.type.kind!=TypeKind::Array || value.resolution.type.rank!=1))
                return invalid("Decimate 1D Array requires a one-dimensional array.");
            return value;
        }
        // Studio represents the NI numeric-matrix overload using its numeric
        // 2D array schema. Values/lengths are deliberately not evaluated here.
        auto matrix=input("matrix"); auto t=matrix.resolution.type;
        if(matrix.resolution.accepted() && (t.kind!=TypeKind::Array || t.rank!=2 || !t.children.front().numeric()))
            return invalid("Matrix functions require a two-dimensional numeric array.");
        const auto isValue=[](std::string_view base) { return base=="elements" || base=="diagonal_values" || base=="subset"; };
        for(const auto &name:wiredInputs) {
            if(name=="matrix") continue;
            const auto split=arrayPaletteSplitPort(name); if(!split) return invalid("Malformed matrix port.");
            if(isValue(split->first)) {
                auto replacement=input(name); const auto &v=replacement.resolution.type;
                if(replacement.resolution.accepted() && !v.numeric() &&
                    !(v.kind==TypeKind::Array && v.rank==2 && v.children.front().numeric()))
                    return invalid("A matrix replacement must be a numeric scalar or two-dimensional numeric array.");
                if(replacement.resolution.accepted() && matrix.resolution.accepted()) {
                    const auto promoted=commonType(t.children.front(),v.kind==TypeKind::Array?v.children.front():v);
                    if(!promoted.accepted()) return {promoted,false,{}};
                    t.children.front()=promoted.type;
                }
            } else {
                auto check=expectedNumber(name,split->first=="row_indices" || split->first=="column_indices");
                if(!check.resolution.accepted()) return check;
            }
        }
        const auto base=requested ? requested->base:port;
        matrix.resolution.type=t;
        if(declared->direction==FunctionPortDirection::Input) {
            if(port=="matrix") return matrix;
            if(!isValue(base)) return expectedNumber(port,base=="row_indices" || base=="column_indices");
            if(!matrix.resolution.accepted()) return matrix;
            auto replacement=input(port).resolution.type;
            return known(replacement.kind==TypeKind::Array ? t:t.children.front());
        }
        if(id=="array.matrix.size") return known(TypeRef::primitive("i32"));
        if(!matrix.resolution.accepted()) return matrix;
        if(kind==ArrayPaletteKind::GetElements) {
            const auto row=arrayPaletteGroupId("row_indices",requested->group), col=arrayPaletteGroupId("column_indices",requested->group);
            const bool scalar=wired(row) && wired(col) && realNumber(input(row).resolution.type) && realNumber(input(col).resolution.type);
            return result(matrix,scalar ? t.children.front():t);
        }
        return matrix;
    }
    if(named(id,{"array.max_min","array.sort_1d","array.search_1d","array.split_1d","array.reverse_1d",
        "array.rotate_1d","array.interpolate_1d","array.threshold_1d","array.transpose_2d"})) {
        const bool interpolation=id=="array.interpolate_1d" || id=="array.threshold_1d";
        for(const auto &name:wiredInputs) {
            if(name=="array" || name=="value") continue;
            auto v=input(name);
            if(v.resolution.accepted() && !realNumber(v.resolution.type))
                return invalid("This index, offset or threshold requires a real numeric scalar.");
        }
        auto array=input("array");
        auto t=id=="array.max_min" ? indexArrayDataType(array.resolution.type):array.resolution.type;
        if(array.resolution.accepted() && (t.kind!=TypeKind::Array ||
            (id!="array.max_min" && t.rank!=(id=="array.transpose_2d"?2:1))))
            return invalid("The connected array has an unsupported rank for this function.");
        if(id=="array.search_1d") {
            auto query=input("value");
            if(query.resolution.accepted() && query.resolution.type.kind==TypeKind::Array)
                return invalid("Search requires an element, not an array.");
            TypeResolution common;
            if(array.resolution.accepted() && query.resolution.accepted()) common=commonType(t.children.front(),query.resolution.type);
            else if(array.resolution.accepted()) common={ResolutionStatus::Compatible,t.children.front(),{},{}};
            else if(query.resolution.accepted()) common=query.resolution;
            if(common.status==ResolutionStatus::Incompatible) return {common,false,{}};
            if(port=="index" || port=="start_index") return known(TypeRef::primitive("i32"));
            if(!common.accepted()) return unresolved();
            return known(port=="array"?TypeRef::array(common.type):common.type);
        }
        if(interpolation && array.resolution.accepted()) {
            const auto &leaf=t.children.front();
            const bool point=leaf.kind==TypeKind::Cluster && leaf.children.size()==2 &&
                std::all_of(leaf.children.begin(),leaf.children.end(),realNumber);
            if(!realNumber(leaf) && !point) return invalid("Interpolation accepts 1D real numeric arrays or 1D arrays of two-real-number point clusters.");
            auto element=leaf;
            if(point) element.children.assign(2,TypeRef::primitive("f64")); else element=TypeRef::primitive("f64");
            if(port=="array") return known(TypeRef::array(element));
        }
        if(declared->direction==FunctionPortDirection::Input && port!="array")
            return known(TypeRef::primitive(interpolation && port!="start_index"?"f64":"i32"));
        if(interpolation && declared->direction==FunctionPortDirection::Output) return known(TypeRef::primitive("f64"));
        if(!array.resolution.accepted()) return array;
        if(id=="array.max_min" && declared->direction==FunctionPortDirection::Output)
            return result(array,port.ends_with("_index") ? (t.rank==1?TypeRef::primitive("i32"):TypeRef::array(TypeRef::primitive("i32"))):t.children.front());
        return array;
    }
    if(id.starts_with("array.conversion.") || named(id,{"cluster.conversion.array_to_cluster","cluster.conversion.cluster_to_array"})) {
        if(id.ends_with("array_to_cluster")) {
            auto array=input("array"); if(!array.resolution.accepted()) return array;
            const auto &t=array.resolution.type;
            if(t.kind!=TypeKind::Array || t.rank!=1) return invalid("Array To Cluster requires a one-dimensional array.");
            unsigned count=9;
            if(!node.text.empty()) {
                const auto parsed=std::from_chars(node.text.data(),node.text.data()+node.text.size(),count);
                if(parsed.ec!=std::errc{} || parsed.ptr!=node.text.data()+node.text.size() || count<1 || count>256)
                    return invalid("Cluster size must be between 1 and 256.");
            }
            if(port=="array") return array;
            std::vector<std::string> names; std::vector<TypeRef> fields(count,t.children.front());
            for(unsigned i=1;i<=count;++i) names.push_back("element"+std::to_string(i));
            // Generated Cluster fields have the same default display names as
            // a real indicator created from this output. Retain those names in
            // the complete Source/FIR schema, alongside their stable IDs.
            auto cluster=TypeRef::cluster(names,std::move(fields));
            cluster.fieldNames=std::move(names);
            return result(array,std::move(cluster));
        }
        if(id.ends_with("cluster_to_array")) {
            auto cluster=input("cluster"); if(!cluster.resolution.accepted()) return cluster;
            const auto &t=cluster.resolution.type;
            if(t.kind!=TypeKind::Cluster || t.children.empty()) return invalid("Cluster To Array requires a non-empty homogeneous cluster.");
            TypeResolution common{ResolutionStatus::Compatible,t.children.front(),{},{}};
            for(const auto &child:t.children) {
                if(child.kind==TypeKind::Array) return invalid("Cluster To Array does not accept direct array fields.");
                common=commonType(common.type,child); if(!common.accepted()) return {common,false,{}};
            }
            return port=="cluster" ? cluster:result(cluster,TypeRef::array(common.type));
        }
        const bool fromText=id.ends_with("spreadsheet_string_to_array");
        const bool toText=id.ends_with("array_to_spreadsheet_string");
        if(fromText || toText) {
            if(formatLiteral) {
                const auto fields=stringFormatFields(*formatLiteral);
                if(!fields)return invalid("Unsupported or incomplete Spreadsheet common format specifier.");
                for(const auto &field:*fields)if(field.code=='T' || field.code=='t')
                    return invalid("NI time conversion codes T and t are unavailable in Spreadsheet common formats.");
            }
            for(const auto &name:wiredInputs) {
                auto v=input(name); if(!v.resolution.accepted()) continue;
                const auto &t=v.resolution.type;
                if(name=="array" || name=="array_type") {
                    if(t.kind!=TypeKind::Array || !(t.children.front().numeric() || t.children.front()==TypeRef::primitive("string")))
                        return invalid("Spreadsheet arrays require numeric or String elements.");
                } else if(t!=TypeRef::primitive("string") && !(fromText && name=="delimiter" && t==TypeRef::array(TypeRef::primitive("string"))))
                    return invalid("Spreadsheet text inputs require a String; parsing delimiters also accept a 1D String array.");
            }
            if(port=="array_type" || (fromText && port=="array")) {
                return wired("array_type") ? input("array_type"):known(TypeRef::array(TypeRef::primitive("f64"),2));
            }
            if(port=="array") return input("array");
            if(fromText && port=="delimiter" && input(port).resolution.type.kind==TypeKind::Array) return input(port);
            if(declared->direction==FunctionPortDirection::Input) {
                auto source=input(port);
                if(wired(port) && !source.resolution.accepted()) return source;
                auto value=known(TypeRef::primitive("string"));
                value.complete=wired(port)?source.complete:declared->requirement==FunctionPortRequirement::Optional;
                return value;
            }
            return known(TypeRef::primitive("string"));
        }
        if(id.ends_with("number_to_boolean_array")) {
            auto value=input("number");
            if(value.resolution.accepted() && !value.resolution.type.integer() && value.resolution.type.name!="fxp") return invalid("Number To Boolean Array requires an integer or real fixed-point scalar.");
            return port=="number" ? value:known(TypeRef::array(TypeRef::primitive("bool")));
        }
        if(id.ends_with("boolean_array_to_number")) {
            auto value=input("array");
            if(value.resolution.accepted() && value.resolution.type!=TypeRef::array(TypeRef::primitive("bool")))
                return invalid("Boolean Array To Number requires a one-dimensional Boolean array.");
            auto output=parseType(node.valueType).type;
            if(!output.integer() && output.name!="fxp") output=TypeRef::primitive("u32");
            auto resolved=known(port=="array"?TypeRef::array(TypeRef::primitive("bool")):output);
            resolved.complete=value.complete && value.resolution.accepted();
            return resolved;
        }
        if(id.ends_with("string_to_byte_array") || id.ends_with("byte_array_to_string")) {
            auto source=input(id.ends_with("string_to_byte_array")?"string":"array");
            const auto expected=id.ends_with("string_to_byte_array")?TypeRef::primitive("string"):TypeRef::array(TypeRef::primitive("u8"));
            if(source.resolution.accepted() && source.resolution.type!=expected)
                return invalid("Byte/String conversions require a scalar String or a one-dimensional U8 array.");
            return known(port=="string"?TypeRef::primitive("string"):TypeRef::array(TypeRef::primitive("u8")));
        }
    }
