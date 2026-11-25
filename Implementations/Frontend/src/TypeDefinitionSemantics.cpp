#include "frog_native_engine/TypeDefinitionSemantics.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/ContainedWidgetSchemaMetadata.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/WidgetDataOperations.hpp"
#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include "frog_native_engine/ListboxWidget.hpp"
#include "frog_native_engine/PictureRing.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/TabControl.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "FrogDocumentJsonValue.hpp"
#include "FrogDocumentJsonWriter.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>

namespace frog::native_engine {
namespace {
using source::JsonValue;
void errorText(std::string *error, std::string value) { if(error)*error=std::move(value); }
std::string jsonText(const JsonValue &value) {
    std::ostringstream out; document_internal::appendJsonValue(out,value);return out.str();
}
void stringField(std::ostringstream &out,std::string_view key,std::string_view value,bool comma=true) {
    document_internal::appendJsonField(out,1,key,value,comma);
}
std::string stringValue(const JsonValue &root,std::string_view key) {
    const auto *value=root.member(key);return value && value->type==JsonValue::Type::String?value->string:std::string{};
}
std::optional<std::uint64_t> revisionValue(const JsonValue &root,std::string_view key) {
    const auto *value=root.member(key);if(!value || value->type!=JsonValue::Type::Number)return {};
    const auto &text=value->string;std::uint64_t number=0;
    const auto result=std::from_chars(text.data(),text.data()+text.size(),number);
    if(result.ec!=std::errc{} || result.ptr!=text.data()+text.size() || !number)return {};
    return number;
}
bool textSafe(std::string_view value) { return !value.empty() && value.size()<=4096 && value.find('\0')==value.npos; }
std::string valueText(const WidgetInstance &widget) {
    if(clusterContainerState(widget)||arrayContainerState(widget))return data_operations::encode(widget);
    if(isEnumWidget(widget))return ringSelectedItemId(widget);
    if(isBooleanWidget(widget)||isButtonWidget(widget))return widget.booleanValue.value_or(false)?"true":"false";
    if(isStringWidget(widget))return std::string(widgetStringValue(widget));
    if(widget.initialValue)return *widget.initialValue;
    if(widget.numericValue)return formatNumericValue(*widget.numericValue);
    return {};
}
std::vector<const ClusterElement *> ordered(const ClusterContainerState &cluster) {
    std::vector<const ClusterElement *> result;for(const auto &field:cluster.elements)result.push_back(&field);
    std::stable_sort(result.begin(),result.end(),[](auto *a,auto *b){return a->logicalOrder<b->logicalOrder;});return result;
}
std::string_view schemaFamily(const WidgetInstance &widget) {
    if(widget.pictureRing)return "picture-ring";
    if(isStringDropdownWidget(widget))return "string-dropdown";
    if(isTabWidget(widget))return "tab";
    if(isButtonWidget(widget)||isBooleanWidget(widget))return "boolean";
    const auto *definition=widgetDefinition(widget);
    if(!definition)return widget.classId;
    auto id=definition->classId;
    for(const std::string_view suffix:{"_control","_indicator"})
        if(id.ends_with(suffix)) {id.remove_suffix(suffix.size());break;}
    return id;
}
void framed(std::ostream &out,std::string_view value) {out<<value.size()<<':'<<value;}
void schemaText(std::ostringstream &out,const WidgetInstance &widget) {
    const auto type=binding::widgetType(widget);out<<type.type.canonical()<<';';
    // Keep V1 fingerprints for the original scalar, cluster and Array shapes.
    if(widget.pictureRing||isTabWidget(widget)||isStringDropdownWidget(widget)||
        !(isNumericWidget(widget)||isBooleanWidget(widget)||isButtonWidget(widget)||isStringWidget(widget)||
          isEnumWidget(widget)||isArrayWidget(widget)||isClusterWidget(widget)))framed(out,schemaFamily(widget));
    if(widget.referenceValue||widget.collection||widget.xyGraph||widget.waveformGraph) {
        framed(out,jsonText(application_control_source::writeType(type.type)));
    }
    if(widget.collection) {
        framed(out,hashing::sha256Hex(widget.collection->keyPresentation));
        framed(out,hashing::sha256Hex(widget.collection->valuePresentation));
    }
    if(isListboxWidget(widget)) {
        const auto state=listbox_widget::state(widget);
        out<<static_cast<int>(state.selection.mode)<<';'<<static_cast<int>(state.projection)<<';';
    }
    if(isNumericWidget(widget)) {
        const auto representation=numericRepresentationKind(widget);out<<representation<<';';
        if(representation=="fixed_point") {const auto fixed=fixedPointConfiguration(widget);
            out<<fixed.signedEncoding<<':'<<fixed.complex<<':'<<fixed.wordLength<<':'<<fixed.integerWordLength<<':'<<fixed.roundingMode<<':'<<fixed.overflowMode<<';';}
    }
    if(typeDefinitionHasChoiceCatalog(widget))for(const auto &item:widget.ringItems) {
        out<<item.id.size()<<':'<<item.id<<item.label.size()<<':'<<item.label<<'='<<item.value<<';';
        if(item.disabled||!item.visible)out<<"flags:"<<item.disabled<<','<<item.visible<<';';
        if(widget.pictureRing)framed(out,item.imageDataUri?hashing::sha256Hex(*item.imageDataUri):std::string{});
        if(isTabWidget(widget)) {framed(out,tabPageCaption(item));out<<item.captionMatchesLabel<<';';}
    }
    if(const auto *cluster=clusterContainerState(widget))for(const auto *field:ordered(*cluster)) {
        out<<field->fieldId.size()<<':'<<field->fieldId;
        if(field->widget) {out<<field->widget->label.size()<<':'<<field->widget->label;schemaText(out,*field->widget);}
    }
    if(const auto *array=arrayContainerState(widget);array && array->elementTemplate)schemaText(out,*array->elementTemplate);
}
void remapChildIdentity(WidgetInstance &widget,std::string id) {
    widget.instanceId=std::move(id);
    if(auto *cluster=clusterContainerState(widget))for(auto &field:cluster->elements)if(field.widget)
        remapChildIdentity(*field.widget,widget.instanceId+"."+field.fieldId);
    if(auto *array=arrayContainerState(widget);array && array->elementTemplate)
        remapChildIdentity(*array->elementTemplate,widget.instanceId+".element");
}
void refreshNestedLinks(WidgetInstance &local,const WidgetInstance &source,bool root=true) {
    if(!root)local.typeDefinition=source.typeDefinition;
    if(auto *cluster=clusterContainerState(local)) {
        const auto *other=clusterContainerState(source);
        if(other)for(auto &field:cluster->elements) {
            const auto found=std::find_if(other->elements.begin(),other->elements.end(),[&](const auto &next){return next.fieldId==field.fieldId;});
            if(field.widget && found!=other->elements.end() && found->widget)refreshNestedLinks(*field.widget,*found->widget,false);
        }
    } else if(auto *array=arrayContainerState(local);array && array->elementTemplate) {
        const auto *other=arrayContainerState(source);if(other && other->elementTemplate)
            refreshNestedLinks(*array->elementTemplate,*other->elementTemplate,false);
    }
}
bool associationShape(const WidgetInstance &local,const WidgetInstance &source,unsigned depth=0) {
    if(depth>128||!typeDefinitionSameWidgetFamily(local,source))return false;
    if(const auto *cluster=clusterContainerState(local)) {
        const auto *other=clusterContainerState(source);if(!other || cluster->elements.size()!=other->elements.size())return false;
        const auto a=ordered(*cluster),b=ordered(*other);
        for(std::size_t i=0;i<a.size();++i)if(!a[i]->widget || !b[i]->widget ||
            a[i]->widget->label!=b[i]->widget->label || !associationShape(*a[i]->widget,*b[i]->widget,depth+1))return false;
        return true;
    }
    if(const auto *array=arrayContainerState(local)) {
        const auto *other=arrayContainerState(source);return other && array->rank==other->rank &&
            array->elementTemplate && other->elementTemplate && associationShape(*array->elementTemplate,*other->elementTemplate,depth+1);
    }
    if(typeDefinitionHasChoiceCatalog(local)) {
        if(local.ringItems.size()!=source.ringItems.size())return false;
        for(std::size_t i=0;i<local.ringItems.size();++i)
            if(local.ringItems[i].label!=source.ringItems[i].label || local.ringItems[i].value!=source.ringItems[i].value)return false;
        return true;
    }
    if(isReferenceWidget(local)&&isReferenceWidget(source))
        return binding::connect(referenceWidgetType(local),referenceWidgetType(source)).accepted();
    return binding::widgetType(local).type==binding::widgetType(source).type ||
        (isNumericWidget(local)&&isNumericWidget(source));
}
bool alignAssociation(WidgetInstance &local,const WidgetInstance &source,bool defaults=true,unsigned depth=0) {
    if(depth>128)return false;
    const auto saved=local.defaultData;local.defaultData.reset();
    if(auto *cluster=clusterContainerState(local)) {
        const auto a=ordered(*cluster),b=ordered(*clusterContainerState(source));
        for(std::size_t i=0;i<a.size();++i) {
            auto *field=const_cast<ClusterElement *>(a[i]);
            if(!alignAssociation(*field->widget,*b[i]->widget,defaults,depth+1))return false;
            field->fieldId=b[i]->fieldId;field->technicalName=b[i]->technicalName;
        }
    } else if(auto *array=arrayContainerState(local)) {
        const auto *other=arrayContainerState(source);const auto before=*array->elementTemplate;
        if(!alignAssociation(*array->elementTemplate,*other->elementTemplate,defaults,depth+1))return false;
        const auto cells=array->elements;array->elements.clear();
        std::vector<ArrayElementValue> preparedCells;preparedCells.reserve(cells.size());
        static_cast<void>(refreshContainedWidgetSchemaMetadata(local));
        for(const auto &cell:cells) {
            auto current=before;
            if(cell.kind==ArrayElementValueKind::Widget) {const auto value=deserializeArrayElementWidget(cell.displayValue,&before);if(!value)return false;current=*value;}
            else materializeArrayElementValue(current,&cell);
            if(!alignAssociation(current,*other->elementTemplate,false,depth+1))return false;
            auto prepared=prepareArrayElementValueFromWidget(local,cell.indices,current);if(!prepared)return false;
            prepared->defined=cell.defined;prepared->enabled=cell.enabled;preparedCells.push_back(std::move(*prepared));
        }
        array=arrayContainerState(local);array->elements=std::move(preparedCells);
    } else if(typeDefinitionHasChoiceCatalog(local)) {
        // First association may match equivalent independently authored items.
        // Later revisions always migrate by the adopted stable identity.
        if(!local.ringItems.empty() && local.ringItems.size()!=source.ringItems.size())return false;
        const auto previous=local.ringItems;
        auto state=isListboxWidget(local)?std::optional{listbox_widget::state(local)}:std::nullopt;
        const auto alignedId=[&](const std::string &id) {
            const auto it=std::find_if(previous.begin(),previous.end(),[&](const auto &item){return item.id==id;});
            return it==previous.end()?id:source.ringItems[static_cast<std::size_t>(it-previous.begin())].id;
        };
        if(local.tabContainer)local.tabContainer->editorPageId=alignedId(local.tabContainer->editorPageId);
        local.ringItems=source.ringItems;local.enumDomainId=source.enumDomainId;local.valueType=source.valueType;
        if((isEnumWidget(local)||isTabWidget(local)) && local.initialValue)
            local.initialValue=alignedId(*local.initialValue);
        if(state) {
            for(auto &id:state->selection.selected)id=alignedId(id);
            state->selection.active=alignedId(state->selection.active);
            state->selection.anchor=alignedId(state->selection.anchor);
            if(!listbox_widget::validate(local,*state).empty())return false;
            static_cast<void>(listbox_widget::setState(local,*state));
        }
    }
    if(defaults && saved) {
        auto value=data_operations::decodeStoredDefault(local,*saved);if(!value || !alignAssociation(*value,source,false,depth+1))return false;
        const auto encoded=data_operations::encode(*value);if(encoded.empty())return false;local.defaultData=encoded;
    }
    return refreshContainedWidgetSchemaMetadata(local);
}
struct Migration {
    bool accept=false;
    std::vector<TypeDefinitionDiagnostic> diagnostics;
    std::size_t count=0;
    void conflict(std::string code,std::string message,const WidgetInstance &old,const WidgetInstance &next,
        const std::string &path) {
        if(accept)return;
        if(diagnostics.size()<256)diagnostics.push_back({std::move(code),std::move(message),old.instanceId,path,
            valueText(old),binding::widgetType(next).type.canonical(),valueText(next)});
        else if(diagnostics.size()==256)diagnostics.push_back({"typedef.review_budget",
            "Additional migration conflicts remain in the retained original data; this instance requires review before execution.",old.instanceId,path,{},{},{}});
    }
    WidgetInstance replacement(const WidgetInstance &old,const WidgetInstance &next) {
        // Standard definitions replace schema/data only. Keep every local
        // presentation override, even when the user approves a new value.
        auto result=old;result.classId=next.classId;result.valueType=next.valueType;
        result.sourceExamplePath=next.sourceExamplePath;result.wfrogPackagePath=next.wfrogPackagePath;
        result.realizationPackageRef=next.realizationPackageRef;result.assetRef=next.assetRef;
        result.initialValue=next.initialValue;result.numericValue=next.numericValue;result.booleanValue=next.booleanValue;
        result.supportTextValue=next.supportTextValue;
        result.numericRepresentationKind=next.numericRepresentationKind;result.numericFixedPointConfiguration=next.numericFixedPointConfiguration;
        result.enumDomainId=next.enumDomainId;result.ringItems=next.ringItems;
        result.pictureRing=next.pictureRing;result.stringDropdown=next.stringDropdown;
        result.numericColorBox=next.numericColorBox;result.ringAllowUndefinedValues=next.ringAllowUndefinedValues;
        result.collection=next.collection;result.xyGraph=next.xyGraph;result.waveformGraph=next.waveformGraph;
        result.pictureDisplay=next.pictureDisplay;result.referenceValue=next.referenceValue;
        result.tabContainer=next.tabContainer;result.listboxState=next.listboxState;
        result.arrayContainer=next.arrayContainer;result.clusterContainer=next.clusterContainer;
        remapChildIdentity(result,old.instanceId);result.role=next.role;result.bounds=old.bounds;
        static_cast<void>(setWidgetRole(result,old.role=="indicator"?WidgetRole::Indicator:WidgetRole::Control));
        result.label=old.label;result.caption=old.caption;result.captionVisible=old.captionVisible;
        result.labelOffsetX=old.labelOffsetX;result.labelOffsetY=old.labelOffsetY;
        result.publicInputId=old.publicInputId;result.publicOutputId=old.publicOutputId;
        result.interfaceMapSlotId=old.interfaceMapSlotId;result.interfaceConnectionRequirement=old.interfaceConnectionRequirement;
        return result;
    }
    void choices(WidgetInstance &result,const WidgetInstance &old,const WidgetInstance &previous,
        const WidgetInstance &next,const std::string &path) {
        result.ringItems=next.ringItems;
        result.enumDomainId=next.enumDomainId;
        result.valueType=next.valueType;
        result.numericRepresentationKind=next.numericRepresentationKind;
        result.ringAllowUndefinedValues=next.ringAllowUndefinedValues;
        // Retain per-occurrence presentation changes, while inheriting a new
        // flag when the occurrence still has its previous definition's value.
        for(auto &item:result.ringItems) {
            const auto find=[&](const auto &list){return std::find_if(list.begin(),list.end(),
                [&](const RingItem &candidate){return candidate.id==item.id;});};
            const auto local=find(old.ringItems),base=find(previous.ringItems);
            if(local!=old.ringItems.end()&&base!=previous.ringItems.end()) {
                if(local->disabled!=base->disabled)item.disabled=local->disabled;
                if(local->visible!=base->visible)item.visible=local->visible;
            }
        }
        if(isListboxWidget(old)) {
            auto selection=listbox_widget::state(old);
            const auto edited=listbox_widget::state(next);
            selection.projection=edited.projection;selection.selection.mode=edited.selection.mode;
            if(const auto reason=listbox_widget::validate(result,selection);!reason.empty()) {
                conflict("typedef.listbox_selection",reason,old,next,path);
                selection=edited;
            }
            static_cast<void>(listbox_widget::setState(result,std::move(selection)));return;
        }
        if(isTabWidget(result)&&result.tabContainer&&next.tabContainer) {
            result.tabContainer->nextPageSerial=next.tabContainer->nextPageSerial;
            if(!tabPageIndex(result,result.tabContainer->editorPageId))
                result.tabContainer->editorPageId=tabEditorPageId(next);
        }
        // Picture Ring saved defaults hold the U16 index alone. Resolve it
        // against the previous catalogue before substituting the new one.
        const auto &prior=old.pictureRing&&old.ringItems.empty()?previous.ringItems:ringItems(old);
        const auto selected=std::find_if(prior.begin(),prior.end(),
            [&](const auto &item){return old.numericValue==double(item.value);});
        if(isStringDropdownWidget(old)&&(selected==prior.end()||widgetStringValue(old)!=selected->label))
            return; // A String assigned independently of the choices is still its local value.
        if(selected==prior.end() && (prior.empty()||ringAllowUndefinedValues(old)))return;
        if(selected!=prior.end()) {
            const auto item=std::find_if(result.ringItems.begin(),result.ringItems.end(),
                [&](const auto &choice){return choice.id==selected->id;});
            if(item!=result.ringItems.end()) {
                result.numericValue=double(item->value);
                if(isStringDropdownWidget(result))static_cast<void>(setWidgetStringValue(result,item->label));
                else if(isEnumWidget(result)||isTabWidget(result))result.initialValue=item->id;
                else result.initialValue=std::to_string(item->value);
                return;
            }
        }
        conflict(isEnumWidget(old)?"typedef.enum_item_removed":"typedef.choice_item_removed",
            "The selected item no longer exists in this Type Def.",old,next,path);
        result.initialValue=next.initialValue;result.numericValue=next.numericValue;
        result.supportTextValue=next.supportTextValue;
    }
    bool numeric(WidgetInstance &result,const WidgetInstance &old,const WidgetInstance &next) {
        result.valueType=next.valueType;result.numericRepresentationKind=next.numericRepresentationKind;
        result.numericFixedPointConfiguration=next.numericFixedPointConfiguration;
        if(numericCanonicalValueTypeIsComplex(old.valueType) || numericCanonicalValueTypeIsComplex(next.valueType)) {
            const auto token=widgetNumericComplexValueLexeme(old);
            if(!token || !numericCanonicalValueTypeIsComplex(next.valueType))return false;
            const auto before=numeric_value::parseComplexNumber(*token,old.valueType);
            const auto after=numeric_value::parseComplexNumber(*token,next.valueType);
            return before && after && before->real==after->real && before->imaginary==after->imaginary &&
                setWidgetNumericComplexValueLexeme(result,*token);
        }
        if(numericCanonicalValueTypeIsIntegral(old.valueType)) {
            const auto token=widgetNumericIntegerValueLexeme(old);if(!token)return false;
            if(numericCanonicalValueTypeIsIntegral(next.valueType))return setWidgetNumericIntegerValueLexeme(result,*token);
            double number=0;const auto read=std::from_chars(token->data(),token->data()+token->size(),number);
            if(read.ec!=std::errc{} || read.ptr!=token->data()+token->size() || !std::isfinite(number))return false;
            if(next.valueType=="f32" && double(float(number))!=number)return false;
            std::array<char,512> buffer{};const auto write=std::to_chars(buffer.data(),buffer.data()+buffer.size(),number,std::chars_format::fixed,0);
            if(write.ec!=std::errc{} || std::string_view(buffer.data(),write.ptr)!=*token)return false;
            if(next.numericRepresentationKind==std::optional<std::string>{"fixed_point"})
                return setWidgetNumericValue(result,number)&&result.numericValue==number;
            result.initialValue=*token;result.numericValue=number;return true;
        }
        const double number=old.numericValue.value_or(0);
        if(!std::isfinite(number))return false;
        if(numericCanonicalValueTypeIsIntegral(next.valueType)) {
            if(std::trunc(number)!=number)return false;
            std::array<char,512> buffer{};const auto write=std::to_chars(buffer.data(),buffer.data()+buffer.size(),number,std::chars_format::fixed,0);
            return write.ec==std::errc{} && setWidgetNumericIntegerValueLexeme(result,std::string_view(buffer.data(),write.ptr));
        }
        if(next.valueType=="f32" && double(float(number))!=number)return false;
        if(next.numericRepresentationKind==std::optional<std::string>{"fixed_point"})
            return setWidgetNumericValue(result,number)&&result.numericValue==number;
        result.numericValue=old.numericValue;result.initialValue=old.initialValue;return true;
    }
    WidgetInstance run(const WidgetInstance &old,const WidgetInstance &previous,const WidgetInstance &next,
        const std::string &path,unsigned depth=0,bool defaults=true) {
        auto result=old;
        if(++count>250000 || depth>128) {
            conflict("typedef.migration_budget","Type Definition migration exceeds the document budget.",old,next,path);return result;
        }
        const auto oldDefault=old.defaultData;result.defaultData.reset();
        if(const auto *target=clusterContainerState(next)) {
            const auto *source=clusterContainerState(old),*prior=clusterContainerState(previous);
            if(!source || !prior) {conflict("typedef.incompatible_type","Replacing this value requires approval.",old,next,path);result=replacement(old,next);}
            else {
                auto *output=clusterContainerState(result);std::vector<ClusterElement> fields;
                for(const auto *field:ordered(*target)) {
                    const auto found=std::find_if(source->elements.begin(),source->elements.end(),[&](const auto &v){return v.fieldId==field->fieldId;});
                    const auto before=std::find_if(prior->elements.begin(),prior->elements.end(),[&](const auto &v){return v.fieldId==field->fieldId;});
                    if(found!=source->elements.end() && found->widget && field->widget && before!=prior->elements.end() && before->widget) {
                        auto copy=*found;copy.logicalOrder=field->logicalOrder;
                        copy.widget=std::make_shared<WidgetInstance>(run(*found->widget,*before->widget,*field->widget,path+"/"+field->fieldId,depth+1,defaults));
                        copy.widget->label=field->widget->label;fields.push_back(std::move(copy));
                    } else {
                        auto copy=*field;if(copy.widget) {copy.widget=std::make_shared<WidgetInstance>(*copy.widget);remapChildIdentity(*copy.widget,old.instanceId+"."+field->fieldId);}
                        if(copy.widget) {
                            result.bounds.width=(std::max)(result.bounds.width,copy.widget->bounds.right()+output->padding);
                            result.bounds.height=(std::max)(result.bounds.height,copy.widget->bounds.bottom()+output->padding);
                        }
                        fields.push_back(std::move(copy));
                    }
                }
                for(const auto &field:source->elements)if(std::none_of(target->elements.begin(),target->elements.end(),[&](const auto &v){return v.fieldId==field.fieldId;}))
                    if(field.widget)conflict("typedef.field_removed","A cluster field and its local data would be removed.",*field.widget,next,path+"/"+field.fieldId);
                output->elements=std::move(fields);
            }
        } else if(const auto *target=arrayContainerState(next)) {
            const auto *source=arrayContainerState(old),*prior=arrayContainerState(previous);
            if(!source || !prior || !source->elementTemplate || !prior->elementTemplate || !target->elementTemplate || source->rank!=target->rank) {
                conflict("typedef.array_rank_or_type","Array rank or element replacement requires approval.",old,next,path);result=replacement(old,next);
            } else {
                auto *output=arrayContainerState(result);
                auto prototype=run(*source->elementTemplate,*prior->elementTemplate,*target->elementTemplate,path+"/element",depth+1,defaults);
                output->elementTemplate=std::make_shared<WidgetInstance>(prototype);
                output->cellWidth=(std::max)(output->cellWidth,prototype.bounds.width);
                output->cellHeight=(std::max)(output->cellHeight,prototype.bounds.height);
                static_cast<void>(refreshContainedWidgetSchemaMetadata(result));
                const auto cells=source->elements;output->elements.clear();
                std::vector<ArrayElementValue> preparedCells;preparedCells.reserve(cells.size());
                for(const auto &cell:cells) {
                    auto current=*source->elementTemplate;
                    if(cell.kind==ArrayElementValueKind::Widget) {
                        const auto decoded=deserializeArrayElementWidget(cell.displayValue,source->elementTemplate.get());
                        if(!decoded) {conflict("typedef.array_value_invalid","The previous Array element cannot be decoded.",old,next,path);continue;}
                        current=*decoded;
                    } else materializeArrayElementValue(current,&cell);
                    auto value=run(current,*prior->elementTemplate,*target->elementTemplate,path+"/cell",depth+1,false);
                    auto prepared=prepareArrayElementValueFromWidget(result,cell.indices,value);
                    if(!prepared) {conflict("typedef.array_value_invalid","The migrated Array element cannot be stored.",current,next,path);continue;}
                    prepared->defined=cell.defined;prepared->enabled=cell.enabled;preparedCells.push_back(std::move(*prepared));
                }
                output=arrayContainerState(result);output->elements=std::move(preparedCells);
                const auto minimum=minimumArrayBoundsForVisibleCounts(result);
                result.bounds.width=(std::max)(result.bounds.width,minimum.width);
                result.bounds.height=(std::max)(result.bounds.height,minimum.height);
            }
        } else if(!typeDefinitionSameWidgetFamily(old,next)) {
            conflict("typedef.incompatible_type","Replacing this control family requires approval.",old,next,path);result=replacement(old,next);
        } else if(typeDefinitionHasChoiceCatalog(old)) {
            choices(result,old,previous,next,path);
        } else if(isNumericWidget(old) && isNumericWidget(next)) {
            const auto fixedKey=[](const WidgetInstance &widget) {
                if(!widget.numericFixedPointConfiguration)return std::string{};const auto &f=*widget.numericFixedPointConfiguration;
                return std::to_string(f.signedEncoding)+":"+std::to_string(f.complex)+":"+std::to_string(f.wordLength)+":"+std::to_string(f.integerWordLength)+":"+f.roundingMode+":"+f.overflowMode;
            };
            if((old.valueType!=next.valueType || old.numericRepresentationKind!=next.numericRepresentationKind || fixedKey(old)!=fixedKey(next)) && !numeric(result,old,next)) {
                conflict("typedef.numeric_loss","The numeric value cannot be represented exactly by the new type.",old,next,path);result=replacement(old,next);
            }
        } else if(isReferenceWidget(old)&&isReferenceWidget(next)) {
            const auto sourceType=referenceWidgetType(old),targetType=referenceWidgetType(next);
            if(sourceType!=targetType) {
                auto schema=referenceWidget(old.instanceId,targetType);
                if(binding::connect(sourceType,targetType).accepted()) {
                    if(old.referenceValue)schema.referenceValue->target=old.referenceValue->target;
                } else {
                    conflict("typedef.incompatible_type","The local reference is not assignable to the new type. Approval resets it to a typed null.",old,next,path);
                }
                result.valueType=std::move(schema.valueType);
                result.referenceValue=std::move(schema.referenceValue);
            }
        } else if(binding::widgetType(old).type!=binding::widgetType(next).type) {
            conflict("typedef.incompatible_type","Replacing this value requires approval.",old,next,path);result=replacement(old,next);
        } else if(result.collection&&previous.collection&&next.collection) {
            if(result.collection->keyPresentation==previous.collection->keyPresentation)
                result.collection->keyPresentation=next.collection->keyPresentation;
            if(result.collection->valuePresentation==previous.collection->valuePresentation)
                result.collection->valuePresentation=next.collection->valuePresentation;
        }
        if(defaults && oldDefault) {
            if(const auto saved=data_operations::decodeStoredDefault(old,*oldDefault)) {
                auto migrated=run(*saved,previous,next,path+"/<default>",depth+1,false);
                const auto encoded=data_operations::encode(migrated);
                if(encoded.empty())conflict("typedef.default_invalid","The migrated saved default cannot be encoded.",*saved,next,path);
                else result.defaultData=encoded;
            } else conflict("typedef.default_invalid","The saved default cannot be decoded without losing data.",old,next,path);
        } else result.defaultData.reset();
        result.typeDefinition=path.empty()?old.typeDefinition:next.typeDefinition;
        if(!refreshContainedWidgetSchemaMetadata(result))conflict("typedef.schema_cycle","The migrated schema contains a cycle.",old,next,path);
        return result;
    }
};
}

bool typeDefinitionSameWidgetFamily(const WidgetInstance &local,const WidgetInstance &source) {
    return schemaFamily(local)==schemaFamily(source);
}
bool typeDefinitionHasChoiceCatalog(const WidgetInstance &widget) {
    return isEnumWidget(widget)||isRingWidget(widget)||isStringDropdownWidget(widget)||isListboxWidget(widget);
}
std::vector<SceneObject> typeDefinitionTabChildren(FrogWidgetDataView canvas,std::string_view rootId) {
    std::unordered_multimap<std::string_view,std::string_view> byParent;
    for(const auto &object:canvas.objects())
        if(!object.parentTabId.empty())byParent.emplace(object.parentTabId,object.id);
    std::vector<std::string_view> parents{rootId};
    std::unordered_set<std::string_view> found{rootId};
    while(!parents.empty()) {
        const auto [first,last]=byParent.equal_range(parents.back());
        parents.pop_back();
        for(auto it=first;it!=last;++it)
            if(found.insert(it->second).second)parents.push_back(it->second);
    }
    std::vector<SceneObject> result;
    for(const auto &object:canvas.objects())
        if(object.id!=rootId && found.contains(object.id))result.push_back(object);
    return result;
}
bool typeDefinitionSupported(const WidgetInstance &root,std::string *error) {
    if(error)error->clear();const auto type=binding::widgetType(root);
    if(!type.valid() || !type.type.known()) {errorText(error,"Type Definition requires a complete, acyclic type: "+type.error);return false;}
    std::vector<std::pair<const WidgetInstance *,unsigned>> pending{{&root,0}};std::size_t count=0;
    while(!pending.empty()) {
        const auto [widget,depth]=pending.back();pending.pop_back();
        if(++count>250000 || depth>128){errorText(error,"Type Definition exceeds the document schema budget.");return false;}
        if(const auto *cluster=clusterContainerState(*widget)) {for(const auto &field:cluster->elements)if(field.widget)pending.push_back({field.widget.get(),depth+1});}
        else if(const auto *array=arrayContainerState(*widget)) {if(!array->elementTemplate){errorText(error,"Type Definition Array requires an element type.");return false;}pending.push_back({array->elementTemplate.get(),depth+1});}
        else if(!widgetSupportsDiagramTerminal(*widget)) {
            errorText(error,"A Type Definition requires a control or indicator with a data terminal.");return false;
        }
        if(typeDefinitionHasChoiceCatalog(*widget)) {
            std::unordered_set<std::string> ids;for(const auto &item:widget->ringItems)
                if(item.id.empty() || !ids.insert(item.id).second){errorText(error,"Choice item identities must be unique and nonempty.");return false;}
            if(widget->pictureRing) {
                if(!isRingWidget(*widget)||widget->ringItems.size()>kPictureRingMaximumItems) {
                    errorText(error,"Invalid Picture Ring catalog.");return false;
                }
                for(std::size_t index=0;index<widget->ringItems.size();++index) {
                    const auto &item=widget->ringItems[index];
                    if(item.value!=index) {errorText(error,"Picture Ring indices must follow catalog order.");return false;}
                    if(!item.imageDataUri||item.imageDataUri->empty()) {
                        errorText(error,"Picture Ring item '"+item.label+"' has no embedded image.");return false;
                    }
                }
            }
        }
    }
    return true;
}
std::string typeDefinitionSchemaFingerprint(const WidgetInstance &widget) {
    if(!typeDefinitionSupported(widget))return {};std::ostringstream text;schemaText(text,widget);return hashing::sha256Hex(text.str());
}
std::string typeDefinitionSchemaFingerprint(const TypeDefinitionDocument &definition) {
    const auto root=typeDefinitionSchemaFingerprint(definition.root);
    if(root.empty()||definition.tabChildren.empty())return root;
    std::ostringstream data;
    data<<root<<std::setprecision(17);
    for(const auto &child:definition.tabChildren) {
        framed(data,child.id);framed(data,child.parentTabId);framed(data,child.parentTabPageId);
        data<<child.bounds.x-definition.root.bounds.x<<','<<child.bounds.y-definition.root.bounds.y
            <<','<<child.bounds.width<<','<<child.bounds.height<<';'<<child.locked<<child.hidden;
        if(child.widget) {
            framed(data,child.widget->role);framed(data,child.widget->label);
            schemaText(data,*child.widget);
        } else if(child.staticVisual) {
            const auto &visual=*child.staticVisual;
            framed(data,visual.mediaRef);framed(data,visual.svgText);
            if(!visual.dibBytes.empty()&&!staticVisualUsesEncodedRasterSource(visual))
                framed(data,hashing::sha256Hex({reinterpret_cast<const char*>(visual.dibBytes.data()),visual.dibBytes.size()}));
            data<<visual.naturalWidth<<','<<visual.naturalHeight<<','<<visual.preserveAspectRatio
                <<','<<visual.rotationQuarterTurns<<','<<visual.flipHorizontal<<visual.flipVertical;
        }
    }
    return hashing::sha256Hex(data.str());
}
bool typeDefinitionSchemaEquivalent(const WidgetInstance &a,const WidgetInstance &b) {
    const auto left=typeDefinitionSchemaFingerprint(a);return !left.empty() && left==typeDefinitionSchemaFingerprint(b);
}
bool typeDefinitionCanAssociate(const WidgetInstance &a,const WidgetInstance &b) {
    return typeDefinitionSupported(a)&&typeDefinitionSupported(b)&&associationShape(a,b);
}
std::string serializeTypeDefinitionLink(const TypeDefinitionLink &link) {
    // Empty paths identify session drafts in Undo/model snapshots. The file
    // owner must save those definitions before any durable consumer write.
    if(!textSafe(link.definitionId)||(!link.sourcePath.empty()&&!textSafe(link.sourcePath))||!link.resolvedRevision||
        link.resolvedSchemaFingerprint.size()!=64 || link.baselineWidgetJson.empty())
        throw std::invalid_argument("An incomplete Type Definition link cannot be serialized.");
    std::ostringstream out;out<<"{\n";stringField(out,"definition_id",link.definitionId);stringField(out,"source_path",link.sourcePath);
    out<<"  \"resolved_revision\": "<<link.resolvedRevision<<",\n";
    stringField(out,"schema_fingerprint",link.resolvedSchemaFingerprint);
    if(!typeDefinitionUsesStructuredBaseline(link))stringField(out,"baseline_widget",link.baselineWidgetJson);
    else out<<"  \"baseline_widget\": "<<link.baselineWidgetJson<<",\n";
    if(!link.tabChildBindings.empty()) {
        out<<"  \"tab_members\": {";
        bool first=true;
        for(const auto &[source,occurrence]:link.tabChildBindings) {
            if(!first)out<<',';
            document_internal::appendJsonString(out,source);out<<':';
            document_internal::appendJsonString(out,occurrence);first=false;
        }
        out<<"},\n";
    }
    stringField(out,"unresolved_reason",link.unresolvedReason);stringField(out,"icon_svg",link.iconSvgText,false);out<<'}';return out.str();
}
std::optional<TypeDefinitionLink> deserializeTypeDefinitionLink(std::string_view source,std::string *error) {
    auto json=source::parseJson(source,error,source::JsonNumberPolicy::PreserveLexeme);
    if(!json || json->type!=JsonValue::Type::Object){errorText(error,"Invalid Type Definition link object.");return {};}
    TypeDefinitionLink link;link.definitionId=stringValue(*json,"definition_id");link.sourcePath=stringValue(*json,"source_path");
    const auto revision=revisionValue(*json,"resolved_revision");link.resolvedSchemaFingerprint=stringValue(*json,"schema_fingerprint");
    link.baselineWidgetJson=stringValue(*json,"baseline_widget");link.unresolvedReason=stringValue(*json,"unresolved_reason");link.iconSvgText=stringValue(*json,"icon_svg");
    if(const auto *baseline=json->member("baseline_widget");baseline&&baseline->type==JsonValue::Type::Object)
        link.baselineWidgetJson=source.substr(baseline->sourceBegin,baseline->sourceEnd-baseline->sourceBegin);
    if(const auto *members=json->member("tab_members")) {
        if(members->type!=JsonValue::Type::Object) {errorText(error,"Invalid Type Def Tab member bindings.");return {};}
        std::unordered_set<std::string> instances;
        for(const auto &[member,target]:members->object) {
            if(!textSafe(member)||target.type!=JsonValue::Type::String||!textSafe(target.string)||
                !instances.insert(target.string).second) {errorText(error,"Invalid or duplicate Type Def Tab member identity.");return {};}
            link.tabChildBindings.emplace(member,target.string);
        }
    }
    if(!textSafe(link.definitionId)||(!link.sourcePath.empty()&&!textSafe(link.sourcePath))||!revision||link.resolvedSchemaFingerprint.size()!=64 ||
        link.baselineWidgetJson.empty() || link.baselineWidgetJson.size()>kMaximumFrogDocumentBytes) {
        errorText(error,"Type Definition link is incomplete or invalid.");return {};
    }
    const auto baseline=deserializeTypeDefinitionWidget(link.baselineWidgetJson);
    if(!baseline || baseline->typeDefinition || !typeDefinitionSupported(*baseline)) {
        errorText(error,"Type Definition recovery snapshot is incomplete or invalid.");return {};
    }
    link.resolvedRevision=*revision;return link;
}
std::string serializeTypeDefinition(const TypeDefinitionDocument &document,std::string *error) {
    if(error)error->clear();if(!textSafe(document.id)||!textSafe(document.name)||!document.revision||!typeDefinitionSupported(document.root,error)) {
        if(error && error->empty())*error="Type Definition identity, name or revision is invalid.";return {};
    }
    const auto clearConnectorBinding=[](WidgetInstance &widget) {
        widget.publicInputId.reset();widget.publicOutputId.reset();
        widget.interfaceMapSlotId.reset();widget.interfaceConnectionRequirement.reset();
    };
    auto root=document.root;root.typeDefinition.reset();clearConnectorBinding(root);
    const bool hasChildren=!document.tabChildren.empty();
    if(hasChildren&&!isTabWidget(root)){errorText(error,"Only a Tab definition can own page controls.");return {};}
    std::string payload;
    try {
        if(hasChildren) {
            FrogWidgetData panel;
            panel.addObject({root.instanceId,root.bounds,root});
            for(const auto &child:document.tabChildren) {
                if(child.id.empty()||panel.objectById(child.id)||child.parentTabId.empty()||
                    (!child.widget && !child.staticVisual)||(child.widget && child.id!=child.widget->instanceId)) {
                    errorText(error,"Invalid or duplicate object in the Tab definition.");return {};
                }
                auto member=child;
                if(member.widget)clearConnectorBinding(*member.widget);
                panel.addObject(std::move(member));
            }
            if(!panel.validateTabMembership(error))return {};
            payload=serializeFrogDraft(panel,document.name);
        } else payload=serializeArrayElementWidget(root);
    } catch(const std::exception &e) {errorText(error,e.what());return {};}
    if(payload.empty()){errorText(error,"Type Definition root cannot be serialized.");return {};}
    std::ostringstream out;out<<"{\n";stringField(out,"format",kTypeDefinitionFormat);out<<"  \"draft_revision\": "<<(hasChildren?2:1)<<",\n";
    stringField(out,"id",document.id);stringField(out,"name",document.name);out<<"  \"revision\": "<<document.revision<<",\n";
    stringField(out,"icon_svg",document.iconSvgText);
    if(hasChildren)stringField(out,"tab_root_id",root.instanceId);
    out<<(hasChildren?"  \"tab_panel\": ":"  \"root\": ")<<payload<<"\n}\n";
    if(out.tellp()>static_cast<std::streamoff>(kMaximumFrogDocumentBytes)) {
        errorText(error,"Type Definition exceeds the FROG document byte limit.");return {};
    }
    return out.str();
}
std::optional<TypeDefinitionDocument> deserializeTypeDefinition(std::string_view source,std::string *error) {
    if(error)error->clear();auto json=source::parseJson(source,error,source::JsonNumberPolicy::PreserveLexeme);
    const auto format=json?revisionValue(*json,"draft_revision"):std::nullopt;
    if(!json || json->type!=JsonValue::Type::Object || stringValue(*json,"format")!=kTypeDefinitionFormat || (format!=1 && format!=2)) {
        errorText(error,"Unsupported Type Definition document format or revision.");return {};
    }
    TypeDefinitionDocument document;document.id=stringValue(*json,"id");document.name=stringValue(*json,"name");document.iconSvgText=stringValue(*json,"icon_svg");
    const auto revision=revisionValue(*json,"revision");
    const auto *stored=json->member(format==2?"tab_panel":"root");
    if(!stored || stored->type!=JsonValue::Type::Object){errorText(error,"Missing Type Definition widget payload.");return {};}
    const auto bytes=source.substr(stored->sourceBegin,stored->sourceEnd-stored->sourceBegin);
    std::optional<WidgetInstance> widget;
    if(format==2) {
        const auto rootId=stringValue(*json,"tab_root_id");
        auto panel=readFrogDraftData(bytes,error);
        if(!panel)return {};
        const auto *object=panel->widgets.objectById(rootId);
        if(!object||!object->widget||!isTabWidget(*object->widget)||!object->parentTabId.empty()||
            !panel->widgets.validateTabMembership(error)) {
            errorText(error,"Invalid root or page membership in the Tab definition.");return {};
        }
        widget=*object->widget;document.tabChildren=typeDefinitionTabChildren(panel->widgets,rootId);
        if(document.tabChildren.size()+1!=panel->widgets.objects().size()) {
            errorText(error,"The Tab definition contains an object outside its root.");return {};
        }
    } else widget=deserializeTypeDefinitionWidget(bytes);
    if(!textSafe(document.id)||!textSafe(document.name)||!revision||!widget||widget->typeDefinition||!typeDefinitionSupported(*widget,error)) {
        if(error && error->empty())*error="Type Definition identity, revision or root is invalid.";return {};
    }
    document.revision=*revision;document.root=*widget;return document;
}
TypeDefinitionMigration migrateTypeDefinitionWidget(const WidgetInstance &local,const WidgetInstance &previous,
    const WidgetInstance &current,bool acceptProposed) {
    Migration migration; migration.accept=acceptProposed;TypeDefinitionMigration result;
    std::string error;if(!typeDefinitionSupported(local,&error)||!typeDefinitionSupported(previous,&error)||!typeDefinitionSupported(current,&error)) {
        result.diagnostics.push_back({"typedef.schema_invalid",error,local.instanceId,{},{},{},{}});return result;
    }
    result.proposedWidget=local;
    if(typeDefinitionSchemaEquivalent(previous,current)&&typeDefinitionSchemaEquivalent(local,current)) {
        refreshNestedLinks(result.proposedWidget,current);
        result.changed=serializeArrayElementWidget(result.proposedWidget)!=serializeArrayElementWidget(local);return result;
    }
    result.proposedWidget=migration.run(local,previous,current,{});result.diagnostics=std::move(migration.diagnostics);
    result.changed=serializeArrayElementWidget(result.proposedWidget)!=serializeArrayElementWidget(local);return result;
}
TypeDefinitionMigration linkWidgetToTypeDefinition(const WidgetInstance &local,const TypeDefinitionDocument &definition,
    std::string path,bool acceptProposed) {
    std::string error;if(!typeDefinitionSupported(local,&error)) {
        TypeDefinitionMigration rejected;rejected.diagnostics.push_back({"typedef.schema_invalid",error,local.instanceId,{},{},{},{}});return rejected;
    }
    auto aligned=local;
    const bool associated=typeDefinitionCanAssociate(local,definition.root)&&alignAssociation(aligned,definition.root);
    auto result=migrateTypeDefinitionWidget(associated?aligned:local,associated?aligned:local,definition.root,acceptProposed);
    TypeDefinitionLink link;link.definitionId=definition.id;link.sourcePath=std::move(path);link.resolvedRevision=definition.revision;
    link.resolvedSchemaFingerprint=typeDefinitionSchemaFingerprint(definition);link.baselineWidgetJson=serializeArrayElementWidget(definition.root);
    link.iconSvgText=definition.iconSvgText;
    if(!result.accepted())link.unresolvedReason=result.diagnostics.front().message;
    result.proposedWidget.typeDefinition=std::move(link);result.changed=true;return result;
}
}
