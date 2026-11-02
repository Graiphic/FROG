#include "frog_native_engine/CanonicalEventExecutionIr.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "CanonicalExecutionIrStorage.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <array>
#include <map>
#include <set>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using Json=source::JsonValue;
using namespace canonical_json;
struct Failure {CanonicalIrDiagnostic diagnostic;};
[[noreturn]] void fail(std::string code,std::string message,std::string location,bool unsupported=false) {
    throw Failure{{std::move(code),std::move(message),std::move(location),unsupported}};
}
std::string location(std::string_view ref) {
    const auto hash=ref.find('#');return hash==ref.npos?std::string(ref):std::string(ref.substr(hash+1));
}
std::string text(const Json *value,std::string_view at) {
    if(!value||value->type!=Json::Type::String)fail("EVENT_BODY_SEAL","The sealed Event projection lost a required string.",std::string(at));
    return value->string;
}
bool transport(std::string_view type) {
    constexpr std::array names{"bool","i32","u32","i64","u64","f64","string","path"};
    return std::find(names.begin(),names.end(),type)!=names.end();
}
void policy(const CanonicalIrReadOptions &options) {
    const auto &a=options.limits;const CanonicalIrLimits b;
    const std::array requested{a.maximumIrBytes,a.maximumSourceBytes,a.maximumNodes,a.maximumEdges,a.maximumWidgets,a.maximumSourceIdBytes,a.maximumIrIdBytes,a.maximumMappingRecords};
    const std::array ceilings{b.maximumIrBytes,b.maximumSourceBytes,b.maximumNodes,b.maximumEdges,b.maximumWidgets,b.maximumSourceIdBytes,b.maximumIrIdBytes,b.maximumMappingRecords};
    for(std::size_t i=0;i<requested.size();++i)if(!requested[i]||requested[i]>ceilings[i])
        fail("EVENT_BODY_RESOURCE_POLICY","Budgets may only lower the common FIR intake ceilings.","/",true);
}
// Admit the WHOLE Event document before projecting any one case. In
// particular PaneRef is not erased merely because it is not selected.
void corridor(const CanonicalEventExecutionIr &ir) {
    for(const auto &region:ir.regions()) {
        for(const auto &[field,type]:region.payloadSchema)if(type.category==sdk::runtime::ValueCategory::ObjectReference)
            fail("EVENT_BODY_PAYLOAD_UNSUPPORTED","The complete occurrence schema contains an unsupported reference, including unexposed fields: "+type.canonicalName,
                location(region.sourceRef)+"/event_payload_fields",true);
        for(const auto &[field,type]:region.payloadSchema)if(!transport(type.canonicalName))
            fail("EVENT_BODY_PAYLOAD_UNSUPPORTED","The complete occurrence schema contains an unsupported type: "+type.canonicalName,
                location(region.sourceRef)+"/event_payload_fields",true);
        if(region.subscription) {
            if(region.subscription->scope!="control"||region.subscription->kind!="value_change"||region.subscription->filter)
                fail("EVENT_BODY_NOTIFICATION_UNSUPPORTED","Only the existing public Value Change and Timeout contracts have a compiled body profile.",location(region.sourceRef)+"/event",true);
            if(region.payloadSchema.size()!=2||!region.payloadSchema.contains("old_value")||!region.payloadSchema.contains("new_value"))
                fail("EVENT_BODY_PAYLOAD_REQUIRED","Compiled Value Change requires the two explicit, complete OldVal/NewVal declarations.",location(region.sourceRef)+"/event_payload_fields",true);
        } else if(!region.payloadSchema.empty())
            fail("EVENT_BODY_TIMEOUT_PAYLOAD","Timeout cannot acquire an occurrence payload.",location(region.sourceRef));
        if(!region.responseSchema.empty())fail("EVENT_BODY_EFFECT_UNSUPPORTED","Filter/default-action responses have no compiled profile here.",location(region.sourceRef),true);
        if(region.nodes.empty())fail("EVENT_BODY_EMPTY_UNSUPPORTED","The current structured compiler requires an actual nonempty body.",location(region.sourceRef),true);
        std::set<std::string> widgets;
        for(const auto &node:region.nodes) {
            if(node.family=="primitive") {
                if(node.kind!="frog.core.add")fail("EVENT_BODY_OPERATION_UNSUPPORTED","This first Event body profile admits the actual f64 Core Add only.",location(node.sourceRef),true);
            } else if(node.family=="widget_value") {
            if(!widgets.insert(node.widgetId).second)fail("EVENT_BODY_WIDGET_READ_UNSUPPORTED","Repeated participation needs an explicit read-instance contract.",location(node.sourceRef),true);
            } else if(node.family!="support"||node.role!="event_data")
                fail("EVENT_BODY_EFFECT_UNSUPPORTED","Only read-only Event Data support belongs to this compiled body profile.",location(node.sourceRef),true);
            for(const auto &port:node.ports)if(!transport(port.typeRef))
                fail("EVENT_BODY_TYPE_UNSUPPORTED","This exact port type has no compiled Event transport: "+port.typeRef,location(node.sourceRef),true);
        }
    }
    for(const auto &widget:ir.widgets())if(!transport(widget.typeRef))
        fail("EVENT_BODY_TYPE_UNSUPPORTED","The Event document contains a widget type outside its explicit compiled transport.","/front_panel/widgets",true);
}
const Json &unit(const CanonicalEventExecutionIr &ir) {
    const auto *value=ir.root().member("unit");
    if(!value||value->type!=Json::Type::Object)fail("EVENT_BODY_SEAL","The sealed Event IR lost its unit.","/unit");
    return *value;
}
const Json &arrayMember(const Json &parent,std::string_view key) {
    const auto *value=parent.member(key);
    if(!value||value->type!=Json::Type::Array)fail("EVENT_BODY_SEAL","The sealed Event IR lost an array.","/unit/"+std::string(key));
    return *value;
}
struct Draft {
    Json tree;
    std::string bytes,documentId,unitId;
    CanonicalIrEventScope scope;
    std::vector<CanonicalIrNode> nodes;
    std::vector<CanonicalIrConnection> edges;
    std::vector<CanonicalIrSourceMap> maps;
    std::vector<CanonicalIrCorrespondence> correspondence;
    CanonicalIrRegion region;
};
Draft project(const CanonicalEventExecutionIr &ir,std::string_view sourceRegionId,const CanonicalIrReadOptions &options) {
    corridor(ir);
    const auto found=std::find_if(ir.regions().begin(),ir.regions().end(),[&](const auto &r){return r.sourceRegionId==sourceRegionId;});
    if(found==ir.regions().end())fail("EVENT_BODY_REGION_MISSING","The exact persistent region identity is absent from this Event Source.","/diagram");
    const auto &selected=*found;Draft out;
    out.documentId="event-body-document:"+hashing::sha256Hex(std::string(ir.documentId())+"\n"+selected.id);
    out.unitId="unit:event-body:"+hashing::sha256Hex(selected.id);
    if(out.documentId.size()>options.limits.maximumSourceIdBytes||out.unitId.size()>options.limits.maximumIrIdBytes)
        fail("EVENT_BODY_RESOURCE_LIMIT","Derived body identities exceed the requested byte budgets.","/document_id",true);
    const auto *diagram=ir.authoritativeSource().section("diagram");
    const auto *outer=diagram?diagram->member("nodes"):nullptr;
    if(!outer||outer->type!=Json::Type::Array)fail("EVENT_BODY_SEAL","Authoritative Event Source lost its diagram.","/diagram");
    for(const auto &node:outer->array)if(const auto *type=node.member("structure_type");type&&type->type==Json::Type::String&&type->string=="event_structure") {
        if(!out.scope.sourceStructureId.empty())fail("EVENT_BODY_SEAL","The sealed single-Event profile changed structure count.","/diagram/nodes");
        out.scope.sourceStructureId=text(node.member("id"),"/diagram/nodes");
    }
    if(out.scope.sourceStructureId.empty())fail("EVENT_BODY_SEAL","Authoritative Event structure identity is absent.","/diagram/nodes");
    out.scope.sourceRegionId=selected.sourceRegionId;out.scope.regionId=selected.id;
    out.region={selected.id,selected.sourceRegionId,{}, {},"event_body",location(selected.sourceRef)};
    const auto &input=unit(ir);std::map<std::string,const Json *> objects;
    for(const auto &object:arrayMember(input,"objects").array)objects.emplace(text(object.member("id"),"/unit/objects"),&object);
    std::set<std::string> targets{selected.id};Json bodyObjects=array(),bodyConnections=array();
    std::set<std::string> actualWidgetIds;
    for(const auto &widget:ir.widgets())actualWidgetIds.insert(widget.id);
    for(const auto &node:selected.nodes) {
        CanonicalIrNode common;common.id=node.id;common.sourceNodeId=node.sourceNodeId;common.kind=node.kind;
        common.role=node.role;common.ports=node.ports;common.regionId=selected.id;common.valueType=node.typeRef;
        const auto raw=objects.find(node.id);if(raw==objects.end())fail("EVENT_BODY_SEAL","An actual body node is absent from the sealed open IR.",location(node.sourceRef));
        Json object=*raw->second;auto *attrs=&object.object.at("attributes");
        if(node.role=="event_data") {
            common.family="event_input";common.eventFieldId=node.fieldId;
            common.widgetId="event-input:"+node.id;
            if(actualWidgetIds.contains(common.widgetId))fail("EVENT_BODY_INPUT_COLLISION","An Event support binding collides with an actual widget.",location(node.sourceRef));
            object.object["family"]=string(common.family);
            attrs->object["supplied_by_event"]=boolean(true);
            attrs->object["event_data_field"]=string(node.fieldId);
            attrs->object.erase("default_value");
        } else if(node.family=="widget_value") {
            common.family=node.family;common.widgetId=node.widgetId;
            common.widgetClass=text(attrs->member("widget_class"),location(node.sourceRef));
            if(const auto *value=attrs->member("default_value"))common.defaultValueJson=encode(*value);
            if(node.role=="control"&&!common.defaultValueJson)fail("EVENT_BODY_SEAL","An actual control lost its exact Source default.",location(node.sourceRef));
            if(node.defaultValue)if(const auto *number=std::get_if<double>(&*node.defaultValue))common.defaultValue=*number;
        } else common.family=node.family;
        attrs->object["region_id"]=string(selected.id);
        out.region.memberNodeIds.push_back(node.id);targets.insert(node.id);out.nodes.push_back(std::move(common));bodyObjects.array.push_back(std::move(object));
    }
    std::map<std::string,const Json *> connections;
    for(const auto &connection:arrayMember(input,"connections").array)connections.emplace(text(connection.member("id"),"/unit/connections"),&connection);
    for(const auto &edge:selected.connections) {
        const auto raw=connections.find(edge.id);if(raw==connections.end())fail("EVENT_BODY_SEAL","An actual body edge is absent from the sealed open IR.",location(edge.sourceRef));
        out.edges.push_back({edge.id,{},edge.from,edge.to});
        // sourceEdgeId is recovered from the actual Source edge declaration,
        // not guessed by decoding an IR ID or from its routing geometry.
        const auto path=location(edge.sourceRef);const Json *source=&ir.authoritativeSource().root();
        // The Source view owns exact JSON Pointer ranges. Its edge path is a
        // known nested diagram path from the already validated Event IR.
        for(const auto &scope:ir.authoritativeSource().diagramScopes())if(path.starts_with(scope.path+"/edges/")) {
            const auto *edges=scope.diagram->member("edges");if(!edges)continue;
            for(const auto &candidate:edges->array)if(candidate.sourceBegin!=std::string_view::npos) {
                const auto *id=candidate.member("id");
                // Preserve the exact ID by selecting the Source array index.
                const auto index=std::size_t(&candidate-edges->array.data());
                if(scope.path+"/edges/"+std::to_string(index)==path)source=&candidate;
            }
        }
        out.edges.back().sourceEdgeId=text(source->member("id"),path);
        targets.insert(edge.id);bodyConnections.array.push_back(*raw->second);
    }
    Json maps=array(),relations=array();
    for(const auto &original:arrayMember(input,"source_map").array) {
        Json record=original;Json refs=array();
        for(const auto &target:arrayMember(original,"target_refs").array)if(targets.contains(text(&target,"/unit/source_map")))refs.array.push_back(target);
        record.object["target_refs"]=refs;
        if(refs.array.empty()){record.object["target_kind"]=string("none");record.object["relation"]=string("non_primary");}
        CanonicalIrSourceMap map{text(record.member("id"),"/unit/source_map"),text(record.member("source_kind"),"/unit/source_map"),text(record.member("source_ref"),"/unit/source_map"),text(record.member("target_kind"),"/unit/source_map"),text(record.member("relation"),"/unit/source_map"),{}};
        for(const auto &target:refs.array)map.targetRefs.push_back(target.string);
        out.maps.push_back(std::move(map));maps.array.push_back(std::move(record));
    }
    for(const auto &original:arrayMember(input,"correspondence").array) {
        Json record=original;Json refs=array();
        for(const auto &target:arrayMember(original,"target_refs").array)if(targets.contains(text(&target,"/unit/correspondence")))refs.array.push_back(target);
        record.object["target_refs"]=refs;
        if(refs.array.empty())record.object["relation_kind"]=string("non_primary_outcome");
        CanonicalIrCorrespondence relation{text(record.member("id"),"/unit/correspondence"),text(record.member("source_ref"),"/unit/correspondence"),text(record.member("relation_kind"),"/unit/correspondence"),{}};
        for(const auto &target:refs.array)relation.targetRefs.push_back(target.string);
        out.correspondence.push_back(std::move(relation));relations.array.push_back(std::move(record));
    }
    Json rootRegion=object();rootRegion.object["id"]=string(selected.id);rootRegion.object["owner_object_id"]=string("");rootRegion.object["role"]=string("event_body");
    for(const auto &original:arrayMember(input,"regions").array)if(text(original.member("id"),"/unit/regions")==selected.id)
        if(const auto *attribution=original.member("attribution"))rootRegion.object["attribution"]=*attribution;
    Json members=array();for(const auto &id:out.region.memberNodeIds)members.array.push_back(string(id));rootRegion.object["member_object_ids"]=std::move(members);
    Json scope=object();scope.object["source_structure_id"]=string(out.scope.sourceStructureId);scope.object["source_region_id"]=string(selected.sourceRegionId);scope.object["region_id"]=string(selected.id);
    Json metadata=object();metadata.object["producer"]=string("frog.native.canonical-event-body-fir");metadata.object["producer_version"]=string("1");metadata.object["profile"]=string(std::string(canonicalEventRegionBodyProfile));metadata.object["body_scope"]=std::move(scope);metadata.object["source_event_document_id"]=string(std::string(ir.documentId()));metadata.object["source_sha256"]=string(std::string(ir.sourceHash()));metadata.object["source_uri"]=string(std::string(ir.sourceUri()));
    Json bodyUnit=object();bodyUnit.object["id"]=string(out.unitId);bodyUnit.object["objects"]=std::move(bodyObjects);bodyUnit.object["connections"]=std::move(bodyConnections);bodyUnit.object["regions"]=array();bodyUnit.object["regions"].array.push_back(std::move(rootRegion));bodyUnit.object["source_map"]=std::move(maps);bodyUnit.object["correspondence"]=std::move(relations);
    out.tree=object();out.tree.object["schema_family"]=string("frog.execution_ir");out.tree.object["schema_version"]=string("0.1");out.tree.object["document_id"]=string(out.documentId);out.tree.object["metadata"]=std::move(metadata);out.tree.object["unit"]=std::move(bodyUnit);out.bytes=encode(out.tree);
    if(out.bytes.size()>options.limits.maximumIrBytes)fail("EVENT_BODY_RESOURCE_LIMIT","The common Event body FIR exceeds its requested byte budget.","/",true);
    return out;
}
}
std::optional<CanonicalExecutionIr> CanonicalExecutionIr::deriveEventRegion(const CanonicalEventExecutionIr &ir,
    std::string_view sourceRegionId,const CanonicalIrReadOptions &options,std::vector<CanonicalIrDiagnostic> *diagnostics) {
    if(diagnostics)diagnostics->clear();
    try {
        policy(options);
        if(options.sourceUri!=ir.sourceUri())fail("EVENT_BODY_SOURCE_URI","A body projection cannot change the authoritative Event Source URI.","/");
        // Respect reduced caller budgets against the COMPLETE Source/Event IR,
        // including regions and payload fields not projected into this body.
        std::optional<CanonicalEventExecutionIr> checked;
        const CanonicalEventExecutionIr *owner=&ir;
        if(!ir.admittedWithin(options.limits)) {
            checked=CanonicalEventExecutionIr::read(ir.originalUtf8(),ir.authoritativeSource(),options,diagnostics);
            if(!checked)return {};
            owner=&*checked;
        }
        auto draft=project(*owner,sourceRegionId,options);auto storage=std::make_shared<Storage>(owner->authoritativeSource());
        storage->original=std::move(draft.bytes);storage->tree=std::move(draft.tree);storage->documentId=std::move(draft.documentId);storage->unitId=std::move(draft.unitId);storage->sourceUri=owner->sourceUri();storage->sourceHash=owner->sourceHash();
        storage->nodes=std::move(draft.nodes);storage->edges=std::move(draft.edges);storage->maps=std::move(draft.maps);storage->correspondence=std::move(draft.correspondence);storage->regions.push_back(std::move(draft.region));storage->eventScope=std::move(draft.scope);
        return CanonicalExecutionIr{std::move(storage)};
    } catch(const Failure &error){if(diagnostics)diagnostics->push_back(error.diagnostic);return {};}
}
std::optional<CanonicalExecutionIr> CanonicalExecutionIr::readEventRegion(std::string_view bytes,const CanonicalEventExecutionIr &ir,
    std::string_view sourceRegionId,const CanonicalIrReadOptions &options,std::vector<CanonicalIrDiagnostic> *diagnostics) {
    if(diagnostics)diagnostics->clear();
    try {
        policy(options);
        if(bytes.size()>options.limits.maximumIrBytes)fail("EVENT_BODY_RESOURCE_LIMIT","The candidate common body FIR exceeds its requested byte budget.","/",true);
        std::string error;const auto tree=source::parseJson(bytes,&error,source::JsonNumberPolicy::PreserveLexeme);
        if(!tree)fail("EVENT_BODY_JSON",std::move(error),"/");
        auto expected=deriveEventRegion(ir,sourceRegionId,options,diagnostics);if(!expected)return {};
        if(!exact(*tree,expected->root()))fail("EVENT_BODY_SOURCE_MISMATCH","Body FIR differs from the complete projection of the authoritative Event Source and exact region.","/");
        auto retained=std::make_shared<Storage>(*expected->storage_);retained->original=bytes;retained->tree=*tree;
        return CanonicalExecutionIr{std::move(retained)};
    } catch(const Failure &error){if(diagnostics)diagnostics->push_back(error.diagnostic);return {};}
}
}
