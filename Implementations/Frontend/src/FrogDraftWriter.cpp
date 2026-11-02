#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/ConnectivityConfiguration.hpp"
#include "frog_native_engine/FrogSourceDocument.hpp"
#include "frog_native_engine/FrogIconPresentation.hpp"
#include "frog_native_engine/DiagramCommentAttachment.hpp"
#include "frog_native_engine/DiagramCommentTextRuns.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/ErrorPaletteBinding.hpp"
#include "frog_native_engine/ClusterLayoutData.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "frog_native_engine/SequenceStructure.hpp"
#include "frog_native_engine/SequenceFrameGeometry.hpp"
#include "frog_native_engine/InPlaceStructure.hpp"
#include "frog_native_engine/CollectionModel.hpp"
#include "frog_native_engine/CollectionBinding.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include "FrogDocumentCaseStructureSerialization.hpp"
#include "FrogDocumentDisabledStructureSerialization.hpp"
#include "FrogDocumentConditionalDisableSerialization.hpp"
#include "FrogDocumentConditionalDisable.hpp"
#include "FrogDocumentEncoding.hpp"
#include "FrogDocumentJsonWriter.hpp"
#include "FrogDocumentStaticVisualSerialization.hpp"
#include "FrogDocumentApplicationControlSerialization.hpp"
#include "FrogDocumentIdeSerialization.hpp"
#include "FrogDocumentArrayStorage.hpp"
#include "FrogDocumentEventStructureSerialization.hpp"
#include "FrogDocumentForLoopSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "FrogDocumentTimedLoopSerialization.hpp"
#include "FrogDocumentWhileLoopSerialization.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include "frog_native_engine/WidgetCatalog.hpp"
#include "frog_native_engine/WidgetPlacementContract.hpp"
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/TabControl.hpp"
#include "FrogDocumentWaveformGraph.hpp"
#include "FrogDocumentXYGraph.hpp"
#include "FrogDocumentIntegerLexeme.hpp"
#include "FrogJsonInternal.hpp"
#include <memory>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <array>
#include <charconv>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <limits>
#include <locale>
#include <map>
#include <optional>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Private-draft writer: ordered JSON emission and persisted presentation.
// Import/loadability policy remains in FrogDocument.cpp.
namespace frog::native_engine {
namespace {
using document_internal::formatRgbHex;
using document_internal::appendJsonField;
using document_internal::appendJsonNumberField;
using document_internal::appendJsonIntegerField;
using document_internal::appendJsonBooleanField;
using document_internal::appendBounds;
using document_internal::appendStaticVisual;
using document_internal::staticVisualMediaType;
using document_internal::appendPrivateInterfaceMap;
using document_internal::hasIdePreferences;
using document_internal::appendIdePreferences;
struct WidgetWriteContext : document_internal::ArrayStorageWriteContext {
    std::uint32_t requiredStateRevision=0;
    explicit WidgetWriteContext(FrogArrayStorageMode selectedMode) {
        mode=selectedMode;
    }
};

// Detect only widget-owned state/embedded data, not arbitrary string text.
// Most stored payloads take the marker-free fast path. JSON trees and nested
// data.default/cell payloads live on the heap and are cleared iteratively.
std::uint32_t storedPayloadStateRevision(std::string_view text) {
    const auto candidate=[](std::string_view value) {
        return value.find("initial_value")!=value.npos ||
            value.find(kFrogWidgetValueDraftFormat)!=value.npos ||
            value.find("numeric-elements-json-le32-v1")!=value.npos ||
            value.find("numeric-cell-text-le32-v1")!=value.npos;
    };
    if(!candidate(text))return 0;
    std::uint32_t required=0;
    using source::JsonValue;
    std::vector<std::unique_ptr<JsonValue>> trees;
    struct Cleanup {std::vector<std::unique_ptr<JsonValue>> &trees;~Cleanup(){
        for(auto &tree:trees)source::internal::clearJsonTree(*tree);
    }} cleanup{trees};
    std::vector<const JsonValue *> pending;
    const auto parse=[&](std::string_view bytes) {
        if(!candidate(bytes))return;
        auto value=source::internal::parseAuthoringJsonCandidate(bytes);
        if(value) {
            std::string error;
            if(!document_internal::inflateArrayStorage(*value,error)) {
                source::internal::clearJsonTree(*value);
                return;
            }
            trees.push_back(std::make_unique<JsonValue>(std::move(*value)));
            pending.push_back(trees.back().get());
        }
    };
    parse(text);
    while(!pending.empty()) {
        const auto *value=pending.back();pending.pop_back();
        if(value->type==JsonValue::Type::Array) {
            for(const auto &child:value->array)pending.push_back(&child);
            continue;
        }
        if(value->type!=JsonValue::Type::Object)continue;
        if(const auto *format=value->member("format");format && format->type==JsonValue::Type::String &&
            format->string==kFrogWidgetValueDraftFormat && document_internal::privateWidgetValueObject(*value)) {
            const auto *revision=value->member("draft_revision");
            // The envelope boundary admitted only these exact known revisions.
            // Embedded reference/configuration values retain their own minimum;
            // later payloads may require more than an earlier complex value.
            required=(std::max)(required,static_cast<std::uint32_t>(revision->number));
            pending.push_back(document_internal::privateWidgetValueObject(*value));
        }
        const auto *type=value->member("valueType"), *state=value->member("state");
        const auto *classId=value->member("classId");
        if(!classId)classId=value->member("class_ref");
        if(classId && classId->type==JsonValue::Type::String && type && type->type==JsonValue::Type::String && state) {
            const auto *initial=state->member("initial_value"), *present=state->member("numeric_value_present");
            if(initial && initial->type==JsonValue::Type::String && present && present->type==JsonValue::Type::Boolean) {
                if (numericCanonicalValueTypeIsComplex(type->string))
                    required=(std::max)(required,kFrogComplexStateDraftRevision);
                if (document_internal::canonicalWidgetWideIntegerLexeme(initial->string,type->string))
                    required=(std::max)(required,kFrogExactIntegerStateDraftRevision);
            }
        }
        const auto queue=[&](const JsonValue *child) {
            if(child && child->type==JsonValue::Type::Object)pending.push_back(child);
        };
        const auto queueCells=[&](const JsonValue *records) {
            if(!records || records->type!=JsonValue::Type::Array)return;
            for(const auto &record:records->array) {
                const auto *child=record.member("value"), *kind=record.member("kind");
                if(!child)child=record.member("widget");
                if(!child)continue;
                if(child->type==JsonValue::Type::Object)queue(child);
                else if(child->type==JsonValue::Type::String && kind && kind->type==JsonValue::Type::String &&
                    kind->string=="widget")parse(child->string);
            }
        };
        if(classId)if(const auto *props=value->member("props")) {
            queue(props->member("element.template"));
            if(const auto *fields=props->member("cluster.elements");fields && fields->type==JsonValue::Type::Array)
                for(const auto &field:fields->array)queue(field.member("widget"));
            if(const auto *defaults=props->member("data.default");defaults && defaults->type==JsonValue::Type::String)
                parse(defaults->string);
            queueCells(props->member("elements"));
        }
        if(value->member("array_cell_value"))if(const auto *data=value->member("value")) {
            queue(data);
            if(const auto *fields=data->member("fields");fields && fields->type==JsonValue::Type::Array)
                for(const auto &field:fields->array)queue(field.member("value"));
            queueCells(data->member("elements"));
            queue(data->member("element_default"));
        }
    }
    return required;
}
using document_internal::base64Encode;
using document_internal::appendIndent;
using document_internal::appendJsonString;
using document_internal::formatNumber;
using document_internal::normalizedInterfaceConnectionRequirement;
bool storedWidgetNeedsExactString(const ArrayElementValue &value) {
    // JSON object source ranges exclude whitespace around the token. Historical
    // full-widget cells may own those bytes, including the ordinary writer's
    // final LF. The compatible string representation preserves them exactly.
    const auto whitespace=[](char c) {return c==' ' || c=='\t' || c=='\r' || c=='\n';};
    return value.kind==ArrayElementValueKind::Widget && !value.displayValue.empty() &&
        (whitespace(value.displayValue.front()) || whitespace(value.displayValue.back()));
}
std::string fontWeightForTextStyle(std::string_view style) {
    return textStyleHasFlag(style, "bold") ? "700" : "400";
}

std::string fontStyleForTextStyle(std::string_view style) {
    return textStyleHasFlag(style, "italic") ? "italic" : "normal";
}

std::string textDecorationForTextStyle(std::string_view style) {
    std::string result;
    if (textStyleHasFlag(style, "underline")) { result = "underline"; }
    if (textStyleHasFlag(style, "strikeout")) {
        if (!result.empty()) { result += ' '; }
        result += "line-through";
    }
    return result.empty() ? "none" : result;
}

bool hasExactWidgetIntegerState(const WidgetInstance &widget) {
    if(!isNumericWidget(widget) || !widget.initialValue)return false;
    if(widget.numericValue && std::abs(*widget.numericValue)<std::ldexp(1.0,53))return false;
    const auto projection=document_internal::widgetWideIntegerCacheProjection(*widget.initialValue,widget.valueType);
    // An explicit numeric mutation supersedes its previous lexical cache.
    return projection && (!widget.numericValue || *projection==*widget.numericValue);
}
bool hasNumericState(const WidgetInstance &widget) {
    return widget.numericValue.has_value() || hasExactWidgetIntegerState(widget) ||
        (numericCanonicalValueTypeIsComplex(widget.valueType) && widget.initialValue.has_value());
}

bool hasBooleanState(const WidgetInstance &widget) noexcept {
    return widget.booleanValue.has_value();
}

bool hasWidgetState(const WidgetInstance &widget) {
    return hasBooleanState(widget) || hasNumericState(widget);
}

bool hasWidgetPropertyOverrides(const WidgetInstance &widget) noexcept {
    return bool(widget.collection) || widget.valueType == "variant" || isXYGraphWidget(widget) || isWaveformGraphWidget(widget) || isLabelWidget(widget) ||
        isArrayWidget(widget) ||
        isClusterWidget(widget) ||
        isNumericWidget(widget) ||
        widget.contextHelpDescription.has_value() ||
        widget.contextTip.has_value() ||
        widget.contextHelpUrl.has_value() ||
        widget.enabledState.has_value() ||
        widget.labelVisible.has_value() ||
        widget.caption.has_value() || widget.captionVisible.has_value() ||
        widget.buttonStateFalseText.has_value() ||
        widget.buttonStateTrueText.has_value() ||
        widget.buttonStateTextVisible.has_value() ||
        widget.buttonStateTextLockInCenter.has_value() ||
        widget.stateFalseFillRgb.has_value() ||
        widget.stateTrueFillRgb.has_value() ||
        widget.stateFalseStrokeRgb.has_value() ||
        widget.stateTrueStrokeRgb.has_value() ||
        widget.stateTextRgb.has_value() ||
        widget.labelHorizontalAlignment.has_value() ||
        widget.labelVerticalAlignment.has_value() ||
        widget.labelSurfaceFillRgb.has_value() ||
        widget.labelSurfaceStrokeRgb.has_value() ||
        ((isStringWidget(widget) || isPathWidget(widget)) && widget.supportTextValue.has_value()) ||
        widget.numericBodyFillRgb.has_value() ||
        widget.numericBodyStrokeRgb.has_value() ||
        widget.stringBodyFillRgb.has_value() ||
        widget.stringBodyStrokeRgb.has_value() ||
        widget.surfaceFillRgb.has_value() ||
        widget.surfaceStrokeRgb.has_value() ||
        widget.numericStepButtonFillRgb.has_value() ||
        widget.numericStepButtonStrokeRgb.has_value() ||
        widget.numericStepArrowFillRgb.has_value() ||
        widget.numericStepArrowStrokeRgb.has_value() ||
        widget.buttonMechanicalAction.has_value() ||
        widget.numericIncrementDecrementVisible.has_value() ||
        widget.numericIncrementDecrementSide.has_value() ||
        widget.pathFolderVisible.has_value() ||
        widget.pathBrowseButtonVisible.has_value() ||
        widget.pathScrollbarVisible.has_value() ||
        widget.pathOverflowMode.has_value() ||
        widget.pathSizeToText.has_value() ||
        widget.pathBrowsePrompt.has_value() ||
        widget.pathBrowsePatternLabel.has_value() ||
        widget.pathBrowsePattern.has_value() ||
        widget.pathBrowseSelectionMode.has_value() ||
        widget.pathBrowseExistenceMode.has_value() ||
        widget.pathBrowseStartPath.has_value() ||
        widget.enumDomainId.has_value() ||
        widget.stringDropdown ||
        ((isRingWidget(widget) || isEnumWidget(widget)) && !widget.ringItems.empty()) ||
        widget.ringAllowUndefinedValues.has_value();
}

void appendWidget(std::ostringstream &output, const SceneObject &object, int indent, bool comma, WidgetWriteContext &storageMode);
// Child widgets and compact Array records are emitted at their original byte
// positions. Borrow the immutable authoring tree instead of copying a complete
// SceneObject/subtree into each nested serializer frame.
struct DeferredWidgetWrite {
    std::size_t offset;
    const WidgetInstance *widget;
    int indent;
    bool comma;
    std::vector<std::string> records;
    std::string readable;
};
std::string serializeStoredElementWidget(const WidgetInstance &widget, WidgetWriteContext &storageMode);

bool hasGeometryOverrides(const WidgetInstance &widget) noexcept {
    return widget.numericValueOffsetX.has_value() ||
        widget.numericValueWidth.has_value() ||
        widget.labelOffsetX.has_value() ||
        widget.labelOffsetY.has_value();
}

bool hasTypographyOverrides(const WidgetInstance &) noexcept {
    // Always store the inheritance marker, including an entirely default font.
    // Compatibility appearance fields cannot describe default versus custom.
    return true;
}

bool hasWidgetBinding(const WidgetInstance &widget) noexcept {
    return supportsPublicInterfaceBinding(widget) &&
        (widget.publicInputId.has_value() ||
        widget.publicOutputId.has_value() ||
        widget.interfaceMapSlotId.has_value());
}

void appendWidgetBinding(std::ostringstream &output, const WidgetInstance &widget, int indent, bool comma = true) {
    appendIndent(output, indent);
    output << "\"binding\": {\n";
    appendJsonField(
        output,
        indent + 1,
        "mode",
        "widget_value",
        true);
    if (widget.publicInputId.has_value()) {
        appendJsonField(output, indent + 1, "public_input_id", widget.publicInputId.value(), true);
    }
    if (widget.publicOutputId.has_value()) {
        appendJsonField(output, indent + 1, "public_output_id", widget.publicOutputId.value(), true);
    }
    if (widget.interfaceMapSlotId.has_value()) {
        appendJsonField(output, indent + 1, "interface_map_slot", widget.interfaceMapSlotId.value(), true);
    }
    appendJsonField(
        output,
        indent + 1,
        "connection_requirement",
        normalizedInterfaceConnectionRequirement(widget.interfaceConnectionRequirement.value_or("recommended")),
        false);
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

bool diagramNodeHasNumericDisplayFormat(const DiagramNode &node) noexcept {
    return node.numericRadixVisible.has_value() || node.numericDisplayFormat.has_value() ||
        node.numericDisplayDigits.has_value() ||
        node.numericPrecisionType.has_value() ||
        node.numericHideTrailingZeros.has_value() ||
        node.numericExponentMultipleOfThree.has_value() ||
        node.numericMinimumFieldWidth.has_value() ||
        node.numericPadding.has_value() ||
        node.numericAlignment.has_value() ||
        node.numericSignDisplay.has_value() ||
        node.numericLocale.has_value() ||
        node.numericDisplayPrefix.has_value() ||
        node.numericDisplaySuffix.has_value() ||
        node.numericRadixShowPrefix.has_value() ||
        node.numericRadixUppercase.has_value() ||
        node.numericRadixGroupSize.has_value() ||
        node.numericRadixInterpretation.has_value() ||
        node.numericCustomFormatString.has_value();
}

bool diagramNodeHasValueTextFont(const DiagramNode &node) noexcept {
    return node.defaultTextGeometryInitialized || !node.fontPointSizeUsesDefault || !node.fontFamily.empty() ||
        node.textStyle != "plain" ||
        std::abs(node.fontPointSize - 15.0) > 0.001 ||
        (node.kind == "comment" && node.commentUseThemeTextColor.has_value()) ||
        node.textRgb.has_value() ||
        node.textBackgroundRgb.has_value();
}

void appendDiagramNodeTextFont(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent) {
    appendIndent(output, indent);
    output << "\"font\": {\n";
    appendJsonField(output, indent + 1, "family", node.fontFamily);
    appendJsonField(output, indent + 1, "style", node.textStyle);
    appendJsonBooleanField(output, indent + 1, "use_default_size", node.fontPointSizeUsesDefault);
    appendJsonBooleanField(output, indent + 1, "geometry_initialized", node.defaultTextGeometryInitialized);
    if (node.kind == "comment" && node.commentUseThemeTextColor.has_value()) {
        appendJsonBooleanField(output, indent + 1, "use_theme_text_color", *node.commentUseThemeTextColor);
    }
    appendJsonNumberField(
        output,
        indent + 1,
        "point_size",
        node.fontPointSize,
        node.textRgb.has_value() || node.textBackgroundRgb.has_value());
    if (node.textRgb.has_value()) {
        appendJsonIntegerField(
            output,
            indent + 1,
            "text_rgb",
            node.textRgb.value(),
            node.textBackgroundRgb.has_value());
    }
    if (node.textBackgroundRgb.has_value()) {
        appendJsonIntegerField(
            output,
            indent + 1,
            "background_rgb",
            node.textBackgroundRgb.value(),
            false);
    }
    appendIndent(output, indent);
    output << "},\n";
}

void appendDiagramCommentTextRuns(std::ostringstream &output, const DiagramNode &node, int indent) {
    if (node.commentTextRuns.empty()) return;
    // Clean a copy: saving must not mutate the authoring model or its history.
    auto normalized = node;
    normalizeCommentTextRuns(normalized, commentTextUtf16Length(node.text));
    if (normalized.commentTextRuns.empty()) return;
    appendIndent(output, indent);
    output << "\"comment_text_runs\": [\n";
    for (std::size_t index = 0; index < normalized.commentTextRuns.size(); ++index) {
        const auto &run = normalized.commentTextRuns[index];
        appendIndent(output, indent + 1); output << "{\n";
        appendIndent(output, indent + 2); output << "\"start_utf16\": " << run.start << ",\n";
        appendIndent(output, indent + 2); output << "\"length_utf16\": " << run.length << ",\n";
        appendIndent(output, indent + 2); output << "\"format\": {\n";
        appendJsonField(output, indent + 3, "family", run.format.fontFamily);
        appendJsonField(output, indent + 3, "style", run.format.textStyle);
        appendJsonNumberField(output, indent + 3, "point_size", run.format.fontPointSize);
        appendJsonBooleanField(output, indent + 3, "use_theme_text_color", run.format.useThemeTextColor,
            run.format.textRgb.has_value());
        if (run.format.textRgb)
            appendJsonIntegerField(output, indent + 3, "text_rgb", *run.format.textRgb, false);
        appendIndent(output, indent + 2); output << "}\n";
        appendIndent(output, indent + 1);
        output << (index + 1 < normalized.commentTextRuns.size() ? "},\n" : "}\n");
    }
    appendIndent(output, indent); output << "],\n";
}

void appendDiagramNodeNumericDisplayFormat(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent) {
    if (!diagramNodeHasNumericDisplayFormat(node)) {
        return;
    }
    appendIndent(output, indent);
    output << "\"display_format\": {\n";
    bool wroteField = false;
    const auto beginField = [&]() {
        if (wroteField) {
            output << ",\n";
        }
        appendIndent(output, indent + 1);
        wroteField = true;
    };
    const auto appendString = [&](std::string_view key, const std::optional<std::string> &value) {
        if (value.has_value()) {
            beginField();
            appendJsonString(output, std::string(key));
            output << ": ";
            appendJsonString(output, value.value());
        }
    };
    const auto appendBool = [&](std::string_view key, const std::optional<bool> &value) {
        if (value.has_value()) {
            beginField();
            appendJsonString(output, std::string(key));
            output << ": " << (value.value() ? "true" : "false");
        }
    };
    const auto appendInt = [&](std::string_view key, const std::optional<int> &value) {
        if (value.has_value()) {
            beginField();
            appendJsonString(output, std::string(key));
            output << ": " << value.value();
        }
    };

    appendBool("radix_visible", node.numericRadixVisible);
    appendString("kind", node.numericDisplayFormat);
    appendInt("digits", node.numericDisplayDigits);
    appendString("precision_type", node.numericPrecisionType);
    appendBool("hide_trailing_zeros", node.numericHideTrailingZeros);
    appendBool("exponent_multiple_of_3", node.numericExponentMultipleOfThree);
    appendInt("minimum_field_width", node.numericMinimumFieldWidth);
    appendString("padding", node.numericPadding);
    appendString("alignment", node.numericAlignment);
    appendString("sign", node.numericSignDisplay);
    appendString("locale", node.numericLocale);
    appendString("prefix", node.numericDisplayPrefix);
    appendString("suffix", node.numericDisplaySuffix);
    appendBool("radix_show_prefix", node.numericRadixShowPrefix);
    appendBool("radix_uppercase", node.numericRadixUppercase);
    appendInt("radix_group_size", node.numericRadixGroupSize);
    appendString("radix_interpretation", node.numericRadixInterpretation);
    appendString("custom_format", node.numericCustomFormatString);
    output << '\n';
    appendIndent(output, indent);
    output << "},\n";
}

void appendDiagram(
    std::ostringstream &output,
    const std::vector<DiagramNode> &nodes,
    const std::vector<DiagramEdge> &edges,
    const std::vector<DiagramWireFragment> &wireFragments,
    int zoomPercent,
    Point viewportOrigin,
    int indent,
    std::string_view ownerStructureId,
    std::string_view ownerRegionId, WidgetWriteContext &storageMode);

void appendDiagramNode(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent,
    bool comma,
    const std::vector<DiagramNode> &allNodes,
    const std::vector<DiagramEdge> &allEdges,
    const std::vector<DiagramWireFragment> &allWireFragments, WidgetWriteContext &storageMode) {
    appendIndent(output, indent);
    output << "{\n";
    appendJsonField(output, indent + 1, "id", node.id);
    if(node.typeDefinition) {
        storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,kFrogTypeDefinitionDraftRevision);
        appendIndent(output,indent+1);output << "\"type_definition\": " << serializeTypeDefinitionLink(*node.typeDefinition) << ",\n";
    }
    if(node.collectionConstant) {
        const auto id=collectionFunctionId(node.type);
        if(!isCollectionConstant(node)||(id=="collection.map.constant")!=(node.collectionConstant->kind==CollectionKind::Map))throw std::invalid_argument("Collection constant identity and kind disagree.");
        appendIndent(output,indent+1);output << "\"studio_collection_constant\": ";document_internal::appendJsonValue(output,writeCollectionAuthoringState(*node.collectionConstant));output << ",\n";
    }
    if(node.collectionBuild) {
        const auto id=collectionFunctionId(node.type);
        if((id!="collection.map.build"&&id!="collection.set.build")||
           (node.collectionBuild->typeWitness.known()&&node.collectionBuild->typeWitness.kind!=(id=="collection.map.build"?binding::TypeKind::Map:binding::TypeKind::Set)))throw std::invalid_argument("Collection Build identity and type witness disagree.");
        appendIndent(output,indent+1);output << "\"studio_collection_build\": ";document_internal::appendJsonValue(output,writeCollectionBuildState(*node.collectionBuild));output << ",\n";
    }
    if (node.applicationControl) {
        storageMode.requiredStateRevision = (std::max)(storageMode.requiredStateRevision, kFrogApplicationControlDraftRevision);
        document_internal::appendApplicationControl(output, *node.applicationControl, indent + 1);
    }
    if (node.type.starts_with("frog.studio.connectivity.")) {
        storageMode.requiredStateRevision = (std::max)(storageMode.requiredStateRevision, kFrogConnectivityDraftRevision);
    }
    if (node.connectivity) {
        // The payload itself requires this revision, including hidden
        // definitions whose diagram ID is outside the connectivity prefix.
        storageMode.requiredStateRevision = (std::max)(storageMode.requiredStateRevision, kFrogConnectivityDraftRevision);
        appendIndent(output, indent + 1);
        output << "\"studio_connectivity\": ";
        document_internal::appendJsonValue(output, serializeConnectivityConfiguration(*node.connectivity));
        output << ",\n";
    }
    if (node.nativeCall) {
        storageMode.requiredStateRevision = (std::max)(storageMode.requiredStateRevision, kFrogConnectivityDraftRevision);
        appendIndent(output, indent + 1);
        output << "\"studio_native_call\": ";
        document_internal::appendJsonValue(output, nativeCallToJson(*node.nativeCall));
        output << ",\n";
    }
    const auto structureType =
        document_internal::canonicalStructureTypeForNodeType(node.type);
    const auto *choiceEntry = findFunctionCatalogEntryByDiagramNodeType(node.type);
    const bool stringDropdown = choiceEntry != nullptr &&
        choiceEntry->catalogId == "string.dropdown_constant";
    const bool timestampConstant = choiceEntry != nullptr &&
        choiceEntry->catalogId == "timestamp.constant";
    appendJsonField(
        output,
        indent + 1,
        "kind",
        structureType.empty() ? std::string_view{ node.kind } : std::string_view{ "structure" });
    if (!structureType.empty()) {
        appendJsonField(output, indent + 1, "structure_type", structureType);
        const auto &alignment = node.structureSubdiagramLabelAlignment;
        appendJsonField(output, indent + 1, "structure_subdiagram_label_alignment",
            alignment == "left" || alignment == "right" ? std::string_view{alignment} : std::string_view{"center"});
    }
    if (node.kind == "primitive" && !node.type.empty()) {
        const bool numericConstant =
            timestampConstant || node.type == "frog.studio.numeric.numeric_constant" ||
            node.type == "numeric.numeric_constant";
        const bool numericArrayConstant =
            node.type == "frog.studio.numeric.numeric_array_constant" ||
            node.type == "numeric.numeric_array_constant";
        const bool untypedArrayConstant =
            node.type == "frog.studio.array.array_constant" ||
            node.type == "array.array_constant";
        const bool enumConstant =
            node.type == "frog.studio.numeric.enum_constant" ||
            node.type == "numeric.enum_constant";
        const bool enumArrayConstant =
            node.type == "frog.studio.numeric.enum_array_constant" ||
            node.type == "numeric.enum_array_constant";
        const bool ringConstant =
            node.type == "frog.studio.numeric.ring_constant" ||
            node.type == "numeric.ring_constant";
        const bool ringArrayConstant =
            node.type == "frog.studio.numeric.ring_array_constant" ||
            node.type == "numeric.ring_array_constant";
        const bool stringConstant =
            node.type == "frog.studio.string.constant" ||
            node.type == "string.constant";
        const bool pathConstant =
            node.type == "frog.studio.path.constant" ||
            node.type == "path.constant";
        const bool stringArrayConstant =
            node.type == "frog.studio.string.string_array_constant" ||
            node.type == "string.string_array_constant" ||
            node.type == "frog.studio.path.path_array_constant" ||
            node.type == "path.path_array_constant";
        const bool numericValueConstant =
            numericConstant || numericArrayConstant || enumConstant ||
            enumArrayConstant || ringConstant || ringArrayConstant;
        const bool booleanConstant =
            node.type == "frog.studio.boolean.true_constant" || node.type == "boolean.true_constant" ||
            node.type == "frog.studio.boolean.false_constant" || node.type == "boolean.false_constant";
        const bool primitiveValueConstant =
            numericValueConstant || stringConstant || stringDropdown || pathConstant ||
            stringArrayConstant || untypedArrayConstant || booleanConstant ||
            node.type.starts_with("frog.studio.array.other_constants.");
        appendJsonField(output, indent + 1, "type", node.type);
        if ((primitiveValueConstant || isBooleanArrayToNumber(node.type)) && !node.valueType.empty()) {
            appendJsonField(output, indent + 1, "value_type", node.valueType);
        }
        // Labels are editor metadata for functions and structures too, not
        // only for value constants.
        appendJsonField(output, indent + 1, "label", node.label);
        appendJsonBooleanField(output, indent + 1, "label_visible", node.labelVisible);
    }
    if (node.type == "frog.studio.array.index" || node.type == "array.index") {
        appendJsonIntegerField(output, indent + 1, "index_array_outputs",
            std::clamp(node.indexArrayOutputs, 1U, 32U));
    }
    if (isRetainFirstError(node.type) && node.retainFirstErrorInputs) {
        appendJsonIntegerField(output, indent + 1, "retain_first_error_inputs",errorRetainCount(node));
    }
    if (node.type == "frog.studio.array.replace_subset" || node.type == "array.replace_subset" ||
        node.type == "frog.studio.array.insert" || node.type == "array.insert") {
        appendJsonIntegerField(output, indent + 1, "array_edit_groups", std::clamp(node.arrayEditGroups, 1U, 32U));
    }
    if (node.type == "frog.studio.array.build" || node.type == "array.build") {
        appendJsonIntegerField(output, indent + 1, "build_array_inputs", std::clamp(node.buildArrayInputs, 1U, 32U));
        appendJsonBooleanField(output, indent + 1, "build_array_concatenate", node.buildArrayConcatenate);
    }
    if (node.type == "frog.studio.array.reshape" || node.type == "array.reshape") {
        appendJsonIntegerField(output, indent + 1, "reshape_array_dimensions", std::clamp(node.reshapeArrayDimensions, 1U, 16U));
    }
    if (node.arrayFunctionTerminals) {
        appendJsonIntegerField(output, indent + 1, "array_function_terminals", std::clamp(node.arrayFunctionTerminals,1U,32U));
    }
    if (node.stringFunctionTerminals != 1U) {
        appendJsonIntegerField(output, indent + 1, "string_function_terminals", std::clamp(node.stringFunctionTerminals,1U,32U));
    }
    if (!node.eventRegistrationRows.empty()) {
        appendIndent(output, indent + 1);
        output << "\"studio_event_registration_rows\": [";
        for (std::size_t i=0; i<node.eventRegistrationRows.size(); ++i) {
            if(i) output << ", ";
            output << "{ \"id\": "; appendJsonString(output,node.eventRegistrationRows[i].id);
            output << ", \"event_kind\": "; appendJsonString(output,node.eventRegistrationRows[i].eventKind);
            output << " }";
        }
        output << "],\n";
    }
    if (node.matrixAppendByRows) appendJsonBooleanField(output, indent + 1, "matrix_append_by_rows", true);
    if (node.comparisonAggregates) appendJsonBooleanField(output, indent + 1, "comparison_aggregates", true);
    if (node.type.starts_with("frog.studio.file.") || node.type.starts_with("file.")) {
        if (node.fileReadLines) appendJsonBooleanField(output, indent + 1, "file_read_lines", true);
        if (!node.fileConvertEol) appendJsonBooleanField(output, indent + 1, "file_convert_eol", false);
        if (node.fileDataType != "f64") appendJsonField(output, indent + 1, "file_data_type", node.fileDataType);
        if (node.fileXmlArray) appendJsonBooleanField(output, indent + 1, "file_xml_array", true);
    }
    if (node.numericColorBox && node.valueType == "u32")
        appendJsonBooleanField(output, indent + 1, "numeric_color_box", true);
    if (isBooleanArrayToNumber(node.type) &&
        node.booleanArraySignExtensionMode != BooleanArraySignExtensionMode::IfOutputIsSigned) {
        appendJsonField(output, indent + 1, "boolean_array_sign_extension_mode",
            std::string(booleanArraySignExtensionModeToken(node.booleanArraySignExtensionMode)));
    }
    if (node.type == "frog.studio.numeric.in_range" || node.type == "numeric.in_range") {
        if (node.inRangeIncludeUpperLimit)
            appendJsonBooleanField(output, indent + 1, "in_range_include_upper_limit", true);
        if (!node.inRangeIncludeLowerLimit)
            appendJsonBooleanField(output, indent + 1, "in_range_include_lower_limit", false);
    }
    if (node.arrayElementTemplate) {
        const auto payload=serializeArrayElementWidget(*node.arrayElementTemplate);
        storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,storedPayloadStateRevision(payload));
        appendJsonField(output, indent + 1, "array_element_template", payload);
        appendJsonBooleanField(output, indent + 1, "array_element_icon", node.arrayElementIcon);
    }
    if (!node.paletteCatalogId.empty()) {
        appendJsonField(output, indent + 1, "palette_catalog_id", node.paletteCatalogId);
    }
    if (!node.constantOwnerNodeId.empty()) {
        appendJsonField(output, indent + 1, "constant_owner_node_id", node.constantOwnerNodeId);
        appendJsonField(output, indent + 1, "constant_field_id", node.constantFieldId);
        appendJsonIntegerField(output, indent + 1, "constant_field_order", node.constantFieldOrder);
    }
    if (node.type == "frog.studio.cluster.constant" || node.type == "frog.studio.error.constant") {
        appendJsonBooleanField(output, indent + 1, "cluster_constant_collapsed", node.clusterConstantCollapsed);
        appendJsonField(output, indent + 1, "cluster_constant_autosizing", node.clusterConstantAutoSizing);
        appendJsonNumberField(output, indent + 1, "cluster_expanded_width", node.clusterConstantExpandedWidth);
        appendJsonNumberField(output, indent + 1, "cluster_expanded_height", node.clusterConstantExpandedHeight);
    }
    if (node.kind == "widget_value") {
        // An authored terminal hint must survive transport independently of
        // its widget. Missing legacy hints need an identified read migration;
        // Save must not erase an explicit type or reconstruct it from the UI.
        appendJsonField(output, indent + 1, "type", node.type);
    }
    if (node.kind == "widget_value" && !node.widgetId.empty()) {
        appendJsonField(output, indent + 1, "widget", node.widgetId);
        appendJsonBooleanField(output, indent + 1, "label_visible", node.labelVisible);
    }
    // One appearance record for every non-structure label. Structures persist
    // their shared outer/subdiagram font through their specialized serializer.
    if (node.kind == "widget_value" || node.kind == "frog_call" ||
        (node.kind == "primitive" && structureType.empty())) {
        appendJsonBooleanField(output, indent + 1, "label_auto_size", node.labelAutoSize);
        appendJsonField(
            output,
            indent + 1,
            "label_vertical_arrangement",
            node.labelVerticalArrangement);
        appendIndent(output, indent + 1);
        output << "\"label_offset\": {\n";
        appendJsonNumberField(output, indent + 2, "x", node.labelOffsetX);
        appendJsonNumberField(output, indent + 2, "y", node.labelOffsetY, false);
        appendIndent(output, indent + 1);
        output << "},\n";
        appendIndent(output, indent + 1);
        output << "\"label_size\": {\n";
        appendJsonNumberField(output, indent + 2, "width", node.labelWidth);
        appendJsonNumberField(output, indent + 2, "height", node.labelHeight, false);
        appendIndent(output, indent + 1);
        output << "},\n";
        appendIndent(output, indent + 1);
        output << "\"label_font\": {\n";
        appendJsonField(output, indent + 2, "family", node.labelFontFamily);
        appendJsonField(output, indent + 2, "style", node.labelTextStyle);
        appendJsonBooleanField(output, indent + 2, "use_default_size", node.labelFontPointSizeUsesDefault);
        appendJsonNumberField(
            output,
            indent + 2,
            "point_size",
            node.labelFontPointSize,
            node.labelTextRgb.has_value() || node.labelBackgroundRgb.has_value());
        if (node.labelTextRgb.has_value()) {
            appendJsonIntegerField(
                output,
                indent + 2,
                "text_rgb",
                node.labelTextRgb.value(),
                node.labelBackgroundRgb.has_value());
        }
        if (node.labelBackgroundRgb.has_value()) {
            appendJsonIntegerField(
                output,
                indent + 2,
                "background_rgb",
                node.labelBackgroundRgb.value(),
                false);
        }
        appendIndent(output, indent + 1);
        output << "},\n";
    }
    if (node.kind == "primitive" &&
        (stringDropdown || timestampConstant || node.type == "frog.studio.numeric.numeric_constant" ||
            node.type == "numeric.numeric_constant" ||
            node.type == "frog.studio.numeric.numeric_array_constant" ||
            node.type == "numeric.numeric_array_constant" ||
            node.type == "frog.studio.array.array_constant" ||
            node.type == "array.array_constant" ||
            node.type == "frog.studio.numeric.enum_constant" ||
            node.type == "numeric.enum_constant" ||
            node.type == "frog.studio.numeric.enum_array_constant" ||
            node.type == "numeric.enum_array_constant" ||
            node.type == "frog.studio.numeric.ring_constant" ||
            node.type == "numeric.ring_constant" ||
            node.type == "frog.studio.numeric.ring_array_constant" ||
            node.type == "numeric.ring_array_constant" ||
            node.type == "frog.studio.string.constant" ||
            node.type == "string.constant" ||
            node.type == "frog.studio.path.constant" ||
            node.type == "path.constant" ||
            node.type == "frog.studio.string.string_array_constant" ||
            node.type == "string.string_array_constant" ||
            node.type == "frog.studio.path.path_array_constant" ||
            node.type == "path.path_array_constant" ||
            node.type.starts_with("frog.studio.array.other_constants.") ||
            node.type == "frog.studio.boolean.true_constant" || node.type == "boolean.true_constant" ||
            node.type == "frog.studio.boolean.false_constant" || node.type == "boolean.false_constant")) {
        if (diagramNodeHasValueTextFont(node)) {
            appendDiagramNodeTextFont(output, node, indent + 1);
        }
        appendDiagramNodeNumericDisplayFormat(output, node, indent + 1);
        if (node.type == "frog.studio.string.constant" ||
            node.type == "string.constant" ||
            node.type == "frog.studio.path.constant" ||
            node.type == "path.constant" ||
            node.type == "frog.studio.string.string_array_constant" ||
            node.type == "string.string_array_constant" ||
            node.type == "frog.studio.path.path_array_constant" ||
            node.type == "path.path_array_constant" ||
            node.type.starts_with("frog.studio.array.other_constants.")) {
            appendIndent(output, indent + 1);
            output << "\"string_constant\": {\n";
            appendJsonField(output, indent + 2, "display_style", node.stringDisplayStyle);
            appendJsonBooleanField(output, indent + 2, "display_style_visible", node.stringDisplayStyleVisible);
            appendJsonBooleanField(output, indent + 2, "limit_to_single_line", node.stringLimitToSingleLine);
            appendJsonBooleanField(output, indent + 2, "wrap_at_word_breaks", node.stringWrapAtWordBreaks);
            appendJsonBooleanField(output, indent + 2, "update_while_typing", node.stringUpdateWhileTyping);
            appendJsonBooleanField(output, indent + 2, "vertical_scrollbar_visible", node.stringVerticalScrollbarVisible);
            appendJsonBooleanField(output, indent + 2, "horizontal_scrollbar_visible", node.stringHorizontalScrollbarVisible);
            appendJsonBooleanField(output, indent + 2, "size_to_text", node.stringSizeToText);
            appendJsonField(output, indent + 2, "text_alignment", node.stringTextAlignment, false);
            appendIndent(output, indent + 1);
            output << "},\n";
        }
        if (node.type == "frog.studio.path.constant" ||
            node.type == "path.constant") {
            appendIndent(output, indent + 1);
            output << "\"path_constant\": {\n";
            appendJsonBooleanField(output, indent + 2, "folder_visible", node.pathFolderVisible);
            appendJsonBooleanField(output, indent + 2, "browse_button_visible", node.pathBrowseButtonVisible);
            appendJsonBooleanField(output, indent + 2, "scrollbar_visible", node.pathScrollbarVisible);
            appendJsonField(output, indent + 2, "overflow_mode", node.pathOverflowMode);
            appendJsonBooleanField(output, indent + 2, "size_to_text", node.pathSizeToText);
            appendJsonField(output, indent + 2, "browse_prompt", node.pathBrowsePrompt);
            appendJsonField(output, indent + 2, "browse_pattern_label", node.pathBrowsePatternLabel);
            appendJsonField(output, indent + 2, "browse_pattern", node.pathBrowsePattern);
            appendJsonField(output, indent + 2, "browse_selection_mode", node.pathBrowseSelectionMode);
            appendJsonField(output, indent + 2, "browse_existence_mode", node.pathBrowseExistenceMode);
            appendJsonField(output, indent + 2, "browse_start_path", node.pathBrowseStartPath, false);
            appendIndent(output, indent + 1);
            output << "},\n";
        }
        // Dropdown strings share the item/selected-key storage of rings, but
        // keep their String value_type and never acquire an enum domain.
        if (stringDropdown || node.type == "frog.studio.numeric.enum_constant" ||
            node.type == "numeric.enum_constant" ||
            node.type == "frog.studio.numeric.enum_array_constant" ||
            node.type == "numeric.enum_array_constant" ||
            node.type == "frog.studio.numeric.ring_constant" ||
            node.type == "numeric.ring_constant" ||
            node.type == "frog.studio.numeric.ring_array_constant" ||
            node.type == "numeric.ring_array_constant") {
            const bool ringConstant = stringDropdown ||
                node.type == "frog.studio.numeric.ring_constant" ||
                node.type == "numeric.ring_constant" ||
                node.type == "frog.studio.numeric.ring_array_constant" ||
                node.type == "numeric.ring_array_constant";
            appendIndent(output, indent + 1);
            output << (ringConstant ? "\"ring_constant\": {\n" : "\"enum_constant\": {\n");
            if (!ringConstant) appendJsonField(output, indent + 2, "domain_id",
                node.enumDomainId.empty() ? "frog.studio.constant." + node.id : node.enumDomainId);
            appendJsonIntegerField(
                output,
                indent + 2,
                "selected_value",
                node.enumSelectedValue);
            if (ringConstant) {
                appendJsonBooleanField(
                    output,
                    indent + 2,
                    "allow_undefined_values",
                    node.ringAllowUndefinedValues);
            }
            appendJsonBooleanField(
                output,
                indent + 2,
                "text_overflow_visible",
                node.enumTextOverflowVisible);
            appendJsonBooleanField(
                output,
                indent + 2,
                "digital_display_visible",
                node.enumDigitalDisplayVisible);
            appendIndent(output, indent + 2);
            output << "\"items\": [\n";
            for (std::size_t index = 0; index < node.enumItems.size(); ++index) {
                const auto &item = node.enumItems[index];
                appendIndent(output, indent + 3);
                output << "{\n";
                appendJsonField(output, indent + 4, "label", item.label);
                appendJsonIntegerField(output, indent + 4, "value", item.value);
                appendJsonBooleanField(output, indent + 4, "disabled", item.disabled);
                appendJsonField(output, indent + 4, "id", item.id);
                appendJsonBooleanField(output, indent + 4, "visible", item.visible, false);
                appendIndent(output, indent + 3);
                output << '}' << (index + 1U < node.enumItems.size() ? "," : "") << '\n';
            }
            appendIndent(output, indent + 2);
            output << "]\n";
            appendIndent(output, indent + 1);
            output << "},\n";
        }
        if (node.type.starts_with("frog.studio.array.other_constants.") ||
            node.type == "frog.studio.array.array_constant" ||
            node.type == "array.array_constant" ||
            node.type == "frog.studio.numeric.numeric_array_constant" ||
            node.type == "numeric.numeric_array_constant" ||
            node.type == "frog.studio.numeric.enum_array_constant" ||
            node.type == "numeric.enum_array_constant" ||
            node.type == "frog.studio.numeric.ring_array_constant" ||
            node.type == "numeric.ring_array_constant" ||
            node.type == "frog.studio.string.string_array_constant" ||
            node.type == "string.string_array_constant" ||
            node.type == "frog.studio.path.path_array_constant" ||
            node.type == "path.path_array_constant") {
            appendIndent(output, indent + 1);
            output << "\"array_constant\": {\n";
            appendJsonIntegerField(output, indent + 2, "rank", std::clamp(node.numericArrayRank, 1U, 16U));
            const auto appendUnsignedArrayField = [&](std::string_view name, const std::vector<std::uint32_t> &values) {
                appendIndent(output, indent + 2);
                appendJsonString(output, std::string(name));
                output << ": [";
                for (std::size_t index = 0; index < values.size(); ++index) {
                    if (index != 0U) output << ", ";
                    output << values[index];
                }
                output << "],\n";
            };
            appendUnsignedArrayField("shape", node.numericArrayShape);
            appendUnsignedArrayField("first_indices", node.numericArrayFirstIndices);
            appendUnsignedArrayField("visible_counts", node.numericArrayVisibleCounts);
            appendJsonBooleanField(
                output,
                indent + 2,
                "one_dimensional_vertical",
                node.numericArrayOneDimensionalVertical);
            appendIndent(output, indent + 2);
            output << "\"cell_size\": {\n";
            appendJsonNumberField(output, indent + 3, "width", node.numericArrayCellWidth);
            appendJsonNumberField(output, indent + 3, "element_gap", node.numericArrayElementGap);
            appendJsonNumberField(output, indent + 3, "height", node.numericArrayCellHeight, false);
            appendIndent(output, indent + 2);
            output << "},\n";
            appendIndent(output, indent + 2);
            for(const auto &payload:node.numericArrayValues)
                storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,storedPayloadStateRevision(payload));
            if (storageMode.mode == FrogArrayStorageMode::Readable) {
                output << "\"values\": [";
                for (std::size_t index = 0; index < node.numericArrayValues.size(); ++index) {
                    if (index != 0U) output << ", ";
                    appendJsonString(output, node.numericArrayValues[index]);
                }
                output << "],\n";
            } else {
            std::ostringstream readable;
            readable << '[';
            for (std::size_t index = 0; index < node.numericArrayValues.size(); ++index) {
                if (index != 0U) readable << ", ";
                appendJsonString(readable, node.numericArrayValues[index]);
            }
            readable << ']';
            const auto readableText = readable.str();
            const bool compactableArray = node.type == "frog.studio.numeric.numeric_array_constant" ||
                node.type == "numeric.numeric_array_constant" || node.type == "frog.studio.array.array_constant" ||
                node.type == "array.array_constant" || node.type.starts_with("frog.studio.array.other_constants.");
            const auto compact = compactableArray && document_internal::compactNumericArrayType(node.valueType)
                ? document_internal::encodeArrayStorage(node.numericArrayValues, storageMode,
                    "numeric-cell-text-le32-v1", readableText.size()) : std::nullopt;
            output << "\"values\": " << (compact ? *compact : readableText) << ",\n";
            }
            appendJsonBooleanField(output, indent + 2, "element_label_visible", node.numericArrayElementLabelVisible, false);
            appendIndent(output, indent + 1);
            output << "},\n";
        }
    }
    if(node.clusterFunctionTerminals)
        appendJsonIntegerField(output, indent+1, "cluster_function_terminals", std::clamp(node.clusterFunctionTerminals,1U,32U));
    if(!node.clusterFieldNames.empty()) {
        appendIndent(output,indent+1); output << "\"cluster_field_names\": [";
        for(std::size_t i=0;i<node.clusterFieldNames.size() && i<32;++i) {
            if(i) output << ", ";
            appendJsonString(output,node.clusterFieldNames[i]);
        }
        output << "],\n";
    }
    if(!node.clusterFieldIdPaths.empty()) {
        appendIndent(output,indent+1); output << "\"cluster_field_id_paths\": [";
        for(std::size_t i=0;i<node.clusterFieldIdPaths.size() && i<32;++i) {
            if(i) output << ", ";
            output << '[';
            for(std::size_t j=0;j<node.clusterFieldIdPaths[i].size();++j) {
                if(j) output << ", ";
                appendJsonString(output,node.clusterFieldIdPaths[i][j]);
            }
            output << ']';
        }
        output << "],\n";
    }
    if(node.clusterShowFullNames)
        appendJsonBooleanField(output,indent+1,"cluster_show_full_names",true);
    if (node.kind != "comment" && !node.text.empty()) {
        appendJsonField(output, indent + 1, "text", node.text);
    }
    // These function faces use the same authored font record as literals.
    // The reader already accepts it for every node; omitting it here used to
    // turn an explicit function font back into the global default on reload.
    if (node.kind == "primitive" && diagramNodeHasValueTextFont(node) &&
        (isClusterByName(node.type) || node.applicationControl || node.nativeCall ||
            node.connectivity || !node.eventRegistrationRows.empty())) {
        appendDiagramNodeTextFont(output, node, indent + 1);
    }
    if (node.kind == "comment") {
        appendJsonBooleanField(output, indent + 1, "comment_auto_size", node.commentAutoSize);
        if (node.commentAttachment) {
            auto attachment = *node.commentAttachment;
            if (normalizeDiagramCommentAttachment(attachment)) {
                appendIndent(output, indent + 1);
                output << "\"comment_attachment\": {\n";
                appendJsonField(output, indent + 2, "kind", std::string(diagramCommentTargetKindToken(attachment.kind)));
                appendJsonField(output, indent + 2, "target_id", attachment.targetId);
                appendJsonNumberField(output, indent + 2, "node_x", attachment.nodeAnchor.x);
                appendJsonNumberField(output, indent + 2, "node_y", attachment.nodeAnchor.y);
                appendJsonNumberField(output, indent + 2, "wire_position", attachment.wirePosition, false);
                appendIndent(output, indent + 1);
                output << "},\n";
            }
        }
        appendJsonField(output, indent + 1, "text", node.text);
        appendJsonField(
            output,
            indent + 1,
            "vertical_arrangement",
            node.textVerticalArrangement);
        appendDiagramNodeTextFont(output, node, indent + 1);
        appendDiagramCommentTextRuns(output, node, indent + 1);
    }
    if (node.kind == "static_visual" && node.staticVisual.has_value()) {
        const auto &visual = node.staticVisual.value();
        appendIndent(output, indent + 1);
        output << "\"visual\": {\n";
        appendJsonField(output, indent + 2, "kind", visual.kind == StaticVisualKind::Dib ? "dib" : "svg");
        appendJsonField(output, indent + 2, "sourceName", visual.sourceName);
        if (visual.mediaRef.size() <= 8U * 1024U * 1024U) {
            appendJsonField(output, indent + 2, "mediaRef", visual.mediaRef);
        } else {
            // Keep each JSON string bounded without inflating PNG/JPG into raw pixels.
            appendIndent(output, indent + 2); output << "\"mediaRefChunks\": [\n";
            for (std::size_t offset = 0; offset < visual.mediaRef.size(); offset += 8U * 1024U * 1024U) {
                const auto chunk = visual.mediaRef.substr(offset, 8U * 1024U * 1024U);
                appendIndent(output, indent + 3); appendJsonString(output, chunk);
                output << (offset + chunk.size() < visual.mediaRef.size() ? ",\n" : "\n");
            }
            appendIndent(output, indent + 2); output << "],\n";
        }
        appendJsonField(output, indent + 2, "mediaType", staticVisualMediaType(visual));
        appendJsonNumberField(output, indent + 2, "naturalWidth", visual.naturalWidth);
        appendJsonNumberField(output, indent + 2, "naturalHeight", visual.naturalHeight);
        appendJsonBooleanField(output, indent + 2, "preserveAspectRatio", visual.preserveAspectRatio);
        appendJsonNumberField(output, indent + 2, "rotationQuarterTurns", visual.rotationQuarterTurns);
        appendJsonBooleanField(output, indent + 2, "flipHorizontal", visual.flipHorizontal);
        appendJsonBooleanField(output, indent + 2, "flipVertical", visual.flipVertical);
        if (visual.kind == StaticVisualKind::Svg) {
            appendJsonField(output, indent + 2, "svgText", visual.svgText, false);
        } else {
            appendJsonField(output, indent + 2, "dibBase64",
                staticVisualUsesEncodedRasterSource(visual) ? std::string{} : base64Encode(visual.dibBytes), false);
        }
        appendIndent(output, indent + 1);
        output << "},\n";
    }
    if (node.kind == "frog_call") {
        appendJsonField(output, indent + 1, "label", node.label);
        appendJsonBooleanField(output, indent + 1, "label_visible", node.labelVisible);
        appendIndent(output, indent + 1);
        output << "\"call\": {\n";
        appendJsonField(output, indent + 2, "document_path", node.callDocumentPath);
        appendJsonField(output, indent + 2, "document_title", node.callDocumentTitle);
        appendJsonField(output, indent + 2, "icon_svg", node.callIconSvgText);
        appendIndent(output, indent + 2);
        output << "\"ports\": [";
        if (!node.callPorts.empty()) {
            output << '\n';
            for (std::size_t index = 0; index < node.callPorts.size(); ++index) {
                const auto &port = node.callPorts[index];
                appendIndent(output, indent + 3);
                output << "{\n";
                appendJsonField(output, indent + 4, "id", port.id);
                appendJsonField(output, indent + 4, "source_port_id", port.sourcePortId);
                appendJsonField(output, indent + 4, "interface_map_slot", port.interfaceMapSlotId);
                appendJsonField(output, indent + 4, "label", port.label);
                appendJsonField(output, indent + 4, "value_type", port.valueType);
                appendJsonField(output, indent + 4, "direction", port.input ? "input" : "output");
                appendJsonField(
                    output,
                    indent + 4,
                    "connection_requirement",
                    port.connectionRequirement);
                appendJsonNumberField(output, indent + 4, "normalized_x", port.normalizedX);
                appendJsonNumberField(output, indent + 4, "normalized_y", port.normalizedY, false);
                appendIndent(output, indent + 3);
                output << '}' << (index + 1U < node.callPorts.size() ? "," : "") << '\n';
            }
            appendIndent(output, indent + 2);
        }
        output << "]\n";
        appendIndent(output, indent + 1);
        output << "},\n";
    }
    if (!node.structureAutoGrow) {
        appendIndent(output, indent + 1);
        output << "\"structure_auto_grow\": false,\n";
    }
    if (!node.forLoopIterationVisible) {
        appendIndent(output, indent + 1);
        output << "\"for_loop_iteration_visible\": false,\n";
    }
    if (!node.documentationSummary.empty() || !node.documentationUrl.empty() ||
        node.instanceDescription.has_value() || node.instanceTip.has_value()) {
        appendIndent(output, indent + 1);
        output << "\"doc\": {\n";
        appendJsonField(output, indent + 2, "summary", node.documentationSummary);
        if (node.instanceDescription) appendJsonField(output, indent + 2, "instance_description", *node.instanceDescription);
        if (node.instanceTip) appendJsonField(output, indent + 2, "tip", *node.instanceTip);
        appendJsonField(output, indent + 2, "url", node.documentationUrl, false);
        appendIndent(output, indent + 1);
        output << "},\n";
    }
    if (!node.invertedInputPorts.empty()) {
        appendIndent(output, indent + 1);
        output << "\"inverted_input_ports\": [";
        for (std::size_t index = 0; index < node.invertedInputPorts.size(); ++index) {
            if (index > 0U) {
                output << ", ";
            }
            appendJsonString(output, node.invertedInputPorts[index]);
        }
        output << "],\n";
    }
    if (node.type == "frog.studio.structures.feedback_node") {
        appendJsonField(
            output,
            indent + 1,
            "feedback_node_configuration",
            normalizeFeedbackNodeConfiguration(node.feedbackNodeConfiguration));
        appendJsonNumberField(output, indent + 1, "feedback_delay", node.feedbackDelay);
        appendJsonField(output, indent + 1, "feedback_initialization", node.feedbackInitialization);
        appendJsonBooleanField(output, indent + 1, "feedback_initialization_explicit", node.feedbackInitializationExplicit);
        appendJsonField(output, indent + 1, "feedback_initializer_loop_id", node.feedbackInitializerLoopId);
        appendJsonBooleanField(output, indent + 1, "feedback_history", node.feedbackHistory);
        appendJsonBooleanField(output, indent + 1, "feedback_z_transform", node.feedbackZTransform);
    }
    if (node.type == diagramStructureNodeType(
            DiagramStructureFamily::Conditional)) {
        document_internal::appendStructureBoundary(output, node, indent + 1);
        output << document_internal::serializeCaseStructureSelectorTerminal(
            node,
            indent + 1);
        appendJsonField(output, indent + 1, "case_profile", "frog.case-editing@1");
        appendJsonNumberField(output, indent + 1, "case_structure_radix", node.caseStructureRadix);
        appendIndent(output, indent + 1);
        output << "\"regions\": [\n";
        std::vector<DiagramCaseStructureRegion> legacyBooleanRegions;
        const auto *regions = &node.caseStructureRegions;
        if (regions->empty()) {
            legacyBooleanRegions = {
                DiagramCaseStructureRegion{
                    std::string{ kCaseStructureTrueRegionId },
                    { DiagramCaseStructureMatcher{ "boolean_exact", "true" } },
                    false, node.caseStructureTrueSubdiagramLabelText },
                DiagramCaseStructureRegion{
                    std::string{ kCaseStructureFalseRegionId },
                    { DiagramCaseStructureMatcher{ "boolean_exact", "false" } },
                    false, node.caseStructureFalseSubdiagramLabelText } };
            regions = &legacyBooleanRegions;
        }
        const auto appendMatcher = [&](
            const DiagramCaseStructureMatcher &matcher,
            int matcherIndent,
            bool commaAfter) {
            appendIndent(output, matcherIndent);
            output << "{\"kind\": ";
            appendJsonString(output, matcher.kind);
            if (matcher.kind == "boolean_exact" &&
                       (matcher.value == "true" || matcher.value == "false")) {
                output << ", \"value\": "
                       << (matcher.value == "true" ? "true" : "false");
            } else if (matcher.kind == "integer_range" || matcher.kind == "enum_range") {
                if (matcher.lower) { output << ", \"lower\": "; appendJsonString(output, *matcher.lower); }
                if (matcher.upper) { output << ", \"upper\": "; appendJsonString(output, *matcher.upper); }
            } else if (matcher.kind == "enum_item") {
                output << ", \"item\": ";
                appendJsonString(output, matcher.value);
            } else {
                // Integer-exact values also stay strings: JSON number readers
                // cannot preserve every 64-bit integer identity.
                output << ", \"value\": ";
                appendJsonString(output, matcher.value);
            }
            output << '}' << (commaAfter ? "," : "") << '\n';
        };
        const auto appendRegion = [
            &output,
            &allNodes,
            &allEdges,
            &allWireFragments,
            &node,
            indent,
            &storageMode,
            &appendMatcher](
                const DiagramCaseStructureRegion &region,
                bool commaAfter) {
            appendIndent(output, indent + 2);
            output << "{\n";
            appendJsonField(output, indent + 3, "id", region.id);
            appendJsonField(output, indent + 3, "title", region.title);
            if (region.draftText) appendJsonField(output, indent + 3, "draft_text", *region.draftText);
            if (region.matches.empty() && !region.isDefault)
                appendJsonBooleanField(output, indent + 3, "draft", true);
            const bool legacyBoolean =
                node.caseStructureSelectorValueType == "bool" &&
                region.matches.size() == 1U &&
                region.matches.front().kind == "boolean_exact" &&
                ((region.id == kCaseStructureTrueRegionId && region.matches.front().value == "true") ||
                 (region.id == kCaseStructureFalseRegionId && region.matches.front().value == "false"));
            if (legacyBoolean) {
                appendJsonBooleanField(
                    output,
                    indent + 3,
                    "match",
                    region.matches.front().value == "true");
            } else {
                appendIndent(output, indent + 3);
                output << "\"matches\": [\n";
                for (std::size_t matcherIndex = 0U;
                     matcherIndex < region.matches.size(); ++matcherIndex) {
                    appendMatcher(
                        region.matches[matcherIndex],
                        indent + 4,
                        matcherIndex + 1U < region.matches.size());
                }
                appendIndent(output, indent + 3);
                output << "],\n";
            }
            if (region.isDefault) {
                appendJsonBooleanField(
                    output, indent + 3, "default", true);
            }
            appendDiagram(
                output,
                allNodes,
                allEdges,
                allWireFragments,
                100,
                Point{},
                indent + 3,
                node.id,
                region.id, storageMode);
            appendIndent(output, indent + 2);
            output << '}' << (commaAfter ? "," : "") << '\n';
        };
        for (std::size_t regionIndex = 0U;
             regionIndex < regions->size(); ++regionIndex) {
            appendRegion(
                (*regions)[regionIndex],
                regionIndex + 1U < regions->size());
        }
        appendIndent(output, indent + 1);
        output << "],\n";
        appendJsonBooleanField(
            output,
            indent + 1,
            "case_structure_true_visible",
            node.caseStructureTrueVisible);
        appendJsonField(
            output,
            indent + 1,
            "case_structure_visible_region_id",
            node.caseStructureVisibleRegionId.empty()
                ? (node.caseStructureTrueVisible
                    ? kCaseStructureTrueRegionId
                    : kCaseStructureFalseRegionId)
                : std::string_view{ node.caseStructureVisibleRegionId });
        appendJsonNumberField(
            output,
            indent + 1,
            "case_structure_selector_normalized_y",
            clampCaseStructureSelectorNormalizedY(
                node.caseStructureSelectorNormalizedY));
        output << document_internal::serializeCaseStructureProperties(node, indent + 1);
    }
    if (node.type == diagramStructureNodeType(
            DiagramStructureFamily::ForLoop)) {
        std::ostringstream bodyDiagram;
        appendDiagram(bodyDiagram, allNodes, allEdges, allWireFragments, 100,
            Point{}, indent + 3, node.id, kForLoopBodyRegionId, storageMode);
        output << document_internal::serializeForLoopStructureProperties(node, bodyDiagram.str(), indent + 1);
    }
    if (node.type == diagramStructureNodeType(
            DiagramStructureFamily::WhileLoop)) {
        std::ostringstream bodyDiagram;
        appendDiagram(bodyDiagram, allNodes, allEdges, allWireFragments, 100, Point{}, indent + 3, node.id, kWhileLoopBodyRegionId, storageMode);
        output << document_internal::serializeWhileLoopStructureProperties(node, bodyDiagram.str(), indent + 1);
    }
    if (node.type == diagramStructureNodeType(
            DiagramStructureFamily::TimedLoop)) { std::ostringstream bodyDiagram; appendDiagram(bodyDiagram, allNodes, allEdges, allWireFragments, 100, Point{}, indent + 3, node.id, kTimedLoopBodyRegionId, storageMode); output << document_internal::serializeTimedLoopStructureProperties(node, bodyDiagram.str(), indent + 1); }
    if (node.type == diagramStructureNodeType(
            DiagramStructureFamily::Event)) {
        std::vector<std::string> caseDiagrams;
        caseDiagrams.reserve(node.eventStructureCases.size());
        for (const auto &eventCase : node.eventStructureCases) {
            std::ostringstream caseDiagram;
            appendDiagram(caseDiagram, allNodes, allEdges, allWireFragments, 100,
                Point{}, indent + 3, node.id, eventCase.id, storageMode);
            caseDiagrams.push_back(caseDiagram.str());
        }
        output << document_internal::serializeEventStructureProperties(
            node, caseDiagrams, indent + 1);
    }
    const auto serializeRegion = [&](std::string_view regionId) {
        std::ostringstream diagram;
        appendDiagram(diagram, allNodes, allEdges, allWireFragments, 100,
            Point{}, indent + 3, node.id, regionId, storageMode);
        return diagram.str();
    };
    if (node.type == diagramStructureNodeType(
            DiagramStructureFamily::Disabled))
        output << document_internal::serializeDisabledStructureProperties(node, serializeRegion, indent + 1);
    if (node.type == diagramStructureNodeType(
            DiagramStructureFamily::ConditionalDisable))
        output << document_internal::serializeConditionalDisableProperties(node, serializeRegion, indent + 1);
    if (node.type=="frog.studio.structures.in_place_element_structure") {
        // Normalize missing historical allocators on a detached metadata-only
        // draft. Saving cannot publish edits into the live document.
        DiagramNode identities;identities.inPlacePairs=node.inPlacePairs;
        identities.inPlaceNextPairSerial=node.inPlaceNextPairSerial;
        seedInPlaceIdentityCounters(identities);
        appendJsonField(output,indent+1,"studio_in_place_profile",node.inPlaceSemanticProfile);
        appendJsonField(output,indent+1,"studio_in_place_next_pair_serial",std::to_string(identities.inPlaceNextPairSerial));
        document_internal::appendStructureTunnels(output,node,indent+1);
        document_internal::appendSubdiagramLabelAppearance(output,node,indent+1);
        appendJsonField(output,indent+1,"studio_subdiagram_label_text",node.forLoopSubdiagramLabelText);
        appendIndent(output,indent+1);output << "\"studio_in_place_pairs\": [\n";
        for(std::size_t i=0;i<node.inPlacePairs.size();++i) {
            const auto &p=node.inPlacePairs[i];appendIndent(output,indent+2);output << "{\n";
            appendJsonField(output,indent+3,"id",p.id);appendJsonField(output,indent+3,"kind",p.kind);
            appendJsonNumberField(output,indent+3,"y",p.y);
            appendJsonNumberField(output,indent+3,"split_dimension",p.splitDimension);
            appendJsonBooleanField(output,indent+3,"mark_as_modifier",p.markAsModifier);
            appendJsonBooleanField(output,indent+3,"allow_parallel_read_only",p.allowParallelReadOnly);
            appendJsonField(output,indent+3,"next_access_serial",std::to_string(identities.inPlacePairs[i].nextAccessSerial));
            appendIndent(output,indent+3);output << "\"accesses\": [\n";
            for(std::size_t j=0;j<p.accesses.size();++j) {
                const auto &a=p.accesses[j];appendIndent(output,indent+4);output << "{\n";
                appendJsonField(output,indent+5,"id",a.id);appendIndent(output,indent+5);output << "\"field_path\": [";
                for(std::size_t k=0;k<a.fieldPath.size();++k) {if(k) output << ',';appendJsonString(output,a.fieldPath[k]);}
                output << "]\n";appendIndent(output,indent+4);output << '}' << (j+1<p.accesses.size()?",":"") << '\n';
            }
            appendIndent(output,indent+3);output << "]\n";appendIndent(output,indent+2);output << '}' << (i+1<node.inPlacePairs.size()?",":"") << '\n';
        }
        appendIndent(output,indent+1);output << "],\n";
        appendIndent(output,indent+1);output << "\"regions\": [{\n";
        appendJsonField(output,indent+2,"id","body");output << serializeRegion("body");
        appendIndent(output,indent+1);output << "}],\n";
    }
    if (isSequenceStructure(node)) {
        document_internal::appendStructureTunnels(output, node, indent + 1);
        document_internal::appendSubdiagramLabelAppearance(output, node, indent + 1);
        // Explicit Studio-private profile, not an executable standard extension.
        appendJsonField(output, indent + 1, "studio_sequence_profile", node.sequenceSemanticProfile);
        appendJsonField(output, indent + 1, "studio_sequence_selected_frame", node.sequenceSelectedFrameId);
        appendIndent(output, indent + 1); output << "\"regions\": [\n";
        for (std::size_t i=0; i<node.sequenceFrames.size(); ++i) {
            const auto &frame=node.sequenceFrames[i];
            appendIndent(output, indent + 2); output << "{\n";
            appendJsonField(output, indent + 3, "id", frame.id);
            appendJsonField(output, indent + 3, "studio_title", frame.title);
            appendJsonField(output, indent + 3, "studio_subdiagram_label_text", frame.subdiagramLabelText);
            appendJsonNumberField(output, indent + 3, "studio_width", sequenceFrameLayout(node,i).width);
            output << serializeRegion(frame.id);
            appendIndent(output, indent + 2); output << '}' << (i+1<node.sequenceFrames.size()?",":"") << '\n';
        }
        appendIndent(output, indent + 1); output << "],\n";
    }
    appendJsonBooleanField(output, indent + 1, "hidden", node.hidden);
    if (node.constantTextBoundsAutomatic.has_value())
        appendJsonBooleanField(output, indent + 1, "constant_text_bounds_automatic", *node.constantTextBoundsAutomatic);
    if (node.constantArrayCellBoundsAutomatic.has_value())
        appendJsonBooleanField(output, indent + 1, "constant_array_cell_bounds_automatic", *node.constantArrayCellBoundsAutomatic);
    appendJsonBooleanField(output, indent + 1, "locked", node.locked);
    appendJsonField(output, indent + 1, "group", node.groupId);
    appendIndent(output, indent + 1);
    output << "\"layout\": {\n";
    appendJsonNumberField(output, indent + 2, "x", node.layout.x);
    appendJsonNumberField(output, indent + 2, "y", node.layout.y);
    appendJsonNumberField(output, indent + 2, "width", node.layout.width);
    appendJsonNumberField(output, indent + 2, "height", node.layout.height, false);
    appendIndent(output, indent + 1);
    output << "}\n";
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendDiagram(
    std::ostringstream &output,
    const std::vector<DiagramNode> &nodes,
    const std::vector<DiagramEdge> &edges,
    const std::vector<DiagramWireFragment> &wireFragments,
    int zoomPercent,
    Point viewportOrigin,
    int indent,
    std::string_view ownerStructureId,
    std::string_view ownerRegionId, WidgetWriteContext &storageMode) {
    std::vector<const DiagramNode *> serializableNodes;
    serializableNodes.reserve(nodes.size());
    for (const auto &node : nodes) {
        if (node.diagramOwnerStructureId == ownerStructureId &&
            node.diagramOwnerRegionId == ownerRegionId &&
            (node.kind != "comment" || !node.text.empty())) {
            serializableNodes.push_back(&node);
        }
    }
    std::vector<const DiagramEdge *> serializableEdges;
    serializableEdges.reserve(edges.size());
    for (const auto &edge : edges) {
        if (edge.diagramOwnerStructureId == ownerStructureId &&
            edge.diagramOwnerRegionId == ownerRegionId) {
            serializableEdges.push_back(&edge);
        }
    }
    std::vector<const DiagramWireFragment *> serializableFragments;
    serializableFragments.reserve(wireFragments.size());
    for (const auto &fragment : wireFragments) {
        if (fragment.diagramOwnerStructureId == ownerStructureId &&
            fragment.diagramOwnerRegionId == ownerRegionId) {
            serializableFragments.push_back(&fragment);
        }
    }

    appendIndent(output, indent);
    output << "\"diagram\": {\n";
    appendJsonIntegerField(
        output,
        indent + 1,
        "zoom_percent",
        static_cast<std::uint32_t>(std::clamp(zoomPercent, 30, 150)));
    if (std::abs(viewportOrigin.x) > 0.001 || std::abs(viewportOrigin.y) > 0.001) {
        appendIndent(output, indent + 1);
        output << "\"viewport_origin\": {\n";
        appendJsonNumberField(output, indent + 2, "x", viewportOrigin.x);
        appendJsonNumberField(output, indent + 2, "y", viewportOrigin.y, false);
        appendIndent(output, indent + 1);
        output << "},\n";
    }
    appendIndent(output, indent + 1);
    output << "\"nodes\": [";
    if (!serializableNodes.empty()) {
        output << '\n';
        for (std::size_t index = 0; index < serializableNodes.size(); ++index) {
            appendDiagramNode(
                output,
                *serializableNodes[index],
                indent + 2,
                index + 1U < serializableNodes.size(),
                nodes,
                edges,
                wireFragments, storageMode);
        }
        appendIndent(output, indent + 1);
    }
    output << "],\n";
    appendIndent(output, indent + 1);
    output << "\"edges\": [";
    if (!serializableEdges.empty()) {
        output << '\n';
        for (std::size_t index = 0; index < serializableEdges.size(); ++index) {
            const auto &edge = *serializableEdges[index];
            appendIndent(output, indent + 2);
            output << "{\n";
            appendJsonField(output, indent + 3, "id", edge.id);
            appendIndent(output, indent + 3);
            output << "\"from\": {\n";
            appendJsonField(output, indent + 4, "node", edge.from.nodeId);
            appendJsonField(output, indent + 4, "port", edge.from.portId, false);
            appendIndent(output, indent + 3);
            output << "},\n";
            appendIndent(output, indent + 3);
            output << "\"to\": {\n";
            appendJsonField(output, indent + 4, "node", edge.to.nodeId);
            appendJsonField(output, indent + 4, "port", edge.to.portId, false);
            appendIndent(output, indent + 3);
            output << '}';
            const bool hasLabel = !edge.label.empty();
            if (!edge.bends.empty() || hasLabel) {
                output << ",\n";
            } else {
                output << '\n';
            }
            if (!edge.bends.empty()) {
                appendIndent(output, indent + 3);
                output << "\"bends\": [\n";
                for (std::size_t bendIndex = 0; bendIndex < edge.bends.size(); ++bendIndex) {
                    const auto &bend = edge.bends[bendIndex];
                    appendIndent(output, indent + 4);
                    output << "{\n";
                    appendJsonNumberField(output, indent + 5, "x", bend.x);
                    appendJsonNumberField(output, indent + 5, "y", bend.y, false);
                    appendIndent(output, indent + 4);
                    output << '}';
                    if (bendIndex + 1U < edge.bends.size()) {
                        output << ',';
                    }
                    output << '\n';
                }
                appendIndent(output, indent + 3);
                output << ']';
                output << (hasLabel ? ",\n" : "\n");
            }
            if (hasLabel) {
                appendIndent(output, indent + 3);
                output << "\"label\": {\n";
                appendJsonField(output, indent + 4, "text", edge.label);
                appendJsonNumberField(
                    output,
                    indent + 4,
                    "normalized_position",
                    std::clamp(edge.labelNormalizedPosition, 0.0, 1.0));
                appendJsonNumberField(
                    output,
                    indent + 4,
                    "offset_x",
                    std::isfinite(edge.labelOffsetX) ? edge.labelOffsetX : 0.0);
                appendJsonNumberField(
                    output,
                    indent + 4,
                    "offset_y",
                    std::isfinite(edge.labelOffsetY) ? edge.labelOffsetY : 0.0,
                    false);
                appendIndent(output, indent + 3);
                output << "}\n";
            }
            appendIndent(output, indent + 2);
            output << '}';
            if (index + 1U < serializableEdges.size()) {
                output << ',';
            }
            output << '\n';
        }
        appendIndent(output, indent + 1);
    }
    output << "],\n";
    appendIndent(output, indent + 1);
    output << "\"wire_fragments\": [";
    if (!serializableFragments.empty()) {
        output << '\n';
        for (std::size_t index = 0; index < serializableFragments.size(); ++index) {
            const auto &fragment = *serializableFragments[index];
            appendIndent(output, indent + 2);
            output << "{\n";
            appendJsonField(output, indent + 3, "id", fragment.id);
            if (!fragment.detachedEdgeId.empty())
                appendJsonField(output, indent + 3, "detached_edge_id", fragment.detachedEdgeId);
            if (!fragment.label.empty()) {
                appendIndent(output, indent + 3);
                output << "\"label\": {\n";
                appendJsonField(output, indent + 4, "text", fragment.label);
                appendJsonNumberField(output, indent + 4, "normalized_position",
                    std::clamp(fragment.labelNormalizedPosition, 0.0, 1.0));
                appendJsonNumberField(output, indent + 4, "offset_x",
                    std::isfinite(fragment.labelOffsetX) ? fragment.labelOffsetX : 0.0);
                appendJsonNumberField(output, indent + 4, "offset_y",
                    std::isfinite(fragment.labelOffsetY) ? fragment.labelOffsetY : 0.0, false);
                appendIndent(output, indent + 3);
                output << "},\n";
            }
            appendIndent(output, indent + 3);
            output << "\"points\": [\n";
            for (std::size_t pointIndex = 0; pointIndex < fragment.points.size(); ++pointIndex) {
                const auto &point = fragment.points[pointIndex];
                appendIndent(output, indent + 4);
                output << "{\n";
                appendJsonNumberField(output, indent + 5, "x", point.x);
                appendJsonNumberField(output, indent + 5, "y", point.y, false);
                appendIndent(output, indent + 4);
                output << '}';
                if (pointIndex + 1U < fragment.points.size()) {
                    output << ',';
                }
                output << '\n';
            }
            appendIndent(output, indent + 3);
            output << ']';
            if (fragment.source.has_value() || fragment.sink.has_value() ||
                !fragment.typeHint.empty() || fragment.typeHintAuthoritative) {
                output << ",\n";
            } else {
                output << '\n';
            }
            const auto appendEndpoint = [&](
                std::string_view name,
                const DiagramEdgeEndpoint &endpoint,
                bool comma) {
                appendIndent(output, indent + 3);
                output << '\"' << name << "\": {\n";
                appendJsonField(output, indent + 4, "node", endpoint.nodeId);
                appendJsonField(output, indent + 4, "port", endpoint.portId, false);
                appendIndent(output, indent + 3);
                output << '}';
                if (comma) {
                    output << ',';
                }
                output << '\n';
            };
            if (fragment.source.has_value()) {
                appendEndpoint(
                    "source",
                    fragment.source.value(),
                    fragment.sink.has_value() || !fragment.typeHint.empty() ||
                        fragment.typeHintAuthoritative);
            }
            if (fragment.sink.has_value()) {
                appendEndpoint(
                    "sink",
                    fragment.sink.value(),
                    !fragment.typeHint.empty() || fragment.typeHintAuthoritative);
            }
            if (!fragment.typeHint.empty()) {
                appendJsonField(
                    output,
                    indent + 3,
                    "type_hint",
                    fragment.typeHint,
                    fragment.typeHintAuthoritative);
            }
            if (fragment.typeHintAuthoritative) {
                appendJsonBooleanField(
                    output,
                    indent + 3,
                    "type_hint_authoritative",
                    true,
                    false);
            }
            appendIndent(output, indent + 2);
            output << '}';
            if (index + 1U < serializableFragments.size()) {
                output << ',';
            }
            output << '\n';
        }
        appendIndent(output, indent + 1);
    }
    output << "]\n";
    appendIndent(output, indent);
    output << "}\n";
}

void appendRuntime(std::ostringstream &output, const WidgetInstance &widget, int indent, bool comma = true) {
    appendIndent(output, indent);
    output << "\"runtime\": {\n";
    appendJsonField(output, indent + 1, "sourceExample", widget.sourceExamplePath);
    appendJsonField(output, indent + 1, "wfrog", widget.wfrogPackagePath);
    appendJsonField(output, indent + 1, "realization", widget.realizationPackageRef);
    appendJsonField(output, indent + 1, "asset", widget.assetRef, widget.popupAssetRef.has_value() || widget.defaultSvgAssetPath.has_value());
    if (widget.popupAssetRef.has_value()) {
        appendJsonField(output, indent + 1, "popupAsset", widget.popupAssetRef.value(), widget.defaultSvgAssetPath.has_value());
    }
    if (widget.defaultSvgAssetPath.has_value()) {
        appendJsonField(output, indent + 1, "defaultSvg", widget.defaultSvgAssetPath.value(), false);
    }
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendWidgetPropertyOverrides(std::ostringstream &output, const WidgetInstance &widget, int indent,
    WidgetWriteContext &storageMode, std::vector<DeferredWidgetWrite> &deferred, bool comma = true) {
    appendIndent(output, indent);
    output << "\"props\": {\n";

    bool wrote = false;
    if(widget.typeDefinition) {
        storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,kFrogTypeDefinitionDraftRevision);
        appendIndent(output,indent+1);output << "\"type_definition\": " << serializeTypeDefinitionLink(*widget.typeDefinition);
        wrote=true;
    }
    if(isXYGraphWidget(widget) && widget.xyGraph) {
        if(wrote)output << ",\n";
        appendIndent(output,indent+1);
        output << "\"xy_graph\": " << document_internal::serializeXYGraph(*widget.xyGraph);
        wrote=true;
    }
    if (isWaveformGraphWidget(widget) && widget.waveformGraph) {
        if(wrote)output << ",\n";
        appendIndent(output, indent + 1);
        output << "\"waveform_graph\": " << document_internal::serializeWaveformGraph(*widget.waveformGraph);
        wrote = true;
    }
    const auto appendCommaIfNeeded = [&]() {
        if (wrote) {
            output << ",\n";
        }
        wrote = true;
    };
    if(widget.collection) {
        appendCommaIfNeeded();appendIndent(output,indent+1);output << "\"collection.state\": ";
        document_internal::appendJsonValue(output,writeCollectionAuthoringState(*widget.collection));
    }
    const auto appendBool = [&](std::string_view name, bool value) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        output << ": " << (value ? "true" : "false");
    };
    const auto appendUnsigned = [&](std::string_view name, std::uint32_t value) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        output << ": " << value;
    };
    const auto appendNumber = [&](std::string_view name, double value) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        output << ": " << formatNumber(value);
    };
    const auto appendString = [&](std::string_view name, std::string_view value) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        output << ": ";
        appendJsonString(output, value);
    };
    const auto appendUnsignedArray = [&](std::string_view name, const std::vector<std::uint32_t> &values) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        output << ": [";
        for (std::size_t index = 0; index < values.size(); ++index) {
            if (index != 0U) {
                output << ", ";
            }
            output << values[index];
        }
        output << ']';
    };
    if (widget.valueType == "variant") {
        appendString("variant.payload",widget.initialValue.value_or(""));
        if (widget.variantShowType) appendBool("variant.show_type",*widget.variantShowType);
        if (widget.variantShowData) appendBool("variant.show_data",*widget.variantShowData);
        if (widget.variantScrollbarVisible) appendBool("variant.scrollbar_visible",*widget.variantScrollbarVisible);
    }
    const auto ringItemsAreSequential = [&]() {
        for (std::size_t index = 0; index < widget.ringItems.size(); ++index) {
            if (widget.ringItems[index].value != static_cast<std::uint32_t>(index)) {
                return false;
            }
        }
        return true;
    };
    const auto appendRingItems = [&]() {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, "items");
        output << ": [\n";
        for (std::size_t index = 0; index < widget.ringItems.size(); ++index) {
            const auto &item = widget.ringItems[index];
            appendIndent(output, indent + 2);
            output << "{ ";
            appendJsonString(output, "id");
            output << ": ";
            appendJsonString(output, item.id.empty() ? "item_" + std::to_string(index) : item.id);
            output << ", ";
            appendJsonString(output, isEnumWidget(widget) ? "text" : "label");
            output << ": ";
            appendJsonString(output, item.label);
            output << ", ";
            appendJsonString(output, isEnumWidget(widget) ? "numeric_value" : "value");
            output << ": " << item.value;
            output << ", ";
            appendJsonString(output, "enabled");
            output << ": " << (item.disabled ? "false" : "true");
            output << ", ";
            appendJsonString(output, "visible");
            output << ": " << (item.visible ? "true" : "false");
            if (isTabWidget(widget)) {
                output << ", ";
                appendJsonString(output, "caption");
                output << ": ";
                appendJsonString(output, item.caption);
                output << ", ";
                appendJsonString(output, "caption_matches_label");
                output << ": " << (item.captionMatchesLabel ? "true" : "false");
            }
            output << " }";
            if (index + 1U < widget.ringItems.size()) {
                output << ',';
            }
            output << '\n';
        }
        appendIndent(output, indent + 1);
        output << ']';
    };

    if (widget.contextHelpDescription.has_value()) {
        appendString("documentation.description", widget.contextHelpDescription.value());
    }
    if (widget.contextTip.has_value()) {
        appendString("documentation.tip", widget.contextTip.value());
    }
    if (widget.defaultData.has_value()) {
        storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,storedPayloadStateRevision(*widget.defaultData));
        appendString("data.default", *widget.defaultData);
    }
    appendNumber("layout.aura_band_px", widgetPlacementAuraBandPx(widget));
    if (widget.contextHelpUrl.has_value()) {
        appendString("documentation.url", widget.contextHelpUrl.value());
    }

    if (isLabelWidget(widget)) {
        appendBool("comment_auto_size", widget.commentAutoSize);
        appendString("label.text", widget.label);
        appendString("text.value", widgetSupportTextValue(widget));
        appendBool("text.visible", widgetLabelVisible(widget));
        appendBool("text.multiline", widgetSupportTextValue(widget).find('\n') != std::string_view::npos);
        appendBool("text.wrap_enabled", false);
        appendString("text.horizontal_alignment", widgetLabelHorizontalAlignment(widget));
        appendString("text.vertical_alignment", widgetLabelVerticalAlignment(widget));
        appendString("text.padding", "0px");
        appendBool("text.clip_overflow", true);
        if (widget.labelTextRgb.has_value()) {
            appendString("style.text.color", formatRgbHex(widget.labelTextRgb.value()));
        }
        if (widget.labelSurfaceFillRgb.has_value()) {
            appendString("style.surface.fill_color", formatRgbHex(widget.labelSurfaceFillRgb.value()));
        }
        if (widget.labelSurfaceStrokeRgb.has_value()) {
            appendString("style.surface.stroke_color", formatRgbHex(widget.labelSurfaceStrokeRgb.value()));
        }
        if (widget.labelFontFamily.has_value()) {
            appendString("style.text.font_family", widget.labelFontFamily.value());
        }
        appendString("style.text.font_size", formatNumber(15.0 * widgetLabelFontScale(widget)) + "px");
        const auto style = widgetLabelTextStyle(widget);
        appendString("style.text.font_weight", fontWeightForTextStyle(style));
        appendString("style.text.font_style", fontStyleForTextStyle(style));
        appendString("style.text.text_decoration", textDecorationForTextStyle(style));
        appendBool("interaction.visible", true);
        appendBool("interaction.enabled", false);
        appendBool("interaction.read_only", true);
        appendString("realization.variant", "rectangular");
    }
    if (const auto *array = arrayContainerState(widget)) {
        if (array->frameFillRgb.has_value()) {
            appendString("style.frame.fill_color", formatRgbHex(array->frameFillRgb.value()));
        }
        if (array->frameStrokeRgb.has_value()) {
            appendString("style.frame.border_color", formatRgbHex(array->frameStrokeRgb.value()));
        }
        if (array->arrayRegionFillRgb.has_value()) {
            appendString("style.array_region.fill_color", formatRgbHex(array->arrayRegionFillRgb.value()));
        }
        if (array->arrayRegionStrokeRgb.has_value()) {
            appendString("style.array_region.border_color", formatRgbHex(array->arrayRegionStrokeRgb.value()));
        }
        if (array->indexContainerFillRgb.has_value()) {
            appendString("style.index_display.container_fill_color", formatRgbHex(array->indexContainerFillRgb.value()));
        }
        if (array->indexContainerStrokeRgb.has_value()) {
            appendString("style.index_display.container_border_color", formatRgbHex(array->indexContainerStrokeRgb.value()));
        }
        if (array->indexValueFillRgb.has_value()) {
            appendString("style.index_display.value_fill_color", formatRgbHex(array->indexValueFillRgb.value()));
        }
        if (array->indexValueStrokeRgb.has_value()) {
            appendString("style.index_display.value_border_color", formatRgbHex(array->indexValueStrokeRgb.value()));
        }
        if (array->indexStepButtonFillRgb.has_value()) {
            appendString("style.index_display.button_fill_color", formatRgbHex(array->indexStepButtonFillRgb.value()));
        }
        if (array->indexStepButtonStrokeRgb.has_value()) {
            appendString("style.index_display.button_border_color", formatRgbHex(array->indexStepButtonStrokeRgb.value()));
        }
        if (array->indexStepArrowFillRgb.has_value()) {
            appendString("style.index_display.arrow_fill_color", formatRgbHex(array->indexStepArrowFillRgb.value()));
        }
        if (array->indexStepArrowStrokeRgb.has_value()) {
            appendString("style.index_display.arrow_border_color", formatRgbHex(array->indexStepArrowStrokeRgb.value()));
        }
        appendUnsigned("dimensions.rank", array->rank);
        appendUnsignedArray("dimensions.shape", array->shape);
        appendBool("dimensions.allow_resize", array->allowResize);
        appendUnsignedArray("viewport.first_indices", array->firstIndices);
        appendUnsignedArray("viewport.visible_counts", array->visibleCounts);
        appendString("viewport.orientation", array->orientation);
        appendNumber("viewport.scrollbar_gap", array->scrollbarGap);
        appendBool("index_display.visible", array->indexDisplayVisible);
        appendNumber("index_display.layout.box_width", array->indexDisplayWidth);
        appendNumber("index_display.layout.box_height", array->indexBoxHeight);
        appendNumber("index_display.layout.gap", array->indexBoxGap);
        appendNumber("index_display.layout.step_width", array->indexStepWidth);
        appendNumber("style.index_display.step_gap", array->indexStepGap);
        appendNumber("style.index_display.value_gap", array->indexValueGap);
        appendString(
            "style.index_display.container_padding",
            formatNumber(array->indexContainerPadding) + "px");
        appendString(
            "style.index_display.container_border_width",
            formatNumber(array->indexContainerBorderWidth) + "px");
        appendBool("display.vertical_scrollbar_visible", array->verticalScrollbarVisible);
        appendBool("display.horizontal_scrollbar_visible", array->horizontalScrollbarVisible);
        appendNumber("element.cell_width", array->cellWidth);
        appendNumber("element.cell_height", array->cellHeight);
        appendNumber("element.gap", array->elementGap);
        const std::string elementClassId = array->elementTemplate != nullptr
            ? array->elementTemplate->classId
            : array->elementClassId;
        const std::string elementValueType = array->elementTemplate != nullptr
            ? array->elementTemplate->valueType
            : array->elementValueType;
        const std::string elementRole = array->elementTemplate != nullptr
            ? array->elementTemplate->role
            : array->elementRole;
        if (!elementClassId.empty()) {
            appendString("element.class_id", elementClassId);
        }
        if (!elementValueType.empty()) {
            appendString("element.value_type", elementValueType);
        }
        if (!elementRole.empty()) {
            appendString("element.role", elementRole);
        }
        if (array->elementTemplate != nullptr && array->widgetComposedElements) {
            appendCommaIfNeeded();
            appendJsonBooleanField(output, indent + 1, "element.icon", array->elementIcon, false);

            appendCommaIfNeeded();
            appendIndent(output, indent + 1);
            appendJsonString(output, "element.template");
            output << ": ";
            deferred.push_back({static_cast<std::size_t>(output.tellp()),
                array->elementTemplate.get(),indent+1,false,{},{}});
        }
        if (!array->elements.empty() && storageMode.mode == FrogArrayStorageMode::Readable) {
            appendCommaIfNeeded();
            appendIndent(output, indent + 1);
            appendJsonString(output, "elements");
            output << ": [\n";
            for (std::size_t index = 0; index < array->elements.size(); ++index) {
                const auto &element = array->elements[index];
                if(element.kind==ArrayElementValueKind::Widget)
                    storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,storedPayloadStateRevision(element.displayValue));
                appendIndent(output, indent + 2);
                output << "{\"indices\": [";
                for (std::size_t dimension = 0; dimension < element.indices.size(); ++dimension) {
                    if (dimension != 0U) {
                        output << ", ";
                    }
                    output << element.indices[dimension];
                }
                output << "], \"value\": ";
                // Aggregate cells already contain a JSON object. Quoting that
                // object at every enclosing Array doubles its escape sequences
                // repeatedly, even when there is only one edited scalar leaf.
                const bool complexNumber = element.kind == ArrayElementValueKind::Number &&
                    numericCanonicalValueTypeIsComplex(elementValueType);
                if (complexNumber) {
                    auto validation = array->elementTemplate ? *array->elementTemplate : WidgetInstance{};
                    validation.classId = elementClassId; validation.valueType = elementValueType;
                    if (!setWidgetNumericComplexValueLexeme(validation, element.displayValue))
                        throw std::runtime_error("Cannot serialize invalid complex Array cell.");
                    storageMode.requiredStateRevision = (std::max)(storageMode.requiredStateRevision, kFrogComplexStateDraftRevision);
                }
                if (complexNumber || element.kind == ArrayElementValueKind::String || storedWidgetNeedsExactString(element)) {
                    appendJsonString(output, element.displayValue);
                } else {
                    output << element.displayValue;
                }
                if (element.kind == ArrayElementValueKind::Widget) output << ", \"kind\": \"widget\"";
                if (complexNumber) output << ", \"kind\": \"number\"";
                if (!element.defined) {
                    output << ", \"defined\": false";
                }
                if (!element.enabled) {
                    output << ", \"enabled\": false";
                }
                output << '}';
                if (index + 1U != array->elements.size()) output << ',';
                output << '\n';
            }
            appendIndent(output, indent + 1);
            output << ']';
        } else if (!array->elements.empty()) {
            appendCommaIfNeeded();
            appendIndent(output, indent + 1);
            appendJsonString(output, "elements");
            std::vector<std::string> records;
            records.reserve(array->elements.size());
            // The existing compact record profile admits JSON numbers only.
            // Complex cells retain their complete readable lexical record.
            bool numericOnly = document_internal::compactNumericArrayType(elementValueType) &&
                !numericCanonicalValueTypeIsComplex(elementValueType);
            std::ostringstream readable;
            readable << "[\n";
            for (std::size_t index = 0; index < array->elements.size(); ++index) {
                const auto &element = array->elements[index];
                if(element.kind==ArrayElementValueKind::Widget)
                    storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,storedPayloadStateRevision(element.displayValue));
                numericOnly = numericOnly && element.kind == ArrayElementValueKind::Number;
                std::ostringstream record;
                record << "{\"indices\": [";
                for (std::size_t dimension = 0; dimension < element.indices.size(); ++dimension) {
                    if (dimension != 0U) {
                        record << ", ";
                    }
                    record << element.indices[dimension];
                }
                record << "], \"value\": ";
                const bool complexNumber = element.kind == ArrayElementValueKind::Number &&
                    numericCanonicalValueTypeIsComplex(elementValueType);
                if (complexNumber) {
                    auto validation = array->elementTemplate ? *array->elementTemplate : WidgetInstance{};
                    validation.classId = elementClassId; validation.valueType = elementValueType;
                    if (!setWidgetNumericComplexValueLexeme(validation, element.displayValue))
                        throw std::runtime_error("Cannot serialize invalid complex Array cell.");
                    storageMode.requiredStateRevision = (std::max)(storageMode.requiredStateRevision, kFrogComplexStateDraftRevision);
                }
                if (complexNumber || element.kind == ArrayElementValueKind::String || storedWidgetNeedsExactString(element)) {
                    appendJsonString(record, element.displayValue);
                } else {
                    record << element.displayValue;
                }
                if (element.kind == ArrayElementValueKind::Widget) record << ", \"kind\": \"widget\"";
                if (complexNumber) record << ", \"kind\": \"number\"";
                if (!element.defined) {
                    record << ", \"defined\": false";
                }
                if (!element.enabled) {
                    record << ", \"enabled\": false";
                }
                record << '}';
                records.push_back(record.str());
                appendIndent(readable, indent + 2);
                readable << records.back();
                if (index + 1U != array->elements.size()) readable << ',';
                readable << '\n';
            }
            appendIndent(readable, indent + 1);
            readable << ']';
            output << ": ";
            if(numericOnly) {
                // A child template consumed storage budgets before its parent's
                // records in the original writer. Preserve that ordering too.
                deferred.push_back({static_cast<std::size_t>(output.tellp()),nullptr,0,false,
                    std::move(records),readable.str()});
            } else output << readable.str();
        }
    }
    if (const auto *cluster = clusterContainerState(widget)) {
        appendNumber("cluster.padding", cluster->padding);
        if(cluster->nextFieldSerial)appendString("cluster.next_field_serial",std::to_string(cluster->nextFieldSerial));
        appendString("cluster.auto_sizing", std::string(clusterAutoSizingName(cluster->autoSizing)));
        if (cluster->frameFillRgb.has_value()) {
            appendString(
                "style.frame.fill_color",
                formatRgbHex(cluster->frameFillRgb.value()));
        }
        if (cluster->frameStrokeRgb.has_value()) {
            appendString(
                "style.frame.border_color",
                formatRgbHex(cluster->frameStrokeRgb.value()));
        }
        if (!cluster->elements.empty()) {
            appendCommaIfNeeded();
            appendIndent(output, indent + 1);
            appendJsonString(output, "cluster.elements");
            output << ": [\n";
            for (std::size_t index = 0U; index < cluster->elements.size(); ++index) {
                const auto &field = cluster->elements[index];
                appendIndent(output, indent + 2);
                output << "{\n";
                appendJsonField(output, indent + 3, "field_id", field.fieldId);
                appendJsonField(output, indent + 3, "technical_name", field.technicalName);
                appendJsonNumberField(
                    output,
                    indent + 3,
                    "logical_order",
                    static_cast<double>(field.logicalOrder));
                appendIndent(output, indent + 3);
                appendJsonString(output, "widget");
                output << ": ";
                if (field.widget != nullptr) {
                    deferred.push_back({static_cast<std::size_t>(output.tellp()),
                        field.widget.get(),indent+3,false,{},{}});
                } else {
                    output << "null\n";
                }
                appendIndent(output, indent + 2);
                output << '}';
                if (index + 1U != cluster->elements.size()) {
                    output << ',';
                }
                output << '\n';
            }
            appendIndent(output, indent + 1);
            output << ']';
        }
    }
    if (widget.labelVisible.has_value()) {
        appendBool("label.visible", widget.labelVisible.value());
    }
    if (widget.caption.has_value() || widget.captionVisible.has_value()) {
        // Older documents used caption.* as aliases for label.*.
        appendBool("caption.independent", true);
        if (widget.caption) appendString("caption.text", *widget.caption);
        appendBool("caption.visible", widget.captionVisible.value_or(false));
    }
    if (isNumericWidget(widget)) {
        if (widget.numericColorBox && widget.valueType == "u32") appendBool("display.color_box", true);
        appendString("data_type.representation", widget.valueType);
        appendString("data_type.named_numeric_size", numericNamedSize(widget));
        appendString("representation.kind", numericRepresentationKind(widget));
        if (numericRepresentationKind(widget) == "fixed_point") {
            const auto configuration = fixedPointConfiguration(widget);
            appendBool("representation.fixed_point.signed", configuration.signedEncoding);
            appendBool("representation.fixed_point.complex", configuration.complex);
            appendNumber("representation.fixed_point.word_length", configuration.wordLength);
            appendNumber(
                "representation.fixed_point.integer_word_length",
                configuration.integerWordLength);
            appendString(
                "representation.fixed_point.rounding_mode",
                configuration.roundingMode);
            appendString(
                "representation.fixed_point.overflow_mode",
                configuration.overflowMode);
        }
    }
    if (isNumericWidget(widget) || isRingWidget(widget)) {
        const bool useTypeDefaults = numericUsesTypeDefaultDataEntry(widget);
        appendBool("data_entry.use_type_defaults", useTypeDefaults);
        if (!useTypeDefaults) {
            if (widget.numericMinimum.has_value()) {
                appendNumber("data_entry.minimum", widget.numericMinimum.value());
            }
            if (widget.numericMaximum.has_value()) {
                appendNumber("data_entry.maximum", widget.numericMaximum.value());
            }
            if (widget.numericMinimumResponse.has_value()) {
                appendString("data_entry.minimum_response", widget.numericMinimumResponse.value());
            }
            if (widget.numericMaximumResponse.has_value()) {
                appendString("data_entry.maximum_response", widget.numericMaximumResponse.value());
            }
            if (widget.numericIncrementStep.has_value()) {
                appendNumber("data_entry.increment_step", widget.numericIncrementStep.value());
            }
            if (widget.numericIncrementResponse.has_value()) {
                appendString("data_entry.increment_response", widget.numericIncrementResponse.value());
            }
            if (widget.numericPageStep.has_value()) {
                appendNumber("data_entry.page_step", widget.numericPageStep.value());
            }
        }
    }
    if (widget.valueType == "timestamp") {
        if (widget.timestampDisplayContent) appendString("timestamp.display.content", *widget.timestampDisplayContent);
        if (widget.timestampShowSeconds) appendBool("timestamp.display.seconds", *widget.timestampShowSeconds);
        if (widget.timestampDateFirst) appendBool("timestamp.display.date_first", *widget.timestampDateFirst);
        if (widget.timestampSingleLine) appendBool("timestamp.display.single_line", *widget.timestampSingleLine);
        if (widget.timestampClockIconVisible) appendBool("timestamp.clock.visible", *widget.timestampClockIconVisible);
    }
    if (isNumericWidget(widget) || isRingWidget(widget) || isEnumWidget(widget)) {
        if (widget.numericDisplayFormat.has_value()) {
            appendString("display.format.kind", widget.numericDisplayFormat.value());
        }
        if ((isRingWidget(widget) || isEnumWidget(widget)) &&
            widget.choiceNumericValueVisible.has_value()) {
            appendBool(
                "display.format.show_numeric_value",
                widget.choiceNumericValueVisible.value());
        }
        if (widget.numericDisplayDigits.has_value()) {
            appendUnsigned("display.format.digits", static_cast<std::uint32_t>((std::max)(1, widget.numericDisplayDigits.value())));
        }
        if (widget.numericPrecisionType.has_value()) {
            appendString("display.format.precision_type", widget.numericPrecisionType.value());
        }
        if (widget.numericHideTrailingZeros.has_value()) {
            appendBool("display.format.hide_trailing_zeros", widget.numericHideTrailingZeros.value());
        }
        if (widget.numericExponentMultipleOfThree.has_value()) {
            appendBool("display.format.exponent_multiple_of_3", widget.numericExponentMultipleOfThree.value());
        }
        if (widget.numericMinimumFieldWidth.has_value()) {
            appendUnsigned(
                "display.format.minimum_field_width",
                static_cast<std::uint32_t>((std::max)(0, widget.numericMinimumFieldWidth.value())));
        }
        if (widget.numericPadding.has_value()) {
            appendString("display.format.padding", widget.numericPadding.value());
        }
        if (widget.numericAlignment.has_value()) {
            appendString("display.format.alignment", widget.numericAlignment.value());
        }
        if (widget.numericSignDisplay.has_value()) {
            appendString("display.format.sign", widget.numericSignDisplay.value());
        }
        if (widget.numericLocale.has_value()) {
            appendString("display.format.locale", widget.numericLocale.value());
        }
        if (widget.numericDisplayPrefix.has_value()) {
            appendString("display.format.prefix", widget.numericDisplayPrefix.value());
        }
        if (widget.numericDisplaySuffix.has_value()) {
            appendString("display.format.suffix", widget.numericDisplaySuffix.value());
        }
        if (widget.numericRadixShowPrefix.has_value()) {
            appendBool("display.format.radix.show_prefix", widget.numericRadixShowPrefix.value());
        }
        if (widget.numericRadixUppercase.has_value()) {
            appendBool("display.format.radix.uppercase", widget.numericRadixUppercase.value());
        }
        if (widget.numericRadixGroupSize.has_value()) {
            appendUnsigned(
                "display.format.radix.group_size",
                static_cast<std::uint32_t>(std::clamp(widget.numericRadixGroupSize.value(), 0, 8)));
        }
        if (widget.numericRadixInterpretation.has_value()) {
            appendString("display.format.radix.interpretation", widget.numericRadixInterpretation.value());
        }
        if (widget.numericCustomFormatString.has_value()) {
            appendString("display.format.custom_string", widget.numericCustomFormatString.value());
        }
        if (isNumericWidget(widget) && widget.numericRadixVisible.has_value()) {
            appendBool("display.radix.visible", widget.numericRadixVisible.value());
        }
    }
    if (!isLabelWidget(widget) && widgetEnabledState(widget) != WidgetEnabledState::Enabled) {
        appendBool("interaction.enabled", false);
        appendNumber(
            "style.disabled.opacity",
            widgetDisabledAppearanceGrayed(widget) ? 0.5 : 1.0);
    }
    if (isStringWidget(widget)) {
        if (isStringDropdownWidget(widget)) {
            appendBool("display.string.dropdown", true);
            appendString("value.selected_id", ringSelectedItemId(widget));
            appendUnsigned("value.selected_value", static_cast<std::uint32_t>(widget.numericValue.value_or(0.0)));
            appendRingItems();
        }
        if (widget.supportTextValue.has_value()) {
            appendString("text.value", widgetStringValue(widget));
        }
        appendString("display.string.style", stringDisplayStyle(widget));
        if (!isStringDropdownWidget(widget))
            appendString("text.horizontal_alignment", widgetStringTextHorizontalAlignment(widget));
        appendBool("display.string.style_selector_visible", stringDisplayStyleVisible(widget));
        appendBool("interaction.text.single_line", stringLimitToSingleLine(widget));
        appendBool("interaction.text.wrap_at_word_breaks", stringWrapAtWordBreaks(widget));
        appendBool("interaction.text.update_while_typing", stringUpdateWhileTyping(widget));
        appendBool("display.vertical_scrollbar_visible", stringVerticalScrollbarVisible(widget));
        appendBool("display.horizontal_scrollbar_visible", stringHorizontalScrollbarVisible(widget));
        appendBool("layout.size_to_text", stringSizeToText(widget));
    }
    if (isPathWidget(widget)) {
        if (widget.supportTextValue.has_value()) {
            appendString("text.value", widgetPathValue(widget));
        }
        appendBool("display.path.scrollbar_visible", pathScrollbarVisible(widget));
        appendString("display.path.overflow", pathOverflowMode(widget));
        appendBool("layout.size_to_text", pathSizeToText(widget));
        appendString("interaction.path.prompt", pathBrowsePrompt(widget));
        appendString("interaction.path.pattern_label", pathBrowsePatternLabel(widget));
        appendString("interaction.path.pattern", pathBrowsePattern(widget));
        appendString("interaction.path.selection_mode", pathBrowseSelectionMode(widget));
        appendString("interaction.path.existence_mode", pathBrowseExistenceMode(widget));
        appendString("interaction.path.start_path", pathBrowseStartPath(widget));
    }
    if (isRingWidget(widget) || isEnumWidget(widget)) {
        if (isEnumWidget(widget)) {
            appendString("data_type.representation", enumRepresentationValueType(widget));
            appendString("data_type.named_numeric_size", enumNamedSize(widget));
            appendString("representation.kind", enumRepresentationKind(widget));
            appendString("value", ringSelectedItemId(widget));
            if (widget.enumDomainId.has_value()) {
                appendString("enum.domain_id", widget.enumDomainId.value());
            }
        } else {
            appendString("data_type.representation", ringRepresentationValueType(widget));
            appendString("data_type.named_numeric_size", ringNamedSize(widget));
            appendString("representation.kind", ringRepresentationKind(widget));
            appendString("value.selected_id", ringSelectedItemId(widget));
            appendUnsigned(
                "value.selected_value",
                static_cast<std::uint32_t>((std::max)(0.0, widget.numericValue.value_or(0.0))));
            appendUnsigned("value.selected_index", static_cast<std::uint32_t>(ringSelectedItemIndex(widget)));
            appendString("value.selected_label", ringSelectedItemLabel(widget));
        }
        if (!widget.ringItems.empty()) {
            appendBool("items.sequential_values", ringItemsAreSequential());
            appendRingItems();
        }
        if (widget.ringAllowUndefinedValues.has_value()) {
            appendBool("data_type.allow_undefined_values", widget.ringAllowUndefinedValues.value());
        }
    }
    if (widget.buttonStateFalseText.has_value()) {
        appendString("state_text.false_text", widget.buttonStateFalseText.value());
    }
    if (widget.buttonStateTrueText.has_value()) {
        appendString("state_text.true_text", widget.buttonStateTrueText.value());
    }
    if (widget.buttonStateTextVisible.has_value()) {
        appendBool("state_text.visible", widget.buttonStateTextVisible.value());
    }
    if (widget.stateTextRgb.has_value()) {
        appendString("state_text.color", formatRgbHex(widget.stateTextRgb.value()));
    }
    if (widget.stateFalseFillRgb.has_value()) {
        appendString("state_colors.false_fill", formatRgbHex(widget.stateFalseFillRgb.value()));
    }
    if (widget.stateTrueFillRgb.has_value()) {
        appendString("state_colors.true_fill", formatRgbHex(widget.stateTrueFillRgb.value()));
    }
    if (widget.stateFalseStrokeRgb.has_value()) {
        appendString("state_colors.false_stroke", formatRgbHex(widget.stateFalseStrokeRgb.value()));
    }
    if (widget.stateTrueStrokeRgb.has_value()) {
        appendString("state_colors.true_stroke", formatRgbHex(widget.stateTrueStrokeRgb.value()));
    }
    if (widget.buttonStateTextLockInCenter.has_value()) {
        appendBool("state_text.lock_in_center", widget.buttonStateTextLockInCenter.value());
    }
    if (widget.buttonMechanicalAction.has_value()) {
        appendString("behavior.mechanical_action", widget.buttonMechanicalAction.value());
    }
    if (widget.numericIncrementDecrementVisible.has_value()) {
        appendBool(
            isNumericWidget(widget)
                ? "display.increment_buttons_visible"
                : "display.increment_decrement_visible",
            widget.numericIncrementDecrementVisible.value());
    }
    if (widget.numericIncrementDecrementSide.has_value()) {
        appendString(
            isNumericWidget(widget)
                ? "display.increment_buttons_side"
                : "increment_decrement.side",
            widget.numericIncrementDecrementSide.value() == NumericIncrementDecrementSide::Left ? "left" : "right");
    }
    if (widget.pathFolderVisible.has_value()) {
        appendBool("folder.visible", widget.pathFolderVisible.value());
    }
    if (widget.pathBrowseButtonVisible.has_value()) {
        appendBool("button.visible", widget.pathBrowseButtonVisible.value());
    }
    if (widget.numericBodyFillRgb.has_value()) {
        appendString("style.body.fill_color", formatRgbHex(widget.numericBodyFillRgb.value()));
    }
    if (!isLabelWidget(widget)) {
        if (widget.labelSurfaceFillRgb)
            appendString("style.label.fill_color", formatRgbHex(*widget.labelSurfaceFillRgb));
        if (widget.labelSurfaceStrokeRgb)
            appendString("style.label.stroke_color", formatRgbHex(*widget.labelSurfaceStrokeRgb));
    }
    if (widget.surfaceFillRgb.has_value()) {
        appendString("style.surface.fill_color", formatRgbHex(widget.surfaceFillRgb.value()));
    }
    if (widget.surfaceStrokeRgb.has_value()) {
        appendString("style.surface.stroke_color", formatRgbHex(widget.surfaceStrokeRgb.value()));
    }
    if (widget.numericBodyStrokeRgb.has_value()) {
        appendString("style.body.stroke_color", formatRgbHex(widget.numericBodyStrokeRgb.value()));
    }
    if (widget.stringBodyFillRgb.has_value()) {
        appendString("style.body.fill_color", formatRgbHex(widget.stringBodyFillRgb.value()));
    }
    if (widget.stringBodyStrokeRgb.has_value()) {
        appendString("style.body.stroke_color", formatRgbHex(widget.stringBodyStrokeRgb.value()));
    }
    if (widget.numericStepButtonFillRgb.has_value()) {
        appendString("style.increment_decrement.button_fill_color", formatRgbHex(widget.numericStepButtonFillRgb.value()));
    }
    if (widget.numericStepButtonStrokeRgb.has_value()) {
        appendString("style.increment_decrement.button_stroke_color", formatRgbHex(widget.numericStepButtonStrokeRgb.value()));
    }
    if (widget.numericStepArrowFillRgb.has_value()) {
        appendString("style.increment_decrement.arrow_fill_color", formatRgbHex(widget.numericStepArrowFillRgb.value()));
    }
    if (widget.numericStepArrowStrokeRgb.has_value()) {
        appendString("style.increment_decrement.arrow_stroke_color", formatRgbHex(widget.numericStepArrowStrokeRgb.value()));
    }
    if (wrote) {
        output << '\n';
    }

    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendWidgetState(std::ostringstream &output, const WidgetInstance &widget, int indent, bool comma = true) {
    appendIndent(output, indent);
    output << "\"state\": {\n";

    bool wroteField = false;
    const auto appendCommaIfNeeded = [&]() {
        if (wroteField) {
            output << ",\n";
        }
        wroteField = true;
    };

    if (widget.booleanValue.has_value()) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        output << "\"value\": " << (widget.booleanValue.value() ? "true" : "false");
    }
    if (isNumericWidget(widget) && numericCanonicalValueTypeIsComplex(widget.valueType) && widget.initialValue) {
        const auto lexical = widgetNumericComplexValueLexeme(widget);
        if (!lexical) throw std::runtime_error("Cannot serialize invalid private complex Numeric state.");
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        output << "\"initial_value\": "; appendJsonString(output, *lexical);
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        output << "\"numeric_value_present\": " << (widget.numericValue ? "true" : "false");
    } else if (isNumericWidget(widget) && widget.valueType == "f64" &&
        widget.initialValue == "+Infinity" && widget.numericValue &&
        std::isinf(*widget.numericValue) && *widget.numericValue > 0.0) {
        // Gamma P's authored optional-limit Control has a semantic +Infinity
        // default. JSON numbers remain finite; use the existing lexical state
        // channel without widening the general Numeric literal grammar.
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        output << "\"initial_value\": "; appendJsonString(output, "+Infinity");
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        output << "\"numeric_value_present\": true";
    } else if (hasExactWidgetIntegerState(widget)) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        output << "\"initial_value\": ";appendJsonString(output,*widget.initialValue);
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        output << "\"numeric_value_present\": " << (widget.numericValue?"true":"false");
    } else if (widget.numericValue.has_value()) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        if (isRingWidget(widget) || isEnumWidget(widget) || isStringDropdownWidget(widget)) {
            output << "\"value.selected_value\": " <<
                static_cast<std::uint32_t>((std::max)(0.0, widget.numericValue.value()));
        } else if (isNumericWidget(widget) && std::isfinite(widget.numericValue.value())) {
            // Persist the finite model value independently from its displayed
            // precision. The shortest general spelling round-trips binary64,
            // including signed zero, without consulting layout or typography.
            std::array<char, 64> number{};
            const auto encoded = std::to_chars(number.data(), number.data() + number.size(),
                widget.numericValue.value(), std::chars_format::general);
            if (encoded.ec != std::errc{})
                throw std::runtime_error("Cannot serialize finite widget numeric state.");
            output << "\"value\": " << std::string_view(number.data(),
                static_cast<std::size_t>(encoded.ptr - number.data()));
        } else if (widget.valueType == "timestamp") {
            // Displayed milliseconds must not truncate the stored timestamp.
            std::ostringstream exactValue;
            exactValue << std::setprecision(std::numeric_limits<double>::max_digits10)
                       << widget.numericValue.value();
            output << "\"value\": " << exactValue.str();
        } else {
            output << "\"value\": " << formatNumber(widget.numericValue.value());
        }
    }
    if (wroteField) {
        output << '\n';
    }

    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendGeometryOverrides(std::ostringstream &output, const WidgetInstance &widget, int indent, bool comma = true) {
    appendIndent(output, indent);
    output << "\"geometry\": {\n";
    bool wrote = false;

    if (widget.numericValueOffsetX.has_value() || widget.numericValueWidth.has_value()) {
        appendIndent(output, indent + 1);
        output << "\"numericValue\": {\n";
        if (widget.numericValueOffsetX.has_value()) {
            appendJsonNumberField(output, indent + 2, "offsetX", widget.numericValueOffsetX.value(), widget.numericValueWidth.has_value());
        }
        if (widget.numericValueWidth.has_value()) {
            appendJsonNumberField(output, indent + 2, "width", widget.numericValueWidth.value(), false);
        }
        appendIndent(output, indent + 1);
        output << '}';
        wrote = true;
    }

    if (widget.labelOffsetX.has_value() || widget.labelOffsetY.has_value()) {
        if (wrote) {
            output << ",\n";
        }
        appendIndent(output, indent + 1);
        output << "\"labelOffset\": {\n";
        appendJsonNumberField(output, indent + 2, "x", widget.labelOffsetX.value_or(0.0));
        appendJsonNumberField(output, indent + 2, "y", widget.labelOffsetY.value_or(0.0), false);
        appendIndent(output, indent + 1);
        output << '}';
        wrote = true;
    }

    if (wrote) {
        output << '\n';
    }
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendTypographyOverrides(std::ostringstream &output, const WidgetInstance &widget, int indent, bool comma = true) {
    appendIndent(output, indent);
    output << "\"typography\": {\n";

    bool wrote = false;
    const auto appendCommaIfNeeded = [&]() {
        if (wrote) {
            output << ",\n";
        }
        wrote = true;
    };
    const auto appendNumber = [&](std::string_view name, double value) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        // Font scales must round-trip exactly; the three-decimal coordinate
        // formatter changes an explicit 20/15 scale when reopening a file.
        std::ostringstream number;
        number.imbue(std::locale::classic());
        number << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
        output << ": " << number.str();
    };
    const auto appendString = [&](std::string_view name, std::string_view value) {
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        output << ": ";
        appendJsonString(output, value);
    };

    appendNumber("inheritanceVersion", 1);
    const auto appendLiteralProvenance = [&](std::string_view name, std::optional<bool> value) {
        if (!value.has_value()) return;
        appendCommaIfNeeded();
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        output << ": " << (*value ? "true" : "false");
    };
    appendLiteralProvenance("constantTextBoundsAutomatic", widget.constantTextBoundsAutomatic);
    appendLiteralProvenance("constantArrayCellBoundsAutomatic", widget.constantArrayCellBoundsAutomatic);
    if (widget.defaultTextGeometryInitialized) {
        // Retain the font context in which authored presentation was measured.
        // Default inheritance still follows the application preference; this
        // receipt is neither a font override nor executable source semantics.
        appendNumber("resolvedPointSize", widget.inheritedTextPointSize);
    }
    if (widget.captionFontScale.has_value()) {
        appendNumber("captionScale", widget.captionFontScale.value());
    }
    if (widget.captionFontFamily.has_value()) {
        appendString("captionFontFamily", widget.captionFontFamily.value());
    }
    if (widget.labelFontScale.has_value()) {
        appendNumber("labelScale", widget.labelFontScale.value());
    }
    if (widget.numericValueFontScale.has_value()) {
        appendNumber("numericValueScale", widget.numericValueFontScale.value());
    }
    if (widget.stringValueFontScale.has_value()) {
        appendNumber("stringValueScale", widget.stringValueFontScale.value());
    }
    if (widget.stateTextFontScale.has_value()) {
        appendNumber("stateTextScale", widget.stateTextFontScale.value());
    }
    if (isBooleanWidget(widget) || isButtonWidget(widget)) {
        // Explicit defaults distinguish new independent state typography from
        // legacy documents where state text inherited the caption's font.
        appendString("stateTextFontFamily", widget.stateTextFontFamily.value_or(""));
        appendString("stateTextStyle", widgetStateTextStyle(widget));
    }
    if (widget.labelFontFamily.has_value()) {
        appendString("labelFontFamily", widget.labelFontFamily.value());
    }
    if (widget.numericValueFontFamily.has_value()) {
        appendString("numericValueFontFamily", widget.numericValueFontFamily.value());
    }
    if (widget.stringValueFontFamily.has_value()) {
        appendString("stringValueFontFamily", widget.stringValueFontFamily.value());
    }
    if (widget.labelTextStyle.has_value()) {
        appendString("labelStyle", widget.labelTextStyle.value());
    }
    if (widget.numericValueTextStyle.has_value()) {
        appendString("numericValueStyle", widget.numericValueTextStyle.value());
    }
    if (widget.stringValueTextStyle.has_value()) {
        appendString("stringValueStyle", widget.stringValueTextStyle.value());
    }
    if (widget.choiceTextHorizontalAlignment.has_value()) {
        appendString(
            "choiceTextHorizontalAlignment",
            widget.choiceTextHorizontalAlignment.value());
    }
    if (widget.stringTextHorizontalAlignment.has_value()) {
        appendString("stringTextHorizontalAlignment",widget.stringTextHorizontalAlignment.value());
    }
    if (widget.labelTextRgb.has_value()) {
        appendString("labelColor", formatRgbHex(widget.labelTextRgb.value()));
    }
    if (widget.numericValueTextRgb.has_value()) {
        appendString("numericValueColor", formatRgbHex(widget.numericValueTextRgb.value()));
    }
    if (widget.stringValueTextRgb.has_value()) {
        appendString("stringValueColor", formatRgbHex(widget.stringValueTextRgb.value()));
    }

    if (!wrote) {
        appendIndent(output, indent + 1);
        output << '\n';
    } else {
        output << '\n';
    }
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendWidgetShallow(std::ostringstream &output, const SceneObject *object,
    const WidgetInstance *instance, int indent, bool comma, WidgetWriteContext &storageMode,
    std::vector<DeferredWidgetWrite> &deferred) {
    if (object && object->staticVisual.has_value()) {
        appendStaticVisual(output, *object, *object->staticVisual, indent, comma);
        return;
    }

    appendIndent(output, indent);
    output << "{\n";
    appendJsonField(output, indent + 1, "id", object ? object->id : instance->instanceId);
    if (object && object->locked) {
        appendJsonBooleanField(output, indent + 1, "locked", object->locked);
    }
    if (object && object->hidden) {
        appendJsonBooleanField(output, indent + 1, "hidden", object->hidden);
    }
    if (object && (!object->parentTabId.empty() || !object->parentTabPageId.empty())) {
        appendJsonField(output, indent + 1, "studio_tab_parent_id", object->parentTabId);
        appendJsonField(output, indent + 1, "studio_tab_page_id", object->parentTabPageId);
    }

    if (instance) {
        const auto &widget = *instance;
        if (isTabWidget(widget)) {
            if (!tabPagesValid(widget)) throw std::invalid_argument("Cannot serialize an invalid Tab page model.");
            appendIndent(output, indent + 1);output << "\"studio_tab\": {\n";
            appendJsonField(output, indent + 2, "editor_page_id", widget.tabContainer->editorPageId);
            appendJsonField(output, indent + 2, "next_page_serial", std::to_string(widget.tabContainer->nextPageSerial));
            appendJsonIntegerField(output, indent + 2, "header_first_visible", widget.tabContainer->headerFirstVisible);
            appendJsonBooleanField(output, indent + 2, "tabs_visible", widget.tabContainer->tabsVisible);
            appendJsonBooleanField(output, indent + 2, "page_labels_display_visible", widget.tabContainer->pageLabelsDisplayVisible, false);
            appendIndent(output, indent + 1);output << "},\n";
        }
        if(widget.classId==kCollectionWidgetClass) {
            if(!widget.collection||collectionType(*widget.collection).canonical()!=widget.valueType)throw std::invalid_argument("Collection widget has mismatched declared state/type.");
        }else if(widget.collection)throw std::invalid_argument("Collection state belongs to the real Collection widget class.");
        const bool hasBinding = hasWidgetBinding(widget);
        appendJsonField(output, indent + 1, "classId", widget.classId);
        appendJsonField(output, indent + 1, "role", widget.role);
        appendJsonField(output, indent + 1, "valueType", widget.valueType);
        if(widget.referenceValue) {
            document_internal::appendApplicationControl(output,*widget.referenceValue,indent+1);
            storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,kFrogApplicationControlDraftRevision);
        }
        appendJsonField(output, indent + 1, "label", widget.label);
        appendBounds(output, widget.bounds, indent + 1);
        if (hasWidgetPropertyOverrides(widget)) {
            appendWidgetPropertyOverrides(output, widget, indent + 1, storageMode, deferred);
        }
        appendRuntime(
            output,
            widget,
            indent + 1,
            hasWidgetState(widget) || hasGeometryOverrides(widget) || hasTypographyOverrides(widget) || hasBinding);
        if (hasWidgetState(widget)) {
            const auto stateRevision = numericCanonicalValueTypeIsComplex(widget.valueType) && widget.initialValue
                ? kFrogComplexStateDraftRevision : hasExactWidgetIntegerState(widget) ? kFrogExactIntegerStateDraftRevision : 0U;
            storageMode.requiredStateRevision=(std::max)(storageMode.requiredStateRevision,stateRevision);
            appendWidgetState(output, widget, indent + 1, hasGeometryOverrides(widget) || hasTypographyOverrides(widget) || hasBinding);
        }
        if (hasGeometryOverrides(widget)) {
            appendGeometryOverrides(output, widget, indent + 1, hasTypographyOverrides(widget) || hasBinding);
        }
        if (hasTypographyOverrides(widget)) {
            appendTypographyOverrides(output, widget, indent + 1, hasBinding);
        }
        if (hasBinding) {
            appendWidgetBinding(output, widget, indent + 1, false);
        }
    } else {
        appendBounds(output, object->bounds, indent + 1, false);
    }

    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendWidgetTree(std::ostringstream &output, const SceneObject *object,
    const WidgetInstance *widget, int indent, bool comma, WidgetWriteContext &storageMode) {
    struct Frame {
        const SceneObject *object;
        const WidgetInstance *widget;
        int indent;
        bool comma;
        bool prepared=false;
        std::string text;
        std::vector<DeferredWidgetWrite> deferred;
        std::size_t next=0,offset=0;
    };
    std::vector<Frame> pending{{object,widget,indent,comma,false,{},{},0,0}};
    std::unordered_set<const WidgetInstance *> active;
    while(!pending.empty()) {
        auto &frame=pending.back();
        if(!frame.prepared) {
            if(frame.widget && !active.insert(frame.widget).second)
                throw std::runtime_error("Cannot serialize a cyclic widget container.");
            std::ostringstream shallow;
            appendWidgetShallow(shallow,frame.object,frame.widget,frame.indent,frame.comma,
                storageMode,frame.deferred);
            frame.text=shallow.str();
            frame.prepared=true;
        }
        if(frame.next==frame.deferred.size()) {
            output.write(frame.text.data()+frame.offset,
                static_cast<std::streamsize>(frame.text.size()-frame.offset));
            if(frame.widget)active.erase(frame.widget);
            pending.pop_back();
            continue;
        }
        const auto &part=frame.deferred[frame.next++];
        output.write(frame.text.data()+frame.offset,
            static_cast<std::streamsize>(part.offset-frame.offset));
        frame.offset=part.offset;
        if(part.widget) {
            // Read all parent references before push_back can relocate frames.
            const auto *child=part.widget;
            const auto childIndent=part.indent;
            const auto childComma=part.comma;
            pending.push_back({nullptr,child,childIndent,childComma,false,{},{},0,0});
        } else {
            const auto compact=document_internal::encodeArrayStorage(part.records,storageMode,
                "numeric-elements-json-le32-v1",part.readable.size());
            output << (compact ? *compact : part.readable);
        }
    }
}

void appendWidget(std::ostringstream &output, const SceneObject &object, int indent, bool comma,
    WidgetWriteContext &storageMode) {
    appendWidgetTree(output,&object,object.widget ? &*object.widget : nullptr,indent,comma,storageMode);
}

void appendObjectGroups(std::ostringstream &output, std::span<const SceneObjectGroup> groups, int indent) {
    appendIndent(output, indent);
    output << "\"groups\": [\n";
    for (std::size_t groupIndex = 0; groupIndex < groups.size(); ++groupIndex) {
        const auto &group = groups[groupIndex];
        appendIndent(output, indent + 1);
        output << "{\n";
        appendJsonField(output, indent + 2, "id", group.id);
        appendIndent(output, indent + 2);
        output << "\"object_ids\": [";
        for (std::size_t index = 0; index < group.objectIds.size(); ++index) {
            if (index > 0U) {
                output << ", ";
            }
            appendJsonString(output, group.objectIds[index]);
        }
        output << "]\n";
        appendIndent(output, indent + 1);
        output << '}';
        if (groupIndex + 1U < groups.size()) {
            output << ',';
        }
        output << '\n';
    }
    appendIndent(output, indent);
    output << "]\n";
}

void appendProgramMetadata(
    std::ostringstream &output,
    std::string_view documentTitle,
    const FrogMetadataProperties &metadata,
    const FrogFileVersionMetadata &versions) {
    output << "  \"metadata\": {\n";
    appendJsonField(output, 2, "name", resolvedFrogProgramName(documentTitle, metadata));
    appendJsonField(output, 2, "name_source", metadata.useDocumentName ? "document" : "custom");
    appendJsonField(output, 2, "description", metadata.description);
    appendJsonField(output, 2, "author", metadata.author);
    appendJsonBooleanField(output, 2, "automatic_versioning", metadata.automaticVersioningEnabled);
    appendJsonField(output, 2, "program_version", normalizeFrogProgramVersion(metadata.programVersion));
    if(versions.createdBy)appendJsonField(output,2,"created_by",*versions.createdBy);
    if(versions.savedBy)appendJsonField(output,2,"saved_by",*versions.savedBy);
    output << "    \"tags\": [";
    for (std::size_t index = 0; index < metadata.tags.size(); ++index) {
        if (index > 0U) {
            output << ", ";
        }
        appendJsonString(output, metadata.tags[index]);
    }
    output << "],\n";
    appendJsonField(output, 2, "documentation_uri", metadata.documentationUri, false);
    output << "  },\n";
}

void appendHostProperties(std::ostringstream &output, const FrogHostProperties &host) {
    output << "  \"host\": {\n";
    appendJsonIntegerField(output, 2, "version", (std::max)(1, host.version));
    output << "    \"launch\": {\n";
    appendJsonField(output, 3, "trigger", host.launchTrigger, false);
    output << "    },\n";
    output << "    \"front_panel_window\": {\n";
    appendJsonField(output, 3, "style", host.windowStyle);
    appendJsonField(output, 3, "initial_position", host.initialPosition);
    appendJsonIntegerField(output, 3, "minimum_width", (std::max)(0, host.minimumWidth));
    appendJsonIntegerField(output, 3, "minimum_height", (std::max)(0, host.minimumHeight));
    appendJsonBooleanField(output, 3, "allow_close", host.allowClose);
    appendJsonBooleanField(output, 3, "allow_resize", host.allowResize);
    appendJsonBooleanField(output, 3, "allow_minimize", host.allowMinimize);
    appendJsonBooleanField(output, 3, "menu_bar_visible", host.menuBarVisible);
    appendJsonBooleanField(output, 3, "toolbar_visible", host.toolbarVisible);
    appendJsonBooleanField(output, 3, "vertical_scrollbar_visible", host.verticalScrollbarVisible);
    appendJsonBooleanField(output, 3, "horizontal_scrollbar_visible", host.horizontalScrollbarVisible);
    appendJsonBooleanField(output, 3, "scroll_to_origin", host.scrollToOrigin, false);
    output << "    }\n";
    output << "  },\n";
}

void appendExecutionPolicy(
    std::ostringstream &output,
    const FrogExecutionPolicy &policy) {
    output << "  \"execution_policy\": {\n";
    appendJsonIntegerField(output, 2, "version", (std::max)(1, policy.version));
    appendJsonBooleanField(output, 2, "debugging_allowed", policy.debuggingAllowed);
    appendJsonField(output, 2, "priority", policy.priority);
    appendJsonField(output, 2, "reentrancy", policy.reentrancy);
    appendJsonField(output, 2, "execution_system", policy.executionSystem);
    appendJsonBooleanField(output, 2, "automatic_error_handling", policy.automaticErrorHandling);
    appendJsonBooleanField(output, 2, "suspend_when_called", policy.suspendWhenCalled);
    appendJsonBooleanField(output, 2, "clear_indicators_when_called", policy.clearIndicatorsWhenCalled, false);
    output << "  },\n";
}

std::string serializeStoredElementWidget(const WidgetInstance &widget, WidgetWriteContext &storageMode) {
    std::ostringstream output;
    appendWidgetTree(output,nullptr,&widget,0,false,storageMode);
    return output.str();
}
} // namespace

std::string serializeArrayElementWidget(const WidgetInstance &widget) {
    WidgetWriteContext storageMode{FrogArrayStorageMode::Readable};
    std::ostringstream output;
    appendWidgetTree(output,nullptr,&widget,0,false,storageMode);
    if(!storageMode.requiredStateRevision)return output.str();
    std::ostringstream envelope;
    envelope << "{\"format\": ";appendJsonString(envelope,kFrogWidgetValueDraftFormat);
    envelope << ", \"draft_revision\": " << storageMode.requiredStateRevision << ", \"widget\": " << output.str() << "}\n";
    return envelope.str();
}

std::string serializeArrayElementValueWidget(const WidgetInstance &value,const WidgetInstance &prototype) {
    if(binding::widgetType(value).type!=binding::widgetType(prototype).type)return {};
    // A compact value is a supported, interpreted payload. Keep unsupported
    // historical Graphs in their original full-widget cell instead of emitting
    // an empty dataset or an unchanged marker that conceals the opaque source.
    std::vector<const WidgetInstance *> owned{&value};
    while(!owned.empty()) {
        const auto *widget=owned.back();owned.pop_back();
        if(widget->collection)return serializeArrayElementWidget(value);
        if(widget->waveformGraph && widget->waveformGraph->preservedSource)return {};
        if(const auto *cluster=clusterContainerState(*widget))
            for(const auto &field:cluster->elements)if(field.widget)owned.push_back(field.widget.get());
        if(const auto *array=arrayContainerState(*widget);array && array->elementTemplate)
            owned.push_back(array->elementTemplate.get());
    }
    const auto equalValues=[](const WidgetInstance &a,const WidgetInstance &b) {
        std::vector<std::pair<const WidgetInstance *,const WidgetInstance *>> pending{{&a,&b}};
        while(!pending.empty()) {
            const auto [left,right]=pending.back();pending.pop_back();
            if(left->initialValue!=right->initialValue || left->numericValue!=right->numericValue ||
                left->booleanValue!=right->booleanValue || left->supportTextValue!=right->supportTextValue ||
                left->referenceValue!=right->referenceValue || bool(left->collection)!=bool(right->collection) ||
                (left->collection&&*left->collection!=*right->collection))return false;
            // Display text normalizes both zero signs to "0"; a compact
            // inherited marker must still distinguish their model values.
            if(left->numericValue && right->numericValue &&
                *left->numericValue==0.0 && *right->numericValue==0.0 &&
                std::signbit(*left->numericValue)!=std::signbit(*right->numericValue))return false;
            if(const auto *cluster=clusterContainerState(*left)) {
                const auto *other=clusterContainerState(*right);
                if(!other || cluster->elements.size()!=other->elements.size())return false;
                for(const auto &field:cluster->elements) {
                    const auto match=std::find_if(other->elements.begin(),other->elements.end(),
                        [&](const auto &candidate){return candidate.fieldId==field.fieldId;});
                    if(match==other->elements.end() || bool(field.widget)!=bool(match->widget))return false;
                    if(field.widget)pending.push_back({field.widget.get(),match->widget.get()});
                }
            } else if(const auto *array=arrayContainerState(*left)) {
                const auto *other=arrayContainerState(*right);
                if(!other || array->rank!=other->rank || array->shape!=other->shape ||
                    array->elements.size()!=other->elements.size() || bool(array->elementTemplate)!=bool(other->elementTemplate))return false;
                for(const auto &cell:array->elements) {
                    const auto match=std::find_if(other->elements.begin(),other->elements.end(),
                        [&](const auto &candidate){return candidate.indices==cell.indices;});
                    if(match==other->elements.end() || match->displayValue!=cell.displayValue ||
                        match->kind!=cell.kind || match->defined!=cell.defined || match->enabled!=cell.enabled)return false;
                }
                if(array->elementTemplate)pending.push_back({array->elementTemplate.get(),other->elementTemplate.get()});
            } else if(isWaveformGraphWidget(*left)) {
                // Graph payloads use their ordinary immutable authoring codec.
                if(serializeArrayElementWidget(*left)!=serializeArrayElementWidget(*right))return false;
            }
        }
        return true;
    };
    struct Part {const WidgetInstance *value,*prototype;std::string text;};
    std::vector<Part> pending{{&value,&prototype,{}}};std::ostringstream output;
    while(!pending.empty()) {
        auto part=std::move(pending.back());pending.pop_back();
        if(!part.value){output<<part.text;continue;}
        const auto &widget=*part.value,&baseline=*part.prototype;
        output<<"{\"array_cell_value\":1,\"valueType\":";
        appendJsonString(output,binding::widgetType(widget).type.canonical());
        if(equalValues(widget,baseline)){output<<'}';continue;}
        output<<",\"value\":";
        if(const auto *cluster=clusterContainerState(widget)) {
            const auto *other=clusterContainerState(baseline);
            output<<"{\"fields\":[";pending.push_back({nullptr,nullptr,"]}}"});
            for(auto it=cluster->elements.rbegin();it!=cluster->elements.rend();++it) {
                const auto match=std::find_if(other->elements.begin(),other->elements.end(),
                    [&](const auto &candidate){return candidate.fieldId==it->fieldId;});
                if(!it->widget || match==other->elements.end() || !match->widget)return {};
                if(it!=cluster->elements.rbegin())pending.push_back({nullptr,nullptr,","});
                pending.push_back({nullptr,nullptr,"}"});
                pending.push_back({it->widget.get(),match->widget.get(),{}});
                std::ostringstream field;field<<"{\"field_id\":";appendJsonString(field,it->fieldId);field<<",\"value\":";
                pending.push_back({nullptr,nullptr,field.str()});
            }
        } else if(const auto *array=arrayContainerState(widget)) {
            const auto *other=arrayContainerState(baseline);
            output<<"{\"shape\":[";
            for(std::size_t i=0;i<array->shape.size();++i){if(i)output<<',';output<<array->shape[i];}
            output<<"],\"elements\":[";
            for(std::size_t i=0;i<array->elements.size();++i) {
                const auto &cell=array->elements[i];if(i)output<<',';
                output<<"{\"indices\":[";
                for(std::size_t axis=0;axis<cell.indices.size();++axis){if(axis)output<<',';output<<cell.indices[axis];}
                output<<"],\"kind\":";appendJsonString(output,cell.kind==ArrayElementValueKind::Widget?"widget":
                    cell.kind==ArrayElementValueKind::Number?"number":cell.kind==ArrayElementValueKind::Boolean?"boolean":"string");
                output<<",\"defined\":"<<(cell.defined?"true":"false")<<",\"enabled\":"<<(cell.enabled?"true":"false")<<",\"value\":";
                // Scalar text remains authoritative (including integer/decimal
                // lexemes). Aggregate text already is a value object.
                if(cell.kind==ArrayElementValueKind::Widget && !storedWidgetNeedsExactString(cell))output<<cell.displayValue;
                else appendJsonString(output,cell.displayValue);
                output<<'}';
            }
            output<<']';
            if(array->elementTemplate && other->elementTemplate &&
                !equalValues(*array->elementTemplate,*other->elementTemplate)) {
                output<<",\"element_default\":";pending.push_back({nullptr,nullptr,"}}"});
                pending.push_back({array->elementTemplate.get(),other->elementTemplate.get(),{}});
            } else output<<"}}";
        } else {
            output<<serializeArrayElementWidget(widget);
            if(isNumericWidget(widget)) {
                output<<",\"initial_value\":";
                if(widget.initialValue)appendJsonString(output,*widget.initialValue);else output<<"null";
                output<<",\"numeric_value_present\":"<<(widget.numericValue?"true":"false");
            }
            output<<'}';
        }
    }
    return output.str();
}

std::string serializeFrogDraft(
    FrogWidgetDataView canvas,
    std::string_view documentTitle,
    std::string_view interfaceMapLayoutPatternId,
    const std::vector<std::string> &interfaceMapVisualTransforms,
    std::string_view fileIconSvgText,
    const std::vector<DiagramNode> &diagramNodes,
    int diagramZoomPercent,
    Point diagramViewportOrigin,
    const std::vector<DiagramEdge> &diagramEdges,
    const std::vector<DiagramWireFragment> &diagramWireFragments,
    std::string_view interfaceBindingMapProfileId,
    const FrogSourceProperties &properties,
    const FrogEditorWindows &editorWindows,
    bool preserveDiagramTerminalTypes,
    const FrogDraftInterface *declaredInterface,FrogDocumentCompressFn compress) {
    if (const auto error = frogIconPresentationError(fileIconSvgText); !error.empty()) {
        throw std::runtime_error("Cannot serialize /icon/svg: " + std::string(error));
    }
    if ((properties.defaultTextPointSize &&
            !validDocumentTextPointSize(*properties.defaultTextPointSize)) ||
        (properties.defaultTextFontFamily &&
            (!properties.defaultTextPointSize ||
             !validDocumentTextFontFamily(*properties.defaultTextFontFamily)))) {
        throw std::runtime_error("Cannot serialize invalid private FROG text defaults.");
    }
    if((properties.fileVersions.createdBy&&!validFrogStudioFileVersion(*properties.fileVersions.createdBy)) ||
        (properties.fileVersions.savedBy&&!validFrogStudioFileVersion(*properties.fileVersions.savedBy)))
        throw std::runtime_error("Cannot serialize invalid GS file provenance versions.");
    WidgetWriteContext storageMode{properties.arrayStorageMode};
    storageMode.compress=compress;
    std::ostringstream output;
    output << "{\n";
    appendJsonField(output, 1, "format", kFrogDraftDocumentFormat);
    const auto revisionFieldBegin=static_cast<std::size_t>(output.tellp());
    appendJsonIntegerField(output, 1, "draft_revision", storageMode.mode == FrogArrayStorageMode::Readable
        ? kCurrentFrogDraftRevision : kFrogArrayStorageDraftRevision);
    const auto revisionFieldEnd=static_cast<std::size_t>(output.tellp());
    if (storageMode.mode != FrogArrayStorageMode::Readable) {
        output << "  \"storage\": {\"version\": 1, \"array_mode\": ";
        appendJsonString(output, document_internal::arrayStorageModeToken(storageMode.mode));
        output << "},\n";
    }
    appendJsonField(output, 1, "language", "FROG");
    appendJsonField(output, 1, "title", documentTitle);
    if (properties.defaultTextPointSize) {
        output << "  \"text_defaults\": {\n";
        appendJsonNumberField(output, 2, "point_size", *properties.defaultTextPointSize,
            properties.defaultTextFontFamily.has_value());
        if (properties.defaultTextFontFamily)
            appendJsonField(output, 2, "font_family", *properties.defaultTextFontFamily, false);
        output << "  },\n";
    }
    appendProgramMetadata(output, documentTitle, properties.metadata, properties.fileVersions);
    appendHostProperties(output, properties.host);
    appendExecutionPolicy(output, properties.executionPolicy);
    if(properties.nativeOptions) {
        const auto &native=*properties.nativeOptions;
        output << "  \"native_options\": {\n";
        // Native intent keeps unknown signed versions for explicit admission refusal.
        output << "    \"version\": " << native.version << ",\n";
        appendJsonField(output,2,"target_id",native.targetId);
        appendJsonField(output,2,"backend_id",native.backendId);
        appendJsonField(output,2,"optimization_profile",native.optimizationProfile,false);
        output << "  },\n";
    }
    output << document_internal::serializeCompilationSymbols(properties);
    if (declaredInterface) {
        // Defaults projected by an editor must not author or normalize a map.
        output << "  \"interface\": " << declaredInterface->declarationUtf8() << ",\n";
        if (const auto retainedMap = declaredInterface->retainedLegacyRootInterfaceMapUtf8()) {
            // A retained historical source member is not rebuilt from caches
            // or grafted into the independently authored declaration.
            output << "  \"interface_map\": " << *retainedMap << ",\n";
        }
    } else if (!interfaceBindingMapProfileId.empty() || !interfaceMapLayoutPatternId.empty() ||
               !interfaceMapVisualTransforms.empty()) {
        // Historical drafts have no independently authored declaration. Keep
        // their existing private presentation map without inferring API ports
        // from widget bindings. The reader already accepts this root member.
        appendPrivateInterfaceMap(output, 1, interfaceBindingMapProfileId, interfaceMapLayoutPatternId,
                                  interfaceMapVisualTransforms);
    }
    if (!fileIconSvgText.empty()) {
        output << "  \"icon\": {\n";
        appendJsonField(output, 2, "svg", fileIconSvgText, false);
        output << "  },\n";
    }
    output << "  \"frontPanel\": {\n";
    const auto canvasSize = canvas.frontPanelCanvasSize();
    if (canvasSize.width > 0.0 && canvasSize.height > 0.0) {
        output << "    \"canvas\": {\n";
        appendJsonNumberField(output, 3, "width", canvasSize.width);
        appendJsonNumberField(output, 3, "height", canvasSize.height);
        appendJsonField(output, 3, "coordinate_space", "panel_pixels", false);
        output << "    },\n";
    }
    output << "    \"grid\": {\n";
    appendJsonNumberField(output, 3, "pitch", canvas.gridPitch(), false);
    output << "    },\n";
    output << "    \"widgets\": [\n";

    const auto &objects = canvas.objects();
    for (std::size_t index = 0; index < objects.size(); ++index) {
        appendWidget(output, objects[index], 3, index + 1 < objects.size(), storageMode);
    }

    output << "    ]";
    if (!canvas.objectGroups().empty()) {
        output << ",\n";
        appendObjectGroups(output, canvas.objectGroups(), 2);
    } else {
        output << '\n';
    }
    output << "  },\n";
    if (hasIdePreferences(canvas) || editorWindows.frontPanel ||
        editorWindows.diagram || editorWindows.frogSource) {
        appendIdePreferences(output, canvas, editorWindows, 1);
    }
    auto resolvedNodes = diagramNodes;
    if (!preserveDiagramTerminalTypes) {
        binding::TypeResolver resolver(diagramNodes, diagramEdges, canvas);
        for (auto &node : resolvedNodes) {
            if (!binding::isCase(node) && !isDiagramStructureNodeType(node.type)) continue;
            if (binding::isCase(node)) {
                const auto selector = resolver.resolve({node.id, "selector"});
                // An unresolved/incompatible connection cannot specialize the
                // editable Case. Retain its committed matchers, String policy and
                // Enum domain together, just as normal Case authoring does. The
                // graph resolver still rejects this connection; persisted hints
                // are not evidence of semantic validity. Tunnel caches below are
                // deliberately recomputed, including their unresolved state.
                if (selector.resolution.accepted() && selector.resolution.type.known())
                    node.caseStructureSelectorValueType = selector.resolution.type.canonical();
            }
            for (auto &boundary : node.caseStructureTunnels) {
                // The public boundary type describes the outside face. Indexing
                // changes the type on the opposite face, not the source document.
                const auto result = resolver.resolve({node.id, boundary.outputFromStructure
                    ? binding::tunnelOutputId(boundary.id) : binding::tunnelInputId(boundary.id)});
                boundary.valueType = result.resolution.accepted() ? result.resolution.type.canonical() : "unknown";
            }
        }
    }
    // Boundary arrays load inputs before outputs. Match that order in the
    // private serialization copy so Save/Read/Save also preserves the exact
    // authoring metadata bytes, without changing the live tunnel vector.
    for (auto &node : resolvedNodes) {
        std::stable_partition(node.caseStructureTunnels.begin(), node.caseStructureTunnels.end(),
            [](const auto &tunnel) { return !tunnel.outputFromStructure; });
    }
    appendDiagram(
        output,
        resolvedNodes,
        diagramEdges,
        diagramWireFragments,
        diagramZoomPercent,
        diagramViewportOrigin,
        1,
        {},
        {}, storageMode);
    output << "}\n";
    auto document=output.str();
    if(storageMode.requiredStateRevision) {
        std::ostringstream revisionField;
        appendJsonIntegerField(revisionField,1,"draft_revision",storageMode.requiredStateRevision);
        document.replace(revisionFieldBegin,revisionFieldEnd-revisionFieldBegin,revisionField.str());
    }
    // The writer returns the final escaped payload, so enforce the existing
    // document reader's bound after escaping and revision replacement.
    if (document.size() > kMaximumFrogDocumentBytes)
        throw std::runtime_error("Cannot serialize a private FROG document larger than 64 MiB.");
    return document;
}

std::string serializeFrogDraftData(const FrogDraftData &document) {
    if(document.privateSourceAwaitingDiagramMigration)return *document.privateSourceAwaitingDiagramMigration;
    if(document.publicSourceAuthority)return std::string(document.publicSourceAuthority->originalUtf8());
    return serializeFrogDraft(document.widgets,document.title,document.interfaceMapLayoutPatternId,
        document.interfaceMapVisualTransforms,document.fileIconSvgText,document.diagramNodes,
        document.diagramZoomPercent,document.diagramViewportOrigin,document.diagramEdges,
        document.diagramWireFragments,document.interfaceBindingMapProfileId,document.properties,
        document.editorWindows,false,document.declaredInterface?&*document.declaredInterface:nullptr);
}
} // namespace frog::native_engine
