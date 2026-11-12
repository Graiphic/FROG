#include "FrogDocumentArrayStorage.hpp"
#include "frog_native_engine/ContextPathIdentity.hpp"
#include "frog_native_engine/ApplicationControlSchema.hpp"
#include "frog_native_engine/ArrayCapacity.hpp"
#include "CanonicalComposedDraftMigration.hpp"
#include "frog_native_engine/CanonicalDraftPortResolution.hpp"
#include "frog_native_engine/graph/XYGraph.hpp"
#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/TimedLoopConfiguration.hpp"
#include "CanonicalDraftPresentation.hpp"
#include "frog_native_engine/EventNotificationContract.hpp"
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "CanonicalBooleanTree.hpp"
#include "CanonicalComparisonTree.hpp"
#include "frog_native_engine/CanonicalNumericFixedConstants.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "frog_native_engine/CanonicalNumericRing.hpp"
#include "frog_native_engine/CanonicalNumericArrayReduction.hpp"
#include "frog_native_engine/CanonicalNumericRandom.hpp"
#include "frog_native_engine/CanonicalNumericAccumulate.hpp"
#include "frog_native_engine/CanonicalNumericBooleanConversion.hpp"
#include "frog_native_engine/CanonicalNumericCalibration.hpp"
#include "frog_native_engine/CanonicalNumericBitReinterpretation.hpp"
#include "frog_native_engine/CanonicalNumericText.hpp"
#include "frog_native_engine/CanonicalNumericComplex.hpp"
#include "frog_native_engine/CanonicalScalePowerTwo.hpp"
#include "frog_native_engine/ExponentialFunctionIdentity.hpp"
#include "CanonicalPrivateInterfaceMap.hpp"
#include "FrogDocumentConditionalDisable.hpp"
#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/CanonicalNativeCallScalar.hpp"
#include "frog_native_engine/CanonicalInPlaceDescriptor.hpp"
#include "frog_native_engine/CanonicalCollectionDescriptor.hpp"
#include "frog_native_engine/CollectionAuthoringState.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/CanonicalComposedMathNative.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FittingNodeBinding.hpp"
#include "frog_native_engine/CalculusNodeBinding.hpp"
#include "frog_native_engine/StatisticsNodeBinding.hpp"
#include "frog_native_engine/InterpolationNodeBinding.hpp"
#include "frog_native_engine/GeometryNodeBinding.hpp"
#include "frog_native_engine/CanonicalStringNative.hpp"
#include "frog_native_engine/CanonicalPathNative.hpp"
#include "frog_native_engine/CanonicalTimestampNative.hpp"
#include "frog_native_engine/CanonicalVariantAnalysisNative.hpp"
#include "frog_native_engine/CanonicalFileNative.hpp"
#include "frog_native_engine/CanonicalErrorHandlingNative.hpp"
#include "frog_native_engine/ErrorPaletteBinding.hpp"
#include "frog_native_engine/CanonicalArrayNative.hpp"
#include "frog_native_engine/LinearAlgebraBinding.hpp"
#include "frog_native_engine/PolynomialBinding.hpp"
#include "frog_native_engine/BlasBinding.hpp"
#include "frog_native_engine/FittingBinding.hpp"
#include "frog_native_engine/CalculusBinding.hpp"
#include "frog_native_engine/CanonicalDifferentialNative.hpp"
#include "frog_native_engine/OptimizationBinding.hpp"
#include "frog_native_engine/CanonicalDiscreteArrayNative.hpp"
#include "frog_native_engine/StatisticsBinding.hpp"
#include "frog_native_engine/InterpolationBinding.hpp"
#include "frog_native_engine/GeometryBinding.hpp"
#include "frog_native_engine/DocumentTextDefaults.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include "frog_native_engine/NotifierFunctionBinding.hpp"
#include "frog_native_engine/QueueFunctionBinding.hpp"
#include "frog_native_engine/NotifierReference.hpp"
#include "frog_native_engine/SemaphoreFunctionBinding.hpp"
#include "frog_native_engine/RendezvousFunctionBinding.hpp"
#include "frog_native_engine/OccurrenceFunctionBinding.hpp"
#include "frog_native_engine/EventFunctionBinding.hpp"
#include "frog_native_engine/EventReference.hpp"
#include "frog_native_engine/OccurrenceReference.hpp"
#include "frog_native_engine/SemaphoreReference.hpp"
#include "frog_native_engine/OccurrenceReferenceLiteral.hpp"
#include "frog_native_engine/QueueReference.hpp"
#include "frog_native_engine/NotifierReference.hpp"
#include "frog_native_engine/RendezvousReference.hpp"
#include "frog_native_engine/BuildArrayBinding.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/SequenceStructure.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/TypeDefinitionLink.hpp"
#include "frog_native_engine/WidgetTypedValueSource.hpp"
#include "frog_native_engine/TimestampSourceProjection.hpp"
#include "frog_native_engine/CanonicalNumericScalarPalette.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog/math/formula_operation_contract.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <map>
#include <set>
#include <stdexcept>
#include <tuple>

namespace frog::native_engine {
namespace {
using namespace canonical_json;
using Domain=DocumentDiagnosticDomain;
DocumentDiagnosticDomain operationDomain(std::string_view type) {
    return findFunctionCatalogEntryByDiagramNodeType(type)||findFunctionCatalogEntryByPrimitiveType(type)?
        Domain::FrontendCapability:Domain::Compatibility;
}
DocumentDiagnosticDomain structureDomain(const Json &node) {
    const auto *type=node.member("type");
    return type&&type->type==Json::Type::String&&isDiagramStructureNodeType(type->string)?
        Domain::FrontendCapability:Domain::Compatibility;
}
struct Failure { CanonicalDraftMigrationDiagnostic diagnostic; };
[[noreturn]] void fail(std::string path,std::string message,
    std::string code,Domain domain) {
    throw Failure{{std::move(code),std::move(path),std::move(message),true,domain}};
}
const Json &objectAt(const Json *value,const std::string &path) {
    if(!value||value->type!=Json::Type::Object)fail(path,"An explicit object is required.", "migration.composed_source_invalid", Domain::Integrity);return *value;
}
const std::vector<Json> &arrayAt(const Json *value,const std::string &path) {
    if(!value||value->type!=Json::Type::Array)fail(path,"An explicit array is required.", "migration.composed_source_invalid", Domain::Integrity);return value->array;
}
const Json &widgetProperties(const Json &widget,const std::string &path) {
    // The current writer omits props when a widget has no property override.
    // Preserve strict validation for an explicitly present, malformed object.
    if(const auto *props=widget.member("props"))return objectAt(props,path+"/props");
    static const Json defaults=object();
    return defaults;
}
std::string text(const Json *value,const std::string &path,bool empty=false) {
    if(!value||value->type!=Json::Type::String||(!empty&&value->string.empty()))fail(path,"An explicit string is required.", "migration.composed_source_invalid", Domain::Integrity);return value->string;
}
void fields(const Json &value,std::initializer_list<std::string_view> names,const std::string &path) {
    objectAt(&value,path);
    for(const auto &[key,_]:value.object)if(std::find(names.begin(),names.end(),key)==names.end())
        fail(pointer(path,key),"This field has no composed-program adaptation contract.","migration.composed_field_unavailable", Domain::Compatibility);
}

void authoredRootFields(const Json &root) {
    fields(root,{"format","draft_revision","language","title","metadata","interface","interface_map","frontPanel","diagram","host","execution_policy","text_defaults","ide","native_options","icon","conditional_compilation"},"/");
    // The common reader and Conditional specialization already consume these
    // exact project/target symbols. Recognize the same authored field here,
    // retaining its strict schema instead of rejecting or erasing it.
    FrogSourceProperties properties;
    if(const auto error=document_internal::readCompilationSymbols(root,properties);!error.empty())
        fail("/conditional_compilation",error,"migration.conditional_symbols_invalid",Domain::Semantics);
}
void authoredWidgetFields(const Json &widget,const std::string &at) { fields(widget,{"id","classId","role","valueType","label","bounds","props","runtime","state","typography","geometry","binding","studio_application_control","studio_tab","studio_tab_parent_id","studio_tab_page_id","hidden","locked"},at); }
// A writer revision is only a format declaration. Route the actual supported
// Reference widget configuration, and reuse the same exact check when the
// composed adapter receives its hash-bound widget/schema witness.
bool configuredTypedNullResourceWidget(const Json &widget,const binding::TypeRef &type,
    std::string_view path) {
    const auto *klass=widget.member("classId"),*declared=widget.member("valueType");
    if(!klass||klass->type!=Json::Type::String||klass->string!="frog.studio.widgets.reference"||
       !declared||declared->type!=Json::Type::String||declared->string!=type.canonical()||
       !(isOccurrenceReference(type)||isSemaphoreReference(type)||isRendezvousReference(type)||isQueueReference(type)||isNotifierReference(type)))return false;
    const auto *configuration=widget.member("studio_application_control");
    if(!configuration)return false;
    const auto value=application_control_source::readConfiguration(*configuration);
    const auto descriptor=canonicalObjectReferenceDescriptor(*configuration,
        std::string(path)+"/studio_application_control",type);
    return value&&descriptor&&descriptor->kind==CanonicalObjectReferenceKind::Resolve&&
        descriptor->effects==std::vector<std::string>{"transport_typed_null"}&&
        exact(*configuration,application_control_source::writeConfiguration(*value));
}
void authoredRegionFields(const Json &region,const std::string &rp) {
    fields(region,{"id","title","match","matches","default","draft","draft_text","diagram","event","event_label","subdiagram_label_text",
                        "event_data_fields","event_payload_fields","event_response_fields","studio_event_vkey_encoding","studio_event_data_node_visible","studio_event_data_node_normalized_y",
                        "studio_event_response_node_visible","studio_event_response_node_normalized_y","studio_event_selected_fields_version",
                        "studio_title","studio_width","studio_subdiagram_label_text"},rp);
    if(const auto *encoding=region.member("studio_event_vkey_encoding"))
        if(encoding->type!=Json::Type::String || encoding->string!="enum-v1")
            fail(rp+"/studio_event_vkey_encoding","Unknown VKey encoding; native codes cannot be guessed as enum ordinals.","migration.event_vkey_encoding_unsupported",Domain::Compatibility);
    if(const auto *event=region.member("event")) {
        const auto at=rp+"/event";fields(*event,{"kind","source","studio_bindings"},at);
        const auto kind=text(event->member("kind"),at+"/kind");
        if(kind!="user"&&kind!="timeout"&&!eventNotificationSpec(kind))
            fail(at+"/kind","The Event descriptor kind has no source contract.","migration.event_kind_unavailable",Domain::Compatibility);
        if(const auto *bindings=event->member("studio_bindings")) {
            const auto &entries=arrayAt(bindings,at+"/studio_bindings");
            for(std::size_t i=0;i<entries.size();++i) {
                const auto path=at+"/studio_bindings/"+std::to_string(i);
                fields(entries[i],{"kind","source","source_scope"},path);
                if(!eventNotificationSpec(text(entries[i].member("kind"),path+"/kind")))
                    fail(path+"/kind","The Event binding kind has no source contract.","migration.event_kind_unavailable",Domain::Compatibility);
            }
        }
    }
}
bool flag(const Json *value,const std::string &path) {
    if(!value||value->type!=Json::Type::Boolean)fail(path,"An explicit Boolean is required.", "migration.composed_source_invalid", Domain::Integrity);return value->boolean;
}
// The composed owned-value corridor already transports these physical widths.
// Keep their admission local; the explicit public scalar contracts are not
// widened. Parse F32 directly so its rounding never passes through double.
std::optional<Json> composedComplexLexeme(std::string_view lexical,std::string_view type) {
    const auto parts=numeric_value::parseComplexNumber(lexical,type);if(!parts)return {};
    const auto component=[&](double value)->std::optional<Json> {
        char buffer[64];const auto written=type=="c32"?
            std::to_chars(buffer,buffer+sizeof(buffer),static_cast<float>(value),std::chars_format::general,std::numeric_limits<float>::max_digits10):
            std::to_chars(buffer,buffer+sizeof(buffer),value,std::chars_format::general,std::numeric_limits<double>::max_digits10);
        if(written.ec!=std::errc{})return {};Json result;result.type=Json::Type::Number;result.string.assign(buffer,written.ptr);return result;
    };
    const auto real=component(parts->real),imaginary=component(parts->imaginary);if(!real||!imaginary)return {};
    auto result=object();result.object={{"real",*real},{"imaginary",*imaginary}};return result;
}
std::optional<Json> composedNumericValue(const Json &value,std::string_view type) {
    if(type=="timestamp") {
        if(value.type!=Json::Type::Number)return {};
        // The real current widget/constant stores binary64 seconds. Project
        // precisely that value, without claiming absent64-bit fraction state.
        return timestamp_source::lexical(value.string);
    }
    if(type=="c32"||type=="c64") {
        if(value.type!=Json::Type::Object||value.object.size()!=2)return {};
        const auto *real=value.member("real"),*imaginary=value.member("imaginary");
        const auto component=type=="c32"?"f32":"f64";
        if(!real||!imaginary||!composedNumericValue(*real,component)||!composedNumericValue(*imaginary,component))return {};
        return value;
    }
    if(type=="f32") {
        if(value.type!=Json::Type::Number)return {};
        float number=0;const auto parsed=std::from_chars(value.string.data(),value.string.data()+value.string.size(),number);
        if(parsed.ec!=std::errc{}||parsed.ptr!=value.string.data()+value.string.size()||!std::isfinite(number))return {};
        return value;
    }
    const bool signedNarrow=type=="i4"||type=="i8"||type=="i16",unsignedNarrow=type=="u4"||type=="u8"||type=="u16";
    if(signedNarrow||unsignedNarrow) {
        if(value.type!=Json::Type::Number)return {};
        const auto lexical=numeric_value::canonicalWideIntegerLexeme(value.string,signedNarrow?"i64":"u64");
        if(!lexical)return {};
        if(signedNarrow) {
            std::int64_t number=0;const auto parsed=std::from_chars(lexical->data(),lexical->data()+lexical->size(),number);
            const std::int64_t low=type=="i4"?-8:type=="i8"?-128:-32768,high=type=="i4"?7:type=="i8"?127:32767;
            if(parsed.ec!=std::errc{}||parsed.ptr!=lexical->data()+lexical->size()||number<low||number>high)return {};
        }else {
            std::uint64_t number=0;const auto parsed=std::from_chars(lexical->data(),lexical->data()+lexical->size(),number);
            const std::uint64_t high=type=="u4"?15:type=="u8"?255:65535;
            if(parsed.ec!=std::errc{}||parsed.ptr!=lexical->data()+lexical->size()||number>high)return {};
        }
        return source::parseJson(*lexical,nullptr,source::JsonNumberPolicy::PreserveLexeme);
    }
    if(typedScalar(value,type))return value;
    return {};
}
std::uint32_t ordinal(const Json *value,const std::string &path) {
    std::uint32_t result{};
    if(!value||value->type!=Json::Type::Number)fail(path,"An exact unsigned ordinal is required.", "migration.composed_source_invalid", Domain::Integrity);
    const auto r=std::from_chars(value->string.data(),value->string.data()+value->string.size(),result);
    if(r.ec!=std::errc{}||r.ptr!=value->string.data()+value->string.size())fail(path,"An exact unsigned ordinal is required.", "migration.composed_source_invalid", Domain::Semantics);return result;
}
Json number(std::uint64_t value) { Json result;result.type=Json::Type::Number;result.string=std::to_string(value);return result; }
std::string bare(std::string_view value) { if(value.starts_with("frog.studio."))value.remove_prefix(12);return std::string(value); }
bool recordLiteral(const Json &node) {
    const auto *raw=node.member("type");if(!raw||raw->type!=Json::Type::String)return false;
    const auto kind=bare(raw->string);return kind=="cluster.constant"||kind=="error.constant";
}
std::optional<double> fixedNumericConstant(const Json &node) {
    const auto *type=node.member("type");if(!type||type->type!=Json::Type::String)return {};
    const auto *entry=findFunctionCatalogEntry(bare(type->string));
    return entry?functionFixedNumericConstantValue(*entry):std::nullopt;
}
std::optional<std::string_view> fixedStringConstant(const Json &node) {
    const auto *kind=node.member("kind"),*type=node.member("type");
    if(!kind||kind->type!=Json::Type::String||kind->string!="primitive"||!type||type->type!=Json::Type::String)return {};
    const auto *entry=findFunctionCatalogEntry(bare(type->string));
    return entry?functionFixedStringConstantValue(*entry):std::nullopt;
}
bool scalarLiteral(const Json &node) {
    if(fixedStringConstant(node))return true;
    if(const auto fixed=fixedNumericConstant(node))return std::isfinite(*fixed);
    const auto *raw=node.member("type");if(!raw||raw->type!=Json::Type::String)return false;
    const auto id=bare(raw->string);
    return id=="numeric.numeric_constant"||id=="numeric.enum_constant"||id=="numeric.ring_constant"||id=="boolean.true_constant"||
        id=="boolean.false_constant"||id=="string.constant"||id=="path.constant"||id=="timestamp.constant"||id=="variant.constant";
}
bool arrayLiteral(const Json &node) {
    const auto *raw=node.member("type");if(!raw||raw->type!=Json::Type::String)return false;
    const auto id=bare(raw->string);
    return id=="numeric.numeric_array_constant"||id=="array.array_constant"||id=="numeric.enum_array_constant"||
        id=="numeric.ring_array_constant"||id=="string.string_array_constant"||id=="path.path_array_constant"||
        id.starts_with("array.other_constants.");
}
bool collectionLiteral(const Json &node) {
    const auto *raw=node.member("type");if(!raw||raw->type!=Json::Type::String)return false;
    const auto id=bare(raw->string);return id=="collection.map.constant"||id=="collection.set.constant";
}
CollectionAuthoringState collectionWidgetState(const Json &widget,const std::string &path) {
    const auto &props=objectAt(widget.member("props"),path+"/props");
    const auto *raw=props.member("collection.state");std::optional<Json> decoded;
    if(raw&&raw->type==Json::Type::String){decoded=source::parseJson(raw->string,nullptr,source::JsonNumberPolicy::PreserveLexeme);raw=decoded?&*decoded:nullptr;}
    std::string why;const auto state=raw?readCollectionAuthoringState(*raw,&why):std::nullopt;
    if(!state||!collectionValue(*state))fail(path,"A Collection widget requires its exact complete authoring state: "+why, "migration.composed_source_invalid", Domain::Integrity);
    return *state;
}
bool presentation(std::string_view key) {
    return key.starts_with("label")||key=="layout"||key=="font"||key=="hidden"||key=="locked"||key=="group"||
        key=="palette_catalog_id"||key=="display_format"||key.starts_with("while_loop_")||key.starts_with("for_loop_")||
        key.starts_with("case_structure_")||key.starts_with("event_structure_")||key.starts_with("structure_subdiagram_")||
        key=="structure_auto_grow"||key=="structure_subdiagram_label_border_rgb";
}
bool requiresScope(const Json &scope,unsigned depth) {
    if(depth>32)return true;
    const auto *nodes=scope.member("nodes");if(!nodes||nodes->type!=Json::Type::Array)return false;
    for(const auto &node:nodes->array) {
        // A dedicated model source owns an immutable call contract even when
        // it is not yet wired to a fitting operation. The composed pass, not
        // the legacy scalar corridor, validates and preserves that contract.
        if(const auto *kind=node.member("kind");kind&&kind->type==Json::Type::String&&(kind->string=="fitting_model_reference"||kind->string=="calculus_function_reference"))return true;
        // Fixed String presets are real zero-input literal producers. Route
        // their closed catalog identities before the configured scalar corridor.
        if(fixedStringConstant(node))return true;
        // Array constants use the existing composed codec even without an Array operation.
        if(arrayLiteral(node))return true;
        if(const auto *type=node.member("type");type&&type->type==Json::Type::String&&bare(type->string)=="variant.constant")return true;
        if(scalarLiteral(node))if(const auto *t=node.member("value_type");t&&t->type==Json::Type::String&&(t->string=="c32"||t->string=="c64"))return true;
        if(const auto *t=node.member("type");t&&t->type==Json::Type::String) {
            const auto id=bare(t->string);
            // A standalone Path already has an exact composed literal codec.
            // Owned Path fields retain their established immutable-Record
            // corridor; do not give the classic String2 profile a Path alias.
            if(id=="path.constant" && !node.member("constant_owner_node_id")) {
                const auto *kind=node.member("kind");
                if(kind && kind->type==Json::Type::String && kind->string=="primitive")return true;
            }
            if(const auto *comparison=comparison_source::definition(t->string)) {
                // New predicates and Compare Aggregates need this profile.
                // Explicit Compare Elements=false retains the scalar corridor.
                const auto *mode=node.member("comparison_aggregates");
                if(!comparison_source::hasLegacyCorridor(t->string)||
                    (mode&&(mode->type!=Json::Type::Boolean||mode->boolean)))return true;
            }
            if(!canonicalVariantAnalysisMigrationIdentity(id).empty())return true;
            if(!canonicalNumericIeeeConstantMigrationIdentity(id).empty())return true;
            if(!canonicalNumericRandomMigrationIdentity(id).empty())return true;
            if(!canonicalNumericAccumulateMigrationIdentity(id).empty())return true;
            if(!canonicalNumericArrayReductionMigrationIdentity(id).empty())return true;
            if(!canonicalNumericBooleanConversionMigrationIdentity(id).empty())return true;
            // New Array reductions need the composed service. Existing plain
            // bool programs keep their scalar Source/LLVM/AOT profile.
            if(const auto *logical=boolean_source::definition(t->string);logical&&boolean_source::contracts::reduction(logical->kind))return true;
            if(!canonicalNumericCalibrationMigrationIdentity(id).empty())return true;
            if(!canonicalNumericBitReinterpretationMigrationIdentity(id).empty())return true;
            if(!canonicalNumericTextMigrationIdentity(id).empty())return true;
            if(id.starts_with("numeric.complex.")||id=="numeric.conversion.to_csg"||id=="numeric.conversion.to_cdb")return true;
            // Keep the established scalar profile for its already supported
            // exact types. New operations route here immediately; mixed and
            // narrow variants of established operations use resolver evidence.
            if(!canonicalNumericScalarPaletteMigrationIdentity(id).empty()&&
               canonicalScalarMigrationIdentity(t->string).empty()&&
               !canonicalScalarSelectionAuthoredContract(t->string))return true;
            if(!canonicalNumericConversionTarget(canonicalNumericAggregateMigrationIdentity(id)).empty())return true;
            if(id=="error.constant"||!canonicalErrorHandlingMigrationIdentity(id).empty()||
               !canonicalVariantPrimitiveMigrationIdentity(id).empty()||id=="cluster.bundle"||id=="cluster.bundle_by_name"||id=="cluster.unbundle"||id=="string.search_replace")return true;
            if(!canonicalDiscreteArrayMigrationIdentity(id).empty()||isOrdinaryDifferentialFunction(id)||isPartialDifferentialFunction(id)||isGeometryFunction(id)||isInterpolationFunction(id)||isStatisticsFunction(id)||isCalculusFunction(id)||isFittingFunction(id)||isBlasFunction(id)||isPolynomialFunction(id)||isLinearAlgebraFunction(id)||!canonicalScalePowerTwoMigrationIdentity(id).empty()||(id!="array.index"&&!canonicalArrayValueMigrationIdentity(id).empty())||!canonicalStringValueMigrationIdentity(id).empty()||!canonicalPathValueMigrationIdentity(id).empty()||!canonicalFileMigrationIdentity(id).empty()||!canonicalTimestampMigrationIdentity(id).empty()||(id.starts_with("math.formula.")&&frog::math::formula::operation(id))||id=="structures.feedback_node"||id=="synchronization.first_call"||id.starts_with("synchronization.queue.")||isNotifierFunction(id)||isSemaphoreFunction(id)||isRendezvousFunction(id)||isOccurrenceFunction(id)||isEventFunction(id)||id.starts_with("collection.")||id=="numeric.enum_constant"||
                id.starts_with("map<")||id.starts_with("set<"))return true;
        }
        if(const auto *kind=node.member("structure_type");kind&&kind->type==Json::Type::String) {
            if(kind->string=="timed_loop"||kind->string=="event_structure"||kind->string=="in_place_element_structure"||kind->string=="sequence_structure")return true;
            if(const auto *boundary=node.member("boundary");boundary&&boundary->type==Json::Type::Object)
                // Even scalar shift registers have real state boundaries.
                // The classic adapter rejects every nonempty boundary; the
                // composed contract already preserves their typed histories.
                for(const auto side:{"inputs","outputs"})if(const auto *entries=boundary->member(side);entries&&entries->type==Json::Type::Array&&!entries->array.empty())return true;
            if(kind->string=="case")if(const auto *terminals=node.member("structure_terminals"))if(const auto *selector=terminals->member("selector"))
                if(const auto *type=selector->member("type");type&&type->type==Json::Type::String&&type->string!="bool"&&type->string!="i64"&&type->string!="u64")return true;
        }
        if(const auto *regions=node.member("regions");regions&&regions->type==Json::Type::Array)
            for(const auto &region:regions->array)if(const auto *body=region.member("diagram");body&&requiresScope(*body,depth+1))return true;
    }
    return false;
}
struct Literal {binding::TypeRef type;Json value;std::vector<std::string> components;};
struct Node {const Json *raw{};std::string path,scope;};
struct Edge {const Json *raw{};std::string path,scope;};
struct Port {
    binding::TypeRef type;Json schema;
    std::set<std::pair<std::string,std::string>> sources;
};
using PortKey=std::tuple<std::string,std::string,bool>;

struct Adapter {
    CanonicalDraftMigrationResult &out;const Json &root;const CanonicalDraftResolvedTypes &resolved;
    std::map<std::string,Node> nodes;
    std::vector<Edge> edges;
    std::map<PortKey,Port> ports;
    std::map<std::string,std::string> scopeOwners;
    std::set<std::string> generatedStatefulDefaultNodes,generatedStatefulDefaultEdges;
    std::map<std::string,std::vector<std::string>> children;
    std::map<std::string,Literal> literals;
    std::set<std::string> activeLiteral,edgeIds,widgetIds,usedPortNodes,sourceNodeIds;
    std::map<std::string,binding::TypeRef> widgetTypes;

    bool hasNativeCalls() const {
        return std::any_of(nodes.begin(),nodes.end(),[](const auto &entry){
            const auto *type=entry.second.raw->member("type");
            return type&&type->type==Json::Type::String&&type->string==kNativeCallDiagramNodeType;
        });
    }

    bool relocatedInitializer(const Edge &edge,const std::string &id,const std::string &portId,bool input) const {
        if(!input||portId!="initial_value")return false;
        const auto found=nodes.find(id);if(found==nodes.end())return false;
        const auto &raw=*found->second.raw;const auto *type=raw.member("type"),*mode=raw.member("feedback_initialization"),*loop=raw.member("feedback_initializer_loop_id");
        if(!type||bare(type->string)!="structures.feedback_node"||!mode||mode->string!="loop_entry"||!loop||loop->string.empty())return false;
        const auto owner=nodes.find(loop->string);if(owner==nodes.end()||owner->second.scope!=edge.scope)return false;
        const auto *kind=owner->second.raw->member("structure_type");
        if(!kind||(kind->string!="while_loop"&&kind->string!="for_loop"))return false;
        auto scope=found->second.scope;
        while(scopeOwners.contains(scope)&&!scopeOwners.at(scope).empty()) {
            const auto &ancestor=scopeOwners.at(scope);if(ancestor==loop->string)return true;
            const auto parent=nodes.find(ancestor);if(parent==nodes.end())break;scope=parent->second.scope;
        }
        return false;
    }

    void index(const Json &scope,const std::string &path,std::string owner={},unsigned depth=0) {
        if(depth>32)fail(path,"Explicit subdiagram nesting exceeds 32.","migration.composed_resource_limit", Domain::Control);
        fields(scope,{"nodes","edges","wire_fragments","zoom_percent","viewport_origin"},path);
        if(const auto *broken=scope.member("wire_fragments");broken&&!arrayAt(broken,path+"/wire_fragments").empty())
            fail(path+"/wire_fragments","Incomplete wires cannot be prepared.", "migration.composed_source_invalid", Domain::Semantics);
        scopeOwners.emplace(path,std::move(owner));
        const auto &ns=arrayAt(scope.member("nodes"),path+"/nodes");
        if(ns.size()>1024-sourceNodeIds.size())fail(path,"The existing 1024-node budget was exceeded.","migration.composed_resource_limit", Domain::Control);
        for(std::size_t i=0;i<ns.size();++i) {
            const auto at=path+"/nodes/"+std::to_string(i);const auto &n=objectAt(&ns[i],at);
            const auto id=text(n.member("id"),at+"/id");
            if(sourceNodeIds.size()>=1024||id.size()>1024||id.find('\0')!=id.npos||!sourceNodeIds.insert(id).second)fail(at,"Node identities must remain distinct and bounded.", "migration.composed_source_invalid", Domain::Integrity);
            if(text(n.member("kind"),at+"/kind")=="comment") {
                draft_presentation::comment(n,scope,at,[](auto path,auto message){
                    fail(std::move(path),std::move(message),"migration.presentation_invalid", Domain::Compatibility);
                });
                out.identities.push_back({"presentation",id,at,std::string(draft_presentation::provenance)+at,false});
                continue;
            }
            nodes.emplace(id,Node{&n,at,path});
            if(const auto *ownerId=n.member("constant_owner_node_id"))children[text(ownerId,at+"/constant_owner_node_id")].push_back(id);
            if(const auto *regions=n.member("regions")) {
                const auto &entries=arrayAt(regions,at+"/regions");std::set<std::string> ids;
                for(std::size_t r=0;r<entries.size();++r) {
                    const auto rp=at+"/regions/"+std::to_string(r);const auto rid=text(entries[r].member("id"),rp+"/id");
                    if(!ids.insert(rid).second)fail(rp,"Region IDs are unique within their explicit owner.", "migration.composed_source_invalid", Domain::Integrity);
                    index(objectAt(entries[r].member("diagram"),rp+"/diagram"),rp+"/diagram",id,depth+1);
                }
            }
        }
        const auto &es=arrayAt(scope.member("edges"),path+"/edges");
        if(es.size()>4096-edges.size())fail(path,"The existing 4096-edge budget was exceeded.","migration.composed_resource_limit", Domain::Control);
        for(std::size_t i=0;i<es.size();++i) {
            const auto at=path+"/edges/"+std::to_string(i);fields(es[i],{"id","from","to","bends","label"},at);
            if(const auto *label=es[i].member("label"))
                draft_presentation::wireLabel(*label,at+"/label",[](auto path,auto message){
                    fail(std::move(path),std::move(message),"migration.presentation_invalid", Domain::Compatibility);
                });
            if(!edgeIds.insert(text(es[i].member("id"),at+"/id")).second)fail(at,"Edge identities must remain distinct.", "migration.composed_source_invalid", Domain::Integrity);
            edges.push_back({&es[i],at,path});
        }
    }
    std::string inputIdentity(const std::string &id,std::string_view portId) const {
        const auto found=nodes.find(id);
        const auto *type=found==nodes.end()?nullptr:found->second.raw->member("type");
        if(portId=="input_2"&&type&&type->type==Json::Type::String&&bare(type->string)=="array.build")
            return "array_b";
        return std::string(portId);
    }
    void resolutions() {
        if(resolved.originalSha256!=out.originalSha256)fail("/","Connector evidence belongs to different source bytes.","migration.composed_resolution_stale", Domain::Integrity);
        if(resolved.ports.size()>32768)fail("/diagram","The composed profile requires bounded complete authored connector evidence.","migration.composed_resolution_required", Domain::Semantics);
        std::size_t descriptorBytes=0;
        for(const auto &entry:resolved.ports) {
            const auto node=nodes.find(entry.nodeId);
            if(node==nodes.end()||entry.portId.empty()||entry.portId.size()>1024||entry.fullTypeJson.size()>65536)
                fail("/diagram","Connector evidence names no actual bounded authored node/port.","migration.composed_resolution_invalid", Domain::Integrity);
            if(entry.fullTypeJson.size()>16U*1024U*1024U-descriptorBytes)
                fail("/diagram","Complete connector schemas exceed the existing aggregate descriptor budget.","migration.composed_resource_limit", Domain::Control);
            descriptorBytes+=entry.fullTypeJson.size();
            const auto full=source::parseJson(entry.fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            CanonicalObjectReferenceDiagnostic diagnostic;
            const auto type=full?canonicalObjectReferenceReadType(*full,&diagnostic):std::nullopt;
            if(!type||!type->known())fail(node->second.path,"A complete exact type schema is required for every connector.","migration.composed_resolution_invalid", Domain::Integrity);
            const auto resolvedPort=entry.input?inputIdentity(entry.nodeId,entry.portId):entry.portId;
            auto [found,inserted]=ports.try_emplace({entry.nodeId,resolvedPort,entry.input},Port{*type,*full,{}});
            if(!inserted&&!canonicalObjectReferenceFullTypeEqual(found->second.type,*type))
                fail(node->second.path,"Repeated scoped connector evidence disagrees about the complete type.","migration.composed_resolution_invalid", Domain::Integrity);
            if(entry.sourceNodeId.empty()!=entry.sourcePortId.empty()||(!entry.input&&(!entry.sourceNodeId.empty()||!entry.sourcePortId.empty())))
                fail(node->second.path,"Only input evidence can name one complete real driver endpoint.","migration.composed_resolution_invalid", Domain::Integrity);
            if(!entry.sourceNodeId.empty()) {
                const bool actual=std::any_of(edges.begin(),edges.end(),[&](const Edge &e){
                    const auto *from=e.raw->member("from"),*to=e.raw->member("to");
                    return from&&to&&text(from->member("node"),e.path)==entry.sourceNodeId&&text(from->member("port"),e.path)==entry.sourcePortId&&
                        text(to->member("node"),e.path)==entry.nodeId&&inputIdentity(entry.nodeId,text(to->member("port"),e.path))==resolvedPort;});
                if(!actual)fail(node->second.path,"Connector evidence invented a source endpoint.","migration.composed_resolution_invalid", Domain::Integrity);
                if(!found->second.sources.emplace(entry.sourceNodeId,entry.sourcePortId).second)
                    fail(node->second.path,"Duplicate connector evidence is not a second driver.","migration.composed_resolution_invalid", Domain::Integrity);
            } else if(!inserted)
                fail(node->second.path,"Duplicate connector declarations need distinct actual regional drivers.","migration.composed_resolution_invalid", Domain::Integrity);
        }
        for(const auto &edge:edges) {
            const auto &from=objectAt(edge.raw->member("from"),edge.path+"/from"),&to=objectAt(edge.raw->member("to"),edge.path+"/to");
            for(const auto side:{"from","to"}) {
                const bool input=std::string_view(side)=="to";const auto &endpoint=input?to:from;
                fields(endpoint,{"node","port"},edge.path+"/"+side);
                const auto id=text(endpoint.member("node"),edge.path),port=text(endpoint.member("port"),edge.path);
                const auto node=nodes.find(id);
                if(node==nodes.end()||(node->second.scope!=edge.scope&&scopeOwners.at(edge.scope)!=id&&!relocatedInitializer(edge,id,port,input))||node->second.raw->member("constant_owner_node_id"))
                    fail(edge.path,"A real wire must stay within its explicit owner/region and cannot wire an owned literal component.", "migration.composed_source_invalid", Domain::Semantics);
                const auto resolved=ports.find({id,input?inputIdentity(id,port):port,input});
                if(resolved==ports.end())fail(edge.path,"A wired connector has no complete authored type/direction evidence.","migration.composed_resolution_required", Domain::Semantics);
                if(input&&!resolved->second.sources.contains({text(from.member("node"),edge.path),text(from.member("port"),edge.path)}))
                    fail(edge.path,"Input evidence did not retain its actual source endpoint.","migration.composed_resolution_required", Domain::Semantics);
            }
        }
        for(const auto &[owner,parts]:children) {
            const auto parent=nodes.find(owner);
            if(parent==nodes.end()||!recordLiteral(*parent->second.raw))fail("/diagram","Owned literal fields need their actual Cluster owner.", "migration.composed_source_invalid", Domain::Semantics);
            for(const auto &part:parts)if(nodes.at(part).scope!=parent->second.scope)fail(nodes.at(part).path,"Literal ownership cannot cross explicit subdiagrams.", "migration.composed_source_invalid", Domain::Semantics);
        }
        if(resolved.widgets.size()>1024)fail("/frontPanel","Widget evidence exceeds the existing widget budget.","migration.composed_resource_limit", Domain::Control);
        std::set<std::string> ids;
        for(const auto &widget:resolved.widgets)if(widget.widgetId.empty()||!ids.insert(widget.widgetId).second)
            fail("/frontPanel","Widget evidence needs distinct actual identities.","migration.composed_resolution_invalid", Domain::Integrity);
    }
    const Port &port(const std::string &id,std::string_view name,bool input) const {
        auto found=ports.find({id,std::string(name),input});
        if(found==ports.end()&&input&&name=="array_b") {
            const auto *type=nodes.at(id).raw->member("type");
            if(type&&type->type==Json::Type::String&&bare(type->string)=="array.build")
                found=ports.find({id,"input_2",true});
        }
        if(found==ports.end())fail(nodes.at(id).path,"The actual semantic connector needs complete type evidence: "+std::string(name),"migration.composed_resolution_required", Domain::Semantics);return found->second;
    }
    Json portList(const std::string &id) {
        Json result=array();
        for(const auto &[key,value]:ports)if(std::get<0>(key)==id) {
            Json entry=object();entry.object={{"id",string(std::get<1>(key))},{"direction",string(std::get<2>(key)?"input":"output")},
                {"type",string(canonicalObjectReferencePhysicalType(value.type).canonical())},{"studio_type_schema",value.schema}};
            result.array.push_back(std::move(entry));
        }
        const auto *authored=nodes.at(id).raw->member("type");
        if(authored&&authored->type==Json::Type::String&&isRetainFirstError(authored->string))
            std::stable_sort(result.array.begin(),result.array.end(),[](const Json &a,const Json &b){
                const bool ai=a.member("direction")->string=="input",bi=b.member("direction")->string=="input";
                if(ai!=bi)return ai;
                return errorRetainIndex(a.member("id")->string).value_or(16)<errorRetainIndex(b.member("id")->string).value_or(16);
            });
        if(authored&&authored->type==Json::Type::String&&bare(authored->string)=="array.build") {
            for(auto &entry:result.array)if(entry.member("direction")->string=="input"&&entry.member("id")->string=="input_2")
                entry.object["id"]=string("array_b");
            std::stable_sort(result.array.begin(),result.array.end(),[](const Json &a,const Json &b){
                const bool ai=a.member("direction")->string=="input",bi=b.member("direction")->string=="input";if(ai!=bi)return ai;
                return buildArrayInputIndex(a.member("id")->string).value_or(32)<buildArrayInputIndex(b.member("id")->string).value_or(32);
            });
        }
        if(authored&&authored->type==Json::Type::String&&
           !canonicalNumericConversionTarget(canonicalNumericAggregateMigrationIdentity(authored->string)).empty())
            std::stable_sort(result.array.begin(),result.array.end(),[](const Json &a,const Json &b){
                return a.member("direction")->string=="input"&&b.member("direction")->string!="input";
            });
        if(authored&&authored->type==Json::Type::String&&!canonicalNumericBooleanConversionMigrationIdentity(authored->string).empty())
            for(auto &entry:result.array)entry.object["id"]=string(std::string(canonicalNumericBooleanConnectorId(authored->string,entry.member("id")->string)));
        const auto *contract=authored&&authored->type==Json::Type::String?canonicalScalarNativeContract(canonicalScalarMigrationIdentity(authored->string)):nullptr;
        if(authored&&authored->type==Json::Type::String&&!canonicalBooleanTreeMigrationIdentity(authored->string).empty())
            std::stable_sort(result.array.begin(),result.array.end(),[](const Json &a,const Json &b){
                const bool ai=a.member("direction")->string=="input",bi=b.member("direction")->string=="input";if(ai!=bi)return ai;
                return canonicalBooleanInputOrdinal(a.member("id")->string).value_or(0)<canonicalBooleanInputOrdinal(b.member("id")->string).value_or(0);
            });
        if(contract&&contract->catalogId=="numeric.add")std::stable_sort(result.array.begin(),result.array.end(),[](const Json &a,const Json &b){
            const bool ai=a.member("direction")->string=="input",bi=b.member("direction")->string=="input";if(ai!=bi)return ai;
            return canonicalNumericAddInputOrdinal(a.member("id")->string).value_or(16)<canonicalNumericAddInputOrdinal(b.member("id")->string).value_or(16);
        });
        if(authored&&authored->type==Json::Type::String&&!canonicalScalePowerTwoMigrationIdentity(authored->string).empty())
            std::stable_sort(result.array.begin(),result.array.end(),[](const Json &a,const Json &b){
                // Provider order is semantic x,n,result, not map order n,x.
                const auto rank=[](const Json &port){const auto &id=port.member("id")->string;return id=="x"?0:id=="n"?1:id=="result"?2:3;};
                return rank(a)<rank(b);
            });
        if(const auto *exponential=authored&&authored->type==Json::Type::String?canonicalExponentialContract(authored->string):nullptr)
            std::stable_sort(result.array.begin(),result.array.end(),[&](const Json &a,const Json &b){
                // Use the existing provider's semantic ABI: y,x,result for
                // binary operations and x,result for unary operations.
                const auto rank=[&](const Json &port){const auto &id=port.member("id")->string;
                    return exponential->binary?(id=="y"?0:id=="x"?1:id=="result"?2:3):(id=="x"?0:id=="result"?1:2);};
                return rank(a)<rank(b);
            });
        if(authored&&authored->type==Json::Type::String&&!canonicalComposedMathMigrationIdentity(authored->string).empty()) {
            if(const auto *definition=findFunctionBindingDefinition(bare(authored->string)))
                std::stable_sort(result.array.begin(),result.array.end(),[&](const Json &a,const Json &b){
                    // Scalar providers consume the contract order, not lexical
                    // map order (atan2 y,x; sincos sin,cos).
                    const auto rank=[&](const Json &value){
                        const bool input=value.member("direction")->string=="input";
                        for(std::size_t i=0;i<definition->ports.size();++i) {
                            const auto &candidate=definition->ports[i];
                            if(candidate.portId==value.member("id")->string&&
                               (candidate.direction==FunctionPortDirection::Input)==input)
                                return i+(input?0:definition->ports.size());
                        }
                        return 2*definition->ports.size();
                    };
                    return rank(a)<rank(b);
                });
        }
        usedPortNodes.insert(id);return result;
    }
    binding::TypeRef enumType(const Json &node,const std::string &at) {
        const auto &configuration=objectAt(node.member("enum_constant"),at+"/enum_constant");
        fields(configuration,{"domain_id","selected_value","text_overflow_visible","digital_display_visible","items"},at+"/enum_constant");
        const auto repr=text(node.member("value_type"),at+"/value_type");const auto representation=binding::parseType(repr);
        if(!representation.valid()||!representation.type.integer()||representation.type.name.starts_with("i"))fail(at,"Enum representation must retain its actual unsigned integer width.", "migration.composed_source_invalid", Domain::Semantics);
        auto result=binding::TypeRef::enumeration(text(configuration.member("domain_id"),at+"/enum_constant/domain_id"),representation.type);
        result.choiceKind=binding::ChoiceKind::Enum;std::set<std::string> ids;std::set<std::uint32_t> values;
        const auto &items=arrayAt(configuration.member("items"),at+"/enum_constant/items");
        if(items.empty()||items.size()>4096)fail(at,"Enum needs a bounded nonempty authored definition.", "migration.composed_source_invalid", Domain::Semantics);
        for(std::size_t i=0;i<items.size();++i) {
            const auto cp=at+"/enum_constant/items/"+std::to_string(i);fields(items[i],{"id","label","value","disabled","visible"},cp);
            binding::ChoiceItemMetadata choice;choice.id=text(items[i].member("id"),cp+"/id");choice.label=text(items[i].member("label"),cp+"/label",true);
            choice.value=ordinal(items[i].member("value"),cp+"/value");
            if(const auto *v=items[i].member("disabled"))choice.disabled=flag(v,cp+"/disabled");
            if(const auto *v=items[i].member("visible"))choice.visible=flag(v,cp+"/visible");
            if(!ids.insert(choice.id).second||!values.insert(choice.value).second)fail(cp,"Enum item IDs and underlying values must be distinct.", "migration.composed_source_invalid", Domain::Semantics);
            if((repr=="u8"&&choice.value>255)||(repr=="u16"&&choice.value>65535))fail(cp,"Enum item exceeds its actual representation width.", "migration.composed_source_invalid", Domain::Semantics);
            result.choices.push_back(std::move(choice));
        }
        const auto selected=ordinal(configuration.member("selected_value"),at+"/enum_constant/selected_value");
        if(!values.contains(selected))fail(at,"Enum literal does not select a real authored item.", "migration.composed_source_invalid", Domain::Semantics);return result;
    }
    binding::TypeRef ringType(const Json &node,const std::string &at) {
        const auto cp=at+"/ring_constant";
        const auto &configuration=objectAt(node.member("ring_constant"),cp);
        fields(configuration,{"selected_value","allow_undefined_values","text_overflow_visible","digital_display_visible","items"},cp);
        const auto repr=text(node.member("value_type"),at+"/value_type");
        const auto parsed=binding::parseType(repr);
        if(!parsed.valid()||parsed.type.kind!=binding::TypeKind::Primitive||!canonicalNumericRingRepresentationSupported(repr))
            fail(at,"This Ring literal needs an implemented exact real/integer carrier; EXT and other unavailable representations cannot become DBL.","migration.ring_representation_unavailable", Domain::FrontendCapability);
        auto result=parsed.type;
        const auto &items=arrayAt(configuration.member("items"),cp+"/items");
        if(items.size()>4096)fail(cp,"Ring choices exceed the bounded authored definition.", "migration.composed_source_invalid", Domain::Semantics);
        if(!items.empty())result.choiceKind=binding::ChoiceKind::Ring;
        std::set<std::string> ids;std::set<std::uint32_t> values;
        for(std::size_t i=0;i<items.size();++i) {
            const auto ip=cp+"/items/"+std::to_string(i);
            fields(items[i],{"id","label","value","disabled","visible"},ip);
            binding::ChoiceItemMetadata choice;
            choice.id=text(items[i].member("id"),ip+"/id");choice.label=text(items[i].member("label"),ip+"/label",true);
            choice.value=ordinal(items[i].member("value"),ip+"/value");
            if(!composedNumericValue(*items[i].member("value"),repr)||!canonicalNumericRingChoiceRepresentable(repr,choice.value))
                fail(ip,"Ring choice is not exactly representable in its actual numeric representation.","migration.ring_choice_unrepresentable", Domain::Semantics);
            if(const auto *v=items[i].member("disabled"))choice.disabled=flag(v,ip+"/disabled");
            if(const auto *v=items[i].member("visible"))choice.visible=flag(v,ip+"/visible");
            if(!ids.insert(choice.id).second||!values.insert(choice.value).second)fail(ip,"Ring item identities and values must be distinct.", "migration.composed_source_invalid", Domain::Semantics);
            result.choices.push_back(std::move(choice));
        }
        const auto selected=ordinal(configuration.member("selected_value"),cp+"/selected_value");
        if(!composedNumericValue(*configuration.member("selected_value"),repr)||!canonicalNumericRingChoiceRepresentable(repr,selected))
            fail(cp,"Ring selected value is not exactly representable in its actual numeric representation.","migration.ring_choice_unrepresentable", Domain::Semantics);
        const bool undefined=flag(configuration.member("allow_undefined_values"),cp+"/allow_undefined_values");
        if(!undefined&&!values.contains(selected))fail(cp,"Ring literal does not select an authored item and undefined values are disabled.", "migration.composed_source_invalid", Domain::Semantics);
        for(const auto key:{"text_overflow_visible","digital_display_visible"})if(const auto *v=configuration.member(key))static_cast<void>(flag(v,cp+"/"+key));
        return result;
    }
    // The map owns each decoded literal for this Adapter's lifetime. Insertion
    // preserves references, and entries are never erased: recursive consumers
    // can borrow the cached value without copying its complete Array payload.
    const Literal &literal(const std::string &id,unsigned depth=0) {
        if(const auto found=literals.find(id);found!=literals.end())return found->second;
        const auto &owner=nodes.at(id);const auto &raw=*owner.raw;const auto &at=owner.path;
        if(depth>32||!activeLiteral.insert(id).second)fail(at,"Literal ownership is cyclic or exceeds depth32.", "migration.composed_source_invalid", Domain::Integrity);
        Literal value;const auto type=bare(text(raw.member("type"),at+"/type"));
        if(collectionLiteral(raw)) {
            std::string why;const auto *cfg=raw.member("studio_collection_constant");
            const auto state=cfg?readCollectionAuthoringState(*cfg,&why):std::nullopt;
            if(!state||state->kind!=(type=="collection.map.constant"?CollectionKind::Map:CollectionKind::Set))
                fail(at,"The Collection constant kind and its complete authoring state differ: "+why, "migration.composed_source_invalid", Domain::Semantics);
            const auto typed=collectionValue(*state);
            if(!typed)fail(at,"A Collection constant has unresolved type models.", "migration.composed_source_invalid", Domain::Semantics);
            value.type=typed->type;value.value=writeCollectionData(*typed);
        }else if(recordLiteral(raw)) {
            const auto found=children.find(id);if(found==children.end()||found->second.empty()||found->second.size()>64)fail(at,"A Record literal requires1..64 real owned fields.", "migration.composed_source_invalid", Domain::FrontendCapability);
            std::map<std::uint32_t,std::string> order;std::vector<std::string> names,ids;std::vector<binding::TypeRef> types;std::set<std::string> unique;
            for(const auto &child:found->second) {
                const auto &entry=nodes.at(child);const auto &n=*entry.raw;
                const auto field=text(n.member("constant_field_id"),entry.path+"/constant_field_id");
                if(!unique.insert(field).second||!order.emplace(ordinal(n.member("constant_field_order"),entry.path),child).second)fail(entry.path,"Literal field IDs and declared order must be distinct.", "migration.composed_source_invalid", Domain::Integrity);
            }
            value.value=object();
            for(const auto &[_,child]:order) {
                const auto &entry=nodes.at(child);const auto &n=*entry.raw;const auto field=text(n.member("constant_field_id"),entry.path);
                const auto &data=literal(child,depth+1);ids.push_back(field);types.push_back(data.type);value.value.object.emplace(field,data.value);value.components.push_back(child);
                const auto *name=n.member("label");names.push_back(name?text(name,entry.path+"/label",true):field);
            }
            value.type=binding::TypeRef::cluster(std::move(ids),std::move(types));value.type.fieldNames=std::move(names);
        } else if(arrayLiteral(raw)) {
            value.type=port(id,"value",false).type;
            if(value.type.kind!=binding::TypeKind::Array||value.type.children.size()!=1||value.type.rank<1||value.type.rank>16)fail(at,"An Array literal needs its complete resolved element schema and rank.", "migration.composed_source_invalid", Domain::Semantics);
            const auto &configuration=objectAt(raw.member("array_constant"),at+"/array_constant");
            fields(configuration,{"rank","shape","first_indices","visible_counts","one_dimensional_vertical","cell_size","values","element_label_visible"},at+"/array_constant");
            if(ordinal(configuration.member("rank"),at)!=value.type.rank)fail(at,"The saved Array rank differs from its resolved output.", "migration.composed_source_invalid", Domain::Semantics);
            const auto &values=arrayAt(configuration.member("values"),at+"/array_constant/values");
            if(values.size()>kMaximumArrayElements)fail(at,"This local typed Array exceeds4294967295 elements.","migration.composed_resource_limit", Domain::Control);
            if(!arrayAllocationFitsBudget(values.size(),sizeof(Json),0,kMaximumFrogJsonTreeBytes))fail(at,"Array preparation representation budget exceeded: elements="+std::to_string(values.size())+", bytes_per_value="+std::to_string(sizeof(Json))+", budget_bytes="+std::to_string(kMaximumFrogJsonTreeBytes)+".","migration.array.representation_budget", Domain::Control);
            Json shape=array(),elements=array();std::size_t count=1;
            const auto &savedShape=arrayAt(configuration.member("shape"),at+"/array_constant/shape");
            if(savedShape.empty()&&value.type.rank==1){shape.array.push_back(number(values.size()));count=values.size();}
            else {
                if(savedShape.size()!=value.type.rank)fail(at,"Array extents must be explicit for every dimension.", "migration.composed_source_invalid", Domain::Integrity);
                std::vector<std::uint32_t> extents;
                for(const auto &extent:savedShape){extents.push_back(ordinal(&extent,at));shape.array.push_back(extent);}
                const auto cardinality=checkedArrayElementCount(extents);
                if(!cardinality)fail(at,"The Array exceeds4294967295 elements.","migration.composed_resource_limit", Domain::Control);
                count=*cardinality;
            }
            if(count!=values.size())fail(at,"Array shape and stored cell count differ; no implicit trimming or padding is performed.", "migration.composed_source_invalid", Domain::Semantics);
            const auto &element=value.type.children.front();
            std::optional<WidgetInstance> prototype;
            if(const auto *storedPrototype=raw.member("array_element_template")) {
                prototype=deserializeArrayElementWidget(text(storedPrototype,at+"/array_element_template"));
                if(!prototype)fail(at,"The Array element prototype is not a valid existing widget record.", "migration.composed_source_invalid", Domain::Integrity);
                const auto schema=binding::widgetType(*prototype);
                if(!schema.valid()||!canonicalObjectReferenceFullTypeEqual(schema.type,element))
                    fail(at,"The stored Array prototype differs from the complete resolved element schema.", "migration.composed_source_invalid", Domain::Semantics);
            }
            elements.array.reserve(count);
            for(const auto &cell:values) {
                const auto lexical=text(&cell,at+"/array_constant/values",true);
                auto parsed=source::parseJson(lexical,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                if(parsed&&parsed->type==Json::Type::Object&&parsed->member("array_cell_value")) {
                    if(!prototype)fail(at,"A compact Array cell requires its actual element prototype.", "migration.composed_source_invalid", Domain::Integrity);
                    const auto materialized=deserializeArrayElementWidget(lexical,&*prototype);
                    if(!materialized||!canonicalObjectReferenceFullTypeEqual(binding::widgetType(*materialized).type,element))
                        fail(at,"The existing Array value decoder rejected the stored cell or its complete type.", "migration.composed_source_invalid", Domain::Semantics);
                    std::string why;auto exactValue=typedWidgetSourceValue(*materialized,&why);
                    if(!exactValue)fail(at,"The actual Array cell has no valid typed Source value: "+why, "migration.composed_source_invalid", Domain::Semantics);
                    elements.array.push_back(std::move(*exactValue));
                } else if(element.kind==binding::TypeKind::Primitive&&(element.name=="c32"||element.name=="c64")) {
                    const auto complex=composedComplexLexeme(lexical,element.name);
                    if(!complex)fail(at,"A complex Array cell lost an exact finite component.", "migration.composed_source_invalid", Domain::Semantics);elements.array.push_back(*complex);
                } else if(element.kind==binding::TypeKind::Primitive&&(element.name=="string"||element.name=="path"))elements.array.push_back(string(lexical));
                else {if(!parsed)fail(at,"A stored Array cell is not an exact typed JSON value.", "migration.composed_source_invalid", Domain::Semantics);elements.array.push_back(std::move(*parsed));}
            }
            value.value=object();
            value.value.object.emplace("shape",std::move(shape));
            value.value.object.emplace("elements",std::move(elements));
        } else if(type=="numeric.enum_constant") {
            value.type=enumType(raw,at);value.value=*raw.member("enum_constant")->member("selected_value");
        } else if(type=="numeric.ring_constant") {
            value.type=ringType(raw,at);value.value=*raw.member("ring_constant")->member("selected_value");
        } else if(type=="boolean.true_constant"||type=="boolean.false_constant") {
            value.type=binding::TypeRef::primitive("bool");const bool expected=type=="boolean.true_constant";
            if(const auto *v=raw.member("text");v&&text(v,at+"/text")!=(expected?"true":"false"))fail(at,"Boolean literal text contradicts its actual operation.", "migration.composed_source_invalid", Domain::Semantics);value.value=boolean(expected);
        } else if(const auto fixed=fixedStringConstant(raw)) {
            if(const auto *v=raw.member("text");v&&text(v,at+"/text",true)!=*fixed)fail(at+"/text","Fixed String literal text contradicts its actual catalog.","migration.fixed_string_value_invalid", Domain::Semantics);
            if(const auto *hint=raw.member("value_type");hint&&(hint->type!=Json::Type::String||hint->string!="string"))fail(at+"/value_type","Fixed String literal type contradicts its actual catalog.","migration.fixed_string_type_invalid", Domain::Semantics);
            value.type=binding::TypeRef::primitive("string");value.value=string(std::string(*fixed));
        } else if(type=="string.constant"||type=="path.constant") {
            value.type=binding::TypeRef::primitive(type=="path.constant"?"path":"string");
            if(type=="path.constant") {
                draft_presentation::literalText(raw,"path",at,[](auto path,auto message,auto code) {
                    fail(std::move(path),std::move(message),std::move(code), Domain::Compatibility);
                });
                if(const auto *hint=raw.member("value_type");hint &&
                    (hint->type!=Json::Type::String || hint->string!="path"))
                    fail(at+"/value_type","Path literal type conflicts with its actual catalog.","migration.composed_literal_type_invalid", Domain::Semantics);
            }
            const auto *v=raw.member("text");value.value=string(v?text(v,at+"/text",true):"");
            if(value.value.string.size()>1'000'000||!canonicalStringUtf8Valid(value.value.string))fail(at,"A literal must remain bounded exact UTF-8.", "migration.composed_source_invalid", Domain::Integrity);
        } else if(const auto fixed=fixedNumericConstant(raw)) {
            if(!std::isfinite(*fixed))fail(at,"An IEEE preset must use its exact compiled zero-input constant operation, never a nonfinite JSON literal.","migration.fixed_constant_realization_invalid", Domain::Semantics);
            char buffer[64];const auto result=std::to_chars(buffer,buffer+sizeof(buffer),*fixed,std::chars_format::general,std::numeric_limits<double>::max_digits10);
            if(result.ec!=std::errc{})fail(at,"The existing finite fixed constant could not be serialized exactly.","migration.fixed_constant_value_invalid", Domain::Semantics);
            value.type=binding::TypeRef::primitive("f64");value.value.type=Json::Type::Number;value.value.string.assign(buffer,result.ptr);
        } else if(type=="variant.constant") {
            if(const auto *declared=raw.member("value_type");declared&&text(declared,at+"/value_type")!="variant")
                fail(at,"Variant literal lost its declared type.","migration.composed_variant_schema_invalid",Domain::Semantics);
            WidgetInstance widget;widget.classId="frog.widgets.variant_control";widget.valueType="variant";
            if(const auto *payload=raw.member("text"))widget.initialValue=text(payload,at+"/text",true);
            std::string why;const auto exact=typedWidgetSourceValue(widget,&why);
            if(!exact)fail(at,"The Variant literal has no exact typed Source projection: "+why,"migration.composed_variant_value_unavailable",Domain::FrontendCapability);
            value.type=binding::TypeRef::primitive("variant");value.value=*exact;
        } else if(type=="timestamp.constant") {
            // Owned Cluster fields use the same exact timestamp projection as
            // top-level controls. Their authored storage remains binary64
            // seconds; the Source carrier is signed64.64, never a DBL alias.
            if(text(raw.member("value_type"),at+"/value_type")!="timestamp")
                fail(at,"Timestamp literal lost its declared type.","migration.composed_source_invalid",Domain::Semantics);
            const auto exact=timestamp_source::lexical(text(raw.member("text"),at+"/text"));
            if(!exact)fail(at,"Timestamp literal cannot be projected exactly.","migration.composed_source_invalid",Domain::Semantics);
            value.type=binding::TypeRef::primitive("timestamp");value.value=*exact;
        } else if(type=="numeric.numeric_constant") {
            const auto scalar=text(raw.member("value_type"),at+"/value_type");const auto parsed=binding::parseType(scalar);
            if(!parsed.valid()||parsed.type.kind!=binding::TypeKind::Primitive)fail(at,"Numeric literal needs an exact declared scalar type.", "migration.composed_source_invalid", Domain::Semantics);
            value.type=parsed.type;const auto lexical=text(raw.member("text"),at+"/text");
            const auto json=(scalar=="c32"||scalar=="c64")?composedComplexLexeme(lexical,scalar):source::parseJson(lexical,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            const auto scalarValue=json?composedNumericValue(*json,scalar):std::nullopt;
            if(!scalarValue)fail(at,"Numeric literal is outside its actual exact type; no floating conversion is inserted.", "migration.composed_source_invalid", Domain::Semantics);value.value=*scalarValue;
        } else if(const auto *configuration=raw.member("studio_application_control")) {
            CanonicalObjectReferenceDiagnostic diagnostic;const auto descriptor=canonicalObjectReferenceDescriptor(*configuration,at+"/studio_application_control",{},&diagnostic);
            if(!descriptor||descriptor->kind!=CanonicalObjectReferenceKind::Resolve)fail(at,descriptor?"Only a real Reference constant can be contained literal data.":diagnostic.message,"migration.composed_literal_unavailable", !descriptor&&diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);
            const auto schema=source::parseJson(descriptor->ports.front().fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            const auto full=schema?canonicalObjectReferenceReadType(*schema):std::nullopt;if(!full)fail(at,"The contained Reference lost its exact type.", "migration.composed_source_invalid", Domain::Semantics);
            value.type=*full;value.value=object();value.value.object.emplace(std::string(canonicalObjectReferenceAttribute),*configuration);
        } else fail(at,"This owned literal family needs an explicit composed-program contract.","migration.composed_literal_unavailable", Domain::Compatibility);
        activeLiteral.erase(id);return literals.emplace(id,std::move(value)).first->second;
    }
    binding::TypeRef widgetType(const Json &widget,const std::string &path,unsigned depth=0,const binding::TypeRef *attested=nullptr) {
        if(depth>32)fail(path,"Widget value nesting exceeds32.", "migration.composed_source_invalid", Domain::Control);
        const auto klass=text(widget.member("classId"),path+"/classId"),declared=text(widget.member("valueType"),path+"/valueType");
        const auto &props=widgetProperties(widget,path);
        if(klass==graph::classId) {
            const auto decoded=deserializeArrayElementWidget(encode(widget));
            if(!decoded || !isWaveformGraphWidget(*decoded) || !decoded->waveformGraph || decoded->waveformGraph->preservedSource)
                fail(path,"Waveform Graph has an unsupported preserved Source configuration; no fallback value may execute.","migration.waveform_graph_unsupported_source",Domain::Compatibility);
            const auto why=attested?graph::validateType(*attested):"The complete typed widget witness is missing.";
            if(!attested || (declared!=graph::valueType && declared!=attested->canonical()) || !why.empty())
                fail(path,"Waveform Graph requires its complete structural schema: "+why,"migration.waveform_graph_schema_invalid",Domain::Semantics);
            return *attested;
        }
        if(klass==xy::classId) {
            const auto decoded=deserializeArrayElementWidget(encode(widget));
            if(!decoded || !isXYGraphWidget(*decoded) || !decoded->xyGraph || decoded->xyGraph->preservedSource)
                fail(path,"Graph XY has an unsupported preserved Source configuration; no fallback schema/value may execute.","migration.xy_graph_unsupported_source", Domain::Compatibility);
            const auto why=attested?xy::validateType(*attested):"The complete typed widget witness is missing.";
            if(!attested || declared!=attested->canonical() || !why.empty())
                fail(path,"Graph XY requires its exact D1-D6 structural schema: "+why,"migration.xy_graph_schema_invalid", Domain::Semantics);
            return *attested;
        }
        if(klass=="frog.studio.widgets.reference") {
            if(!attested||!configuredTypedNullResourceWidget(widget,*attested,path))
                fail(path,"The composed resource-reference widget requires its exact nominal schema and typed null Source configuration.","migration.composed_widget_unavailable", Domain::Semantics);
            return *attested;
        }
        if(klass=="frog.studio.widgets.collection") {
            const auto state=collectionWidgetState(widget,path);const auto type=collectionType(state);
            if(type.canonical()!=declared)fail(path,"The Collection widget declaration differs from its complete stored models.", "migration.composed_source_invalid", Domain::Semantics);
            return type;
        }
        if(klass=="frog.widgets.variant_control") {
            if(declared!="variant"||(attested&&(attested->kind!=binding::TypeKind::Primitive||attested->name!="variant")))
                fail(path,"The actual Variant widget requires its exact Variant schema.","migration.composed_variant_schema_invalid",Domain::Semantics);
            // panel() separately requires its hash-bound current/default witness;
            // stored default records also reuse this real-class schema check.
            return attested?*attested:binding::TypeRef::primitive("variant");
        }
        if(klass=="frog.widgets.cluster_control"||klass=="frog.widgets.cluster_indicator") {
            const auto &parts=arrayAt(props.member("cluster.elements"),path+"/props/cluster.elements");
            if(parts.empty()||parts.size()>64)fail(path,"A Cluster widget needs1..64 actual fields.", "migration.composed_source_invalid", Domain::FrontendCapability);
            std::map<std::uint32_t,const Json*> order;std::set<std::string> unique;
            for(const auto &part:parts) {
                const auto field=text(part.member("field_id"),path);
                if(!unique.insert(field).second||!order.emplace(ordinal(part.member("logical_order"),path),&part).second)
                    fail(path,"Widget field IDs and explicit logical orders must be distinct.", "migration.composed_source_invalid", Domain::Integrity);
            }
            std::vector<std::string> ids,names;std::vector<binding::TypeRef> types;
            for(const auto &[_,part]:order) {
                const auto &child=objectAt(part->member("widget"),path+"/props/cluster.elements/widget");
                ids.push_back(text(part->member("field_id"),path));names.push_back(text(child.member("label"),path,true));
                const binding::TypeRef *childSchema=nullptr;
                if(attested&&attested->kind==binding::TypeKind::Cluster) {
                    const auto field=std::find(attested->fieldIds.begin(),attested->fieldIds.end(),ids.back());
                    if(field==attested->fieldIds.end()||static_cast<std::size_t>(field-attested->fieldIds.begin())>=attested->children.size())
                        fail(path,"The codec's Cluster witness lost an actual stable field.","migration.composed_resolution_invalid", Domain::Integrity);
                    childSchema=&attested->children[static_cast<std::size_t>(field-attested->fieldIds.begin())];
                }
                types.push_back(widgetType(child,path+"/props/cluster.elements/widget",depth+1,childSchema));
            }
            auto result=binding::TypeRef::cluster(std::move(ids),std::move(types));result.fieldNames=std::move(names);return result;
        }
        const bool numeric=klass=="frog.widgets.numeric_control"||klass=="frog.widgets.numeric_indicator";
        const bool boolean=klass=="frog.widgets.boolean_control"||klass=="frog.widgets.boolean_indicator"||klass=="frog.widgets.button";
        const bool stringClass=klass=="frog.widgets.string_control"||klass=="frog.widgets.string_indicator";
        const bool pathClass=klass=="frog.widgets.path_control"||klass=="frog.widgets.path_indicator";
        const bool enumClass=klass=="frog.widgets.enum_control"||klass=="frog.widgets.enum_indicator"||
            klass=="frog.studio.widgets.tab_control"||klass=="frog.studio.widgets.tab_indicator";
        const bool ringClass=klass=="frog.widgets.ring_control"||klass=="frog.widgets.ring_indicator";
        const auto *dropdown=props.member("display.string.dropdown");
        const bool dropdownClass=stringClass&&dropdown&&dropdown->type==Json::Type::Boolean&&dropdown->boolean;
        if(enumClass||ringClass||dropdownClass||(attested&&(attested->kind==binding::TypeKind::Enum||attested->choiceKind!=binding::ChoiceKind::None))) {
            // The hash-bound common codec owns choices, nominal domains and
            // current/default values. Only attest the real graphical family
            // here; never replace an Enum with its integer representation.
            bool matching=false;
            if(attested&&attested->known()) {
                if(enumClass&&attested->kind==binding::TypeKind::Enum&&attested->choiceKind==binding::ChoiceKind::Enum&&attested->children.size()==1) {
                    const auto &representation=attested->children.front();
                    matching=representation.kind==binding::TypeKind::Primitive&&
                        (representation.name=="u8"||representation.name=="u16"||representation.name=="u32")&&
                        (declared==representation.canonical()||declared=="enum_item_id");
                } else if(ringClass)matching=attested->kind==binding::TypeKind::Primitive&&attested->numeric()&&
                    attested->choiceKind==binding::ChoiceKind::Ring&&declared==attested->canonical();
                else if(dropdownClass)matching=attested->kind==binding::TypeKind::Primitive&&attested->name=="string"&&
                    attested->choiceKind==binding::ChoiceKind::Dropdown&&declared=="string";
            }
            if(!matching)fail(path,"The choice widget family and its complete codec schema disagree.","migration.composed_widget_unavailable", Domain::Semantics);
            if(ringClass&&!canonicalNumericRingRepresentationSupported(attested->name))
                fail(path,"The Ring has no implemented exact carrier in its declared representation.","migration.ring_representation_unavailable", Domain::FrontendCapability);
            if(ringClass&&!canonicalNumericRingMetadataValid(*attested))
                fail(path,"Ring choices are not exactly representable in the declared numeric type; no choice is silently rounded.","migration.ring_choice_unrepresentable", Domain::Semantics);
            return *attested;
        }
        const auto type=binding::parseType(declared);
        if(!type.valid()||type.type.kind!=binding::TypeKind::Primitive||
            (!numeric&&!boolean&&!stringClass&&!pathClass)||(boolean&&declared!="bool")||
            (stringClass&&declared!="string")||(pathClass&&declared!="path")||(numeric&&!type.type.numeric()&&type.type.name!="timestamp"))
            fail(path,"The composed widget value requires its real supported class and complete exact schema.","migration.composed_widget_unavailable",
                !type.valid()||(boolean&&declared!="bool")||(stringClass&&declared!="string")||(pathClass&&declared!="path")||
                (numeric&&(!type.type.numeric()&&type.type.name!="timestamp"))?Domain::Semantics:
                !numeric&&!boolean&&!stringClass&&!pathClass?Domain::Compatibility:Domain::FrontendCapability);
        return type.type;
    }
    Json scalarWidgetValue(const Json &widget,const binding::TypeRef &schema,const std::string &path) {
        const auto &props=widgetProperties(widget,path);
        if(schema.kind==binding::TypeKind::Primitive&&schema.name=="variant") {
            const auto decoded=deserializeArrayElementWidget(encode(widget));
            if(!decoded||decoded->classId!="frog.widgets.variant_control"||
                !canonicalObjectReferenceFullTypeEqual(binding::widgetType(*decoded).type,schema))
                fail(path,"The Variant value lost its actual widget class or exact schema.","migration.composed_variant_schema_invalid",Domain::Semantics);
            std::string why;const auto projected=typedWidgetSourceValue(*decoded,&why);
            if(!projected)
                fail(path,"The actual Variant value has no lossless typed Source projection: "+why,"migration.composed_variant_value_unavailable",Domain::FrontendCapability);
            return *projected;
        }
        if(schema.kind==binding::TypeKind::Map||schema.kind==binding::TypeKind::Set) {
            const auto state=collectionWidgetState(widget,path);const auto typed=collectionValue(state);
            if(!typed||!canonicalObjectReferenceFullTypeEqual(typed->type,schema))fail(path,"Collection current/default data lost its complete schema.", "migration.composed_source_invalid", Domain::Semantics);
            return writeCollectionData(*typed);
        }
        if(schema.kind==binding::TypeKind::Cluster) {
            const auto &parts=arrayAt(props.member("cluster.elements"),path+"/props/cluster.elements");Json result=object();
            for(std::size_t i=0;i<schema.fieldIds.size();++i) {
                const auto found=std::find_if(parts.begin(),parts.end(),[&](const Json &p){return text(p.member("field_id"),path)==schema.fieldIds[i];});
                if(found==parts.end())fail(path,"A stored Cluster value lost an actual stable field.", "migration.composed_source_invalid", Domain::Semantics);
                result.object.emplace(schema.fieldIds[i],scalarWidgetValue(objectAt(found->member("widget"),path),schema.children[i],path+"/"+schema.fieldIds[i]));
            }
            return result;
        }
        if(schema.name=="string"||schema.name=="path") {
            const auto *value=props.member("text.value");
            // Absence retains the real widget codec's intrinsic value; an
            // explicit empty String/Path remains distinct. Do not duplicate
            // those defaults here or bypass the hash-bound witnesses below.
            std::string content;
            if(value)content=text(value,path+"/props/text.value",true);
            else {
                const auto decoded=deserializeArrayElementWidget(encode(widget));
                if(!decoded||!canonicalObjectReferenceFullTypeEqual(binding::widgetType(*decoded).type,schema)||
                    (schema.name=="string"?!isStringWidget(*decoded):!isPathWidget(*decoded)))
                    fail(path,"The existing widget codec could not resolve the intrinsic String/Path value.", "migration.composed_source_invalid", Domain::Semantics);
                content=schema.name=="string"?widgetStringValue(*decoded):widgetPathValue(*decoded);
            }
            if(content.size()>1'000'000||!canonicalStringUtf8Valid(content))fail(path,"Widget String value exceeds the exact UTF-8 transport budget.", "migration.composed_source_invalid", Domain::Control);return string(content);
        }
        const auto &state=objectAt(widget.member("state"),path+"/state");const auto *raw=state.member("value");
        if(const auto *lexical=state.member("initial_value")) {
            const auto literal=text(lexical,path+"/state/initial_value");
            const auto value=(schema.name=="c32"||schema.name=="c64")?composedComplexLexeme(literal,schema.name):source::parseJson(literal,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            const auto scalarValue=value?composedNumericValue(*value,schema.canonical()):std::nullopt;
            if(!scalarValue)fail(path,"Stored lexical widget state is outside its exact declared width.", "migration.composed_source_invalid", Domain::Semantics);return *scalarValue;
        }
        if(!raw)fail(path,"A scalar widget needs its explicit current state.", "migration.composed_source_invalid", Domain::Integrity);
        if(schema.name=="i32"||schema.name=="u32"||schema.name=="i64"||schema.name=="u64") {
            if(raw->type!=Json::Type::Number)fail(path,"An exact integer widget requires numeric source state.", "migration.composed_source_invalid", Domain::Integrity);
            const auto lexical=numeric_value::canonicalWideIntegerLexeme(raw->string,schema.name=="i32"?"i64":schema.name=="u32"?"u64":schema.name);
            const auto value=lexical?source::parseJson(*lexical,nullptr,source::JsonNumberPolicy::PreserveLexeme):std::nullopt;
            if(!value||!typedScalar(*value,schema.name))fail(path,"An integer widget value is outside its exact width.", "migration.composed_source_invalid", Domain::Semantics);return *value;
        }
        const auto scalarValue=composedNumericValue(*raw,schema.name);
        if(!scalarValue)fail(path,"A widget value contradicts its exact declared type.", "migration.composed_source_invalid", Domain::Semantics);return *scalarValue;
    }
    void identifyWidgetTree(const Json &widget,const std::string &path,unsigned depth=0) {
        if(depth>32)fail(path,"Widget ownership exceeds32.", "migration.composed_source_invalid", Domain::Control);
        const auto id=text(widget.member("id"),path+"/id");
        // Root IDs are document identities. Contained IDs are scoped by their
        // actual owner/field path; independent Error Clusters legitimately
        // reuse status/code/source. The runtime registry qualifies those paths
        // and refuses an ambiguous bare alias when it is actually referenced.
        if(id.size()>1024||(depth==0&&!widgetIds.insert(id).second))fail(path,"Root widget identities must be distinct and every identity bounded.", "migration.composed_source_invalid", Domain::Integrity);
        const auto *props=widget.member("props");
        if(props)if(const auto *parts=props->member("cluster.elements")) {
            std::set<std::string> fields;
            for(std::size_t i=0;i<arrayAt(parts,path).size();++i) {
                const auto &part=parts->array[i];const auto field=text(part.member("field_id"),path);
                if(!fields.insert(field).second)fail(path,"Contained field IDs must be distinct within their actual owner.", "migration.composed_source_invalid", Domain::Integrity);
                identifyWidgetTree(objectAt(part.member("widget"),path),path+"/props/cluster.elements/"+std::to_string(i)+"/widget",depth+1);
            }
        }
    }
    Json panel() {
        const auto &raw=objectAt(root.member("frontPanel"),"/frontPanel");fields(raw,{"canvas","grid","widgets"},"/frontPanel");
        Json result=object(),list=array();if(const auto *canvas=raw.member("canvas"))result.object.emplace("canvas",*canvas);
        const auto &widgets=arrayAt(raw.member("widgets"),"/frontPanel/widgets");
        if(widgets.size()>1024)fail("/frontPanel","The existing widget budget was exceeded.", "migration.composed_source_invalid", Domain::Control);
        for(std::size_t i=0;i<widgets.size();++i) {
            const auto at="/frontPanel/widgets/"+std::to_string(i),target="/front_panel/widgets/"+std::to_string(i);const auto &authored=objectAt(&widgets[i],at);
            const auto normalization=normalizeCanonicalDraftWidget(authored,at,&resolved);
            for(const auto &d:normalization.diagnostics)if(d.blocking)fail(d.path,d.message,d.code,d.domain);
            const auto &widget=normalization.current?*normalization.current:authored;
            authoredWidgetFields(widget,at);
            if(const auto *locked=widget.member("locked");locked&&locked->type!=Json::Type::Boolean)
                fail(at+"/locked","The retained authoring lock must be an explicit Boolean.","migration.composed_widget_flag_invalid", Domain::Integrity);
            const auto id=text(widget.member("id"),at+"/id"),role=text(widget.member("role"),at+"/role");
            if(role!="control"&&role!="indicator")fail(at,"The actual widget role must remain control or indicator.", "migration.composed_source_invalid", Domain::Semantics);
            identifyWidgetTree(widget,at);
            const auto witness=std::find_if(resolved.widgets.begin(),resolved.widgets.end(),[&](const auto &w){return w.widgetId==id;});
            if(witness==resolved.widgets.end()||std::count_if(resolved.widgets.begin(),resolved.widgets.end(),[&](const auto &w){return w.widgetId==id;})!=1)
                fail(at,"A widget needs one hash-bound current/default projection from the common private value codec.","migration.composed_widget_resolution_required", Domain::Semantics);
            const auto full=source::parseJson(witness->fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            const auto attested=full?canonicalObjectReferenceReadType(*full):std::nullopt;
            bool compositeArray=false,choiceProjection=false,resourceReferenceProjection=false,complexProjection=false;
            if(attested) {
                std::vector<const binding::TypeRef*> pending{&*attested};
                while(!pending.empty()) {
                    const auto *t=pending.back();pending.pop_back();
                    compositeArray=compositeArray||t->kind==binding::TypeKind::Array;
                    choiceProjection=choiceProjection||t->kind==binding::TypeKind::Enum||t->choiceKind!=binding::ChoiceKind::None;
                    resourceReferenceProjection=resourceReferenceProjection||isQueueReference(*t)||isNotifierReference(*t)||isOccurrenceReference(*t)||isSemaphoreReference(*t)||isRendezvousReference(*t);
                    complexProjection=complexProjection||(t->kind==binding::TypeKind::Primitive&&(t->name=="c32"||t->name=="c64"));
                    // The child of Reference<T> is its target/message model,
                    // not a value contained in the widget. A Notifier<Array>
                    // remains a Reference widget, never an Array widget.
                    if(t->kind!=binding::TypeKind::Reference)
                        for(const auto &c:t->children)pending.push_back(&c);
                }
            }
            const bool projectedValue=compositeArray||choiceProjection||resourceReferenceProjection||complexProjection;
            // The private codec owns compact/sparse widget storage. Consume its
            // bounded typed projection of the same hash-bound model instead of
            // implementing a competing private document decoder here.
            const auto widgetClass=text(widget.member("classId"),at);
            const bool xyWidget=widgetClass==xy::classId || widgetClass==graph::classId;
            auto schema=compositeArray&&attested&&!xyWidget?*attested:widgetType(widget,at,0,attested?&*attested:nullptr);
            if(compositeArray&&!xyWidget) {
                const auto klass=text(widget.member("classId"),at);
                if((schema.kind==binding::TypeKind::Array&&klass!="frog.widgets.array_control"&&klass!="frog.widgets.array_indicator")||
                   (schema.kind==binding::TypeKind::Cluster&&klass!="frog.widgets.cluster_control"&&klass!="frog.widgets.cluster_indicator")||
                   (schema.kind!=binding::TypeKind::Array&&schema.kind!=binding::TypeKind::Cluster&&klass!="frog.studio.widgets.collection"))
                    fail(at,"The composed value schema does not match its actual widget family.","migration.composed_widget_unavailable", Domain::Semantics);
            }
            if(!attested||!canonicalObjectReferenceFullTypeEqual(*attested,schema))fail(at,"Current/default widget evidence has another complete schema.","migration.composed_resolution_invalid", Domain::Integrity);
            const auto typedProjection=[&](const std::string &encoded,const std::string &encodedWidget) {
                if(encoded.empty()||encoded.size()>8U*1024U*1024U)fail(at,"An exact typed widget projection is required.", "migration.composed_source_invalid", Domain::Integrity);
                const auto raw=source::parseJson(encoded,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                CanonicalObjectReferenceDiagnostic why;
                const std::vector<CanonicalObjectReferencePort> signature{{"value",canonicalObjectReferencePhysicalType(schema).canonical(),canonical_json::encode(canonicalObjectReferenceWriteType(schema)),false,true}};
                bool accepted=raw&&canonicalObjectReferenceRecordDescriptor(CanonicalObjectReferenceKind::Resolve,schema,signature,{},&*raw,at,&why).has_value();
                if(!accepted&&raw) {
                    // The general Record literal profile remains finite. Only
                    // actual Graph leaves admit explicit IEEE gaps, through
                    // the same hash-bound current/default widget codec that
                    // produced this projection. Ordinary sibling fields keep
                    // their existing finite policy.
                    const auto actual=deserializeArrayElementWidget(encodedWidget);
                    const auto containsXY=[&](const auto &self,const WidgetInstance &value,unsigned depth)->bool {
                        if(depth>32)return false;
                        if(isXYGraphWidget(value))return true;
                        if(const auto *cluster=clusterContainerState(value))
                            for(const auto &field:cluster->elements)if(field.widget&&self(self,*field.widget,depth+1))return true;
                        if(const auto *array=arrayContainerState(value);array&&array->elementTemplate)
                            return self(self,*array->elementTemplate,depth+1);
                        return false;
                    };
                    if(actual&&actual->instanceId==id&&containsXY(containsXY,*actual,0)) {
                        const auto actualType=binding::widgetType(*actual);
                        const auto projected=typedWidgetSourceValue(*actual);
                        accepted=actualType.valid()&&canonicalObjectReferenceFullTypeEqual(actualType.type,schema)&&projected&&exact(*projected,*raw);
                    }
                }
                if(!accepted)fail(at,"Invalid exact typed widget projection: "+why.message, "migration.composed_source_invalid", Domain::Integrity);
                return *raw;
            };
            widgetTypes.emplace(id,schema);
            for(const auto &[nodeId,node]:nodes)if(const auto *wid=node.raw->member("widget");wid&&wid->type==Json::Type::String&&wid->string==id) {
                const auto &actual=port(nodeId,"value",role=="indicator");
                if(!canonicalObjectReferenceFullTypeEqual(actual.type,schema))fail(at,"Widget schema and its actual authored participation disagree.","migration.composed_resolution_invalid", Domain::Integrity);
            }
            const auto current=projectedValue?typedProjection(witness->currentValueJson,witness->currentWidgetJson):scalarWidgetValue(widget,schema,at);auto initial=current;
            const auto witnessWidget=[&](const std::string &encoded,const char *phase) {
                if(encoded.size()>8U*1024U*1024U)fail(at,"Widget codec evidence exceeds its existing budget.", "migration.composed_source_invalid", Domain::Control);
                const auto record=source::parseJson(encoded,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                if(!record)fail(at,std::string("The common ")+phase+" widget record is invalid.", "migration.composed_source_invalid", Domain::Integrity);
                const Json *actual=&*record;if(const auto *nested=record->member("widget"))actual=nested;
                if(text(actual->member("id"),at)!=id||!canonicalObjectReferenceFullTypeEqual(widgetType(*actual,at,0,&schema),schema))
                    fail(at,std::string("The common ")+phase+" widget projection changed current identities or schema.", "migration.composed_source_invalid", Domain::Integrity);
                return *actual;
            };
            const auto decodeWitness=[&](const std::string &encoded,const char *phase) {return scalarWidgetValue(witnessWidget(encoded,phase),schema,at);};
            if((choiceProjection||resourceReferenceProjection||complexProjection)&&!compositeArray) {
                static_cast<void>(witnessWidget(witness->currentWidgetJson,"current"));
                static_cast<void>(witnessWidget(witness->defaultWidgetJson,"default"));
            }
            if(!projectedValue&&!exact(decodeWitness(witness->currentWidgetJson,"current"),current))fail(at,"The codec's current value does not match the actual source value.","migration.composed_resolution_stale", Domain::Integrity);
            initial=projectedValue?typedProjection(witness->defaultValueJson,witness->defaultWidgetJson):decodeWitness(witness->defaultWidgetJson,"default");
            const auto &props=widgetProperties(widget,at);
            if(const auto *stored=props.member("data.default");stored&&!projectedValue) {
                const auto rawDefault=text(stored,at+"/props/data.default",true);
                if(!rawDefault.empty()) {
                    const auto decoded=source::parseJson(rawDefault,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                    if(!decoded)fail(at+"/props/data.default","The authored stored default is invalid JSON.", "migration.composed_source_invalid", Domain::Integrity);
                    const Json *actual=&*decoded;
                    if(const auto *nested=decoded->member("widget"))actual=nested;
                    const auto storedSchema=widgetType(objectAt(actual,at),at+"/props/data.default");
                    if(!canonicalObjectReferenceFullTypeEqual(storedSchema,schema))
                        out.diagnostics.push_back({"migration.composed_historical_widget_default_migrated",at+"/props/data.default",
                            "The stored historical default has another field schema. The existing common default projection preserved the current stable fields and migrated matching identities only; no positional pairing or source rewrite was performed.",false});
                }
            }
            Json declaration=object(),properties=object();properties.object.emplace("default_value",initial);
            declaration.object={{"id",string(id)},{"role",string(role)},{"class_ref",*widget.member("classId")},
                {"value_type",string(canonicalObjectReferencePhysicalType(schema).canonical())},{"props",std::move(properties)},
                {"studio_type_schema",canonicalObjectReferenceWriteType(schema)},{"studio_widget",widget}};
            if(const auto *bounds=widget.member("bounds"))declaration.object.emplace("layout",*bounds);
            list.array.push_back(std::move(declaration));if(role=="control")out.controls.push_back({id,canonicalObjectReferencePhysicalType(schema).canonical(),encode(current)});
            out.identities.push_back({"widget",id,at,target,false});
        }
        result.object.emplace("widgets",std::move(list));return result;
    }
    std::string operation(const Json &node,const std::string &at) {
        const auto authored=text(node.member("type"),at+"/type"),id=bare(authored);
        if(authored==kNativeCallDiagramNodeType)return std::string(kNativeCallScalarOperationId);
        if(!canonicalNumericBooleanConversionMigrationIdentity(authored).empty()) {
            auto mode=BooleanArraySignExtensionMode::IfOutputIsSigned;
            if(const auto *saved=node.member("boolean_array_sign_extension_mode")) {
                const auto value=text(saved,at+"/boolean_array_sign_extension_mode");
                if(value=="never")mode=BooleanArraySignExtensionMode::Never;
                else if(value=="always")mode=BooleanArraySignExtensionMode::Always;
                else if(value!="if_output_is_signed")fail(at,"Unknown Boolean Array sign-extension mode.","migration.numeric_boolean_mode_invalid", Domain::Compatibility);
            }
            const auto operation=canonicalNumericBooleanConversionMigrationIdentity(authored,mode);
            const auto nodeId=text(node.member("id"),at+"/id");
            std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)signature.push_back({std::string(canonicalNumericBooleanConnectorId(authored,std::get<1>(key))),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;if(!canonicalNumericBooleanConversionSignatureValid(operation,signature,&why,&terminal))fail(at+"/"+terminal,why,"migration.numeric_boolean_signature_invalid", Domain::Semantics);return std::string(operation);
        }
        if(const auto textual=canonicalNumericTextMigrationIdentity(authored);!textual.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;if(!canonicalNumericTextSignatureValid(textual,signature,&why,&terminal))fail(at+"/"+terminal,why,"migration.numeric_text_signature_invalid", Domain::Semantics);return std::string(textual);
        }
        if(const auto bits=canonicalNumericBitReinterpretationMigrationIdentity(authored);!bits.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;if(!canonicalNumericBitReinterpretationSignatureValid(bits,signature,&why,&terminal))fail(at+"/"+terminal,why,"migration.numeric_bit_reinterpretation_signature_invalid", Domain::Semantics);return std::string(bits);
        }
        if(const auto fixed=canonicalNumericIeeeConstantMigrationIdentity(authored);!fixed.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;if(!canonicalNumericIeeeConstantSignatureValid(fixed,signature,&why,&terminal))fail(at+"/"+terminal,why,"migration.fixed_constant_signature_invalid", Domain::Semantics);
            return std::string(fixed);
        }
        if(const auto calibration=canonicalNumericCalibrationMigrationIdentity(authored);!calibration.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;if(!canonicalNumericCalibrationSignatureValid(calibration,signature,&why,&terminal))fail(at+"/"+terminal,why,"migration.numeric_calibration_signature_invalid", Domain::Semantics);return std::string(calibration);
        }
        if(const auto accumulate=canonicalNumericAccumulateMigrationIdentity(authored);!accumulate.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)
                signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;if(!canonicalNumericAccumulateSignatureValid(accumulate,signature,&why,&terminal))
                fail(at+(terminal.empty()?"":"/"+terminal),why,"migration.numeric_accumulate_signature_invalid",Domain::Semantics);
            return std::string(accumulate);
        }
        if(const auto random=canonicalNumericRandomMigrationIdentity(authored);!random.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;if(!canonicalNumericRandomSignatureValid(random,signature,&why,&terminal))
                fail(at+(terminal.empty()?"":"/"+terminal),why,"migration.numeric_random_signature_invalid", Domain::Semantics);
            return std::string(random);
        }
        const bool comparisonAggregates=node.member("comparison_aggregates")?
            flag(node.member("comparison_aggregates"),at+"/comparison_aggregates"):false;
        if(const auto comparison=canonicalComparisonTreeMigrationIdentity(authored,comparisonAggregates);!comparison.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");
            std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)
                signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;
            if(!canonicalComparisonTreeSignatureValid(comparison,signature,&why,&terminal))
                fail(at+(terminal.empty()?"":"/"+terminal),why,"migration.comparison_signature_invalid", Domain::Semantics);
            // The same bool/i32/u32/i64/u64/f64 exact scalar instructions and
            // Select ABI remain available inside an otherwise composed graph.
            if(const auto legacy=canonicalComparisonTreeLegacyIdentity(authored,signature,comparisonAggregates);!legacy.empty())return std::string(legacy);
            return std::string(comparison);
        }
        if(const auto logical=canonicalBooleanTreeMigrationIdentity(authored);!logical.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");
            std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)
                signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;
            if(!canonicalBooleanTreeSignatureValid(logical,signature,&why,&terminal))
                fail(at+(terminal.empty()?"":"/"+terminal),why,"migration.boolean_signature_invalid", Domain::Semantics);
            if(canonicalBooleanTreeShouldUse(logical,signature))return std::string(logical);
            const auto count=std::count_if(signature.begin(),signature.end(),[](const auto &p){return p.input;});
            const auto scalar=canonicalScalarMigrationIdentityForArity(authored,count);
            if(scalar.empty())fail(at,"The exact scalar Boolean arity has no current realization.","migration.boolean_signature_invalid", Domain::Semantics);
            return std::string(scalar);
        }
        if(const auto reduction=canonicalNumericArrayReductionMigrationIdentity(authored);!reduction.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why,terminal;if(!canonicalNumericArrayReductionSignatureValid(reduction,signature,&why,&terminal))fail(at+"/"+terminal,why,"migration.numeric_reduction_signature_invalid", Domain::Semantics);return std::string(reduction);
        }
        if(const auto *complex=canonicalNumericComplexContract(authored)) {
            const auto nodeId=text(node.member("id"),at+"/id");std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string why;if(!canonicalNumericComplexSignatureValid(authored,signature,&why))fail(at,why,"migration.numeric_complex_signature_invalid", Domain::Semantics);
            return std::string(complex->operationId);
        }
        if(const auto numeric=canonicalNumericScalarPaletteMigrationIdentity(authored,
                node.member("in_range_include_lower_limit")?flag(node.member("in_range_include_lower_limit"),at):true,
                node.member("in_range_include_upper_limit")?flag(node.member("in_range_include_upper_limit"),at):false);!numeric.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");
            std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)
                signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            const bool lower=node.member("in_range_include_lower_limit")?flag(node.member("in_range_include_lower_limit"),at):true;
            const bool upper=node.member("in_range_include_upper_limit")?flag(node.member("in_range_include_upper_limit"),at):false;
            if(canonicalNumericScalarPaletteShouldUse(authored,signature,lower,upper))return std::string(numeric);
            if(const auto *selection=canonicalScalarSelectionAuthoredContract(authored);
               selection&&selection->kind==CanonicalScalarSelectionKind::MaxMin) {
                const auto &data=port(nodeId,"x",true).type;
                if(data.kind!=binding::TypeKind::Primitive||
                   !canonicalScalarSelectionTypeAccepted(*selection,data.canonical()))
                    fail(at,"Max & Min has no exact admitted selection type.","migration.numeric_selection_signature_invalid", Domain::Semantics);
                for(const auto &actual:signature)if(!binding::fullTypeEqual(actual.type,data))
                    fail(at+"/"+actual.id,"Max & Min requires its exact complete common data schema.","migration.numeric_selection_signature_invalid", Domain::Semantics);
                return std::string(selection->operationId);
            }
        }
        if(const auto scale=canonicalScalePowerTwoMigrationIdentity(authored);!scale.empty())return std::string(scale);
        if(canonicalExponentialIdentity(authored))return authored;
        if(id=="array.index") {
            const auto count=node.member("index_array_outputs")?ordinal(node.member("index_array_outputs"),at+"/index_array_outputs"):1u;
            if(count<1||count>32)fail(at+"/index_array_outputs","Index Array requires its real 1..32 output groups.","migration.array_index_configuration_invalid", Domain::Semantics);
            const auto nodeId=text(node.member("id"),at+"/id");
            const auto &input=port(nodeId,"array",true);
            const auto index=ports.find({nodeId,"index",true});
            if(count!=1||input.type.kind!=binding::TypeKind::Array||input.type.rank!=1||
               index==ports.end()||index->second.type.canonical()!="i32"||index->second.sources.empty())
                return std::string(canonicalArrayIndexExtendedId);
            const auto &output=port(nodeId,"element",false);
            if(input.type.children.size()!=1||input.sources.empty()||
               !canonicalObjectReferenceFullTypeEqual(input.type.children.front(),output.type))
                fail(at,"Index Array requires its connected exact Array and element schema.","migration.array_index_variant_unavailable", Domain::FrontendCapability);
        }
        if(id=="array.matrix.build"&&node.member("matrix_append_by_rows")&&flag(node.member("matrix_append_by_rows"),at+"/matrix_append_by_rows"))
            return std::string(canonicalMatrixBuildRowsId);
        if(const auto *poly=findPolynomialFunction(authored))return std::string(poly->operationId);
        if(const auto discrete=canonicalDiscreteArrayMigrationIdentity(authored);!discrete.empty())return std::string(discrete);
        if(const auto operation=canonicalDifferentialMigrationIdentity(authored);!operation.empty())return std::string(operation);
        if(const auto *optimization=optimizationDefinition(authored))return optimization->definitionId;
        if(const auto *linear=findLinearAlgebraFunction(authored))return std::string(linear->operationId);
        if(const auto *blas=findBlasFunction(authored))return std::string(blas->operationId);
        if(isGeometryFunction(authored)) {
            const auto *selected=node.member("geometry_variant");
            const auto operation=geometryOperationId(authored,selected?text(selected,at+"/geometry_variant"):std::string{});
            if(operation.empty())fail(at+"/geometry_variant","Unknown Geometry variant for this operation.","migration.geometry.variant_invalid",Domain::Semantics);
            return std::string(operation);
        }
        if(isInterpolationFunction(authored)) {
            const auto *selected=node.member("interpolation_variant");
            const auto operation=interpolationOperationId(authored,selected?text(selected,at+"/interpolation_variant"):std::string{});
            if(operation.empty())fail(at+"/interpolation_variant","Unknown Interpolation variant for this operation.","migration.interpolation.variant_invalid",Domain::Semantics);
            return std::string(operation);
        }
        if(isStatisticsFunction(authored)) {
            const auto *selected=node.member("statistics_variant");
            const auto operation=statisticsOperationId(authored,selected?text(selected,at+"/statistics_variant"):std::string{});
            if(operation.empty())fail(at+"/statistics_variant","Unknown Statistics variant for this operation.","migration.statistics.variant_invalid",Domain::Semantics);
            return std::string(operation);
        }
        if(isCalculusFunction(authored)) {
            const auto *selected=node.member("calculus_variant");
            const auto operation=calculusOperationId(authored,selected?text(selected,at+"/calculus_variant"):std::string{});
            if(operation.empty())fail(at+"/calculus_variant","Unknown Calculus variant for this operation.","migration.calculus.variant_invalid",Domain::Semantics);
            return std::string(operation);
        }
        if(isFittingFunction(authored)) {
            const auto *selected=node.member("fitting_variant");
            const auto operation=fittingOperationId(authored,selected?text(selected,at+"/fitting_variant"):std::string{});
            if(operation.empty())fail(at+"/fitting_variant","Unknown Fitting variant for this operation.","migration.fitting.variant_invalid",Domain::Semantics);
            return std::string(operation);
        }
        if(const auto arrayValue=canonicalArrayValueMigrationIdentity(authored);!arrayValue.empty())return std::string(arrayValue);
        if(const auto aggregate=canonicalNumericAggregateMigrationIdentity(authored);!aggregate.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");
            std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)
                signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::stable_sort(signature.begin(),signature.end(),[](const auto &a,const auto &b){
                if(a.input!=b.input)return a.input;
                return canonicalNumericAddInputOrdinal(a.id).value_or(16)<canonicalNumericAddInputOrdinal(b.id).value_or(16);
            });
            if(!canonicalNumericConversionTarget(aggregate).empty()) {
                std::string message,terminal;
                if(!canonicalNumericAggregateSignatureValid(aggregate,signature,&message,&terminal))
                    fail(at+(terminal.empty()?"":"/"+terminal),message,"migration.numeric_conversion_signature_invalid", Domain::Semantics);
                return std::string(aggregate);
            } else if(aggregate==canonicalNumericExactAbsId) {
                const auto input=std::find_if(signature.begin(),signature.end(),[](const auto &p){return p.input&&p.id=="in";});
                if(input!=signature.end()&&(input->type.canonical()=="i8"||input->type.canonical()=="i16"||input->type.canonical()=="f32")) {
                    std::string message,port;
                    if(!canonicalNumericAggregateSignatureValid(aggregate,signature,&message,&port))
                        fail(at+"/"+port,message,"migration.numeric_exact_abs_signature_invalid", Domain::Semantics);
                    return std::string(aggregate);
                }
            } else if(canonicalNumericAggregateShouldUse(signature)) {
                std::string message,port;
                if(!canonicalNumericAggregateSignatureValid(canonicalNumericAggregateAddId,signature,&message,&port))
                    fail(at+"/"+port,message,"migration.numeric_aggregate_signature_invalid", Domain::Semantics);
                return std::string(canonicalNumericAggregateAddId);
            }
        }
        if((id=="string.search_split"||authored=="string.search_split"||authored=="frog.studio.string.search_split")&&node.member("text")&&node.member("text")->string=="single_character")
            return "frog.studio.string.search_split.single_character.bytes.v1";
        // Preserve complete resolved connectors. In particular, an omitted
        // time_stamp stays absent so the native activation samples its clock.
        if(const auto variant=canonicalVariantAnalysisMigrationIdentity(authored);!variant.empty())return std::string(variant);
        if(const auto timestamp=canonicalTimestampMigrationIdentity(authored);!timestamp.empty())return std::string(timestamp);
        if(const auto fileValue=canonicalFileMigrationIdentity(authored);!fileValue.empty())return std::string(fileValue);
        if(const auto pathValue=canonicalPathValueMigrationIdentity(authored);!pathValue.empty())return std::string(pathValue);
        if(const auto stringValue=canonicalStringValueMigrationIdentity(authored);!stringValue.empty())return std::string(stringValue);
        if(id.starts_with("math.formula."))if(const auto formula=frog::math::formula::operation(id))
            return std::string(frog::math::formula::operationId(*formula));
        if(id=="structures.feedback_node")return "frog.studio.structures.feedback_node";
        if(id=="synchronization.first_call")return "frog.studio.synchronization.first_call";
        if(isNotifierFunction(id))return "frog.studio."+id;
        if(isSemaphoreFunction(id))return "frog.studio."+id;
        if(isRendezvousFunction(id))return "frog.studio."+id;
        if(isOccurrenceFunction(id))return "frog.studio."+id;
        if(isEventFunction(id))return "frog.studio."+id;
        if(id=="array.build") {
            const auto nodeId=text(node.member("id"),at+"/id");
            const auto count=ordinal(node.member("build_array_inputs"),at+"/build_array_inputs");
            if(!count||count>32)
                fail(at,"Build Array requires 1..32 authored inputs.","migration.build_array_configuration_invalid", Domain::Semantics);
            const bool concatenate=flag(node.member("build_array_concatenate"),at+"/build_array_concatenate");
            const auto &output=port(nodeId,"result",false).type;
            if(output.kind!=binding::TypeKind::Array||output.children.size()!=1)
                fail(at,"Build Array requires a resolved Array result.","migration.build_array_type_invalid", Domain::Semantics);
            const bool resourceReference=output.rank==1&&(notifierReferenceElementType(output.children.front())||
                isRendezvousReference(output.children.front())||isOccurrenceReference(output.children.front()));
            if(!resourceReference) {
                std::vector<int> ranks; ranks.reserve(count);
                for(unsigned index=0;index<count;++index) {
                    const auto &input=port(nodeId,buildArrayInputId(index),true).type;
                    ranks.push_back(input.kind==binding::TypeKind::Array?input.rank:0);
                    const auto &element=input.kind==binding::TypeKind::Array&&input.children.size()==1?input.children.front():input;
                    if(!canonicalObjectReferenceFullTypeEqual(element,output.children.front()))
                        fail(at,"Build Array inputs must retain their resolved common element schema.","migration.build_array_type_invalid", Domain::Semantics);
                }
                const auto mode=buildArrayMode(concatenate,ranks);
                if(mode.invalidRanks||output.rank!=mode.outputRank)
                    fail(at,"Build Array result rank differs from its authored construction mode.","migration.build_array_type_invalid", Domain::Semantics);
                return std::string(mode.concatenate?canonicalArrayConcatenateOwnedId:canonicalArrayBuildOwnedId);
            }
            if(concatenate)fail(at,"Resource reference Build Array requires non-concatenating mode.","migration.notifier_array_mode_invalid", Domain::Semantics);
            for(unsigned index=0;index<count;++index)if(!canonicalObjectReferenceFullTypeEqual(port(nodeId,buildArrayInputId(index),true).type,output.children.front()))
                fail(at,"Build Array requires identical complete reference input schemas.","migration.notifier_array_type_invalid", Domain::Semantics);
            if(isRendezvousReference(output.children.front()))return "frog.studio.array.build.rendezvous1d.v1";
            if(isOccurrenceReference(output.children.front()))return "frog.studio.array.build.occurrence1d.v1";
            return "frog.studio.array.build.notifier1d.v1";
        }
        if(isContextPathFunction(authored))return "frog.studio."+id;
        if(const auto collection=canonicalCollectionOperationId(authored);!collection.empty())return std::string(collection);
        if(id=="cluster.bundle_by_name")return "frog.studio.record.bundle_by_name.v1";
        if(id=="cluster.unbundle_by_name")return "frog.studio.record.unbundle_by_name.v1";
        if(id=="cluster.bundle")return "frog.studio.record.bundle.v1";
        if(id=="cluster.unbundle")return "frog.studio.record.unbundle.v1";
        if(const auto error=canonicalErrorHandlingMigrationIdentity(id);!error.empty())return std::string(error);
        if(const auto operation=canonicalVariantPrimitiveMigrationIdentity(id);!operation.empty())return std::string(operation);
        if(id=="string.search_replace")return "frog.studio.string.search_replace.v1";
        if(isQueueFunction(id))return "frog.studio."+id;
        if(const auto stringOperation=canonicalStringMigrationIdentity(authored);!stringOperation.empty())return std::string(stringOperation);
        if(const auto math=canonicalComposedMathMigrationIdentity(authored);!math.empty()) {
            const auto nodeId=text(node.member("id"),at+"/id");
            std::vector<CanonicalComposedMathPort> signature;
            for(const auto &[key,value]:ports)if(std::get<0>(key)==nodeId)
                signature.push_back({std::get<1>(key),value.type,std::get<2>(key),!std::get<2>(key)||!value.sources.empty()});
            std::string message,invalidPort;
            if(!canonicalComposedMathSignatureValid(math,signature,&message,&invalidPort))
                fail(at+(invalidPort.empty()?"":"/"+invalidPort),message,"migration.composed_math_variant_unavailable", Domain::FrontendCapability);
            return std::string(math);
        }
        if(const auto scalar=canonicalScalarMigrationIdentity(authored);!scalar.empty()) {
            const auto *contract=canonicalScalarNativeContract(scalar);
            if(contract&&contract->catalogId=="numeric.increment") {
                const auto nodeId=text(node.member("id"),at+"/id");
                const auto evidence=std::find_if(resolved.scalars.begin(),resolved.scalars.end(),[&](const auto &v){return v.nodeId==nodeId;});
                if(evidence==resolved.scalars.end()||evidence->authoredOperation!=authored||evidence->operands.size()!=1)fail(at,"Increment has no exact current source-operand evidence.","migration.scalar_resolution_incomplete", Domain::Semantics);
                const auto &actual=evidence->operands.front();const auto &input=port(nodeId,"in",true);const auto &output=port(nodeId,"result",false);
                if(actual.inputPortId!="in"||!actual.accepted||!actual.complete||actual.valueType.empty()||actual.sourceNodeId.empty()||actual.sourcePortId.empty()||input.type.canonical()!=actual.valueType||input.sources.size()!=1||!input.sources.contains({actual.sourceNodeId,actual.sourcePortId})||!canonicalObjectReferenceFullTypeEqual(input.type,output.type)||!canonicalObjectReferenceFullTypeEqual(input.type,port(actual.sourceNodeId,actual.sourcePortId,false).type))fail(at,"Increment source, input and result must retain the same complete actual operand type.","migration.scalar_resolution_incomplete", Domain::Semantics);
                const auto operation=canonicalScalarMigrationIdentityForType(authored,actual.valueType);
                if(operation.empty())fail(at,"Increment has no admitted implementation for the exact current operand type.","migration.scalar_type_unavailable", Domain::FrontendCapability);
                return std::string(operation);
            }
            if(contract&&contract->catalogId=="numeric.add") {
                const auto nodeId=text(node.member("id"),at+"/id");
                const auto evidence=std::find_if(resolved.scalars.begin(),resolved.scalars.end(),[&](const auto &v){return v.nodeId==nodeId;});
                if(evidence==resolved.scalars.end()||evidence->authoredOperation!=authored)fail(at,"Add has no exact current operand evidence.","migration.scalar_resolution_incomplete", Domain::Semantics);
                std::vector<std::string> inputs;std::string operandType;
                for(const auto &actual:evidence->operands) {
                    const auto &declared=port(nodeId,actual.inputPortId,true);
                    if(!actual.accepted||!actual.complete||actual.valueType.empty()||actual.sourceNodeId.empty()||actual.sourcePortId.empty()||declared.type.canonical()!=actual.valueType||!declared.sources.contains({actual.sourceNodeId,actual.sourcePortId}))fail(at,"Add operand evidence differs from its actual source endpoint/type.","migration.scalar_resolution_incomplete", Domain::Semantics);
                    if(!operandType.empty()&&operandType!=actual.valueType)fail(at,"Add requires identical actual operand types, without conversion.","migration.scalar_mixed_operands_unavailable", Domain::FrontendCapability);
                    operandType=actual.valueType;inputs.push_back(actual.inputPortId);
                }
                if(!canonicalNumericAddInputPortsValid(inputs))fail(at,"Add requires a,b and actual ascending input_3..input_16.","migration.numeric_add_ports_invalid", Domain::Semantics);
                if(inputs.size()>2) {
                    const auto operation=canonicalScalarMigrationIdentityForType(canonicalScalarMigrationIdentityForArity(authored,inputs.size()),operandType);
                    if(operation.empty())fail(at,"Add reduction has no exact supported operand implementation.","migration.scalar_type_unavailable", Domain::FrontendCapability);
                    return std::string(operation);
                }
                const auto operation=canonicalScalarMigrationIdentityForType(authored,operandType);
                if(operation.empty())fail(at,"Binary Add has no exact supported operand implementation.","migration.scalar_type_unavailable", Domain::FrontendCapability);
                return std::string(operation);
            }
            if(contract && contract->catalogId.starts_with("numeric.")) {
                const auto nodeId=text(node.member("id"),at+"/id");
                const auto &input=port(nodeId,std::string(contract->inputA),true);
                const auto typed=canonicalScalarMigrationIdentityForType(authored,input.type.canonical());
                if(typed.empty())fail(at,"The scalar numeric operation has no admitted implementation for its exact operand type.","migration.scalar_type_unavailable", Domain::FrontendCapability);
                return std::string(typed);
            }
            return std::string(scalar);
        }
        if(node.member("studio_application_control")) {
            const auto &cfg=objectAt(node.member("studio_application_control"),at+"/studio_application_control");std::string why;
            if(!application_control_source::preflight(cfg,&why))fail(at,why, "migration.composed_source_invalid", Domain::Semantics);
            const auto decoded=application_control_source::readConfiguration(cfg,&why);
            if(!decoded)fail(at,"The authored application-control operation has no admitted native implementation.","migration.composed_operation_unavailable", Domain::FrontendCapability);
            return std::string(canonicalObjectReferenceOperation(decoded->kind==ApplicationControlKind::Reference?CanonicalObjectReferenceKind::Resolve:decoded->kind==ApplicationControlKind::Invoke?CanonicalObjectReferenceKind::Invoke:decoded->kind==ApplicationControlKind::Close?CanonicalObjectReferenceKind::Close:CanonicalObjectReferenceKind::Property));
        }
        fail(at,"The actual authored operation has no composed-program implementation contract: "+authored,"migration.composed_operation_unavailable", operationDomain(authored));
    }
    void nodeFields(const Json &node,const std::string &path) {
        // The composed pass and its scalar-literal adapter share the same
        // strict presentation contract; neither changes the U32 color value.
        draft_presentation::colorBox(node,path,[](auto at,auto message,auto code) {
            fail(std::move(at),std::move(message),std::move(code),Domain::Compatibility);
        });
        if(const auto *alignment=node.member("structure_subdiagram_label_alignment")) {
            const auto *kind=node.member("kind");
            if(!kind||kind->type!=Json::Type::String||kind->string!="structure"||alignment->type!=Json::Type::String||
                (alignment->string!="left"&&alignment->string!="center"&&alignment->string!="right"))
                fail(path+"/structure_subdiagram_label_alignment","Subdiagram label alignment requires a structure owner and left, center or right.",
                    "migration.presentation_invalid",Domain::Compatibility);
        }
        for(const auto key:{"file_read_lines","file_convert_eol"})if(const auto *value=node.member(key)) {
            const auto *type=node.member("type");
            const auto owner=type&&type->type==Json::Type::String?bare(type->string):std::string{};
            const bool actual=key==std::string_view("file_read_lines")?owner=="file.text_read":
                owner=="file.text_read"||owner=="file.text_write";
            if(!actual||value->type!=Json::Type::Boolean)
                fail(pointer(path,key),"File options must remain Boolean and belong to their actual File operation.","migration.file.configuration_invalid", Domain::Semantics);
        }
        const auto *terminalKind=node.member("kind"),*terminalWidget=node.member("widget");
        const auto pairedArray=terminalWidget&&terminalWidget->type==Json::Type::String?widgetTypes.find(terminalWidget->string):widgetTypes.end();
        const bool actualArrayTerminal=terminalKind&&terminalKind->type==Json::Type::String&&terminalKind->string=="widget_value"&&
            pairedArray!=widgetTypes.end()&&pairedArray->second.kind==binding::TypeKind::Array&&pairedArray->second.children.size()==1;
        if(actualArrayTerminal) {
            // Create Indicator retains this derived presentation on the paired
            // Diagram terminal. The Front Panel is the sole value authority;
            // its complete Array element schema must agree with the template.
            if(const auto *prototype=node.member("array_element_template")) {
                const auto decoded=deserializeArrayElementWidget(text(prototype,path+"/array_element_template"));
                if(!decoded)fail(path+"/array_element_template","The Array terminal has no valid element presentation widget.","migration.array.terminal_template_invalid", Domain::Semantics);
                const auto schema=binding::widgetType(*decoded);
                if(!schema.valid()||!canonicalObjectReferenceFullTypeEqual(schema.type,pairedArray->second.children.front()))
                    fail(path+"/array_element_template","The Array terminal template differs from its exact linked widget element schema.","migration.array.terminal_template_invalid", Domain::Semantics);
            }
            if(const auto *icon=node.member("array_element_icon");icon&&icon->type!=Json::Type::Boolean)
                fail(path+"/array_element_icon","Array terminal icon presentation must be Boolean.","migration.presentation_invalid", Domain::Semantics);
        }
        if(const auto *automatic=node.member("constant_text_bounds_automatic")) {
            if(!scalarLiteral(node)&&!fixedNumericConstant(node)&&!recordLiteral(node)&&!arrayLiteral(node)&&!collectionLiteral(node))
                fail(path+"/constant_text_bounds_automatic","Automatic constant text bounds require an actual literal owner.","migration.presentation_invalid", Domain::Semantics);
            if(automatic->type!=Json::Type::Boolean)
                fail(path+"/constant_text_bounds_automatic","Automatic constant text bounds must be Boolean.","migration.presentation_invalid", Domain::Semantics);
        }
        if(const auto *automatic=node.member("constant_array_cell_bounds_automatic")) {
            // Older Create Indicator retained this literal-only display hint
            // on its paired Array terminal. It is inert, with an exact owner.
            const auto *kind=node.member("kind"),*widget=node.member("widget");
            const auto paired=widget&&widget->type==Json::Type::String?widgetTypes.find(widget->string):widgetTypes.end();
            const bool historicalArrayTerminal=kind&&kind->type==Json::Type::String&&kind->string=="widget_value"&&
                paired!=widgetTypes.end()&&paired->second.kind==binding::TypeKind::Array;
            if(!arrayLiteral(node)&&!historicalArrayTerminal)
                fail(path+"/constant_array_cell_bounds_automatic","Automatic Array cell bounds require an actual Array literal owner.","migration.presentation_invalid", Domain::Semantics);
            if(automatic->type!=Json::Type::Boolean)
                fail(path+"/constant_array_cell_bounds_automatic","Automatic Array cell bounds must be Boolean.","migration.presentation_invalid", Domain::Semantics);
        }
        if(node.member("studio_native_call")) {
            const auto *type=node.member("type");
            if(!type||type->type!=Json::Type::String||type->string!=kNativeCallDiagramNodeType)
                fail(path,"NativeCall configuration belongs only to its actual configured node.","migration.native_call_configuration_invalid", Domain::Semantics);
            if(ordinal(root.member("draft_revision"),"/draft_revision")!=7)
                fail(path,"Configured NativeCall requires its explicitly identified revision-7 writer.","migration.native_call_revision_unavailable", Domain::Compatibility);
        }
        if(const auto *mode=node.member("comparison_aggregates")) {
            const auto *type=node.member("type");
            const auto *definition=type&&type->type==Json::Type::String?comparison_source::definition(type->string):nullptr;
            if(!definition||!comparison_source::contracts::binary(definition->kind)||mode->type!=Json::Type::Boolean)
                fail(path+"/comparison_aggregates","Compare Elements/Aggregates belongs only to its actual binary Comparison owner and must remain Boolean.","migration.comparison_mode_invalid", Domain::Semantics);
        }
        const auto arrayOwner=node.member("type")&&node.member("type")->type==Json::Type::String?bare(node.member("type")->string):std::string{};
        for(const auto &[key,maximum]:std::array<std::pair<std::string_view,unsigned>,2>{{{"array_edit_groups",32},{"reshape_array_dimensions",16}}})if(const auto *count=node.member(key)) {
            const bool correctOwner=key=="array_edit_groups"?(arrayOwner=="array.replace_subset"||arrayOwner=="array.insert"):arrayOwner=="array.reshape";
            if(!correctOwner||ordinal(count,pointer(path,key))<1||ordinal(count,pointer(path,key))>maximum)
                fail(pointer(path,key),"Array configuration requires its real bounded operation owner.","migration.array_configuration_invalid", Domain::Semantics);
        }
        if(const auto *rows=node.member("matrix_append_by_rows"))if(arrayOwner!="array.matrix.build"||rows->type!=Json::Type::Boolean)
            fail(path+"/matrix_append_by_rows","Append Rows belongs only to Build Matrix and must remain Boolean.","migration.array_configuration_invalid", Domain::Semantics);
        if(const auto *count=node.member("array_function_terminals")) {
            const auto *type=node.member("type");
            if(!type||type->type!=Json::Type::String||canonicalArrayValueMigrationIdentity(type->string).empty()||
               ordinal(count,path+"/array_function_terminals")==0||ordinal(count,path+"/array_function_terminals")>(arrayOwner=="array.initialize"?16U:32U))
                fail(path+"/array_function_terminals","Array connector configuration needs its actual supported owner and 1..32 terminals.", "migration.composed_source_invalid", Domain::Semantics);
        }
        for(const auto &[key,_]:node.object)if(key.starts_with("feedback_")) {
            const auto *type=node.member("type");
            if(!type||type->type!=Json::Type::String||bare(type->string)!="structures.feedback_node")
                fail(pointer(path,key),"Feedback configuration cannot silently apply to another operation.","migration.feedback.owner_invalid", Domain::Semantics);
        }
        if(node.member("studio_collection_constant")&&!collectionLiteral(node))
            fail(path,"Collection constant state belongs only to the corresponding Map/Set literal.", "migration.composed_source_invalid", Domain::Semantics);
        if(node.member("studio_collection_build")) {
            const auto *type=node.member("type");const auto id=type&&type->type==Json::Type::String?canonicalCollectionOperationId(type->string):std::string_view{};
            if(id!="frog.collection.map.build"&&id!="frog.collection.set.build")fail(path,"Collection Build row configuration belongs only to Build Map or Build Set.", "migration.composed_source_invalid", Domain::Semantics);
        }
        if(node.member("studio_sequence_profile")||node.member("studio_sequence_selected_frame")||node.member("studio_sequence_frame_numbers_visible")) {
            const auto *kind=node.member("structure_type");
            if(!kind||kind->type!=Json::Type::String||kind->string!="sequence_structure")fail(path,"Sequence configuration belongs only to its actual structure.","migration.sequence.owner_invalid", Domain::Semantics);
            if(const auto *visible=node.member("studio_sequence_frame_numbers_visible");visible&&visible->type!=Json::Type::Boolean)
                fail(path+"/studio_sequence_frame_numbers_visible","Sequence frame number visibility must remain Boolean.","migration.composed_source_invalid",Domain::Integrity);
        }
        if(const auto *variant=node.member("geometry_variant")) {
            const auto owner=bare(text(node.member("type"),path));
            if(!isGeometryFunction(owner)||geometryOperationId(owner,text(variant,path+"/geometry_variant")).empty())
                fail(path+"/geometry_variant","Geometry configuration must identify a known variant of its actual operation.","migration.geometry.variant_invalid",Domain::Semantics);
        }
        if(const auto *variant=node.member("interpolation_variant")) {
            const auto owner=bare(text(node.member("type"),path));
            if(!isInterpolationFunction(owner)||interpolationOperationId(owner,text(variant,path+"/interpolation_variant")).empty())
                fail(path+"/interpolation_variant","Interpolation configuration must identify a known variant of its actual operation.","migration.interpolation.variant_invalid",Domain::Semantics);
        }
        if(const auto *variant=node.member("statistics_variant")) {
            const auto owner=text(node.member("type"),path);
            if(!isStatisticsFunction(owner)||statisticsOperationId(owner,text(variant,path+"/statistics_variant")).empty())
                fail(path+"/statistics_variant","Unknown Statistics variant for this operation.","migration.statistics.variant_invalid",Domain::Semantics);
        }
        if(const auto *selected=node.member("statistics_outputs")) {
            const auto owner=text(node.member("type"),path);
            if(!isStatisticsExpressFunction(owner))fail(path+"/statistics_outputs","Only Statistics Express owns a selected output set.","migration.statistics.outputs_invalid",Domain::Semantics);
            std::set<std::string> seen;
            for(const auto &value:arrayAt(selected,path+"/statistics_outputs")) {
                const auto port=text(&value,path+"/statistics_outputs");
                const auto declared=statisticsOutputPortIds(owner);
                if(port=="error_out"||std::find(declared.begin(),declared.end(),port)==declared.end()||!seen.insert(port).second)
                    fail(path+"/statistics_outputs","Unknown or duplicate selected Statistics output.","migration.statistics.outputs_invalid",Domain::Semantics);
            }
            if(seen.empty())fail(path+"/statistics_outputs","Select at least one Statistics result.","migration.statistics.outputs_invalid",Domain::Semantics);
        }
        if(const auto *variant=node.member("calculus_variant")) {
            // Callable constructors have exact native identities, unlike
            // palette operation aliases. Keep their qualified owner here.
            const auto owner=text(node.member("type"),path);
            if(!(node.member("kind")&&text(node.member("kind"),path)=="calculus_function_reference"&&((text(variant,path)=="function_1d"&&calculusFunctionReferenceDimensions(owner)==1)||(text(variant,path)=="function_2d"&&calculusFunctionReferenceDimensions(owner)==2)||(text(variant,path)=="function_3d"&&calculusFunctionReferenceDimensions(owner)==3)))&&(!isCalculusFunction(owner)||calculusOperationId(owner,text(variant,path+"/calculus_variant")).empty()))
                fail(path+"/calculus_variant","Calculus configuration must identify a known variant of its actual operation.","migration.calculus.variant_invalid",Domain::Semantics);
        }
        if(const auto *variant=node.member("fitting_variant")) {
            const auto owner=bare(text(node.member("type"),path));
            if(!isFittingFunction(owner)||fittingOperationId(owner,text(variant,path+"/fitting_variant")).empty())
                fail(path+"/fitting_variant","Fitting configuration must identify a known variant of its actual operation.","migration.fitting.variant_invalid",Domain::Semantics);
        }
        for(const auto &[key,_]:node.object) {
            if(presentation(key)||key=="numeric_color_box"||key=="file_read_lines"||key=="file_convert_eol"||key=="id"||key=="kind"||key=="type"||key=="widget"||key=="value_type"||key=="text"||key=="studio_native_call"||
                key=="timed_loop_subdiagram_label_text"||key=="timed_loop_schedule"||key=="timing_nodes"||key=="studio_timed_loop_options"||key=="timed_loop_background_opacity_percent"||key=="timed_loop_background_rgb"||key=="timed_loop_condition_normalized_x"||key=="timed_loop_condition_normalized_y"||key=="timed_loop_feedback_fields"||key=="timed_loop_feedback_timing_node_normalized_y"||key=="timed_loop_feedback_visible_row_count"||key=="timed_loop_initial_fields"||key=="timed_loop_initial_timing_node_normalized_y"||key=="timed_loop_initial_visible_row_count"||key=="timed_loop_iteration_normalized_x"||key=="timed_loop_iteration_normalized_y"||key=="timed_loop_result_fields"||key=="timed_loop_result_timing_node_normalized_y"||key=="timed_loop_result_visible_row_count"||key=="timed_loop_subframe_opacity_percent"||key=="timed_loop_subframe_rgb"||key=="timed_loop_update_fields"||key=="timed_loop_update_timing_node_normalized_y"||key=="timed_loop_update_visible_row_count"||
                key=="structure_type"||key=="boundary"||key=="structure_terminals"||key=="structure_tunnels"||key=="regions"||
                key=="constant_owner_node_id"||key=="constant_field_id"||key=="constant_field_order"||key=="constant_text_bounds_automatic"||key=="constant_array_cell_bounds_automatic"||
                (key=="call"&&node.member("kind")&&(text(node.member("kind"),path)=="fitting_model_reference"||text(node.member("kind"),path)=="calculus_function_reference"))||key=="geometry_variant"||key=="interpolation_variant"||key=="statistics_variant"||key=="statistics_outputs"||key=="calculus_variant"||key=="fitting_variant"||key=="comparison_aggregates"||key=="binding_profile"||key=="case_profile"||key=="enum_constant"||key=="string_constant"||key=="path_constant"||
                (key=="ring_constant"&&node.member("type")&&bare(text(node.member("type"),path))=="numeric.ring_constant")||
                key=="cluster_constant_collapsed"||key=="cluster_constant_autosizing"||key=="cluster_expanded_width"||key=="cluster_expanded_height"||
                key=="cluster_function_terminals"||key=="cluster_field_names"||key=="cluster_field_id_paths"||key=="cluster_show_full_names"||
                key=="string_function_terminals"||key=="array_function_terminals"||key=="array_edit_groups"||key=="reshape_array_dimensions"||key=="matrix_append_by_rows"||key=="retain_first_error_inputs"||key=="studio_application_control"||
                (key=="boolean_array_sign_extension_mode"&&node.member("type")&&!canonicalNumericBooleanConversionMigrationIdentity(text(node.member("type"),path)).empty())||
                ((key=="in_range_include_lower_limit"||key=="in_range_include_upper_limit")&&node.member("type")&&bare(text(node.member("type"),path))=="numeric.in_range")||
                ((key=="build_array_inputs"||key=="build_array_concatenate")&&node.member("type")&&bare(text(node.member("type"),path))=="array.build")||
                (key=="index_array_outputs"&&node.member("type")&&bare(text(node.member("type"),path))=="array.index")||
                key=="studio_event_dynamic_terminals_visible"||key=="studio_event_dynamic_terminals_normalized_y"||key=="studio_event_response_node_normalized_y"||
                (key=="studio_event_registration_rows"&&node.member("type")&&bare(text(node.member("type"),path))=="dialog.events.register_for_events")||
                key=="studio_in_place_pairs"||key=="studio_in_place_profile"||key=="studio_in_place_next_pair_serial"||
                key=="studio_subdiagram_label_text"||key=="studio_sequence_profile"||key=="studio_sequence_selected_frame"||key=="studio_sequence_frame_numbers_visible"||key=="array_constant"||
                ((key=="array_element_template"||key=="array_element_icon")&&(arrayLiteral(node)||actualArrayTerminal))||
                key=="studio_collection_constant"||key=="studio_collection_build"||
                key=="feedback_node_configuration"||key=="feedback_delay"||key=="feedback_initialization"||
                key=="feedback_initialization_explicit"||key=="feedback_initializer_loop_id"||
                key=="feedback_history"||key=="feedback_z_transform")continue;
            fail(pointer(path,key),"The authored node field has no composed-program semantic contract: "+key,"migration.composed_field_unavailable", Domain::Compatibility);
        }
    }
    void componentMappings(const std::string &id,const std::string &target) {
        for(const auto &component:literals.at(id).components) {
            const auto &entry=nodes.at(component);const auto field=text(entry.raw->member("constant_field_id"),entry.path);const auto where=pointer(target,field);
            out.identities.push_back({"literal_component",component,entry.path,where,false});
            componentMappings(component,where);
        }
    }
    CanonicalObjectReferenceDescriptor recordDescriptor(const Json &node,const std::string &id,
        const std::string &path,const std::string &target) {
        const auto authored=bare(text(node.member("type"),path));
        const bool select=authored=="cluster.unbundle"||authored=="cluster.unbundle_by_name";
        const bool byName=authored.ends_with("_by_name");
        const auto rail=ports.find({id,"cluster",true});
        const bool connectedRail=rail!=ports.end()&&!rail->second.sources.empty();
        const bool construct=authored=="cluster.bundle"&&!connectedRail;
        if(!construct&&!connectedRail)fail(path,"Bundle By Name and Unbundle require their actual connected complete Record rail.", "migration.composed_source_invalid", Domain::Semantics);
        const auto &schema=construct?port(id,"result",false).type:port(id,"cluster",true).type;
        if(schema.kind!=binding::TypeKind::Cluster)fail(path,"The real Bundle/Unbundle rail must be a complete Record.", "migration.composed_source_invalid", Domain::Semantics);
        std::vector<CanonicalObjectReferencePort> signature;std::vector<std::vector<std::string>> paths;
        struct ActivePort {std::string name;const Port *actual;bool input;unsigned row;};
        std::vector<ActivePort> active;
        for(const auto &[key,actual]:ports)if(std::get<0>(key)==id) {
            const auto &name=std::get<1>(key);const bool input=std::get<2>(key);
            // Unconnected schema rails and replacement rows are editor
            // affordances, not runtime inputs with invented default values.
            if(construct&&input&&name=="cluster")continue;
            unsigned row=0;
            if(name.starts_with("element")) {
                const auto parsed=std::from_chars(name.data()+7,name.data()+name.size(),row);
                if(parsed.ec!=std::errc{}||parsed.ptr!=name.data()+name.size()||!row||row>32||name!="element"+std::to_string(row))fail(path,"An active Record port must retain its real bounded elementN identity.", "migration.composed_source_invalid", Domain::Semantics);
                if(input&&authored=="cluster.bundle"&&!construct&&actual.sources.empty())continue;
            }
            if(input&&actual.sources.size()!=1)fail(path,"Every active Record input requires its one actual connected source: "+name, "migration.composed_source_invalid", Domain::Semantics);
            active.push_back({name,&actual,input,row});
        }
        // Semantic elementN order must not become map/string order at row10.
        std::stable_sort(active.begin(),active.end(),[](const ActivePort &a,const ActivePort &b){
            const auto rank=[](const ActivePort &p){return p.input&&p.name=="cluster"?0:p.row?1:2;};
            if(rank(a)!=rank(b))return rank(a)<rank(b);
            return a.row!=b.row?a.row<b.row:a.name<b.name;
        });
        for(const auto &entry:active) {
            const auto &actual=*entry.actual;const auto &name=entry.name;const auto row=entry.row;
            signature.push_back({name,canonicalObjectReferencePhysicalType(actual.type).canonical(),encode(actual.schema),entry.input,entry.input,{}});
            if(!row)continue;
            if(byName) {
                const auto &selected=arrayAt(node.member("cluster_field_id_paths"),path+"/cluster_field_id_paths");
                if(row>selected.size())fail(path,"An active By Name row needs its persisted stable field path.", "migration.composed_source_invalid", Domain::Semantics);
                std::vector<std::string> parts;for(const auto &part:arrayAt(&selected[row-1],path))parts.push_back(text(&part,path));
                if(parts.empty()||parts.size()>32)fail(path,"A By Name path requires1..32 actual stable IDs.", "migration.composed_source_invalid", Domain::Integrity);paths.push_back(std::move(parts));
            } else {
                if(row>schema.fieldIds.size())fail(path,"An ordinal Record row exceeds the actual authored schema.", "migration.composed_source_invalid", Domain::Semantics);paths.push_back({schema.fieldIds[row-1]});
            }
        }
        CanonicalObjectReferenceDiagnostic diagnostic;
        const auto descriptor=canonicalObjectReferenceRecordDescriptor(construct?CanonicalObjectReferenceKind::Construct:select?CanonicalObjectReferenceKind::Select:CanonicalObjectReferenceKind::Update,
            schema,std::move(signature),std::move(paths),nullptr,target+"/"+std::string(canonicalObjectReferenceAttribute)+"/configuration",&diagnostic);
        if(!descriptor)fail(path,diagnostic.message,"migration.composed_source_invalid", diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);return *descriptor;
    }
    // Materialize an optional stateful numeric input as a real typed constant in the
    // immutable adapted scope. Neither the authored document nor its UI value
    // is mutated/read. Complete schemas come from the hash-bound resolver.
    void materializeStatefulNumericDefaults(const Json &rawNode,const std::string &at,
        const std::string &targetPath,Json &nodeList,Json &edgeList) {
        const auto authored=text(rawNode.member("type"),at);
        const bool accumulate=!canonicalNumericAccumulateMigrationIdentity(authored).empty();
        if(!accumulate&&canonicalNumericRandomMigrationIdentity(authored)!=canonicalNumericRandomRangeId)return;
        const std::vector<std::string> names=accumulate?
            std::vector<std::string>{"x","y","initial_value","initialize"}:
            std::vector<std::string>{"lower_limit","upper_limit"};
        const auto ownerId=text(rawNode.member("id"),at+"/id");
        for(const auto &name:names) {
            const auto &actual=port(ownerId,name,true);
            if(!actual.sources.empty())continue;
            const auto token=accumulate?std::optional<std::string>{"0"}:canonicalNumericRandomDefault(actual.type,name);
            if(!token)fail(at,"The optional numeric input has no exact resolved default.","migration.numeric_random_default_unavailable", Domain::FrontendCapability);
            const auto unique=[&](std::string stem,auto &generated,const auto &occupied) {
                auto candidate=stem;std::size_t suffix=0;
                while(occupied.contains(candidate)||generated.contains(candidate))candidate=stem+":"+std::to_string(++suffix);
                generated.insert(candidate);return candidate;
            };
            const auto stem=std::string(accumulate?"accumulate-default:":"random-default:")+
                hashing::sha256Hex(std::to_string(ownerId.size())+":"+ownerId+":"+std::string(name));
            const auto id=unique(stem,generatedStatefulDefaultNodes,nodes);
            const auto edgeId=unique(stem+":edge",generatedStatefulDefaultEdges,edgeIds);
            auto declaration=object(),connectors=array(),connector=object();
            connector.object={{"id",string("value")},{"direction",string("output")},
                {"type",string(canonicalObjectReferencePhysicalType(actual.type).canonical())},{"studio_type_schema",actual.schema}};
            connectors.array.push_back(std::move(connector));
            Json value;if(accumulate&&name=="initialize")value=boolean(false);
            else {value.type=Json::Type::Number;value.string=*token;}
            declaration.object={{"id",string(id)},{"kind",string("constant")},{"type",string(actual.type.canonical())},
                {"value",std::move(value)},{"constant_profile",string(std::string(canonicalComposedProgramProfile))},
                {"studio_ports",std::move(connectors)},{"studio_type_schema",actual.schema}};
            const auto target=targetPath+"/nodes/"+std::to_string(nodeList.array.size());
            nodeList.array.push_back(std::move(declaration));
            auto edge=object(),from=object(),to=object();from.object={{"node",string(id)},{"port",string("value")}};
            to.object={{"node",string(ownerId)},{"port",string(name)}};
            edge.object={{"id",string(edgeId)},{"from",std::move(from)},{"to",std::move(to)}};
            out.identities.push_back({"node",id,at+"/type",target,true});
            out.identities.push_back({"edge",edgeId,at+"/type",targetPath+"/edges/"+std::to_string(edgeList.array.size()),true});
            edgeList.array.push_back(std::move(edge));
        }
    }
    Json scope(const Json &raw,const std::string &path,const std::string &targetPath) {
        Json result=object(),nodeList=array(),edgeList=array();result.object.emplace("profile",string(std::string(canonicalComposedProgramProfile)));
        const auto &ns=arrayAt(raw.member("nodes"),path+"/nodes");
        for(std::size_t i=0;i<ns.size();++i) {
            const auto at=path+"/nodes/"+std::to_string(i);const auto &node=ns[i];
            if(text(node.member("kind"),at+"/kind")=="comment")continue; // validated and attributed by index()
            nodeFields(node,at);
            const auto id=text(node.member("id"),at+"/id"),kind=text(node.member("kind"),at+"/kind");
            if(node.member("constant_owner_node_id"))continue;
            const auto target=targetPath+"/nodes/"+std::to_string(nodeList.array.size());Json item=object();
            item.object={{"id",string(id)},{"kind",string(kind)},{"studio_ports",portList(id)}};
            if(const auto *layout=node.member("layout"))item.object.emplace("layout",*layout);
            out.identities.push_back({"node",id,at,target,false});
            if(kind=="widget_value") {
                const auto widget=text(node.member("widget"),at+"/widget");const auto found=widgetTypes.find(widget);
                if(found==widgetTypes.end())fail(at,"A widget participation needs its actual root declaration.", "migration.composed_source_invalid", Domain::Semantics);
                item.object.emplace("widget",string(widget));item.object.emplace("value_type",string(canonicalObjectReferencePhysicalType(found->second).canonical()));
                item.object.emplace("studio_type_schema",canonicalObjectReferenceWriteType(found->second));
            } else if(kind=="calculus_function_reference") {
                if(!calculusFunctionReferenceDimensions(text(node.member("type"),at)))
                    fail(at,"The model reference must preserve its exact native model identity.","migration.calculus.function_identity_invalid",Domain::Semantics);
                const auto &call=objectAt(node.member("call"),at+"/call");
                item.object["kind"]=string("primitive");item.object["type"]=string(text(node.member("type"),at));
                item.object["studio_authored_type"]=string(text(node.member("type"),at));
                auto config=object();config.object["call"]=call;
                item.object["studio_configuration"]=std::move(config);
            } else if(kind=="fitting_model_reference") {
                if(!nativeNumericalModelReference(text(node.member("type"),at)))
                    fail(at,"The model reference must preserve its exact native model identity.","migration.fitting.model_identity_invalid",Domain::Semantics);
                const auto &call=objectAt(node.member("call"),at+"/call");
                item.object["kind"]=string("primitive");item.object["type"]=string(text(node.member("type"),at));
                item.object["studio_authored_type"]=string(text(node.member("type"),at));
                auto config=object();config.object["call"]=call;
                item.object["studio_configuration"]=std::move(config);
            } else if(kind=="primitive"&&(recordLiteral(node)||scalarLiteral(node)||arrayLiteral(node)||collectionLiteral(node))) {
                const auto &value=literal(id);std::string producer="value";
                if(collectionLiteral(node)) {
                    bool found=false;
                    for(const auto &[key,_]:ports)if(std::get<0>(key)==id&&!std::get<2>(key)) {
                        if(found)fail(at,"A Collection literal has exactly one effective producer.","migration.composed_resolution_invalid", Domain::Semantics);
                        producer=std::get<1>(key);found=true;
                    }
                    const auto primary=value.type.kind==binding::TypeKind::Map?"map":"set";
                    if(!found||(producer!=primary&&producer!="value"))fail(at,"The Collection producer differs from its actual kind or legacy alias.","migration.composed_resolution_invalid", Domain::Integrity);
                }
                const auto &actual=port(id,producer,false);
                if(!canonicalObjectReferenceFullTypeEqual(actual.type,value.type))fail(at,"Literal definition and complete resolved output schema differ.","migration.composed_resolution_invalid", Domain::Integrity);
                item.object["kind"]=string("constant");item.object.emplace("type",string(canonicalObjectReferencePhysicalType(value.type).canonical()));item.object.emplace("value",value.value);
                item.object.emplace("constant_profile",string(std::string(canonicalComposedProgramProfile)));item.object.emplace("studio_type_schema",canonicalObjectReferenceWriteType(value.type));
                item.object.emplace("studio_authored_type",*node.member("type"));componentMappings(id,target+"/value");
                std::vector<const binding::TypeRef*> pending{&value.type};bool references=false;
                while(!pending.empty()){const auto *type=pending.back();pending.pop_back();references=references||type->kind==binding::TypeKind::Reference;for(const auto &child:type->children)pending.push_back(&child);}
                if(references) {
                    CanonicalObjectReferenceDiagnostic diagnostic;const auto descriptor=canonicalObjectReferenceRecordDescriptor(CanonicalObjectReferenceKind::Resolve,value.type,
                        {{"value",canonicalObjectReferencePhysicalType(value.type).canonical(),encode(canonicalObjectReferenceWriteType(value.type)),false,true,{}}},{},&value.value,
                        target+"/"+std::string(canonicalObjectReferenceAttribute)+"/configuration",&diagnostic);
                    if(!descriptor)fail(at,diagnostic.message,"migration.composed_literal_unavailable", diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);
                    item.object["kind"]=string("primitive");item.object["type"]=string(std::string(canonicalObjectReferenceOperation(descriptor->kind)));
                    item.object.erase("value");item.object.erase("constant_profile");item.object.emplace(std::string(canonicalObjectReferenceAttribute),canonicalObjectReferenceAttributes(*descriptor));
                }
            } else if(kind=="primitive") {
                item.object.emplace("type",string(operation(node,at)));item.object.emplace("studio_authored_type",*node.member("type"));Json cfg=object();
                if(!canonicalErrorHandlingMigrationIdentity(text(node.member("type"),at)).empty()||
                   !canonicalFileMigrationIdentity(text(node.member("type"),at)).empty()||
                   !canonicalVariantAnalysisMigrationIdentity(text(node.member("type"),at)).empty()) {
                    // The editor accepts a scalar Error by its Bool/I32/String
                    // roles, including a user's own field IDs. The compiled
                    // Error services have one exact internal Record contract.
                    // Keep the user's declarations untouched; the ordinary FIR
                    // edge builder inserts typed Select/Construct bridges at
                    // these service boundaries instead of relabelling values.
                    const auto error=binding::TypeRef::cluster({"status","code","source"},
                        {binding::TypeRef::primitive("bool"),binding::TypeRef::primitive("i32"),binding::TypeRef::primitive("string")});
                    for(auto &declared:item.object.at("studio_ports").array) {
                        const auto schema=canonicalObjectReferenceReadType(declared.object.at("studio_type_schema"));
                        if(schema&&binding::isErrorCluster(*schema)) {
                            declared.object["type"]=string(error.canonical());
                            declared.object["studio_type_schema"]=canonicalObjectReferenceWriteType(error);
                        }
                    }
                }
                if(!canonicalVariantAnalysisMigrationIdentity(text(node.member("type"),at)).empty()) {
                    // Only the service's own connectors gain their fixed Enum
                    // definitions. User declarations and values stay intact;
                    // the normal FIR edge builder owns typed boundary bridges.
                    for(auto &declared:item.object.at("studio_ports").array) {
                        const auto portId=text(declared.member("id"),at);
                        const bool input=text(declared.member("direction"),at)=="input";
                        const auto exact=canonicalVariantAnalysisInternalPortType(text(node.member("type"),at),portId,input);
                        const auto actual=canonicalObjectReferenceReadType(declared.object.at("studio_type_schema"));
                        if(!exact||!actual||actual->canonical()!=exact->canonical())
                            fail(at,"The actual Data Type Analysis connector differs from its exact current contract.","migration.variant_analysis_signature_invalid", Domain::Semantics);
                        std::vector<std::pair<const binding::TypeRef*,const binding::TypeRef*>> pending{{&*actual,&*exact}};
                        while(!pending.empty()) {
                            const auto [a,e]=pending.back();pending.pop_back();
                            if(a->kind!=e->kind||a->name!=e->name||a->rank!=e->rank||a->extent!=e->extent||a->fieldIds!=e->fieldIds||a->children.size()!=e->children.size()||
                               (a->kind==binding::TypeKind::Enum&&!a->choices.empty()&&*a!=*e))
                                fail(at,"A Data Type Analysis Enum domain/definition or structural connector constraint conflicts with its compiled contract.","migration.variant_analysis_signature_invalid", Domain::Semantics);
                            for(std::size_t i=0;i<a->children.size();++i)pending.push_back({&a->children[i],&e->children[i]});
                        }
                        declared.object["type"]=string(canonicalObjectReferencePhysicalType(*exact).canonical());
                        declared.object["studio_type_schema"]=canonicalObjectReferenceWriteType(*exact);
                    }
                }
                if(text(node.member("type"),at)==kNativeCallDiagramNodeType) {
                    CanonicalNativeCallDiagnostic diagnostic;
                    const auto descriptor=canonicalNativeCallScalarDescriptor(objectAt(node.member("studio_native_call"),at+"/studio_native_call"),
                        target+"/native_call_configuration",&diagnostic);
                    if(!descriptor)fail(at+"/studio_native_call",diagnostic.message,
                        diagnostic.unsupported?"migration.native_call_signature_unavailable":"migration.native_call_configuration_invalid", diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);
                    if(!scopeOwners.at(path).empty())fail(at,"The scalar5 NativeCall corridor requires an actual flat root region.","migration.native_call_region_unavailable", Domain::FrontendCapability);
                    Json activePorts=array();
                    for(const auto &declared:descriptor->ports) {
                        const auto &actual=port(id,declared.id,declared.input);
                        const auto expected=binding::parseType(declared.type);
                        if(!expected.valid()||actual.type!=expected.type)
                            fail(at,"The configured NativeCall port differs from its exact scalar5 signature.","migration.native_call_configuration_invalid", Domain::Semantics);
                        Json entry=object();entry.object={{"id",string(declared.id)},{"direction",string(declared.input?"input":"output")},
                            {"type",string(declared.type)},{"studio_type_schema",actual.schema}};activePorts.array.push_back(std::move(entry));
                        out.identities.push_back({"native_call_port",id+":"+declared.id,at+"/studio_native_call",target+"/native_call_configuration",false});
                    }
                    if(activePorts.array.size()!=item.object.at("studio_ports").array.size())
                        fail(at,"NativeCall contains a connector absent from its derived scalar5 signature.","migration.native_call_configuration_invalid", Domain::Semantics);
                    item.object["studio_ports"]=std::move(activePorts);
                    item.object.emplace("native_call_profile",string(std::string(kNativeCallScalarProfile)));
                    item.object.emplace("native_call_configuration",*source::parseJson(descriptor->configurationJson,nullptr,source::JsonNumberPolicy::PreserveLexeme));
                    out.identities.push_back({"native_call_configuration",id,at+"/studio_native_call",target+"/native_call_configuration",false});
                }
                for(const auto key:{"cluster_function_terminals","cluster_field_names","cluster_field_id_paths","cluster_show_full_names","string_function_terminals","array_function_terminals","retain_first_error_inputs","studio_event_registration_rows","studio_application_control","build_array_inputs","build_array_concatenate","index_array_outputs","array_edit_groups","reshape_array_dimensions","matrix_append_by_rows"})
                    if(const auto *value=node.member(key))cfg.object.emplace(key,*value);
                const auto authored=bare(text(node.member("type"),at));
                if(authored=="array.conversion.array_to_cluster"||authored=="cluster.conversion.array_to_cluster") {
                    // The existing authoring resolver stores Cluster Size in
                    // text and seals its exact fields in the result schema.
                    // Keep that authored configuration beside the derived type.
                    if(const auto *size=node.member("text"))cfg.object.emplace("authored_cluster_size",*size);
                    cfg.object.emplace("resolved_cluster_size",number(port(id,"cluster",false).type.children.size()));
                }
                if(!canonicalFileMigrationIdentity(authored).empty()) {
                    const auto wired=[&](std::string_view portId,bool input) {
                        return std::any_of(edges.begin(),edges.end(),[&](const auto &e){
                            const auto *endpoint=e.raw->member(input?"to":"from");
                            return endpoint&&endpoint->member("node")&&endpoint->member("port")&&
                                endpoint->member("node")->string==id&&endpoint->member("port")->string==portId;
                        });
                    };
                    DiagramNode owner;owner.fileReadLines=node.member("file_read_lines")?flag(node.member("file_read_lines"),at+"/file_read_lines"):false;
                    owner.fileConvertEol=node.member("file_convert_eol")?flag(node.member("file_convert_eol"),at+"/file_convert_eol"):true;
                    const auto configuration=canonicalFileNativeConfiguration(owner,
                        wired(authored=="file.exists"?"path":"file",true),
                        authored=="file.text_read"&&wired("count",true),
                        authored!="file.exists"&&wired("file_ref",false));
                    const auto encoded=canonicalFileNativeConfigurationJson(configuration);
                    const auto parsed=source::parseJson(encoded,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                    if(!parsed)fail(at,"The exact File recipe could not be sealed.","migration.file.configuration_invalid", Domain::Semantics);
                    cfg=*parsed;
                }
                item.object.emplace("studio_configuration",std::move(cfg));
                if(authored=="structures.feedback_node") {
                    const auto *rawDelay=node.member("feedback_delay");
                    const auto delay=rawDelay?ordinal(rawDelay,at+"/feedback_delay"):1U;
                    if(!delay||delay>128)fail(at,"Feedback Delay must be in1..128 for this target.","migration.feedback.delay_invalid", Domain::Semantics);
                    const auto *rawMode=node.member("feedback_initialization");
                    const auto mode=rawMode?text(rawMode,at+"/feedback_initialization"):"compile_or_load";
                    if(mode!="compile_or_load"&&mode!="first_call"&&mode!="loop_entry")fail(at,"Feedback initialization mode is unknown.","migration.feedback.initialization_invalid", Domain::Compatibility);
                    const auto *rawLoop=node.member("feedback_initializer_loop_id");
                    const auto loop=rawLoop?text(rawLoop,at+"/feedback_initializer_loop_id",true):std::string{};
                    const bool history=node.member("feedback_history")?flag(node.member("feedback_history"),at+"/feedback_history"):false;
                    Json feedback=object();feedback.object={{"profile",string("frog.studio.feedback@1")},{"delay",number(delay)},
                        {"initialization",string(mode)},{"initializer_loop_id",string(loop)},{"history",canonical_json::boolean(history)}};
                    item.object.emplace("studio_feedback",std::move(feedback));
                }
                if(const auto collection=canonicalCollectionOperationId(authored);!collection.empty()) {
                    CollectionBuildState build;const bool isBuild=collection=="frog.collection.map.build"||collection=="frog.collection.set.build";
                    if(const auto *saved=node.member("studio_collection_build")) {
                        std::string why;const auto parsed=readCollectionBuildState(*saved,&why);
                        if(!isBuild||!parsed)fail(at,"The Collection Build configuration is not valid for this operation: "+why, "migration.composed_source_invalid", Domain::Semantics);
                        build=*parsed;
                    }
                    const std::vector<std::string> noRows;
                    item.object.emplace("studio_collection",canonicalCollectionConfiguration(isBuild?std::span<const std::string>{build.rows}:std::span<const std::string>{noRows}));
                }
                if(authored=="cluster.bundle_by_name"||authored=="cluster.unbundle_by_name"||authored=="cluster.bundle"||authored=="cluster.unbundle") {
                    const auto descriptor=recordDescriptor(node,id,at,target);
                    item.object["type"]=string(std::string(canonicalObjectReferenceOperation(descriptor.kind)));
                    Json activePorts=array();
                    for(const auto &port:descriptor.ports) {
                        const auto full=source::parseJson(port.fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                        if(!full)fail(at,"The active Record connector lost its complete canonical schema.", "migration.composed_source_invalid", Domain::Semantics);
                        Json entry=object();entry.object={{"id",string(port.id)},{"direction",string(port.input?"input":"output")},{"type",string(port.type)},{"studio_type_schema",*full}};
                        activePorts.array.push_back(std::move(entry));
                    }
                    item.object["studio_ports"]=std::move(activePorts);
                    const auto errorType=binding::parseType(kNativeCallScalarErrorType);
                    const bool nativeErrorSelection=hasNativeCalls()&&descriptor.kind==CanonicalObjectReferenceKind::Select&&
                        errorType.valid()&&port(id,"cluster",true).type==errorType.type;
                    if(nativeErrorSelection) {
                        // A plain Error projection needs no reference host. Reuse
                        // the existing immutable Record selector, keeping the
                        // real Unbundle ports and stable selected field IDs.
                        Json selected=array();std::size_t row=0;
                        for(const auto &declared:descriptor.ports)if(!declared.input) {
                            const auto &field=descriptor.fieldPaths.at(row++);
                            if(field.size()!=1)fail(at,"A scalar5 Error projection selects one actual top-level field.","migration.native_call_error_projection_invalid", Domain::Semantics);
                            Json entry=object();entry.object={{"port",string(declared.id)},{"field_id",string(field.front())}};selected.array.push_back(std::move(entry));
                            out.identities.push_back({"selected_field",id+":"+declared.id,at,target+"/selected_fields/"+std::to_string(row-1)+"/field_id",false});
                        }
                        item.object["type"]=string("frog.core.unbundle.by_name");item.object.emplace("selected_fields_version",number(1));
                        item.object.emplace("selected_fields",std::move(selected));
                    } else {
                        item.object.emplace(std::string(canonicalObjectReferenceAttribute),canonicalObjectReferenceAttributes(descriptor));
                        out.identities.push_back({"object_reference_configuration",id,at,target+"/"+std::string(canonicalObjectReferenceAttribute),false});
                    }
                }
                if(const auto *configuration=node.member("studio_application_control")) {
                    CanonicalObjectReferenceDiagnostic diagnostic;
                    const auto decoded=application_control_source::readConfiguration(*configuration);
                    const bool reference=decoded&&decoded->kind==ApplicationControlKind::Reference;
                    const bool referenceInput=decoded&&applicationControlHasReferenceInput(*decoded);
                    const auto descriptor=canonicalObjectReferenceDescriptor(*configuration,
                        target+"/"+std::string(canonicalObjectReferenceAttribute)+"/configuration",port(id,reference||!referenceInput?"reference_out":"reference_in",referenceInput).type,&diagnostic);
                    if(!descriptor)fail(at+"/studio_application_control",diagnostic.message,
                        diagnostic.unsupported?"migration.composed_operation_unavailable":"migration.composed_source_invalid", diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);
                    item.object["type"]=string(std::string(canonicalObjectReferenceOperation(descriptor->kind)));
                    item.object.emplace(std::string(canonicalObjectReferenceAttribute),canonicalObjectReferenceAttributes(*descriptor));
                    out.identities.push_back({"object_reference_configuration",id,at+"/studio_application_control",target+"/"+std::string(canonicalObjectReferenceAttribute),false});
                }
            } else if(kind=="structure") {
                const auto type=text(node.member("structure_type"),at+"/structure_type");
                if(type!="while_loop"&&type!="timed_loop"&&type!="for_loop"&&type!="case"&&type!="event_structure"&&type!="in_place_element_structure"&&type!="sequence_structure")fail(at,"This structure family has no composed native contract.","migration.composed_structure_unavailable", structureDomain(node));
                item.object.emplace("structure_type",string(type));item.object.emplace("studio_authored_type",*node.member("type"));
                if(type=="timed_loop") {
                    try {
                        const auto schedule=timed_loop::readSchedule(objectAt(node.member("timed_loop_schedule"),at+"/timed_loop_schedule"));
                        const auto options=node.member("studio_timed_loop_options")?timed_loop::readOptions(*node.member("studio_timed_loop_options")):DiagramTimedLoopOptions{};
                        auto config=object();config.object={{"profile",string(std::string(timed_loop::profile))},{"schedule",timed_loop::scheduleJson(schedule)},{"options",timed_loop::optionsJson(options)}};
                        item.object.emplace("studio_timed_loop",std::move(config));
                        out.identities.push_back({"timed_loop_configuration",id,at+"/timed_loop_schedule",target+"/studio_timed_loop",false});
                    }catch(const std::exception &e){fail(at+"/timed_loop_schedule",e.what(),"migration.timed_loop.configuration_invalid",Domain::Semantics);}
                }
                if(type=="sequence_structure") {
                    const auto *declared=node.member("studio_sequence_profile");
                    const auto semantic=declared?text(declared,at+"/studio_sequence_profile"):"flat_sequence";
                    if(semantic!="flat_sequence"&&semantic!="stacked_sequence")fail(at,"The Sequence semantic profile is unknown.","migration.sequence.profile_unavailable", Domain::Compatibility);
                    auto transfers=array();
                    for(const auto &raw:arrayAt(node.member("structure_tunnels"),at+"/structure_tunnels")) {
                        const auto tid=text(raw.member("id"),at);DiagramCaseStructureTunnel t;t.id=tid;
                        t.outputFromStructure=flag(raw.member("output_from_structure"),at);
                        t.sequenceFrameId=text(raw.member("studio_sequence_frame"),at);
                        if(const auto *other=raw.member("studio_sequence_outer_frame"))t.sequenceOuterFrameId=text(other,at);
                        t.useDefaultIfUnwired=raw.member("use_default_if_unwired")&&flag(raw.member("use_default_if_unwired"),at);
                        if(raw.member("shift_register_pair_id")||raw.member("shift_register_history_index")||
                            (raw.member("conditional")&&flag(raw.member("conditional"),at))||
                            (raw.member("tunnel_mode")&&text(raw.member("tunnel_mode"),at)!="last_value"))
                            fail(at,"A Sequence passage is an exact value transfer, not an indexed or stateful tunnel.","migration.sequence.transfer_invalid", Domain::Semantics);
                        auto transfer=object();transfer.object={{"id",string(tid)},
                            {"source_frame",string(std::string(sequenceTunnelInputFrameId(t)))},
                            {"target_frame",string(std::string(sequenceTunnelOutputFrameId(t)))},
                            {"use_default_if_unwired",boolean(t.useDefaultIfUnwired)}};
                        transfers.array.push_back(std::move(transfer));
                    }
                    auto configuration=object();configuration.object={{"profile",string("frog.studio.sequence@1")},
                        {"semantic_profile",string(semantic)},{"transfers",std::move(transfers)}};
                    item.object.emplace("studio_sequence",std::move(configuration));
                    out.identities.push_back({"sequence_configuration",id,at+"/structure_tunnels",target+"/studio_sequence",false});
                }
                if(type=="in_place_element_structure") {
                    const auto *declared=node.member("studio_in_place_profile");
                    if(declared&&(declared->type!=Json::Type::String||declared->string!=canonicalInPlaceProfile))fail(at,"This IPE semantic profile is not supported.","migration.ipe.profile_unavailable", Domain::Compatibility);
                    std::string why;const auto *rawPairs=node.member("studio_in_place_pairs");
                    const auto pairs=rawPairs?canonicalInPlaceReadPairs(*rawPairs,true,&why):std::nullopt;
                    if(!pairs)fail(at,why.empty()?"IPE pair descriptors are required.":why,"migration.ipe.configuration_invalid", Domain::Semantics);
                    item.object.emplace("studio_in_place",canonicalInPlaceConfiguration(*pairs));
                    out.identities.push_back({"in_place_configuration",id,at+"/studio_in_place_pairs",target+"/studio_in_place",false});
                }
                const auto &boundary=objectAt(node.member("boundary"),at+"/boundary");fields(boundary,{"inputs","outputs"},at+"/boundary");
                auto normalizedBoundary=boundary;
                for(const auto side:{"inputs","outputs"})for(auto &entry:normalizedBoundary.object.at(side).array) {
                    const auto *mode=entry.member("tunnel_mode");
                    if(mode&&mode->type==Json::Type::String&&mode->string=="indexing") {
                        const auto tunnel=text(entry.member("id"),at);
                        // The saved hint is presentation state. Actual current
                        // connectors are hash-bound to this same model snapshot.
                        entry.object["type"]=string(port(id,"tunnel:"+tunnel+":input",true).type.canonical());
                    }
                }
                item.object.emplace("boundary",std::move(normalizedBoundary));
                // Historical private IPE writers emitted no intrinsic-terminal
                // object: this structure has border pairs instead. Adapt that
                // absence explicitly without relaxing the other families.
                const auto *authoredTerminals=node.member("structure_terminals");
                auto terminals=(type=="in_place_element_structure"||type=="sequence_structure")&&!authoredTerminals?
                    object():objectAt(authoredTerminals,at+"/structure_terminals");
                if(type=="in_place_element_structure"&&!terminals.object.empty())
                    fail(at+"/structure_terminals","IPE has no intrinsic structure terminals.","migration.ipe.terminal_invalid", Domain::Semantics);
                item.object.emplace("structure_terminals",std::move(terminals));
                if(const auto *tunnels=node.member("structure_tunnels"))item.object.emplace("structure_tunnels",*tunnels);
                if(const auto *profile=node.member("case_profile"))item.object.emplace("case_profile",*profile);
                Json regions=array(),eventCases=array();const auto &authored=arrayAt(node.member("regions"),at+"/regions");
                for(std::size_t r=0;r<authored.size();++r) {
                    const auto rp=at+"/regions/"+std::to_string(r),tp=target+"/regions/"+std::to_string(r);const auto &region=objectAt(&authored[r],rp);
                    authoredRegionFields(region,rp);
                    if(region.member("draft_text")||(region.member("draft")&&flag(region.member("draft"),rp+"/draft")))fail(rp,"Unapplied Case text cannot execute.", "migration.composed_source_invalid", Domain::Semantics);
                    Json converted=object(),eventCase=object();const auto rid=text(region.member("id"),rp+"/id");converted.object.emplace("id",string(rid));eventCase.object.emplace("id",string(rid));
                    for(const auto key:{"match","matches","default","event","event_data_fields","event_payload_fields","event_response_fields"})
                        if(const auto *value=region.member(key)){converted.object.emplace(key,*value);if(std::string_view(key).starts_with("event"))eventCase.object.emplace(key,*value);}
                    if(type=="case")if(const auto *legacyMatch=region.member("match")) {
                        // The common Studio writer retains Boolean Case labels
                        // in its historical compact form. Normalize only that
                        // known representation into the explicit FIR profile.
                        if(legacyMatch->type!=Json::Type::Boolean||region.member("matches"))
                            fail(rp,"A compact Boolean Case match cannot conflict with an explicit match list.","migration.case_match_ambiguous", Domain::Semantics);
                        auto matcher=object(),matches=array();
                        matcher.object={{"kind",string("boolean_exact")},{"value",*legacyMatch}};
                        matches.array.push_back(std::move(matcher));converted.object.erase("match");
                        converted.object["matches"]=std::move(matches);
                    }
                    converted.object.emplace("diagram",scope(objectAt(region.member("diagram"),rp+"/diagram"),rp+"/diagram",tp+"/diagram"));
                    out.identities.push_back({"region",std::to_string(id.size())+":"+id+":"+rid,rp,tp,false});
                    regions.array.push_back(std::move(converted));if(type=="event_structure")eventCases.array.push_back(std::move(eventCase));
                }
                item.object.emplace("regions",std::move(regions));
                if(type=="event_structure") {Json event=object();event.object={{"profile",string(std::string(canonicalComposedProgramProfile))},{"cases",std::move(eventCases)}};
                    for(const auto &[key,value]:node.object)if(key.starts_with("studio_event_")&&!key.ends_with("_visible")&&!key.ends_with("_normalized_y"))event.object.emplace(key,value);
                    item.object.emplace("studio_event",std::move(event));}
            } else fail(at,"This authored node family has no composed-program implementation contract.","migration.composed_node_unavailable", (kind=="frog_call"||kind=="static_visual"||kind=="conditional_default")?Domain::FrontendCapability:Domain::Compatibility);
            nodeList.array.push_back(std::move(item));
            if(kind=="primitive"&&node.member("type")&&
                (!canonicalNumericRandomMigrationIdentity(text(node.member("type"),at)).empty()||
                 !canonicalNumericAccumulateMigrationIdentity(text(node.member("type"),at)).empty()))
                materializeStatefulNumericDefaults(node,at,targetPath,nodeList,edgeList);
        }
        const auto &es=arrayAt(raw.member("edges"),path+"/edges");
        for(std::size_t i=0;i<es.size();++i) {
            const auto at=path+"/edges/"+std::to_string(i),id=text(es[i].member("id"),at+"/id");
            auto edge=es[i];
            auto &to=edge.object.at("to");const auto &target=nodes.at(to.member("node")->string);
            if(const auto *type=target.raw->member("type");type&&type->type==Json::Type::String&&bare(type->string)=="array.build"&&to.member("port")->string=="input_2")
                to.object["port"]=string("array_b");
            for(const auto side:{"from","to"}) {
                auto &endpoint=edge.object.at(side);const auto &owner=nodes.at(endpoint.member("node")->string);
                if(const auto *authored=owner.raw->member("type");authored&&authored->type==Json::Type::String)
                    endpoint.object["port"]=string(std::string(canonicalNumericBooleanConnectorId(authored->string,endpoint.member("port")->string)));
            }
            // The complete label remains in authored-source provenance. Public
            // executable wiring retains only its technical identity/geometry.
            if(edge.object.erase("label"))
                out.identities.push_back({"presentation",id+":label",at+"/label",std::string(draft_presentation::provenance)+at+"/label",false});
            edgeList.array.push_back(std::move(edge));
            out.identities.push_back({"edge",id,at,targetPath+"/edges/"+std::to_string(edgeList.array.size()-1),false});
        }
        result.object.emplace("nodes",std::move(nodeList));result.object.emplace("edges",std::move(edgeList));result.object.emplace("wire_fragments",array());return result;
    }
};
}
bool canonicalDraftNeedsComposition(const Json &root,const CanonicalDraftResolvedTypes *resolved) {
    // Routing only: a real independently declared interface, explicit widget
    // IDs/directions/types and active typed terminals request the existing
    // complete-port pass. The composed adapter still checks every full schema
    // against the hash-bound witness; a visual map alone never selects this.
    if(resolved)if(const auto *declaration=root.member("interface");declaration&&declaration->type==Json::Type::Object) {
        const auto *panel=root.member("frontPanel"),*diagram=root.member("diagram");
        const auto *widgets=panel?panel->member("widgets"):nullptr;
        const auto *inputs=declaration->member("inputs"),*outputs=declaration->member("outputs");
        if(widgets&&widgets->type==Json::Type::Array&&diagram&&inputs&&outputs&&
           inputs->type==Json::Type::Array&&outputs->type==Json::Type::Array&&(!inputs->array.empty()||!outputs->array.empty())) {
            std::set<std::string> declaredIds;bool complete=true;
            const auto terminal=[&](const auto &self,const Json &scope,std::string_view widget,unsigned depth)->bool {
                if(depth>32)return false;const auto *nodes=scope.member("nodes");if(!nodes||nodes->type!=Json::Type::Array)return false;
                for(const auto &node:nodes->array) {
                    if(node.member("kind")&&node.member("kind")->type==Json::Type::String&&node.member("kind")->string=="widget_value"&&
                       node.member("widget")&&node.member("widget")->type==Json::Type::String&&node.member("widget")->string==widget)return true;
                    if(const auto *regions=node.member("regions");regions&&regions->type==Json::Type::Array)
                        for(const auto &region:regions->array)if(const auto *body=region.member("diagram");body&&self(self,*body,widget,depth+1))return true;
                }
                return false;
            };
            for(const bool input:{true,false})for(const auto &port:(input?inputs:outputs)->array) {
                const auto *id=port.member("id"),*type=port.member("type");
                if(!id||id->type!=Json::Type::String||id->string.empty()||!declaredIds.insert(id->string).second||!type||type->type!=Json::Type::String){complete=false;continue;}
                const auto declared=binding::parseType(type->string);unsigned matches=0;
                if(!declared.valid()||!declared.type.known()){complete=false;continue;}
                for(const auto &widget:widgets->array) {
                    const auto *bindingConfig=widget.member("binding"),*role=widget.member("role"),*wid=widget.member("id"),*spelling=widget.member("valueType");
                    const auto *bound=bindingConfig?bindingConfig->member(input?"public_input_id":"public_output_id"):nullptr;
                    if(!bound||bound->type!=Json::Type::String||bound->string!=id->string)continue;
                    const auto actual=spelling&&spelling->type==Json::Type::String?binding::parseType(spelling->string):binding::TypeParseResult{};
                    if(!role||role->type!=Json::Type::String||role->string!=(input?"control":"indicator")||!wid||wid->type!=Json::Type::String||
                       !actual.valid()||!actual.type.known()||actual.type.canonical()!=declared.type.canonical()||!terminal(terminal,*diagram,wid->string,0)){complete=false;continue;}
                    if(input) {
                        const auto *policy=bindingConfig->member("connection_requirement"),*declaredPolicy=port.member("connection");
                        const auto expected=declaredPolicy&&declaredPolicy->type==Json::Type::String?declaredPolicy->string:"recommended";
                        if(policy&&(policy->type!=Json::Type::String||policy->string!=expected)){complete=false;continue;}
                    }
                    ++matches;
                }
                if(matches!=1)complete=false;
            }
            if(complete)return true;
        }
    }

    // Complete, hash-bound connector evidence was requested by the common
    // resolver when it found a real scalar NumericImplicit edge. This only
    // selects the composed adapter; FIR still verifies the receiver policy,
    // scopes, exact schemas and the derived conversion support.
    if(resolved) {
        // The immutable port vector owns these borrowed keys until return.
        std::map<std::pair<std::string_view,std::string_view>,const CanonicalDraftPortResolution*> outputs;
        for(const auto &port:resolved->ports)if(!port.input)outputs.emplace(std::pair{std::string_view(port.nodeId),std::string_view(port.portId)},&port);
        for(const auto &input:resolved->ports)if(input.input&&!input.sourceNodeId.empty()) {
            const auto output=outputs.find({input.sourceNodeId,input.sourcePortId});
            if(output==outputs.end())continue;
            const auto sourceSchema=source::parseJson(output->second->fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            const auto targetSchema=source::parseJson(input.fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            const auto sourceType=sourceSchema?canonicalObjectReferenceReadType(*sourceSchema):std::nullopt;
            const auto targetType=targetSchema?canonicalObjectReferenceReadType(*targetSchema):std::nullopt;
            if(sourceType&&targetType&&canonicalNumericScalarCoercionSupported(*sourceType,*targetType)&&
                binding::connect(*sourceType,*targetType,binding::ConnectionPolicy::NumericImplicit).status==binding::ResolutionStatus::Coercion)return true;
        }
    }
    if(resolved)for(const auto &scalar:resolved->scalars)
        if(comparison_source::definition(scalar.authoredOperation)) {
            // The first pass has actual source types, not promoted targets.
            // It selects the existing complete-port pass; no value is inferred.
            std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &operand:scalar.operands) {
                const auto type=binding::parseType(operand.valueType);
                signature.push_back({operand.inputPortId,type.valid()?type.type:binding::TypeRef{},true,
                    operand.accepted&&operand.complete&&!operand.sourceNodeId.empty()&&!operand.sourcePortId.empty()});
            }
            if(canonicalComparisonTreeShouldUse(scalar.authoredOperation,signature))return true;
        }
    if(resolved)for(const auto &scalar:resolved->scalars)
        if(!canonicalBooleanTreeMigrationIdentity(scalar.authoredOperation).empty())
            for(const auto &operand:scalar.operands)
                if(!operand.valueType.empty()&&operand.valueType!="bool")return true;
    if(resolved)for(const auto &scalar:resolved->scalars)
        if(!canonicalNumericAggregateMigrationIdentity(scalar.authoredOperation).empty()||
           !canonicalNumericScalarPaletteMigrationIdentity(scalar.authoredOperation).empty()) {
            // The first pass has only actual operand evidence; request the
            // common complete-port resolver before choosing a concrete service.
            // This is routing, not admission: the adapter validates every full
            // schema, source endpoint and result after resolution.
            std::string_view first;
            for(const auto &operand:scalar.operands) {
                const auto type=std::string_view(operand.valueType);
                if(operand.accepted&&operand.complete&&!type.empty()) {
                    if((type!="i32"&&type!="u32"&&type!="i64"&&type!="u64"&&type!="f64")||
                       (!first.empty()&&type!=first))return true;
                    first=type;
                }
            }
            std::vector<CanonicalNumericAggregatePort> signature;
            for(const auto &port:resolved->ports)if(port.nodeId==scalar.nodeId) {
                const auto raw=source::parseJson(port.fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                const auto type=raw?canonicalObjectReferenceReadType(*raw):std::nullopt;
                if(type)signature.push_back({port.portId,*type,port.input,!port.input||!port.sourceNodeId.empty()});
            }
            std::stable_sort(signature.begin(),signature.end(),[](const auto &a,const auto &b){
                if(a.input!=b.input)return a.input;
                return canonicalNumericAddInputOrdinal(a.id).value_or(16)<canonicalNumericAddInputOrdinal(b.id).value_or(16);
            });
            if(canonicalNumericAggregateShouldUse(signature))return true;
            if(canonicalNumericScalarPaletteShouldUse(scalar.authoredOperation,signature))return true;
        }
    if(const auto *panel=root.member("frontPanel"))if(const auto *widgets=panel->member("widgets");widgets&&widgets->type==Json::Type::Array)
        for(const auto &widget:widgets->array) {
            const auto *klass=widget.member("classId");
            if(!klass||klass->type!=Json::Type::String)continue;
            if(klass->string.find("cluster_")!=klass->string.npos||klass->string=="frog.studio.widgets.tab_control"||
               klass->string=="frog.studio.widgets.tab_indicator"||klass->string=="frog.widgets.array_control"||
               klass->string=="frog.widgets.array_indicator"||klass->string==xy::classId||klass->string==graph::classId)return true;
            const auto *declared=widget.member("valueType");
            if(!declared||declared->type!=Json::Type::String)continue;
            if(klass->string=="frog.studio.widgets.reference") {
                const auto type=binding::parseType(declared->string);
                if(type.valid()&&type.type.known()&&configuredTypedNullResourceWidget(widget,type.type,"/frontPanel/widgets"))
                    return true;
            }
            // Route the existing composed widget families, not arbitrary
            // unsupported scalars. Full schemas, values and ports remain
            // validated by that adapter and by native/UI admission.
            if(klass->string=="frog.widgets.ring_control"||klass->string=="frog.widgets.ring_indicator"||
               klass->string=="frog.widgets.variant_control"||klass->string=="frog.widgets.variant_indicator"||
               klass->string=="frog.widgets.enum_control"||klass->string=="frog.widgets.enum_indicator"||
               (klass->string=="frog.widgets.string_control"&&declared->string=="string")||
               (klass->string=="frog.widgets.path_control"&&declared->string=="path")||
               ((klass->string=="frog.widgets.numeric_control"||klass->string=="frog.widgets.numeric_indicator")&&
                (declared->string=="c32"||declared->string=="c64"||declared->string=="f32"||declared->string=="i4"||declared->string=="u4"||declared->string=="i8"||declared->string=="u8"||
                 declared->string=="i16"||declared->string=="u16"||declared->string=="timestamp")))return true;
        }
    const auto *diagram=root.member("diagram");return diagram&&requiresScope(*diagram,0);
}

std::vector<CanonicalDraftMigrationDiagnostic> validateCanonicalSubFrogProjectionSource(
    std::string_view bytes,FrogDraftDataView authored,bool typeDefinitionsValidated) {
    CanonicalDraftMigrationResult report;
    try {
        if(bytes.size()>8U*1024U*1024U)fail("/","Source bytes exceed the existing preparation contract.","migration.composed_resource_limit", Domain::Control);
        const auto parsed=source::parseJson(bytes,nullptr,source::JsonNumberPolicy::PreserveLexeme);
        if(!parsed)fail("/","The exact authored source is invalid JSON.", "migration.composed_source_invalid", Domain::Integrity);
        const auto &root=objectAt(&*parsed,"/");authoredRootFields(root);
        if(text(root.member("format"),"/format")!="frog.document.draft")fail("/format","Sub-FROG projection requires identified private Studio source.", "migration.composed_source_invalid", Domain::Compatibility);
        const auto revision=ordinal(root.member("draft_revision"),"/draft_revision");
        if(revision!=2&&revision!=4&&revision!=5&&revision!=6&&revision!=7&&!(revision==8&&typeDefinitionsValidated))
            fail("/draft_revision","This revision needs the existing exact-source Type Definition resolution handoff.","migration.composed_type_definition_resolution_required", Domain::Dependency);
        if(const auto *metadata=root.member("metadata"))objectAt(metadata,"/metadata");
        if(const auto *policy=root.member("execution_policy"))
            fields(*policy,{"version","debugging_allowed","priority","reentrancy","execution_system","automatic_error_handling","suspend_when_called","clear_indicators_when_called"},"/execution_policy");
        if(const auto *native=root.member("native_options")) {
            fields(*native,{"version","target_id","backend_id","optimization_profile"},"/native_options");
            if(ordinal(native->member("version"),"/native_options/version")!=1)fail("/native_options/version","Native launch configuration requires version1.", "migration.composed_source_invalid", Domain::Compatibility);
            for(const auto name:{"target_id","backend_id","optimization_profile"}) {
                const auto value=text(native->member(name),std::string("/native_options/")+name);
                if(value.size()>1024||value.find('\0')!=value.npos)fail(std::string("/native_options/")+name,"Native launch choices must remain bounded exact strings.", "migration.composed_source_invalid", Domain::Integrity);
            }
        }
        if(const auto *defaults=root.member("text_defaults")) {
            fields(*defaults,{"point_size","font_family"},"/text_defaults");
            const auto *size=defaults->member("point_size");
            if(!size||!typedScalar(*size,"f64")||!validDocumentTextPointSize(std::stod(size->string)))fail("/text_defaults/point_size","Document typography requires its actual finite6..144pt size.", "migration.composed_source_invalid", Domain::Semantics);
            if(const auto *family=defaults->member("font_family");family&&!validDocumentTextFontFamily(text(family,"/text_defaults/font_family")))fail("/text_defaults/font_family","Document typography requires its actual bounded UTF-8 family.", "migration.composed_source_invalid", Domain::Semantics);
        }

        if(const auto *icon=root.member("icon"))draft_presentation::icon(*icon,[](auto path,auto message){fail(std::move(path),std::move(message),"migration.presentation_invalid", Domain::Compatibility);});
        if(const auto *map=root.member("interface_map"))if(const auto issue=private_interface_map::validate(*map,"/interface_map"))
            fail(issue->path,issue->message,std::string(private_interface_map::diagnosticCode(issue->kind,canonicalDraftNeedsComposition(root))), issue->kind==private_interface_map::IssueKind::UnknownField?Domain::Compatibility:Domain::Integrity);
        CanonicalDraftResolvedTypes unused;Adapter adapter{report,root,unused};
        const auto &diagram=objectAt(root.member("diagram"),"/diagram");
        std::set<std::string> activeNodes,activeEdges;
        for(const auto &node:authored.diagramNodes)activeNodes.insert(node.id);
        for(const auto &edge:authored.diagramEdges)activeEdges.insert(edge.id);
        const auto visit=[&](const auto &self,const Json &scope,const std::string &path,unsigned depth)->void {
            if(depth>32)fail(path,"Explicit subdiagram nesting exceeds32.","migration.composed_resource_limit", Domain::Control);
            const auto &nodes=arrayAt(scope.member("nodes"),path+"/nodes");
            const auto active=[&](const auto &scan,const Json &value)->bool {
                if(value.type==Json::Type::Object) {
                    if(const auto *id=value.member("id");id&&id->type==Json::Type::String&&activeNodes.contains(id->string))return true;
                    for(const auto &[_,child]:value.object)if(scan(scan,child))return true;
                } else if(value.type==Json::Type::Array)for(const auto &child:value.array)if(scan(scan,child))return true;
                return false;
            };
            if(path=="/diagram"||active(active,scope))fields(scope,{"nodes","edges","wire_fragments","zoom_percent","viewport_origin"},path);
            for(std::size_t i=0;i<nodes.size();++i) {
                const auto at=path+"/nodes/"+std::to_string(i);const auto &node=objectAt(&nodes[i],at);const auto id=text(node.member("id"),at+"/id");
                if(activeNodes.contains(id)) {
                    if(text(node.member("kind"),at+"/kind")=="comment")
                        draft_presentation::comment(node,scope,at,[](auto p,auto m){fail(std::move(p),std::move(m),"migration.presentation_invalid", Domain::Compatibility);});
                    else adapter.nodes.emplace(id,Node{&node,at,path});
                }
                if(const auto *regions=node.member("regions"))for(std::size_t r=0;r<arrayAt(regions,at+"/regions").size();++r)
                    if(const auto *body=regions->array[r].member("diagram"))self(self,*body,at+"/regions/"+std::to_string(r)+"/diagram",depth+1);
            }
            const auto &edges=arrayAt(scope.member("edges"),path+"/edges");
            for(std::size_t e=0;e<edges.size();++e)if(activeEdges.contains(text(edges[e].member("id"),path)))
                fields(edges[e],{"id","from","to","bends","label"},path+"/edges/"+std::to_string(e));
        };
        visit(visit,diagram,"/diagram",0);
        const auto &panel=objectAt(root.member("frontPanel"),"/frontPanel");fields(panel,{"canvas","grid","widgets"},"/frontPanel");
        const auto &widgets=arrayAt(panel.member("widgets"),"/frontPanel/widgets");
        if(widgets.size()>1024)fail("/frontPanel/widgets","The existing widget budget was exceeded.","migration.composed_resource_limit", Domain::Control);
        std::set<std::string> ownedLinkPaths;
        const auto ownedLink=[&](const Json &value,const TypeDefinitionLink *owner,const std::string &at) {
            if(const auto *link=value.member("type_definition")) {
                const auto path=at+"/type_definition";
                fields(*link,{"definition_id","source_path","resolved_revision","schema_fingerprint","baseline_widget","unresolved_reason","icon_svg"},path);
                if(!typeDefinitionsValidated)
                    fail(path,"An execution-link deferral needs accepted exact-source Type Definition resolution.","migration.composed_type_definition_resolution_required", Domain::Dependency);
                std::string error;const auto decoded=deserializeTypeDefinitionLink(encode(*link),&error);
                if(!decoded)fail(path,error, "migration.composed_source_invalid", Domain::Compatibility);
                if(!owner||*owner!=*decoded)
                    fail(path,"The retained link is not the parsed type_definition of this actual owner.","migration.composed_type_definition_owner_invalid", Domain::Integrity);
                ownedLinkPaths.insert(path);
            }
        };
        for(const auto &[id,node]:adapter.nodes) {
            const auto live=std::find_if(authored.diagramNodes.begin(),authored.diagramNodes.end(),[&](const auto &n){return n.id==id;});
            ownedLink(*node.raw,live!=authored.diagramNodes.end()&&live->typeDefinition?&*live->typeDefinition:nullptr,node.path);
        }
        const auto widgetLinks=[&](const auto &self,const Json &widget,const WidgetInstance &live,const std::string &at,unsigned depth)->void {
            if(depth>32)fail(at,"Widget ownership exceeds32.", "migration.composed_source_invalid", Domain::Control);
            const auto *props=widget.member("props");
            if(props)ownedLink(*props,live.typeDefinition?&*live.typeDefinition:nullptr,at+"/props");
            const auto *parts=props?props->member("cluster.elements"):nullptr;
            if(parts&&parts->type==Json::Type::Array)for(std::size_t i=0;i<parts->array.size();++i) {
                const auto &part=parts->array[i];const auto field=text(part.member("field_id"),at);
                const auto *child=part.member("widget");if(!child)continue;
                const WidgetInstance *actual=nullptr;
                if(live.clusterContainer)for(const auto &candidate:live.clusterContainer->elements)
                    if(candidate.fieldId==field&&candidate.widget){actual=candidate.widget.get();break;}
                if(actual)self(self,*child,*actual,at+"/props/cluster.elements/"+std::to_string(i)+"/widget",depth+1);
            }
        };
        for(std::size_t i=0;i<widgets.size();++i) {
            const auto at="/frontPanel/widgets/"+std::to_string(i);const auto id=text(widgets[i].member("id"),at+"/id");
            if(const auto *live=authored.canvas.widgetById(id))widgetLinks(widgetLinks,widgets[i],*live,at,0);
        }
        const auto allLinkFields=[&](const auto &self,const Json &value,const std::string &at,bool skipNodeRegions)->void {
            if(value.type==Json::Type::Object) {
                if(const auto *link=value.member("type_definition")) {
                    const auto path=at+"/type_definition";
                    fields(*link,{"definition_id","source_path","resolved_revision","schema_fingerprint","baseline_widget","unresolved_reason","icon_svg"},path);
                    if(!ownedLinkPaths.contains(path))
                        fail(path,"A type_definition outside its actual parsed node/widget owner cannot be deferred.","migration.composed_type_definition_owner_invalid", Domain::Integrity);
                }
                for(const auto &[key,child]:value.object) {
                    // Every active child node is checked independently. An
                    // excluded compile-time region is retained, not admitted.
                    if(skipNodeRegions&&key=="regions")continue;
                    self(self,child,pointer(at,key),false);
                }
            } else if(value.type==Json::Type::Array)for(std::size_t i=0;i<value.array.size();++i)self(self,value.array[i],at+"/"+std::to_string(i),false);
        };
        for(const auto &[_,node]:adapter.nodes)allLinkFields(allLinkFields,*node.raw,node.path,true);
        allLinkFields(allLinkFields,panel,"/frontPanel",false);
        const auto linkFields=[&](Json &value,const std::string &at) {
            if(value.member("type_definition")) {
                if(!ownedLinkPaths.contains(at+"/type_definition"))
                    fail(at+"/type_definition","Only an attested actual owner link may be removed in this validation copy.","migration.composed_type_definition_owner_invalid", Domain::Integrity);
                value.object.erase("type_definition");
            }
        };
        CanonicalDraftResolvedTypes widgetResolutions;
        widgetResolutions.originalSha256=hashing::sha256Hex(bytes);
        std::vector<std::string_view> legacyWidgetIds;
        for(const auto &widget:widgets)if(const auto *legacy=widget.member("class_ref");legacy&&legacy->type==Json::Type::String&&legacy->string=="frog.widgets.numeric_control")
            if(const auto *id=widget.member("id");id&&id->type==Json::Type::String)legacyWidgetIds.push_back(id->string);
        if(!legacyWidgetIds.empty())for(const auto &d:appendCanonicalDraftWidgetResolutions(authored,widgetResolutions,legacyWidgetIds))if(d.blocking)
            fail(d.path,d.message,d.code,d.domain);
        std::set<std::string> unvalidatedWidgets;
        for(std::size_t i=0;i<widgets.size();++i) {
            try {
            const auto at="/frontPanel/widgets/"+std::to_string(i);auto widget=objectAt(&widgets[i],at);linkFields(widget,at);
            const auto normalization=normalizeCanonicalDraftWidget(widget,at,&widgetResolutions);
            for(const auto &d:normalization.diagnostics)if(d.blocking)fail(d.path,d.message,d.code,d.domain);
            if(normalization.current)widget=*normalization.current;
            authoredWidgetFields(widget,at);adapter.identifyWidgetTree(widget,at);
            const auto id=text(widget.member("id"),at+"/id");const auto *live=authored.canvas.widgetById(id);
            if(!live)fail(at,"The exact authored widget has no matching model identity.", "migration.composed_source_invalid", Domain::Integrity);
            const auto schema=binding::widgetType(*live);
            if(!schema.valid()||!schema.type.known())fail(at,"The actual widget's complete type is unresolved.", "migration.composed_source_invalid", Domain::Semantics);
            adapter.widgetTypes.emplace(id,schema.type);
            // The ordinary panel adapter retains arbitrary property bags in its
            // studio_widget provenance. Do not reinterpret or erase those bags.
            static_cast<void>(widgetProperties(widget,at));
            } catch(const Failure &failure) {
                report.diagnostics.push_back(failure.diagnostic);
                if(const auto *id=widgets[i].member("id");id&&id->type==Json::Type::String)unvalidatedWidgets.insert(id->string);
            }
        }
        for(const auto &[id,entry]:adapter.nodes) {
            try {
            auto node=*entry.raw;linkFields(node,entry.path);
            const auto *kind=node.member("kind"),*type=node.member("type");
            if(kind&&kind->type==Json::Type::String&&kind->string=="widget_value")
                if(const auto *widget=node.member("widget");widget&&widget->type==Json::Type::String&&unvalidatedWidgets.contains(widget->string))
                    continue; // Upstream schema failure leaves this terminal unchecked, not corrupt.
            if(kind&&kind->type==Json::Type::String&&kind->string=="frog_call") {
                // The actual Studio writer persists kind+call, without type.
                // A present type is an optional redundant owner assertion only.
                if((type&&(type->type!=Json::Type::String||type->string!="frog.studio.frog_call"))||!node.member("call"))
                    fail(entry.path,"Only an actual Studio FROG-call owner may carry the closed call payload.","subfrog.interface.invalid", Domain::Semantics);
                // Only this owner/key is deferred to the projector's complete
                // declaration, signature, binding and dependency validation.
                node.object.erase("call");
            }
            adapter.nodeFields(node,entry.path);
            if(const auto *regions=node.member("regions"))for(std::size_t r=0;r<arrayAt(regions,entry.path+"/regions").size();++r)
                authoredRegionFields(regions->array[r],entry.path+"/regions/"+std::to_string(r));
            } catch(const Failure &failure) {report.diagnostics.push_back(failure.diagnostic);}
        }
    } catch(const Failure &failure) {report.diagnostics.push_back(failure.diagnostic);}
    catch(const std::bad_alloc &) {report.diagnostics.push_back({"migration.composed_resource_limit","/","Not enough memory to inspect this operation.",true,Domain::Control});}
    catch(const std::exception &failure) {report.diagnostics.push_back({"migration.composed_source_invalid","/",failure.what(),true,Domain::Control});}
    return report.diagnostics;
}

CanonicalDraftMigrationResult prepareCanonicalComposedDraftMigration(std::string_view bytes,std::string_view uri,const CanonicalDraftResolvedTypes *resolved) {
    CanonicalDraftMigrationResult result;result.originalSource=std::make_shared<const std::string>(bytes);result.originalUri=uri;result.originalSha256=hashing::sha256Hex(bytes);
    try {
        if(bytes.size()>8U*1024U*1024U||uri.empty()||uri.size()>8192||uri.find('#')!=uri.npos)fail("/","Source bytes and identity exceed the existing preparation contract.", "migration.composed_source_invalid", Domain::Integrity);
        const auto parsed=source::parseJson(bytes,nullptr,source::JsonNumberPolicy::PreserveLexeme);
        if(!parsed)fail("/","The original source is invalid JSON.", "migration.composed_source_invalid", Domain::Integrity);
        const auto &root=objectAt(&*parsed,"/");
        authoredRootFields(root);
        if(text(root.member("format"),"/format")!="frog.document.draft")fail("/format","This adapter requires identified Studio source.", "migration.composed_source_invalid", Domain::Compatibility);
        const auto revision=ordinal(root.member("draft_revision"),"/draft_revision");
        if(revision!=2&&revision!=4&&revision!=5&&revision!=6&&revision!=7)fail("/draft_revision","The private writer revision is not identified.", "migration.composed_source_invalid", Domain::Compatibility);
        const auto *metadata=root.member("metadata");
        if(metadata&&metadata->type!=Json::Type::Object)
            fail("/metadata","File metadata must be an object.","migration.metadata_invalid", Domain::Integrity);
        if(!resolved)fail("/diagram","Composed program preparation needs the common authored type resolver.","migration.composed_resolution_required", Domain::Control);
        if(const auto *map=root.member("interface_map")) {
            if(const auto issue=private_interface_map::validate(*map,"/interface_map"))
                fail(issue->path,issue->message,std::string(private_interface_map::diagnosticCode(issue->kind,true)), issue->kind==private_interface_map::IssueKind::UnknownField?Domain::Compatibility:Domain::Integrity);
            result.diagnostics.push_back({"migration.composed_interface_map_preserved","/interface_map",
                "Retained the validated private presentation map in the exact authored-source provenance; no public ports, widget associations or executable boundary bindings were inferred.",false});
        }
        if(const auto *defaults=root.member("text_defaults")) {
            fields(*defaults,{"point_size","font_family"},"/text_defaults");
            const auto *size=defaults->member("point_size");
            if(!size||!typedScalar(*size,"f64")||!validDocumentTextPointSize(std::stod(size->string)))fail("/text_defaults/point_size","Document typography needs its actual finite6..144pt size.", "migration.composed_source_invalid", Domain::Semantics);
            if(const auto *family=defaults->member("font_family");family&&!validDocumentTextFontFamily(text(family,"/text_defaults/font_family")))fail("/text_defaults/font_family","Document typography needs its actual bounded UTF-8 family.", "migration.composed_source_invalid", Domain::Semantics);
        }
        if(const auto *icon=root.member("icon"))draft_presentation::icon(*icon,[](auto path,auto message){
            fail(std::move(path),std::move(message),"migration.presentation_invalid", Domain::Compatibility);
        });
        Adapter adapter{result,root,*resolved};adapter.index(objectAt(root.member("diagram"),"/diagram"),"/diagram");adapter.resolutions();
        Json output=object();output.object={{"spec_version",string("0.1")},{"metadata",metadata?*metadata:object()}};
        // Validate the common hash-bound widget projection before using it as
        // type evidence. No interface ID is associated to a widget by its label,
        // order, role, or legacy spelling; declarations remain declarations.
        auto panel=adapter.panel();
        Json declaration=object();declaration.object={{"inputs",array()},{"outputs",array()}};
        if(const auto *raw=root.member("interface");raw&&raw->type!=Json::Type::Null) {
            fields(*raw,{"inputs","outputs","map"},"/interface");declaration=*raw;
            std::set<std::string> ids;
            for(const auto side:{"inputs","outputs"}) {
                const auto path=std::string("/interface/")+side;const auto &entries=arrayAt(raw->member(side),path);
                for(std::size_t index=0;index<entries.size();++index) {
                    const auto at=path+"/"+std::to_string(index);const auto &entry=objectAt(&entries[index],at);
                    const auto id=text(entry.member("id"),at+"/id"),spelling=text(entry.member("type"),at+"/type");
                    const auto parsedType=binding::parseType(spelling);
                    if(!ids.insert(id).second)fail(at+"/id","Declared interface IDs must remain unique; no orphan binding is invented.", "migration.composed_source_invalid", Domain::Integrity);
                    if(!parsedType.valid())fail(at+"/type","The declared interface type has invalid syntax; its original declaration remains retained.", "migration.composed_source_invalid", Domain::Semantics);
                    auto resolvedType=parsedType.type;
                    if(!resolvedType.known() && !parsedType.legacy)fail(at+"/type","The declared interface type is unresolved.", "migration.composed_source_invalid", Domain::Semantics);
                    if(!resolvedType.known()) {
                        // The historical display spelling has no field IDs.
                        // Admit this one known Error profile only when the same
                        // document supplies a complete attested standard schema.
                        // Positional fields and interface bindings are never guessed.
                        const auto expected=binding::TypeRef::cluster({"status","code","source"},
                            {binding::TypeRef::primitive("bool"),binding::TypeRef::primitive("i32"),binding::TypeRef::primitive("string")});
                        bool attested=false,ambiguous=false;
                        if(parsedType.legacy && spelling=="cluster:bool|i32|string")
                            for(const auto &widget:arrayAt(panel.member("widgets"),"/front_panel/widgets")) {
                                const auto *authoring=widget.member("studio_widget");const auto *marker=authoring?authoring->member("valueType"):nullptr;
                                if(!marker || marker->type!=Json::Type::String || marker->string!=spelling)continue;
                                const auto *full=widget.member("studio_type_schema");const auto schema=full?canonicalObjectReferenceReadType(*full):std::nullopt;
                                if(!schema || *schema!=expected)ambiguous=true;
                                else {attested=true;resolvedType=*schema;}
                            }
                        if(!attested || ambiguous || !resolvedType.known())
                            fail(at+"/type","This valid historical interface declaration has no unambiguous complete field-identity schema for the canonical boundary. The private declaration is retained; no executable binding is invented.",
                                "migration.composed_interface_schema_unavailable", Domain::FrontendCapability);
                    }
                    if(parsedType.legacy) {
                        declaration.object.at(side).array[index].object["type"]=string(resolvedType.canonical());
                        result.diagnostics.push_back({"migration.composed_historical_interface_type_adapted",at+"/type",
                            "Adapted historical declaration type '"+spelling+"' to complete canonical type '"+resolvedType.canonical()+
                            "' using the validated type codec/schema. Its original private declaration, identity, connection policy and map are preserved; no widget association or executable boundary binding was created.",false});
                    }
                }
            }
        }
        output.object.emplace("interface",std::move(declaration));output.object.emplace("front_panel",std::move(panel));
        output.object.emplace("diagram",adapter.scope(*root.member("diagram"),"/diagram","/diagram"));
        for(const auto key:{"host","execution_policy"})if(const auto *value=root.member(key))output.object.emplace(key,*value);
        Json ide=root.member("ide")?objectAt(root.member("ide"),"/ide"):object();
        if(const auto *defaults=root.member("text_defaults"))ide.object["text_defaults"]=*defaults;
        Json recovery=ide.member("recoverability")?objectAt(ide.member("recoverability"),"/ide/recoverability"):object();Json provenance=object();
        provenance.object={{"original_uri",string(std::string(uri))},{"original_sha256",string(result.originalSha256)},
            {"profile",string("frog.studio.explicit-draft-to-public@1")},{"preserved_authoring_source",*parsed}};
        recovery.object["graiphic_studio_migration"]=std::move(provenance);ide.object["recoverability"]=std::move(recovery);output.object.emplace("ide",std::move(ide));
        std::string error;
        if(document_internal::boundedDocumentJsonValueCount(output)>kMaximumFrogJsonValueCount) {
            auto &migration=output.object.at("ide").object.at("recoverability").object.at("graiphic_studio_migration");
            migration.object.emplace("original_draft_revision",*parsed->member("draft_revision"));
            if(document_internal::compactDraftArrayProvenance(migration.object.at("preserved_authoring_source"),error))
                result.diagnostics.push_back({"migration.array_provenance_compacted","/ide/recoverability/graiphic_studio_migration/preserved_authoring_source","Retained numeric authoring cells using the existing exact draft Array storage profile; source identity, values, types and presentation are preserved without a second scalar JSON tree.",false});
            else fail("/ide/recoverability",error.empty()?"The generated Source exceeds its bounded JSON representation budget.":error,"migration.composed_public_source_resource_limit",Domain::Control);
        }
        auto source=FrogSourceDocument::read(encode(output),&error);
        if(!source||source->envelopeStatus()!=FrogSourceEnvelopeStatus::Recognized||!source->diagnostics().empty())fail("/",error.empty()?"The derived local Source failed envelope validation.":error,frogJsonResourceLimit(error)?"migration.composed_public_source_resource_limit":"migration.composed_public_source_invalid",frogJsonResourceLimit(error)?Domain::Control:Domain::Semantics);
        result.publicSource=std::make_shared<const FrogSourceDocument>(std::move(*source));
        result.diagnostics.push_back({"migration.composed_program_profile","/diagram",
            "Derived the explicit local composed-program Source profile with complete types, authored regions, live widget declarations and actual connector identities. Native FIR, providers and target admission remain mandatory; no simulation fallback or executable claim is implied.",false});
    } catch(const Failure &failure) {result.publicSource.reset();result.diagnostics.push_back(failure.diagnostic);}
    catch(const std::bad_alloc &) {result.publicSource.reset();result.diagnostics.push_back({"migration.composed_resource_limit","/","Not enough memory to complete this operation.",true,Domain::Control});}
    catch(const std::exception &failure) {result.publicSource.reset();result.diagnostics.push_back({"migration.composed_source_invalid","/",failure.what(),true,Domain::Control});}
    return result;
}
}
