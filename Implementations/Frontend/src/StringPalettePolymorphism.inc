// Included in the shared candidate/live binding resolver. No runtime evaluation.
    if(id=="string.empty_string_or_path" || id=="string.lexical_class") {
        auto value=input("value"); if(!value.resolution.accepted()) return value;
        const bool lexical=id=="string.lexical_class";
        const auto converted=mapLeaves(value.resolution.type,[&](const TypeRef &t)->TypeRef {
            const bool accepted=lexical?(t.name=="string" || realNumber(t)):named(t.name,{"string","path","variant"});
            return accepted?TypeRef::primitive(lexical?"i16":"bool"):TypeRef{};
        });
        if(!converted.known()) return invalid("Unsupported character or emptiness-test schema.");
        return declared->direction==FunctionPortDirection::Input?value:result(value,converted);
    }
    if(id=="string.conversion.flatten_to_json" || id=="string.conversion.unflatten_from_json") {
        const bool decode=id.ends_with("unflatten_from_json");
        if(declared->direction==FunctionPortDirection::Input && port!=(decode?"type":"value")) return input(port);
        if(port=="error_out") return known(parseType(declared->defaultValueType).type);
        // The JSON text output has exactly one type even before a value schema
        // is supplied. Do not invent the arbitrary input/decoded schema, nor
        // bypass validation when an actual (possibly invalid) value is wired.
        if(!decode && declared->direction==FunctionPortDirection::Output && !wired("value")) {
            auto value=known(TypeRef::primitive("string")); value.complete=false; return value;
        }
        auto value=input(decode?"type":"value");
        if(!value.resolution.accepted()) return value;
        const auto validJson=[&](auto &&self,const TypeRef &t)->bool {
            if(t.kind==TypeKind::Array) return t.rank>0 && t.children.size()==1 && t.children[0].kind!=TypeKind::Array && self(self,t.children[0]);
            if(t.kind==TypeKind::Cluster) {
                bool namedField=false,unnamedField=false;
                for(std::size_t i=0;i<t.fieldIds.size();++i) {
                    namedField|=!t.fieldIds[i].empty(); unnamedField|=t.fieldIds[i].empty();
                    if(!t.fieldIds[i].empty() && std::find(t.fieldIds.begin(),t.fieldIds.begin()+i,t.fieldIds[i])!=t.fieldIds.begin()+i) return false;
                }
                return !(namedField && unnamedField) && std::all_of(t.children.begin(),t.children.end(),[&](const auto &child){return self(self,child);});
            }
            return named(t.name,{"bool","string"}) || (realNumber(t) && t.name!="fxp");
        };
        if(!validJson(validJson,value.resolution.type)) return invalid("JSON supports Boolean, real non-FXP numeric and String schemas with consistently unique cluster names.");
        if(declared->direction==FunctionPortDirection::Input) return input(port);
        return decode && port=="result"?value:known(parseType(declared->defaultValueType).type);
    }
    if(named(id,{"string.to_upper","string.to_lower","string.rotate","string.reverse"})) {
        auto value=input("value"); if(!value.resolution.accepted()) return value;
        const bool ascii=id=="string.to_upper" || id=="string.to_lower";
        const auto converted=mapLeaves(value.resolution.type,[&](const TypeRef &t)->TypeRef {
            return t.name=="string" || (ascii && realNumber(t))?t:TypeRef{};
        });
        if(!converted.known()) return invalid("This character operation requires String leaves or, for case conversion, real ASCII-code values.");
        return result(value,converted);
    }
    if(id=="string.conversion.format_value") {
        if(formatLiteral) {
            const auto fields=stringFormatFields(*formatLiteral);
            if(!fields)return invalid("Unsupported or incomplete Format Value specifier.");
            for(const auto &field:*fields)if(field.argument!=0 || field.code=='T' || field.code=='t' || field.code=='[')
                return invalid("Format Value accepts one numeric argument and excludes time and scan-set conversion codes.");
        }
        if(declared->direction==FunctionPortDirection::Input && port!="value") return input(port);
        auto value=input("value"); if(!value.resolution.accepted()) return value;
        if(!value.resolution.type.numeric()) return invalid("Format Value requires one scalar numeric value.");
        return declared->direction==FunctionPortDirection::Input ? (port=="value"?value:input(port)) : known(TypeRef::primitive("string"));
    }
    if (id.starts_with("string.conversion.") && (id.ends_with("_string_to_number") || id=="string.conversion.scan_value")) {
        if(port!="value" && port!="result") return declared->direction==FunctionPortDirection::Input?input(port):known(parseType(declared->defaultValueType).type);
        auto value=input("value"), target=input("default_value");
        if(!value.resolution.accepted()) return value;
        if(!target.resolution.accepted()) return target;
        if(!target.resolution.type.numeric()) return invalid("The default representation must be a scalar numeric type.");
        auto converted=mapLeaves(value.resolution.type,[&](const TypeRef &t)->TypeRef { return t.name=="string"?target.resolution.type:TypeRef{}; });
        if(!converted.known()) return invalid("String-to-number conversion requires String leaves.");
        if(declared->direction==FunctionPortDirection::Input)
            return port=="value"?value:port=="default_value"?target:known(parseType(declared->defaultValueType).type);
        return port=="result"?result(value,std::move(converted)):known(parseType(declared->defaultValueType).type);
    }
    if (id.starts_with("string.conversion.number_to_") && id.ends_with("_string")) {
        if(declared->direction==FunctionPortDirection::Input && port!="value") return input(port);
        auto value=input("value"); if(!value.resolution.accepted()) return value;
        auto converted=mapLeaves(value.resolution.type,[](const TypeRef &t)->TypeRef { return t.numeric()?TypeRef::primitive("string"):TypeRef{}; });
        if(!converted.known()) return invalid("Number-to-String conversion requires numeric leaves.");
        if(declared->direction==FunctionPortDirection::Input)
            return port=="value"?value:known(parseType(declared->defaultValueType).type);
        return result(value,std::move(converted));
    }
    if (isStringFormat(id)) {
        const auto fields=formatLiteral?stringFormatFields(*formatLiteral):std::optional<std::vector<StringFormatField>>{};
        if(formatLiteral && !fields) return invalid("Unsupported or incomplete format specifier.");
        const auto count=fields?stringFormatArgumentCount(*fields):std::clamp(node.stringFunctionTerminals,1U,32U);
        for(const auto &name:wiredInputs) if(const auto index=stringPaletteIndex(id,name)) {
            if(*index>=count) return invalid("This formatting parameter has not been exposed.");
            auto value=input(name); if(!value.resolution.accepted()) return value;
            const auto &t=value.resolution.type;
            if(!(t.numeric() || t.kind==TypeKind::Enum || named(t.name,{"string","path","bool","timestamp"})))
                return invalid("Formatting parameters require supported scalar values, not arrays or clusters.");
            if(fields) for(const auto &f:*fields) if(f.argument==*index) {
                const bool text=f.code=='s', time=f.code=='T' || f.code=='t';
                if(f.code=='[' || (t.name=="timestamp" && !time) ||
                    (time && !((f.code=='T' && t.name=="timestamp") || realNumber(t))) ||
                    (!time && !text && !(t.numeric() || t.kind==TypeKind::Enum || t.name=="bool")) ||
                    (text && !(named(t.name,{"string","path","bool"}) || t.kind==TypeKind::Enum)))
                    return invalid("The connected scalar does not match its constant format specifier.");
            }
        }
        if(const auto index=stringPaletteIndex(id,port)) {
            if(*index>=count) return invalid("This formatting parameter has not been exposed.");
            if(!wired(port) && fields) for(const auto &field:*fields) if(field.argument==*index) {
                // Match Create Constant to the same format contract used for
                // candidate validation (%s must not offer a DBL constant).
                auto value=known(TypeRef::primitive(field.code=='s'?"string":field.code=='T'?"timestamp":"f64"));
                value.complete=false; return value;
            }
        }
        return declared->direction==FunctionPortDirection::Input ? input(port) : known(parseType(declared->defaultValueType).type);
    }
    if (isStringScan(id)) {
        const auto fields=formatLiteral?stringFormatFields(*formatLiteral):std::optional<std::vector<StringFormatField>>{};
        if(formatLiteral && !fields) return invalid("Unsupported or incomplete scan format specifier.");
        const auto count=fields?stringFormatArgumentCount(*fields):std::clamp(node.stringFunctionTerminals,1U,32U);
        const auto target=[&](unsigned index) {
            const auto key=index?"default_value_"+std::to_string(index+1):"default_value";
            if(wired(key)) return input(key);
            if(fields) for(const auto &f:*fields) if(f.argument==index)
                return known(TypeRef::primitive(f.code=='s' || f.code=='['?"string":f.code=='T'?"timestamp":"f64"));
            return known(TypeRef::primitive("f64"));
        };
        for(const auto &name:wiredInputs) if(const auto index=stringPaletteIndex(id,name)) {
            if(*index>=count) return invalid("This scan parameter has not been exposed.");
            auto v=input(name); if(!v.resolution.accepted()) return v;
            const auto &t=v.resolution.type;
            if(!(t.numeric() || t.kind==TypeKind::Enum || named(t.name,{"bool","string","path","timestamp"})))
                return invalid("Scan parameters require supported scalar defaults, not arrays or clusters.");
            if(fields) for(const auto &f:*fields) if(f.argument==*index) {
                const bool text=f.code=='s' || f.code=='[', time=f.code=='T' || f.code=='t';
                if((text && !(named(t.name,{"string","path","bool"}) || t.kind==TypeKind::Enum)) ||
                    (time && !((f.code=='T' && t.name=="timestamp") || realNumber(t))) ||
                    (!text && !time && !(t.numeric() || t.kind==TypeKind::Enum || t.name=="bool")))
                    return invalid("The connected default does not match its constant scan format specifier.");
            }
        }
        if(const auto index=stringPaletteIndex(id,port)) {
            if(*index>=count) return invalid("This scan parameter has not been exposed.");
            return target(*index);
        }
        return declared->direction==FunctionPortDirection::Input ? input(port) : known(parseType(declared->defaultValueType).type);
    }
    if (id=="string.match_regular_expression") {
        if(const auto index=stringPaletteIndex(id,port); index && *index>=std::clamp(node.stringFunctionTerminals,1U,32U))
            return invalid("This submatch output has not been exposed.");
        return declared->direction==FunctionPortDirection::Input ? input(port) : known(parseType(declared->defaultValueType).type);
    }
    if (id == "string.length") {
        auto value=input("value");
        if (!value.resolution.accepted()) return value;
        auto output=mapLeaves(value.resolution.type, [](const TypeRef &leaf) -> TypeRef {
            return leaf.name=="string" ? TypeRef::primitive("i32") : TypeRef{};
        });
        if (!output.known()) return invalid("String Length requires String leaves.");
        return declared->direction==FunctionPortDirection::Input ? value : result(value,std::move(output));
    }
