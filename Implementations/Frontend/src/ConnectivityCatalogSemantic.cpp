#include "frog_native_engine/ConnectivityCatalog.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include <algorithm>

namespace frog::native_engine {
#include "ConnectivityCatalog.generated.inc"
std::span<const ConnectivityFunctionMetadata> connectivityFunctions() { return generatedFunctions(); }
std::span<const ConnectivityPaletteMetadata> connectivityPalettes() { return generatedPalettes(); }
const ConnectivityFunctionMetadata *findConnectivityFunction(std::string_view id) {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    const auto functions=connectivityFunctions();
    const auto found=std::find_if(functions.begin(),functions.end(),[id](const auto &f){return f.catalogId==id;});
    return found==functions.end()?nullptr:&*found;
}
const ConnectivityPaletteMetadata *findConnectivityPalette(std::string_view id) {
    if(id.empty())id="connectivity";
    const auto palettes=connectivityPalettes();
    const auto found=std::find_if(palettes.begin(),palettes.end(),[id](const auto &p){return p.id==id;});
    return found==palettes.end()?nullptr:&*found;
}
namespace {
const ConnectivityInstanceMetadata *selectedInstance(const ConnectivityFunctionMetadata &function,const DiagramNode &node) {
    if(!node.connectivity || node.connectivity->instanceId.empty())return nullptr;
    for(const auto &instance:function.instances)if(instance.id==node.connectivity->instanceId)return &instance;
    return nullptr;
}
bool missingInstance(const ConnectivityFunctionMetadata &function,const DiagramNode &node) {
    return node.connectivity && !node.connectivity->instanceId.empty() && !selectedInstance(function,node);
}
std::string missingInstanceReason(const DiagramNode &node) {
    return "Saved Connectivity instance '"+node.connectivity->instanceId+"' is unavailable; its signature cannot be established.";
}
const binding::TypeRef *evidenceType(std::span<const ConnectivityTypeEvidence> evidence,std::string_view id) {
    for(const auto &item:evidence)if(item.portId==id)return &item.valueType;
    return nullptr;
}
bool targetShape(const binding::TypeRef &type,std::string_view shape) {
    using Kind=binding::TypeKind;
    if(shape=="scalar")return type.kind==Kind::Primitive || type.kind==Kind::Reference || type.kind==Kind::Enum;
    if(shape=="cluster")return type.kind==Kind::Cluster;
    if(type.kind!=Kind::Array || type.children.size()!=1)return false;
    if(shape=="array_rank_2")return type.rank==2;
    if(type.rank!=1)return false;
    if(shape=="array_rank_1")return true;
    if(shape=="array_rank_1_cluster")return type.children.front().kind==Kind::Cluster;
    if(shape=="array_rank_1_noncluster")return type.children.front().kind!=Kind::Cluster;
    return false;
}
} // namespace
const ConnectivityInstanceMetadata *connectivitySelectedInstance(
    const ConnectivityFunctionMetadata &function,const DiagramNode &node,
    std::span<const ConnectivityTypeEvidence> evidence) {
    if(const auto *selected=selectedInstance(function,node))return selected;
    if(missingInstance(function,node) || evidence.empty() ||
        (function.configuration!="polymorphic" && function.configuration!="type_input"))return nullptr;
    const ConnectivityInstanceMetadata *candidate=nullptr;std::size_t candidates=0;
    for(const auto &instance:function.instances) {
        bool compatible=true,matched=false;
        for(const auto &item:evidence) {
            const auto port=std::find_if(instance.ports.begin(),instance.ports.end(),
                [&](const auto &p){return !p.output && p.id==item.portId;});
            if(port==instance.ports.end()){compatible=false;break;}
            const auto parsed=binding::parseType(port->type);
            if(!parsed.valid() || !parsed.type.known())continue;
            if(!item.valueType.known()){compatible=false;break;}
            matched=true;
            if(!binding::connect(item.valueType,parsed.type,binding::ConnectionPolicy::Exact).accepted()){
                compatible=false;break;
            }
        }
        if(compatible && matched){candidate=&instance;++candidates;}
    }
    return candidates==1?candidate:nullptr;
}
binding::TypeResolution connectivityDatabaseConversion(const DiagramNode &node,
    std::span<const ConnectivityTypeEvidence> evidence) {
    using Status=binding::ResolutionStatus;
    const auto *function=findConnectivityFunction(node.type);
    if(!function || function->catalogId!="connectivity.database.variant_to_data")
        return {Status::Incompatible,{},"binding.database_conversion_node","This is not Database Variant To Data."};
    if(missingInstance(*function,node))return {Status::Unresolved,{},"binding.connectivity_instance_unavailable",missingInstanceReason(node)};
    const auto *selected=selectedInstance(*function,node);
    binding::TypeRef source,target=node.connectivity?node.connectivity->targetType:binding::TypeRef{};
    const auto *sourceEvidence=evidenceType(evidence,"database_variant");
    const auto &declaredPorts=selected?selected->ports:function->inputs;
    for(const auto &p:declaredPorts)if(p.id=="database_variant" && !p.output) {
        const auto parsed=binding::parseType(p.type); if(parsed.valid())source=parsed.type;
    }
    if(sourceEvidence) {
        if(selected && sourceEvidence->known() && source.known() &&
            !binding::connect(*sourceEvidence,source,binding::ConnectionPolicy::Exact).accepted())
            return {Status::Incompatible,{},"binding.database_instance_shape",
                "Database variant input does not match the explicitly selected instance '"+selected->id+"'."};
        source=*sourceEvidence;
    }
    if(const auto *wired=evidenceType(evidence,"type"))target=*wired;
    if(!source.known())return {Status::Unresolved,{},"binding.database_source_unresolved","Database Variant To Data requires a concrete scalar, 1D or 2D variant source shape."};
    const ConnectivityConversionRule *rule=nullptr;
    for(const auto &candidate:function->conversionMatrix) {
        const auto parsed=binding::parseType(candidate.sourceType);
        if(parsed.valid() && parsed.type.known() &&
            binding::connect(source,parsed.type,binding::ConnectionPolicy::Exact).accepted()) {rule=&candidate;break;}
    }
    if(!rule) {
        if(function->conversionMatrix.empty())return {Status::Unresolved,{},"binding.database_matrix_unavailable","The reviewed database conversion matrix is unavailable."};
        return {Status::Incompatible,{},"binding.database_source_shape",
            "Database Variant To Data accepts only scalar variant, 1D variant array or 2D variant array; received "+source.canonical()+"."};
    }
    if(!target.known())return {Status::Unresolved,{},"binding.database_target_unresolved","Database Variant To Data requires a concrete target type specimen."};
    if(!std::any_of(rule->targetShapes.begin(),rule->targetShapes.end(),[&](const auto &shape){return targetShape(target,shape);}))
        return {Status::Incompatible,{},"binding.database_conversion_shape",
            "Database source rank "+std::to_string(rule->sourceRank)+" cannot convert to "+target.canonical()+". "+rule->rule};
    return {Status::Compatible,target,{},{}};
}
std::vector<ConnectivityConfiguredPort> connectivityNodePorts(
    const DiagramNode &node,std::span<const ConnectivityTypeEvidence> evidence) {
    const auto *function=findConnectivityFunction(node.type);
    if(!function)return {};
    if(node.nativeCall) {
        std::vector<ConnectivityConfiguredPort> result;
        for(const auto &port:nativeCallPorts(*node.nativeCall,sizeof(void*)*8))
            result.push_back({port.id,port.name,port.output,port.valueType,port.description,port.unresolvedReason});
        return result;
    }
    const bool databaseConversion=function->catalogId=="connectivity.database.variant_to_data";
    const auto unavailableInstance=missingInstance(*function,node);
    if(node.connectivity && node.connectivity->kind!=ConnectivityNodeKind::Polymorphic && !databaseConversion) {
        auto result=connectivityConfiguredPorts(*node.connectivity,evidence);
        if(unavailableInstance)for(auto &p:result){p.valueType={};p.unresolvedReason=missingInstanceReason(node);}
        return result;
    }
    std::vector<ConnectivityConfiguredPort> result;
    const ConnectivityInstanceMetadata *selected=connectivitySelectedInstance(*function,node,evidence);
    auto append=[&](const ConnectivityPortMetadata &p,bool output){
        const auto parsed=binding::parseType(p.type);
        result.push_back({p.id,p.name,output,parsed.valid()?parsed.type:binding::TypeRef{},p.description,
            parsed.valid()&&parsed.type.known()?"":"Type not established: "+p.type});
    };
    if(selected) for(const auto &p:selected->ports)append(p,p.output);
    else {
        for(const auto &p:function->inputs)append(p,false);
        for(const auto &p:function->outputs)append(p,true);
    }
    if(unavailableInstance) {
        for(auto &p:result){p.valueType={};p.unresolvedReason=missingInstanceReason(node);}
        return result;
    }
    if(databaseConversion) {
        binding::TypeRef target=node.connectivity?node.connectivity->targetType:binding::TypeRef{};
        for(const auto &item:evidence)if(item.portId=="type")target=item.valueType;
        const auto conversion=connectivityDatabaseConversion(node,evidence);
        for(auto &p:result) {
            if(p.id=="type") {
                p.valueType=target;p.unresolvedReason=conversion.accepted()?"":conversion.message;
            } else if(p.id=="data") {
                p.valueType=conversion.accepted()?target:binding::TypeRef{};
                p.unresolvedReason=conversion.accepted()?"":conversion.message;
            }
        }
    }
    return result;
}
}
