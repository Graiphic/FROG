#include "frog_native_engine/ApplicationControlSchema.hpp"
#include "frog_native_engine/graph/XYGraphReferenceContract.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/WidgetCatalog.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/TabControl.hpp"
#include "frog_native_engine/QueueReferenceLiteral.hpp"
#include "frog_native_engine/NotifierReferenceLiteral.hpp"
#include "frog_native_engine/SemaphoreReferenceLiteral.hpp"
#include "frog_native_engine/RendezvousReferenceLiteral.hpp"
#include "frog_native_engine/OccurrenceReferenceLiteral.hpp"
#include "frog_native_engine/EventReferenceLiteral.hpp"
#include "frog_native_engine/DiagramLiteralSchema.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include <tuple>
#include <algorithm>
#include <initializer_list>
#include <set>
#include <utility>

namespace frog::native_engine {
namespace {
using binding::TypeRef;
TypeRef primitive(std::string_view value) { return TypeRef::primitive(value); }
TypeRef settingType(std::string domain,std::initializer_list<std::string_view> choices) {
    auto type=TypeRef::enumeration(std::move(domain),primitive("u8"));
    type.choiceKind=binding::ChoiceKind::Enum;
    for(const auto choice:choices)type.choices.push_back({std::string(choice),std::string(choice),
        static_cast<std::uint32_t>(type.choices.size()),false,true});
    return type;
}
TypeRef pointType() { return TypeRef::cluster({"x","y"},{primitive("f64"),primitive("f64")}); }
TypeRef sizeType() { return TypeRef::cluster({"width","height"},{primitive("f64"),primitive("f64")}); }
TypeRef rectType() { return TypeRef::cluster({"x","y","width","height"},
    {primitive("f64"),primitive("f64"),primitive("f64"),primitive("f64")}); }
std::string_view referenceWidgetBase(std::string_view id) {
    if(id=="frog.control.color_box" || id=="frog.control.timestamp")return "frog.widgets.numeric_control";
    if(id=="frog.control.dropdown_string")return "frog.widgets.string_control";
    if(id=="frog.control.error_cluster")return "frog.widgets.cluster_control";
    return id;
}
std::string memberDisplayName(const ApplicationControlMemberDescriptor &member) {
    std::string label;
    for(const auto &part:member.menuPath){if(!label.empty())label+='.';label+=part;}
    if(!label.empty())label+='.';
    return label+member.label;
}
TypeRef effectiveReference(const ApplicationControlNode &node, const TypeRef &wired) {
    return wired.kind == binding::TypeKind::Unknown ||
        (node.kind==ApplicationControlKind::Property && node.target.kind!=ApplicationControlTargetKind::Unbound)
        ? applicationControlReferenceType(node) : wired;
}
bool stableId(std::string_view id) {
    return !id.empty() && std::all_of(id.begin(),id.end(),[](unsigned char c) {
        return (c>='a'&&c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c=='_' || c=='-' || c=='.';
    });
}
}
// Pure reference type construction belongs with typed reference values.
// It is shared by numeric payload validation without linking the editor/document.
binding::TypeRef applicationControlReferenceType(const ApplicationControlNode &node) {
    return binding::TypeRef::reference(node.expectedClass,node.valueType.known()?std::optional{node.valueType}:std::nullopt);
}

std::optional<std::uint32_t> applicationControlRingItemMaximum(const TypeRef &valueType) noexcept {
    if(valueType.kind!=binding::TypeKind::Primitive)return {};
    if(valueType.name=="u8")return 0xFFU;
    if(valueType.name=="u16")return 0xFFFFU;
    if(valueType.name=="u32" || valueType.name=="u64")return 0xFFFFFFFFU;
    return {};
}
std::vector<std::string> applicationControlReferenceClasses() {
    std::vector<std::string> result;
    for(const auto classId:binding::nativeObjectReferenceClasses())result.emplace_back(classId);
    for (const auto &definition : widgetDefinitions()) result.emplace_back(definition.classId);
    // File already has an independent resource reference contract.
    result.emplace_back("file");
    return result;
}
static std::vector<ApplicationControlMemberDescriptor> baseApplicationControlMembers(std::string_view classId,
    const TypeRef &valueType) {
    const auto classes=applicationControlReferenceClasses();
    if (std::find(classes.begin(),classes.end(),classId)==classes.end() &&
        !findWidgetDefinitionByClassId(classId)) return {};
    std::vector<ApplicationControlMemberDescriptor> result;
    const auto property=[&](std::string id,std::string label,std::string group,TypeRef type,
            bool read=true,bool write=false,bool available=true,std::string reason={}) {
        ApplicationControlMemberDescriptor item;
        item.id=std::move(id);item.label=std::move(label);item.group=std::move(group);
        item.type=std::move(type);item.readable=read;item.writable=write;
        item.authoringAvailable=available;item.unavailableReason=std::move(reason);
        item.help="Private document authoring provider; runtime access is unavailable.";
        if(item.id=="position" || item.id=="bounds" || item.id=="size")
            item.help="Logical document/container coordinates in model units. Root writes use the canvas bounds service and its minimum sizes; contained geometry is read-only here. Runtime access is unavailable.";
        else if(item.id.ends_with(".font.size"))
            item.help="Effective text size in points, including inherited size. A write creates an override for this part; Use Default Font restores inheritance. Runtime access is unavailable.";
        else if(item.id=="array_element")
            item.help="Reference to the element presentation template. Value/default access is unavailable on this object; access cells through Array data operations.";
        else if(item.id=="numeric.minimum" || item.id=="numeric.maximum" || item.id=="numeric.increment")
            item.help="Typed Numeric Data Entry authoring property. The current model stores limits/steps as f64; values that cannot be represented exactly are rejected. Complex policy and runtime provider are unavailable.";
        else if(item.id=="numeric.radix")
            item.help="View of the existing integer display format: Decimal=10, Hexadecimal=16, Octal=8, Binary=2. A write changes that format; it does not create an independent radix state.";
        else if(item.id=="items.strings")
            item.help="EDIT only. Position edits preserve existing item identities/values/flags; appended items use the existing next-free U32 transport. Removing the selected item uses the existing setter: first enabled Numeric item, first visible enabled Dropdown item, then its empty-domain policy. Enum schema changes use the shared document command and terminal/schema synchronization; runtime mutation is unavailable.";
        else if(item.id=="items.strings_and_values")
            item.help="EDIT only. Values retain the target unsigned representation T and the existing U32 item storage; U8/U16 reject values outside their range. Labels/values remain associated, with existing row IDs/flags preserved. Removing the selected value uses the existing Ring setter: first enabled item when undefined values are disabled; otherwise retain the value. Runtime mutation and signed/floating item storage are unavailable.";
        else if(item.id=="label.transparent")
            item.help="Transparent uses the existing transparent-color model. To make a transparent label opaque, set an explicit Background Color first; no inherited color is invented.";
        result.push_back(std::move(item));
    };
    const auto method=[&](std::string id,std::string label,bool available,
            std::vector<ApplicationControlParameter> params={},std::optional<TypeRef> output={}) {
        ApplicationControlMemberDescriptor item;
        item.kind=ApplicationControlMemberKind::Method;item.id=std::move(id);item.label=std::move(label);
        item.group="Methods";item.authoringAvailable=available;item.parameters=std::move(params);item.resultType=std::move(output);
        if(!available)item.unavailableReason="No provider for this operation in the current authoring context.";
        item.help="No runtime or execution side effect is implied by configuring this method.";
        result.push_back(std::move(item));
    };
    property("identity.object_id","Object ID","Identity",primitive("string"));
    property("identity.class","Class","Identity",primitive("string"));
    property("identity.owner","Owner","Identity",TypeRef::reference("frog.object"));
    const auto *definition=findWidgetDefinitionByClassId(referenceWidgetBase(classId));
    const bool control=definition || classId=="frog.control";
    if(!control) {
        if(classId=="frog.program") {
            property("front_panel","Front Panel","References",TypeRef::reference("frog.front_panel"));
            property("front_panel.open_state","Front Panel.Open State","Front Panel",primitive("bool"),true,false,true);
            property("execution.state","Execution State","Execution",primitive("string"),true,false,false,"Runtime provider unavailable.");
            method("front_panel.open","Front Panel.Open",false,{{"activate","Activate",primitive("bool"),ApplicationControlParameterDirection::In,false,"true"}});
            method("front_panel.close","Front Panel.Close",false);
        }
        if(classId=="frog.front_panel") {
            property("controls","Controls[]","References",TypeRef::array(TypeRef::reference("frog.control")));
            method("find_child","Find Child",true,{{"path","Path",primitive("string")}},TypeRef::reference("frog.control"));
        }
        return result;
    }
    const auto cap=[&](WidgetCapability c){return definition && widgetDefinitionHasCapability(*definition,c);};
    const TypeRef dataType=valueType.known()?valueType:primitive("variant");
    property("identity.role","Role","Identity",primitive("string"));
    const bool hasValue=!definition || definition->terminal.direction!=WidgetTerminalDirection::None;
    if(hasValue) {
        property("value","Value","Value",dataType,true,true);
        property("default_value","Default Value","Value",dataType,true,true);
        property("value_type","Value Type","Value",primitive("string"));
        property("value_signaling","Value (Signaling)","Value",dataType,false,true,false,"Event publication provider unavailable.");
    }
    property("interaction.visible","Visible","Appearance",primitive("bool"),true,true);
    property("interaction.enabled","Enabled","Appearance",primitive("bool"),true,true,!definition || cap(WidgetCapability::EnabledState),
        !definition || cap(WidgetCapability::EnabledState)?"":"The widget has no enabled-state provider.");
    auto enabledType=TypeRef::enumeration("frog.widget.enabled_state",primitive("u8"));
    enabledType.choiceKind=binding::ChoiceKind::Enum;
    enabledType.choices={{"enabled","Enabled",0,false,true},{"disabled","Disabled",1,false,true},
        {"grayed","Disabled and Grayed",2,false,true}};
    property("interaction.disabled_state","Disabled","Appearance",std::move(enabledType),true,true,
        !definition || cap(WidgetCapability::EnabledState),!definition || cap(WidgetCapability::EnabledState)?"":"The widget has no enabled-state provider.");
    property("description","Description","Help",primitive("string"),true,true);
    property("tip_strip","Tip Strip","Help",primitive("string"),true,true);
    property("bounds","Bounds","Layout",rectType());
    property("position","Position","Layout",pointType(),true,true);
    property("size","Size","Layout",sizeType(),true,true);
    if(!definition || cap(WidgetCapability::Label)) {
        property("label.text","Label.Text","Label",primitive("string"),true,true);
        property("label.visible","Label.Visible","Label",primitive("bool"),true,true);
        property("caption.text","Caption.Text","Caption",primitive("string"),true,true);
        property("caption.visible","Caption.Visible","Caption",primitive("bool"),true,true);
    }
    const auto font=[&](std::string_view part,std::string_view label,bool styles=true) {
        const auto prefix=std::string(part)+".";
        property(prefix+"font.name",std::string(label)+".Font.Name",std::string(label),primitive("string"),true,true);
        property(prefix+"font.size",std::string(label)+".Font.Size",std::string(label),primitive("f64"),true,true);
        property(prefix+"use_default_font",std::string(label)+".Use Default Font",std::string(label),primitive("bool"),true,true);
        if(styles) {
            property(prefix+"font.bold",std::string(label)+".Font.Bold",std::string(label),primitive("bool"),true,true);
            property(prefix+"font.italic",std::string(label)+".Font.Italic",std::string(label),primitive("bool"),true,true);
        }
    };
    if(!definition || cap(WidgetCapability::Font)) {
        font("label","Label");font("caption","Caption",false);
        if(definition && (definition->family==WidgetFamily::Numeric || definition->family==WidgetFamily::StringPath ||
            definition->family==WidgetFamily::Enum || definition->family==WidgetFamily::Ring))font("text","Text");
        if(definition && (definition->family==WidgetFamily::Button || definition->family==WidgetFamily::Boolean))font("boolean_text","Boolean Text");
    }
    if(!definition || cap(WidgetCapability::Color)) {
        property("label.text_color","Label.Text Color","Label",primitive("u32"),true,true);
        property("label.background_color","Label.Background Color","Label",primitive("u32"),true,true);
        property("label.transparent","Label.Transparent","Label",primitive("bool"),true,true);
        if(definition && (definition->family==WidgetFamily::Numeric || definition->family==WidgetFamily::StringPath ||
            definition->family==WidgetFamily::Enum || definition->family==WidgetFamily::Ring))
            property("text.text_color","Text.Text Color","Text",primitive("u32"),true,true);
        if(definition && (definition->family==WidgetFamily::Button || definition->family==WidgetFamily::Boolean))
            property("boolean_text.text_color","Boolean Text.Text Color","Boolean Text",primitive("u32"),true,true);
    }
    property("label.alignment","Label.Alignment","Label",primitive("string"),true,true);
    if(definition && (definition->family==WidgetFamily::Button || definition->family==WidgetFamily::Boolean)) {
        property("boolean_text.false","Boolean Text.False","Boolean Text",primitive("string"),true,true);
        property("boolean_text.true","Boolean Text.True","Boolean Text",primitive("string"),true,true);
        property("boolean_text.visible","Boolean Text.Visible","Boolean Text",primitive("bool"),true,true);
    }
    if(cap(WidgetCapability::MechanicalAction)) {
        property("mechanical_action","Mechanical Action","Operation",primitive("string"),true,true);
    }
    if(cap(WidgetCapability::IncrementDecrement))
        property("numeric.step_visible","Increment/Decrement Visible","Appearance",primitive("bool"),true,true);
    if(cap(WidgetCapability::DataEntry) && definition->family==WidgetFamily::Numeric) {
        const bool policy=valueType.kind!=binding::TypeKind::Primitive ||
            (valueType.name!="c32" && valueType.name!="c64" && valueType.name!="bigint" &&
             valueType.name!="biguint" && valueType.name!="timestamp");
        const std::string reason=policy?"":"The current Data Entry model has no policy for complex, arbitrary-width or timestamp bounds/steps.";
        property("numeric.minimum","Data Entry.Minimum","Data Entry",dataType,true,true,policy,reason);
        property("numeric.maximum","Data Entry.Maximum","Data Entry",dataType,true,true,policy,reason);
        property("numeric.increment","Data Entry.Increment","Data Entry",dataType,true,true,policy,reason);
    }
    if(definition && definition->family==WidgetFamily::Numeric) {
        const bool numericDisplay=!(valueType.kind==binding::TypeKind::Primitive && valueType.name=="timestamp");
        property("numeric.format","Display.Format","Display",primitive("string"),true,true,numericDisplay,
            numericDisplay?"":"Timestamp uses its own display provider.");
        property("numeric.precision","Display.Precision","Display",primitive("i32"),true,true,numericDisplay,
            numericDisplay?"":"Timestamp uses its own precision provider.");
        auto radix=TypeRef::enumeration("frog.numeric.radix",primitive("u8"));
        radix.choiceKind=binding::ChoiceKind::Enum;
        radix.choices={{"decimal","Decimal",10,false,true},{"hexadecimal","Hexadecimal",16,false,true},
            {"octal","Octal",8,false,true},{"binary","Binary",2,false,true}};
        const bool integerDisplay=valueType.kind==binding::TypeKind::Unknown ||
            (valueType.kind==binding::TypeKind::Primitive && numericCanonicalValueTypeIsIntegral(valueType.name));
        property("numeric.radix","Display.Radix","Display",std::move(radix),true,true,integerDisplay,
            integerDisplay?"":"The current Numeric editor exposes radix formats only for integer representations.");
        if(classId=="frog.control.timestamp" || (valueType.kind==binding::TypeKind::Primitive && valueType.name=="timestamp")) {
            property("timestamp.display_content","Displayed Information","Timestamp",
                settingType("frog.timestamp.display_content",{"date_time","date","time"}),true,true);
            property("timestamp.precision","Time Precision","Timestamp",
                settingType("frog.timestamp.precision",{"seconds","minutes"}),true,true);
            property("timestamp.order","Order","Timestamp",
                settingType("frog.timestamp.order",{"time_date","date_time"}),true,true);
            property("timestamp.layout","Layout","Timestamp",
                settingType("frog.timestamp.layout",{"two_lines","single_line"}),true,true);
            property("timestamp.show_seconds","Show Seconds","Timestamp",primitive("bool"),true,true);
            property("timestamp.date_first","Date First","Timestamp",primitive("bool"),true,true);
            property("timestamp.single_line","Single Line","Timestamp",primitive("bool"),true,true);
            property("timestamp.clock_icon_visible","Clock Icon Visible","Timestamp",primitive("bool"),true,true);
        }
    }
    if(definition && definition->family==WidgetFamily::StringPath && classId.find("string")!=std::string_view::npos) {
        property("string.text","Text","Text",primitive("string"),true,true);
        property("string.display_style","Display Style","Display",primitive("string"),true,true);
        property("string.word_wrap","Word Wrap","Display",primitive("bool"),true,true);
        property("string.multiline","Multiline","Display",primitive("bool"),true,true);
        property("string.scrollbar","Scrollbar Visible","Display",primitive("bool"),true,true);
        property("string.selection_start","Selection.Start","Text",primitive("i32"),true,true);
        property("string.selection_end","Selection.End","Text",primitive("i32"),true,true);
        property("string.selected_text","Selected Text","Text",primitive("string"),true,false);
        method("string.select_all","Select All",true);
        method("string.select_text","Select Text",true,{{"start","Start",primitive("i32")},{"end","End",primitive("i32")}});
        for(auto &member:result)if(member.id.starts_with("string.selection_") ||
            member.id=="string.selected_text" || member.id=="string.select_all" || member.id=="string.select_text")
            member.help="Native active String editing session only. Indices are UTF-16 code units; ranges must be ordered, within the text and must not split a surrogate pair. Selection changes neither the value nor document history, and never opens an editor. Runtime provider unavailable.";
    }
    const bool stringChoices=definition && definition->family==WidgetFamily::StringPath && classId.find("string")!=std::string_view::npos;
    if(cap(WidgetCapability::Items) || stringChoices) {
        property("items.strings",definition->family==WidgetFamily::Enum?"Items.Strings[]":"Strings[]",
            "Items",TypeRef::array(primitive("string")),true,true);
        property("items","Items","Items",TypeRef::array(TypeRef::cluster({"id","label","value","disabled","visible"},
            {primitive("string"),primitive("string"),primitive("u32"),primitive("bool"),primitive("bool")})),true,true,
            !stringChoices,stringChoices?"Dropdown labels and values are Strings. Internal numeric item identities are not an Application Control value list.":"");
        property("selected_item_text","Selected Item Text","Items",primitive("string"));
        if(definition->family==WidgetFamily::Ring || definition->family==WidgetFamily::Enum) {
            property("numeric.format","Display.Format","Display",primitive("string"),true,true);
            property("choice.numeric_value_visible","Numeric Value Visible","Appearance",primitive("bool"),true,true);
        }
        if(definition->family==WidgetFamily::Ring) {
            property("ring.allow_undefined_values","Allow Undefined Values","Items",primitive("bool"),true,true);
            const bool storedValues=applicationControlRingItemMaximum(dataType).has_value();
            property("items.strings_and_values","Strings And Values[]","Items",
                TypeRef::array(TypeRef::cluster({"label","value"},{primitive("string"),dataType})),true,true,storedValues,
                storedValues?"":"The current RingItem model stores nonnegative U32 item values; signed and floating item storage is unavailable.");
        }
    }
    if(cap(WidgetCapability::ClusterContainer)) {
        property("controls","Controls[]","References",TypeRef::array(TypeRef::reference("frog.control")));
        method("find_child","Find Child",true,{{"path","Path",primitive("string")}},TypeRef::reference("frog.control"));
    }
    if(cap(WidgetCapability::ArrayContainer)) {
        property("array_element","Array Element","References",TypeRef::reference("frog.control"));
        const auto indices=TypeRef::array(primitive("u32"));
        property("array_size","Array Size","Array",indices);
        property("array.index_values","Index Values","Array",indices,true,true);
        property("array.visible_elements","Visible Elements","Array",indices,true,true);
        property("array.index_visible","Index Display Visible","Array",primitive("bool"),true,true);
        property("array.vertical_scrollbar","Vertical Scrollbar Visible","Array",primitive("bool"),true,true);
        property("array.horizontal_scrollbar","Horizontal Scrollbar Visible","Array",primitive("bool"),true,true);
    }
    if(hasValue) {
        method("reinitialize","Reinitialize To Default",true);
        method("make_default","Make Current Value Default",true);
    }
    method("set_focus","Set Focus",false);
    return result;
}
std::string applicationControlReferenceClass(const WidgetInstance &widget) {
    if(widget.classId=="frog.studio.widgets.collection")return "frog.control";
    if(isNumericColorBox(widget))return "frog.control.color_box";
    if(isStringDropdownWidget(widget))return "frog.control.dropdown_string";
    const auto type=binding::widgetType(widget);
    if(type.valid() && type.type.kind==binding::TypeKind::Primitive && type.type.name=="timestamp")
        return "frog.control.timestamp";
    if(isClusterWidget(widget) && type.valid() && binding::isErrorCluster(type.type) &&
        type.type.fieldIds==std::vector<std::string>{"status","code","source"})
        return "frog.control.error_cluster";
    return widget.classId;
}
namespace {
using Descriptor=ApplicationControlMemberDescriptor;
using Direction=ApplicationControlParameterDirection;
std::vector<Descriptor> referenceCatalogue(std::string_view classId,const TypeRef &valueType,
    const WidgetInstance *presentation) {
    auto result=baseApplicationControlMembers(classId,valueType);
    if(result.empty())return result;
    const auto *definition=findWidgetDefinitionByClassId(referenceWidgetBase(classId));
    const bool control=definition || classId=="frog.control";
    const bool text=classId=="frog.text.label" || classId=="frog.text.caption" || classId=="frog.text.boolean";
    const auto property=[&](std::string id,std::string label,TypeRef type,bool read=true,
        bool write=false,bool available=true,std::string reason={}) {
        Descriptor member;member.id=std::move(id);member.label=std::move(label);
        member.type=std::move(type);member.readable=read;member.writable=write;
        member.authoringAvailable=available;member.unavailableReason=std::move(reason);
        member.help="The descriptor configures a typed operation; target capabilities are checked separately. Runtime access requires its own provider.";
        result.push_back(std::move(member));
    };
    const auto method=[&](std::string id,std::string label,bool available,
        std::vector<ApplicationControlParameter> params={},std::optional<TypeRef> output={}) {
        Descriptor member;member.id=std::move(id);member.label=std::move(label);
        member.kind=ApplicationControlMemberKind::Method;member.authoringAvailable=available;
        member.parameters=std::move(params);member.resultType=std::move(output);
        if(!available)member.unavailableReason="The required service is not available in this provider.";
        result.push_back(std::move(member));
    };
    const auto unavailable=[&](std::string id,std::string label,TypeRef type,std::string reason,
        bool read=true,bool write=true) {property(std::move(id),std::move(label),std::move(type),read,write,false,std::move(reason));};
    if(classId==xy::classId) {
        auto graph=xy_reference::members();result.insert(result.end(),std::make_move_iterator(graph.begin()),std::make_move_iterator(graph.end()));
    }
    property("identity.class_name","Class Name",primitive("string"));
    if(control || text || classId=="frog.front_panel" || classId=="frog.pane") {
        property("identity.owning_frog","Owning FROG",TypeRef::reference("frog.program"));
        property("identity.owning_pane","Owning Pane",TypeRef::reference("frog.pane"));
    }
    if(control) {
        property("indicator","Indicator",primitive("bool"));
        unavailable("interaction.blinking","Blinking",primitive("bool"),"A blinking service is not registered.");
        unavailable("interaction.mouse_wheel","Built-In Mouse Wheel Support",primitive("bool"),"Per-control wheel policy is not exposed by this provider.");
        unavailable("control_index","Control Index",primitive("i32"),"Indexed terminal access is not registered; an object ID is not an index.",true,false);
        unavailable("focus_key_binding","Focus Key Binding",primitive("variant"),"A public key-binding service is not registered.");
        unavailable("key_focus","Key Focus",primitive("bool"),"The native focus service is not registered for property access.");
        unavailable("skip_when_tabbing","Skip When Tabbing",primitive("bool"),"A per-control Tab order service is not registered.");
        unavailable("synchronous_display","Synchronous Display",primitive("bool"),"The provider cannot guarantee synchronous rendering.");
        unavailable("data_binding.configuration","Configuration",primitive("variant"),"Data Binding provider is unavailable.");
        unavailable("datasocket.configuration","Configuration",primitive("variant"),"DataSocket provider is unavailable.");
        unavailable("xcontrol.configuration","Configuration",primitive("variant"),"XControl compatibility provider is unavailable.");
        method("object_highlight","Object Highlight",true);
        method("fit_control_to_pane","Fit Control To Pane",false);
        method("get_image","Get Image",false);
        method("get_terminal_image","Get Terminal Image",false);
        method("start_drag","Start Drag",false);
        method("datasocket.attach","Attach DataSocket",false);
        method("datasocket.remove","Remove DataSocket",false);
        method("data_binding.bind","Bind",false);
        method("data_binding.unbind","Unbind",false);
        if(!definition || widgetDefinitionHasCapability(*definition,WidgetCapability::Label)) {
            property("label.reference","Reference",TypeRef::reference("frog.text.label"));
            property("caption.reference","Reference",TypeRef::reference("frog.text.caption"));
        }
        if(definition && (definition->family==WidgetFamily::Boolean || definition->family==WidgetFamily::Button))
            property("boolean_text.reference","Reference",TypeRef::reference("frog.text.boolean"));
        property("bounds.area_height","Area Height",primitive("f64"));
        property("bounds.area_width","Area Width",primitive("f64"));
        property("position.left","Left",primitive("f64"),true,true);
        property("position.top","Top",primitive("f64"),true,true);
        property("size.width","Width",primitive("f64"),true,true);
        property("size.height","Height",primitive("f64"),true,true);
    }
    if(text) {
        property("text","Text",primitive("string"),true,true);
        property("interaction.visible","Visible",primitive("bool"),true,true);
        property("bounds","All Elements",rectType());
        property("bounds.area_height","Area Height",primitive("f64"));
        property("bounds.area_width","Area Width",primitive("f64"));
        property("position","All Elements",pointType());
        property("position.left","Left",primitive("f64"));
        property("position.top","Top",primitive("f64"));
        property("size","All Elements",sizeType());
        property("size.width","Width",primitive("f64"));
        property("size.height","Height",primitive("f64"));
        property("attached_object","Attached Object",TypeRef::reference("frog.object"));
        property("font.name","Name",primitive("string"),true,true);
        property("font.size","Size",primitive("f64"),true,true);
        unavailable("font.all","All Elements",TypeRef::cluster({"name","size","bold","italic","underline","color"},
            {primitive("string"),primitive("f64"),primitive("bool"),primitive("bool"),primitive("bool"),primitive("u32")}),
            "The provider does not offer an atomic complete FontSpec operation.");
        property("use_default_font","Use Default Font",primitive("bool"),true,true);
        const bool styles=classId!="frog.text.caption";
        property("font.bold","Bold",primitive("bool"),true,true,styles,
            styles?"":"Caption does not store an independent bold setting.");
        property("font.italic","Italic",primitive("bool"),true,true,styles,
            styles?"":"Caption does not store an independent italic setting.");
        property("font.underline","Underline",primitive("bool"),true,true,styles,
            styles?"":"Caption does not store an independent underline setting.");
        property("text_color","Text",primitive("u32"),true,true,classId!="frog.text.caption",
            classId=="frog.text.caption"?"Caption has no independent text color field.":"");
        property("background_color","Background",primitive("u32"),true,true,classId=="frog.text.label",
            classId=="frog.text.label"?"":"This text part has no background color field.");
        property("transparent","Transparent",primitive("bool"),true,true,classId=="frog.text.label",
            classId=="frog.text.label"?"":"This text part has no independent transparency field.");
        property("alignment","Justification",primitive("string"),true,true,classId=="frog.text.label",
            classId=="frog.text.label"?"":"This text part has no independent justification field.");
        unavailable("lock","Lock",primitive("bool"),"The text part has no separate manipulation lock.");
        unavailable("scroll_position","Scroll Position",pointType(),"This text part has no independent scroll state.");
        unavailable("enable_hyperlinks","Enable Hyperlinks",primitive("bool"),"Hyperlink rendering is not registered for this text part.");
    }
    const bool stringClass=definition && definition->family==WidgetFamily::StringPath &&
        referenceWidgetBase(classId).find("string")!=std::string_view::npos;
    if(stringClass) {
        method("string.append_string","Append String",true,{{"text","String",primitive("string")}});
        method("string.byte_offset_from_point","Byte Offset from Point",true,{{"point","Point",pointType()}},primitive("i32"));
        method("string.get_nth_line","Get Nth Line",true,{{"line_index","Line Index",primitive("u32")},
            {"line_text","Line Text",primitive("string"),Direction::Out},
            {"start","Start Byte Offset",primitive("u32"),Direction::Out},
            {"end","End Byte Offset",primitive("u32"),Direction::Out}});
        method("string.size_to_text","Size to Text",true);
        property("string.horizontal_scrollbar","Horizontal Scrollbar Visible",primitive("bool"),true,true);
        property("string.vertical_scrollbar","Vertical Scrollbar Visible",primitive("bool"),true,true);
        property("string.update_while_typing","Update While Typing",primitive("bool"),true,true);
        unavailable("string.allow_dragging","Allow Dragging",primitive("bool"),"A text drag data service is not registered.");
        unavailable("string.allow_dropping","Allow Dropping",primitive("bool"),"A text drop data service is not registered.");
        if(classId=="frog.control.dropdown_string" || valueType.choiceKind==binding::ChoiceKind::Dropdown) {
            property("dropdown.strings_and_values","Strings And Values[]",TypeRef::array(TypeRef::cluster(
                {"label","value"},{primitive("string"),primitive("string")})),true,true);
            unavailable("dropdown.allow_undefined_strings","Allow Undefined Strings",primitive("bool"),"The current Dropdown String uses a closed item list.");
        }
    }
    if(definition && (definition->family==WidgetFamily::Ring || definition->family==WidgetFamily::Enum)) {
        const auto numeric=valueType.kind==binding::TypeKind::Enum && valueType.children.size()==1?valueType.children.front():valueType;
        unavailable("numeric.minimum","Minimum",numeric.known()?numeric:primitive("variant"),"Named Numeric limits are not exposed by the current provider.");
        unavailable("numeric.maximum","Maximum",numeric.known()?numeric:primitive("variant"),"Named Numeric limits are not exposed by the current provider.");
        unavailable("numeric.increment","Increment",numeric.known()?numeric:primitive("variant"),"Named Numeric increments are not exposed by the current provider.");
    }
    if(definition && referenceWidgetBase(classId).find("path")!=std::string_view::npos) {
        property("path.browse_button_visible","Browse Button Visible",primitive("bool"),true,true);
        method("path.browse_for_path","Browse for Path",false);
        unavailable("path.browse_mode","Mode",primitive("string"),"A public per-control browse-mode configuration is unavailable.");
        unavailable("path.match_pattern","Match Pattern",primitive("string"),"A public per-control browse-filter configuration is unavailable.");
    }
    if(definition && widgetDefinitionHasCapability(*definition,WidgetCapability::ArrayContainer)) {
        const WidgetInstance *element=nullptr;
        if(presentation)if(const auto *array=arrayContainerState(*presentation))element=array->elementTemplate.get();
        TypeRef elementType=valueType.kind==binding::TypeKind::Array && !valueType.children.empty()?valueType.children.front():TypeRef{};
        for(auto &member:result)if(member.id=="array_element") {
            if(element) {
                const auto schema=binding::widgetType(*element);
                member.type=TypeRef::reference(applicationControlReferenceClass(*element),
                    schema.valid() && schema.type.known()?std::optional{schema.type}:std::nullopt);
            } else member.type=TypeRef::reference("frog.control",elementType.known()?std::optional{elementType}:std::nullopt);
            member.migrationAliases.push_back("array_element.reference");
        }
        unavailable("array.selection_start","Selection Start[]",TypeRef::array(primitive("u32")),"The Array model does not store a cell selection range.");
        unavailable("array.selection_size","Selection Size[]",TypeRef::array(primitive("u32")),"The Array model does not store a cell selection range.");
        unavailable("array.index_rect","Index Rect",rectType(),"Measured index bounds are not exposed by the current provider.",true,false);
    }
    if(classId=="frog.program") {
        property("name","Name",primitive("string"));property("path","Path",primitive("path"));
        property("modified","Modified",primitive("bool"));
        property("execution.instance_id","Execution Instance ID",primitive("string"));
        property("application","Application",TypeRef::reference("frog.application"));
        method("save","Save",true);
        for(auto &member:result)if(member.id=="execution.state" || member.id=="front_panel.open" || member.id=="front_panel.close") {
            member.authoringAvailable=true;member.unavailableReason.clear();
        }
        unavailable("connector_pattern","Connector Pattern",primitive("variant"),"A public connector metadata provider is not registered.",true,false);
        unavailable("dependencies","Dependencies",TypeRef::array(primitive("path")),"A dependency provider is not registered.",true,false);
        method("run","Run",false);method("abort","Abort",false);
        method("pause","Pause",false);method("resume","Resume",false);
        method("save_as","Save As",true,{{"path","Path",primitive("path")}});method("compile","Compile",false);
    }
    if(classId=="frog.application") {
        for(const auto &[id,label]:std::initializer_list<std::pair<const char*,const char*>>{
            {"name","Name"},{"version","Version"},{"kind","Kind"},{"active_frog_name","Active FROG Name"}})
            property(id,label,primitive("string"));
        property("executable_path","Executable Path",primitive("path"));
        property("application_directory","Application Directory",primitive("path"));
        property("command_line_arguments","Command Line Arguments",TypeRef::array(primitive("string")));
        property("active_frog","Active FROG Reference",TypeRef::reference("frog.program"));
        property("open_frogs","Open FROGs[]",TypeRef::array(TypeRef::reference("frog.program")));
        unavailable("projects","Projects[]",TypeRef::array(TypeRef::reference("frog.project")),"Project capability unavailable.",true,false);
        unavailable("active_project","Active Project",TypeRef::reference("frog.project"),"Project capability unavailable.",true,false);
        method("application.new_frog","New FROG",true,{},TypeRef::reference("frog.program"));
        method("application.open_frog","Open FROG",true,{{"path","Path",primitive("path")}},TypeRef::reference("frog.program"));
        method("application.resolve_frog","Resolve FROG",true,{{"path","Path",primitive("path")}},TypeRef::reference("frog.program"));
        method("project.new","New",false);method("project.open","Open",false);
    }
    if(classId=="frog.front_panel")property("panes","Panes[]",TypeRef::array(TypeRef::reference("frog.pane")));
    if(classId=="frog.pane") {
        property("controls","Controls[]",TypeRef::array(TypeRef::reference("frog.control")));
        unavailable("origin","Origin",pointType(),"The provider has no separate Pane-origin service.");
        unavailable("content_area_rect","Content Area Rect",rectType(),"Measured Pane content bounds are not exposed.",true,false);
        unavailable("background_color","Background Color",primitive("u32"),"A separate Pane background service is not registered.");
    }
    if(classId=="file") {
        property("file.valid","Valid",primitive("bool"),true,false,false,"The file manager has no public handle-inspection API.");
        property("file.path","Path",primitive("path"),true,false,false,"The file manager has no public handle-path inspection API.");
        property("file.position","Position",primitive("i64"),true,true,false,"File access requires the runtime handle provider.");
        property("file.size","Size",primitive("i64"),true,true,false,"File access requires the runtime handle provider.");
        property("file.access_mode","Access Mode",primitive("string"),true,false,false,"The file manager has no public access-mode inspection API.");
        method("file.seek","Seek",false,{{"offset","Offset",primitive("i64")},{"origin","Origin",primitive("u16")}});
        method("file.flush","Flush",false);method("file.close","Close",false);
        method("file.read_text","Read Text",false,{{"count","Count",primitive("i64"),Direction::In,false,"-1"},
            {"data","Data",primitive("string"),Direction::Out},{"eof","End of File",primitive("bool"),Direction::Out}});
        method("file.write_text","Write Text",false,{{"data","Data",primitive("string")}});
        method("file.write_binary","Write Binary",false,{{"data","Data",TypeRef::array(primitive("u8"))}});
    }
    if(classId=="frog.project" || classId=="frog.project_item" || classId=="frog.target") {
        unavailable("name","Name",primitive("string"),"Project capability unavailable.",true,false);
        unavailable("path","Path",primitive("path"),"Project capability unavailable.",true,false);
        if(classId=="frog.project") {
            unavailable("application","Application",TypeRef::reference("frog.application"),"Project capability unavailable.",true,false);
            unavailable("root","Root",TypeRef::reference("frog.project_item"),"Project capability unavailable.",true,false);
            unavailable("targets","Targets[]",TypeRef::array(TypeRef::reference("frog.target")),"Project capability unavailable.",true,false);
            unavailable("modified","Modified",primitive("bool"),"Project capability unavailable.",true,false);
            method("save","Save",false);method("close","Close",false);
        } else if(classId=="frog.project_item") {
            unavailable("parent","Parent",TypeRef::reference("frog.project_item"),"Project capability unavailable.",true,false);
            unavailable("children","Children[]",TypeRef::array(TypeRef::reference("frog.project_item")),"Project capability unavailable.",true,false);
            unavailable("project","Project",TypeRef::reference("frog.project"),"Project capability unavailable.",true,false);
            unavailable("target","Target",TypeRef::reference("frog.target"),"Project capability unavailable.",true,false);
            method("add_item","Add Item",false);method("remove","Remove From Project",false);
        }
        for(auto &member:result){member.authoringAvailable=false;member.runtimeAvailable=false;member.unavailableReason="Project capability unavailable.";}
    }
    // Defaults preserve every existing member ID. UI ordering/path metadata
    // lives here beside each actual typed operation, never in a second menu table.
    std::uint32_t extra=0;
    for(auto &member:result) {
        member.declaringClass=control?"frog.control":text?"frog.text":std::string(classId);
        member.menuSection=control?3U:2U;member.menuOrder=10000U+extra++;
        member.memberPath={member.id};
        if(member.kind==ApplicationControlMemberKind::Method){member.menuSection=control?3U:2U;member.menuOrder=20000U+extra++;}
    }
    const auto place=[&](std::string_view id,std::string label,std::vector<std::string> path,
        std::uint32_t section,std::uint32_t order,std::string declared="frog.control",std::vector<std::string> operation={}) {
        for(auto &member:result)if(member.id==id) {
            member.label=std::move(label);member.menuPath=std::move(path);member.menuSection=section;
            member.menuOrder=order;member.declaringClass=std::move(declared);
            if(!operation.empty())member.memberPath=std::move(operation);
            return;
        }
    };
    place("identity.class","Class ID",{},0,0,"frog.object");
    place("identity.class_name","Class Name",{},0,1,"frog.object");
    place("identity.owner","Owner",{},0,2,"frog.object");
    place("identity.owning_frog","Owning FROG",{},0,3,"frog.object");
    place("identity.object_id","Object ID",{},0,4,"frog.object");
    place("bounds","All Elements",{"Bounds"},1,100,"frog.object");
    place("bounds.area_height","Area Height",{"Bounds"},1,101,"frog.object");
    place("bounds.area_width","Area Width",{"Bounds"},1,102,"frog.object");
    place("position","All Elements",{"Position"},1,110,"frog.object");
    place("position.left","Left",{"Position"},1,111,"frog.object");
    place("position.top","Top",{"Position"},1,112,"frog.object");
    place("size","All Elements",{"Size"},2,860,"frog.object");
    place("size.width","Width",{"Size"},2,861,"frog.object");
    place("size.height","Height",{"Size"},2,862,"frog.object");
    std::uint32_t commonOrder=200;
    for(const auto id:{"interaction.blinking","interaction.mouse_wheel","caption.reference","control_index",
        "data_binding.configuration","datasocket.configuration","description","interaction.disabled_state",
        "focus_key_binding","indicator","key_focus","label.reference","identity.owning_pane","skip_when_tabbing",
        "synchronous_display","tip_strip","value","value_signaling","interaction.visible","xcontrol.configuration"}) {
        for(auto &member:result)if(member.id==id){member.menuSection=2;member.menuOrder=commonOrder;}
        commonOrder+=30;
    }
    place("data_binding.configuration","Configuration",{"Data Binding"},2,320);
    place("datasocket.configuration","Configuration",{"DataSocket"},2,350);
    place("xcontrol.configuration","Configuration",{"XControl"},2,770);
    place("caption.reference","Reference",{"Caption"},2,260);
    place("label.reference","Reference",{"Label"},2,530);
    place("boolean_text.reference","Reference",{"Boolean Text"},3,1000,"frog.control.boolean");
    // Existing flat properties are explicit paths to their text object.
    for(const auto &[part,caption,base]:std::initializer_list<std::tuple<const char*,const char*,std::uint32_t>>{
        {"label","Label",530},{"caption","Caption",260},{"boolean_text","Boolean Text",1000}}) {
        const auto prefix=std::string(part)+".";
        place(prefix+"text","Text",{caption},3,base+10,"frog.text",{prefix+"reference","text"});
        place(prefix+"visible","Visible",{caption},2,base+5,"frog.text",{prefix+"reference","interaction.visible"});
        place(prefix+"font.name","Name",{caption,"Font"},3,base+12,"frog.text",{prefix+"reference","font.name"});
        place(prefix+"font.size","Size",{caption,"Font"},3,base+13,"frog.text",{prefix+"reference","font.size"});
        place(prefix+"font.bold","Bold",{caption,"Font"},3,base+14,"frog.text",{prefix+"reference","font.bold"});
        place(prefix+"font.italic","Italic",{caption,"Font"},3,base+15,"frog.text",{prefix+"reference","font.italic"});
        place(prefix+"use_default_font","Use Default Font",{caption,"Font"},3,base+16,"frog.text",{prefix+"reference","use_default_font"});
        place(prefix+"text_color","Text",{caption,"Colors"},2,base+6,"frog.text",{prefix+"reference","text_color"});
        place(prefix+"background_color","Background",{caption,"Colors"},2,base+7,"frog.text",{prefix+"reference","background_color"});
        place(prefix+"transparent","Transparent",{caption,"Colors"},2,base+8,"frog.text",{prefix+"reference","transparent"});
        place(prefix+"alignment","Justification",{caption},3,base+17,"frog.text",{prefix+"reference","alignment"});
    }
    place("font.name","Name",{"Font"},3,400,"frog.text");
    place("font.all","All Elements",{"Font"},3,399,"frog.text");
    place("font.size","Size",{"Font"},3,401,"frog.text");
    place("font.bold","Bold",{"Font"},3,402,"frog.text");
    place("font.italic","Italic",{"Font"},3,403,"frog.text");
    place("font.underline","Underline",{"Font"},3,404,"frog.text");
    place("use_default_font","Use Default Font",{"Font"},3,405,"frog.text");
    place("text_color","Text",{"Colors"},2,200,"frog.text");
    place("background_color","Background",{"Colors"},2,201,"frog.text");
    place("transparent","Transparent",{"Colors"},2,202,"frog.text");
    if(text)for(auto &member:result)if(member.id=="use_default_font")member.migrationAliases={"font.use_default"};
    place("numeric.minimum","Minimum",{"Data Entry Limits"},3,1100,"frog.control.numeric");
    place("numeric.maximum","Maximum",{"Data Entry Limits"},3,1101,"frog.control.numeric");
    place("numeric.increment","Increment",{"Data Entry Limits"},3,1102,"frog.control.numeric");
    place("numeric.format","Format",{"Display"},3,1120,"frog.control.numeric");
    place("numeric.precision","Precision",{"Display"},3,1121,"frog.control.numeric");
    place("numeric.radix","Radix",{"Display"},3,1122,"frog.control.numeric");
    std::uint32_t stringOrder=1150;
    for(const auto id:{"string.display_style","string.word_wrap","string.multiline","string.scrollbar",
        "string.horizontal_scrollbar","string.vertical_scrollbar","string.update_while_typing",
        "string.allow_dragging","string.allow_dropping","string.selection_start","string.selection_end","string.selected_text"}) {
        for(auto &member:result)if(member.id==id){member.menuSection=3;member.menuOrder=stringOrder;member.declaringClass="frog.control.string";}
        ++stringOrder;
    }
    place("items.strings","Strings[]",{},4,1200,"frog.control.named_numeric");
    place("items.strings_and_values","Strings And Values[]",{},4,1201,"frog.control.ring");
    place("dropdown.strings_and_values","Strings And Values[]",{},4,1201,"frog.control.dropdown_string");
    place("array_element","Reference",{"Array Element"},3,1300,"frog.control.array");
    place("array.index_values","Index Values[]",{},3,1310,"frog.control.array");
    place("array.selection_start","Selection Start[]",{},3,1311,"frog.control.array");
    place("array.selection_size","Selection Size[]",{},3,1312,"frog.control.array");
    place("object_highlight","Object Highlight",{},0,0,"frog.object");
    std::uint32_t methodsOrder=100;
    for(const auto id:{"datasocket.attach","data_binding.bind","data_binding.unbind","fit_control_to_pane","get_image",
        "get_terminal_image","reinitialize","datasocket.remove","start_drag","make_default","set_focus"}) {
        for(auto &member:result)if(member.id==id){member.menuSection=1;member.menuOrder=methodsOrder;}
        methodsOrder+=10;
    }
    place("data_binding.bind","Bind",{"Data Binding"},1,110);
    place("data_binding.unbind","Unbind",{"Data Binding"},1,111);
    place("string.append_string","Append String",{},2,300,"frog.control.string");
    place("string.byte_offset_from_point","Byte Offset from Point",{},2,301,"frog.control.string");
    place("string.get_nth_line","Get Nth Line",{},2,302,"frog.control.string");
    place("string.size_to_text","Size to Text",{},2,303,"frog.control.string");
    place("string.select_all","Select All",{},3,320,"frog.control.string");
    place("string.select_text","Select Text",{},3,321,"frog.control.string");
    place("front_panel","Reference",{"Front Panel"},2,200,"frog.program");
    place("front_panel.open_state","Open State",{"Front Panel"},2,201,"frog.program");
    place("front_panel.open","Open",{"Front Panel"},2,210,"frog.program");
    place("front_panel.close","Close",{"Front Panel"},2,211,"frog.program");
    place("path.browse_mode","Mode",{"Browse Options"},3,1300,"frog.control.path");
    place("path.match_pattern","Match Pattern",{"Browse Options"},3,1301,"frog.control.path");
    place("project.new","New",{"Project"},3,1500,"frog.application");
    place("project.open","Open",{"Project"},3,1501,"frog.application");
    if(classId=="frog.control.color_box") {
        std::erase_if(result,[](const auto &m){return m.id.starts_with("numeric.") || m.id.starts_with("text.");});
    }
    if(classId=="frog.control.timestamp") {
        std::erase_if(result,[](const auto &m){return m.id.starts_with("numeric.");});
    }
    // Subobject groups include the actual object catalogue, not aliases of Value.
    if(control) {
        for(const auto &[id,label,cls,base]:std::initializer_list<std::tuple<const char*,const char*,const char*,std::uint32_t>>{
            {"label","Label","frog.text.label",530},{"caption","Caption","frog.text.caption",260},
            {"boolean_text","Boolean Text","frog.text.boolean",1000}}) {
            const auto prefix=std::string(id)+".";
            if(std::none_of(result.begin(),result.end(),[&](const auto &m){return m.id==prefix+"reference";}))continue;
            for(auto child:referenceCatalogue(cls,{},nullptr)) {
                const auto childId=child.id;
                child.id=prefix+(childId=="interaction.visible"?"visible":childId);
                child.menuPath.insert(child.menuPath.begin(),label);
                child.menuOrder=base+1+child.menuOrder/100;
                child.memberPath.insert(child.memberPath.begin(),prefix+"reference");
                const auto existing=std::find_if(result.begin(),result.end(),[&](const auto &m){return m.id==child.id;});
                if(existing!=result.end()) {
                    // Existing flat IDs keep their executor and permissions,
                    // but use the same ordered subobject definition in menus.
                    existing->menuPath=std::move(child.menuPath);existing->label=std::move(child.label);
                    existing->menuSection=child.menuSection;existing->menuOrder=child.menuOrder;
                    existing->declaringClass=std::move(child.declaringClass);existing->memberPath=std::move(child.memberPath);
                } else result.push_back(std::move(child));
            }
        }
    }
    std::stable_sort(result.begin(),result.end(),[](const auto &a,const auto &b){return a.menuOrder<b.menuOrder;});
    return result;
}
}
std::string applicationControlMemberLabel(const ApplicationControlMemberDescriptor &member) {
    return memberDisplayName(member);
}
std::vector<ApplicationControlMemberDescriptor> applicationControlMembers(std::string_view classId,
    const TypeRef &valueType,const WidgetInstance *presentation) {
    auto result=referenceCatalogue(classId,valueType,presentation);
    // Only the known Array presentation chain is expanded. Owner/Pane/FROG
    // references stay explicit Reference leaves and cannot create menu cycles.
    if(std::none_of(result.begin(),result.end(),[](const auto &m){return m.id=="array_element";}))return result;
    const auto *current=presentation;
    auto schema=valueType;
    std::vector<std::string> prefix,operation;
    std::set<const WidgetInstance *> visited;
    while(schema.kind==binding::TypeKind::Array && schema.children.size()==1) {
        if(current && !visited.insert(current).second)break;
        const auto *array=current?arrayContainerState(*current):nullptr;
        current=array?array->elementTemplate.get():nullptr;
        auto elementSchema=schema.children.front();schema=std::move(elementSchema);
        prefix.push_back("Array Element");operation.push_back("array_element");
        std::string childClass="frog.control";
        if(current) {
            const auto actual=binding::widgetType(*current);if(actual.valid())schema=actual.type;
            childClass=applicationControlReferenceClass(*current);
        } else if(schema.kind==binding::TypeKind::Array)childClass="frog.widgets.array_control";
        else if(schema.kind==binding::TypeKind::Cluster)childClass="frog.widgets.cluster_control";
        else if(schema.kind==binding::TypeKind::Enum)childClass="frog.widgets.enum_control";
        else if(schema.kind==binding::TypeKind::Primitive) {
            if(schema.name=="string")childClass=schema.choiceKind==binding::ChoiceKind::Dropdown?"frog.control.dropdown_string":"frog.widgets.string_control";
            else if(schema.name=="path")childClass="frog.widgets.path_control";
            else if(schema.name=="bool")childClass="frog.widgets.boolean_control";
            else if(schema.name=="timestamp")childClass="frog.control.timestamp";
            else if(schema.numeric())childClass="frog.widgets.numeric_control";
        }
        auto children=referenceCatalogue(childClass,schema,current);
        for(auto &child:children) {
            if(child.kind==ApplicationControlMemberKind::Method)continue;
            std::string id;
            for(const auto &step:operation)id+=step+".";
            child.id=id+child.id;
            child.menuPath.insert(child.menuPath.begin(),prefix.begin(),prefix.end());
            child.memberPath.insert(child.memberPath.begin(),operation.begin(),operation.end());
            child.menuOrder=1301U+static_cast<std::uint32_t>(result.size());
            // A presentation template is not an indexed data cell.
            if(child.id.ends_with(".value") || child.id.ends_with(".default_value") || child.id.ends_with(".value_signaling")) {
                child.authoringAvailable=false;child.runtimeAvailable=false;
                child.unavailableReason="Array Element is the shared presentation template; access data through Array Value and indexing.";
            }
            result.push_back(std::move(child));
        }
        if(schema.kind!=binding::TypeKind::Array)break;
    }
    return result;
}
const WidgetInstance *applicationControlReferencePresentation(const DiagramNode &node,
    std::span<const DiagramNode> nodes,std::span<const DiagramEdge> edges,FrogWidgetDataView canvas) {
    const auto *current=&node;
    std::string port;
    std::vector<std::string> fields,members;
    std::set<std::string> visited;
    const auto byId=[&](std::string_view id)->const DiagramNode* {
        const auto found=std::find_if(nodes.begin(),nodes.end(),[&](const auto &n){return n.id==id;});
        return found==nodes.end()?nullptr:&*found;
    };
    const auto incoming=[&](const DiagramNode &owner,std::string_view input)->const DiagramEdge* {
        const DiagramEdge *found=nullptr;
        for(const auto &edge:edges)if(edge.to.nodeId==owner.id && edge.to.portId==input) {
            if(found)return nullptr;found=&edge;
        }
        return found;
    };
    const auto follow=[&](const DiagramEdge *edge)->bool {
        if(!edge)return false;current=byId(edge->from.nodeId);port=edge->from.portId;return current!=nullptr;
    };
    const auto finish=[&](const ApplicationControlNode &reference)->const WidgetInstance* {
        if(reference.kind!=ApplicationControlKind::Reference || reference.target.kind!=ApplicationControlTargetKind::Widget ||
            !reference.target.ownerDocumentId.empty() || !fields.empty())return nullptr;
        const auto *widget=canvas.widgetById(reference.target.widgetId);
        for(auto selected=members.rbegin();widget && selected!=members.rend();++selected) {
            const auto schema=binding::widgetType(*widget);
            const auto member=applicationControlMember(applicationControlReferenceClass(*widget),schema.valid()?schema.type:TypeRef{},*selected,widget);
            if(!member || member->type.kind!=binding::TypeKind::Reference)return nullptr;
            for(const auto &step:member->memberPath) {
                if(step!="array_element")return nullptr;
                const auto *array=arrayContainerState(*widget);widget=array?array->elementTemplate.get():nullptr;
                if(!widget)return nullptr;
            }
        }
        return widget;
    };
    // Derive ordinals from the current full source schema, never valueType
    // display text. Construction is lazy for ordinary direct references.
    std::optional<binding::TypeResolver> schemas;
    const auto schema=[&](const DiagramNode &owner,std::string_view output) {
        if(!schemas)schemas.emplace(nodes,edges,canvas);
        return schemas->resolve({owner.id,std::string(output)}).resolution;
    };
    while(current && visited.insert(current->id).second) {
        if(isClusterUnbundleFunction(current->type)) {
            const auto row=clusterFunctionRow(port);
            if(!row)return nullptr;
            const auto record=schema(*current,"cluster");
            if(!record.accepted())return nullptr;
            const auto selected=clusterRecordFieldIdPath(*current,record.type,*row);
            if(selected.empty())return nullptr;
            fields.insert(fields.begin(),selected.begin(),selected.end());
            if(!follow(incoming(*current,"cluster")))return nullptr;
            continue;
        }
        const auto clusterId=clusterFunctionId(current->type);
        if(clusterId=="cluster.properties") {
            if(port!="value" || current->text.empty())return nullptr;
            fields.insert(fields.begin(),current->text);
            if(!follow(incoming(*current,"cluster")))return nullptr;
            continue; // Existing legacy direct-ID selection, never Value.
        }
        if(clusterId=="cluster.bundle" || clusterId=="cluster.build") {
            if(port!=(clusterId=="cluster.build"?"cluster":"result") || fields.empty())return nullptr;
            const auto *rail=clusterId=="cluster.bundle"?incoming(*current,"cluster"):nullptr;
            const auto record=schema(*current,rail?"cluster":port);
            if(!record.accepted() || record.type.kind!=binding::TypeKind::Cluster)return nullptr;
            const auto selected=std::find(record.type.fieldIds.begin(),record.type.fieldIds.end(),fields.front());
            if(selected==record.type.fieldIds.end())return nullptr;
            const auto row=static_cast<unsigned>(selected-record.type.fieldIds.begin());
            const auto *replacement=(clusterId=="cluster.build" || row<clusterFunctionCount(*current))
                ? incoming(*current,clusterFunctionRowId(row)) : nullptr;
            if(replacement) {
                fields.erase(fields.begin());if(!follow(replacement))return nullptr;
            } else if(!rail || !follow(rail))return nullptr;
            continue;
        }
        if(isClusterBundleByName(current->type)) {
            if(port!="result" || fields.empty())return nullptr;
            const DiagramEdge *replacement=nullptr;std::size_t prefix=0;
            for(unsigned row=0;row<clusterFunctionCount(*current);++row) {
                if(row>=current->clusterFieldIdPaths.size())continue;
                const auto &selection=current->clusterFieldIdPaths[row];
                if(selection.empty() || selection.size()>fields.size() ||
                    !std::equal(selection.begin(),selection.end(),fields.begin()))continue;
                // Ambiguous overlapping replacements and unwired replacements
                // have no statically valid object witness.
                if(replacement)return nullptr;
                replacement=incoming(*current,clusterFunctionRowId(row));
                if(!replacement)return nullptr;
                prefix=selection.size();
            }
            if(replacement) {
                fields.erase(fields.begin(),fields.begin()+prefix);
                if(!follow(replacement))return nullptr;
            } else if(!follow(incoming(*current,"cluster")))return nullptr;
            continue;
        }
        if(isDiagramClusterConstant(*current)) {
            if(fields.empty())return nullptr;
            const DiagramNode *field=nullptr;
            for(const auto &candidate:nodes)if(candidate.constantOwnerNodeId==current->id && candidate.constantFieldId==fields.front()) {
                if(field)return nullptr;field=&candidate;
            }
            if(!field || !isDiagramLiteralConstant(*field))return nullptr;
            fields.erase(fields.begin());current=field;port=std::string(diagramLiteralValuePortId(*field));continue;
        }
        if(current->kind=="widget_value") {
            const auto *widget=canvas.widgetById(current->widgetId);
            for(const auto &fieldId:fields) {
                const auto *cluster=widget?clusterContainerState(*widget):nullptr;
                if(!cluster)return nullptr;
                const WidgetInstance *field=nullptr;
                for(const auto &candidate:cluster->elements)if(candidate.fieldId==fieldId) {
                    if(field)return nullptr;field=candidate.widget.get();
                }
                widget=field;
            }
            fields.clear();
            return widget && isReferenceWidget(*widget) && widget->referenceValue?finish(*widget->referenceValue):nullptr;
        }
        if(!current->applicationControl || !fields.empty())return nullptr;
        const auto &config=*current->applicationControl;
        if(!port.empty() && port!="reference_out" && config.kind==ApplicationControlKind::Property) {
            const auto row=std::find_if(config.properties.begin(),config.properties.end(),
                [&](const auto &r){return applicationControlPropertyPortId(r.id)==port;});
            if(row==config.properties.end() || row->mode!=ApplicationControlPropertyMode::Read)return nullptr;
            // Resolve the registered operation path after reaching its actual
            // source object, so a previous cached class cannot erase evidence.
            members.push_back(row->memberId);
        }
        if(config.kind==ApplicationControlKind::Reference)return finish(config);
        const auto *source=incoming(*current,"reference_in");
        if(source) {if(!follow(source))return nullptr;continue;}
        // An implicit local link is a symbolic Reference, without reading Value.
        if(std::any_of(edges.begin(),edges.end(),[&](const auto &e){return e.to.nodeId==current->id && e.to.portId=="reference_in";}))return nullptr;
        auto implicit=config;implicit.kind=ApplicationControlKind::Reference;return finish(implicit);
    }
    return nullptr;
}

std::optional<ApplicationControlMemberDescriptor> applicationControlMember(std::string_view classId,
    const TypeRef &valueType,std::string_view memberId,const WidgetInstance *presentation) {
    for(auto &member:applicationControlMembers(classId,valueType,presentation))
        if(member.id==memberId || std::find(member.migrationAliases.begin(),member.migrationAliases.end(),memberId)!=member.migrationAliases.end())
            return std::move(member);
    return {};
}
std::string applicationControlClassLabel(std::string_view classId,const TypeRef &value) {
    if(classId==kOccurrenceReferenceClass)return "Occurrence";
    if(classId==kRendezvousReferenceClass)return "Rendezvous";
    if(classId==kReferenceWidgetClass)return "Reference";
    if(classId==kTabControlWidgetClass || classId==kTabIndicatorWidgetClass)return "Tab";
    if(classId=="frog.control.color_box")return "Color Box";
    if(classId=="frog.control.dropdown_string")return "Dropdown String";
    if(binding::isErrorCluster(value))return "Error";
    if(value.kind==binding::TypeKind::Primitive && value.name=="timestamp")return "Timestamp";
    if(value.kind==binding::TypeKind::Primitive && value.name=="variant")return "Variant";
    if(const auto *definition=findWidgetDefinitionByClassId(referenceWidgetBase(classId))) {
        if(widgetDefinitionHasCapability(*definition,WidgetCapability::ArrayContainer))return "Array";
        if(widgetDefinitionHasCapability(*definition,WidgetCapability::ClusterContainer))return "Cluster";
        switch(definition->family) {
        case WidgetFamily::Button:case WidgetFamily::Boolean:return "Boolean";
        case WidgetFamily::Numeric:return classId.find("color_box")!=std::string_view::npos?"Color Box":"Numeric";
        case WidgetFamily::StringPath:return value.name=="path" || classId.find("path")!=std::string_view::npos?"Path":"String";
        case WidgetFamily::Enum:return "Enum";
        case WidgetFamily::Ring:return "Ring";
        default:break;
        }
        if(const auto *entry=findWidgetCatalogEntryByClassId(classId)) {
            std::string label(entry->displayName);
            for(const auto suffix:{std::string_view{" Control"},std::string_view{" Indicator"}})
                if(label.ends_with(suffix))label.resize(label.size()-suffix.size());
            return label;
        }
    }
    if(classId=="frog.control.color_box")return "Color Box";
    if(classId=="frog.control.dropdown_string")return "Dropdown String";
    if(classId=="frog.control.timestamp")return "Timestamp";
    if(classId=="frog.control.error_cluster")return "Error";
    if(classId=="frog.text.label")return "Label";
    if(classId=="frog.text.caption")return "Caption";
    if(classId=="frog.text.boolean")return "Boolean Text";
    if(classId=="frog.pane")return "Pane";
    if(classId=="file")return "File";
    if(classId=="frog.project")return "Project";
    if(classId=="frog.project_item")return "Project Item";
    if(classId=="frog.target")return "Target";
    if(classId=="frog.application")return "App";
    if(classId=="frog.program")return "FROG";
    if(classId=="frog.front_panel")return "Front Panel";
    if(classId=="frog.control")return "Control";
    return "Object";
}
bool applicationControlHasReferenceInput(const ApplicationControlNode &node) noexcept {
    return node.kind!=ApplicationControlKind::Reference &&
        (node.kind!=ApplicationControlKind::Property || node.target.kind==ApplicationControlTargetKind::Unbound);
}
std::vector<ApplicationControlPort> applicationControlPorts(const ApplicationControlNode &node,const TypeRef &wired,const WidgetInstance *presentation) {
    const auto ref=effectiveReference(node,wired);
    if(ref.kind!=binding::TypeKind::Reference || !ref.known())return {};
    const auto value=ref.children.empty()?TypeRef{}:ref.children.front();
    using D=ApplicationControlPortDirection;
    std::vector<ApplicationControlPort> result;
    const auto add=[&](std::string id,std::string label,D direction,TypeRef type,
        std::optional<std::size_t> row={},bool required=false,std::optional<std::string> fallback={}) {
        result.push_back({std::move(id),std::move(label),row,direction,std::move(type),required,std::move(fallback)});
    };
    if(node.kind==ApplicationControlKind::Reference) {add("reference_out","Reference",D::Output,ref);return result;}
    if(applicationControlHasReferenceInput(node))
        add("reference_in","Reference",D::Input,ref,{},node.target.kind==ApplicationControlTargetKind::Unbound);
    if(node.kind!=ApplicationControlKind::Close)add("reference_out","Reference",D::Output,ref);
    const auto error=TypeRef::cluster({"status","code","source"},{primitive("bool"),primitive("i32"),primitive("string")});
    add("error_in","Error",D::Input,error);add("error_out","Error",D::Output,error);
    if(node.kind==ApplicationControlKind::Property) {
        for(std::size_t i=0;i<node.properties.size();++i) {
            const auto &row=node.properties[i];const auto member=applicationControlMember(ref.name,value,row.memberId,presentation);
            if(!member || member->kind!=ApplicationControlMemberKind::Property) {
                // A previous selection keeps its stable wire anchor when a
                // new class has no such property. Unknown type stays broken.
                if(!row.memberId.empty())add(applicationControlPropertyPortId(row.id),"Unavailable property",
                    row.mode==ApplicationControlPropertyMode::Read?D::Output:D::Input,{},i);
                continue;
            }
            add(applicationControlPropertyPortId(row.id),memberDisplayName(*member),
                row.mode==ApplicationControlPropertyMode::Read?D::Output:D::Input,member->type,i,
                row.mode==ApplicationControlPropertyMode::Write);
        }
    } else if(node.kind==ApplicationControlKind::Invoke) {
        const auto method=applicationControlMember(ref.name,value,node.methodId,presentation);
        if(method)for(auto &port:applicationControlMethodPorts(*method,node.parameters))result.push_back(std::move(port));
    }
    return result;
}
std::vector<ApplicationControlDiagnostic> validateApplicationControlNode(const ApplicationControlNode &node,const TypeRef &wired,const WidgetInstance *presentation) {
    std::vector<ApplicationControlDiagnostic> result;
    const auto error=[&](std::string code,std::string row,std::string message){result.push_back({std::move(code),std::move(row),std::move(message)});};
    if(node.kind==ApplicationControlKind::Property && !applicationControlHasReferenceInput(node) &&
        wired.kind!=binding::TypeKind::Unknown)
        error("application_control.target_conflict",{},"The linked Property Node also has a reference input; disconnect the wire or explicitly disconnect its target.");
    const auto ref=effectiveReference(node,wired);
    if(!ref.known() || ref.kind!=binding::TypeKind::Reference) {
        error("application_control.reference_type",{},"A complete reference type is required.");return result;
    }
    if(isNullQueueReferenceValue(node) && isQueueReference(ref) && ref.children.size()==1 &&
        ref.children.front()==node.valueType) return result;
    if(isNullNotifierReferenceValue(node) && isNotifierReference(ref) && ref.children.size()==1 &&
        binding::fullTypeEqual(ref.children.front(),node.valueType))return result;
    if(isNullSemaphoreReferenceValue(node) && isSemaphoreReference(ref))return result;
    if(isNullRendezvousReferenceValue(node) && isRendezvousReference(ref))return result;
    if(isNullOccurrenceReferenceValue(node) && isOccurrenceReference(ref))return result;
    if(isNullEventReferenceValue(node) && (isUserEventReference(ref) || isEventRegistrationReference(ref)) &&
        ref.children.size()==1 && ref.children.front()==node.valueType)return result;
    const auto classes=applicationControlReferenceClasses();
    if(std::find(classes.begin(),classes.end(),ref.name)==classes.end() && !findWidgetDefinitionByClassId(ref.name))
        error("application_control.class",{},"Unknown reference class.");
    if(node.target.kind==ApplicationControlTargetKind::Widget && node.target.widgetId.empty() &&
        (node.kind==ApplicationControlKind::Reference || wired.kind==binding::TypeKind::Unknown))
        error("application_control.unbound",{},"The widget reference has no target identity.");
    // A complete typed-null constant is transportable; Property/Invoke still
    // require an actual runtime target before accessing its value or effects.
    if(node.kind==ApplicationControlKind::Reference && node.target.kind==ApplicationControlTargetKind::Unbound &&
        (ref.children.size()!=1 || !node.valueType.known() || ref.children.front()!=node.valueType || node.expectedClass!=ref.name))
        error("application_control.unbound",{},"The typed reference is not linked to a target.");
    const auto value=ref.children.empty()?TypeRef{}:ref.children.front();
    std::set<std::string> ids;
    if(node.kind==ApplicationControlKind::Property) {
        if(node.properties.empty())error("application_control.unconfigured",{},"Select a property.");
        for(const auto &row:node.properties) {
            if(!stableId(row.id) || !ids.insert(row.id).second)error("application_control.row_id",row.id,"Property occurrence IDs must be unique and stable.");
            const auto member=applicationControlMember(ref.name,value,row.memberId,presentation);
            if(!member || member->kind!=ApplicationControlMemberKind::Property)error("application_control.property",row.id,"Select a property of the effective reference class.");
            else if(row.mode==ApplicationControlPropertyMode::Read?!member->readable:!member->writable)
                error("application_control.permission",row.id,"This property does not allow the selected mode.");
            else if(!member->authoringAvailable && !member->runtimeAvailable)
                error("application_control.capability",row.id,member->unavailableReason.empty()?"This property requires an unavailable provider capability.":member->unavailableReason);
        }
    } else if(node.kind==ApplicationControlKind::Invoke) {
        const auto method=applicationControlMember(ref.name,value,node.methodId,presentation);
        if(!method || method->kind!=ApplicationControlMemberKind::Method)error("application_control.method",{},"Select one method of the effective reference class.");
        else if(!method->authoringAvailable && !method->runtimeAvailable)
            error("application_control.capability",{},method->unavailableReason.empty()?"This method requires an unavailable provider capability.":method->unavailableReason);
        if(method && method->kind==ApplicationControlMemberKind::Method)for(const auto &binding:node.parameters) {
            const auto param=std::find_if(method->parameters.begin(),method->parameters.end(),[&](const auto &p){return p.id==binding.id;});
            if(!stableId(binding.id) || !ids.insert(binding.id).second || param==method->parameters.end())error("application_control.parameter",binding.id,"Unknown or duplicate parameter identity.");
            else if(param->required && !binding.enabled)error("application_control.required",binding.id,"A required parameter cannot be hidden.");
        }
    }
    return result;
}
} // namespace frog::native_engine
