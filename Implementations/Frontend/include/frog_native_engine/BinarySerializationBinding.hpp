#pragma once
#include "frog_native_engine/BindingType.hpp"

namespace frog::native_engine {
inline bool isBinarySerializationFunction(std::string_view id) {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return id=="numeric.data_manipulation.flatten_to_byte_array" ||
        id=="numeric.data_manipulation.unflatten_from_byte_array" ||
        id=="string.conversion.flatten_to_string" || id=="string.conversion.unflatten_from_string";
}
inline bool binarySerializationInteger(const binding::TypeRef &type) {
    if(type.kind!=binding::TypeKind::Primitive)return false;
    for(const auto name:{"i8","u8","i16","u16","i32","u32","i64","u64"})if(type.name==name)return true;
    return false;
}
inline bool binarySerializationTypeSupported(const binding::TypeRef &type,unsigned depth=0) {
    using binding::TypeKind;
    if(depth>32)return false;
    if(type.kind==TypeKind::Array)return type.rank>0 && type.rank<=16 && type.children.size()==1 &&
        binarySerializationTypeSupported(type.children.front(),depth+1);
    if(type.kind==TypeKind::Cluster) {
        if(type.children.size()>64)return false;
        for(const auto &child:type.children)if(!binarySerializationTypeSupported(child,depth+1))return false;
        return true;
    }
    if(type.kind==TypeKind::Enum)return type.children.size()==1 && binarySerializationInteger(type.children.front());
    return binarySerializationInteger(type) || (type.kind==TypeKind::Primitive &&
        (type.name=="bool" || type.name=="string" || type.name=="f32" || type.name=="f64"));
}
inline bool binarySerializationByteOrderType(const binding::TypeRef &type) {
    return binarySerializationInteger(type) || (type.kind==binding::TypeKind::Enum &&
        type.name=="byte_order" && type.children.size()==1 && binarySerializationInteger(type.children.front()));
}
}
