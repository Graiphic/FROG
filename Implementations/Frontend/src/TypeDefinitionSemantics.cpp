#include "frog_native_engine/TypeDefinitionSemantics.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/ContainedWidgetSchemaMetadata.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/WidgetDataOperations.hpp"
#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "FrogDocumentJsonValue.hpp"
#include "FrogDocumentJsonWriter.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

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
void schemaText(std::ostringstream &out,const WidgetInstance &widget) {
    const auto type=binding::widgetType(widget);out<<type.type.canonical()<<';';
    if(isNumericWidget(widget)) {
        const auto representation=numericRepresentationKind(widget);out<<representation<<';';
        if(representation=="fixed_point") {const auto fixed=fixedPointConfiguration(widget);
            out<<fixed.signedEncoding<<':'<<fixed.complex<<':'<<fixed.wordLength<<':'<<fixed.integerWordLength<<':'<<fixed.roundingMode<<':'<<fixed.overflowMode<<';';}
    }
    if(isEnumWidget(widget))for(const auto &item:widget.ringItems)
        out<<item.id.size()<<':'<<item.id<<item.label.size()<<':'<<item.label<<'='<<item.value<<';';
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
    if(depth>128)return false;
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
    if(isEnumWidget(local) || isEnumWidget(source)) {
        if(!isEnumWidget(local)||!isEnumWidget(source)||local.ringItems.size()!=source.ringItems.size())return false;
        for(std::size_t i=0;i<local.ringItems.size();++i)
            if(local.ringItems[i].label!=source.ringItems[i].label || local.ringItems[i].value!=source.ringItems[i].value)return false;
        return true;
    }
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
        static_cast<void>(refreshContainedWidgetSchemaMetadata(local));
        for(const auto &cell:cells) {
            auto current=before;
            if(cell.kind==ArrayElementValueKind::Widget) {const auto value=deserializeArrayElementWidget(cell.displayValue,&before);if(!value)return false;current=*value;}
            else materializeArrayElementValue(current,&cell);
            if(!alignAssociation(current,*other->elementTemplate,false,depth+1))return false;
            auto prepared=prepareArrayElementValueFromWidget(local,cell.indices,current);if(!prepared)return false;
            prepared->defined=cell.defined;prepared->enabled=cell.enabled;array=arrayContainerState(local);array->elements.push_back(std::move(*prepared));
        }
    } else if(isEnumWidget(local)) {
        const auto id=ringSelectedItemId(local);const auto found=std::find_if(local.ringItems.begin(),local.ringItems.end(),[&](const auto &item){return item.id==id;});
        if(found==local.ringItems.end())return false;const auto index=static_cast<std::size_t>(found-local.ringItems.begin());
        local.ringItems=source.ringItems;local.enumDomainId=source.enumDomainId;local.valueType=source.valueType;
        local.numericValue=double(source.ringItems[index].value);local.initialValue=source.ringItems[index].id;
    }
    if(defaults && saved) {
        auto value=data_operations::decode(*saved);if(!value || !alignAssociation(*value,source,false,depth+1))return false;
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
        result.arrayContainer=next.arrayContainer;result.clusterContainer=next.clusterContainer;
        remapChildIdentity(result,old.instanceId);result.role=old.role;result.bounds=old.bounds;
        const bool indicator=old.role=="indicator";
        if(isClusterWidget(result))static_cast<void>(setClusterWidgetIndicatorRole(result,indicator));
        else if(isArrayWidget(result))static_cast<void>(setArrayWidgetIndicatorRole(result,indicator));
        else if(isEnumWidget(result))static_cast<void>(setEnumWidgetIndicatorRole(result,indicator));
        else if(isBooleanWidget(result))static_cast<void>(setBooleanWidgetIndicatorRole(result,indicator));
        else if(isButtonWidget(result))static_cast<void>(setButtonWidgetIndicatorRole(result,indicator));
        else if(isStringWidget(result))static_cast<void>(setStringWidgetIndicatorRole(result,indicator));
        else if(isNumericWidget(result))static_cast<void>(setNumericWidgetIndicatorRole(result,indicator));
        result.label=old.label;result.caption=old.caption;result.captionVisible=old.captionVisible;
        result.labelOffsetX=old.labelOffsetX;result.labelOffsetY=old.labelOffsetY;
        result.publicInputId=old.publicInputId;result.publicOutputId=old.publicOutputId;
        result.interfaceMapSlotId=old.interfaceMapSlotId;result.interfaceConnectionRequirement=old.interfaceConnectionRequirement;
        return result;
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
                    prepared->defined=cell.defined;prepared->enabled=cell.enabled;output=arrayContainerState(result);output->elements.push_back(std::move(*prepared));
                }
                const auto minimum=minimumArrayBoundsForVisibleCounts(result);
                result.bounds.width=(std::max)(result.bounds.width,minimum.width);
                result.bounds.height=(std::max)(result.bounds.height,minimum.height);
            }
        } else if(isEnumWidget(old) && isEnumWidget(next)) {
            const auto id=ringSelectedItemId(old);
            result.ringItems=next.ringItems;result.valueType=next.valueType;
            for(auto &item:result.ringItems) {
                const auto prior=std::find_if(old.ringItems.begin(),old.ringItems.end(),[&](const auto &candidate){return candidate.id==item.id;});
                if(prior!=old.ringItems.end()) {item.disabled=prior->disabled;item.visible=prior->visible;}
            }
            result.numericRepresentationKind=next.numericRepresentationKind;result.enumDomainId=next.enumDomainId;
            const auto item=std::find_if(next.ringItems.begin(),next.ringItems.end(),[&](const auto &v){return v.id==id;});
            if(item==next.ringItems.end()) {
                conflict("typedef.enum_item_removed","The selected Enum identity no longer exists.",old,next,path);
                result.initialValue=next.initialValue;result.numericValue=next.numericValue;
            } else {result.initialValue=item->id;result.numericValue=double(item->value);}
        } else if(isNumericWidget(old) && !isEnumWidget(old) && isNumericWidget(next) && !isEnumWidget(next)) {
            const auto fixedKey=[](const WidgetInstance &widget) {
                if(!widget.numericFixedPointConfiguration)return std::string{};const auto &f=*widget.numericFixedPointConfiguration;
                return std::to_string(f.signedEncoding)+":"+std::to_string(f.complex)+":"+std::to_string(f.wordLength)+":"+std::to_string(f.integerWordLength)+":"+f.roundingMode+":"+f.overflowMode;
            };
            if((old.valueType!=next.valueType || old.numericRepresentationKind!=next.numericRepresentationKind || fixedKey(old)!=fixedKey(next)) && !numeric(result,old,next)) {
                conflict("typedef.numeric_loss","The numeric value cannot be represented exactly by the new type.",old,next,path);result=replacement(old,next);
            }
        } else if(binding::widgetType(old).type!=binding::widgetType(next).type) {
            conflict("typedef.incompatible_type","Replacing this value requires approval.",old,next,path);result=replacement(old,next);
        }
        if(defaults && oldDefault) {
            if(const auto saved=data_operations::decode(*oldDefault)) {
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

bool typeDefinitionSupported(const WidgetInstance &root,std::string *error) {
    if(error)error->clear();const auto type=binding::widgetType(root);
    if(!type.valid() || !type.type.known()) {errorText(error,"Type Definition requires a complete, acyclic type: "+type.error);return false;}
    std::vector<std::pair<const WidgetInstance *,unsigned>> pending{{&root,0}};std::size_t count=0;
    while(!pending.empty()) {
        const auto [widget,depth]=pending.back();pending.pop_back();
        if(++count>250000 || depth>128){errorText(error,"Type Definition exceeds the document schema budget.");return false;}
        if(const auto *cluster=clusterContainerState(*widget)) {for(const auto &field:cluster->elements)if(field.widget)pending.push_back({field.widget.get(),depth+1});}
        else if(const auto *array=arrayContainerState(*widget)) {if(!array->elementTemplate){errorText(error,"Type Definition Array requires an element type.");return false;}pending.push_back({array->elementTemplate.get(),depth+1});}
        else if(!(isNumericWidget(*widget)||isBooleanWidget(*widget)||isButtonWidget(*widget)||isStringWidget(*widget)||isEnumWidget(*widget))) {
            errorText(error,"This widget family is not supported by standard Type Definitions V1.");return false;
        }
        if(isEnumWidget(*widget)) {
            std::unordered_set<std::string> ids;for(const auto &item:widget->ringItems)
                if(item.id.empty() || !ids.insert(item.id).second){errorText(error,"Enum item identities must be unique and nonempty.");return false;}
        }
    }
    return true;
}
std::string typeDefinitionSchemaFingerprint(const WidgetInstance &widget) {
    if(!typeDefinitionSupported(widget))return {};std::ostringstream text;schemaText(text,widget);return hashing::sha256Hex(text.str());
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
    stringField(out,"schema_fingerprint",link.resolvedSchemaFingerprint);stringField(out,"baseline_widget",link.baselineWidgetJson);
    stringField(out,"unresolved_reason",link.unresolvedReason);stringField(out,"icon_svg",link.iconSvgText,false);out<<'}';return out.str();
}
std::optional<TypeDefinitionLink> deserializeTypeDefinitionLink(std::string_view source,std::string *error) {
    auto json=source::parseJson(source,error,source::JsonNumberPolicy::PreserveLexeme);
    if(!json || json->type!=JsonValue::Type::Object){errorText(error,"Invalid Type Definition link object.");return {};}
    TypeDefinitionLink link;link.definitionId=stringValue(*json,"definition_id");link.sourcePath=stringValue(*json,"source_path");
    const auto revision=revisionValue(*json,"resolved_revision");link.resolvedSchemaFingerprint=stringValue(*json,"schema_fingerprint");
    link.baselineWidgetJson=stringValue(*json,"baseline_widget");link.unresolvedReason=stringValue(*json,"unresolved_reason");link.iconSvgText=stringValue(*json,"icon_svg");
    if(!textSafe(link.definitionId)||(!link.sourcePath.empty()&&!textSafe(link.sourcePath))||!revision||link.resolvedSchemaFingerprint.size()!=64 ||
        link.baselineWidgetJson.empty() || link.baselineWidgetJson.size()>8U*1024U*1024U) {
        errorText(error,"Type Definition link is incomplete or invalid.");return {};
    }
    const auto baseline=deserializeArrayElementWidget(link.baselineWidgetJson);
    if(!baseline || baseline->typeDefinition || !typeDefinitionSupported(*baseline)) {
        errorText(error,"Type Definition recovery snapshot is incomplete or invalid.");return {};
    }
    link.resolvedRevision=*revision;return link;
}
std::string serializeTypeDefinition(const TypeDefinitionDocument &document,std::string *error) {
    if(error)error->clear();if(!textSafe(document.id)||!textSafe(document.name)||!document.revision||!typeDefinitionSupported(document.root,error)) {
        if(error && error->empty())*error="Type Definition identity, name or revision is invalid.";return {};
    }
    auto root=document.root;root.typeDefinition.reset();const auto payload=source::parseJson(serializeArrayElementWidget(root));
    if(!payload){errorText(error,"Type Definition root cannot be serialized.");return {};}
    std::ostringstream out;out<<"{\n";stringField(out,"format",kTypeDefinitionFormat);out<<"  \"draft_revision\": 1,\n";
    stringField(out,"id",document.id);stringField(out,"name",document.name);out<<"  \"revision\": "<<document.revision<<",\n";
    stringField(out,"icon_svg",document.iconSvgText);out<<"  \"root\": ";document_internal::appendJsonValue(out,*payload);out<<"\n}\n";return out.str();
}
std::optional<TypeDefinitionDocument> deserializeTypeDefinition(std::string_view source,std::string *error) {
    if(error)error->clear();auto json=source::parseJson(source,error,source::JsonNumberPolicy::PreserveLexeme);
    if(!json || json->type!=JsonValue::Type::Object || stringValue(*json,"format")!=kTypeDefinitionFormat || revisionValue(*json,"draft_revision")!=1) {
        errorText(error,"Unsupported Type Definition document format or revision.");return {};
    }
    TypeDefinitionDocument document;document.id=stringValue(*json,"id");document.name=stringValue(*json,"name");document.iconSvgText=stringValue(*json,"icon_svg");
    const auto revision=revisionValue(*json,"revision");const auto *stored=json->member("root");
    const auto widget=stored?deserializeArrayElementWidget(jsonText(*stored)):std::nullopt;
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
    link.resolvedSchemaFingerprint=typeDefinitionSchemaFingerprint(definition.root);link.baselineWidgetJson=serializeArrayElementWidget(definition.root);
    link.iconSvgText=definition.iconSvgText;
    if(!result.accepted())link.unresolvedReason=result.diagnostics.front().message;
    result.proposedWidget.typeDefinition=std::move(link);result.changed=true;return result;
}
}
