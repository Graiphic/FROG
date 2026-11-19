// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/ListboxWidget.hpp"
#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include <charconv>
#include <cmath>
#include <stdexcept>
#include <unordered_map>

namespace frog::native_engine::listbox_widget {
namespace {
using Json=source::JsonValue;
namespace cj=canonical_json;
constexpr std::string_view propertyKeys[]={"items","selection.mode","binding.value_path","selection.selected_ids",
    "active.id","selection.anchor_id","activation.double_click_enabled","activation.enter_key_enabled"};
constexpr std::string_view stateKeys[]={"value","value.selected_id","value.selected_ids","value.selected_value","value.selected_values","value.selected_indexes"};
const Json *field(const Json &node,std::string_view name){return node.type==Json::Type::Object?node.member(name):nullptr;}
std::string text(const Json &node,std::string_view name,std::string fallback={}) {
    const auto *p=field(node,name);if(!p)return fallback;
    if(p->type!=Json::Type::String)throw std::runtime_error(std::string(name)+": expected text");return p->string;
}
bool flag(const Json &node,std::string_view name,bool fallback) {
    const auto *p=field(node,name);if(!p)return fallback;
    if(p->type!=Json::Type::Boolean)throw std::runtime_error(std::string(name)+": expected Boolean");return p->boolean;
}
std::optional<std::uint64_t> unsignedValue(const Json &v) {
    if(v.type!=Json::Type::Number)return {};
    const auto lexical=numeric_value::canonicalWideIntegerLexeme(v.string,"u64");if(!lexical)return {};
    std::uint64_t result=0;const auto parsed=std::from_chars(lexical->data(),lexical->data()+lexical->size(),result);
    if(parsed.ec!=std::errc{}||parsed.ptr!=lexical->data()+lexical->size())return {};return result;
}
std::optional<std::uint16_t> itemValue(const Json &v) {
    const auto value=unsignedValue(v);if(!value||*value>65535)return {};return std::uint16_t(*value);
}
const Json &sequence(const Json &value) {
    if(value.type==Json::Type::Array)return value;
    const auto *elements=field(value,"elements"),*shape=field(value,"shape");
    if(!elements||elements->type!=Json::Type::Array||!shape||shape->type!=Json::Type::Array||shape->array.size()!=1||
        unsignedValue(shape->array.front())!=elements->array.size())throw std::runtime_error("selection: expected a rank-one Array with its exact shape and elements");
    return *elements;
}
std::vector<std::string> ids(const Json &v) {
    const auto &array=sequence(v);if(array.array.size()>65536)throw std::runtime_error("selection.selected_ids: expected a bounded array");
    std::vector<std::string> result;result.reserve(array.array.size());
    for(const auto &id:array.array){if(id.type!=Json::Type::String)throw std::runtime_error("selection.selected_ids: expected item identities");result.push_back(id.string);}return result;
}
Json strings(const std::vector<std::string> &v){auto a=cj::array();for(const auto &s:v)a.array.push_back(cj::string(s));return a;}
Json number(std::uint32_t n){Json j;j.type=Json::Type::Number;j.number=n;j.string=std::to_string(n);return j;}
Json typedArray(Json elements) {
    auto shape=cj::array();shape.array.push_back(number(std::uint32_t(elements.array.size())));
    auto result=cj::object();result.object={{"shape",std::move(shape)},{"elements",std::move(elements)}};return result;
}
bool sameValues(const Json &left,const Json &right) {
    const auto &a=sequence(left),&b=sequence(right);if(a.array.size()!=b.array.size())return false;
    for(std::size_t i=0;i<a.array.size();++i)if(!unsignedValue(a.array[i])||unsignedValue(a.array[i])!=unsignedValue(b.array[i]))return false;
    return true;
}
Json selectedValues(const WidgetInstance &widget,const State &s,bool indexes) {
    std::unordered_map<std::string_view,std::uint32_t> values;values.reserve(widget.ringItems.size());
    for(std::size_t i=0;i<widget.ringItems.size();++i)values.emplace(widget.ringItems[i].id,indexes?std::uint32_t(i):widget.ringItems[i].value);
    auto result=cj::array();for(const auto &id:s.selection.selected) {
        const auto found=values.find(id);if(found==values.end())throw std::runtime_error("value.selected_ids: item has no declared identity");
        result.array.push_back(number(found->second));
    }return result;
}
void readSelectionValue(const WidgetInstance &widget,State &s,const Json &value) {
    if(s.projection==Projection::SelectedIds){s.selection.selected=ids(value);return;}
    if(s.projection==Projection::Selection) {
        if(value.type!=Json::Type::Object)throw std::runtime_error("value: expected a full Listbox selection object");
        const auto *selected=value.member("selected_ids");if(!selected)throw std::runtime_error("value.selected_ids: required");
        s.selection.selected=ids(*selected);
        if(const auto *values=value.member("selected_values");values&&!sameValues(*values,selectedValues(widget,s,false)))throw std::runtime_error("value.selected_values: contradicts selected identities");
        if(const auto *indexes=value.member("selected_indexes");indexes&&!sameValues(*indexes,selectedValues(widget,s,true)))throw std::runtime_error("value.selected_indexes: contradicts selected identities");
        return;
    }
    std::vector<const Json *> values;
    if(s.projection==Projection::SelectedValue)values.push_back(&value);
    else {
        const auto &array=sequence(value);if(array.array.size()>65536)throw std::runtime_error("value.selected_values: expected a bounded array");
        for(const auto &v:array.array)values.push_back(&v);
    }
    std::unordered_map<std::uint32_t,const RingItem *> byValue;byValue.reserve(widget.ringItems.size());
    for(const auto &item:widget.ringItems)byValue.emplace(item.value,&item);
    s.selection.selected.clear();
    for(const auto *v:values) {
        const auto n=itemValue(*v);if(!n)throw std::runtime_error("value.selected_values: item value is outside U16");
        if(s.projection==Projection::SelectedValue&&widget.ringItems.empty()&&*n==0)continue;
        const auto found=byValue.find(*n);
        if(found==byValue.end())throw std::runtime_error("value.selected_values: item value has no declared identity");
        s.selection.selected.push_back(found->second->id);
    }
}
Json retained(const State &s,std::string_view key) {
    const auto root=s.preservedProperties?source::parseJson(*s.preservedProperties,nullptr,source::JsonNumberPolicy::PreserveLexeme):std::nullopt;
    const auto *part=root?root->member(key):nullptr;return part?*part:cj::object();
}
}
std::vector<item_selection::Item> items(const WidgetInstance &widget) {
    std::vector<item_selection::Item> result;result.reserve(widget.ringItems.size());
    for(const auto &item:widget.ringItems)result.push_back({item.id,!item.disabled,item.visible});return result;
}
State state(const WidgetInstance &widget) {
    if(widget.listboxState) {
        auto result=*widget.listboxState;
        if(result.projection!=Projection::SelectedValue||!result.diagnostic.empty())return result;
        // The historical scalar setter still owns its exact U16 projection.
        result.selection.selected.clear();
        for(const auto &item:widget.ringItems)if(widget.numericValue==double(item.value)){result.selection.selected={item.id};break;}
        return result;
    }
    State result;
    for(const auto &item:widget.ringItems)if(widget.numericValue==double(item.value)){
        result.selection.selected={item.id};result.selection.active=item.id;result.selection.anchor=item.id;break;}
    return result;
}
std::string validate(const WidgetInstance &widget,const State &s) {
    if(!s.diagnostic.empty())return s.diagnostic;
    if(!isListboxWidget(widget))return "classId: not a Listbox";
    if(const auto why=item_selection::validate(s.selection,items(widget));!why.empty())return why;
    const bool legacyEmpty=widget.ringItems.empty()&&s.selection.selected.empty()&&widget.numericValue==0.0;
    if(s.projection==Projection::SelectedValue&&(s.selection.mode!=item_selection::Mode::Single||(!legacyEmpty&&s.selection.selected.size()!=1)))
        return "binding.value_path: value.selected_value requires one selected item in single mode; use selected_values, selected_ids or value for other modes";
    std::unordered_set<std::uint32_t> values;
    for(const auto &item:widget.ringItems)if(item.value>65535||!values.insert(item.value).second)return "items.value: this projection profile requires unique U16 item values";
    return {};
}
binding::TypeRef bindingType(Projection projection) {
    using T=binding::TypeRef;
    if(projection==Projection::SelectedValue)return T::primitive("u16");
    if(projection==Projection::SelectedValues)return T::array(T::primitive("u16"));
    if(projection==Projection::SelectedIds)return T::array(T::primitive("string"));
    return T::cluster({"selected_ids","selected_indexes","selected_values"},
        {T::array(T::primitive("string")),T::array(T::primitive("u32")),T::array(T::primitive("u16"))});
}
std::optional<Json> bindingValue(const WidgetInstance &widget,std::string *error) {
    if(error)error->clear();const auto s=state(widget);const auto why=validate(widget,s);
    if(!why.empty()){if(error)*error=why;return {};}
    if(s.projection==Projection::SelectedIds)return typedArray(strings(s.selection.selected));
    auto values=selectedValues(widget,s,false);
    if(s.projection==Projection::SelectedValues)return typedArray(std::move(values));
    if(s.projection==Projection::SelectedValue)return values.array.empty()?number(0):values.array.front();
    auto result=cj::object();result.object={{"selected_ids",typedArray(strings(s.selection.selected))},
        {"selected_indexes",typedArray(selectedValues(widget,s,true))},{"selected_values",typedArray(std::move(values))}};return result;
}
bool setState(WidgetInstance &widget,State s,std::string *error) {
    const auto why=validate(widget,s);if(error)*error=why;if(!why.empty())return false;
    if(s.projection==Projection::SelectedValue&&!s.selection.selected.empty())for(const auto &item:widget.ringItems)if(item.id==s.selection.selected.front()){
        widget.numericValue=item.value;widget.initialValue=std::to_string(item.value);break;}
    widget.valueType=bindingType(s.projection).canonical();widget.listboxState=std::move(s);return true;
}
bool assignBindingValue(WidgetInstance &widget,const Json &value,std::string *error) {
    try {
        auto next=state(widget);readSelectionValue(widget,next,value);
        const auto selected=next.selection.selected;
        next.selection.active=selected.empty()?std::string{}:selected.back();next.selection.anchor=selected.empty()?std::string{}:selected.front();
        return setState(widget,std::move(next),error);
    }catch(const std::exception &e){if(error)*error=e.what();return false;}
}
void readState(WidgetInstance &widget,const Json &node) {
    auto next=state(widget);const auto *p=field(node,"props"),*v=field(node,"state");
    const auto empty=cj::object();const auto &props=p?*p:empty,&value=v?*v:empty;
    try {
        if(const auto *raw=field(props,"items")) {
            if(raw->type!=Json::Type::Array || raw->array.size()>65536)throw std::runtime_error("items: expected a bounded array");
            for(const auto &item:raw->array) {
                if(item.type==Json::Type::String)continue; // Existing compact label form.
                if(item.type!=Json::Type::Object)throw std::runtime_error("items: expected item records");
                if(const auto *id=item.member("id");id && (id->type!=Json::Type::String || id->string.empty()))throw std::runtime_error("items.id: expected a nonempty identity");
                for(const auto key:{"value","numeric_value"})if(const auto *value=item.member(key);value && !itemValue(*value))
                    throw std::runtime_error("items.value: this native projection requires an exact U16 item value");
            }
        }
        const auto mode=item_selection::parseMode(text(props,"selection.mode","single"));
        if(!mode)throw std::runtime_error("selection.mode: expected none, single, multiple or range");next.selection.mode=*mode;
        const auto projection=parseProjection(text(props,"binding.value_path","value.selected_value"));
        if(!projection)throw std::runtime_error("binding.value_path: unsupported Listbox projection");next.projection=*projection;
        next.doubleClickActivates=flag(props,"activation.double_click_enabled",true);next.enterActivates=flag(props,"activation.enter_key_enabled",true);
        std::optional<std::vector<std::string>> mirror;
        if(const auto *selected=field(props,"selection.selected_ids")){mirror=ids(*selected);next.selection.selected=*mirror;}
        bool explicitValue=false;
        if(const auto *selected=field(value,projectionPath(*projection))){readSelectionValue(widget,next,*selected);explicitValue=true;}
        else if(const auto *selected=field(value,"value")){readSelectionValue(widget,next,*selected);explicitValue=true;}
        if(mirror&&explicitValue&&*mirror!=next.selection.selected)throw std::runtime_error("selection.selected_ids: contradicts the explicit projected value");
        next.selection.active=text(props,"active.id",next.selection.selected.empty()?std::string{}:next.selection.selected.back());
        next.selection.anchor=text(props,"selection.anchor_id",next.selection.selected.empty()?std::string{}:next.selection.selected.front());
        if(widget.valueType!=bindingType(next.projection).canonical())throw std::runtime_error("valueType: declared type contradicts binding.value_path");
        if(const auto why=validate(widget,next);!why.empty())throw std::runtime_error(why);
    }catch(const std::exception &e) {
        next.diagnostic=e.what();auto saved=cj::object(),savedProps=cj::object(),savedState=cj::object();
        for(const auto key:propertyKeys)if(const auto *raw=field(props,key))savedProps.object.emplace(key,*raw);
        for(const auto key:stateKeys)if(const auto *raw=field(value,key))savedState.object.emplace(key,*raw);
        saved.object={{"props",std::move(savedProps)},{"state",std::move(savedState)}};next.preservedProperties=cj::encode(saved);
    }
    widget.listboxState=std::move(next);
}
Json properties(const WidgetInstance &widget) {
    const auto s=state(widget);if(s.preservedProperties)return retained(s,"props");
    auto result=cj::object();result.object={{"selection.mode",cj::string(std::string(item_selection::modeName(s.selection.mode)))},
        {"binding.value_path",cj::string(std::string(projectionPath(s.projection)))},{"selection.selected_ids",strings(s.selection.selected)},
        {"active.id",cj::string(s.selection.active)},{"selection.anchor_id",cj::string(s.selection.anchor)},
        {"activation.double_click_enabled",cj::boolean(s.doubleClickActivates)},{"activation.enter_key_enabled",cj::boolean(s.enterActivates)}};return result;
}
Json currentState(const WidgetInstance &widget) {
    const auto s=state(widget);if(s.preservedProperties)return retained(s,"state");
    auto result=cj::object();if(auto value=bindingValue(widget)) {
        if(s.projection==Projection::SelectedValues||s.projection==Projection::SelectedIds){auto elements=sequence(*value);*value=std::move(elements);}
        else if(s.projection==Projection::Selection)for(auto &[_,part]:value->object){auto elements=sequence(part);part=std::move(elements);}
        result.object.emplace(std::string(projectionPath(s.projection)),std::move(*value));
    }
    return result;
}
} // namespace frog::native_engine::listbox_widget
