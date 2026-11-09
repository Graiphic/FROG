#include "frog_native_engine/DraftSourcePreparation.hpp"
#include "frog_native_engine/DiagramLiteralSchema.hpp"
#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"

#include <algorithm>
#include <charconv>
#include <set>

namespace frog::native_engine {
namespace {
const DiagramNode *nodeById(const std::vector<DiagramNode> &nodes,std::string_view id) {
    const auto found=std::find_if(nodes.begin(),nodes.end(),[&](const auto &node){return node.id==id;});
    return found==nodes.end()?nullptr:&*found;
}
std::optional<WidgetInstance> decode(const std::vector<DiagramNode> &nodes,const DiagramNode &node,
    std::string &error,std::set<std::string> &active,unsigned depth) {
    if(depth>32 || !active.insert(node.id).second){error="Constant ownership exceeds its bounded depth or contains a cycle.";return {};}
    const auto finish=[&](std::optional<WidgetInstance> widget) {
        active.erase(node.id);
        if(widget){widget->instanceId=node.id;widget->role="control";widget->label=node.label;
            widget->labelVisible=node.labelVisible;widget->bounds=node.layout;widget->typeDefinition=node.typeDefinition;}
        return widget;
    };
    const bool array=node.type.find("array_constant")!=std::string::npos;
    const bool enumeration=node.type.find("enum")!=std::string::npos;
    const bool boolean=node.type.find("boolean.")!=std::string::npos || node.valueType=="bool";
    const bool string=node.type.find("string.")!=std::string::npos || node.valueType=="string";
    const bool cluster=isDiagramClusterConstant(node);
    const std::string_view klass=cluster?"frog.widgets.cluster_control":array?"frog.widgets.array_control":
        enumeration?"frog.widgets.enum_control":string?"frog.widgets.string_control":
        boolean?"frog.widgets.boolean_control":"frog.widgets.numeric_control";
    const auto *entry=findWidgetCatalogEntryByClassId(klass);
    if(!entry || !isDiagramLiteralConstant(node)){error="This Diagram object is not a supported typed constant.";return finish({});}
    auto widget=makeWidgetData(node.id,*entry,node.layout);
    if(cluster) {
        auto *container=clusterContainerState(widget);container->elements.clear();
        for(const auto *field:diagramClusterConstantFields(nodes,node.id)) {
            const auto child=decode(nodes,*field,error,active,depth+1);if(!child)return finish({});
            container->elements.push_back({field->constantFieldId,field->label,field->constantFieldOrder,
                std::make_shared<WidgetInstance>(*child)});
        }
        widget.valueType=binding::widgetType(widget).type.canonical();
    } else if(array) {
        auto *container=arrayContainerState(widget);container->rank=node.numericArrayRank;
        container->shape=node.numericArrayShape;
        if(container->shape.empty() && container->rank==1)container->shape={static_cast<std::uint32_t>(node.numericArrayValues.size())};
        if(container->rank<1 || container->rank>16 || container->shape.size()!=container->rank){error="The constant Array rank and shape disagree.";return finish({});}
        container->firstIndices=node.numericArrayFirstIndices;container->visibleCounts=node.numericArrayVisibleCounts;
        container->orientation=node.numericArrayOneDimensionalVertical?"vertical":"horizontal";
        container->cellWidth=node.numericArrayCellWidth;container->cellHeight=node.numericArrayCellHeight;
        if(node.arrayElementTemplate)container->elementTemplate=std::make_shared<WidgetInstance>(*node.arrayElementTemplate);
        else {
            auto element=node;element.id=node.id+".element";element.constantOwnerNodeId.clear();element.typeDefinition.reset();
            element.type=enumeration?"frog.studio.numeric.enum_constant":string?"frog.studio.string.constant":
                node.valueType=="bool"?"frog.studio.boolean.false_constant":"frog.studio.numeric.numeric_constant";
            element.text="0";element.numericArrayValues.clear();
            const auto child=decode(nodes,element,error,active,depth+1);if(!child)return finish({});
            container->elementTemplate=std::make_shared<WidgetInstance>(*child);
        }
        std::size_t count=1;
        for(const auto extent:container->shape){if(extent>262144 || (extent && count>262144/extent)){error="The constant Array exceeds its value budget.";return finish({});}count*=extent;}
        if(count!=node.numericArrayValues.size()){error="The constant Array shape and number of cells disagree.";return finish({});}
        std::vector<ArrayElementValue> cells;cells.reserve(count);
        for(std::size_t index=0;index<count;++index) {
            std::vector<std::uint32_t> indices(container->rank);auto remainder=index;
            for(std::size_t axis=container->rank;axis-->0;){indices[axis]=static_cast<std::uint32_t>(remainder%container->shape[axis]);remainder/=container->shape[axis];}
            auto value=deserializeArrayElementWidget(node.numericArrayValues[index],container->elementTemplate.get());
            ArrayElementValue cell;cell.indices=std::move(indices);cell.displayValue=node.numericArrayValues[index];
            if(value)cell.kind=ArrayElementValueKind::Widget;
            else if(isEnumWidget(*container->elementTemplate)) {
                std::uint32_t selected=0;const auto &text=cell.displayValue;
                const auto parsed=std::from_chars(text.data(),text.data()+text.size(),selected);
                const auto &items=ringItems(*container->elementTemplate);
                const auto item=std::find_if(items.begin(),items.end(),[&](const auto &candidate){return candidate.value==selected;});
                if(parsed.ec!=std::errc{} || parsed.ptr!=text.data()+text.size() || item==items.end()){error="An Enum Array constant value is absent from its domain.";return finish({});}
                cell.displayValue=item->id;cell.kind=ArrayElementValueKind::Number;
            } else if(isStringWidget(*container->elementTemplate))cell.kind=ArrayElementValueKind::String;
            else if(isBooleanWidget(*container->elementTemplate))cell.kind=ArrayElementValueKind::Boolean;
            cells.push_back(std::move(cell));
        }
        container->elements=std::move(cells);
        widget.valueType=binding::widgetType(widget).type.canonical();
    } else if(enumeration) {
        const auto parsed=binding::parseType(node.valueType);
        widget.valueType=parsed.type.kind==binding::TypeKind::Enum && !parsed.type.children.empty()?parsed.type.children.front().canonical():node.valueType;
        if(!parsed.valid() || (widget.valueType!="u8" && widget.valueType!="u16" && widget.valueType!="u32")) {
            error="An Enum constant needs its exact U8, U16 or U32 representation.";return finish({});
        }
        static_cast<void>(setEnumRepresentationKind(widget,widget.valueType));
        if(enumRepresentationValueType(widget)!=widget.valueType) {
            error="The Enum constant representation could not be restored.";return finish({});
        }
        widget.enumDomainId=node.enumDomainId;widget.ringItems=node.enumItems;
        const bool present=std::any_of(widget.ringItems.begin(),widget.ringItems.end(),[&](const auto &item){return item.value==node.enumSelectedValue&&!item.disabled;});
        // The setter reports a change, not validity: the catalogue's initial
        // zero is already selected when a surviving Enum item moves to zero.
        if(!present || (widgetNumericValue(widget)!=node.enumSelectedValue&&!setRingSelectedValue(widget,node.enumSelectedValue))) {
            error="An Enum constant value is absent from its declared domain.";return finish({});
        }
    } else if(string)static_cast<void>(setWidgetStringValue(widget,node.text));
    else if(boolean)static_cast<void>(setWidgetBooleanValue(widget,node.text=="true" || node.type.ends_with("true_constant")));
    else {
        widget.valueType=node.valueType.empty()?"f64":node.valueType;
        if(numericCanonicalValueTypeIsComplex(widget.valueType)) {
            if(!setWidgetNumericComplexValueLexeme(widget,node.text)){error="The constant is outside its declared complex representation.";return finish({});}
        } else if(numericCanonicalValueTypeIsIntegral(widget.valueType)) {
            if(!setWidgetNumericIntegerValueLexeme(widget,node.text.empty()?"0":node.text)){error="The constant is outside its declared integer representation.";return finish({});}
        } else {
            double value=0;const auto text=node.text.empty()?std::string{"0"}:node.text;
            const auto parsed=std::from_chars(text.data(),text.data()+text.size(),value);
            if(parsed.ec!=std::errc{} || parsed.ptr!=text.data()+text.size() || !setWidgetNumericValue(widget,value)){error="The constant is outside its declared numeric representation.";return finish({});}
            widget.initialValue=text;
        }
    }
    return finish(std::move(widget));
}
bool install(std::vector<DiagramNode> &nodes,const std::string &id,const WidgetInstance &widget,
    std::string &error,unsigned depth) {
    if(depth>32){error="The constant composition exceeds its bounded depth.";return false;}
    const auto *old=nodeById(nodes,id);if(!old){error="The actual constant node is missing.";return false;}
    auto node=*old;node.typeDefinition=widget.typeDefinition;
    const auto type=binding::widgetType(widget);if(!type.valid()||!type.type.known()){error="The proposed constant schema is unresolved.";return false;}
    if(const auto *cluster=clusterContainerState(widget)) {
        if(node.type!="frog.studio.error.constant" || !binding::isErrorCluster(type.type))node.type="frog.studio.cluster.constant";
        node.valueType=type.type.canonical();
        std::set<std::string> retained;
        std::size_t serial=0;
        for(const auto &field:cluster->elements) {
            if(!field.widget){error="A proposed Cluster field has no value.";return false;}
            const auto fields=diagramClusterConstantFields(nodes,id);
            const auto existing=std::find_if(fields.begin(),fields.end(),[&](const auto *candidate){return candidate->constantFieldId==field.fieldId;});
            std::string childId;
            if(existing!=fields.end())childId=(*existing)->id;
            else {
                do{childId=id+".typedef_field_"+std::to_string(serial++);}while(nodeById(nodes,childId));
                DiagramNode child;child.id=childId;child.kind="primitive";
                child.layout={node.layout.x+12,node.layout.y+12+32.0*field.logicalOrder,80,28};
                child.diagramOwnerStructureId=node.diagramOwnerStructureId;child.diagramOwnerRegionId=node.diagramOwnerRegionId;
                child.constantOwnerNodeId=id;child.constantFieldId=field.fieldId;nodes.push_back(std::move(child));
                node.layout.height=(std::max)(node.layout.height,52.0+32.0*field.logicalOrder);
                node.layout.width=(std::max)(node.layout.width,104.0);
            }
            retained.insert(childId);
            if(!install(nodes,childId,*field.widget,error,depth+1))return false;
            auto child=std::find_if(nodes.begin(),nodes.end(),[&](const auto &candidate){return candidate.id==childId;});
            child->constantFieldOrder=field.logicalOrder;child->label=field.widget->label;
        }
        std::set<std::string> removed;
        for(const auto &child:nodes)if(child.constantOwnerNodeId==id && !retained.contains(child.id))removed.insert(child.id);
        bool expanded=true;
        while(expanded){expanded=false;for(const auto &child:nodes)if(removed.contains(child.constantOwnerNodeId))expanded=removed.insert(child.id).second || expanded;}
        nodes.erase(std::remove_if(nodes.begin(),nodes.end(),[&](const auto &child){return removed.contains(child.id);}),nodes.end());
    } else if(const auto *array=arrayContainerState(widget)) {
        if(!array->elementTemplate){error="The proposed Array needs its real typed element template.";return false;}
        node.type="frog.studio.numeric.numeric_array_constant";
        node.arrayElementTemplate=*array->elementTemplate;node.valueType=binding::widgetType(*array->elementTemplate).type.canonical();
        node.numericArrayRank=array->rank;node.numericArrayShape=array->shape;node.numericArrayValues.clear();
        // Installing into a fresh palette shell must already produce the
        // canonical viewport vectors written by Save/Open. Keep authored
        // axes and initialise only axes absent from the previous constant.
        node.numericArrayFirstIndices.resize(node.numericArrayRank,0U);
        node.numericArrayVisibleCounts.resize(node.numericArrayRank,1U);
        if(node.numericArrayRank!=1U)node.numericArrayOneDimensionalVertical=false;
        normalizeDraftArrayConstantVisibleCounts(node);
        std::size_t count=1;for(const auto extent:array->shape){if(extent>262144 || (extent&&count>262144/extent)){error="The proposed Array exceeds its value budget.";return false;}count*=extent;}
        std::vector<std::string> preparedValues(count,serializeArrayElementValueWidget(*array->elementTemplate,*array->elementTemplate));
        for(const auto &cell:array->elements)if(cell.defined) {
            if(cell.indices.size()!=array->rank){error="A stored Array cell has an invalid rank.";return false;}
            std::size_t index=0;
            for(std::size_t axis=0;axis<array->rank;++axis){if(cell.indices[axis]>=array->shape[axis]){error="A stored Array cell is outside its shape.";return false;}index=index*array->shape[axis]+cell.indices[axis];}
            auto value=*array->elementTemplate;materializeArrayElementValue(value,&cell);
            preparedValues[index]=serializeArrayElementValueWidget(value,*array->elementTemplate);
        }
        node.numericArrayValues=std::move(preparedValues);
    } else if(isEnumWidget(widget)) {
        node.type="frog.studio.numeric.enum_constant";node.valueType=enumRepresentationValueType(widget);
        node.enumDomainId=widget.enumDomainId.value_or("");node.enumItems=ringItems(widget);
        node.enumSelectedValue=static_cast<std::uint32_t>(widgetNumericValue(widget));
    } else if(isStringWidget(widget)) {node.type="frog.studio.string.constant";node.valueType="string";node.text=widgetStringValue(widget);}
    else if(isBooleanWidget(widget)||isButtonWidget(widget)) {
        const bool value=widgetBooleanValue(widget);node.type=value?"frog.studio.boolean.true_constant":"frog.studio.boolean.false_constant";
        node.valueType="bool";node.text=value?"true":"false";
    } else if(isNumericWidget(widget)) {
        node.type="frog.studio.numeric.numeric_constant";node.valueType=widget.valueType;
        node.text=numericCanonicalValueTypeIsComplex(widget.valueType)?widgetNumericComplexValueLexeme(widget).value_or("0+0i"):
            numericCanonicalValueTypeIsIntegral(widget.valueType)?widgetNumericIntegerValueLexeme(widget).value_or("0"):
            widget.initialValue.value_or(formatNumericValue(widgetNumericValue(widget)));
    } else {error="The proposed constant family is not supported by standard Type Def V1.";return false;}
    auto target=std::find_if(nodes.begin(),nodes.end(),[&](const auto &candidate){return candidate.id==id;});
    *target=std::move(node);return true;
}
}
std::optional<WidgetInstance> typeDefinitionConstantWidget(const std::vector<DiagramNode> &nodes,
    std::string_view id,std::string *error) {
    std::string reason;const auto *node=nodeById(nodes,id);
    if(!node){if(error)*error="The constant node does not exist.";return {};}
    std::set<std::string> active;auto result=decode(nodes,*node,reason,active,0);
    if(error)*error=std::move(reason);return result;
}
bool installTypeDefinitionConstantWidget(std::vector<DiagramNode> &nodes,std::string_view id,
    const WidgetInstance &widget,std::string *error) {
    auto candidate=nodes;std::string reason;
    if(!install(candidate,std::string(id),widget,reason,0)){if(error)*error=std::move(reason);return false;}
    nodes=std::move(candidate);return true;
}
} // namespace frog::native_engine
