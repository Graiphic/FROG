#include "FrogDocumentApplicationControlSerialization.hpp"
#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
namespace frog::native_engine::document_internal {
void appendApplicationControl(std::ostringstream &out,const ApplicationControlNode &cfg,int indent){
    application_control_source::appendConfiguration(out,cfg,indent);
}
bool readApplicationControl(const JsonValue &node,DiagramNode &output,std::string *error){
    const auto *value=node.member("studio_application_control");if(!value)return true;
    std::string why;auto decoded=application_control_source::readConfiguration(*value,&why,application_control_source::ReadPolicy::PrivateCompatible);
    const auto fail=[&](std::string message){if(error)*error="Application Control '"+output.id+"': "+message;return false;};
    if(!decoded)return fail(std::move(why));const auto &c=*decoded;
    const auto *entry=findFunctionCatalogEntryByDiagramNodeType(output.type);
    const std::string_view catalogId=c.kind==ApplicationControlKind::Reference?"application_control.reference":
        c.kind==ApplicationControlKind::Property?"application_control.property_node":
        c.kind==ApplicationControlKind::Invoke?"application_control.invoke_node":"application_control.close_reference";
    if(output.kind!="primitive" || !entry || entry->catalogId!=catalogId)
        return fail("Private operation configuration does not match the node type.");
    output.applicationControl=std::move(*decoded);return true;
}
}
