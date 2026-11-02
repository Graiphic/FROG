#pragma once
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include <span>
#include <vector>

namespace frog::native_engine {
// Positional Unbundle output count follows its incoming schema. A saved grip
// count is presentation state; derive ports without editing the source/history.
[[nodiscard]] inline std::vector<DiagramNode> projectClusterFunctionPorts(
    std::span<const DiagramNode> nodes,std::span<const DiagramEdge> edges,
    FrogWidgetDataView canvas) {
    std::vector<DiagramNode> projected(nodes.begin(),nodes.end());
    const auto passes=static_cast<std::size_t>(std::count_if(projected.begin(),projected.end(),
        [](const auto &n){return clusterFunctionId(n.type)=="cluster.unbundle";}));
    // Refresh the resolver/cache each pass for chains whose upstream count was
    // also stale. An acyclic chain needs at most one pass per Unbundle.
    for(std::size_t pass=0;pass<passes;++pass) {
        std::vector<std::pair<std::size_t,unsigned>> counts;
        {
            binding::TypeResolver resolver(projected,edges,canvas);
            for(std::size_t i=0;i<projected.size();++i)if(clusterFunctionId(projected[i].type)=="cluster.unbundle") {
                const auto schema=resolver.resolve({projected[i].id,"cluster"}).resolution;
                if(schema.accepted()&&schema.type.kind==binding::TypeKind::Cluster&&!schema.type.children.empty()&&schema.type.children.size()<=32&&
                   projected[i].clusterFunctionTerminals!=schema.type.children.size())
                    counts.emplace_back(i,static_cast<unsigned>(schema.type.children.size()));
            }
        }
        if(counts.empty())break;
        for(const auto &[i,count]:counts)projected[i].clusterFunctionTerminals=count;
    }
    return projected;
}
}
