#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/PreparedWidgetValueAssignment.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/WidgetDataCompatibility.hpp"

#include <algorithm>
#include <memory>
#include <utility>

namespace frog::native_engine {
namespace {
namespace values=data_operations::values;
struct GraphEnvelope { std::string source,diagnostic; };
struct Preparation {
    bool useFieldIdentities=false;
    std::vector<std::unique_ptr<WidgetInstance>> decodedCells;
    std::vector<GraphEnvelope> graphEnvelopes;
    std::vector<ApplicationControlNode> referenceEnvelopes;
    std::vector<CollectionTypedValue> collectionEnvelopes;
    std::vector<std::shared_ptr<const xy::Dataset>> xyEnvelopes;
};
// The established data-only clipboard/default contract pairs Record fields
// positionally or by identity. Adapt keys, never the destination schema.
std::optional<source::JsonValue> xyValueForSchema(const binding::TypeRef &from,
    const binding::TypeRef &to,const source::JsonValue &value,bool identities) {
    using Kind=binding::TypeKind;using Json=source::JsonValue;
    if(from.kind!=to.kind)return {};
    if(from.kind==Kind::Primitive)return from.name==to.name?std::optional{value}:std::nullopt;
    if(from.kind==Kind::Array) {
        if(from.children.size()!=1 || to.children.size()!=1)return {};
        auto output=value;
        auto *elements=output.type==Json::Type::Array?&output:
            output.object.contains("elements")?&output.object.at("elements"):nullptr;
        if(!elements || elements->type!=Json::Type::Array)return {};
        for(auto &element:elements->array) {
            auto mapped=xyValueForSchema(from.children[0],to.children[0],element,identities);
            if(!mapped)return {};element=std::move(*mapped);
        }
        return output;
    }
    if(from.kind!=Kind::Cluster || from.children.size()!=to.children.size() || value.type!=Json::Type::Object)return {};
    Json output;output.type=Json::Type::Object;
    for(std::size_t i=0;i<to.children.size();++i) {
        std::size_t sourceIndex=i;
        if(identities) {
            const auto found=std::find(from.fieldIds.begin(),from.fieldIds.end(),to.fieldIds[i]);
            if(found==from.fieldIds.end())return {};sourceIndex=std::size_t(found-from.fieldIds.begin());
        }
        const auto *field=value.member(from.fieldIds[sourceIndex]);if(!field)return {};
        auto mapped=xyValueForSchema(from.children[sourceIndex],to.children[i],*field,identities);
        if(!mapped)return {};output.object.emplace(to.fieldIds[i],std::move(*mapped));
    }
    return output;
}
std::vector<const ClusterElement *> fields(const WidgetInstance &widget) {
    std::vector<const ClusterElement *> result;
    if(const auto *cluster=clusterContainerState(widget))
        for(const auto &field:cluster->elements)result.push_back(&field);
    std::stable_sort(result.begin(),result.end(),[](auto *a,auto *b){return a->logicalOrder<b->logicalOrder;});
    return result;
}
std::optional<values::CellKind> cellKind(ArrayElementValueKind kind) {
    switch(kind) {
    case ArrayElementValueKind::Number:return values::CellKind::Number;
    case ArrayElementValueKind::String:return values::CellKind::String;
    case ArrayElementValueKind::Boolean:return values::CellKind::Boolean;
    case ArrayElementValueKind::Widget:return values::CellKind::Composed;
    }
    return {};
}
std::optional<ArrayElementValueKind> cellKind(values::CellKind kind) {
    switch(kind) {
    case values::CellKind::Number:return ArrayElementValueKind::Number;
    case values::CellKind::String:return ArrayElementValueKind::String;
    case values::CellKind::Boolean:return ArrayElementValueKind::Boolean;
    case values::CellKind::Composed:return ArrayElementValueKind::Widget;
    }
    return {};
}
std::optional<values::Tree> project(const WidgetInstance &root,bool incoming,Preparation &preparation) {
    values::Tree result;result.nodes.emplace_back();
    struct Pending { const WidgetInstance *widget;std::size_t index; };
    std::vector<Pending> pending{{&root,0}};
    while(!pending.empty()) {
        const auto task=pending.back();pending.pop_back();const auto &widget=*task.widget;
        if(!data_operations::supported(widget))return {};
        const auto parsed=binding::widgetType(widget);
        if(!parsed.valid() && !isWaveformGraphWidget(widget))return {};
        values::Node node;node.type=parsed.type;
        node.graphFamily=isWaveformGraphWidget(widget);
        node.scalar={widget.initialValue,widget.numericValue,widget.booleanValue,widget.supportTextValue};
        if(widget.referenceValue) {
            node.scalar.referenceEnvelope=preparation.referenceEnvelopes.size();
            preparation.referenceEnvelopes.push_back(*widget.referenceValue);
        }
        if(widget.collection) {
            const auto typed=collectionValue(*widget.collection);if(!typed)return {};
            node.scalar.collectionEnvelope=preparation.collectionEnvelopes.size();preparation.collectionEnvelopes.push_back(*typed);
        }
        if(widget.xyGraph) {
            const auto data=xy::displayedData(*widget.xyGraph);if(!data)return {};
            if(!xy::validateType(data->datatype).empty() || !binding::fullTypeEqual(data->datatype,parsed.type))return {};
            node.scalar.xyEnvelope=preparation.xyEnvelopes.size();
            preparation.xyEnvelopes.push_back(data);
        }
        const auto child=[&](const WidgetInstance &value) {
            const auto index=result.nodes.size();result.nodes.emplace_back();pending.push_back({&value,index});return index;
        };
        if(isClusterWidget(widget)) {
            node.container=values::Container::Cluster;
            for(const auto *field:fields(widget)) {
                if(!field->widget)return {};
                node.fieldIds.push_back(field->fieldId);node.children.push_back(child(*field->widget));
            }
        } else if(const auto *array=arrayContainerState(widget)) {
            node.container=values::Container::Array;node.shape=array->shape;
            if(array->elementTemplate)node.children.push_back(child(*array->elementTemplate));
            // Existing destination cells are replaced, never interpreted by
            // assignment. Preserve the preceding canonical codec guard rather
            // than adding a new refusal for dormant destination payloads.
            if(incoming)for(const auto &cell:array->elements) {
                const auto kind=cellKind(cell.kind);if(!kind)return {};
                values::Cell value;value.indices=cell.indices;value.kind=*kind;
                value.defined=cell.defined;value.enabled=cell.enabled;
                if(cell.kind==ArrayElementValueKind::Widget) {
                    auto decoded=deserializeArrayElementWidget(cell.displayValue,array->elementTemplate.get());
                    if(!decoded)return {};
                    preparation.decodedCells.push_back(std::make_unique<WidgetInstance>(std::move(*decoded)));
                    value.composedValue=child(*preparation.decodedCells.back());
                } else value.scalarLexeme=cell.displayValue;
                node.cells.push_back(std::move(value));
            }
        } else if(widget.waveformGraph) {
            const auto &graphState=*widget.waveformGraph;values::GraphValue value;
            if(graphState.preservedSource) {
                value.samples=std::make_shared<const graph::Dataset>(graphState.initialData);
                value.acceptedByValueOwner=true;value.retainedEnvelope=preparation.graphEnvelopes.size();
                preparation.graphEnvelopes.push_back({*graphState.preservedSource,graphState.diagnostic});
            } else {
                value.samples=std::make_shared<const graph::Dataset>(graphState.displayedData());
                value.acceptedByValueOwner=graph::validate(*value.samples).empty();
            }
            node.graph=std::move(value);
        }
        result.nodes[task.index]=std::move(node);
    }
    return result;
}
bool apply(WidgetInstance &target,const values::Tree &tree,std::size_t index,const Preparation &preparation) {
    const auto &value=tree.nodes[index];
    if(value.container==values::Container::Cluster) {
        const auto destination=fields(target);
        if(destination.size()!=value.children.size())return false;
        for(std::size_t i=0;i<destination.size();++i)
            if(destination[i]->fieldId!=value.fieldIds[i] || !destination[i]->widget ||
                !apply(*destination[i]->widget,tree,value.children[i],preparation))return false;
    } else if(value.container==values::Container::Array) {
        auto *array=arrayContainerState(target);if(!array)return false;
        if(!value.children.empty() && (!array->elementTemplate ||
            !apply(*array->elementTemplate,tree,value.children[0],preparation)))return false;
        std::vector<ArrayElementValue> cells;cells.reserve(value.cells.size());
        for(const auto &stored:value.cells) {
            const auto kind=cellKind(stored.kind);if(!kind)return false;
            ArrayElementValue cell;cell.indices=stored.indices;cell.kind=*kind;
            cell.defined=stored.defined;cell.enabled=stored.enabled;
            if(stored.kind==values::CellKind::Composed) {
                if(!array->elementTemplate || !stored.composedValue)return false;
                auto current=*array->elementTemplate;
                if(!apply(current,tree,*stored.composedValue,preparation))return false;
                cell.displayValue=serializeArrayElementValueWidget(current,*array->elementTemplate);
                // Same private writer and validation gate. Unsupported legacy
                // Graph rewriting stays an explicit rejection, not empty data.
                if(cell.displayValue.empty() || !deserializeArrayElementWidget(cell.displayValue,array->elementTemplate.get()))return false;
            } else cell.displayValue=stored.scalarLexeme;
            cells.push_back(std::move(cell));
        }
        array->shape=value.shape;array->elements=std::move(cells);
    } else if(value.assignGraph) {
        if(!target.waveformGraph || !value.graph || !value.graph->samples)return false;
        auto &graph=*target.waveformGraph;graph.initialData=*value.graph->samples;
        if(value.graph->retainedEnvelope) {
            if(*value.graph->retainedEnvelope>=preparation.graphEnvelopes.size())return false;
            const auto &envelope=preparation.graphEnvelopes[*value.graph->retainedEnvelope];
            graph.preservedSource=envelope.source;graph.diagnostic=envelope.diagnostic;
        } else {graph.preservedSource.reset();graph.diagnostic.clear();}
        graph.current.reset();++graph.dataRevision;
    } else {
        if(target.xyGraph) {
            if(!value.scalar.xyEnvelope || *value.scalar.xyEnvelope>=preparation.xyEnvelopes.size())return false;
            const auto &data=preparation.xyEnvelopes[*value.scalar.xyEnvelope];
            if(!data)return false;
            const auto mapped=xyValueForSchema(data->datatype,target.xyGraph->datatype,data->exactSource,preparation.useFieldIdentities);
            if(!mapped || !xy::setInitialData(*target.xyGraph,target.xyGraph->datatype,*mapped))return false;
        }
        if(target.collection) {
            if(!value.scalar.collectionEnvelope||*value.scalar.collectionEnvelope>=preparation.collectionEnvelopes.size())return false;
            if(!assignCollectionData(*target.collection,preparation.collectionEnvelopes[*value.scalar.collectionEnvelope]))return false;
        }
        target.initialValue=value.scalar.initialValue;target.numericValue=value.scalar.numericValue;
        target.booleanValue=value.scalar.booleanValue;target.supportTextValue=value.scalar.supportTextValue;
        if(target.classId==kReferenceWidgetClass) {
            if(value.scalar.referenceEnvelope) {
                if(*value.scalar.referenceEnvelope>=preparation.referenceEnvelopes.size())return false;
                target.referenceValue=preparation.referenceEnvelopes[*value.scalar.referenceEnvelope];
            } else clearReferenceWidgetValue(target);
        }
    }
    return true;
}
}
bool assignPreparedPrivateWidgetValues(WidgetInstance &detachedTarget,const WidgetInstance &source,
    bool useFieldIdentities) {
    Preparation preparation;
    preparation.useFieldIdentities=useFieldIdentities;
    const auto targetValue=project(detachedTarget,false,preparation);
    const auto sourceValue=project(source,true,preparation);
    if(!targetValue || !sourceValue)return false;
    const auto result=values::assign(*targetValue,*sourceValue,useFieldIdentities?
        values::FieldPolicy::FieldIdentity:values::FieldPolicy::LogicalOrder);
    return result && apply(detachedTarget,*result,result->root,preparation);
}
} // namespace frog::native_engine
