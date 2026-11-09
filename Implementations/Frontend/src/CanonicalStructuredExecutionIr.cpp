#include "frog_native_engine/CanonicalFrontendMetrics.hpp"
#include "frog_native_engine/ContextPathIdentity.hpp"
#include "CanonicalStructuredExecutionIr.hpp"
#include "CanonicalComposedExecutionIr.hpp"
#include "frog_native_engine/CanonicalRecordConstant.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/CanonicalScalarSelection.hpp"
#include "frog_native_engine/CanonicalStringNative.hpp"
#include "frog_native_engine/CanonicalArrayNative.hpp"
#include "frog_native_engine/CanonicalNativeCallScalar.hpp"
#include "frog_native_engine/ExponentialFunctionIdentity.hpp"
#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/HyperbolicFunctionIdentity.hpp"
#include "frog_native_engine/GatingFunctionIdentity.hpp"
#include "frog_native_engine/DiscreteMathFunctionIdentity.hpp"
#include "frog_native_engine/TrigonometricFunctionIdentity.hpp"
#include "frog_native_engine/BesselFunctionIdentity.hpp"
#include "frog_native_engine/GammaFunctionIdentity.hpp"
#include "frog_native_engine/HypergeometricFunctionIdentity.hpp"
#include "frog_native_engine/EllipticFunctionIdentity.hpp"
#include "frog_native_engine/ExponentialIntegralsFunctionIdentity.hpp"
#include "frog_native_engine/ErrorFunctionsFunctionIdentity.hpp"
#include "frog_native_engine/EllipticParabolicFunctionIdentity.hpp"
#include "frog_native_engine/DiagramTunnelIdentity.hpp"
#include "frog_native_engine/ButtonMechanicalAction.hpp"
#include <algorithm>
#include <deque>
#include <set>

namespace frog::native_engine::structured_ir {
namespace {
using namespace canonical_json;
struct Failure {CanonicalIrDiagnostic diagnostic;};
constexpr std::string_view scalarConstantProfile="frog.studio.scalar-constant@1";
bool recordCapabilityFailure(std::string_view code) noexcept {
    return code=="IR_RECORD_TRANSPORT_UNSUPPORTED"||code=="IR_RECORD_RESOURCE_LIMIT";
}
[[noreturn]] void fail(std::string code,std::string message,std::string path,bool unsupported=false){throw Failure{{std::move(code),std::move(message),std::move(path),unsupported}};}
const Json &obj(const Json *v,const std::string &p){if(!v||v->type!=Json::Type::Object)fail("IR_OBJECT","An explicit object is required.",p);return *v;}
const std::vector<Json> &arr(const Json *v,const std::string &p){if(!v||v->type!=Json::Type::Array)fail("IR_ARRAY","An explicit array is required.",p);return v->array;}
std::string text(const Json *v,const std::string &p){if(!v||v->type!=Json::Type::String||v->string.empty())fail("IR_STRING","A nonempty string is required.",p);return v->string;}
std::optional<std::string> caseInteger(const Json *value,std::string_view type) {
    if(!value||value->type!=Json::Type::String||value->string.empty())return {};
    const auto &text=value->string;const std::size_t first=text.front()=='-'?1U:0U;
    if(first==text.size()||!std::all_of(text.begin()+first,text.end(),[](char c){return c>='0'&&c<='9';}))return {};
    if(type=="i64"){std::int64_t n=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),n);if(r.ec==std::errc{}&&r.ptr==text.data()+text.size())return std::to_string(n);}
    else if(type=="u64"){std::uint64_t n=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),n);if(r.ec==std::errc{}&&r.ptr==text.data()+text.size())return std::to_string(n);}
    return {};
}
void fields(const Json &v,std::initializer_list<std::string_view> names,const std::string &p){
    for(const auto &[key,_]:v.object)if(std::find(names.begin(),names.end(),key)==names.end())fail("IR_UNSUPPORTED_FIELD","No admitted semantics for field: "+key,p,true);
}
bool flag(const Json *v,bool expected,const std::string &p){if(!v||v->type!=Json::Type::Boolean||v->boolean!=expected)fail("IR_TERMINAL_CONTRACT","Structure terminal differs from its public contract.",p);return expected;}
std::string identity(const Json *v,const std::string &p,std::size_t max){auto id=text(v,p);if(id.size()>max)fail("IR_RESOURCE_LIMIT","Source identity exceeds its byte budget.",p,true);return id;}
Json strings(const std::vector<std::string> &values){auto a=array();for(const auto &v:values)a.array.push_back(string(v));return a;}
Json attribution(const std::vector<std::string> &refs){auto a=object();a.object={{"source_refs",strings(refs)},{"mode",string(refs.size()==1?"direct":"multi_contributor")}};return a;}
struct SourceNode {const Json *raw;std::string pointer,region;};
struct SourceEdge {const Json *raw;std::string pointer,region,owner;};
struct Widget {const Json *raw;std::string pointer,type,klass,role;std::optional<std::string> initial;};
std::vector<CanonicalIrPort> discreteMathPorts(std::string_view identity) {
    if(identity=="frog.studio.math.discrete.gcd")return {{"x",true,"i32"},{"y",true,"i32"},{"gcd",false,"i32"}};
    if(identity=="frog.studio.math.discrete.lcm")return {{"x",true,"i32"},{"y",true,"i32"},{"lcm",false,"i64"}};
    if(identity=="frog.studio.math.discrete.factorial")return {{"n",true,"i32"},{"result",false,"f64"}};
    if(identity=="frog.studio.math.discrete.prime_factor")return {{"n",true,"i64"},{"factors",false,"array<i64,rank=1>"}};
    if(identity=="frog.studio.math.discrete.check_prime")return {{"n",true,"i64"},{"prime",false,"bool"}};
    if(identity=="frog.studio.math.discrete.permute")return {{"n",true,"u32"},{"k",true,"u32"},{"result",false,"f64"}};
    if(identity=="frog.studio.math.discrete.binomial")return {{"n",true,"u32"},{"k",true,"u32"},{"coefficient",false,"f64"},{"error",false,"i32"}};
    if(identity=="frog.studio.math.discrete.continued_fraction")return {{"A",true,"array<f64,rank=1>"},{"B",true,"array<f64,rank=1>"},{"result",false,"f64"},{"error",false,"i32"}};
    return {};
}
std::vector<CanonicalIrPort> besselPorts(std::string_view identity) {
    if(identity=="frog.studio.math.special.airy_ai_bi")return {{"x",true,"f64"},{"ai",false,"f64"},{"bi",false,"f64"}};
    const bool integerOrder=identity=="frog.studio.math.special.bessel_mod_k"||identity=="frog.studio.math.special.bessel_sph_j"||identity=="frog.studio.math.special.bessel_sph_y";
    return {{"x",true,"f64"},{integerOrder?"n":"v",true,integerOrder?"i32":"f64"},{"value",false,"f64"}};
}
std::vector<CanonicalIrPort> gammaPorts(std::string_view identity) {
    if(identity=="frog.studio.math.gamma.log_factorial")return {{"n",true,"i32"},{"value",false,"f64"}};
    if(identity=="frog.studio.math.beta.complete")return {{"x",true,"f64"},{"y",true,"f64"},{"value",false,"f64"}};
    if(identity=="frog.studio.math.beta.regularized_lower")return {{"x",true,"f64"},{"y",true,"f64"},{"a",true,"f64"},{"value",false,"f64"}};
    if(canonicalGammaInfiniteLimit(identity,"a"))return {{"x",true,"f64"},{"a",true,"f64"},{"value",false,"f64"}};
    return {{"x",true,"f64"},{"value",false,"f64"}};
}
std::vector<CanonicalIrPort> hypergeometricPorts(std::string_view identity) {
    if(identity==hypergeometricOperationIds[0])return {{"x",true,"f64"},{"a",true,"f64"},{"b",true,"f64"},{"c",true,"f64"},{"value",false,"f64"}};
    return {{"x",true,"f64"},{"a",true,"f64"},{"b",true,"f64"},{"value",false,"f64"}};
}
std::vector<CanonicalIrPort> ellipticPorts(std::string_view identity) {
    if(canonicalEllipticOptionalInput(identity,"a"))return {{"k",true,"f64"},{"a",true,"f64"},{"value",false,"f64"}};
    return {{"k",true,"f64"},{"value",false,"f64"}};
}
std::vector<CanonicalIrPort> ellipticParabolicPorts(std::string_view identity) {
    if(identity==ellipticParabolicOperationIds[0])return {{"argument",true,"f64"},{"parameter",true,"f64"},{"cn",false,"f64"},{"dn",false,"f64"},{"sn",false,"f64"},{"amplitude",false,"f64"}};
    return {{"argument",true,"f64"},{"order",true,"f64"},{"value",false,"f64"}};
}
std::vector<CanonicalIrPort> errorFunctionsPorts(std::string_view identity) {
    if(identity==errorFunctionsOperationIds[5])return {{"x",true,"f64"},{"cosine_integral",false,"f64"},{"sine_integral",false,"f64"}};
    return {{"x",true,"f64"},{"result",false,"f64"}};
}
std::vector<CanonicalIrPort> exponentialIntegralsPorts(std::string_view identity) {
    if(identity==exponentialIntegralsOperationIds[0])return {{"x",true,"f64"},{"n",true,"i32"},{"result",false,"f64"}};
    return {{"x",true,"f64"},{"result",false,"f64"}};
}
bool gammaInfiniteLimitWidget(const Json &scope,std::string_view widget,bool &found) {
    const auto *nodes=scope.member("nodes"),*edges=scope.member("edges");
    if(!nodes||nodes->type!=Json::Type::Array)return true;
    for(const auto &node:nodes->array){
        const auto *kind=node.member("kind"),*wid=node.member("widget"),*id=node.member("id");
        if(kind&&kind->type==Json::Type::String&&kind->string=="widget_value"&&wid&&wid->type==Json::Type::String&&wid->string==widget){
            if(!id||id->type!=Json::Type::String||!edges||edges->type!=Json::Type::Array)return false;
            bool connected=false;
            for(const auto &edge:edges->array){const auto *from=edge.member("from"),*to=edge.member("to");
                if(!from||!from->member("node")||from->member("node")->string!=id->string)continue;
                if(!from->member("port")||from->member("port")->string!="value"||!to||!to->member("node")||!to->member("port")||to->member("port")->string!="a")return false;
                const auto target=std::find_if(nodes->array.begin(),nodes->array.end(),[&](const auto &other){const auto *otherId=other.member("id");return otherId&&otherId->string==to->member("node")->string;});
                if(target==nodes->array.end()||!target->member("type")||!canonicalGammaInfiniteLimit(target->member("type")->string,"a"))return false;
                connected=true;
            }
            if(!connected)return false;found=true;
        }
        if(const auto *regions=node.member("regions");regions&&regions->type==Json::Type::Array)
            for(const auto &region:regions->array)if(const auto *diagram=region.member("diagram"))if(!gammaInfiniteLimitWidget(*diagram,widget,found))return false;
    }
    return true;
}
void discreteArrayCapability(const Json &scope,const std::string &path) {
    const auto *nodes=scope.member("nodes");if(!nodes||nodes->type!=Json::Type::Array)return;
    for(std::size_t i=0;i<nodes->array.size();++i) {
        const auto &node=nodes->array[i];const auto at=path+"/nodes/"+std::to_string(i);
        const auto *kind=node.member("kind"),*type=node.member("type");
        if(kind&&kind->type==Json::Type::String&&kind->string=="primitive"&&type&&type->type==Json::Type::String) {
            std::string_view operation=type->string;
            if(operation.starts_with("frog.studio."))operation.remove_prefix(12);
            if(operation.starts_with("math.pde."))
                fail("IR_PDE_AGGREGATE_CAPABILITY_UNAVAILABLE","Partial Differential Equations requires typed PDEProblem/Array/callable transport not admitted by this LLVM target. Explicit Simulation is separate; no fallback was started.",at,true);
            if(operation.starts_with("math.ode.")||operation=="math.dae.radau_iia5"||operation=="math.dae.radau_iia5.reference")
                fail("IR_ODE_AGGREGATE_CAPABILITY_UNAVAILABLE","Ordinary Differential Equations "+type->string+" requires Array, cluster and, for Reference instances, typed callable transport not admitted by the current LLVM execution target. Authored-document execution through the existing native C++ operators is separate; no public Source fallback was started.",at,true);
        }
        if(kind&&kind->type==Json::Type::String&&kind->string=="primitive"&&type&&type->type==Json::Type::String&&
            canonicalDiscreteMathIdentity(type->string)&&!canonicalDiscreteMathScalarIdentity(type->string))
            fail("IR_DISCRETE_ARRAY_CAPABILITY_UNAVAILABLE","Discrete Math "+type->string+" has a fixed Array signature requiring Array transport absent from the current native target; local Highlight is separate and no native fallback is allowed.",at,true);
        if(kind&&kind->type==Json::Type::String&&kind->string=="primitive"&&type&&type->type==Json::Type::String&&canonicalBesselComplexIdentity(type->string))
            fail("IR_BESSEL_COMPLEX_CAPABILITY_UNAVAILABLE","Hankel and Kelvin have mandatory complex f64 output, absent from the current native SDK carrier; no real-only native fallback is permitted.",at,true);
        if(const auto *regions=node.member("regions");regions&&regions->type==Json::Type::Array)
            for(std::size_t r=0;r<regions->array.size();++r)if(const auto *diagram=regions->array[r].member("diagram"))
                discreteArrayCapability(*diagram,at+"/regions/"+std::to_string(r)+"/diagram");
    }
}
struct Builder {
    const FrogSourceDocument &source;const CanonicalIrReadOptions &options;
    Draft out;
    std::vector<SourceNode> sourceNodes;
    std::vector<SourceEdge> sourceEdges;
    std::map<std::string,std::size_t> nodeIndex;
    std::map<std::string,Widget> widgets;
    std::map<std::string,std::string> participants;
    std::set<std::string> edgeIds;
    std::size_t derivedRecordDescriptorBytes=0;
    std::set<std::string> resolvedRecordNodes;
    std::vector<CanonicalIrShiftRegister> registers(const Json &raw,const std::string &path,CanonicalIrNode &node){
        const auto &boundary=obj(raw.member("boundary"),path+"/boundary");fields(boundary,{"inputs","outputs"},path);
        std::map<std::string,CanonicalIrShiftRegister> groups;std::set<std::string> ids;
        for(const auto side:{"inputs","outputs"}){
            const bool output=std::string_view(side)=="outputs";
            const auto &entries=arr(boundary.member(side),path+"/boundary/"+side);
            if(entries.size()>options.limits.maximumEdges)fail("IR_RESOURCE_LIMIT","Too many structure boundary members.",path,true);
            for(std::size_t i=0;i<entries.size();++i){
                const auto at=path+"/boundary/"+side+"/"+std::to_string(i);const auto &entry=obj(&entries[i],at);
                // Other tunnel modes still require their own native lowering.
                if(!entry.member("shift_register_pair_id"))fail("IR_STRUCTURE_BOUNDARY_UNSUPPORTED","This target admits explicit shift-register boundaries only.",at,true);
                if(node.kind=="case")fail("IR_SHIFT_REGISTER_STRUCTURE","A shift register belongs to a For or While loop.",at);
                fields(entry,{"id","type","shift_register_pair_id","shift_register_history_index","tunnel_mode","conditional","use_default_if_unwired"},at);
                const auto id=identity(entry.member("id"),at,options.limits.maximumSourceIdBytes);
                const auto pair=identity(entry.member("shift_register_pair_id"),at,options.limits.maximumSourceIdBytes);
                const auto type=text(entry.member("type"),at);
                if(type!="bool"&&type!="i32"&&type!="u32"&&type!="i64"&&type!="u64"&&type!="f64")
                    fail("IR_SHIFT_REGISTER_TYPE_UNSUPPORTED","This native target carries exact scalar shift-register values; aggregate, String and reference transport require their existing separate target capabilities.",at,true);
                if(!ids.insert(id).second)fail("IR_SHIFT_REGISTER_ID","Boundary tunnel identities must be unique.",at);
                if(const auto *mode=entry.member("tunnel_mode");mode&&(mode->type!=Json::Type::String||mode->string!="last_value"))fail("IR_SHIFT_REGISTER_MODE","A shift register cannot index or concatenate its values.",at);
                for(const auto name:{"conditional","use_default_if_unwired"})if(const auto *value=entry.member(name);value&&(value->type!=Json::Type::Boolean||value->boolean))fail("IR_SHIFT_REGISTER_MODE","A shift register has no conditional/default-if-unwired output mode.",at);
                std::size_t history=0;
                if(const auto *value=entry.member("shift_register_history_index")){
                    if(value->type!=Json::Type::Number)fail("IR_SHIFT_REGISTER_HISTORY","A history index must be a bounded non-negative integer.",at);
                    const auto parsed=std::from_chars(value->string.data(),value->string.data()+value->string.size(),history);
                    if(parsed.ec!=std::errc{}||parsed.ptr!=value->string.data()+value->string.size()||history>=options.limits.maximumNodes)
                        fail("IR_SHIFT_REGISTER_HISTORY","A history index must be a bounded non-negative integer.",at);
                }
                auto &group=groups[pair];group.pairId=pair;
                const auto input=diagram_identity::tunnelInputId(id),read=diagram_identity::tunnelOutputId(id);
                node.ports.push_back({input,true,type});node.ports.push_back({read,false,type});
                if(output){
                    if(history||!group.writePortId.empty())fail("IR_SHIFT_REGISTER_PAIR","Each stack has exactly one right terminal, at history index zero.",at);
                    group.writePortId=input;group.resultPortId=read;
                }else group.stages.push_back({id,input,read,type,history});
            }
        }
        std::vector<CanonicalIrShiftRegister> result;
        for(auto &[_,group]:groups){
            if(group.stages.empty()||group.writePortId.empty())fail("IR_SHIFT_REGISTER_PAIR","An explicit stack needs left history and one right writer.",path);
            std::sort(group.stages.begin(),group.stages.end(),[](const auto &a,const auto &b){return a.historyIndex<b.historyIndex;});
            const auto writer=std::find_if(node.ports.begin(),node.ports.end(),[&](const auto &p){return p.id==group.writePortId;});
            for(std::size_t i=0;i<group.stages.size();++i)if(group.stages[i].historyIndex!=i||group.stages[i].typeRef!=writer->typeRef)
                fail("IR_SHIFT_REGISTER_HISTORY","History indices must be unique and contiguous, and every member must preserve the same exact type.",path);
            result.push_back(std::move(group));
        }
        if(const auto *metadata=raw.member("structure_tunnels");metadata&&!arr(metadata,path).empty()&&result.empty())
            fail("IR_STRUCTURE_BOUNDARY_UNSUPPORTED","Tunnel placement cannot invent an executable boundary.",path,true);
        return result;
    }
    std::string ref(std::string_view p)const{return options.sourceUri+"#"+std::string(p);}
    void mapping(std::string id,std::string kind,std::string p,std::string target,std::string targetKind,std::string relation="direct_primary"){
        out.maps.push_back({std::move(id),std::move(kind),ref(p),std::move(targetKind),std::move(relation),target.empty()?std::vector<std::string>{}:std::vector<std::string>{std::move(target)}});
    }
    void correspondence(std::string id,std::string p,std::string target={},std::string kind="non_primary_outcome"){
        out.correspondence.push_back({std::move(id),ref(p),std::move(kind),target.empty()?std::vector<std::string>{}:std::vector<std::string>{std::move(target)}});
    }
    void declarations(){
        const auto &root=obj(&source.root(),"/");fields(root,{"spec_version","metadata","interface","diagram","front_panel","connector","icon","ide","cache","host","execution_policy"},"/");
        // Diagnose the unavailable operation before its real Array widgets are
        // rejected by this scalar host's declaration corridor.
        if(const auto *diagram=root.member("diagram"))discreteArrayCapability(*diagram,"/diagram");
        const auto &interface=obj(root.member("interface"),"/interface");fields(interface,{"inputs","outputs"},"/interface");
        for(const auto side:{"inputs","outputs"})if(!arr(interface.member(side),"/interface").empty())fail("IR_PUBLIC_INTERFACE_UNSUPPORTED","This corridor needs a host-bound typed widget boundary.","/interface",true);
        if(const auto *policy=root.member("execution_policy")){
            obj(policy,"/execution_policy");fields(*policy,{"version","debugging_allowed"},"/execution_policy");
            if(!policy->member("version")||policy->member("version")->type!=Json::Type::Number||policy->member("version")->string!="1")fail("IR_SOURCE_POLICY_UNSUPPORTED","Only public execution policy version 1 is admitted.","/execution_policy",true);
            if(const auto *allowed=policy->member("debugging_allowed");!allowed||allowed->type!=Json::Type::Boolean)fail("IR_SOURCE_POLICY","debugging_allowed must be explicit Boolean.","/execution_policy");
        }
        // Host policies affect launch/presentation, never the compiled graph.
        // This module does not reinterpret or drop them; the exact source stays owned.
        if(const auto *host=root.member("host")){
            obj(host,"/host");fields(*host,{"version","launch","front_panel_window"},"/host");
            if(!host->member("version")||host->member("version")->type!=Json::Type::Number||host->member("version")->string!="1")fail("IR_SOURCE_POLICY_UNSUPPORTED","Only public host version 1 is admitted.","/host",true);
            if(const auto *launch=host->member("launch")){obj(launch,"/host/launch");fields(*launch,{"trigger"},"/host/launch");const auto trigger=text(launch->member("trigger"),"/host/launch");if(trigger!="manual"&&trigger!="on_open")fail("IR_SOURCE_POLICY","Invalid public launch trigger.","/host/launch");}
            if(const auto *window=host->member("front_panel_window")){
                obj(window,"/host/front_panel_window");fields(*window,{"style","title_mode","custom_title","initial_position","minimum_width","minimum_height","allow_close","allow_resize","allow_minimize","menu_bar_visible","toolbar_visible","vertical_scrollbar_visible","horizontal_scrollbar_visible","scroll_to_origin"},"/host/front_panel_window");
                for(const auto &[key,v]:window->object){
                    if(key=="style"&&(v.type!=Json::Type::String||(v.string!="standard"&&v.string!="dialog")))fail("IR_SOURCE_POLICY","Invalid host window style.","/host/front_panel_window");
                    else if(key=="title_mode"&&(v.type!=Json::Type::String||(v.string!="program_name"&&v.string!="custom")))fail("IR_SOURCE_POLICY","Invalid host title mode.","/host/front_panel_window");
                    else if(key=="initial_position"&&(v.type!=Json::Type::String||(v.string!="unchanged"&&v.string!="centered"&&v.string!="maximized"&&v.string!="minimized"&&v.string!="custom")))fail("IR_SOURCE_POLICY","Invalid initial window position.","/host/front_panel_window");
                    else if((key=="minimum_width"||key=="minimum_height")&&(!typedScalar(v,"f64")||std::stod(v.string)<0))fail("IR_SOURCE_POLICY","Invalid host minimum dimensions.","/host/front_panel_window");
                    else if(key=="custom_title"&&v.type!=Json::Type::String)fail("IR_SOURCE_POLICY","Host title must be text.","/host/front_panel_window");
                    else if((key.starts_with("allow_")||key.ends_with("_visible")||key=="scroll_to_origin")&&v.type!=Json::Type::Boolean)fail("IR_SOURCE_POLICY","Host visibility/action policy must be Boolean.","/host/front_panel_window");
                }
            }
        }
        const auto &panel=obj(root.member("front_panel"),"/front_panel");fields(panel,{"canvas","widgets"},"/front_panel");
        const auto &ws=arr(panel.member("widgets"),"/front_panel/widgets");if(ws.size()>options.limits.maximumWidgets)fail("IR_RESOURCE_LIMIT","Widget budget exceeded.","/front_panel",true);
        for(std::size_t i=0;i<ws.size();++i){
            const auto p="/front_panel/widgets/"+std::to_string(i);const auto &w=obj(&ws[i],p);
            fields(w,{"id","role","widget","class_ref","value_type","layout","props","parts","doc","tags"},p);
            const auto wid=identity(w.member("id"),p,options.limits.maximumSourceIdBytes);
            Widget info{&w,p,text(w.member("value_type"),p),text(w.member("widget")?w.member("widget"):w.member("class_ref"),p),text(w.member("role"),p),{}};
            if(w.member("widget")&&w.member("class_ref")&&text(w.member("class_ref"),p)!=info.klass)fail("IR_SOURCE_CLASS_CONFLICT","Widget class references conflict.",p);
            const bool numeric=info.klass=="frog.widgets.numeric_control"||info.klass=="frog.widgets.numeric_indicator"||info.klass=="frog.ui.standard.numeric_control"||info.klass=="frog.ui.standard.numeric_indicator";
            const bool booleanClass=info.klass=="frog.widgets.boolean_control"||info.klass=="frog.widgets.boolean_indicator"||info.klass=="frog.widgets.button";
            const bool textIndicator=(info.klass=="frog.widgets.string_indicator"&&info.type=="string")||(info.klass=="frog.widgets.path_indicator"&&info.type=="path");
            if(info.klass=="frog.widgets.array_control"||info.klass=="frog.widgets.array_indicator")fail("IR_ARRAY_WIDGET_UNAVAILABLE","The first Array corridor does not admit Array Front Panel declarations or synthesize scalar replacements.",p,true);
            if((!numeric&&!booleanClass&&!textIndicator)||(numeric&&info.type!="i32"&&info.type!="u32"&&info.type!="i64"&&info.type!="u64"&&info.type!="f64")||(booleanClass&&info.type!="bool"))fail("IR_WIDGET_UNSUPPORTED","Only exact scalar Numeric i32/u32/i64/u64/f64, Boolean/Button and headless String/Path indicator declarations are admitted; no String/Path control or Cluster widget is implied.",p,true);
            if((info.role!="control"&&info.role!="indicator")||(info.klass.ends_with("_indicator")&&info.role!="indicator")||
                (info.klass.ends_with("_control")&&info.role!="control"))fail("IR_WIDGET_ROLE","Widget class and role conflict.",p);
            if(const auto *props=w.member("props")){
                obj(props,p+"/props");
                for(const auto &[key,v]:props->object){
                    if(key=="data_entry.increment_step") {
                        const auto at=pointer(p+"/props",key);
                        double step=0;
                        const auto parsed=std::from_chars(v.string.data(),v.string.data()+v.string.size(),step);
                        if(!numeric||v.type!=Json::Type::Number||parsed.ec!=std::errc{}||
                            parsed.ptr!=v.string.data()+v.string.size()||!std::isfinite(step)||step<0)
                            fail("IR_WIDGET_PROPERTY_INVALID","Numeric increment_step requires a finite nonnegative JSON number on a Numeric widget.",at);
                        // The current native host realizes the ordinary step of 1.
                        // Preserve the authored property and require decimal
                        // equality, rather than accepting a neighbor rounded to 1.
                        const auto exactStep=numeric_value::canonicalWideIntegerLexeme(v.string,"u64");
                        if(!exactStep||*exactStep!="1")
                            fail("IR_WIDGET_BEHAVIOR_UNSUPPORTED","This native widget host realizes data_entry.increment_step=1 only; the authored nonnegative step is valid but unavailable in this configuration.",at,true);
                        continue;
                    }
                    if(key!="default_value"&&key!="value"&&key!="data_type.representation"&&key!="data_type.named_numeric_size"&&key!="representation.kind"&&key!="data_entry.use_type_defaults"&&
                        !key.starts_with("display.")&&!key.starts_with("state_text.")&&!key.starts_with("style.")&&key!="behavior.mechanical_action")fail("IR_WIDGET_PROPERTY_UNSUPPORTED","Property has no admitted value/presentation contract: "+key,p,true);
                    if(key=="data_type.representation"&&(v.type!=Json::Type::String||v.string!=info.type))fail("IR_SOURCE_TYPE_CONFLICT","Widget representation differs from its declared type.",p);
                }
                const auto *initial=props->member("default_value");if(!initial)initial=props->member("value");
                if(initial){
                    bool gammaLimit=false;
                    if(info.type=="f64"&&info.role=="control"&&initial->type==Json::Type::String&&(initial->string=="+Inf"||initial->string=="+Infinity")){
                        bool found=false;const auto *diagram=root.member("diagram");gammaLimit=diagram&&gammaInfiniteLimitWidget(*diagram,wid,found)&&found;
                    }
                    if(!typedScalar(*initial,info.type)&&!(textIndicator&&initial->type==Json::Type::String&&initial->string.size()<=1'000'000U)&&!gammaLimit)fail("IR_TYPED_VALUE","Default is outside its exact scalar/String/Path indicator type; +Infinity is admitted only at a Gamma P/Q limit.",p);info.initial=encode(*initial);
                }
                if(props->member("value")&&props->member("default_value")&&!exact(*props->member("value"),*props->member("default_value")))fail("IR_DEFAULT_CONFLICT","Two default-value declarations disagree.",p);
                if(const auto *action=props->member("behavior.mechanical_action");action &&
                    (!booleanClass || action->type!=Json::Type::String || !isSupportedButtonMechanicalAction(action->string)))
                    fail("IR_WIDGET_PROPERTY_INVALID","Mechanical action must name one of the six supported Boolean/Button actions; indicators retain it as inactive metadata.",p+"/props/behavior.mechanical_action");
            }
            if(info.role=="control"&&!info.initial)fail("IR_DEFAULT_UNSUPPORTED","A control needs its actual declared default; no value is invented.",p,true);
            if(!widgets.emplace(wid,std::move(info)).second)fail("IR_SOURCE_DUPLICATE_ID","Duplicate widget identity.",p);
        }
    }
    void scope(const Json &raw,const std::string &p,const std::string &region={},const std::string &owner={}){
        obj(&raw,p);fields(raw,{"nodes","edges","annotations","wire_fragments"},p);
        if(const auto *fragments=raw.member("wire_fragments");fragments&&!arr(fragments,p).empty())fail("IR_INCOMPLETE_SOURCE","Incomplete wires cannot become compiled connections.",p);
        const auto &ns=arr(raw.member("nodes"),p+"/nodes");if(out.nodes.size()+ns.size()>options.limits.maximumNodes)fail("IR_RESOURCE_LIMIT","Node budget exceeded.",p,true);
        for(std::size_t i=0;i<ns.size();++i){
            const auto path=p+"/nodes/"+std::to_string(i);const auto &n=obj(&ns[i],path);const auto sid=identity(n.member("id"),path,options.limits.maximumSourceIdBytes);
            CanonicalIrNode node;node.id=encodedId("node",sid);node.sourceNodeId=sid;node.family=text(n.member("kind"),path);node.regionId=region;
            if(!nodeIndex.emplace(sid,out.nodes.size()).second)fail("IR_SOURCE_DUPLICATE_ID","Node identities must remain unique across regions.",path);
            if(node.family=="widget_value"){
                fields(n,{"id","kind","widget","layout","doc","tags"},path);node.kind="widget_value";node.widgetId=text(n.member("widget"),path);
                const auto found=widgets.find(node.widgetId);if(found==widgets.end())fail("IR_WIDGET_MISMATCH","Terminal refers to no actual widget declaration.",path);
                const auto &w=found->second;node.role=w.role;node.widgetClass=w.klass;node.valueType=w.type;node.defaultValueJson=w.initial;
                if(w.initial&&w.type=="f64"){auto v=source::parseJson(*w.initial);node.defaultValue=v->number;}
                node.ports.push_back({"value",w.role=="indicator",w.type});
                if(!participants.emplace(node.widgetId,node.id).second)fail("IR_WIDGET_UNSUPPORTED","Repeated natural participation requires an explicit read-instance contract.",path,true);
            }else if(node.family=="constant"&&n.member(canonicalObjectReferenceAttribute)){
                fields(n,{"id","kind","type","value","constant_profile","layout","doc","tags",canonicalObjectReferenceAttribute},path);
                if(text(n.member("constant_profile"),path)!=canonicalObjectReferenceProfile)fail("IR_OBJECT_REFERENCE_PROFILE","The complete local literal profile must be explicit.",path);
                CanonicalObjectReferenceDiagnostic diagnostic;node.objectReference=canonicalObjectReferenceDescriptorFromAttributes(*n.member(canonicalObjectReferenceAttribute),path+"/"+std::string(canonicalObjectReferenceAttribute)+"/configuration",&diagnostic);
                if(!node.objectReference)fail(diagnostic.code,diagnostic.message,path,diagnostic.unsupported);
                const auto config=source::parseJson(node.objectReference->configurationJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                if(node.objectReference->kind!=CanonicalObjectReferenceKind::Resolve||node.objectReference->ports.size()!=1||node.objectReference->ports.front().id!="value"||!config||!config->member("value")||!n.member("value")||!exact(*config->member("value"),*n.member("value"))||node.objectReference->ports.front().type!=text(n.member("type"),path))
                    fail("IR_OBJECT_REFERENCE_LITERAL","The real constant and its complete local descriptor disagree.",path);
                node.kind=std::string(canonicalObjectReferenceOperation(node.objectReference->kind));node.literalProfile=std::string(canonicalObjectReferenceProfile);node.valueType=node.objectReference->ports.front().type;node.literalValueJson=encode(*n.member("value"));
                node.ports.push_back({"value",false,node.valueType});
            }else if(node.family=="constant"){
                fields(n,{"id","kind","type","value","constant_profile","layout","doc","tags"},path);
                const auto *profile=n.member("constant_profile");
                if(!profile||profile->type!=Json::Type::String||(profile->string!=scalarConstantProfile&&profile->string!=canonicalImmutableRecordProfile&&profile->string!=canonicalStringConstantProfile))
                    fail("IR_CONSTANT_PROFILE_UNSUPPORTED","A constant requires its explicit admitted local scalar, immutable Record or String2 profile.",path+"/constant_profile",true);
                node.kind="frog.core.constant";node.valueType=text(n.member("type"),path+"/type");node.literalProfile=profile->string;
                const auto *literal=n.member("value");
                if(node.valueType.starts_with("array<"))fail("IR_ARRAY_LITERAL_UNAVAILABLE","Array literals are not producers in this first local corridor; use actual scalar Build inputs.",path+"/type",true);
                if(node.literalProfile==canonicalStringConstantProfile) {
                    if(node.valueType!="string"||!literal||literal->type!=Json::Type::String)fail("IR_STRING_LITERAL_TYPE","The local String literal requires exact String type and JSON String value.",path+"/value");
                    if(literal->string.size()>canonicalStringMaximumBytes)fail("IR_STRING_RESOURCE_LIMIT","The local String literal exceeds 1 000 000 bytes.",path+"/value",true);
                    if(!canonicalStringUtf8Valid(literal->string))fail("IR_STRING_LITERAL_ENCODING","The local String value must be exact valid UTF-8 including NUL.",path+"/value");
                }else if(node.literalProfile==canonicalImmutableRecordProfile) {
                    CanonicalRecordDiagnostic diagnostic;const auto type=canonicalRecordTransportType(node.valueType,&diagnostic);
                    if(!type)fail(diagnostic.code,diagnostic.message,path+"/type",recordCapabilityFailure(diagnostic.code));
                    if(!literal||!canonicalRecordLiteralValid(*type,*literal,&diagnostic))fail(diagnostic.code.empty()?"IR_RECORD_LITERAL_REQUIRED":diagnostic.code,diagnostic.message.empty()?"An exact declared immutable Record value is required.":diagnostic.message,path+"/value",recordCapabilityFailure(diagnostic.code));
                }else {
                    if(node.valueType!="bool"&&node.valueType!="f64"&&node.valueType!="i32"&&node.valueType!="u32"&&node.valueType!="i64"&&node.valueType!="u64")fail("IR_CONSTANT_TYPE_UNSUPPORTED","The scalar constant profile retains its six exact scalar types.",path+"/type",true);
                    if(!literal||!typedScalar(*literal,node.valueType))fail("IR_CONSTANT_VALUE","The literal must match its exact declared scalar type.",path+"/value");
                }
                node.literalValueJson=encode(*literal);node.ports.push_back({"value",false,node.valueType});
            }else if(node.family=="primitive"){
                node.kind=text(n.member("type"),path);
                if(n.member(canonicalObjectReferenceAttribute))fields(n,{"id","kind","type","layout","doc","tags",canonicalObjectReferenceAttribute},path);
                else if(node.kind==kNativeCallScalarOperationId)fields(n,{"id","kind","type","layout","doc","tags","native_call_profile","native_call_configuration"},path);
                else if(node.kind=="frog.core.unbundle.by_name")fields(n,{"id","kind","type","layout","doc","tags","selected_fields_version","selected_fields"},path);
                else if(const auto *arrayContract=canonicalArrayNativeContract(node.kind)) {
                    if(arrayContract->operation==CanonicalArrayOperation::Build)fields(n,{"id","kind","type","layout","doc","tags","array_execution_profile","input_ports"},path);
                    else fields(n,{"id","kind","type","layout","doc","tags","array_execution_profile"},path);
                }
                else if(const auto *stringContract=canonicalStringNativeContract(node.kind);stringContract&&stringContract->operation==CanonicalStringOperation::ConcatenateBytes)
                    fields(n,{"id","kind","type","layout","doc","tags","input_ports"},path);
                else if(const auto *scalar=canonicalScalarNativeContract(node.kind);scalar&&scalar->maximumInputCount)
                    fields(n,{"id","kind","type","layout","doc","tags","input_ports"},path);
                else fields(n,{"id","kind","type","layout","doc","tags"},path);
                if(n.member(canonicalObjectReferenceAttribute)) {
                    CanonicalObjectReferenceDiagnostic diagnostic;node.objectReference=canonicalObjectReferenceDescriptorFromAttributes(*n.member(canonicalObjectReferenceAttribute),path+"/"+std::string(canonicalObjectReferenceAttribute)+"/configuration",&diagnostic);
                    if(!node.objectReference)fail(diagnostic.code,diagnostic.message,path,diagnostic.unsupported);
                    if(node.kind!=canonicalObjectReferenceOperation(node.objectReference->kind))fail("IR_OBJECT_REFERENCE_OPERATION","The real primitive and the descriptor operation disagree.",path);
                    for(const auto &port:node.objectReference->ports)node.ports.push_back({port.id,port.input,port.type});
                }else if(node.kind==kNativeCallScalarOperationId) {
                    if(text(n.member("native_call_profile"),path+"/native_call_profile")!=kNativeCallScalarProfile)
                        fail("IR_NATIVE_CALL_PROFILE","The local scalar5 NativeCall profile must be explicit.",path+"/native_call_profile");
                    CanonicalNativeCallDiagnostic diagnostic;
                    node.nativeCall=canonicalNativeCallScalarDescriptor(obj(n.member("native_call_configuration"),path+"/native_call_configuration"),path+"/native_call_configuration",&diagnostic);
                    if(!node.nativeCall)fail(diagnostic.code,diagnostic.message,diagnostic.location,diagnostic.unsupported);
                    if(node.nativeCall->ports.size()>options.limits.maximumMappingRecords)
                        fail("IR_RESOURCE_LIMIT","NativeCall ports exceed the existing FIR record budget.",path,true);
                    for(const auto &port:node.nativeCall->ports) {
                        if(port.id.empty()||port.id.size()>options.limits.maximumSourceIdBytes)
                            fail("IR_RESOURCE_LIMIT","A NativeCall connector identity exceeds the existing Source-ID byte budget.",path,true);
                        node.ports.push_back({port.id,port.input,port.type});
                    }
                }
                else if(node.kind=="frog.core.unbundle.by_name") {
                    const auto *version=n.member("selected_fields_version");
                    if(!version||version->type!=Json::Type::Number||version->string!="1")fail("IR_UNBUNDLE_VERSION","selected_fields_version must be the integer 1.",path+"/selected_fields_version");
                    const auto &selected=arr(n.member("selected_fields"),path+"/selected_fields");
                    if(selected.empty()||selected.size()>64)fail("IR_UNBUNDLE_SELECTION","Unbundle needs 1–64 persisted output/field identity pairs.",path+"/selected_fields");
                    node.ports.push_back({"record",true,{}});std::set<std::string> outputs;
                    for(std::size_t i=0;i<selected.size();++i) {
                        const auto at=path+"/selected_fields/"+std::to_string(i);const auto &pair=obj(&selected[i],at);fields(pair,{"port","field_id"},at);
                        const auto id=text(pair.member("port"),at+"/port"),field=text(pair.member("field_id"),at+"/field_id");
                        if(!canonicalRecordOutputIdentity(id,options.limits.maximumSourceIdBytes)||!outputs.insert(id).second)fail("IR_UNBUNDLE_PORT","Output identities must be unique bounded id_strings other than record.",at+"/port");
                        if(field.size()>1024||std::any_of(field.begin(),field.end(),[](unsigned char c){return c<32;}))fail("IR_UNBUNDLE_FIELD","A selected field is an exact direct canonical identity, not a private path.",at+"/field_id");
                        node.selectedFields.push_back(field);node.ports.push_back({id,false,{}});
                    }
                }
                else if(const auto *arrayContract=canonicalArrayNativeContract(node.kind)) {
                    const auto *profile=n.member("array_execution_profile");
                    if(!profile||profile->type!=Json::Type::String||profile->string!=canonicalArrayReal1dProfile)fail("IR_ARRAY_PROFILE","The exact local Array real1d profile must be explicit.",path+"/array_execution_profile");
                    if(arrayContract->operation==CanonicalArrayOperation::Build) {
                        std::vector<std::string> inputs;const auto &raw=arr(n.member("input_ports"),path+"/input_ports");
                        if(raw.empty()||raw.size()>canonicalArrayMaximumBuildInputs)fail("IR_ARRAY_PORTS","Build requires1..32 exact contiguous inputs.",path+"/input_ports");
                        for(std::size_t p=0;p<raw.size();++p){if(raw[p].type!=Json::Type::String||raw[p].string.size()>8)fail("IR_ARRAY_PORTS","Build input identity must be an exact bounded canonical token.",path+"/input_ports");inputs.push_back(raw[p].string);}
                        if(!canonicalArrayBuildPortsValid(inputs))fail("IR_ARRAY_PORTS","Build identities must remain contiguous array_a,array_b,input_3..input_32.",path+"/input_ports");
                        for(const auto &input:inputs)node.ports.push_back({input,true,"f64"});
                    } else {node.ports.push_back({"array",true,std::string(canonicalArraySourceType)});if(arrayContract->operation==CanonicalArrayOperation::Index)node.ports.push_back({"index",true,"i32"});}
                    node.ports.push_back({std::string(arrayContract->outputPort),false,std::string(arrayContract->outputType)});
                }
                else if(const auto *stringContract=canonicalStringNativeContract(node.kind)) {
                    std::vector<std::string> inputs;
                    if(stringContract->operation==CanonicalStringOperation::LengthBytes)inputs.push_back("value");
                    else {
                        const auto &rawPorts=arr(n.member("input_ports"),path+"/input_ports");
                        if(rawPorts.size()>canonicalStringMaximumInputs)fail("IR_STRING_PORTS","Concat has at most 16 explicit ordered inputs.",path+"/input_ports");
                        for(std::size_t i=0;i<rawPorts.size();++i) {
                            if(rawPorts[i].type!=Json::Type::String||rawPorts[i].string.size()>8)fail("IR_STRING_PORTS","A String input identity must be its exact bounded canonical port token.",path+"/input_ports/"+std::to_string(i));
                            inputs.push_back(rawPorts[i].string);
                        }
                    }
                    if(!canonicalStringInputPortsValid(*stringContract,inputs))fail("IR_STRING_PORTS","The exact String input list must retain a/b and ascending actual input_N identities.",path+"/input_ports");
                    for(const auto &input:inputs)node.ports.push_back({input,true,"string"});
                    node.ports.push_back({"result",false,std::string(stringContract->outputType)});
                }
                else if(const auto *selection=canonicalScalarSelectionContract(node.kind)) {
                    for(const auto port:selection->inputs)node.ports.push_back({std::string(port),true,port=="select"?"bool":""});
                    for(const auto port:selection->outputs)node.ports.push_back({std::string(port),false,{}});
                }
                else if(const auto *scalar=canonicalScalarNativeContract(node.kind)) {
                    const bool boolean=scalar->domain==CanonicalScalarDomain::Boolean;
                    const bool arithmetic=scalar->domain==CanonicalScalarDomain::FiniteF64Arithmetic;
                    const std::string inputType=boolean?"bool":arithmetic?"f64":"";
                    const bool compare=scalar->domain==CanonicalScalarDomain::ExactEquality||scalar->domain==CanonicalScalarDomain::ExactOrder||scalar->domain==CanonicalScalarDomain::ZeroComparison;
                    if(scalar->maximumInputCount) {
                        const auto &raw=arr(n.member("input_ports"),path+"/input_ports");
                        if(raw.size()<scalar->inputCount||raw.size()>scalar->maximumInputCount)fail(boolean?"IR_BOOLEAN_PORTS":"IR_NUMERIC_ADD_PORTS",boolean?"Boolean reduction requires2..16 explicit actual operands.":"Add reduction requires2..16 explicit actual operands.",path+"/input_ports");
                        std::vector<std::string> inputs;for(const auto &port:raw)inputs.push_back(text(&port,path+"/input_ports"));
                        if(!(boolean?canonicalBooleanInputPortsValid(inputs):canonicalNumericAddInputPortsValid(inputs)))fail(boolean?"IR_BOOLEAN_PORTS":"IR_NUMERIC_ADD_PORTS",boolean?"Boolean ports must be a,b,then strictly ascending actual input_3..input_16.":"Add ports must be a,b,then strictly ascending actual input_3..input_16.",path+"/input_ports");
                        for(const auto &port:inputs)node.ports.push_back({port,true,inputType});
                    }else {
                        node.ports.push_back({std::string(scalar->inputA),true,inputType});
                        if(scalar->inputCount==2)node.ports.push_back({std::string(scalar->inputB),true,inputType});
                    }
                    node.ports.push_back({std::string(scalar->output),false,compare?"bool":(scalar->domain==CanonicalScalarDomain::UnaryReal64||scalar->domain==CanonicalScalarDomain::IntegerDivideReal64)?"f64":inputType});
                }
                else if(isContextPathFunction(node.kind)) { node.valueType="path";node.ports={{"result",false,"path"}}; }
                else if(canonicalHyperbolicIdentity(node.kind)||canonicalGatingIdentity(node.kind))node.ports={{"x",true,{}},{"result",false,"f64"}};
                else if(canonicalTrigonometricIdentity(node.kind)) {
                    if(node.kind=="frog.studio.math.trigonometric.atan2")node.ports={{"y",true,{}},{"x",true,{}},{"result",false,"f64"}};
                    else if(node.kind=="frog.studio.math.trigonometric.sincos")node.ports={{"x",true,{}},{"sin",false,"f64"},{"cos",false,"f64"}};
                    else node.ports={{"x",true,{}},{"result",false,"f64"}};
                }
                else if(canonicalExponentialIdentity(node.kind)) {
                    const auto *contract=canonicalExponentialContract(node.kind);
                    if(contract->binary)node.ports={{"y",true,"f64"},{"x",true,"f64"},{"result",false,"f64"}};
                    else node.ports={{"x",true,"f64"},{"result",false,"f64"}};
                }
                else if(canonicalDiscreteMathIdentity(node.kind))node.ports=discreteMathPorts(node.kind);
                else if(canonicalBesselIdentity(node.kind))node.ports=besselPorts(node.kind);
                else if(canonicalGammaIdentity(node.kind))node.ports=gammaPorts(node.kind);
                else if(canonicalHypergeometricIdentity(node.kind))node.ports=hypergeometricPorts(node.kind);
                else if(canonicalEllipticIdentity(node.kind))node.ports=ellipticPorts(node.kind);
                else if(canonicalExponentialIntegralsIdentity(node.kind))node.ports=exponentialIntegralsPorts(node.kind);
                else if(canonicalErrorFunctionsIdentity(node.kind))node.ports=errorFunctionsPorts(node.kind);
                else if(canonicalEllipticParabolicIdentity(node.kind))node.ports=ellipticParabolicPorts(node.kind);
                else fail("IR_PRIMITIVE_UNSUPPORTED","Primitive has no admitted typed native realization.",path,true);
            }else if(node.family=="structure"){
                for(const auto &[key,_]:n.object)if(key!="id"&&key!="kind"&&key!="structure_type"&&key!="boundary"&&key!="structure_terminals"&&key!="regions"&&key!="layout"&&key!="doc"&&key!="tags"&&
                    key!="structure_tunnels"&&key!="case_profile"&&!key.starts_with("while_loop_")&&!key.starts_with("for_loop_")&&!key.starts_with("case_structure_"))fail("IR_UNSUPPORTED_FIELD","Unsupported structure field: "+key,path,true);
                node.kind=text(n.member("structure_type"),path);if(node.kind!="while_loop"&&node.kind!="for_loop"&&node.kind!="case")fail("IR_STRUCTURE_UNSUPPORTED","Only public While, For and exact bool/i64/u64 Case are admitted in this corridor.",path,true);
                if(const auto *profile=n.member("case_profile");profile&&(node.kind!="case"||text(profile,path)!="frog.case-editing@1"))fail("IR_CASE_PROFILE_UNSUPPORTED","Unknown or misplaced Case editing profile.",path+"/case_profile",true);
                if(n.member("while_loop_stop_if_true"))fail("IR_WHILE_PRIVATE_SEMANTICS","Public While must not retain a private stop-if-true semantic flag; migrate through Boolean Not.",path);
                const auto &terminals=obj(n.member("structure_terminals"),path);
                const auto terminal=[&](const char *name,const char *type,const char *role,const char *body,bool outer,bool readOnly){
                    const auto &t=obj(terminals.member(name),path+"/structure_terminals/"+name);fields(t,{"type","outer_visible","exposed_in_body","read_only","body_port","outer_port","role"},path);
                    if(text(t.member("type"),path)!=type||text(t.member("role"),path)!=role||text(t.member("body_port"),path)!=body)
                        fail("IR_TERMINAL_CONTRACT","Structure terminal differs from the published typed role.",path);
                    flag(t.member("outer_visible"),outer,path);flag(t.member("exposed_in_body"),true,path);flag(t.member("read_only"),readOnly,path);
                    if(outer){if(text(t.member("outer_port"),path)!=name)fail("IR_TERMINAL_CONTRACT","Outer structure port identity differs from its public role.",path);}
                    else if(t.member("outer_port"))fail("IR_TERMINAL_CONTRACT","An internal-only terminal cannot invent an outer port.",path);
                };
                if(node.kind=="while_loop"){
                    fields(terminals,{"condition","index"},path);terminal("condition","bool","continue_while_true","condition",false,false);terminal("index","i64","index","iteration",false,true);
                    node.ports={{"iteration",false,"i64"},{"condition",true,"bool"}};
                }else if(node.kind=="for_loop"){
                    fields(terminals,{"count","index"},path);terminal("count","i64","count","count_value",true,true);terminal("index","i64","index","iteration",false,true);
                    node.ports={{"count",true,"i64"},{"iteration",false,"i64"},{"count_value",false,"i64"}};
                }else{
                    fields(terminals,{"selector"},path);const auto selectorType=text(obj(terminals.member("selector"),path).member("type"),path);
                    if(selectorType!="bool"&&selectorType!="i64"&&selectorType!="u64")fail("IR_CASE_SELECTOR_UNSUPPORTED","Only exact bool/i64/u64 Case selectors are admitted.",path+"/structure_terminals/selector/type",true);
                    terminal("selector",selectorType.c_str(),"selector","selected_case",true,true);
                    node.ports={{"selector",true,selectorType},{"selected_case",false,selectorType}};
                }
            }else if(node.family=="call"||node.family=="subfrog"||node.family=="sub_frog")
                fail("IR_CALL_UNSUPPORTED","Sub-FROG calls require their explicit source, instance and return-boundary contracts before native admission.",path,true);
            else fail("IR_FAMILY_UNSUPPORTED","Execution family is outside the admitted native corridor.",path,true);
            out.nodes.push_back(std::move(node));sourceNodes.push_back({&n,path,region});
            if(!region.empty()){const auto found=std::find_if(out.regions.begin(),out.regions.end(),[&](const auto &r){return r.id==region;});found->memberNodeIds.push_back(out.nodes.back().id);}
            if(out.nodes.back().family=="structure"){
                const auto nodeId=out.nodes.back().id,kind=out.nodes.back().kind;const auto &regions=arr(n.member("regions"),path);
                const bool caseKind=kind=="case";
                const auto selectorType=caseKind?out.nodes[nodeIndex.at(sid)].ports.front().typeRef:std::string{};
                if((caseKind&&((selectorType=="bool"&&regions.size()!=2)||regions.empty()))||(!caseKind&&(regions.size()!=1||text(regions[0].member("id"),path)!="body")))fail("IR_REGION_CONTRACT","A loop owns one body; Boolean Case owns true/false arms; integer Case owns explicit regions and one default.",path);
                CanonicalIrStructure structure;structure.nodeId=nodeId;structure.kind=kind;
                structure.shiftRegisters=registers(n,path,out.nodes[nodeIndex.at(sid)]);
                if(!caseKind)structure.iterationPortId="iteration";
                if(kind=="while_loop")structure.conditionPortId="condition";
                if(kind=="for_loop"){structure.countPortId="count";structure.countValuePortId="count_value";}
                if(caseKind){structure.selectorPortId="selector";structure.selectedCasePortId="selected_case";}
                std::set<std::string> regionIds,integerMatches;std::set<bool> matches;std::size_t defaultCount=0;
                for(std::size_t r=0;r<regions.size();++r){
                    const auto rp=path+"/regions/"+std::to_string(r);const auto &rawRegion=obj(&regions[r],rp);
                    const auto rid=identity(rawRegion.member("id"),rp,options.limits.maximumSourceIdBytes);
                    if(!regionIds.insert(rid).second)fail("IR_REGION_CONTRACT","Region identities must remain unique within their owner.",rp);
                    const auto regionId=encodedId("region",sid)+":"+encodedId("",rid).substr(1);structure.regionIds.push_back(regionId);
                    if(caseKind&&selectorType!="bool") {
                        fields(rawRegion,{"id","diagram","matches","default"},rp);
                        const auto *defaultValue=rawRegion.member("default");
                        if(defaultValue&&defaultValue->type!=Json::Type::Boolean)fail("IR_CASE_MATCH","The default flag must be Boolean.",rp);
                        const bool fallback=defaultValue&&defaultValue->boolean;
                        if(fallback&&++defaultCount>1)fail("IR_CASE_DEFAULT_DUPLICATE","An integer Case has one default region.",rp);
                        std::optional<std::string> exact;
                        if(const auto *many=rawRegion.member("matches")) {
                            const auto &ms=arr(many,rp);if(ms.size()>1)fail("IR_CASE_MATCH_UNSUPPORTED","Integer Case unions and ranges need separate admission.",rp,true);
                            if(!ms.empty()) {
                                const auto &m=obj(&ms.front(),rp);fields(m,{"kind","value"},rp);
                                if(text(m.member("kind"),rp)!="integer_exact")fail("IR_CASE_MATCH_UNSUPPORTED","Only integer_exact decimal strings are admitted.",rp,true);
                                exact=caseInteger(m.member("value"),selectorType);
                                if(!exact)fail("IR_CASE_MATCH","Integer Case label is outside its exact selector type.",rp);
                                if(!integerMatches.insert(*exact).second)fail("IR_CASE_MATCH","Integer Case labels overlap across regions.",rp);
                            }
                        }
                        if(!exact&&!fallback)fail("IR_CASE_MATCH","An integer Case region needs an exact match or default.",rp);
                        structure.caseArms.push_back({regionId,std::move(exact),fallback});
                    }else if(caseKind){
                        fields(rawRegion,{"id","diagram","match","matches"},rp);const Json *match=rawRegion.member("match");
                        if(const auto *many=rawRegion.member("matches")){
                            const auto &ms=arr(many,rp);if(match||ms.size()!=1)fail("IR_CASE_MATCH_UNSUPPORTED","This Boolean Case corridor needs one explicit, unambiguous Boolean match per region.",rp,true);
                            const auto &m=obj(&ms[0],rp);fields(m,{"kind","value"},rp);if(text(m.member("kind"),rp)!="boolean_exact")fail("IR_CASE_MATCH_UNSUPPORTED","Only exact Boolean Case matches are admitted.",rp,true);match=m.member("value");
                        }
                        if(!match||match->type!=Json::Type::Boolean||!matches.insert(match->boolean).second)fail("IR_CASE_MATCH","Boolean Case requires one true and one false region with no default.",rp);
                        structure.caseArms.push_back({regionId,encode(*match),false});
                    }else fields(rawRegion,{"id","diagram"},rp);
                    out.regions.push_back({regionId,rid,nodeId,{},caseKind?"case_arm":"body",rp});
                    scope(obj(rawRegion.member("diagram"),rp),rp+"/diagram",regionId,sid);
                }
                if(caseKind&&selectorType!="bool"&&defaultCount!=1)fail("IR_CASE_DEFAULT_REQUIRED","The integer Case profile requires one explicit default.",path);
                out.structures.push_back(std::move(structure));
            }
        }
        const auto &es=arr(raw.member("edges"),p+"/edges");if(sourceEdges.size()+es.size()>options.limits.maximumEdges)fail("IR_RESOURCE_LIMIT","Edge budget exceeded.",p,true);
        for(std::size_t i=0;i<es.size();++i){const auto path=p+"/edges/"+std::to_string(i);const auto &edge=obj(&es[i],path);fields(edge,{"id","from","to","bends","doc","tags"},path);
            const auto id=identity(edge.member("id"),path,options.limits.maximumSourceIdBytes);if(!edgeIds.insert(id).second)fail("IR_SOURCE_DUPLICATE_ID","Duplicate edge identity.",path);sourceEdges.push_back({&edge,path,region,owner});}
    }
    CanonicalIrPort &port(std::string_view sourceId,std::string_view portId,const std::string &p){
        const auto n=nodeIndex.find(std::string(sourceId));if(n==nodeIndex.end())fail("IR_EDGE_ENDPOINT","Edge refers to a missing source node.",p);
        auto &ports=out.nodes[n->second].ports;const auto found=std::find_if(ports.begin(),ports.end(),[&](const auto &v){return v.id==portId;});
        if(found==ports.end())fail("IR_EDGE_ENDPOINT","Edge refers to no real declared port.",p);return *found;
    }
    enum class BoundaryRole {None,Initializer,Read,Write,Result};
    BoundaryRole boundaryRole(const CanonicalIrNode &node,std::string_view portId)const{
        const auto structure=std::find_if(out.structures.begin(),out.structures.end(),[&](const auto &s){return s.nodeId==node.id;});
        if(structure==out.structures.end())return BoundaryRole::None;
        for(const auto &group:structure->shiftRegisters){
            if(group.writePortId==portId)return BoundaryRole::Write;
            if(group.resultPortId==portId)return BoundaryRole::Result;
            for(const auto &stage:group.stages){
                if(stage.initializerPortId==portId)return BoundaryRole::Initializer;
                if(stage.readPortId==portId)return BoundaryRole::Read;
            }
        }
        return BoundaryRole::None;
    }
    void arrayCorridor() {
        if(std::none_of(out.nodes.begin(),out.nodes.end(),[](const auto &n){return canonicalArrayNativeContract(n.kind);}))return;
        for(const auto &n:out.nodes) {
            if(n.family=="structure"||!n.regionId.empty())fail("IR_ARRAY_REGION_UNAVAILABLE","The first Array capability is flat; owned Array regions, tunnels and states have no admitted contract.",n.sourceNodeId,true);
            if(n.literalProfile==canonicalImmutableRecordProfile||n.literalProfile==canonicalStringConstantProfile||n.kind=="frog.core.unbundle.by_name"||canonicalStringNativeContract(n.kind))
                fail("IR_ARRAY_COMPOSITION_UNAVAILABLE","Array real1d does not compose with Record/String/Event owned profiles in this first target.",n.sourceNodeId,true);
            for(const auto &p:n.ports) {
                if(p.typeRef=="string"||p.typeRef=="path"||p.typeRef.starts_with("cluster<")||p.typeRef.starts_with("ref<"))fail("IR_ARRAY_COMPOSITION_UNAVAILABLE","Array real1d admits only its exact Array values and scalar support.",n.sourceNodeId,true);
                if(p.typeRef.starts_with("array<")) {
                    if(p.typeRef!=canonicalArraySourceType)fail("IR_ARRAY_TYPE_UNSUPPORTED","Only exact dynamic rank1 array<f64> belongs to this local target; no fixed extent, nested Array, rank or coercion is inferred.",n.sourceNodeId,true);
                    if(!canonicalArrayNativeContract(n.kind))fail("IR_ARRAY_CONSUMER_UNAVAILABLE","Array constants/widgets/other consumers are not admitted by the three-operation corridor.",n.sourceNodeId,true);
                }
            }
        }
    }
    void nativeCallCorridor() {
        if(std::none_of(out.nodes.begin(),out.nodes.end(),[](const auto &n){return n.nativeCall.has_value();}))return;
        for(const auto &n:out.nodes) {
            if(n.family=="structure"||!n.regionId.empty())fail("IR_NATIVE_CALL_REGION_UNAVAILABLE","The first local scalar5 call corridor is flat; no region is flattened into another owner.",n.sourceNodeId,true);
            if(canonicalArrayNativeContract(n.kind)||canonicalStringNativeContract(n.kind))fail("IR_NATIVE_CALL_COMPOSITION_UNAVAILABLE","Scalar5 does not acquire the Array3/String2 execution profiles.",n.sourceNodeId,true);
            for(const auto &p:n.ports)if(p.typeRef.starts_with("array<")||p.typeRef.starts_with("ref<")||
                (p.typeRef.starts_with("cluster<")&&p.typeRef!=kNativeCallScalarErrorType))
                fail("IR_NATIVE_CALL_COMPOSITION_UNAVAILABLE","Only the exact status/code/source error Record accompanies scalar5 calls.",n.sourceNodeId,true);
        }
    }
    bool nativeCallOptional(const CanonicalIrNode &node,std::string_view id)const {
        if(!node.nativeCall)return false;
        const auto p=std::find_if(node.nativeCall->ports.begin(),node.nativeCall->ports.end(),[&](const auto &p){return p.id==id&&p.input;});
        return p!=node.nativeCall->ports.end()&&(p->requirement!=NativeCallInputRequirement::Required||p->defaultCanonicalValue.has_value());
    }
    bool objectOptional(const CanonicalIrNode &node,std::string_view id)const {
        if(!node.objectReference)return false;
        const auto p=std::find_if(node.objectReference->ports.begin(),node.objectReference->ports.end(),[&](const auto &p){return p.input&&p.id==id;});return p!=node.objectReference->ports.end()&&!p->required;
    }
    void connectivity(){
        if(std::any_of(out.nodes.begin(),out.nodes.end(),[](const auto &n){return n.objectReference.has_value();})){
            for(const auto &n:out.nodes)if(n.nativeCall||canonicalArrayNativeContract(n.kind)||canonicalStringNativeContract(n.kind))fail("IR_OBJECT_REFERENCE_COMPOSITION_UNAVAILABLE","Local object references do not acquire NativeCall/Array3/String2 execution profiles.",n.sourceNodeId,true);
        }
        nativeCallCorridor();
        arrayCorridor();
        std::map<std::pair<std::string,std::string>,unsigned> drivers;
        for(const auto &e:sourceEdges){
            std::string ends[2],ports[2];unsigned sideIndex=0;
            for(const auto side:{"from","to"}){const auto &end=obj(e.raw->member(side),e.pointer);fields(end,{"node","port"},e.pointer);ends[sideIndex]=text(end.member("node"),e.pointer);ports[sideIndex]=text(end.member("port"),e.pointer);
                auto &p=port(ends[sideIndex],ports[sideIndex],e.pointer);if(p.input!=(sideIndex==1))fail("IR_PORT_DIRECTION","Edge direction contradicts its real typed terminal.",e.pointer);
                const auto &node=out.nodes.at(nodeIndex.at(ends[sideIndex]));
                const bool inner=ends[sideIndex]==e.owner;
                const auto role=boundaryRole(node,ports[sideIndex]);
                const bool innerPort=ports[sideIndex]=="iteration"||ports[sideIndex]=="condition"||ports[sideIndex]=="count_value"||ports[sideIndex]=="selected_case"||role==BoundaryRole::Read||role==BoundaryRole::Write;
                if(node.regionId!=e.region&&!(inner&&innerPort))fail("IR_REGION_CROSSING","A region crossing must use the owning structure terminal, never layout or an unrelated scope.",e.pointer);
                if(node.family=="structure"&&((inner&&!innerPort)||(!inner&&(ports[sideIndex]!="count"&&ports[sideIndex]!="selector"&&role!=BoundaryRole::Initializer&&role!=BoundaryRole::Result))))fail("IR_STRUCTURE_TERMINAL_SCOPE","Structure terminals must be used from their declared outer/inner scopes.",e.pointer);
                ++sideIndex;}
            if(++drivers[{ends[1],ports[1]}]!=1)fail("IR_MULTIPLE_DRIVER","A real input has multiple dataflow drivers.",e.pointer);
            out.connections.push_back({encodedId("edge",text(e.raw->member("id"),e.pointer)),text(e.raw->member("id"),e.pointer),{encodedId("node",ends[0]),ports[0]},{encodedId("node",ends[1]),ports[1]}});
        }
        // Types are resolved from explicit declarations/structure terminals and
        // the primitive equality constraints, without narrowing through double.
        bool changed=true;for(std::size_t pass=0;changed&&pass<=out.nodes.size()*3;++pass){changed=false;
            const auto unify=[&](std::string &a,std::string &b,const std::string &p){if(!a.empty()&&!b.empty()&&a!=b)fail("IR_TYPE_MISMATCH","Connected exact types differ; this corridor has no implicit numeric conversion.",p);if(a.empty()&&!b.empty()){a=b;changed=true;}if(b.empty()&&!a.empty()){b=a;changed=true;}};
            for(const auto &e:sourceEdges){const auto &from=*e.raw->member("from"),&to=*e.raw->member("to");auto &a=port(text(from.member("node"),e.pointer),text(from.member("port"),e.pointer),e.pointer).typeRef;
                auto &b=port(text(to.member("node"),e.pointer),text(to.member("port"),e.pointer),e.pointer).typeRef;
                const auto &sink=out.nodes.at(nodeIndex.at(text(to.member("node"),e.pointer)));
                const bool specialPromotion=(canonicalGammaIdentity(sink.kind)||canonicalHypergeometricIdentity(sink.kind)||canonicalEllipticIdentity(sink.kind)||canonicalExponentialIntegralsIdentity(sink.kind)||canonicalErrorFunctionsIdentity(sink.kind)||canonicalEllipticParabolicIdentity(sink.kind))&&b=="f64"&&(a=="i32"||a=="u32"||a=="i64"||a=="u64");
                if(canonicalArrayNativeContract(sink.kind)&&!a.empty()&&!b.empty()&&a!=b)
                    fail("IR_ARRAY_TYPE_UNSUPPORTED","Array real1d requires exact declared f64 operands and a real i32 Index; no numeric/rank/extent conversion is inserted.",e.pointer,true);
                if(canonicalExponentialIdentity(sink.kind)&&!a.empty()&&a!="f64")
                    fail("IR_EXPONENTIAL_NATIVE_TYPE_UNSUPPORTED","The Exponential compiled real64 profile requires exact f64 operands; no integer, f32, complex or aggregate conversion is inserted.",e.pointer,true);
                // Unbundle input descriptors are copied only from the actual
                // driver, under their aggregate byte budget below.
                if(sink.kind!="frog.core.unbundle.by_name"||text(to.member("port"),e.pointer)!="record")if(!specialPromotion)unify(a,b,e.pointer);}
            for(auto &node:out.nodes)if(node.kind=="frog.core.unbundle.by_name"&&!resolvedRecordNodes.contains(node.id)) {
                const auto &sourcePath=sourceNodes[nodeIndex.at(node.sourceNodeId)].pointer;
                const auto driver=std::find_if(sourceEdges.begin(),sourceEdges.end(),[&](const auto &edge){const auto *to=edge.raw->member("to");return to&&text(to->member("node"),edge.pointer)==node.sourceNodeId&&text(to->member("port"),edge.pointer)=="record";});
                if(driver==sourceEdges.end())continue;const auto &from=*driver->raw->member("from");
                auto &actual=port(text(from.member("node"),driver->pointer),text(from.member("port"),driver->pointer),driver->pointer).typeRef;if(actual.empty())continue;
                CanonicalRecordDiagnostic diagnostic;const auto record=canonicalRecordTransportType(actual,&diagnostic);
                if(!record)fail(diagnostic.code,diagnostic.message,sourcePath,recordCapabilityFailure(diagnostic.code));
                const auto charge=[&](std::string_view type){const auto maximum=std::min<std::size_t>(options.limits.maximumIrBytes,16U*1024U*1024U);if(type.size()>maximum||derivedRecordDescriptorBytes>maximum-type.size())fail("IR_UNBUNDLE_DESCRIPTOR_LIMIT","Derived Record descriptors exceed the aggregate byte budget.",sourcePath,true);derivedRecordDescriptorBytes+=type.size();};
                charge(actual);unify(node.ports.front().typeRef,actual,driver->pointer);
                for(std::size_t i=0;i<node.selectedFields.size();++i) {
                    const auto member=std::find(record->fieldIds.begin(),record->fieldIds.end(),node.selectedFields[i]);
                    if(member==record->fieldIds.end())fail("IR_UNBUNDLE_FIELD","Selected field is absent from the actual driver Record.",sourcePath+"/selected_fields/"+std::to_string(i)+"/field_id");
                    auto type=record->children[std::size_t(member-record->fieldIds.begin())].canonical();charge(type);unify(node.ports[i+1].typeRef,type,sourcePath+"/selected_fields/"+std::to_string(i));
                }
                resolvedRecordNodes.insert(node.id);
            }
            for(auto &n:out.nodes)if(const auto *selection=canonicalScalarSelectionContract(n.kind)) {
                for(auto &port:n.ports)if(port.id!="select")unify(n.ports.front().typeRef,port.typeRef,n.sourceNodeId);
                if(!n.ports.front().typeRef.empty()&&!canonicalScalarSelectionTypeAccepted(*selection,n.ports.front().typeRef))
                    fail("IR_SCALAR_SELECTION_TYPE_UNSUPPORTED","Scalar selection requires identical supported data types, without conversion or lifting.",n.sourceNodeId,true);
            }
            for(auto &n:out.nodes)if(const auto *scalar=canonicalScalarNativeContract(n.kind)) {
                if(scalar->inputCount==2)for(std::size_t input=1;input+1<n.ports.size();++input)unify(n.ports[0].typeRef,n.ports[input].typeRef,n.sourceNodeId);
                const bool arithmetic=scalar->domain==CanonicalScalarDomain::ExistingAdd||scalar->domain==CanonicalScalarDomain::FiniteF64Arithmetic||
                    scalar->domain==CanonicalScalarDomain::UnarySame||scalar->domain==CanonicalScalarDomain::UnarySigned||scalar->domain==CanonicalScalarDomain::UnarySignedChecked||scalar->domain==CanonicalScalarDomain::UnaryRounding||scalar->domain==CanonicalScalarDomain::UnaryIntegerRounding;
                if(arithmetic)unify(n.ports[0].typeRef,n.ports.back().typeRef,n.sourceNodeId);
                const auto &type=n.ports[0].typeRef;
                if(!type.empty()&&!canonicalScalarNativeOperandAccepted(*scalar,type)) {
                    if(n.kind=="frog.math.round"&&type=="f64")fail("IR_MATH_ROUND_F64_PROFILE_UNAVAILABLE","Published pure Math Round f64 has no realization in this consumer; authored ties-to-even uses the distinct private round_nearest.even64.v1 profile; explicit historical fenv64.v1 remains supported.",n.sourceNodeId,true);
                    if((scalar->domain==CanonicalScalarDomain::ExistingAdd&&scalar->policy==CanonicalScalarPolicy::CoreCheckedFinite)||n.kind=="frog.core.less")
                        fail("IR_PRIMITIVE_TYPE","Arithmetic/comparison operands must be admitted numeric scalars.",n.sourceNodeId);
                    fail("IR_SCALAR_NATIVE_TYPE_UNSUPPORTED","The exact operand type is outside this declared native scalar contract; no implicit cast is inserted.",n.sourceNodeId,true);
                }
            }
        }
        for(auto &n:out.nodes){
            if(canonicalTrigonometricIdentity(n.kind))for(const auto &p:n.ports)
                if(p.input&&!p.typeRef.empty()&&p.typeRef!="f64"&&p.typeRef!="i64"&&p.typeRef!="u64")
                    fail("IR_TRIGONOMETRIC_TYPE_UNSUPPORTED","This native target admits Trigonometric f64 and i64/u64 promoted to f64; f32, complex and aggregate execution require their separate target capability. Complex Atan2 is unavailable.",n.sourceNodeId,true);
            if(canonicalHyperbolicIdentity(n.kind) && !n.ports.front().typeRef.empty() &&
                n.ports.front().typeRef!="f64" && n.ports.front().typeRef!="i64" && n.ports.front().typeRef!="u64")
                fail("IR_HYPERBOLIC_TYPE_UNSUPPORTED","This native target admits Hyperbolic f64 and i64/u64 promoted to f64; f32, complex and aggregate execution require their separate target capability.",n.sourceNodeId,true);
            if(canonicalGatingIdentity(n.kind) && !n.ports.front().typeRef.empty() &&
                n.ports.front().typeRef!="f64" && n.ports.front().typeRef!="i64" && n.ports.front().typeRef!="u64")
                fail("IR_GATING_TYPE_UNSUPPORTED","This native target admits Gating f64 and i64/u64 promoted to f64; f32 and real aggregate execution require their separate target capability. Complex inputs are outside the Gating contract.",n.sourceNodeId,true);
            for(const auto &p:n.ports){if(p.typeRef.empty())fail("IR_TYPE_UNRESOLVED","No real type determines this port.",n.sourceNodeId);if(p.input&&drivers[{n.sourceNodeId,p.id}]!=1&&boundaryRole(n,p.id)!=BoundaryRole::Initializer&&!(canonicalBesselIdentity(n.kind)&&p.id!="x")&&!canonicalGammaOptionalInput(n.kind,p.id)&&!canonicalEllipticOptionalInput(n.kind,p.id)&&!nativeCallOptional(n,p.id)&&!objectOptional(n,p.id))fail("IR_MISSING_DRIVER","A required input has no source driver.",n.sourceNodeId+":"+p.id);}
            if(n.valueType.empty()&&n.family=="primitive") {
                const auto primary=std::find_if(n.ports.begin(),n.ports.end(),[](const auto &p){return !p.input;});
                n.valueType=canonicalDiscreteMathIdentity(n.kind)&&primary!=n.ports.end()?primary->typeRef:n.ports.back().typeRef;
            }
        }
        std::map<std::string,std::vector<std::size_t>> scopes;for(std::size_t i=0;i<out.nodes.size();++i)scopes[out.nodes[i].regionId].push_back(i);
        for(const auto &[region,members]:scopes){std::map<std::size_t,unsigned> indegree;std::map<std::size_t,std::vector<std::size_t>> next;for(const auto i:members)indegree[i]=0;
            for(const auto &e:sourceEdges)if(e.region==region){const auto a=nodeIndex.at(text(e.raw->member("from")->member("node"),e.pointer)),b=nodeIndex.at(text(e.raw->member("to")->member("node"),e.pointer));
                if(indegree.contains(a)&&indegree.contains(b)){next[a].push_back(b);++indegree[b];}}
            std::deque<std::size_t> ready;for(const auto &[id,count]:indegree)if(!count)ready.push_back(id);std::size_t seen=0;
            while(!ready.empty()){const auto id=ready.front();ready.pop_front();++seen;for(const auto to:next[id])if(!--indegree[to])ready.push_back(to);}
            if(seen!=members.size())fail("IR_CYCLE","A dataflow cycle needs explicit state; While iteration is not an implicit feedback value.",region);
        }
    }
    void write(){
        Json root=object(),meta=object(),unit=object(),objects=array(),connections=array(),regions=array(),maps=array(),relations=array();
        for(std::size_t i=0;i<out.nodes.size();++i){const auto &n=out.nodes[i];const auto &src=sourceNodes[i];auto record=object(),classification=object(),attributes=object(),ports=array();
            classification.object["primary_or_support"]=string("primary");
            if(n.family=="primitive") {
                const auto *scalar=canonicalScalarNativeContract(n.kind);
                if(n.objectReference){classification.object["library_id"]=string("frog.studio.object-reference");classification.object["profile_id"]=string(std::string(canonicalObjectReferenceProfile));}
                else if(n.nativeCall){classification.object["library_id"]=string("frog.studio.connectivity");classification.object["profile_id"]=string(std::string(kNativeCallScalarProfile));}
                else if(const auto *selection=canonicalScalarSelectionContract(n.kind)){classification.object["library_id"]=string("frog.studio.scalar-selection");classification.object["profile_id"]=string(std::string(selection->semanticVersion));}
                else if(canonicalArrayNativeContract(n.kind))classification.object["library_id"]=string("frog.studio.array");
                else if(canonicalStringNativeContract(n.kind))classification.object["library_id"]=string("frog.studio.string");
                else if(n.kind.starts_with("frog.studio.boolean."))classification.object["library_id"]=string("frog.studio.boolean");
                else if(scalar&&scalar->policy==CanonicalScalarPolicy::MathValueV1)classification.object["library_id"]=string("frog.math");
                else if(scalar&&scalar->policy!=CanonicalScalarPolicy::CoreCheckedFinite)
                    classification.object["library_id"]=string(n.kind.starts_with("frog.studio.numeric.")?"frog.studio.numeric":"frog.studio.comparison");
                else classification.object["library_id"]=string((canonicalHyperbolicIdentity(n.kind)||canonicalGatingIdentity(n.kind)||canonicalDiscreteMathIdentity(n.kind)||canonicalTrigonometricIdentity(n.kind)||canonicalBesselIdentity(n.kind)||canonicalGammaIdentity(n.kind)||canonicalHypergeometricIdentity(n.kind)||canonicalEllipticIdentity(n.kind)||canonicalExponentialIntegralsIdentity(n.kind)||canonicalErrorFunctionsIdentity(n.kind)||canonicalEllipticParabolicIdentity(n.kind)||canonicalExponentialIdentity(n.kind))?"frog.studio.math":"frog.core");
            }
            if(n.family=="primitive"&&n.kind.starts_with("frog.math.")) {
                attributes.object["call_class"]=string("standard_library_value");attributes.object["effect"]=string("pure");
            }
            if(n.family=="primitive"&&n.kind=="frog.studio.numeric.round_nearest.fenv64.v1") {
                attributes.object["call_class"]=string("private_scalar_value");attributes.object["effect"]=string("floating_environment");
            }
            if(n.family=="primitive"&&n.kind=="frog.studio.numeric.round_nearest.even64.v1") {
                attributes.object["call_class"]=string("private_scalar_value");attributes.object["effect"]=string("pure");
            }
            if(n.family=="primitive"&&canonicalExponentialIdentity(n.kind))attributes.object["exponential_execution_policy"]=string(std::string(exponentialReal64SemanticVersion));
            if(const auto *arrayContract=canonicalArrayNativeContract(n.kind)) {
                attributes.object["array_execution_profile"]=string(std::string(canonicalArrayReal1dProfile));
                attributes.object["call_class"]=string("private_owned_array_value");attributes.object["effect"]=string("pure");
                if(arrayContract->operation==CanonicalArrayOperation::Build){std::vector<std::string> inputs;for(const auto &p:n.ports)if(p.input)inputs.push_back(p.id);attributes.object["input_ports"]=strings(inputs);}
            }
            if(const auto *stringContract=canonicalStringNativeContract(n.kind)) {
                attributes.object["string_execution_policy"]=string(std::string(stringContract->semanticVersion));
                attributes.object["call_class"]=string("private_owned_string_value");attributes.object["effect"]=string("pure");
                if(stringContract->operation==CanonicalStringOperation::ConcatenateBytes) {
                    std::vector<std::string> inputs;for(const auto &p:n.ports)if(p.input)inputs.push_back(p.id);
                    attributes.object["input_ports"]=strings(inputs);
                }
            }
            if(n.objectReference){attributes.object[std::string(canonicalObjectReferenceAttribute)]=canonicalObjectReferenceAttributes(*n.objectReference);attributes.object["call_class"]=string("private_typed_object_service");attributes.object["effect"]=string("instance_object_effects");}
            if(n.nativeCall) {
                attributes.object["frog_studio_native_call"]=canonicalNativeCallScalarAttributes(*n.nativeCall);
                auto requirement=object();requirement.object={{"requirement_id",string(std::string(kNativeCallProviderRequirement))},
                    {"capabilities",strings({"native_call.scalar5.exact"})},{"binding",string("local_runtime_service")},{"version",string(std::string(kNativeCallScalarProfile))}};
                auto model=object();model.object={{"call_class",string("external_provider")},{"effect",string("provider_native")},
                    {"status_model",string("local_result_tuple")},{"status_outputs",strings({"error_out"})},{"provider_requirement",std::move(requirement)}};
                attributes.object["call_model"]=std::move(model);
            }
            if(n.family=="structure")classification.object["structure_family"]=string(n.kind);
            if(n.family=="constant")classification.object["library_id"]=string(n.objectReference?"frog.studio.object-reference":"frog.core");
            attributes.object["source_node_id"]=string(n.sourceNodeId);if(!n.regionId.empty())attributes.object["region_id"]=string(n.regionId);
            if(const auto *scalar=canonicalScalarNativeContract(n.kind);scalar&&(scalar->policy!=CanonicalScalarPolicy::CoreCheckedFinite||n.kind.starts_with("frog.studio.boolean.")))
                attributes.object["scalar_execution_policy"]=string(std::string(scalar->semanticVersion));
            if(n.family=="structure"){
                const auto s=std::find_if(out.structures.begin(),out.structures.end(),[&](const auto &s){return s.nodeId==n.id;});
                if(!s->shiftRegisters.empty()){
                    auto groups=array();
                    for(const auto &group:s->shiftRegisters){
                        auto g=object(),stages=array();
                        for(const auto &stage:group.stages){auto v=object();v.object={{"tunnel_id",string(stage.tunnelId)},
                            {"initializer_port",string(stage.initializerPortId)},{"read_port",string(stage.readPortId)},
                            {"type_ref",string(stage.typeRef)},{"history_index",*source::parseJson(std::to_string(stage.historyIndex))}};stages.array.push_back(std::move(v));}
                        g.object={{"pair_id",string(group.pairId)},{"write_port",string(group.writePortId)},
                            {"result_port",string(group.resultPortId)},{"stages",std::move(stages)}};groups.array.push_back(std::move(g));
                    }
                    attributes.object["shift_registers"]=std::move(groups);
                }
            }
            std::vector<std::string> refs{ref(src.pointer)};std::vector<std::string> correspondenceRefs;
            if(n.family=="widget_value"){const auto &w=widgets.at(n.widgetId);attributes.object["widget_id"]=string(n.widgetId);attributes.object["widget_class"]=string(n.widgetClass);attributes.object["value_type"]=string(n.valueType);
                if(n.defaultValueJson)attributes.object["default_value"]=*source::parseJson(*n.defaultValueJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);refs.push_back(ref(w.pointer));correspondenceRefs.push_back(encodedId("declaration",n.widgetId));}
            if(n.family=="constant"){
                attributes.object["constant_profile"]=string(n.literalProfile);attributes.object["value_type"]=string(n.valueType);
                attributes.object["literal_value"]=*source::parseJson(*n.literalValueJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                for(const auto key:{"constant_profile","type","value"})refs.push_back(ref(src.pointer+"/"+key));
            }
            if(n.kind=="frog.core.unbundle.by_name") {
                attributes.object["selected_fields_version"]=*source::parseJson("1");auto selections=array();
                for(std::size_t i=0;i<n.selectedFields.size();++i){auto selected=object();selected.object={{"port",string(n.ports[i+1].id)},{"field_id",string(n.selectedFields[i])}};selections.array.push_back(std::move(selected));refs.push_back(ref(src.pointer+"/selected_fields/"+std::to_string(i)+"/port"));refs.push_back(ref(src.pointer+"/selected_fields/"+std::to_string(i)+"/field_id"));}
                attributes.object["selected_fields"]=std::move(selections);refs.push_back(ref(src.pointer+"/selected_fields_version"));
            }
            if(const auto *arrayContract=canonicalArrayNativeContract(n.kind)) {
                refs.push_back(ref(src.pointer+"/array_execution_profile"));
                if(arrayContract->operation==CanonicalArrayOperation::Build)for(std::size_t p=0;p+1<n.ports.size();++p)refs.push_back(ref(src.pointer+"/input_ports/"+std::to_string(p)));
            }
            if(const auto *stringContract=canonicalStringNativeContract(n.kind);stringContract&&stringContract->operation==CanonicalStringOperation::ConcatenateBytes)
                for(std::size_t p=0;p+1<n.ports.size();++p)refs.push_back(ref(src.pointer+"/input_ports/"+std::to_string(p)));
            if(const auto *scalar=canonicalScalarNativeContract(n.kind);scalar&&scalar->maximumInputCount)
                for(std::size_t p=0;p+1<n.ports.size();++p)refs.push_back(ref(src.pointer+"/input_ports/"+std::to_string(p)));
            if(n.objectReference)refs.push_back(ref(src.pointer+"/"+std::string(canonicalObjectReferenceAttribute)));
            if(n.nativeCall){refs.push_back(ref(src.pointer+"/native_call_profile"));refs.push_back(ref(src.pointer+"/native_call_configuration"));}
            for(const auto &p:n.ports){auto v=object(),metadata=object();metadata.object={{"source_node_id",string(n.sourceNodeId)},{"source_port_id",string(p.id)}};
                const std::string role=n.family!="structure"?"data":p.id=="condition"?"condition":p.id=="iteration"?"iteration":p.id=="selector"||p.id=="selected_case"?"selector":"data";
                v.object={{"id",string(p.id)},{"direction",string(p.input?"in":"out")},{"role",string(role)},{"type_ref",string(p.typeRef)},
                    {"boundary_kind",string(n.family=="structure"?"structure_boundary":"none")},{"metadata",std::move(metadata)}};ports.array.push_back(std::move(v));}
            record.object={{"id",string(n.id)},{"family",string(n.family)},{"kind",string(n.kind)},{"classification",std::move(classification)},{"ports",std::move(ports)},
                {"attributes",std::move(attributes)},{"attribution",attribution(refs)},{"correspondence_refs",strings(correspondenceRefs)}};
            if(!n.role.empty())record.object["role"]=string(n.role);
            if(n.family=="structure"){const auto s=std::find_if(out.structures.begin(),out.structures.end(),[&](const auto &s){return s.nodeId==n.id;});record.object["regions_owned"]=strings(s->regionIds);}
            objects.array.push_back(std::move(record));mapping(encodedId("map-node",n.sourceNodeId),n.family=="structure"?"structure":"diagram_node",src.pointer,n.id,"object");
            if(const auto *scalar=canonicalScalarNativeContract(n.kind);scalar&&scalar->maximumInputCount)
                for(std::size_t p=0;p+1<n.ports.size();++p)mapping(encodedId(std::string(scalar->catalogId=="numeric.add"?"map-numeric-add-input-":"map-boolean-input-")+std::to_string(p),n.sourceNodeId),"other",src.pointer+"/input_ports/"+std::to_string(p),n.id,"object");
            if(n.objectReference)mapping(encodedId("map-object-reference",n.sourceNodeId),"other",src.pointer+"/"+std::string(canonicalObjectReferenceAttribute),n.id,"object");
            if(n.nativeCall) {
                mapping(encodedId("map-native-call-profile",n.sourceNodeId),"other",src.pointer+"/native_call_profile",n.id,"object");
                mapping(encodedId("map-native-call-configuration",n.sourceNodeId),"other",src.pointer+"/native_call_configuration",n.id,"object");
                for(std::size_t i=0;i<n.nativeCall->arguments.size();++i)
                    mapping(encodedId("map-native-call-argument-"+std::to_string(i),n.sourceNodeId),"other",src.pointer+"/native_call_configuration/arguments/"+std::to_string(i),n.id,"object");
            }
            if(n.kind=="frog.core.unbundle.by_name") {
                mapping(encodedId("map-selection-version",n.sourceNodeId),"other",src.pointer+"/selected_fields_version",n.id,"object");
                for(std::size_t i=0;i<n.selectedFields.size();++i)for(const auto key:{"port","field_id"})mapping(encodedId("map-selection-"+std::to_string(i)+"-"+key,n.sourceNodeId),"other",src.pointer+"/selected_fields/"+std::to_string(i)+"/"+key,n.id,"object");
            }
            if(const auto *arrayContract=canonicalArrayNativeContract(n.kind)) {
                mapping(encodedId("map-array-profile",n.sourceNodeId),"other",src.pointer+"/array_execution_profile",n.id,"object");
                if(arrayContract->operation==CanonicalArrayOperation::Build)for(std::size_t p=0;p+1<n.ports.size();++p)
                    mapping(encodedId("map-array-input-"+std::to_string(p),n.sourceNodeId),"other",src.pointer+"/input_ports/"+std::to_string(p),n.id,"object");
            }
            if(const auto *stringContract=canonicalStringNativeContract(n.kind);stringContract&&stringContract->operation==CanonicalStringOperation::ConcatenateBytes)
                for(std::size_t p=0;p+1<n.ports.size();++p)mapping(encodedId("map-string-input-"+std::to_string(p),n.sourceNodeId),"other",src.pointer+"/input_ports/"+std::to_string(p),n.id,"object");
            if(n.family=="constant")for(const auto key:{"constant_profile","type","value"})
                mapping(encodedId("map-constant-"+std::string(key),n.sourceNodeId),"other",src.pointer+"/"+key,n.id,"object");
            for(const auto key:{"layout","doc","tags"})if(src.raw->member(key))correspondence(encodedId("node-"+std::string(key),n.sourceNodeId),src.pointer+"/"+key);
            if(n.family=="structure")for(const auto &[name,_]:src.raw->member("structure_terminals")->object)mapping(encodedId("map-terminal-"+name,n.sourceNodeId),"structure_terminal",src.pointer+"/structure_terminals/"+name,n.id,"object");
            if(n.family=="structure"){
                for(const auto side:{"inputs","outputs"}){
                    const auto &members=src.raw->member("boundary")->member(side)->array;
                    for(std::size_t t=0;t<members.size();++t)mapping(encodedId("map-boundary-"+std::string(side)+"-"+std::to_string(t),n.sourceNodeId),"structure_terminal",src.pointer+"/boundary/"+side+"/"+std::to_string(t),n.id,"object");
                }
                if(src.raw->member("structure_tunnels"))correspondence(encodedId("tunnel-placement",n.sourceNodeId),src.pointer+"/structure_tunnels");
            }
        }
        for(std::size_t i=0;i<out.connections.size();++i){const auto &e=out.connections[i];auto record=object(),from=object(),to=object(),metadata=object();
            from.object={{"object_id",string(e.from.objectId)},{"port_id",string(e.from.portId)}};to.object={{"object_id",string(e.to.objectId)},{"port_id",string(e.to.portId)}};
            const auto &raw=sourceEdges[i];const auto &endpoint=*raw.raw->member("from");const auto type=port(text(endpoint.member("node"),raw.pointer),e.from.portId,raw.pointer).typeRef;
            metadata.object={{"source_edge_id",string(e.sourceEdgeId)},{"type_ref",string(type)}};
            record.object={{"id",string(e.id)},{"kind",string("dataflow")},{"from",std::move(from)},{"to",std::move(to)},
                {"attribution",attribution({ref(raw.pointer)})},{"metadata",std::move(metadata)}};connections.array.push_back(std::move(record));
            mapping(encodedId("map-edge",e.sourceEdgeId),"diagram_edge",raw.pointer,e.id,"connection");
            if(raw.raw->member("bends"))correspondence(encodedId("edge-bends",e.sourceEdgeId),raw.pointer+"/bends");
        }
        for(const auto &r:out.regions){auto record=object();record.object={{"id",string(r.id)},{"owner_object_id",string(r.ownerNodeId)},{"role",string(r.role)},{"member_object_ids",strings(r.memberNodeIds)}};
            regions.array.push_back(std::move(record));const auto owner=std::find_if(out.nodes.begin(),out.nodes.end(),[&](const auto &n){return n.id==r.ownerNodeId;});const auto index=std::size_t(owner-out.nodes.begin());
            mapping(encodedId("map-region",owner->sourceNodeId)+":"+encodedId("",r.sourceRegionId).substr(1),"other",r.sourcePointer.empty()?sourceNodes[index].pointer+"/regions/0":r.sourcePointer,r.id,"region");}
        for(const auto &[wid,w]:widgets){const auto found=participants.find(wid);const std::string target=found==participants.end()?"":found->second;
            mapping(encodedId("map-widget",wid),"widget_instance",w.pointer,target,target.empty()?"none":"object","non_primary");
            correspondence(encodedId("declaration",wid),w.pointer,target,"declaration_reference");
            correspondence(encodedId("widget-class",wid),w.pointer+(w.raw->member("widget")?"/widget":"/class_ref"),target,"declaration_reference");
            for(const auto key:{"layout","parts","doc","tags"})if(w.raw->member(key))correspondence(encodedId("widget-"+std::string(key),wid),w.pointer+"/"+key);
            if(w.initial)mapping(encodedId("map-default",wid),"widget_member",w.pointer+(w.raw->member("props")->member("default_value")?"/props/default_value":"/props/value"),target,target.empty()?"none":"object",target.empty()?"non_primary":"direct_primary");
        }
        for(const auto key:{"connector","icon","ide","cache","host","execution_policy"})if(source.section(key))correspondence("section:"+std::string(key),"/"+std::string(key));
        const auto writeMappings=[&]{for(const auto &m:out.maps){auto v=object();v.object={{"id",string(m.id)},{"source_kind",string(m.sourceKind)},{"source_ref",string(m.sourceRef)},{"target_kind",string(m.targetKind)},{"target_refs",strings(m.targetRefs)},{"relation",string(m.relation)}};maps.array.push_back(std::move(v));}
            for(const auto &c:out.correspondence){auto v=object();v.object={{"id",string(c.id)},{"source_ref",string(c.sourceRef)},{"relation_kind",string(c.relationKind)},{"target_refs",strings(c.targetRefs)}};relations.array.push_back(std::move(v));}};writeMappings();
        if(out.maps.size()+out.correspondence.size()>options.limits.maximumMappingRecords)fail("IR_RESOURCE_LIMIT","Mapping budget exceeded.","/",true);
        meta.object={{"producer",string("frog.native.canonical-structured-ir")},{"producer_version",string("1")},{"notes",strings({"Accepted subset: frog.canonical.typed-structures.v1","Exact source bytes: "+out.hash,"Source URI: "+options.sourceUri})}};
        if(std::any_of(out.nodes.begin(),out.nodes.end(),[](const auto &n){return n.family=="constant"&&n.literalProfile!=canonicalStringConstantProfile;}))
            meta.object["notes"].array.push_back(string("Local extension: "+std::string(scalarConstantProfile)+"; not the published canonical constant family"));
        if(std::any_of(out.nodes.begin(),out.nodes.end(),[](const auto &n){return n.literalProfile==canonicalStringConstantProfile||canonicalStringNativeContract(n.kind);}))
            meta.object["notes"].array.push_back(string("Local extension: "+std::string(canonicalStringBytesProfile)+"; byte Length i32 and ordered scalar Concatenate, distinct from published abstract frog.text"));
        if(std::any_of(out.nodes.begin(),out.nodes.end(),[](const auto &n){return canonicalArrayNativeContract(n.kind);}))
            meta.object["notes"].array.push_back(string("Local extension: "+std::string(canonicalArrayReal1dProfile)+"; finite f64 Build/Index/Size, distinct from published Collections"));
        unit.object={{"id",string(out.unitId)},{"objects",std::move(objects)},{"connections",std::move(connections)},{"regions",std::move(regions)},{"source_map",std::move(maps)},{"correspondence",std::move(relations)}};
        root.object={{"schema_family",string("frog.execution_ir")},{"schema_version",string("0.1")},{"document_id",string(out.documentId)},{"metadata",std::move(meta)},{"unit",std::move(unit)}};
        out.bytes=encode(root);if(out.bytes.size()>options.limits.maximumIrBytes)fail("IR_RESOURCE_LIMIT","Structured canonical artifact exceeds its byte budget.","/",true);
    }
};
}
bool required(const FrogSourceDocument &source)noexcept{
    if(composed(source))return true;
    if(const auto *host=source.section("host");host&&!host->object.empty())return true;
    if(const auto *policy=source.section("execution_policy");policy&&!policy->object.empty())return true;
    if(const auto *diagram=source.section("diagram"))if(const auto *nodes=diagram->member("nodes"))for(const auto &n:nodes->array){
        const auto *kind=n.member("kind");if(kind&&kind->type==Json::Type::String&&(kind->string=="structure"||kind->string=="constant"))return true;
        const auto *type=n.member("type");if(kind&&kind->string=="primitive"&&type&&type->string!="frog.core.add")return true;
    }
    if(const auto *panel=source.section("front_panel"))if(const auto *widgets=panel->member("widgets"))for(const auto &w:widgets->array){const auto *type=w.member("value_type");if(type&&type->type==Json::Type::String&&type->string!="f64")return true;}
    return false;
}
std::optional<Draft> build(const FrogSourceDocument &source,std::string_view documentId,const CanonicalIrReadOptions &options,std::vector<CanonicalIrDiagnostic> *diagnostics){
    if(composed(source))return buildComposed(source,documentId,options,diagnostics);
    try{
        if(source.envelopeStatus()!=FrogSourceEnvelopeStatus::Recognized||!source.diagnostics().empty())fail("IR_SOURCE_UNSUPPORTED","A recognized, internally consistent public source is required.","/",true);
        Builder builder{source,options};builder.out.documentId=documentId;builder.out.unitId="unit:main";builder.out.hash="sha256:"+hashing::sha256Hex(source.originalUtf8());
        {const CanonicalFrontendPhaseScope semantic(options.metrics,CanonicalFrontendPhase::Semantics);
            builder.declarations();builder.scope(obj(source.section("diagram"),"/diagram"),"/diagram");builder.connectivity();}
        {const CanonicalFrontendPhaseScope construction(options.metrics,CanonicalFrontendPhase::FirConstruction);builder.write();}
        return std::move(builder.out);
    }catch(const Failure &failure){if(diagnostics)diagnostics->push_back(failure.diagnostic);return {};}
    catch(const std::exception &failure){if(diagnostics)diagnostics->push_back({"IR_INVALID_SOURCE",failure.what(),"/",false});return {};}
}
bool matches(std::string_view bytes,const Draft &expected,std::vector<CanonicalIrDiagnostic> *diagnostics){
    const auto actual=source::parseJson(bytes,nullptr,source::JsonNumberPolicy::PreserveLexeme),reference=source::parseJson(expected.bytes,nullptr,source::JsonNumberPolicy::PreserveLexeme);
    if(actual&&reference&&exact(*actual,*reference))return true;
    if(diagnostics)diagnostics->push_back({"IR_SOURCE_ARTIFACT_MISMATCH","The supplied structured IR differs from the independently admitted authoritative source, including types, regions, mappings or exact values.","/",false});return false;
}
}
