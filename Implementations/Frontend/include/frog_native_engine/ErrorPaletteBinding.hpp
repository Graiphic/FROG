#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FunctionErrorPortLayout.hpp"
#include "frog_native_engine/BooleanConstantGeometry.hpp"
#include "frog_native_engine/RetainFirstErrorVisualLayout.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>

namespace frog::native_engine {
inline constexpr std::string_view kErrorClusterType = "cluster<status:bool,code:i32,source:string>";
// Expanded Error constants use compact editable fields, independently of the
// larger standalone palette defaults. Keep a clear gutter around each field.
inline constexpr double kErrorConstantPadding = 2.0;
inline constexpr std::array<DiagramElementGeometry,3> kErrorConstantFieldGeometry{
    DiagramElementGeometry{kBooleanConstantWidth,kBooleanConstantHeight,kBooleanConstantWidth,kBooleanConstantHeight},
    DiagramElementGeometry{kBooleanConstantWidth,24.0,24.0,24.0},
    DiagramElementGeometry{kBooleanConstantWidth,24.0,24.0,24.0}
};
inline constexpr double kErrorConstantWidth = kBooleanConstantWidth + 2 * kErrorConstantPadding;
inline constexpr double kErrorConstantHeight = kBooleanConstantHeight + 48.0 + 4 * kErrorConstantPadding;
inline std::string_view errorPaletteId(std::string_view id) {
    if(id.starts_with("frog.studio.")) id.remove_prefix(12);
    return id;
}
inline bool isErrorPaletteFunction(std::string_view id) {
    id=errorPaletteId(id);
    return id.starts_with("error.") && id!="error.constant" && id!="error.terminal";
}
inline bool isRetainFirstError(std::string_view id) { return errorPaletteId(id)=="error.retain_first"; }
inline unsigned errorRetainCount(const DiagramNode &node) {
    return node.retainFirstErrorInputs
        ? std::clamp(*node.retainFirstErrorInputs,2U,static_cast<unsigned>(kDiagramExtensionMaximumInputCount))
        : RetainFirstErrorVisualLayout::countForLegacyHeight(node.layout.height);
}
inline void normalizeRetainFirstErrorLayout(DiagramNode &node) {
    // Capture legacy arity before replacing its height. Subsequent layout,
    // save/load and Undo use the same durable count rather than decoding again.
    node.retainFirstErrorInputs=errorRetainCount(node);
    node.layout.width=RetainFirstErrorVisualLayout::width;
    node.layout.height=RetainFirstErrorVisualLayout(*node.retainFirstErrorInputs).height();
}
inline std::optional<unsigned> errorRetainIndex(std::string_view port) {
    if(port=="a") return 0;
    if(port=="b") return 1;
    if(!port.starts_with("input_")) return {};
    port.remove_prefix(6);
    unsigned n=0; const auto parsed=std::from_chars(port.data(),port.data()+port.size(),n);
    if(port.empty() || port.front()=='0' || parsed.ec!=std::errc{} || parsed.ptr!=port.data()+port.size() || n<3 || n>kDiagramExtensionMaximumInputCount) return {};
    return n-1;
}
struct ErrorPalettePort { std::string id; bool output; double y; double x=-1; };
inline std::vector<ErrorPalettePort> errorPaletteUnalignedPorts(const DiagramNode &node,bool multipleCodes=false) {
    std::vector<ErrorPalettePort> ports;
    if(isRetainFirstError(node.type)) {
        const RetainFirstErrorVisualLayout layout(errorRetainCount(node));
        for(unsigned i=0;i<layout.inputCount;++i)
            ports.push_back({i==0?"a":i==1?"b":"input_"+std::to_string(i+1),false,layout.inputY(i)});
        ports.push_back({"error_out",true,layout.outputY()});
        return ports;
    }
    const auto *definition=findFunctionBindingDefinition(errorPaletteId(node.type));
    if(!definition) return ports;
    for(const auto direction:{FunctionPortDirection::Input,FunctionPortDirection::Output}) {
        unsigned row=0;
        for(const auto &p:definition->ports) if(p.direction==direction) {
            if(p.portId=="codes_cleared" && !multipleCodes) continue;
            ports.push_back({p.portId,direction==FunctionPortDirection::Output,18.0*++row});
        }
    }
    if(definition->ports.size()<=2) for(auto &p:ports) p.y=24;
    alignFunctionErrorPortRows(ports,[](const auto &p)->const auto&{return p.id;},
        [](const auto &p){return p.output;},[](auto &p)->double&{return p.y;});
    return ports;
}
inline double errorPaletteHeight(const DiagramNode &node) {
    if(isRetainFirstError(node.type)) return RetainFirstErrorVisualLayout(errorRetainCount(node)).height();
    double height=48;
    for(const auto &p:errorPaletteUnalignedPorts(node,true)) height=(std::max)(height,p.y+18);
    return height;
}
// Measured NI body contacts; see the Error NI connector report and independent
// test oracle. Keep the established canvas/height and semantic port order.
struct ErrorPaletteContact { std::string_view catalog,port; double x,y; };
inline constexpr ErrorPaletteContact kReviewedErrorContacts[] = {
    {"error.build_cluster","code",0.018828451882845192,0.13912133891213393},
    {"error.build_cluster","description",0.018828451882845192,0.3797071129707113},
    {"error.build_cluster","include_call_chain",0.3797071129707113,0.018828451882845192},
    {"error.build_cluster","make_warning",0.62029288702928864,0.018828451882845192},
    {"error.build_cluster","error_in",0.018828451882845192,0.86087866108786615},
    {"error.build_cluster","error_out",0.9811715481171549,0.86087866108786615},
    {"error.clear_cluster","error_in",0.018828451882845192,0.86087866108786615},
    {"error.clear_cluster","error_out",0.9811715481171549,0.86087866108786615},
    {"error.display","type_of_dialog",0.018828451882845192,0.62029288702928864},
    {"error.display","error_in",0.018828451882845192,0.86087866108786615},
    {"error.display","error",0.9811715481171549,0.13912133891213393},
    {"error.display","code",0.9811715481171549,0.3797071129707113},
    // Official NXG connectors for the GS operations with that declared origin.
    {"error.demultiplex","error_in",0.018828451882845189,0.13912133891213388},
    {"error.demultiplex","errors_out",0.98117154811715468,0.13912133891213388},
    {"error.downgrade_to_warning","error_in",0.018828451882845189,0.13912133891213388},
    {"error.downgrade_to_warning","error_out",0.98117154811715468,0.13912133891213388},
    {"error.is_error_or_warning","error_in",0.018828451882845189,0.13912133891213388},
    {"error.is_error_or_warning","result",0.98117154811715468,0.13912133891213388},
    {"error.is_error","error_in",0.018828451882845189,0.13912133891213388},
    {"error.is_error","result",0.98117154811715468,0.13912133891213388},
    {"error.is_warning","error_in",0.018828451882845189,0.13912133891213388},
    {"error.is_warning","result",0.98117154811715468,0.13912133891213388},
    {"error.multiplex","error_in_1",0.018828451882845189,0.13912133891213388},
    {"error.multiplex","error_in_2",0.018828451882845189,0.37970711297071125},
    {"error.multiplex","error_in_3",0.018828451882845189,0.62029288702928864},
    {"error.multiplex","error_array_in",0.018828451882845189,0.86087866108786615},
    {"error.multiplex","error_out",0.98117154811715468,0.13912133891213388},
    {"error.read_call_chain","error_in",0.018828451882845189,0.13912133891213388},
    {"error.read_call_chain","call_chain",0.98117154811715468,0.13912133891213388},
    {"error.read_call_chain","has_location",0.98117154811715468,0.37970711297071125},
    {"error.read_code","error_in",0.018828451882845189,0.13912133891213388},
    {"error.read_code","code",0.98117154811715468,0.13912133891213388},
    {"error.read_location","error_in",0.018828451882845189,0.13912133891213388},
    {"error.read_location","location",0.98117154811715468,0.13912133891213388},
    {"error.upgrade_to_error","error_in",0.018828451882845189,0.13912133891213388},
    {"error.upgrade_to_error","error_out",0.98117154811715468,0.13912133891213388},
    {"error.write_code","error_in",0.018828451882845189,0.13912133891213388},
    {"error.write_code","code",0.018828451882845189,0.37970711297071125},
    {"error.write_code","error_out",0.98117154811715468,0.13912133891213388},
};
inline bool hasReviewedErrorPaletteLayout(std::string_view id) {
    id=errorPaletteId(id);
    return std::any_of(std::begin(kReviewedErrorContacts),std::end(kReviewedErrorContacts),
        [id](const auto &p){return p.catalog==id;});
}
inline std::vector<ErrorPalettePort> errorPalettePorts(const DiagramNode &node,bool multipleCodes=false) {
    auto ports=errorPaletteUnalignedPorts(node,multipleCodes);
    const auto id=errorPaletteId(node.type);
    const double height=errorPaletteHeight(node);
    for(auto &port:ports)for(const auto &contact:kReviewedErrorContacts)
        if(contact.catalog==id && contact.port==port.id) {
            port.x=contact.x;port.y=contact.y*height;break;
        }
    return ports;
}
// A literal preset uses the existing editable cluster fields and persistence.
// It has no special runtime exception semantics.
inline std::vector<DiagramNode> errorConstantFields(DiagramNode &parent) {
    parent.valueType=kErrorClusterType;
    parent.clusterConstantCollapsed=false;
    parent.clusterConstantAutoSizing="vertical";
    parent.clusterConstantExpandedWidth=kErrorConstantWidth;
    parent.clusterConstantExpandedHeight=kErrorConstantHeight;
    parent.layout.width=kErrorConstantWidth; parent.layout.height=kErrorConstantHeight;
    std::vector<DiagramNode> fields;
    const std::array<std::string_view,3> names{"status","code","source"};
    const std::array<std::string_view,3> types{"bool","i32","string"};
    const std::array<std::string_view,3> nodes{"frog.studio.boolean.false_constant","frog.studio.numeric.numeric_constant","frog.studio.string.constant"};
    double y=parent.layout.y+kErrorConstantPadding;
    for(unsigned i=0;i<3;++i) {
        DiagramNode field; field.kind="primitive"; field.type=nodes[i];
        field.id=parent.id+"."+std::string(names[i]);
        field.constantOwnerNodeId=parent.id; field.constantFieldId=names[i]; field.constantFieldOrder=i;
        field.valueType=types[i]; field.text=i==0?"false":i==1?"0":"";
        field.label=names[i]; field.labelVisible=false; field.labelWidth=54; field.labelHeight=22;
        const auto geometry=kErrorConstantFieldGeometry[i];
        field.layout={parent.layout.x+kErrorConstantPadding,y,geometry.naturalWidth,geometry.naturalHeight};
        y+=geometry.naturalHeight+kErrorConstantPadding;
        field.fontPointSize=documentDefaultTextPointSize();
        field.diagramOwnerStructureId=parent.diagramOwnerStructureId;
        field.diagramOwnerRegionId=parent.diagramOwnerRegionId;
        fields.push_back(std::move(field));
    }
    return fields;
}
}
