#pragma once
#include "frog_native_engine/DiagramExecutionContract.hpp"

namespace frog::native_engine {
// Shared Source/editor projection of the two faces of an indexed tunnel.
// Loop ownership and mode availability are checked by the respective caller.
[[nodiscard]] inline binding::TypeResolution diagramIndexedTunnelType(binding::TypeRef source,
    bool output,DiagramTunnelIndexingPolicy policy) {
    using namespace binding;
    const auto invalid=[](const char *message){return TypeResolution{ResolutionStatus::Incompatible,{},"binding.tunnel_mode",message};};
    if(!source.known())return {ResolutionStatus::Unresolved,{},"binding.type_unresolved","An indexed tunnel requires its complete source schema."};
    if(policy==DiagramTunnelIndexingPolicy::AtomicElements) {
        if(output)source=TypeRef::array(std::move(source),1);
        else if(source.kind==TypeKind::Map&&source.children.size()==2) {
            auto pair=TypeRef::cluster({"key","value"},{source.children[0],source.children[1]});
            pair.fieldNames={"key","value"};source=std::move(pair);
        }else if(source.kind==TypeKind::Set&&source.children.size()==1)source=source.children.front();
        else return invalid("Atomic indexed input requires a Map or Set.");
    }else if(policy==DiagramTunnelIndexingPolicy::LeadingDimension) {
        if(output) {
            if(source.kind==TypeKind::Array) {
                if(source.rank>=16)return invalid("Indexing would exceed the maximum Array rank of16.");
                ++source.rank;source.extent.reset();
            }else source=TypeRef::array(std::move(source),1);
        }else {
            if(source.kind!=TypeKind::Array)return invalid("Leading-dimension indexed input requires an Array.");
            if(source.rank==1)source=source.children.front();
            else {--source.rank;source.extent.reset();}
        }
    }else return invalid("The tunnel indexing policy is unknown.");
    return {ResolutionStatus::Compatible,std::move(source),{}, {}};
}
}
