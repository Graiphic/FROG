#include "frog_native_engine/ObjectReferenceSchema.hpp"
#include "frog_native_engine/ApplicationControlSchema.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include <algorithm>
#include <set>
namespace frog::native_engine {
std::string objectReferenceTextPartId(std::string_view owner,std::string_view part) {
    return std::to_string(owner.size())+":"+std::string(owner)+"/"+std::string(part);
}
const ObjectReferenceRecord *ObjectReferenceSchema::find(ObjectReferenceKind kind,std::string_view id)const noexcept {
    const auto found=std::find_if(records.begin(),records.end(),[&](const auto &record){return record.kind==kind&&record.id==id;});
    return found==records.end()?nullptr:&*found;
}
ObjectReferenceSchema buildFrogObjectReferenceSchema(FrogWidgetDataView canvas) {
    const auto invalid=[] {ObjectReferenceSchema result;result.error="Reference targets contain a duplicate identity, cycle or invalid Cluster field.";return result;};
    std::vector<ObjectReferenceRecord> records{
        {ObjectReferenceKind::Application,"application","frog.application",{},{}},
        {ObjectReferenceKind::Program,"program","frog.program","application",{}},
        {ObjectReferenceKind::FrontPanel,"front_panel","frog.front_panel","program",{}},
        {ObjectReferenceKind::Pane,"front_panel","frog.pane","front_panel",{}}};
    std::map<std::string,DocumentReferenceLocation,std::less<>> locations;
    std::map<std::string,std::pair<std::string,std::string>,std::less<>> textParts;
    std::map<std::pair<ObjectReferenceKind,std::string>,std::vector<std::string>> children;
    struct Pending {const WidgetInstance *widget;DocumentReferenceLocation location;std::string parent;};
    std::vector<Pending> pending;
    for(auto it=canvas.objects().rbegin();it!=canvas.objects().rend();++it)if(it->widget)
        pending.push_back({&*it->widget,{it->id,{}},"front_panel"});
    std::set<const WidgetInstance *> seen;
    while(!pending.empty()) {
        auto current=std::move(pending.back());pending.pop_back();
        const auto &widget=*current.widget;
        if(!seen.insert(current.widget).second || widget.instanceId.empty() || locations.contains(widget.instanceId)) {
            return invalid();
        }
        const auto type=binding::widgetType(widget);
        records.push_back({ObjectReferenceKind::Widget,widget.instanceId,applicationControlReferenceClass(widget),current.parent,
            type.valid()?type.type:binding::TypeRef{}});
        children[{current.location.path.empty()?ObjectReferenceKind::FrontPanel:ObjectReferenceKind::Widget,current.parent}].push_back(widget.instanceId);
        locations.emplace(widget.instanceId,current.location);
        const auto addText=[&](std::string_view part,std::string_view classId) {
            const auto id=objectReferenceTextPartId(widget.instanceId,part);
            records.push_back({ObjectReferenceKind::TextPart,id,std::string(classId),widget.instanceId,{}});
            textParts.emplace(id,std::pair{widget.instanceId,std::string(part)});
        };
        const auto *definition=findWidgetDefinitionByClassId(widget.classId);
        if(definition && widgetDefinitionHasCapability(*definition,WidgetCapability::Label)) {
            addText("label","frog.text.label");addText("caption","frog.text.caption");
        }
        if(isBooleanWidget(widget)||isButtonWidget(widget))addText("boolean_text","frog.text.boolean");

        // Array templates are presentation objects, never an indexed cell.
        if(const auto *array=arrayContainerState(widget);array && array->elementTemplate) {
            auto child=current.location;child.path.push_back({{}, {}});child.presentationOnly=true;
            pending.push_back({array->elementTemplate.get(),std::move(child),widget.instanceId});
        }
        // Cluster fields keep their stable IDs, including presentation children.
        if(const auto *cluster=clusterContainerState(widget)) {
            std::vector<const ClusterElement *> fields;
            std::set<std::string_view> fieldIds;
            for(const auto &field:cluster->elements) {
                if(field.fieldId.empty() || !fieldIds.insert(field.fieldId).second || !field.widget)return invalid();
                fields.push_back(&field);
            }
            std::stable_sort(fields.begin(),fields.end(),[](auto *a,auto *b){return a->logicalOrder<b->logicalOrder;});
            for(auto field=fields.rbegin();field!=fields.rend();++field) {
                auto child=current.location;child.path.push_back({(*field)->fieldId,{}});
                pending.push_back({(*field)->widget.get(),std::move(child),widget.instanceId});
            }
        }
    }

    ObjectReferenceSchema result;
    result.records=std::move(records);result.locations=std::move(locations);
    result.children=std::move(children);result.textParts=std::move(textParts);
    return result;
}
}
