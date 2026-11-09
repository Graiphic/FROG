#include "frog_native_engine/CanonicalVariantAnalysisNative.hpp"
#include "frog_native_engine/VariantAnalysisEnums.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <array>
namespace frog::native_engine {
namespace {
using Type=binding::TypeRef;
constexpr std::array<std::string_view,14> catalogs{"variant.data_type_analysis.get_numeric_info","variant.data_type_analysis.find_contained_data_types","variant.data_type_analysis.is_error_cluster","variant.data_type_analysis.get_type_info","variant.data_type_analysis.get_array_info","variant.data_type_analysis.get_cluster_info","variant.data_type_analysis.get_fixed_point_info","variant.data_type_analysis.get_waveform_info","variant.data_type_analysis.get_refnum_info","variant.data_type_analysis.get_user_defined_refnum_info","variant.data_type_analysis.get_user_defined_hardware_reference_info","variant.data_type_analysis.get_hardware_reference_info","variant.data_type_analysis.is_or_contains_g_type","variant.data_type_analysis.disconnect_g_types"};
constexpr std::array<std::string_view,14> operations{"frog.studio.variant.data_type_analysis.get_numeric_info.v1","frog.studio.variant.data_type_analysis.find_contained_data_types.v1","frog.studio.variant.data_type_analysis.is_error_cluster.v1","frog.studio.variant.data_type_analysis.get_type_info.v1","frog.studio.variant.data_type_analysis.get_array_info.v1","frog.studio.variant.data_type_analysis.get_cluster_info.v1","frog.studio.variant.data_type_analysis.get_fixed_point_info.v1","frog.studio.variant.data_type_analysis.get_waveform_info.v1","frog.studio.variant.data_type_analysis.get_refnum_info.v1","frog.studio.variant.data_type_analysis.get_user_defined_refnum_info.v1","frog.studio.variant.data_type_analysis.get_user_defined_hardware_reference_info.v1","frog.studio.variant.data_type_analysis.get_hardware_reference_info.v1","frog.studio.variant.data_type_analysis.is_or_contains_g_type.v1","frog.studio.variant.data_type_analysis.disconnect_g_types.v1"};
std::string_view bare(std::string_view id){if(id.starts_with("frog.studio."))id.remove_prefix(12);return id;}
Type enumeration(std::string domain,std::string base){
    auto type=Type::enumeration(domain,Type::primitive(base));type.choiceKind=binding::ChoiceKind::Enum;
    for(const auto label:variantAnalysisEnumLabels(domain)){const auto i=static_cast<std::uint32_t>(type.choices.size());type.choices.push_back({"item_"+std::to_string(i),std::string(label),i,false,true});}return type;
}
Type error(){return Type::cluster({"status","code","source"},{Type::primitive("bool"),Type::primitive("i32"),Type::primitive("string")});}
}
std::string_view canonicalVariantPrimitiveMigrationIdentity(std::string_view id)noexcept {
    id=bare(id);
    constexpr std::array<std::string_view,7> names{"variant.to_variant","variant.to_data","variant.set_attribute","variant.get_attribute","variant.delete_attribute","variant.to_flattened_bytes","variant.from_flattened_bytes"};
    constexpr std::array<std::string_view,7> targets{"frog.studio.variant.to_variant.v1","frog.studio.variant.to_data.v1","frog.studio.variant.set_attribute.v1","frog.studio.variant.get_attribute.v1","frog.studio.variant.delete_attribute.v1","frog.studio.variant.to_flattened_bytes.v1","frog.studio.variant.from_flattened_bytes.v1"};
    for(std::size_t i=0;i<names.size();++i)if(id==names[i])return targets[i];return {};
}
std::string_view canonicalVariantAnalysisMigrationIdentity(std::string_view id)noexcept {id=bare(id);for(std::size_t i=0;i<catalogs.size();++i)if(catalogs[i]==id)return operations[i];return {};}
std::span<const std::string_view> canonicalVariantAnalysisOperationIds()noexcept{return operations;}
std::optional<binding::TypeRef> canonicalVariantAnalysisInternalPortType(std::string_view catalog,std::string_view port,bool input){
    catalog=bare(catalog);if(canonicalVariantAnalysisMigrationIdentity(catalog).empty())return {};
    if(input&&port=="variant")return Type::primitive("variant");if((input&&port=="error_in")||(!input&&port=="error_out"))return error();
    if(catalog==catalogs[0]&&!input){
        if(port=="enum_names")return Type::array(Type::primitive("string"));
        if(port=="units")return Type::array(Type::cluster({"Unit Type","Exponent"},{enumeration("variant.analysis.unit_type","u16"),Type::primitive("i16")}));
        if(port=="representation")return enumeration("variant.analysis.numeric_representation","u32");
    }
    if(catalog==catalogs[1]){
        if(input&&port=="data_type")return enumeration("variant.analysis.data_type","u8");
        if(input&&port=="check_refnums")return Type::primitive("bool");if(input&&port=="class_name")return Type::primitive("string");
        if(!input&&port=="contains_data_type")return Type::primitive("bool");
    }
    if(catalog==catalogs[2]&&!input&&port=="is_error_cluster")return Type::primitive("bool");
    const auto *definition=findFunctionBindingDefinition(catalog);if(!definition)return {};
    for(const auto &p:definition->ports)if(p.portId==port&&(p.direction==FunctionPortDirection::Input)==input){
        auto parsed=binding::parseType(p.typeExpression);if(!parsed.valid()||!parsed.type.known())return {};
        const auto complete=[&](auto &&self,Type &t)->void{for(auto &c:t.children)self(self,c);if(t.kind==binding::TypeKind::Enum&&t.children.size()==1)t=enumeration(t.name,t.children[0].name);};
        complete(complete,parsed.type);return parsed.type;
    }
    return {};
}
}
