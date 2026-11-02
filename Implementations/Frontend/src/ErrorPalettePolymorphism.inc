// NXG Error palette authoring. Diagnostics remain typed values, not exceptions.
if(id=="error.filter_code") {
    auto codes=input("codes");
    if(codes.resolution.status==ResolutionStatus::Incompatible) return codes;
    const auto &type=codes.resolution.type;
    const bool array=type.kind==TypeKind::Array;
    if(type.known() && !(type==TypeRef::primitive("i32") ||
        (array && type.rank==1 && type.children.size()==1 && type.children.front()==TypeRef::primitive("i32"))))
        return invalid("Filter Error Code requires one I32 code or a one-dimensional I32 array.");
    if(port=="codes") return codes;
    if(port=="codes_cleared") {
        if(!array) return invalid("Codes cleared exists only for the array overload of Filter Error Code.");
        return known(TypeRef::array(TypeRef::primitive("i32")));
    }
    if(port=="error_cleared") return known(TypeRef::primitive("bool"));
    return known(parseType(kErrorClusterType).type);
}
if(id=="error.retain_first") {
    // Private binding limits constrain source ports; an editor rail height
    // never removes a connected operand from semantic type resolution.
    const auto count=definition.maximumInputCount;
    for(const auto &name:wiredInputs) {
        const auto index=errorRetainIndex(name);
        if(!index || *index>=count) return invalid("Retain First Error wire references a removed input.");
        auto value=input(name);
        if(value.resolution.accepted() && !isErrorCluster(value.resolution.type))
            return invalid("Retain First Error accepts scalar error clusters, not arrays or unrelated clusters.");
    }
    if(port!="error_out") {
        const auto index=errorRetainIndex(port);
        if(!index || *index>=count) return invalid("Retain First Error input does not exist.");
    }
    return known(parseType(kErrorClusterType).type);
}
