// One closed binary schema policy for String and U8-array transports.
if(isBinarySerializationFunction(id)) {
    const bool decode=id.find("unflatten_from_")!=std::string_view::npos;
    const auto payloadPort=decode?"type":"anything";
    for(const auto &name:wiredInputs) {
        const auto value=input(name);
        if(!value.resolution.accepted())return value;
        const auto &type=value.resolution.type;
        if(name==payloadPort && !binarySerializationTypeSupported(type))
            return invalid("Binary serialization requires Boolean, integer, SGL/DBL, String, enum, Array or Cluster data; references and opaque types are unsupported.");
        if(name=="byte_order" && !binarySerializationByteOrderType(type))
            return invalid("Byte order requires the byte_order enum or a legacy integer (0 big-endian, 1 native, 2 little-endian).");
    }
    const auto operand=[&](std::string_view name) {
        if(wired(name))return input(name);
        const auto *p=findFunctionBindingPort(definition,name);
        auto hint=known(p?parseType(p->defaultValueType).type:TypeRef{});
        hint.complete=false;
        return hint;
    };
    if(declared->direction==FunctionPortDirection::Input)return operand(port);
    if(decode && port=="value")return operand("type");
    return known(parseType(declared->defaultValueType).type);
}
