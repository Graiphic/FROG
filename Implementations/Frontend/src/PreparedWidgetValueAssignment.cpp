#include "frog_native_engine/PreparedWidgetValueAssignment.hpp"
#include <algorithm>
#include <utility>

namespace frog::native_engine::data_operations::values {
bool compatibleTypes(const binding::TypeRef &a,const binding::TypeRef &b,FieldPolicy policy) {
    if(a.kind==binding::TypeKind::Map||b.kind==binding::TypeKind::Map||a.kind==binding::TypeKind::Set||b.kind==binding::TypeKind::Set)return a.known()&&b.known()&&binding::fullTypeEqual(a,b);
    if(!a.known() || !b.known() || a.kind!=b.kind || a.rank!=b.rank ||
        a.extent!=b.extent || a.children.size()!=b.children.size())return false;
    if(policy==FieldPolicy::FieldIdentity) {
        // Existing saved-default rule, including the canonical Enum owner.
        if(a.kind==binding::TypeKind::Enum)return a==b;
        if(a.kind==binding::TypeKind::Cluster) {
            if(a.fieldIds.size()!=a.children.size() || b.fieldIds.size()!=b.children.size())return false;
            for(std::size_t i=0;i<a.children.size();++i) {
                if(a.fieldIds[i].empty() || std::count(a.fieldIds.begin(),a.fieldIds.end(),a.fieldIds[i])!=1)return false;
                const auto found=std::find(b.fieldIds.begin(),b.fieldIds.end(),a.fieldIds[i]);
                if(found==b.fieldIds.end() || std::count(b.fieldIds.begin(),b.fieldIds.end(),a.fieldIds[i])!=1)return false;
                const auto j=static_cast<std::size_t>(found-b.fieldIds.begin());
                if(!compatibleTypes(a.children[i],b.children[j],policy))return false;
            }
            return true;
        }
        if(a.name!=b.name)return false;
    } else if(a.kind!=binding::TypeKind::Cluster && a.name!=b.name)return false;
    for(std::size_t i=0;i<a.children.size();++i)
        if(!compatibleTypes(a.children[i],b.children[i],policy))return false;
    return true;
}
namespace {
bool traversable(const Tree &tree) {
    if(tree.root>=tree.nodes.size())return false;
    struct Visit { std::size_t index;bool leaving; };
    std::vector<unsigned char> state(tree.nodes.size());
    std::vector<Visit> pending{{tree.root,false}};
    while(!pending.empty()) {
        const auto visit=pending.back();pending.pop_back();
        if(visit.index>=tree.nodes.size())return false;
        if(visit.leaving){state[visit.index]=2;continue;}
        if(state[visit.index]==1)return false;
        if(state[visit.index]==2)continue;
        state[visit.index]=1;pending.push_back({visit.index,true});
        const auto &node=tree.nodes[visit.index];
        if(node.container!=Container::Leaf && node.container!=Container::Cluster && node.container!=Container::Array)return false;
        if(node.graphFamily && node.container!=Container::Leaf)return false;
        if(node.container==Container::Leaf && (!node.children.empty() || !node.fieldIds.empty() || !node.cells.empty()))return false;
        if(node.container==Container::Cluster && node.fieldIds.size()!=node.children.size())return false;
        if(node.container==Container::Cluster && !node.cells.empty())return false;
        if(node.container==Container::Array && node.children.size()>1)return false;
        for(const auto child:node.children)pending.push_back({child,false});
        for(const auto &cell:node.cells) {
            if(cell.kind!=CellKind::Number && cell.kind!=CellKind::String &&
                cell.kind!=CellKind::Boolean && cell.kind!=CellKind::Composed)return false;
            if(cell.kind==CellKind::Composed) {
                if(!cell.composedValue)return false;
                pending.push_back({*cell.composedValue,false});
            } else if(cell.composedValue)return false;
        }
    }
    return true;
}
}
std::optional<Tree> assign(const Tree &target,const Tree &source,FieldPolicy policy) {
    if(!traversable(target) || !traversable(source))return {};
    Tree result;result.nodes.emplace_back();
    struct Task { const Tree *target;std::size_t targetIndex,sourceIndex,resultIndex;bool cells; };
    std::vector<Task> pending{{&target,target.root,source.root,0,false}};
    while(!pending.empty()) {
        const auto task=pending.back();pending.pop_back();
        if(task.cells) {
            // The inherited value is complete before remapping every explicit
            // composed cell against that destination prototype, as before.
            const auto prototype=result.nodes[task.resultIndex].children;
            for(std::size_t i=0;i<source.nodes[task.sourceIndex].cells.size();++i) {
                const auto &cell=source.nodes[task.sourceIndex].cells[i];
                if(cell.kind!=CellKind::Composed)continue;
                if(prototype.size()!=1 || !cell.composedValue)return {};
                const auto child=result.nodes.size();result.nodes.emplace_back();
                result.nodes[task.resultIndex].cells[i].composedValue=child;
                pending.push_back({&result,prototype.front(),*cell.composedValue,child,false});
            }
            continue;
        }
        // Copy this shallow node before growing result.nodes: a composed cell
        // can use an already-calculated result prototype as its destination.
        const auto dst=task.target->nodes[task.targetIndex];
        const auto &src=source.nodes[task.sourceIndex];
        const bool compatible=(dst.graphFamily || src.graphFamily)?dst.graphFamily && src.graphFamily:
            compatibleTypes(dst.type,src.type,policy);
        if(!compatible || dst.container!=src.container)return {};
        auto node=dst;node.assignGraph=false;
        if(dst.container==Container::Cluster) {
            if(dst.children.size()!=src.children.size())return {};
            result.nodes[task.resultIndex]=std::move(node);
            for(std::size_t i=0;i<dst.children.size();++i) {
                std::size_t from=i;
                if(policy==FieldPolicy::FieldIdentity) {
                    const auto found=std::find(src.fieldIds.begin(),src.fieldIds.end(),dst.fieldIds[i]);
                    if(found==src.fieldIds.end())return {};
                    from=static_cast<std::size_t>(found-src.fieldIds.begin());
                }
                const auto child=result.nodes.size();result.nodes.emplace_back();
                result.nodes[task.resultIndex].children[i]=child;
                pending.push_back({task.target,dst.children[i],src.children[from],child,false});
            }
        } else if(dst.container==Container::Array) {
            if(dst.children.size()!=src.children.size())return {};
            node.shape=src.shape;node.cells=src.cells;
            result.nodes[task.resultIndex]=std::move(node);
            pending.push_back({task.target,task.targetIndex,task.sourceIndex,task.resultIndex,true});
            if(!dst.children.empty()) {
                const auto child=result.nodes.size();result.nodes.emplace_back();
                result.nodes[task.resultIndex].children[0]=child;
                pending.push_back({task.target,dst.children[0],src.children[0],child,false});
            }
        } else {
            if(dst.graph && src.graph) {
                if(!src.graph->acceptedByValueOwner || !src.graph->samples)return {};
                node.graph=src.graph;node.assignGraph=true;
            } else node.scalar=src.scalar;
            result.nodes[task.resultIndex]=std::move(node);
        }
    }
    return result;
}
} // namespace frog::native_engine::data_operations::values
