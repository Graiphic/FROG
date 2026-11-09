#include "frog_native_engine/graph/XYGraphReferenceContract.hpp"
#include <algorithm>
#include <utility>

namespace frog::native_engine::xy_reference {
using Type=binding::TypeRef;
std::vector<ApplicationControlMemberDescriptor> members(){
    std::vector<ApplicationControlMemberDescriptor> result;
    const auto property=[&](std::string id,std::string label,Type type,bool writable){ApplicationControlMemberDescriptor m;m.id=std::move(id);m.label=std::move(label);m.type=std::move(type);m.readable=true;m.writable=writable;m.authoringAvailable=m.runtimeAvailable=true;m.group="Graph XY";m.help="Stable IDs identify plots, axes, cursors and annotations. JSON edits use the shared validated Graph XY configuration; Runtime edits remain instance-owned.";result.push_back(std::move(m));};
    property("xy.configuration","Configuration (JSON)",Type::primitive("string"),true);
    property("xy.view","Current View (JSON)",Type::primitive("string"),true);
    property("xy.plot_count","Present Plot Count",Type::primitive("u32"),false);
    property("xy.plot_area","Local Plot Area",Type::cluster({"x","y","width","height"},{Type::primitive("f64"),Type::primitive("f64"),Type::primitive("f64"),Type::primitive("f64")}),false);
    for(const auto name:{"plots","axes","cursors","annotations","plane"})property("xy."+std::string(name),std::string(name)+" (JSON)",Type::primitive("string"),true);
    const auto method=[&](std::string id,std::string label,bool options,bool output){ApplicationControlMemberDescriptor m;m.id=std::move(id);m.label=std::move(label);m.kind=ApplicationControlMemberKind::Method;m.authoringAvailable=m.runtimeAvailable=true;m.group="Graph XY";m.help="Options are an explicit JSON object with stable target IDs. Invalid targets or ranges fail atomically; data and presentation are separate effects.";if(options){const bool required=m.id!="xy.fit"&&m.id!="xy.export_data";m.parameters.push_back({"options","Options (JSON)",Type::primitive("string"),ApplicationControlParameterDirection::In,required,required?std::nullopt:std::optional<std::string>{"\"{}\""}});}if(output)m.resultType=Type::primitive("string");result.push_back(std::move(m));};
    method("xy.fit","Fit Scales",true,false);method("xy.clear","Clear Data",false,false);
    method("xy.data_to_plot","Data To Local Coordinates",true,true);method("xy.plot_to_data","Local Coordinates To Data",true,true);
    method("xy.find_point","Find Source Point",true,true);method("xy.export_data","Export Data (CSV/TSV)",true,true);
    method("xy.export_image","Export Image (PNG Data URL)",true,true);
    result.back().authoringAvailable=false;
    result.back().help="Returns PNG bytes as a data:image/png;base64 String through an installed detached native drawing service. At most one million pixels; the complete String is limited to 1,000,000 bytes (PNG at most 749,982 bytes). No file is written. A host without this service refuses admission explicitly.";
    method("xy.cursor.add","Add Cursor",true,true);method("xy.cursor.remove","Remove Cursor",true,false);
    method("xy.cursor.center","Bring Cursor To Centre",true,false);method("xy.cursor.go_to","Centre View On Cursor",true,false);
    method("xy.cursor.previous","Previous Source Point",true,false);method("xy.cursor.next","Next Source Point",true,false);
    method("xy.annotation.add","Add Annotation",true,true);method("xy.annotation.remove","Remove Annotation",true,false);
    method("xy.annotation.clear","Remove All Annotations",false,false);return result;
}
bool supported(std::string_view id){const auto all=members();return std::any_of(all.begin(),all.end(),[&](const auto &m){return m.id==id;});}
std::optional<ApplicationControlMemberDescriptor> member(std::string_view id) {
    for(auto &descriptor:members())if(descriptor.id==id)return std::move(descriptor);
    return {};
}
} // namespace frog::native_engine::xy_reference
