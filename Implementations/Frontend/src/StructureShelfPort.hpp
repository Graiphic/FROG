#pragma once
#include "frog_native_engine/DiagramStructureShelfCatalog.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/EventStructureSchema.hpp"
#include <optional>

namespace frog::native_engine {
struct StructureShelfPort {FunctionPortDirection direction;std::string_view type;};
inline std::optional<StructureShelfPort> structureShelfPort(const DiagramNode &n,std::string_view port) {
    if(n.type=="frog.studio.structures.timed_loop") {
        if(port=="iteration")return StructureShelfPort{FunctionPortDirection::Output,"i64"};
        if(port=="condition")return StructureShelfPort{FunctionPortDirection::Input,"bool"};
    }
    port=port.substr(0,port.find('#'));
    if(n.type=="frog.studio.structures.event_structure" &&
       (port=="event_response.discard" || port=="event_response.silent_shutdown"))
        return StructureShelfPort{FunctionPortDirection::Input,"bool"};
    if(n.type=="frog.studio.structures.event_structure" && port.starts_with("event_response.") &&
       isEventAuthoringResponseField(port.substr(15)))
        // Direction is stable; only the actual case's validated schema may
        // supply the type and permission to expose/write this response.
        return StructureShelfPort{FunctionPortDirection::Input,"unknown"};
    if(n.type=="frog.studio.structures.event_structure" && port.starts_with("event_data.")) {
        if(const auto *field=diagramStructureShelfFieldById(diagramEventStructureDataFields(),port.substr(11)))
            return StructureShelfPort{FunctionPortDirection::Output,field->valueType};
        // These authoring identities are typed only with an explicit Event
        // region and source schema by TypeResolver, never from this catalog.
        if(isEventAuthoringDataField(port.substr(11)))
            return StructureShelfPort{FunctionPortDirection::Output,"unknown"};
    }
    if(n.type=="frog.studio.structures.timed_loop")for(const auto &s:kDiagramTimedLoopTimingNodes)
        if(port.starts_with(s.portPrefix))if(const auto *field=diagramStructureShelfFieldById(s.fields,port.substr(s.portPrefix.size())))
            return StructureShelfPort{s.direction=="input"?FunctionPortDirection::Input:FunctionPortDirection::Output,field->valueType};
    return {};
}
}
