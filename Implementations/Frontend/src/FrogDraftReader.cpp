#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/ArrayCapacity.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/DiagramLiteralSchema.hpp"
#include "frog_native_engine/ConnectivityConfiguration.hpp"
#include "frog_native_engine/FrogIconPresentation.hpp"
#include "frog_native_engine/DiagramCommentTextRuns.hpp"
#include "frog_native_engine/DiagramCommentAttachment.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include "frog_native_engine/CollectionModel.hpp"
#include "frog_native_engine/CollectionBinding.hpp"
#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/FrogSourceDocument.hpp"
#include "frog_native_engine/NumericColorBox.hpp"
#include "frog_native_engine/RingItemIdentity.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/ClusterLayoutData.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/ErrorPaletteBinding.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "FrogDocumentConditionalDisable.hpp"
#include "FrogDocumentEncoding.hpp"
#include "FrogDraftJson.hpp"
#include "FrogDocumentArrayStorage.hpp"
#include "FrogDocumentIdentity.hpp"
#include "FrogDocumentJsonValue.hpp"
#include "FrogDocumentJsonWriter.hpp"
#include "FrogDocumentApplicationControlSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "FrogDocumentPublicImport.hpp"
#include "FrogJsonInternal.hpp"
#include "PortableText.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include "frog_native_engine/WidgetCatalog.hpp"
#include "frog_native_engine/WidgetPlacementContract.hpp"
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/TabControl.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include "FrogDocumentWaveformGraph.hpp"
#include "FrogDocumentXYGraph.hpp"
#include "FrogDocumentIntegerLexeme.hpp"
#include "FrogDocumentUnavailableNumeric.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <limits>
#include <map>
#include <optional>
#include <queue>
#include <sstream>
#include <set>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
namespace frog::native_engine {
namespace {
using document_internal::base64Decode;
using document_internal::formatNumber;
using document_internal::normalizedInterfaceConnectionRequirement;
using document_internal::diagramEdgeSerialFromId;
using document_internal::diagramNodeSerialFromId;
using document_internal::diagramWireFragmentSerialFromId;
using document_internal::documentWidgetCatalogEntry;
using document_internal::widgetSerialFromId;
using document_internal::JsonValue;
using document_internal::jsonBooleanField;
using document_internal::jsonNumberField;
using document_internal::jsonPixelField;
using document_internal::jsonStringArrayField;
using document_internal::jsonStringField;
using document_internal::jsonUnsignedArrayField;
using document_internal::jsonUnsignedField;
using document_internal::mutableObjectField;
using document_internal::objectField;
using document_internal::parseCssPixelValue;
FrogDraftInterfaceMapSettings retainedInterfaceMapSettings(
    const JsonValue *interfaceDeclaration, const JsonValue *rootFallback,
    const JsonValue *retainedRootMap = nullptr) {
    FrogDraftInterfaceMapSettings settings;
    const JsonValue *interfaceMap = nullptr;
    if (interfaceDeclaration != nullptr) {
        interfaceMap = objectField(*interfaceDeclaration, "map");
        if (interfaceMap == nullptr) {
            interfaceMap = objectField(*interfaceDeclaration, "interface_map");
        }
    }
    if (interfaceMap == nullptr && rootFallback != nullptr) {
        interfaceMap = objectField(*rootFallback, "interface_map");
    }
    if (interfaceMap == nullptr) interfaceMap = retainedRootMap;
    if (interfaceMap != nullptr) {
        settings.mapPresent = true;
        settings.profileId = jsonStringField(*interfaceMap, "binding_map_profile").value_or(jsonStringField(*interfaceMap, "layout_profile_id").value_or({}));
        settings.patternId =
            jsonStringField(*interfaceMap, "layout_pattern_id")
                .value_or(jsonStringField(*interfaceMap, "layoutPatternId").value_or({}));
        settings.transformTokens = jsonStringArrayField(*interfaceMap, "visual_transforms");
        if (settings.transformTokens.empty()) {
            settings.transformTokens = jsonStringArrayField(*interfaceMap, "visualTransforms");
        }
    }
    return settings;
}

bool isTransparentColorLiteral(std::string_view text) noexcept;
std::optional<std::uint32_t> parseRgbHex(std::string_view text) {
    if (isTransparentColorLiteral(text)) {
        return kTransparentColorModel;
    }
    if (!text.empty() && text.front() == '#') {
        text.remove_prefix(1);
    }
    if (text.size() != 6U) {
        return std::nullopt;
    }
    std::uint32_t value = 0;
    for (const char ch : text) {
        const int digit = portable_internal::asciiHexDigit(ch);
        if (digit < 0) {
            return std::nullopt;
        }
        value = (value << 4U) | static_cast<std::uint32_t>(digit);
    }
    return value;
}
std::string firstCssFontFamily(std::string_view text) {
    const auto comma = text.find(',');
    auto family = std::string(portable_internal::trimAsciiWhitespace(
        comma == std::string_view::npos ? text : text.substr(0, comma)));
    if (family.size() >= 2U &&
        ((family.front() == '"' && family.back() == '"') ||
         (family.front() == '\'' && family.back() == '\''))) {
        family = family.substr(1, family.size() - 2U);
    }
    return family;
}
std::string textStyleFromCssFields(
    std::string_view weight,
    std::string_view style,
    std::string_view decoration) {
    auto normalizedWeight = std::string(
        portable_internal::trimAsciiWhitespace(weight));
    auto normalizedStyle = std::string(
        portable_internal::trimAsciiWhitespace(style));
    auto normalizedDecoration = std::string(
        portable_internal::trimAsciiWhitespace(decoration));
    std::transform(normalizedWeight.begin(), normalizedWeight.end(), normalizedWeight.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    std::transform(normalizedStyle.begin(), normalizedStyle.end(), normalizedStyle.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    std::transform(normalizedDecoration.begin(), normalizedDecoration.end(), normalizedDecoration.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });

    std::string result;
    if (normalizedDecoration.find("line-through") != std::string::npos ||
        normalizedDecoration.find("strikeout") != std::string::npos) {
        result += "strikeout ";
    }
    if (normalizedDecoration.find("underline") != std::string::npos) {
        result += "underline ";
    }
    if (normalizedStyle.find("italic") != std::string::npos) {
        result += "italic ";
    }
    char *end = nullptr;
    const long numericWeight = std::strtol(normalizedWeight.c_str(), &end, 10);
    if (normalizedWeight == "bold" || (end != nullptr && *end == '\0' && numericWeight >= 700)) {
        result += "bold";
    }
    return canonicalTextStyle(result);
}

bool isTransparentColorLiteral(std::string_view text) noexcept {
    return text == "transparent" || text == "Transparent" || text == "TRANSPARENT";
}

// Preserve the aggregate cell object as a payload without turning nested
// objects back into recursively escaped strings. An explicit traversal also
// avoids imposing a call-stack depth limit on the authoring adapter.
std::string aggregateCellJson(const JsonValue &root) {
    struct Part { const JsonValue *value; std::string text; };
    std::vector<Part> pending{{&root,{}}};
    std::ostringstream output;
    while (!pending.empty()) {
        auto part=std::move(pending.back());pending.pop_back();
        if (!part.value) { output<<part.text;continue; }
        const auto &value=*part.value;
        switch (value.type) {
        case JsonValue::Type::Null: output<<"null";break;
        case JsonValue::Type::Boolean: output<<(value.boolean?"true":"false");break;
        case JsonValue::Type::Number: output<<(value.string.empty()?formatNumber(value.number):value.string);break;
        case JsonValue::Type::String: document_internal::appendJsonString(output,value.string);break;
        case JsonValue::Type::Array:
            output<<'[';pending.push_back({nullptr,"]"});
            for (auto it=value.array.rbegin();it!=value.array.rend();++it) {
                if (it!=value.array.rbegin()) pending.push_back({nullptr,","});
                pending.push_back({&*it,{}});
            }
            break;
        case JsonValue::Type::Object:
            output<<'{';pending.push_back({nullptr,"}"});
            for (auto it=value.object.rbegin();it!=value.object.rend();++it) {
                if (it!=value.object.rbegin()) pending.push_back({nullptr,","});
                pending.push_back({&it->second,{}});
                std::ostringstream key;document_internal::appendJsonString(key,it->first);
                pending.push_back({nullptr,key.str()+":"});
            }
            break;
        }
    }
    return output.str();
}

std::optional<ArrayElementValue> jsonArrayElementValue(const JsonValue &value,std::string_view source={}) {
    if (value.type != JsonValue::Type::Object) {
        return std::nullopt;
    }
    const auto indices = jsonUnsignedArrayField(value, "indices");
    const auto *rawValue = objectField(value, "value");
    if (indices.empty() || rawValue == nullptr) {
        return std::nullopt;
    }

    ArrayElementValue element;
    element.indices = indices;
    element.defined = jsonBooleanField(value, "defined").value_or(true);
    element.enabled = jsonBooleanField(value, "enabled").value_or(true);
    switch (rawValue->type) {
    case JsonValue::Type::Number:
        // The JSON lexeme is authoritative: projecting through binary64 here
        // loses U64 precision, signed zero and authored decimal/exponent text.
        element.displayValue = rawValue->string.empty()
            ? formatNumber(rawValue->number) : rawValue->string;
        element.kind = ArrayElementValueKind::Number;
        break;
    case JsonValue::Type::String:
        element.displayValue = rawValue->string;
        {
            const auto kind=jsonStringField(value,"kind").value_or("");
            element.kind=kind=="widget"?ArrayElementValueKind::Widget:
                kind=="number"?ArrayElementValueKind::Number:
                kind=="boolean"?ArrayElementValueKind::Boolean:ArrayElementValueKind::String;
        }
        break;
    case JsonValue::Type::Boolean:
        element.displayValue = rawValue->boolean ? "true" : "false";
        element.kind = ArrayElementValueKind::Boolean;
        break;
    case JsonValue::Type::Object:
        if (jsonStringField(value, "kind").value_or("") != "widget") return std::nullopt;
        element.displayValue = rawValue->sourceBegin!=std::string_view::npos &&
            rawValue->sourceBegin<=rawValue->sourceEnd && rawValue->sourceEnd<=source.size()
            ? std::string(source.substr(rawValue->sourceBegin,rawValue->sourceEnd-rawValue->sourceBegin))
            : aggregateCellJson(*rawValue);
        element.kind = ArrayElementValueKind::Widget;
        break;
    default:
        return std::nullopt;
    }
    return element;
}

std::optional<Rect> jsonBounds(const JsonValue &value) {
    const auto *bounds = objectField(value, "bounds");
    if (bounds == nullptr) {
        return std::nullopt;
    }

    const auto x = jsonNumberField(*bounds, "x");
    const auto y = jsonNumberField(*bounds, "y");
    const auto width = jsonNumberField(*bounds, "width");
    const auto height = jsonNumberField(*bounds, "height");
    if (!x.has_value() || !y.has_value() || !width.has_value() || !height.has_value()) {
        return std::nullopt;
    }
    return Rect{ x.value(), y.value(), width.value(), height.value() };
}

std::optional<Rect> jsonLayoutBounds(const JsonValue &value) {
    const auto *layout = objectField(value, "layout");
    if (layout == nullptr) {
        return std::nullopt;
    }

    const auto x = jsonNumberField(*layout, "x");
    const auto y = jsonNumberField(*layout, "y");
    const auto width = jsonNumberField(*layout, "width");
    const auto height = jsonNumberField(*layout, "height");
    if (!x.has_value() || !y.has_value() || !width.has_value() || !height.has_value()) {
        return std::nullopt;
    }
    return Rect{ x.value(), y.value(), width.value(), height.value() };
}

std::optional<Rect> jsonObjectBounds(const JsonValue &value) {
    if (const auto bounds = jsonBounds(value)) {
        return bounds;
    }
    return jsonLayoutBounds(value);
}

void overrideStringIfPresent(std::string &target, const JsonValue &object, std::string_view fieldName) {
    const auto value = jsonStringField(object, fieldName);
    if (value.has_value()) {
        target = value.value();
    }
}

const JsonValue *studioStaticVisualObject(const JsonValue &value) noexcept {
    const auto *studio = objectField(value, "studio");
    return studio == nullptr ? nullptr : objectField(*studio, "staticVisual");
}

void loadEmbeddedStaticVisual(const JsonValue &visualObject, Rect bounds, StaticVisual &visual) {
    const auto kind = jsonStringField(visualObject, "kind").value_or("svg");
    visual.kind = kind == "dib" ? StaticVisualKind::Dib : StaticVisualKind::Svg;
    visual.sourceName = jsonStringField(visualObject, "sourceName").value_or(visual.sourceName);
    visual.mediaRef = jsonStringField(visualObject, "mediaRef").value_or(visual.mediaRef);
    if (const auto *chunks = objectField(visualObject, "mediaRefChunks"); chunks && chunks->type == JsonValue::Type::Array) {
        visual.mediaRef.clear();
        for (const auto &chunk : chunks->array) {
            if (chunk.type != JsonValue::Type::String) { visual.mediaRef.clear(); break; }
            visual.mediaRef += chunk.string;
        }
    }
    visual.mediaType = jsonStringField(visualObject, "mediaType").value_or(visual.mediaType);
    visual.svgText = jsonStringField(visualObject, "svgText").value_or(visual.svgText);
    visual.dibBytes = base64Decode(jsonStringField(visualObject, "dibBase64").value_or({}));
    visual.naturalWidth = jsonNumberField(visualObject, "naturalWidth").value_or(bounds.width);
    visual.naturalHeight = jsonNumberField(visualObject, "naturalHeight").value_or(bounds.height);
    visual.preserveAspectRatio = jsonBooleanField(visualObject, "preserveAspectRatio").value_or(true);
    const int rotation = static_cast<int>(jsonNumberField(visualObject, "rotationQuarterTurns").value_or(visual.rotationQuarterTurns));
    visual.rotationQuarterTurns = ((rotation % 4) + 4) % 4;
    visual.flipHorizontal = jsonBooleanField(visualObject, "flipHorizontal").value_or(visual.flipHorizontal);
    visual.flipVertical = jsonBooleanField(visualObject, "flipVertical").value_or(visual.flipVertical);
}

SceneObject sceneObjectFromJsonImpl(const JsonValue &value, int fallbackIndex,std::string_view source);

int childWidgetFallbackIndex(int parent, std::size_t index) {
    const auto wide = static_cast<std::uint64_t>(parent) * 1000U + index + 1U;
    return static_cast<int>(wide % static_cast<std::uint64_t>((std::numeric_limits<int>::max)()));
}

SceneObject sceneObjectFromJson(const JsonValue &encodedValue, int fallbackIndex,std::string_view source={}) {
    const auto *decodedValue=document_internal::privateWidgetValueObject(encodedValue);
    if(!decodedValue)return {};
    const auto &value=*decodedValue;
    const auto restoreTabParent = [&](SceneObject &object) {
        object.parentTabId = jsonStringField(value, "studio_tab_parent_id").value_or("");
        object.parentTabPageId = jsonStringField(value, "studio_tab_page_id").value_or("");
    };
    // Decode children once, bottom-up, rather than retaining a large native
    // SceneObject stack frame for every nested Array/Cluster.
    struct Cache : std::map<const JsonValue *, SceneObject> { std::string_view source; };
    static thread_local Cache *active = nullptr;
    if(source.empty() && active)source=active->source;
    if (active) {
        const auto found = active->find(&value);
        if (found != active->end()) {
            auto result = std::move(found->second);
            active->erase(found);
            restoreTabParent(result);
            return result;
        }
    }
    Cache cache;
    cache.source=source;
    struct Restore { Cache *&slot; Cache *previous; ~Restore() { slot = previous; } } restore{active, active};
    active = &cache;
    struct Frame { const JsonValue *value; int index; bool visited; };
    std::vector<Frame> pending{{&value, fallbackIndex, false}};
    while (!pending.empty()) {
        const auto frame = pending.back();pending.pop_back();
        if (frame.visited) {
            cache.emplace(frame.value, sceneObjectFromJsonImpl(*frame.value, frame.index,source));
            continue;
        }
        pending.push_back({frame.value, frame.index, true});
        const auto *props = objectField(*frame.value, "props");
        if (!props) continue;
        if (const auto *element = objectField(*props, "element.template"))
            pending.push_back({element, childWidgetFallbackIndex(frame.index, 0U), false});
        if (const auto *fields = objectField(*props, "cluster.elements"); fields && fields->type == JsonValue::Type::Array) {
            for (std::size_t index = fields->array.size(); index > 0U; --index)
                if (const auto *child = objectField(fields->array[index - 1U], "widget"))
                    pending.push_back({child, childWidgetFallbackIndex(frame.index, index - 1U), false});
        }
    }
    auto result = std::move(cache.at(&value));
    restoreTabParent(result);
    return result;
}

SceneObject sceneObjectFromJsonImpl(const JsonValue &value, int fallbackIndex,std::string_view source) {
    const auto id = jsonStringField(value, "id").value_or("widget-load-" + std::to_string(fallbackIndex));
    const auto bounds = jsonObjectBounds(value).value_or(Rect{ 0.0, 0.0, 96.0, 32.0 });
    const bool locked = jsonBooleanField(value, "locked").value_or(false);
    const bool hidden = jsonBooleanField(value, "hidden").value_or(false);
    const auto type = jsonStringField(value, "type");
    const auto classRef = jsonStringField(value, "class_ref");
    if ((type.has_value() && type.value() == "staticVisual") ||
        (classRef.has_value() && classRef.value() == "frog.widgets.image_static")) {
        StaticVisual visual;
        visual.naturalWidth = bounds.width;
        visual.naturalHeight = bounds.height;

        if (const auto *props = objectField(value, "props")) {
            visual.mediaRef = jsonStringField(*props, "media_ref").value_or({});
            visual.mediaType = jsonStringField(*props, "media_type").value_or({});
            visual.naturalWidth = jsonNumberField(*props, "media.intrinsic_width").value_or(visual.naturalWidth);
            visual.naturalHeight = jsonNumberField(*props, "media.intrinsic_height").value_or(visual.naturalHeight);
            visual.preserveAspectRatio = jsonBooleanField(*props, "media.preserve_aspect_ratio").value_or(true);
            const int rotation = static_cast<int>(jsonNumberField(*props, "transform.rotation_quarter_turns").value_or(visual.rotationQuarterTurns));
            visual.rotationQuarterTurns = ((rotation % 4) + 4) % 4;
            visual.flipHorizontal = jsonBooleanField(*props, "transform.flip_horizontal").value_or(visual.flipHorizontal);
            visual.flipVertical = jsonBooleanField(*props, "transform.flip_vertical").value_or(visual.flipVertical);
        }

        if (const auto *embeddedVisual = studioStaticVisualObject(value)) {
            loadEmbeddedStaticVisual(*embeddedVisual, bounds, visual);
        } else if (const auto *visualObject = objectField(value, "visual")) {
            if (jsonStringField(*visualObject, "kind").has_value() ||
                jsonStringField(*visualObject, "svgText").has_value() ||
                jsonStringField(*visualObject, "dibBase64").has_value()) {
                loadEmbeddedStaticVisual(*visualObject, bounds, visual);
            }
        }

        return SceneObject{ id, bounds, std::nullopt, std::move(visual), locked, hidden };
    }

    const auto classId = jsonStringField(value, "classId").value_or(classRef.value_or({}));
    if (classId.empty()) {
        return SceneObject{ id, bounds, std::nullopt, std::nullopt, locked, hidden };
    }

    const auto *entry = documentWidgetCatalogEntry(classId);
    if (entry == nullptr) {
        return SceneObject{ id, bounds, std::nullopt, std::nullopt, locked, hidden };
    }

    auto widget = makeWidgetData(id, *entry, bounds);
    // A legacy Label frame was authored without an automatic-sizing flag.
    // Preserve it on load; newly authored Labels still default to auto-fit.
    if (isLabelWidget(widget)) widget.commentAutoSize = false;
    widget.classId = classId;
    overrideStringIfPresent(widget.classId, value, "classId");
    if (!classRef.has_value()) {
        overrideStringIfPresent(widget.role, value, "role");
    }
    if (widget.classId == "frog.widgets.boolean_control" ||
        widget.classId == "frog.widgets.array_control" ||
        widget.classId == "frog.widgets.cluster_control") widget.role = "control";
    else if (widget.classId == "frog.widgets.boolean_indicator" ||
             widget.classId == "frog.widgets.array_indicator" ||
             widget.classId == "frog.widgets.cluster_indicator") widget.role = "indicator";
    // Older documents omitted the role-based default (shown on controls).
    // New widgets persist Hidden explicitly; preserve older authored displays.
    if (isBooleanWidget(widget)) widget.buttonStateTextVisible.reset();
    if (isPictureWidget(widget)) static_cast<void>(setPictureWidgetIndicatorRole(widget, true));
    overrideStringIfPresent(widget.valueType, value, "valueType");
    overrideStringIfPresent(widget.valueType, value, "value_type");
    // Initialize metadata from an explicit supported Complex declaration.
    // A stale legacy display property must not leave the generic Numeric
    // prototype's Float64 representation on a real CSG/CDB widget.
    if(isNumericWidget(widget)&&numericCanonicalValueTypeIsComplex(widget.valueType))
        widget.numericRepresentationKind=widget.valueType=="c32"?"complex_float32":"complex_float64";
    if (isReferenceWidget(widget) && value.member("studio_application_control")) {
        DiagramNode carrier;carrier.id=id;carrier.kind="primitive";
        if(const auto *reference=findFunctionCatalogEntry("application_control.reference"))
            carrier.type=functionDiagramNodeType(*reference);
        if(!document_internal::readApplicationControl(value,carrier,nullptr) || !carrier.applicationControl ||
            carrier.applicationControl->kind!=ApplicationControlKind::Reference)
            return SceneObject{id,bounds,std::nullopt,std::nullopt,locked,hidden};
        widget.referenceValue=std::move(carrier.applicationControl);
    }
    if (isReferenceWidget(widget) && !binding::widgetType(widget).valid())
        return SceneObject{id,bounds,std::nullopt,std::nullopt,locked,hidden};
    if (isPictureWidget(widget)) widget.valueType = "frog.array<u32,rank=2>";
    overrideStringIfPresent(widget.label, value, "label");

    if(widget.classId==kCollectionWidgetClass) {
        const auto *props=objectField(value,"props");const auto *stored=props?props->member("collection.state"):nullptr;
        const auto state=stored?readCollectionAuthoringState(*stored):std::nullopt;
        if(!state||collectionType(*state).canonical()!=widget.valueType)return SceneObject{id,bounds,std::nullopt,std::nullopt,locked,hidden};
        widget.collection=*state;
    }
    if (const auto *props = objectField(value, "props")) {
        if(const auto *link=props->member("type_definition")) {
            std::ostringstream encoded;document_internal::appendJsonValue(encoded,*link);
            const auto decoded=deserializeTypeDefinitionLink(encoded.str());
            if(!decoded)return SceneObject{id,bounds,std::nullopt,std::nullopt,locked,hidden};
            widget.typeDefinition=*decoded;
        }
        if (widget.valueType == "variant") {
            widget.initialValue=jsonStringField(*props,"variant.payload").value_or("");
            widget.variantShowType=jsonBooleanField(*props,"variant.show_type");
            widget.variantShowData=jsonBooleanField(*props,"variant.show_data");
            widget.variantScrollbarVisible=jsonBooleanField(*props,"variant.scrollbar_visible");
        }
        overrideStringIfPresent(widget.label, *props, "label");
        if (isXYGraphWidget(widget)) {
            if(const auto *graphValue=objectField(*props,"xy_graph"))
                document_internal::deserializeXYGraph(*graphValue,*widget.xyGraph);
            if(binding::parseType(widget.valueType).type.canonical()!=widget.xyGraph->datatype.canonical()) {
                if(!widget.xyGraph->preservedSource)
                    widget.xyGraph->preservedSource=document_internal::serializeXYGraph(*widget.xyGraph);
                widget.xyGraph->diagnostic="Graph XY declared terminal type and retained data schema disagree.";
            }
        }
        if (isWaveformGraphWidget(widget)) {
            if (const auto *graphValue = objectField(*props, "waveform_graph"))
                document_internal::deserializeWaveformGraph(*graphValue, *widget.waveformGraph);
        }
        overrideStringIfPresent(widget.label, *props, "label.text");
        if (const auto auraBand = jsonNumberField(*props, "layout.aura_band_px")) {
            widget.placementAuraBand = (std::max)(0.0, auraBand.value());
        }
        if (isArrayWidget(widget)) {
            auto *array = arrayContainerState(widget);
            if (array != nullptr) {
                if (const auto color = jsonStringField(*props, "style.frame.fill_color")) {
                    array->frameFillRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.frame.border_color")) {
                    array->frameStrokeRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.array_region.fill_color")) {
                    array->arrayRegionFillRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.array_region.border_color")) {
                    array->arrayRegionStrokeRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.index_display.container_fill_color")) {
                    array->indexContainerFillRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.index_display.container_border_color")) {
                    array->indexContainerStrokeRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.index_display.value_fill_color")) {
                    array->indexValueFillRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.index_display.value_border_color")) {
                    array->indexValueStrokeRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.index_display.button_fill_color")) {
                    array->indexStepButtonFillRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.index_display.button_border_color")) {
                    array->indexStepButtonStrokeRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.index_display.arrow_fill_color")) {
                    array->indexStepArrowFillRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.index_display.arrow_border_color")) {
                    array->indexStepArrowStrokeRgb = parseRgbHex(color.value());
                }
                if (const auto rank = jsonUnsignedField(*props, "dimensions.rank")) {
                    array->rank = std::clamp(rank.value(), 1U, 16U);
                }
                auto shape = jsonUnsignedArrayField(*props, "dimensions.shape");
                if (!shape.empty()) {
                    array->shape = std::move(shape);
                }
                auto firstIndices = jsonUnsignedArrayField(*props, "viewport.first_indices");
                if (!firstIndices.empty()) {
                    array->firstIndices = std::move(firstIndices);
                }
                auto visibleCounts = jsonUnsignedArrayField(*props, "viewport.visible_counts");
                if (!visibleCounts.empty()) {
                    array->visibleCounts = std::move(visibleCounts);
                }
                if (const auto orientation = jsonStringField(*props, "viewport.orientation")) {
                    if (orientation.value() == "vertical" ||
                        orientation.value() == "horizontal" ||
                        orientation.value() == "grid" ||
                        orientation.value() == "matrix") {
                        array->orientation = orientation.value();
                    }
                }
                if (const auto allowResize = jsonBooleanField(*props, "dimensions.allow_resize")) {
                    array->allowResize = allowResize.value();
                }
                if (const auto visible = jsonBooleanField(*props, "index_display.visible")) {
                    array->indexDisplayVisible = visible.value();
                }
                if (const auto visible = jsonBooleanField(*props, "display.vertical_scrollbar_visible")) {
                    array->verticalScrollbarVisible = visible.value();
                } else if (const auto legacyVisible = jsonBooleanField(*props, "scrollbars.vertical.visible")) {
                    array->verticalScrollbarVisible = legacyVisible.value();
                }
                if (const auto visible = jsonBooleanField(*props, "display.horizontal_scrollbar_visible")) {
                    array->horizontalScrollbarVisible = visible.value();
                } else if (const auto legacyVisible = jsonBooleanField(*props, "scrollbars.horizontal.visible")) {
                    array->horizontalScrollbarVisible = legacyVisible.value();
                }
                array->scrollbarGap = (std::max)(
                    0.0,
                    jsonNumberField(*props, "viewport.scrollbar_gap").value_or(array->scrollbarGap));
                array->indexDisplayWidth = (std::max)(
                    24.0,
                    jsonNumberField(*props, "index_display.layout.box_width")
                        .value_or(jsonNumberField(*props, "index_display.layout.width")
                            .value_or(array->indexDisplayWidth)));
                array->indexBoxHeight = (std::max)(
                    20.0,
                    jsonNumberField(*props, "index_display.layout.box_height").value_or(array->indexBoxHeight));
                array->indexBoxGap = (std::max)(
                    0.0,
                    jsonNumberField(*props, "index_display.layout.gap").value_or(array->indexBoxGap));
                array->indexStepWidth = (std::clamp)(
                    jsonNumberField(*props, "index_display.layout.step_width").value_or(array->indexStepWidth),
                    10.0,
                    array->indexDisplayWidth - 8.0);
                array->indexStepGap = (std::max)(
                    0.0,
                    jsonPixelField(*props, "style.index_display.step_gap").value_or(array->indexStepGap));
                array->indexValueGap = (std::max)(
                    0.0,
                    jsonPixelField(*props, "style.index_display.value_gap").value_or(array->indexValueGap));
                array->indexContainerPadding = (std::max)(
                    0.0,
                    jsonPixelField(*props, "style.index_display.container_padding")
                        .value_or(array->indexContainerPadding));
                array->indexContainerBorderWidth = (std::max)(
                    0.0,
                    jsonPixelField(*props, "style.index_display.container_border_width")
                        .value_or(array->indexContainerBorderWidth));
                const auto cellWidth = jsonNumberField(*props, "element.cell_width")
                    .value_or(jsonNumberField(*props, "viewport.column_width").value_or(array->cellWidth));
                const auto cellHeight = jsonNumberField(*props, "element.cell_height")
                    .value_or(jsonNumberField(*props, "viewport.row_height").value_or(array->cellHeight));
                array->cellWidth = (std::max)(24.0, cellWidth);
                array->cellHeight = (std::max)(20.0, cellHeight);
                array->elementGap = (std::clamp)(
                    jsonNumberField(*props, "element.gap").value_or(array->elementGap),
                    0.0,
                    64.0);
                array->elementClassId = jsonStringField(*props, "element.class_id").value_or("");
                array->elementValueType = jsonStringField(*props, "element.value_type").value_or("");
                array->elementRole = jsonStringField(*props, "element.role").value_or(widget.role);
                array->widgetComposedElements =
                    objectField(*props, "element.template") != nullptr ||
                    jsonStringField(*props, "element.template_ref").has_value();
                if (!array->widgetComposedElements && !array->elementClassId.empty()) {
                    array->widgetComposedElements =
                        documentWidgetCatalogEntry(array->elementClassId) != nullptr;
                }
                array->elements.clear();
                if (const auto *elements = objectField(*props, "elements");
                    elements != nullptr && elements->type == JsonValue::Type::Array) {
                    array->elements.reserve(elements->array.size());
                    for (const auto &item : elements->array) {
                        if (objectField(item, "widget") != nullptr) {
                            array->widgetComposedElements = true;
                        }
                        if (auto elementValue = jsonArrayElementValue(item,source)) {
                            array->elements.push_back(std::move(elementValue.value()));
                        }
                    }
                }
                // With no element schema or cells, an absent role is an
                // absent private editing cache, not a chosen control element.
                // Explicit roles and all configured/legacy fallbacks remain.
                if (!objectField(*props, "element.role") && !array->widgetComposedElements &&
                    array->elementClassId.empty() && array->elementValueType.empty() && array->elements.empty())
                    array->elementRole.clear();
                array->elementIcon = jsonBooleanField(*props, "element.icon").value_or(false);
                const bool widgetComposedElements = array->widgetComposedElements;

                std::optional<WidgetInstance> element;
                std::optional<std::string> authoredElementId;
                if (const auto *elementTemplate = objectField(*props, "element.template")) {
                    if (const auto *definition=document_internal::privateWidgetValueObject(*elementTemplate))
                        authoredElementId=jsonStringField(*definition,"id");
                    auto parsed = sceneObjectFromJson(*elementTemplate, childWidgetFallbackIndex(fallbackIndex, 0U));
                    if (parsed.widget.has_value()) {
                        element = std::move(parsed.widget.value());
                    }
                }
                if (!element.has_value() && widgetComposedElements) {
                    if (const auto elementClassId = jsonStringField(*props, "element.class_id")) {
                        JsonValue elementObject;
                        elementObject.type = JsonValue::Type::Object;
                        const auto appendStringValue = [](JsonValue &object, std::string key, std::string text) {
                            JsonValue field;
                            field.type = JsonValue::Type::String;
                            field.string = std::move(text);
                            object.object.emplace(std::move(key), std::move(field));
                        };
                        const auto appendNumberValue = [](JsonValue &object, std::string key, double number) {
                            JsonValue field;
                            field.type = JsonValue::Type::Number;
                            field.number = number;
                            object.object.emplace(std::move(key), std::move(field));
                        };

                        appendStringValue(elementObject, "id", id + ".element");
                        appendStringValue(elementObject, "classId", elementClassId.value());
                        appendStringValue(
                            elementObject,
                            "role",
                            jsonStringField(*props, "element.role").value_or(widget.role));
                        appendStringValue(
                            elementObject,
                            "value_type",
                            jsonStringField(*props, "element.value_type").value_or(""));

                        JsonValue layout;
                        layout.type = JsonValue::Type::Object;
                        appendNumberValue(layout, "x", 0.0);
                        appendNumberValue(layout, "y", 0.0);
                        appendNumberValue(layout, "width", array->cellWidth);
                        appendNumberValue(layout, "height", array->cellHeight);
                        elementObject.object.emplace("layout", std::move(layout));

                        JsonValue elementProps;
                        elementProps.type = JsonValue::Type::Object;
                        constexpr std::string_view elementPropsPrefix = "element.props.";
                        for (const auto &[key, field] : props->object) {
                            if (key.rfind(elementPropsPrefix, 0) == 0U) {
                                elementProps.object.emplace(
                                    key.substr(elementPropsPrefix.size()),
                                    field);
                            }
                        }
                        elementObject.object.emplace("props", std::move(elementProps));

                        JsonValue runtime;
                        runtime.type = JsonValue::Type::Object;
                        if (const auto asset = jsonStringField(*props, "element.asset_ref")) {
                            appendStringValue(runtime, "asset", asset.value());
                        }
                        if (const auto popup = jsonStringField(*props, "element.popup_asset_ref")) {
                            appendStringValue(runtime, "popupAsset", popup.value());
                        }
                        if (!runtime.object.empty()) {
                            elementObject.object.emplace("runtime", std::move(runtime));
                        }

                        auto parsed = sceneObjectFromJson(elementObject, childWidgetFallbackIndex(fallbackIndex, 0U));
                        if (parsed.widget.has_value()) {
                            element = std::move(parsed.widget.value());
                            const auto &parsedProps = *objectField(elementObject, "props");
                            if (isPathWidget(element.value())) {
                                if (const auto iconVisible = jsonBooleanField(parsedProps, "display.icon_visible")) {
                                    static_cast<void>(setPathFolderVisible(element.value(), iconVisible.value()));
                                }
                                if (const auto browseVisible = jsonBooleanField(parsedProps, "browse.button_visible")) {
                                    static_cast<void>(setPathBrowseButtonVisible(element.value(), browseVisible.value()));
                                }
                            }
                        }
                    }
                }
                if (element.has_value()) {
                    const auto authoredCellWidth=array->cellWidth, authoredCellHeight=array->cellHeight;
                    const bool authoredTemplate=objectField(*props, "element.template")!=nullptr;
                    if (!array->elementIcon && !authoredTemplate) {
                        element->bounds.width = authoredCellWidth;
                        element->bounds.height = authoredCellHeight;
                    }
                    static_cast<void>(setArrayElementTemplate(widget, element.value()));
                    // Template replacement commits a new container state.
                    // The pointer obtained before that transaction is expired.
                    array = arrayContainerState(widget);
                    // Installation authors a new derived ID; reading an explicit
                    // template preserves its existing source identity instead.
                    if (array && array->elementTemplate && authoredElementId && !authoredElementId->empty())
                        array->elementTemplate->instanceId=*authoredElementId;
                    // Cell allocation and authored element geometry are separate.
                    // Legacy drafts without a template still derive its bounds
                    // above, while explicit templates keep their own dimensions.
                    if (array != nullptr && (array->elementIcon || authoredTemplate)) {
                        array->cellWidth=authoredCellWidth; array->cellHeight=authoredCellHeight;
                    }
                }

                array = arrayContainerState(widget);
                if (array != nullptr) {
                    array->widgetComposedElements = widgetComposedElements;
                    array->rank = std::clamp(array->rank, 1U, 16U);
                    array->shape.resize(array->rank, 1U);
                    array->firstIndices.resize(array->rank, 0U);
                    const std::size_t visibleAxisCount = array->rank == 1U ? 1U : 2U;
                    const auto minimumVisibleAxis = minimumArrayVisibleAxisCount(array->rank);
                    array->visibleCounts.resize(visibleAxisCount, minimumVisibleAxis);
                    for (auto &count : array->visibleCounts) {
                        count = std::clamp(count,minimumVisibleAxis,kMaximumArrayVisibleCells);
                    }
                    if(array->rank>1) {
                        const auto grid=boundedArrayViewportGrid({array->visibleCounts[0],array->visibleCounts[1]});
                        array->visibleCounts={grid.rows,grid.columns};
                    }
                }
            }
        }
        if (isClusterWidget(widget)) {
            auto *cluster = clusterContainerState(widget);
            if (cluster != nullptr) {
                if(const auto serial=jsonStringField(*props,"cluster.next_field_serial")) {
                    std::uint64_t next=0;const auto read=std::from_chars(serial->data(),serial->data()+serial->size(),next);
                    if(read.ec!=std::errc{} || read.ptr!=serial->data()+serial->size())
                        return SceneObject{id,bounds,std::nullopt,std::nullopt,locked,hidden};
                    cluster->nextFieldSerial=next;
                }
                cluster->autoSizing = parseClusterAutoSizing(
                    jsonStringField(*props, "cluster.auto_sizing").value_or("none"));
                if (const auto padding = jsonNumberField(*props, "cluster.padding")) {
                    cluster->padding = (std::max)(0.0, padding.value());
                }
                if (const auto color = jsonStringField(*props, "style.frame.fill_color")) {
                    cluster->frameFillRgb = parseRgbHex(color.value());
                }
                if (const auto color = jsonStringField(*props, "style.frame.border_color")) {
                    cluster->frameStrokeRgb = parseRgbHex(color.value());
                }
                cluster->elements.clear();
                if (const auto *elements = objectField(*props, "cluster.elements");
                    elements != nullptr && elements->type == JsonValue::Type::Array) {
                    cluster->elements.reserve(elements->array.size());
                    for (std::size_t index = 0U; index < elements->array.size(); ++index) {
                        const auto &fieldValue = elements->array[index];
                        const auto *widgetValue = objectField(fieldValue, "widget");
                        if (widgetValue == nullptr) {
                            continue;
                        }
                        auto parsed = sceneObjectFromJson(
                            *widgetValue,
                            childWidgetFallbackIndex(fallbackIndex, index));
                        if (!parsed.widget.has_value() ||
                            !isSupportedClusterElementWidget(parsed.widget.value())) {
                            continue;
                        }
                        const auto order = jsonUnsignedField(fieldValue, "logical_order")
                            .value_or(static_cast<std::uint32_t>(index));
                        std::string fieldId = jsonStringField(fieldValue, "field_id")
                            .value_or(id + ".field." + std::to_string(order));
                        auto contained = std::make_shared<WidgetInstance>(
                            std::move(parsed.widget.value()));
                        // Field identity addresses the cluster schema; a stored
                        // widget identity belongs to its presentation instance.
                        // Preserve both. Only old children without an explicit
                        // ID use the historical field-derived fallback.
                        const auto *definition=document_internal::privateWidgetValueObject(*widgetValue);
                        if (!definition || !jsonStringField(*definition,"id"))
                            contained->instanceId = fieldId;
                        contained->publicInputId.reset();
                        contained->publicOutputId.reset();
                        contained->interfaceMapSlotId.reset();
                        contained->interfaceConnectionRequirement.reset();
                        cluster->elements.push_back(ClusterElement{
                            std::move(fieldId),
                            jsonStringField(fieldValue, "technical_name")
                                .value_or("field_" + std::to_string(order)),
                            order,
                            std::move(contained)
                        });
                    }
                    std::stable_sort(
                        cluster->elements.begin(),
                        cluster->elements.end(),
                        [](const ClusterElement &left, const ClusterElement &right) {
                            return left.logicalOrder < right.logicalOrder;
                        });
                }
            }
        }
        if (isNumericWidget(widget)) {
            if (const auto representation = jsonStringField(*props, "representation.kind")) {
                static_cast<void>(setNumericRepresentationKind(widget, representation.value()));
            } else if (const auto legacyRepresentation = jsonStringField(*props, "data_type.representation")) {
                static_cast<void>(setNumericRepresentationKind(widget, legacyRepresentation.value()));
            } else if (const auto namedSize = jsonStringField(*props, "data_type.named_numeric_size")) {
                static_cast<void>(setNumericRepresentationKind(widget, namedSize.value()));
            }
            if (numericRepresentationKind(widget) == "fixed_point") {
                auto configuration = fixedPointConfiguration(widget);
                configuration.signedEncoding =
                    jsonBooleanField(*props, "representation.fixed_point.signed")
                        .value_or(configuration.signedEncoding);
                configuration.complex =
                    jsonBooleanField(*props, "representation.fixed_point.complex")
                        .value_or(configuration.complex);
                if (const auto wordLength =
                        jsonNumberField(*props, "representation.fixed_point.word_length")) {
                    configuration.wordLength = static_cast<int>(std::lround(wordLength.value()));
                }
                if (const auto integerWordLength =
                        jsonNumberField(*props, "representation.fixed_point.integer_word_length")) {
                    configuration.integerWordLength =
                        static_cast<int>(std::lround(integerWordLength.value()));
                }
                if (const auto roundingMode =
                        jsonStringField(*props, "representation.fixed_point.rounding_mode")) {
                    configuration.roundingMode = roundingMode.value();
                }
                if (const auto overflowMode =
                        jsonStringField(*props, "representation.fixed_point.overflow_mode")) {
                    configuration.overflowMode = overflowMode.value();
                }
                static_cast<void>(setWidgetFixedPointConfiguration(widget, configuration));
            }
        }
        if (isNumericWidget(widget) || isRingWidget(widget)) {
            widget.numericColorBox = isNumericWidget(widget) && widget.valueType == "u32" &&
                jsonBooleanField(*props, "display.color_box").value_or(false);
            const bool useTypeDefaults = jsonBooleanField(*props, "data_entry.use_type_defaults").value_or(false);
            if (!useTypeDefaults) {
                if (const auto minimum = jsonNumberField(*props, "data_entry.minimum")) {
                    widget.numericMinimum = minimum.value();
                }
                if (const auto maximum = jsonNumberField(*props, "data_entry.maximum")) {
                    widget.numericMaximum = maximum.value();
                }
                if (const auto response = jsonStringField(*props, "data_entry.minimum_response")) {
                    widget.numericMinimumResponse = response.value();
                }
                if (const auto response = jsonStringField(*props, "data_entry.maximum_response")) {
                    widget.numericMaximumResponse = response.value();
                }
                if (const auto increment = jsonNumberField(*props, "data_entry.increment_step")) {
                    static_cast<void>(setWidgetNumericIncrementStep(widget, increment.value()));
                }
                if (const auto response = jsonStringField(*props, "data_entry.increment_response")) {
                    widget.numericIncrementResponse = response.value();
                }
                if (const auto pageStep = jsonNumberField(*props, "data_entry.page_step")) {
                    widget.numericPageStep = pageStep.value();
                }
            }
        }
        if (widget.valueType == "timestamp") {
            const auto content = jsonStringField(*props, "timestamp.display.content");
            if (content && (*content == "date_time" || *content == "date" || *content == "time"))
                widget.timestampDisplayContent = *content;
            widget.timestampShowSeconds = jsonBooleanField(*props, "timestamp.display.seconds");
            widget.timestampDateFirst = jsonBooleanField(*props, "timestamp.display.date_first");
            widget.timestampSingleLine = jsonBooleanField(*props, "timestamp.display.single_line");
            widget.timestampClockIconVisible = jsonBooleanField(*props, "timestamp.clock.visible");
        }
        if (isNumericWidget(widget) || isRingWidget(widget) || isEnumWidget(widget)) {
            if (const auto format = jsonStringField(*props, "display.format.kind")) {
                widget.numericDisplayFormat = format.value();
            }
            if (const auto showNumericValue =
                    jsonBooleanField(*props, "display.format.show_numeric_value")) {
                widget.choiceNumericValueVisible = showNumericValue.value();
            }
            if (const auto digits = jsonNumberField(*props, "display.format.digits")) {
                widget.numericDisplayDigits = std::clamp(static_cast<int>(std::lround(digits.value())), 1, 18);
            }
            if (const auto precision = jsonStringField(*props, "display.format.precision_type")) {
                widget.numericPrecisionType = precision.value();
            }
            if (const auto hideZeros = jsonBooleanField(*props, "display.format.hide_trailing_zeros")) {
                widget.numericHideTrailingZeros = hideZeros.value();
            }
            if (const auto exponentThree = jsonBooleanField(*props, "display.format.exponent_multiple_of_3")) {
                widget.numericExponentMultipleOfThree = exponentThree.value();
            }
            if (const auto minimumWidth = jsonNumberField(*props, "display.format.minimum_field_width")) {
                widget.numericMinimumFieldWidth = std::clamp(static_cast<int>(std::lround(minimumWidth.value())), 0, 64);
            }
            if (const auto padding = jsonStringField(*props, "display.format.padding")) {
                widget.numericPadding = padding.value();
            }
            if (const auto alignment = jsonStringField(*props, "display.format.alignment")) {
                widget.numericAlignment = alignment.value();
            }
            if (const auto sign = jsonStringField(*props, "display.format.sign")) {
                widget.numericSignDisplay = sign.value();
            }
            if (const auto locale = jsonStringField(*props, "display.format.locale")) {
                widget.numericLocale = locale.value();
            }
            if (const auto prefix = jsonStringField(*props, "display.format.prefix")) {
                widget.numericDisplayPrefix = prefix.value();
            }
            if (const auto suffix = jsonStringField(*props, "display.format.suffix")) {
                widget.numericDisplaySuffix = suffix.value();
            }
            if (const auto showPrefix = jsonBooleanField(*props, "display.format.radix.show_prefix")) {
                widget.numericRadixShowPrefix = showPrefix.value();
            }
            if (const auto uppercase = jsonBooleanField(*props, "display.format.radix.uppercase")) {
                widget.numericRadixUppercase = uppercase.value();
            }
            if (const auto groupSize = jsonNumberField(*props, "display.format.radix.group_size")) {
                widget.numericRadixGroupSize = static_cast<int>(std::clamp(groupSize.value(), 0.0, 8.0));
            }
            if (const auto interpretation = jsonStringField(*props, "display.format.radix.interpretation")) {
                widget.numericRadixInterpretation = interpretation.value();
            }
            if (const auto custom = jsonStringField(*props, "display.format.custom_string")) {
                widget.numericCustomFormatString = custom.value();
            }
            if (isNumericWidget(widget)) {
                if (const auto radixVisible = jsonBooleanField(*props, "display.radix.visible")) {
                    static_cast<void>(setNumericRadixVisible(widget, radixVisible.value()));
                }
            }
        }
        if (const auto description = jsonStringField(*props, "documentation.description")) {
            widget.contextHelpDescription = description.value();
        } else if (const auto summary = jsonStringField(*props, "documentation.summary")) {
            widget.contextHelpDescription = summary.value();
        }
        if (const auto url = jsonStringField(*props, "documentation.url")) {
            widget.contextHelpUrl = url.value();
        }
        widget.contextTip = jsonStringField(*props, "documentation.tip");
        widget.defaultData = jsonStringField(*props, "data.default");
        const bool interactionEnabled = jsonBooleanField(*props, "interaction.enabled").value_or(true);
        if (!interactionEnabled) {
            const double disabledOpacity = jsonNumberField(*props, "style.disabled.opacity").value_or(1.0);
            static_cast<void>(setWidgetEnabledState(
                widget,
                disabledOpacity < 0.999
                    ? WidgetEnabledState::DisabledAndGrayed
                    : WidgetEnabledState::Disabled));
        }
        if (isLabelWidget(widget)) {
            widget.commentAutoSize = jsonBooleanField(*props, "comment_auto_size").value_or(false);
            if (const auto textValue = jsonStringField(*props, "text.value")) {
                static_cast<void>(setWidgetSupportText(widget, textValue.value()));
            }
            if (const auto textVisible = jsonBooleanField(*props, "text.visible")) {
                static_cast<void>(setWidgetLabelVisible(widget, textVisible.value()));
            }
            if (const auto horizontalAlignment = jsonStringField(*props, "text.horizontal_alignment")) {
                static_cast<void>(setWidgetLabelHorizontalAlignment(widget, horizontalAlignment.value()));
            }
            if (const auto verticalAlignment = jsonStringField(*props, "text.vertical_alignment")) {
                static_cast<void>(setWidgetLabelVerticalAlignment(widget, verticalAlignment.value()));
            }
            if (const auto textColor = jsonStringField(*props, "style.text.color")) {
                if (const auto rgb = parseRgbHex(textColor.value())) {
                    static_cast<void>(setWidgetLabelTextColor(widget, rgb.value()));
                }
            }
            if (const auto fillColor = jsonStringField(*props, "style.surface.fill_color")) {
                if (const auto rgb = parseRgbHex(fillColor.value())) {
                    static_cast<void>(setWidgetLabelSurfaceFillColor(widget, rgb.value()));
                }
            }
            if (const auto strokeColor = jsonStringField(*props, "style.surface.stroke_color")) {
                if (const auto rgb = parseRgbHex(strokeColor.value())) {
                    static_cast<void>(setWidgetLabelSurfaceStrokeColor(widget, rgb.value()));
                }
            }
            if (const auto fontFamily = jsonStringField(*props, "style.text.font_family")) {
                static_cast<void>(setWidgetLabelFontFamily(widget, firstCssFontFamily(fontFamily.value())));
            }
            if (const auto fontSize = jsonStringField(*props, "style.text.font_size")) {
                if (const auto pixels = parseCssPixelValue(fontSize.value())) {
                    static_cast<void>(setWidgetLabelFontScale(widget, pixels.value() / 15.0));
                }
            }
            const auto textStyle = textStyleFromCssFields(
                jsonStringField(*props, "style.text.font_weight").value_or({}),
                jsonStringField(*props, "style.text.font_style").value_or({}),
                jsonStringField(*props, "style.text.text_decoration").value_or({}));
            static_cast<void>(setWidgetLabelTextStyle(widget, textStyle));
        }
        if (isStringWidget(widget)) {
            widget.stringDropdown = jsonBooleanField(*props, "display.string.dropdown").value_or(false);
            if (const auto textValue = jsonStringField(*props, "text.value")) {
                static_cast<void>(setWidgetStringValue(widget, textValue.value()));
            } else if (const auto legacyValue = jsonStringField(*props, "value")) {
                static_cast<void>(setWidgetStringValue(widget, legacyValue.value()));
            }
            if (const auto textColor = jsonStringField(*props, "style.text.color")) {
                if (const auto rgb = parseRgbHex(textColor.value())) {
                    static_cast<void>(setWidgetStringValueTextColor(widget, rgb.value()));
                }
            }
            if (const auto fontFamily = jsonStringField(*props, "style.text.font_family")) {
                static_cast<void>(setWidgetStringValueFontFamily(widget, firstCssFontFamily(fontFamily.value())));
            }
            if (const auto fontSize = jsonStringField(*props, "style.text.font_size")) {
                if (const auto pixels = parseCssPixelValue(fontSize.value())) {
                    static_cast<void>(setWidgetStringValueFontScale(widget, pixels.value() / 15.0));
                }
            }
            const auto textStyle = textStyleFromCssFields(
                jsonStringField(*props, "style.text.font_weight").value_or({}),
                jsonStringField(*props, "style.text.font_style").value_or({}),
                jsonStringField(*props, "style.text.text_decoration").value_or({}));
            static_cast<void>(setWidgetStringValueTextStyle(widget, textStyle));
            if (const auto style = jsonStringField(*props, "display.string.style")) {
                static_cast<void>(setStringDisplayStyle(widget, style.value()));
            }
            if (const auto alignment = jsonStringField(*props, "text.horizontal_alignment")) {
                static_cast<void>(setWidgetStringTextHorizontalAlignment(widget, alignment.value()));
            }
            if (const auto visible = jsonBooleanField(*props, "display.string.style_selector_visible")) {
                static_cast<void>(setStringDisplayStyleVisible(widget, visible.value()));
            }
            if (const auto enabled = jsonBooleanField(*props, "interaction.text.single_line")) {
                static_cast<void>(setStringLimitToSingleLine(widget, enabled.value()));
            }
            if (const auto enabled = jsonBooleanField(*props, "interaction.text.wrap_at_word_breaks")) {
                static_cast<void>(setStringWrapAtWordBreaks(widget, enabled.value()));
            }
            if (const auto enabled = jsonBooleanField(*props, "interaction.text.update_while_typing")) {
                static_cast<void>(setStringUpdateWhileTyping(widget, enabled.value()));
            }
            if (const auto visible = jsonBooleanField(*props, "display.vertical_scrollbar_visible")) {
                static_cast<void>(setStringVerticalScrollbarVisible(widget, visible.value()));
            }
            if (const auto visible = jsonBooleanField(*props, "display.horizontal_scrollbar_visible")) {
                static_cast<void>(setStringHorizontalScrollbarVisible(widget, visible.value()));
            }
            if (const auto enabled = jsonBooleanField(*props, "layout.size_to_text")) {
                static_cast<void>(setStringSizeToText(widget, enabled.value()));
            }
        }
        if (isPathWidget(widget)) {
            if (const auto textValue = jsonStringField(*props, "text.value")) {
                static_cast<void>(setWidgetPathValue(widget, textValue.value()));
            } else if (const auto legacyValue = jsonStringField(*props, "value")) {
                static_cast<void>(setWidgetPathValue(widget, legacyValue.value()));
            }
            if (const auto textColor = jsonStringField(*props, "style.text.color")) {
                if (const auto rgb = parseRgbHex(textColor.value())) {
                    static_cast<void>(setWidgetStringValueTextColor(widget, rgb.value()));
                }
            }
            if (const auto fontFamily = jsonStringField(*props, "style.text.font_family")) {
                static_cast<void>(setWidgetStringValueFontFamily(widget, firstCssFontFamily(fontFamily.value())));
            }
            if (const auto fontSize = jsonStringField(*props, "style.text.font_size")) {
                if (const auto pixels = parseCssPixelValue(fontSize.value())) {
                    static_cast<void>(setWidgetStringValueFontScale(widget, pixels.value() / 15.0));
                }
            }
            const auto textStyle = textStyleFromCssFields(
                jsonStringField(*props, "style.text.font_weight").value_or({}),
                jsonStringField(*props, "style.text.font_style").value_or({}),
                jsonStringField(*props, "style.text.text_decoration").value_or({}));
            static_cast<void>(setWidgetStringValueTextStyle(widget, textStyle));
            if (const auto visible = jsonBooleanField(*props, "display.path.scrollbar_visible")) {
                static_cast<void>(setPathScrollbarVisible(widget, visible.value()));
            }
            if (const auto overflow = jsonStringField(*props, "display.path.overflow")) {
                static_cast<void>(setPathOverflowMode(widget, overflow.value()));
            }
            if (const auto enabled = jsonBooleanField(*props, "layout.size_to_text")) {
                static_cast<void>(setPathSizeToText(widget, enabled.value()));
            }
            if (const auto prompt = jsonStringField(*props, "interaction.path.prompt")) {
                static_cast<void>(setPathBrowsePrompt(widget, prompt.value()));
            }
            if (const auto label = jsonStringField(*props, "interaction.path.pattern_label")) {
                static_cast<void>(setPathBrowsePatternLabel(widget, label.value()));
            }
            if (const auto pattern = jsonStringField(*props, "interaction.path.pattern")) {
                static_cast<void>(setPathBrowsePattern(widget, pattern.value()));
            }
            if (const auto mode = jsonStringField(*props, "interaction.path.selection_mode")) {
                static_cast<void>(setPathBrowseSelectionMode(widget, mode.value()));
            }
            if (const auto mode = jsonStringField(*props, "interaction.path.existence_mode")) {
                static_cast<void>(setPathBrowseExistenceMode(widget, mode.value()));
            }
            if (const auto path = jsonStringField(*props, "interaction.path.start_path")) {
                static_cast<void>(setPathBrowseStartPath(widget, path.value()));
            }
        }
        if (isRingWidget(widget) || isEnumWidget(widget) || isStringDropdownWidget(widget)) {
            if (isEnumWidget(widget)) {
                if (const auto representation = jsonStringField(*props, "representation.kind")) {
                    static_cast<void>(setEnumRepresentationKind(widget, representation.value()));
                } else if (const auto legacyRepresentation = jsonStringField(*props, "data_type.representation")) {
                    static_cast<void>(setEnumRepresentationKind(widget, legacyRepresentation.value()));
                } else if (const auto namedSize = jsonStringField(*props, "data_type.named_numeric_size")) {
                    static_cast<void>(setEnumRepresentationKind(widget, namedSize.value()));
                }
            } else if (isRingWidget(widget)) {
                if (const auto representation = jsonStringField(*props, "representation.kind")) {
                    static_cast<void>(setRingRepresentationKind(widget, representation.value()));
                } else if (const auto legacyRepresentation = jsonStringField(*props, "data_type.representation")) {
                    static_cast<void>(setRingRepresentationKind(widget, legacyRepresentation.value()));
                } else if (const auto namedSize = jsonStringField(*props, "data_type.named_numeric_size")) {
                    static_cast<void>(setRingRepresentationKind(widget, namedSize.value()));
                }
            }
            if (const auto domainId = jsonStringField(*props, "enum.domain_id")) {
                widget.enumDomainId = domainId.value();
            }
            if (const auto allowUndefined = jsonBooleanField(*props, "data_type.allow_undefined_values")) {
                static_cast<void>(setRingAllowUndefinedValues(widget, allowUndefined.value()));
            } else if (const auto legacyAllowUndefined = jsonBooleanField(*props, "allow_undefined_values")) {
                static_cast<void>(setRingAllowUndefinedValues(widget, legacyAllowUndefined.value()));
            }
            auto selectedItemText = jsonStringField(*props, "value");
            if (!selectedItemText.has_value()) {
                selectedItemText = jsonStringField(*props, "value.selected_id");
            }
            std::optional<std::uint32_t> selectedItemValueFromText;
            if (const auto *items = objectField(*props, "items");
                items != nullptr && items->type == JsonValue::Type::Array) {
                std::vector<RingItem> parsedItems;
                parsedItems.reserve(items->array.size());
                for (std::size_t index = 0; index < items->array.size(); ++index) {
                    const auto &item = items->array[index];
                    if (item.type == JsonValue::Type::String) {
                        const auto itemValue = static_cast<std::uint32_t>(index);
                        if (selectedItemText.has_value() && selectedItemText.value() == item.string) {
                            selectedItemValueFromText = itemValue;
                        }
                        parsedItems.push_back(RingItem{
                            item.string,
                            itemValue,
                            false,
                            "item_" + std::to_string(index),
                            true });
                        continue;
                    }
                    if (item.type != JsonValue::Type::Object) {
                        continue;
                    }
                    RingItem parsed;
                    const auto itemId = jsonStringField(item, "id");
                    const auto itemText = jsonStringField(item, "text");
                    parsed.id = itemId.value_or("item_" + std::to_string(index));
                    parsed.label = jsonStringField(item, "label").value_or(
                        itemText.value_or("Item " + std::to_string(index)));
                    parsed.value = jsonUnsignedField(item, "value").value_or(
                        jsonUnsignedField(item, "numeric_value").value_or(static_cast<std::uint32_t>(index)));
                    if (const auto enabled = jsonBooleanField(item, "enabled")) {
                        parsed.disabled = !enabled.value();
                    } else {
                        parsed.disabled = jsonBooleanField(item, "disabled").value_or(false);
                    }
                    parsed.visible = jsonBooleanField(item, "visible").value_or(true);
                    if (isTabWidget(widget)) {
                        parsed.captionMatchesLabel = jsonBooleanField(item, "caption_matches_label").value_or(true);
                        parsed.caption = jsonStringField(item, "caption").value_or(parsed.label);
                    }
                    if (selectedItemText.has_value()) {
                        const auto &selected = selectedItemText.value();
                        if ((itemId.has_value() && selected == itemId.value()) ||
                            (!isStringDropdownWidget(widget) &&
                             ((itemText.has_value() && selected == itemText.value()) || selected == parsed.label))) {
                            selectedItemValueFromText = parsed.value;
                        }
                    }
                    parsedItems.push_back(std::move(parsed));
                }
                if (isTabWidget(widget)) {
                    // Page identity is authored containment. Never repair a
                    // duplicate ID into a different page behind its children.
                    widget.ringItems = std::move(parsedItems);
                } else if (isStringDropdownWidget(widget)) {
                    widget.ringItems = std::move(parsedItems);
                    widget.numericValue = selectedItemValueFromText.value_or(
                        jsonUnsignedField(*props, "value.selected_value").value_or(0));
                } else {
                    static_cast<void>(setRingItems(widget, std::move(parsedItems)));
                }
                if (!isStringDropdownWidget(widget) && selectedItemValueFromText.has_value()) {
                    static_cast<void>(setRingSelectedValue(widget, selectedItemValueFromText.value()));
                }
            }
        }
        if (const auto labelVisible = jsonBooleanField(*props, "label.visible")) {
            // Loading preserves explicitly stored defaults. Edit commands may
            // omit them, but a read must not erase authored presentation data.
            widget.labelVisible = labelVisible.value();
        }
        if (jsonBooleanField(*props, "caption.independent").value_or(false)) {
            widget.caption = jsonStringField(*props, "caption.text");
            widget.captionVisible = jsonBooleanField(*props, "caption.visible");
        } else {
            // Preserve pre-Caption documents and realization defaults: their
            // caption.* properties were the technical Label, not a UI caption.
            if (const auto text = jsonStringField(*props, "caption.text")) widget.label = *text;
            if (const auto visible = jsonBooleanField(*props, "caption.visible")) {
                static_cast<void>(setWidgetLabelVisible(widget, *visible));
            }
        }
        if (const auto stateFalseText = jsonStringField(*props, "state_text.false_text")) {
            widget.buttonStateFalseText = stateFalseText.value();
        }
        if (const auto stateTrueText = jsonStringField(*props, "state_text.true_text")) {
            widget.buttonStateTrueText = stateTrueText.value();
        }
        if (const auto stateTextVisible = jsonBooleanField(*props, "state_text.visible")) {
            widget.buttonStateTextVisible = stateTextVisible.value();
        }
        if (const auto stateTextColor = jsonStringField(*props, "state_text.color")) {
            if (const auto rgb = parseRgbHex(stateTextColor.value())) {
                widget.stateTextRgb = rgb.value();
            }
        }
        if (const auto falseFill = jsonStringField(*props, "state_colors.false_fill")) {
            if (const auto rgb = parseRgbHex(falseFill.value())) {
                widget.stateFalseFillRgb = rgb.value();
            }
        }
        if (const auto trueFill = jsonStringField(*props, "state_colors.true_fill")) {
            if (const auto rgb = parseRgbHex(trueFill.value())) {
                widget.stateTrueFillRgb = rgb.value();
            }
        }
        if (const auto falseStroke = jsonStringField(*props, "state_colors.false_stroke")) {
            if (const auto rgb = parseRgbHex(falseStroke.value())) {
                widget.stateFalseStrokeRgb = rgb.value();
            }
        }
        if (const auto trueStroke = jsonStringField(*props, "state_colors.true_stroke")) {
            if (const auto rgb = parseRgbHex(trueStroke.value())) {
                widget.stateTrueStrokeRgb = rgb.value();
            }
        }
        if (const auto stateTextLockInCenter = jsonBooleanField(*props, "state_text.lock_in_center")) {
            static_cast<void>(setButtonStateTextLockedInCenter(widget, stateTextLockInCenter.value()));
        }
        if (const auto mechanicalAction = jsonStringField(*props, "behavior.mechanical_action")) {
            static_cast<void>(setButtonMechanicalAction(widget, mechanicalAction.value()));
        }
        auto incrementDecrementVisible = jsonBooleanField(*props, "increment_decrement.visible");
        if (!incrementDecrementVisible.has_value()) {
            incrementDecrementVisible = jsonBooleanField(*props, "display.increment_decrement_visible");
        }
        if (!incrementDecrementVisible.has_value()) {
            incrementDecrementVisible = jsonBooleanField(*props, "display.increment_buttons_visible");
        }
        if (incrementDecrementVisible.has_value()) {
            widget.numericIncrementDecrementVisible = incrementDecrementVisible.value();
        }
        auto incrementDecrementSide = jsonStringField(*props, "increment_decrement.side");
        if (!incrementDecrementSide.has_value()) {
            incrementDecrementSide = jsonStringField(*props, "display.increment_decrement_side");
        }
        if (!incrementDecrementSide.has_value()) {
            incrementDecrementSide = jsonStringField(*props, "display.increment_buttons_side");
        }
        if (incrementDecrementSide.has_value()) {
            if (incrementDecrementSide.value() == "left") {
                widget.numericIncrementDecrementSide = NumericIncrementDecrementSide::Left;
            } else if (incrementDecrementSide.value() == "right") {
                widget.numericIncrementDecrementSide = NumericIncrementDecrementSide::Right;
            }
        }
        if (const auto folderVisible = jsonBooleanField(*props, "folder.visible")) {
            if (isPathWidget(widget)) widget.pathFolderVisible = folderVisible.value();
        }
        if (const auto buttonVisible = jsonBooleanField(*props, "button.visible")) {
            if (isPathWidget(widget)) widget.pathBrowseButtonVisible = buttonVisible.value();
        } else if (const auto browseButtonVisible = jsonBooleanField(*props, "browse_button.visible")) {
            if (isPathWidget(widget)) widget.pathBrowseButtonVisible = browseButtonVisible.value();
        }
        if (const auto fillColor = jsonStringField(*props, "style.body.fill_color")) {
            if (const auto rgb = parseRgbHex(fillColor.value())) {
                static_cast<void>(setWidgetNumericBodyFillColor(widget, rgb.value()));
                static_cast<void>(setWidgetStringBodyFillColor(widget, rgb.value()));
            }
        }
        if (widget.valueType == "variant" || isPictureWidget(widget) || isWaveformGraphWidget(widget)) {
            if (const auto fillColor = jsonStringField(*props, "style.surface.fill_color"))
                widget.surfaceFillRgb = parseRgbHex(fillColor.value());
            if (const auto strokeColor = jsonStringField(*props, "style.surface.stroke_color"))
                widget.surfaceStrokeRgb = parseRgbHex(strokeColor.value());
        }
        if (!isLabelWidget(widget)) {
            if (const auto fillColor = jsonStringField(*props, "style.label.fill_color"))
                static_cast<void>(setWidgetLabelSurfaceFillColor(widget, parseRgbHex(fillColor.value())));
            if (const auto strokeColor = jsonStringField(*props, "style.label.stroke_color"))
                static_cast<void>(setWidgetLabelSurfaceStrokeColor(widget, parseRgbHex(strokeColor.value())));
        }
        if (const auto strokeColor = jsonStringField(*props, "style.body.stroke_color")) {
            if (const auto rgb = parseRgbHex(strokeColor.value())) {
                static_cast<void>(setWidgetNumericBodyStrokeColor(widget, rgb.value()));
                static_cast<void>(setWidgetStringBodyStrokeColor(widget, rgb.value()));
            }
        }
        if (const auto buttonFill = jsonStringField(*props, "style.increment_decrement.button_fill_color")) {
            if (const auto rgb = parseRgbHex(buttonFill.value())) {
                static_cast<void>(setWidgetNumericStepButtonFillColor(widget, rgb.value()));
            }
        }
        if (const auto buttonStroke = jsonStringField(*props, "style.increment_decrement.button_stroke_color")) {
            if (const auto rgb = parseRgbHex(buttonStroke.value())) {
                static_cast<void>(setWidgetNumericStepButtonStrokeColor(widget, rgb.value()));
            }
        }
        if (const auto arrowFill = jsonStringField(*props, "style.increment_decrement.arrow_fill_color")) {
            if (const auto rgb = parseRgbHex(arrowFill.value())) {
                static_cast<void>(setWidgetNumericStepArrowFillColor(widget, rgb.value()));
            }
        }
        if (const auto arrowStroke = jsonStringField(*props, "style.increment_decrement.arrow_stroke_color")) {
            if (const auto rgb = parseRgbHex(arrowStroke.value())) {
                static_cast<void>(setWidgetNumericStepArrowStrokeColor(widget, rgb.value()));
            }
        }
        if (const auto selectedValue = jsonUnsignedField(*props, "value.selected_value")) {
            if (isRingWidget(widget) || isEnumWidget(widget)) {
                static_cast<void>(setRingSelectedValue(widget, selectedValue.value()));
            }
        } else if (const auto booleanValue = jsonBooleanField(*props, "value")) {
            static_cast<void>(setWidgetBooleanValue(widget, booleanValue.value()));
        } else if ((isRingWidget(widget) || isEnumWidget(widget)) &&
                   jsonUnsignedField(*props, "value").has_value()) {
            static_cast<void>(setRingSelectedValue(
                widget,
                jsonUnsignedField(*props, "value").value()));
        } else if (const auto numericValue = jsonNumberField(*props, "value")) {
            const auto *token=objectField(*props,"value");
            if(setWidgetNumericValue(widget,numericValue.value()) && token &&
                document_internal::canonicalWidgetWideIntegerLexeme(token->string,widget.valueType))widget.initialValue=token->string;
        }
    }

    if (const auto *visual = objectField(value, "visual")) {
        overrideStringIfPresent(widget.assetRef, *visual, "asset_ref");
    }

    if (const auto *binding = objectField(value, "binding")) {
        if (const auto publicInput = jsonStringField(*binding, "public_input_id")) {
            widget.publicInputId = publicInput.value();
            widget.publicOutputId.reset();
        }
        if (const auto publicOutput = jsonStringField(*binding, "public_output_id")) {
            widget.publicOutputId = publicOutput.value();
            widget.publicInputId.reset();
        }
        if (const auto slotId = jsonStringField(*binding, "interface_map_slot")) {
            widget.interfaceMapSlotId = slotId.value();
        } else if (const auto legacySlotId = jsonStringField(*binding, "slot_id")) {
            widget.interfaceMapSlotId = legacySlotId.value();
        }
        if (const auto requirement = jsonStringField(*binding, "connection_requirement")) {
            widget.interfaceConnectionRequirement = normalizedInterfaceConnectionRequirement(requirement.value());
        } else if (const auto legacyRequirement = jsonStringField(*binding, "requirement")) {
            widget.interfaceConnectionRequirement = normalizedInterfaceConnectionRequirement(legacyRequirement.value());
        }
    }

    if (const auto *runtime = objectField(value, "runtime")) {
        overrideStringIfPresent(widget.sourceExamplePath, *runtime, "sourceExample");
        overrideStringIfPresent(widget.wfrogPackagePath, *runtime, "wfrog");
        overrideStringIfPresent(widget.realizationPackageRef, *runtime, "realization");
        overrideStringIfPresent(widget.assetRef, *runtime, "asset");
        if (const auto popup = jsonStringField(*runtime, "popupAsset")) {
            widget.popupAssetRef = popup.value();
        }
        if (const auto defaultSvg = jsonStringField(*runtime, "defaultSvg")) {
            widget.defaultSvgAssetPath = defaultSvg.value();
        }
    }

    if (const auto *state = objectField(value, "state")) {
        if (const auto minimum = jsonNumberField(*state, "minimum")) {
            widget.numericMinimum = minimum.value();
        }
        if (const auto maximum = jsonNumberField(*state, "maximum")) {
            widget.numericMaximum = maximum.value();
        }
        if (const auto selectedValue = jsonUnsignedField(*state, "value.selected_value")) {
            if (isRingWidget(widget) || isEnumWidget(widget)) {
                static_cast<void>(setRingSelectedValue(widget, selectedValue.value()));
            }
        } else if ((isRingWidget(widget) || isEnumWidget(widget)) &&
                   jsonUnsignedField(*state, "value").has_value()) {
            static_cast<void>(setRingSelectedValue(
                widget,
                jsonUnsignedField(*state, "value").value()));
        } else if (const auto numericValue = jsonNumberField(*state, "value")) {
            const auto *token=objectField(*state,"value");
            if(setWidgetNumericValue(widget,numericValue.value()) && token &&
                document_internal::canonicalWidgetWideIntegerLexeme(token->string,widget.valueType))widget.initialValue=token->string;
        } else if (const auto booleanValue = jsonBooleanField(*state, "value")) {
            static_cast<void>(setWidgetBooleanValue(widget, booleanValue.value()));
        } else if (const auto stringValue = jsonStringField(*state, "value")) {
            static_cast<void>(setWidgetStringValue(widget, stringValue.value()));
        }
        if(isNumericWidget(widget))if(const auto lexical=jsonStringField(*state,"initial_value")) {
            if (numericCanonicalValueTypeIsComplex(widget.valueType)) {
                const auto *present = objectField(*state, "numeric_value_present");
                if (!setWidgetNumericComplexValueLexeme(widget, *lexical)) {
                    widget.initialValue = *lexical;
                    widget.numericValue.reset();
                } else if (present && present->type == JsonValue::Type::Boolean && !present->boolean) {
                    widget.numericValue.reset();
                }
            } else if (widget.valueType == "f64" && *lexical == "+Infinity" &&
                !objectField(*state, "value")) {
                // Narrow semantic default written by Gamma P's Create Control.
                // It does not admit NaN, -Infinity, other spellings or non-f64
                // state, and does not pass through the finite Numeric editor.
                const auto *present = objectField(*state, "numeric_value_present");
                if (present && present->type == JsonValue::Type::Boolean && present->boolean) {
                    widget.initialValue = *lexical;
                    widget.numericValue = std::numeric_limits<double>::infinity();
                }
            } else {
            const auto projection=document_internal::widgetWideIntegerCacheProjection(*lexical,widget.valueType);
            const auto *present=objectField(*state,"numeric_value_present");
            const auto *cache=objectField(*state,"value");
            const bool flagValid=!present || present->type==JsonValue::Type::Boolean;
            const bool cacheMatches=!cache || (cache->type==JsonValue::Type::Number && projection && cache->number==*projection);
            if(projection && flagValid && cacheMatches && setWidgetNumericValue(widget,*projection)) {
                widget.initialValue=*lexical;
                if(present && !present->boolean)widget.numericValue.reset();
            }
            }
        }
    }

    if (const auto *geometry = objectField(value, "geometry")) {
        if (const auto *numericValue = objectField(*geometry, "numericValue")) {
            if (const auto offsetX = jsonNumberField(*numericValue, "offsetX")) {
                widget.numericValueOffsetX = offsetX.value();
            }
            if (const auto width = jsonNumberField(*numericValue, "width")) {
                widget.numericValueWidth = width.value();
            }
        }
        if (const auto *labelOffset = objectField(*geometry, "labelOffset")) {
            if (const auto x = jsonNumberField(*labelOffset, "x")) {
                widget.labelOffsetX = x.value();
            }
            if (const auto y = jsonNumberField(*labelOffset, "y")) {
                widget.labelOffsetY = y.value();
            }
        }
    }

    if (const auto *typography = objectField(value, "typography")) {
        widget.constantTextBoundsAutomatic = jsonBooleanField(*typography, "constantTextBoundsAutomatic");
        widget.constantArrayCellBoundsAutomatic = jsonBooleanField(*typography, "constantArrayCellBoundsAutomatic");
        if (jsonNumberField(*typography, "inheritanceVersion").value_or(0) >= 1) {
            // Public appearance properties are a compatibility projection.
            // Only the optional typography scales are explicit size choices.
            widget.labelFontScale.reset();
            widget.captionFontScale.reset();
            widget.numericValueFontScale.reset();
            widget.stringValueFontScale.reset();
            widget.stateTextFontScale.reset();
            // Private presentation context, not an explicit font override or
            // a program interface. Avoid reinterpreting already measured
            // authored frames merely because a transient flag was lost.
            if (const auto resolved = jsonNumberField(*typography, "resolvedPointSize");
                    resolved && std::isfinite(*resolved) && *resolved >= 6.0 && *resolved <= 144.0) {
                widget.inheritedTextPointSize = *resolved;
                widget.defaultTextGeometryInitialized = true;
            }
        }
        if (const auto scale = jsonNumberField(*typography, "captionScale")) {
            widget.captionFontScale = std::clamp(scale.value(), 0.4, 9.6);
        }
        if (const auto family = jsonStringField(*typography, "captionFontFamily")) {
            static_cast<void>(setWidgetCaptionFontFamily(widget, family.value()));
        }
        if (const auto scale = jsonNumberField(*typography, "labelScale")) {
            static_cast<void>(setWidgetLabelFontScale(widget, scale.value()));
        }
        if (const auto scale = jsonNumberField(*typography, "numericValueScale")) {
            static_cast<void>(setWidgetNumericValueFontScale(widget, scale.value()));
        }
        if (const auto scale = jsonNumberField(*typography, "stringValueScale")) {
            static_cast<void>(setWidgetStringValueFontScale(widget, scale.value()));
        }
        if (const auto scale = jsonNumberField(*typography, "stateTextScale")) {
            static_cast<void>(setWidgetStateTextFontScale(widget, scale.value()));
        }
        if (const auto family = jsonStringField(*typography, "labelFontFamily")) {
            static_cast<void>(setWidgetLabelFontFamily(widget, family.value()));
        }
        if (const auto family = jsonStringField(*typography, "numericValueFontFamily")) {
            static_cast<void>(setWidgetNumericValueFontFamily(widget, family.value()));
        }
        if (const auto family = jsonStringField(*typography, "stringValueFontFamily")) {
            static_cast<void>(setWidgetStringValueFontFamily(widget, family.value()));
        }
        if (const auto style = jsonStringField(*typography, "labelStyle")) {
            static_cast<void>(setWidgetLabelTextStyle(widget, style.value()));
        }
        if (const auto style = jsonStringField(*typography, "numericValueStyle")) {
            static_cast<void>(setWidgetNumericValueTextStyle(widget, style.value()));
        }
        if (const auto style = jsonStringField(*typography, "stringValueStyle")) {
            static_cast<void>(setWidgetStringValueTextStyle(widget, style.value()));
        }
        if (isBooleanWidget(widget) || isButtonWidget(widget)) {
            static_cast<void>(setWidgetStateTextFontFamily(widget,
                jsonStringField(*typography, "stateTextFontFamily").value_or(
                    widget.labelFontFamily.value_or(""))));
            static_cast<void>(setWidgetStateTextStyle(widget,
                jsonStringField(*typography, "stateTextStyle").value_or(
                    std::string(widgetLabelTextStyle(widget)))));
        }
        if (const auto alignment = jsonStringField(*typography, "choiceTextHorizontalAlignment")) {
            static_cast<void>(setWidgetChoiceTextHorizontalAlignment(widget, alignment.value()));
        }
        if (const auto alignment = jsonStringField(*typography, "stringTextHorizontalAlignment")) {
            static_cast<void>(setWidgetStringTextHorizontalAlignment(widget, alignment.value()));
        }
        if (const auto color = jsonStringField(*typography, "labelColor")) {
            if (const auto rgb = parseRgbHex(color.value())) {
                static_cast<void>(setWidgetLabelTextColor(widget, rgb.value()));
            }
        }
        if (const auto color = jsonStringField(*typography, "numericValueColor")) {
            if (const auto rgb = parseRgbHex(color.value())) {
                static_cast<void>(setWidgetNumericValueTextColor(widget, rgb.value()));
            }
        }
        if (const auto color = jsonStringField(*typography, "stringValueColor")) {
            if (const auto rgb = parseRgbHex(color.value())) {
                static_cast<void>(setWidgetStringValueTextColor(widget, rgb.value()));
            }
        }
    }

    const auto *storedTypography = objectField(value, "typography");
    if (!storedTypography ||
        jsonNumberField(*storedTypography, "inheritanceVersion").value_or(0) < 1) {
        // Legacy palette placement stamped 1.30 onto otherwise default fonts.
        // Preserve every other stored size; it may be an authored override.
        for (auto *scale : {&widget.labelFontScale, &widget.numericValueFontScale,
                &widget.stringValueFontScale}) {
            if (*scale && (std::abs(**scale - 1.30) < 0.001 ||
                    std::abs(**scale - 1.0) < 0.001)) scale->reset();
        }
        // Old palette Boolean state text used the separate 12 pt baseline.
        if (widget.stateTextFontScale && (std::abs(*widget.stateTextFontScale - 0.8) < 0.001 ||
                std::abs(*widget.stateTextFontScale - 1.0) < 0.001))
            widget.stateTextFontScale.reset();
    }
    if (!supportsPublicInterfaceBinding(widget)) {
        widget.publicInputId.reset();
        widget.publicOutputId.reset();
        widget.interfaceMapSlotId.reset();
        widget.interfaceConnectionRequirement.reset();
    }

    if (isBooleanWidget(widget)) {
        static_cast<void>(setBooleanWidgetIndicatorRole(
            widget,
            widget.role == "indicator"));
    }

    if (isTabWidget(widget)) {
        if (!widget.tabContainer) widget.tabContainer = TabContainerState{};
        if (const auto *state = objectField(value, "studio_tab")) {
            widget.tabContainer->editorPageId = jsonStringField(*state, "editor_page_id").value_or("");
            widget.tabContainer->headerFirstVisible = jsonUnsignedField(*state, "header_first_visible").value_or(0U);
            widget.tabContainer->tabsVisible = jsonBooleanField(*state, "tabs_visible").value_or(true);
            widget.tabContainer->pageLabelsDisplayVisible = jsonBooleanField(*state, "page_labels_display_visible").value_or(false);
            if (const auto serial = jsonStringField(*state, "next_page_serial")) {
                std::uint64_t parsed = 0;
                const auto conversion = std::from_chars(serial->data(), serial->data() + serial->size(), parsed);
                if (conversion.ec == std::errc{} && conversion.ptr == serial->data() + serial->size())
                    widget.tabContainer->nextPageSerial = parsed;
            }
        }
        if (!tabPageIndex(widget, widget.tabContainer->editorPageId) && !widget.ringItems.empty())
            widget.tabContainer->editorPageId = widget.ringItems.front().id;
        widget.tabContainer->headerFirstVisible = widget.ringItems.empty() ? 0U : (std::min)(
            widget.tabContainer->headerFirstVisible, static_cast<std::uint32_t>(widget.ringItems.size() - 1U));
    }

    return SceneObject{ id, bounds, std::move(widget), std::nullopt, locked, hidden };
}

[[nodiscard]] bool matchesHistoricalDraftRevisionLexeme(
    std::string_view text,
    std::uint32_t revision) noexcept {
    // The parser has already established JSON number grammar. Compare its
    // exact decimal value, because binary64 can round a fractional revision
    // such as 2.0000000000000001 to a supported integer.
    const bool negative = !text.empty() && text.front() == '-';
    if (negative) text.remove_prefix(1U);
    const auto exponentPosition = text.find_first_of("eE");
    const auto mantissa = text.substr(0U, exponentPosition);
    if (revision == 0U) return mantissa.find_first_not_of("0.") == std::string_view::npos;
    if (negative) return false;

    std::int64_t digitCount = 0;
    std::int64_t integerDigits = 0;
    std::int64_t significantPosition = -1;
    bool fractional = false;
    for (const char digit : mantissa) {
        if (digit == '.') {
            fractional = true;
            continue;
        }
        if (!fractional) ++integerDigits;
        if (digit != '0') {
            if (digit != static_cast<char>('0' + revision) || significantPosition != -1) return false;
            significantPosition = digitCount;
        }
        ++digitCount;
    }
    if (significantPosition == -1) return false;

    std::int64_t exponent = 0;
    if (exponentPosition != std::string_view::npos) {
        auto exponentText = text.substr(exponentPosition + 1U);
        const bool exponentNegative = exponentText.front() == '-';
        if (exponentNegative || exponentText.front() == '+') exponentText.remove_prefix(1U);
        const auto firstNonzero = exponentText.find_first_not_of('0');
        exponentText = firstNonzero == std::string_view::npos ? "0" : exponentText.substr(firstNonzero);
        const auto parsed = std::from_chars(
            exponentText.data(), exponentText.data() + exponentText.size(), exponent);
        if (parsed.ec != std::errc{} || parsed.ptr != exponentText.data() + exponentText.size()) return false;
        if (exponentNegative) exponent = -exponent;
    }
    return exponent == significantPosition - integerDigits + 1;
}

[[nodiscard]] bool isHistoricalByteStringDraft(const JsonValue &root) {
    const auto *format = root.member("format");
    if (format == nullptr || format->type != JsonValue::Type::String ||
        format->string != kFrogDraftDocumentFormat || root.member("spec_version") != nullptr) {
        return false;
    }

    const auto *revision = root.member("draft_revision");
    if (revision == nullptr) return true;
    if (revision->type != JsonValue::Type::Number) return false;
    const auto resolved = resolveFrogDraftRevision(
        { FrogDraftRevisionFieldKind::Number, revision->number });
    // Revisions 3/4 retain the private byte-string authoring profile.
    // Public source and unsupported future revisions stay strict.
    constexpr std::uint32_t lastByteStringRevision = kFrogExactIntegerStateDraftRevision;
    return resolved.has_value() && resolved->sourceRevision <= lastByteStringRevision &&
        matchesHistoricalDraftRevisionLexeme(revision->string, resolved->sourceRevision);
}

} // namespace

bool validFrogEditorWindowPlacement(const FrogEditorWindowPlacement &placement) noexcept {
    const auto &r = placement.bounds;
    // Reject malformed/unbounded presentation metadata before any OS conversion.
    return std::isfinite(r.x) && std::isfinite(r.y) &&
        std::isfinite(r.width) && std::isfinite(r.height) &&
        std::abs(r.x) <= 10000000.0 && std::abs(r.y) <= 10000000.0 &&
        r.width >= 1.0 && r.height >= 1.0 &&
        r.width <= 100000.0 && r.height <= 100000.0;
}

namespace {
bool exactUnsignedJsonArray(const JsonValue &owner,std::string_view key,
    std::vector<std::uint32_t> &result) {
    const auto *values=objectField(owner,key);
    if(!values || values->type!=JsonValue::Type::Array)return false;
    result.clear();result.reserve(values->array.size());
    for(const auto &value:values->array) {
        if(value.type!=JsonValue::Type::Number || !std::isfinite(value.number) ||
            value.number<0 || value.number>(std::numeric_limits<std::uint32_t>::max)() ||
            value.number!=std::trunc(value.number))return false;
        result.push_back(static_cast<std::uint32_t>(value.number));
    }
    return true;
}
bool arrayCellEncodedTypeMatchesPrototype(const binding::TypeRef &encoded,
    const binding::TypeRef &prototype) {
    // The private compact profile writes canonical(), which encodes the
    // recursive shape, ordered field IDs, Enum domain and representation.
    // It does not encode choice definitions/IDs or display field names. An
    // unchanged marker inherits those facts from its actual prototype; it
    // cannot provide an independent definition. Explicit scalar values still
    // pass the complete TypeRef equality check in applyArrayCellValueJson.
    std::vector<std::pair<const binding::TypeRef *,const binding::TypeRef *>> pending{
        {&encoded,&prototype}};
    while(!pending.empty()) {
        const auto [left,right]=pending.back();pending.pop_back();
        if(left->kind!=right->kind || left->name!=right->name ||
            left->rank!=right->rank || left->extent!=right->extent ||
            left->fieldIds!=right->fieldIds || left->children.size()!=right->children.size())return false;
        for(std::size_t index=0;index<left->children.size();++index)
            pending.push_back({&left->children[index],&right->children[index]});
    }
    return true;
}
bool validateStoredArrayCellValues(WidgetInstance &root);
bool applyArrayCellValueJson(WidgetInstance &destination,const JsonValue &root,std::string_view source,
    bool apply=true,bool migrate=false,bool *compactValueOnly=nullptr) {
    // The decoded value can change values, never its prototype's type tree.
    // Resolve that tree once, then carry its stable subtypes through this call.
    // Re-resolving every descendant rescans all deeper prototypes at each Array
    // boundary and makes deeply mixed Array/Cluster snapshots grow cubically.
    const auto schema=binding::widgetType(destination);
    if(!schema.valid())return false;
    struct Frame {WidgetInstance *destination;const JsonValue *value;
        const binding::TypeRef *type;bool apply;};
    std::vector<Frame> pending{{&destination,&root,&schema.type,apply}};
    std::unordered_map<const binding::TypeRef *,std::string> canonicalTypes;
    std::size_t validatedRootMarkerBytes=0;
    while(!pending.empty()) {
        const auto frame=pending.back();pending.pop_back();
        auto &widget=*frame.destination;const auto &payload=*frame.value;
        if(payload.type!=JsonValue::Type::Object)return false;
        if(payload.member("format") && !document_internal::privateWidgetValueObject(payload))return false;
        if(!objectField(payload,"array_cell_value")) {
            if(compactValueOnly)*compactValueOnly=false;
            auto legacy=sceneObjectFromJson(payload,1,source).widget;
            if(!legacy || *frame.type!=binding::widgetType(*legacy).type ||
                !validateStoredArrayCellValues(*legacy))return false;
            if(frame.apply)applyArrayElementWidgetValue(widget,*legacy);
            continue;
        }
        const auto *version=objectField(payload,"array_cell_value");
        if(version->type!=JsonValue::Type::Number || version->number!=1.0)return false;
        const auto marker=jsonStringField(payload,"valueType").value_or("");
        bool exactValidatedMarker=false;
        if(validatedRootMarkerBytes && marker.size()<=validatedRootMarkerBytes) {
            auto [at,inserted]=canonicalTypes.try_emplace(frame.type);
            if(inserted)at->second=frame.type->canonical();
            exactValidatedMarker=marker==at->second;
        }
        if(!exactValidatedMarker) {
            const auto savedType=binding::parseType(marker);
            if(!savedType.valid())return false;
            const bool matches=arrayCellEncodedTypeMatchesPrototype(savedType.type,*frame.type);
            if(!matches && (!migrate || savedType.type.kind!=frame.type->kind ||
                (savedType.type.kind==binding::TypeKind::Primitive &&
                    (!savedType.type.numeric() || !frame.type->numeric()))))return false;
            // The root always uses the real parser and full structural check.
            // Its accepted spelling bounds this fast path without copying a
            // parser limit or accepting a longer expression. Only canonical
            // descendants of that proved tree qualify; aliases/whitespace or
            // changed migration types still pass through the ordinary parser.
            if(frame.value==&root && matches)validatedRootMarkerBytes=marker.size();
        }
        const auto *value=objectField(payload,"value");
        if(!value)continue; // The actual prototype supplies this unchanged value.
        if(value->type!=JsonValue::Type::Object)return false;
        if(auto *cluster=clusterContainerState(widget)) {
            const auto *fields=objectField(*value,"fields");
            if(!fields || fields->type!=JsonValue::Type::Array || (!migrate && fields->array.size()!=cluster->elements.size()))return false;
            std::unordered_set<std::string> seen;
            for(const auto &field:fields->array) {
                const auto id=jsonStringField(field,"field_id");const auto *data=objectField(field,"value");
                if(!id || id->empty() || !data || !seen.insert(*id).second)return false;
                const auto found=std::find_if(cluster->elements.begin(),cluster->elements.end(),
                    [&](const auto &candidate){return candidate.fieldId==*id;});
                if(found==cluster->elements.end()){if(migrate)continue;return false;}
                if(!found->widget)return false;
                const auto typeField=std::find(frame.type->fieldIds.begin(),frame.type->fieldIds.end(),*id);
                if(typeField==frame.type->fieldIds.end())return false;
                const auto typeIndex=static_cast<std::size_t>(typeField-frame.type->fieldIds.begin());
                if(typeIndex>=frame.type->children.size())return false;
                pending.push_back({found->widget.get(),data,&frame.type->children[typeIndex],frame.apply});
            }
        } else if(auto *array=arrayContainerState(widget)) {
            if(frame.type->children.size()!=1)return false;
            const auto &elementType=frame.type->children.front();
            std::vector<std::uint32_t> shape;
            const auto *elements=objectField(*value,"elements");
            if(!exactUnsignedJsonArray(*value,"shape",shape) || shape.size()!=array->rank ||
                !checkedArrayElementCount(shape) ||
                !elements || elements->type!=JsonValue::Type::Array ||
                (!elements->array.empty() && !array->elementTemplate))return false;
            std::vector<ArrayElementValue> cells;cells.reserve(elements->array.size());
            std::set<std::vector<std::uint32_t>> seen;
            for(const auto &entry:elements->array) {
                std::vector<std::uint32_t> indices;
                if(!exactUnsignedJsonArray(entry,"indices",indices) || indices.size()!=array->rank)return false;
                for(const auto flag:{"defined","enabled"})
                    if(const auto *raw=objectField(entry,flag);raw && raw->type!=JsonValue::Type::Boolean)return false;
                const auto kind=jsonStringField(entry,"kind");
                if(!kind || (*kind!="widget" && *kind!="number" && *kind!="boolean" && *kind!="string"))return false;
                auto cell=jsonArrayElementValue(entry,source);
                if(!cell || cell->indices.size()!=array->rank || !seen.insert(cell->indices).second)return false;
                const auto decodedKind=cell->kind==ArrayElementValueKind::Widget?"widget":
                    cell->kind==ArrayElementValueKind::Number?"number":
                    cell->kind==ArrayElementValueKind::Boolean?"boolean":"string";
                if(*kind!=decodedKind)return false;
                for(std::size_t axis=0;axis<shape.size();++axis)
                    if(cell->defined && cell->indices[axis]>=shape[axis])return false;
                if(cell->kind==ArrayElementValueKind::Widget) {
                    const auto *data=objectField(entry,"value");
                    if(!array->elementTemplate || !data)return false;
                    if(data->type==JsonValue::Type::String) {
                        if(compactValueOnly)*compactValueOnly=false;
                        // A historical full-widget cell can retain whitespace
                        // outside its object as exact stored bytes. Validate
                        // that compatible representation without rewriting it.
                        const auto legacy=deserializeArrayElementWidget(cell->displayValue,array->elementTemplate.get());
                        if(!legacy || binding::widgetType(*legacy).type!=elementType)return false;
                    } else {
                        if(data->type!=JsonValue::Type::Object)return false;
                        pending.push_back({array->elementTemplate.get(),data,&elementType,false});
                    }
                } else if(array->elementTemplate) {
                    const auto &element=*array->elementTemplate;
                    const bool compatible=cell->kind==ArrayElementValueKind::Number
                        ? isNumericWidget(element) || isRingWidget(element) || isEnumWidget(element)
                        : cell->kind==ArrayElementValueKind::Boolean
                            ? isBooleanWidget(element) || isButtonWidget(element)
                            : isStringWidget(element) || isPathWidget(element) || isEnumWidget(element);
                    if(!compatible)return false;
                    if (cell->kind == ArrayElementValueKind::Number &&
                        numericCanonicalValueTypeIsComplex(element.valueType)) {
                        auto validation = element;
                        if (!setWidgetNumericComplexValueLexeme(validation, cell->displayValue)) return false;
                    }
                }
                cells.push_back(std::move(*cell));
            }
            if(frame.apply){array->shape=shape;array->elements=std::move(cells);}
            if(const auto *defaults=objectField(*value,"element_default")) {
                if(!array->elementTemplate)return false;
                pending.push_back({array->elementTemplate.get(),defaults,&elementType,frame.apply});
            }
        } else {
            const auto *state=objectField(*value,"state"),*props=objectField(*value,"props");
            if(isWaveformGraphWidget(widget)) {
                // The compact profile always authors an explicit dataset.
                // Legacy full-widget loading may tolerate missing properties;
                // a new cell must not silently replace damaged data by defaults.
                const auto *graphValue=props && props->type==JsonValue::Type::Object
                    ? objectField(*props,"waveform_graph"):nullptr;
                const auto *data=graphValue && graphValue->type==JsonValue::Type::Object
                    ? objectField(*graphValue,"initial_data"):nullptr;
                if(!data || data->type!=JsonValue::Type::Array)return false;
            }
            auto scalar=sceneObjectFromJson(*value,1,source).widget;
            if(!scalar)return false;
            // The scalar envelope is independently authored data. A valid
            // outer value marker must not conceal an invalid inner lexeme.
            if (numericCanonicalValueTypeIsComplex(scalar->valueType) &&
                !widgetNumericComplexValueLexeme(*scalar)) return false;
            if(isWaveformGraphWidget(*scalar) && (!scalar->waveformGraph ||
                scalar->waveformGraph->preservedSource ||
                !graph::validate(scalar->waveformGraph->initialData).empty()))return false;
            if(*frame.type!=binding::widgetType(*scalar).type) {
                if(!migrate || !isNumericWidget(widget) || !isNumericWidget(*scalar))return false;
                if(frame.apply)static_cast<void>(setWidgetNumericValue(widget,widgetNumericValue(*scalar)));
                continue;
            }
            // Cell numeric text is authoritative. The ordinary widget loader
            // may project a JSON Number through binary64 for its editor value.
            const auto *present=objectField(payload,"numeric_value_present");
            if(present && present->type!=JsonValue::Type::Boolean)return false;
            const auto *number=state?objectField(*state,"value"):nullptr;
            if(!number && props)number=objectField(*props,"value");
            if(isNumericWidget(*scalar) && number && number->type==JsonValue::Type::Number && !number->string.empty()) {
                scalar->initialValue=number->string;
                if(binding::widgetType(*scalar).type.integer() && (!present || !present->boolean))scalar->numericValue.reset();
            }
            if(const auto *initial=objectField(payload,"initial_value")) {
                if(initial->type==JsonValue::Type::String)scalar->initialValue=initial->string;
                else if(initial->type==JsonValue::Type::Null)scalar->initialValue.reset();
                else return false;
            }
            if(present && !present->boolean)scalar->numericValue.reset();
            if (numericCanonicalValueTypeIsComplex(scalar->valueType) &&
                !widgetNumericComplexValueLexeme(*scalar)) return false;
            if(frame.apply)applyArrayElementWidgetValue(widget,*scalar);
        }
    }
    return true;
}

// Validate only the new value profile. Historical full-widget/string cells
// retain their established reader. Walking the real prototypes avoids making
// a detached copy of an entire aggregate for every stored cell at import.
bool validateNewArrayCellValue(WidgetInstance *prototype,std::string_view payload) {
    if(payload.find("\"array_cell_value\"")==std::string_view::npos)return true;
    if(!prototype || payload.size()>8U*1024U*1024U)return false;
    auto value=source::internal::parseAuthoringJsonCandidate(payload);
    struct Cleanup {std::optional<JsonValue> &value;~Cleanup(){
        if(value)source::internal::clearJsonTree(*value);
    }} cleanup{value};
    return value && value->type==JsonValue::Type::Object &&
        (!objectField(*value,"array_cell_value") || applyArrayCellValueJson(*prototype,*value,payload,false));
}
bool validateStoredArrayCellValues(WidgetInstance &root) {
    std::vector<WidgetInstance *> pending{&root};
    while(!pending.empty()) {
        auto *widget=pending.back();pending.pop_back();
        if (numericCanonicalValueTypeIsComplex(widget->valueType) &&
            !widgetNumericComplexValueLexeme(*widget)) return false;
        if(auto *cluster=clusterContainerState(*widget))
            for(auto &field:cluster->elements)if(field.widget)pending.push_back(field.widget.get());
        if(auto *array=arrayContainerState(*widget)) {
            if(!checkedArrayElementCount(array->shape))return false;
            if(array->elementTemplate)pending.push_back(array->elementTemplate.get());
            for(const auto &cell:array->elements) {
                const auto &type = array->elementTemplate ? array->elementTemplate->valueType : array->elementValueType;
                if (cell.kind == ArrayElementValueKind::Number && numericCanonicalValueTypeIsComplex(type) &&
                    !numeric_value::parseComplexNumber(cell.displayValue,type)) return false;
                if(cell.kind==ArrayElementValueKind::Widget &&
                    !validateNewArrayCellValue(array->elementTemplate.get(),cell.displayValue))return false;
            }
        }
    }
    return true;
}
} // namespace

std::optional<WidgetInstance> deserializeArrayElementWidget(std::string_view source,const WidgetInstance *prototype,
    bool migrateSurvivingFields,bool *compactValueOnly) {
    if(compactValueOnly)*compactValueOnly=false;
    if (source.size() > 8U * 1024U * 1024U) return std::nullopt;
    auto root = frog::native_engine::source::internal::parseAuthoringJsonCandidate(source);
    struct Cleanup { std::optional<JsonValue> &root; ~Cleanup() {
        if (root) frog::native_engine::source::internal::clearJsonTree(*root);
    } } cleanup{root};
    if (!root || root->type != JsonValue::Type::Object) return std::nullopt;
    if(root->member("format") && !document_internal::privateWidgetValueObject(*root))return std::nullopt;
    std::string error;
    if (!document_internal::inflateArrayStorage(*root, error)) return std::nullopt;
    try{if((prototype&&!unavailableComplexNumericIdentity(prototype->valueType).empty())||
        !document_internal::unavailableNumericFacts(*root,true).empty())return std::nullopt;}
    catch(...){return std::nullopt;}
    if(objectField(*root,"array_cell_value")) {
        if(!prototype)return std::nullopt;
        auto value=*prototype;
        bool decodedCompactValueOnly=true;
        if(!applyArrayCellValueJson(value,*root,source,true,migrateSurvivingFields,
                compactValueOnly?&decodedCompactValueOnly:nullptr))return std::nullopt;
        // A persisted cell contains authored data. An unchanged marker must
        // not borrow a live Graph snapshot from its presentation prototype.
        std::vector<WidgetInstance *> pending{&value};
        while(!pending.empty()) {
            auto *widget=pending.back();pending.pop_back();
            if(widget->waveformGraph) {
                widget->waveformGraph->current.reset();
                if(!widget->waveformGraph->preservedSource)widget->waveformGraph->diagnostic.clear();
            }
            if(auto *cluster=clusterContainerState(*widget))
                for(auto &field:cluster->elements)if(field.widget)pending.push_back(field.widget.get());
            if(auto *array=arrayContainerState(*widget);array && array->elementTemplate)
                pending.push_back(array->elementTemplate.get());
        }
        if(compactValueOnly)*compactValueOnly=decodedCompactValueOnly;
        return value;
    }
    auto value=sceneObjectFromJson(*root, 1,source).widget;
    if(value && !validateStoredArrayCellValues(*value))return std::nullopt;
    return value;
}

std::optional<FrogDraftInterfaceMapSettings> readFrogDraftInterfaceMapSettings(
    const FrogDraftInterface &declaration, std::string *error) {
    if (error) error->clear();
    const auto root = source::parseJson(declaration.declarationUtf8(), error,
        source::JsonNumberPolicy::PreserveLexeme);
    const auto refuse = [&](std::string message) -> std::optional<FrogDraftInterfaceMapSettings> {
        if (error) *error = std::move(message);
        return std::nullopt;
    };
    if (!root) return std::nullopt;
    if (root->type != JsonValue::Type::Object)
        return refuse("A private interface declaration must be an object before presentation projection.");
    const auto validateMap = [&](const JsonValue &map, std::string_view key) -> std::string {
        if (map.type != JsonValue::Type::Object)
            return "Retained private interface " + std::string(key) + " must be an object for presentation projection.";
        for (const auto field : std::array<std::string_view, 4>{
            "binding_map_profile", "layout_profile_id", "layout_pattern_id", "layoutPatternId"}) {
            const auto *value = map.member(field);
            if (value && value->type != JsonValue::Type::String)
                return "Retained private map field " + std::string(field) + " must be a string for presentation projection.";
        }
        for (const auto field : std::array<std::string_view, 2>{"visual_transforms", "visualTransforms"}) {
            const auto *value = map.member(field);
            if (!value) continue;
            if (value->type != JsonValue::Type::Array || std::any_of(value->array.begin(), value->array.end(),
                [](const auto &item) { return item.type != JsonValue::Type::String; }))
                return "Retained private map field " + std::string(field) + " must be an array of strings for presentation projection.";
        }
        return {};
    };
    // An internally present recognized map must never be repaired by falling
    // back to the outer historical source member, even if that member is valid.
    bool innerMapPresent = false;
    for (const auto key : std::array<std::string_view, 2>{"map", "interface_map"}) {
        const auto *map = root->member(key);
        if (!map) continue;
        innerMapPresent = true;
        if (const auto problem = validateMap(*map, key); !problem.empty()) return refuse(problem);
    }
    if (!innerMapPresent) {
        if (const auto bytes = declaration.retainedLegacyRootInterfaceMapUtf8()) {
            const auto map = source::parseJson(*bytes, error, source::JsonNumberPolicy::PreserveLexeme);
            if (!map) return std::nullopt;
            if (const auto problem = validateMap(*map, "root.interface_map"); !problem.empty()) return refuse(problem);
            return retainedInterfaceMapSettings(&*root, nullptr, &*map);
        }
    }
    return retainedInterfaceMapSettings(&*root, nullptr);
}

std::optional<source::JsonValue> readFrogDraftJson(std::string_view source,std::string *error) {
    constexpr std::string_view bom{"\xEF\xBB\xBF",3};
    const bool hasBom=source.starts_with(bom);if(hasBom)source.remove_prefix(bom.size());
    std::string localError;
    auto *diagnostic = error ? error : &localError;
    auto root = frog::native_engine::source::parseJson(source, diagnostic);
    if (!root && *diagnostic == "JSON nesting depth exceeds the supported maximum of 128.") {
        auto candidate = frog::native_engine::source::internal::parseAuthoringJsonCandidate(source);
        if (candidate && isHistoricalByteStringDraft(*candidate)) {
            root = std::move(candidate);
            diagnostic->clear();
        } else if (candidate) {
            frog::native_engine::source::internal::clearJsonTree(*candidate);
        }
    }
    if (!root.has_value()) {
        auto candidate = frog::native_engine::source::internal::parseLegacyByteStringDraftCandidate(source);
        if (!candidate.has_value() || !isHistoricalByteStringDraft(*candidate)) {
            // A public, mixed, unknown or malformed envelope must retain the
            // original strict JSON diagnostic, never inherit draft leniency.
            return std::nullopt;
        }
        root = std::move(candidate);
        if (error != nullptr) error->clear();
    }
    if(hasBom && (!root->member("format")||root->member("format")->string!=kFrogDraftDocumentFormat)) {
        if(error)*error="The private BOM compatibility adapter does not admit public source.";return std::nullopt;
    }
    return root;
}

std::optional<FrogDraftData> readFrogDraftData(
    std::string_view source,
    std::string *error,
    FrogDocumentReadDiagnostic *readDiagnostic) {
    if (readDiagnostic) *readDiagnostic = {};
    const auto originalSource = source;
    constexpr std::string_view utf8Bom{"\xEF\xBB\xBF", 3U};
    const bool privateTransportBom = source.starts_with(utf8Bom);
    if (privateTransportBom && originalSource.size() > kMaximumFrogDocumentBytes) {
        if (error) *error = "FROG document exceeds the maximum supported size of 64 MiB.";
        return std::nullopt;
    }
    if (privateTransportBom) source.remove_prefix(utf8Bom.size());
    const auto sourceOffset = originalSource.size() - source.size();
    const auto originalPosition = [sourceOffset](std::size_t offset) {
        return offset == std::string_view::npos ? offset : offset + sourceOffset;
    };
    auto root=readFrogDraftJson(source,error);
    struct Cleanup { std::optional<JsonValue> &root; ~Cleanup() {
        if (root) frog::native_engine::source::internal::clearJsonTree(*root);
    } } cleanup{root};
    if(!root)return std::nullopt;
    if (root->type != JsonValue::Type::Object) {
        if (error != nullptr) {
            *error = "FROG document root must be an object.";
        }
        return std::nullopt;
    }

    std::vector<const JsonValue *> referenceEnvelopes{&*root};
    while(!referenceEnvelopes.empty()) {
        const auto *value=referenceEnvelopes.back();referenceEnvelopes.pop_back();
        if(value->type==JsonValue::Type::Object) {
            if(jsonStringField(*value,"classId")==kReferenceWidgetClass || jsonStringField(*value,"classId")==kCollectionWidgetClass) {
                const auto widget=sceneObjectFromJson(*value,1,source).widget;
                if(!widget || !binding::widgetType(*widget).valid()) {
                    if(error)*error="Invalid reference or Collection widget value and complete schema.";
                    return std::nullopt;
                }
            }
            for(const auto &[key,child]:value->object)referenceEnvelopes.push_back(&child);
        } else if(value->type==JsonValue::Type::Array)
            for(const auto &child:value->array)referenceEnvelopes.push_back(&child);
    }
    const auto format = jsonStringField(root.value(), "format");
    const bool publicSource = !format.has_value();
    if (privateTransportBom && publicSource) {
        if (error) *error = "The private BOM compatibility adapter does not admit public source.";
        return std::nullopt;
    }
    if (format.has_value() && objectField(*root, "spec_version")) {
        if (error) *error = document_internal::publicImportError(
            "format/spec_version", "ambiguous private-draft and public-source authorities");
        return std::nullopt;
    }
    if (format.has_value() && format.value() != kFrogDraftDocumentFormat) {
        if (error != nullptr) {
            *error = "Unsupported FROG document format.";
        }
        return std::nullopt;
    }
    if (publicSource) {
        const auto importError = document_internal::validatePublicImportEnvelope(*root);
        if (!importError.empty()) {
            if (error) *error = importError;
            return std::nullopt;
        }
    }
    // Capture counts before canonical flattening can filter malformed entries.
    const auto publicGraphCounts = publicSource
        ? document_internal::publicImportGraphCounts(*root) : std::nullopt;

    if (format.has_value()) {
        FrogDraftRevisionField revisionField;
        if (const auto *rawRevision = objectField(root.value(), "draft_revision");
            rawRevision != nullptr) {
            if (rawRevision->type == JsonValue::Type::Number) {
                revisionField.kind = FrogDraftRevisionFieldKind::Number;
                revisionField.number = rawRevision->number;
            } else {
                revisionField.kind = FrogDraftRevisionFieldKind::InvalidType;
            }
        }

        std::string revisionError;
        if (!resolveFrogDraftRevision(revisionField, &revisionError).has_value()) {
            if (error != nullptr) {
                *error = std::move(revisionError);
            }
            return std::nullopt;
        }
    }

    if (const auto conditionError = document_internal::validateConditionalDisableJson(*root);
        !conditionError.empty()) {
        if (error) *error = conditionError;
        return std::nullopt;
    }
    // Reject incomplete links and old revision claims before projecting any
    // widgets, so a malformed link can never become an independent local copy.
    std::vector<const JsonValue *> typeDefinitionValues{&*root};
    while(!typeDefinitionValues.empty()) {
        const auto *value=typeDefinitionValues.back();typeDefinitionValues.pop_back();
        if(const auto *link=value->member("type_definition")) {
            if(publicSource || jsonUnsignedField(*root,"draft_revision").value_or(0U)<kFrogTypeDefinitionDraftRevision) {
                if(error)*error="Type Definition links require private draft_revision 8.";return std::nullopt;
            }
            std::ostringstream encoded;document_internal::appendJsonValue(encoded,*link);
            if(!deserializeTypeDefinitionLink(encoded.str(),error))return std::nullopt;
        }
        for(const auto &[key,child]:value->object)typeDefinitionValues.push_back(&child);
        for(const auto &child:value->array)typeDefinitionValues.push_back(&child);
    }
    FrogDraftData document;
    if (!publicSource) {
        if (const auto *defaults = objectField(*root, "text_defaults")) {
            const auto pointSize = jsonNumberField(*defaults, "point_size");
            if (defaults->type != JsonValue::Type::Object || !pointSize ||
                !validDocumentTextPointSize(*pointSize)) {
                if (error) *error = "Private FROG text_defaults.point_size must be a finite number from 6 to 144 pt.";
                return std::nullopt;
            }
            document.properties.defaultTextPointSize = *pointSize;
            if (const auto *family = objectField(*defaults, "font_family")) {
                if (family->type != JsonValue::Type::String ||
                    !validDocumentTextFontFamily(family->string)) {
                    if (error) *error = "Private FROG text_defaults.font_family must be nonempty UTF-8 of at most 256 bytes without NUL.";
                    return std::nullopt;
                }
                document.properties.defaultTextFontFamily = family->string;
            }
        }
    }
    const ScopedDocumentTextDefaultsBinding documentTextContext{{
        &document.properties.defaultTextPointSize, &document.properties.defaultTextFontFamily}};
    if (publicSource) {
        auto authority = FrogSourceDocument::read(source, error);
        if (!authority) return std::nullopt;
        document.publicSourceAuthority = std::make_shared<const FrogSourceDocument>(std::move(*authority));
    }
    if (!publicSource) {
        std::string storageError;
        const bool allowCompact = jsonUnsignedField(*root, "draft_revision").value_or(0U) >= kFrogArrayStorageDraftRevision;
        if ((!allowCompact && objectField(*root, "storage")) ||
            !document_internal::readArrayStorageMode(*root, document.properties.arrayStorageMode, storageError) ||
            !document_internal::inflateArrayStorage(*root, storageError, allowCompact)) {
            if (error) *error = storageError.empty() ? "FROG storage options require private draft_revision 3." : storageError;
            return std::nullopt;
        }
    }
    // This check precedes every widget materialization and numeric setter.
    // The existing recovery owner can preserve the complete original source;
    // ordinary Read must not turn unsupported Complex aliases into CDB.
    if(!publicSource)try{const auto unavailable=document_internal::unavailableNumericFacts(*root);if(!unavailable.empty()){
        const auto &fact=unavailable.front();if(error)*error="Numeric representation '"+fact.spelling+"' ("+fact.canonicalIdentity+") at "+fact.pointer+
            " has no exact native carrier/persistence projection. Open in explicit recovery to preserve its original type, lexical value and configuration; no CDB conversion is permitted.";
        return std::nullopt;
    }}catch(const std::exception &failure){if(error)*error=failure.what();return std::nullopt;}
    if (const auto symbolError = document_internal::readCompilationSymbols(*root, document.properties); !symbolError.empty()) {
        if (error) *error = symbolError;
        return std::nullopt;
    }
    document.title = jsonStringField(root.value(), "title").value_or("Untitled 1");
    if (const auto *metadata = objectField(root.value(), "metadata")) {
        for(const auto key:{"created_by","saved_by"})if(const auto *version=metadata->member(key)) {
            if(version->type!=JsonValue::Type::String || !validFrogStudioFileVersion(version->string)) {
                if(error)*error="Invalid metadata."+std::string(key)+": a GS version must have four unsigned decimal components.";
                return std::nullopt;
            }
            (std::string_view(key)=="created_by"?document.properties.fileVersions.createdBy:
                document.properties.fileVersions.savedBy)=version->string;
        }
        const auto persistedName = jsonStringField(*metadata, "name").value_or(document.title);
        const auto nameSource = jsonStringField(*metadata, "name_source");
        if (nameSource.value_or({}) == "custom") {
            document.properties.metadata.useDocumentName = false;
            document.properties.metadata.customProgramName = persistedName;
        } else {
            document.properties.metadata.useDocumentName = true;
            if (!nameSource.has_value()) {
                // Revision 0/1 used metadata.name as both the document title and
                // program name. Preserve that legacy document identity.
                document.title = persistedName;
            }
        }
        document.properties.metadata.description =
            jsonStringField(*metadata, "description").value_or({});
        if (document.properties.metadata.description.empty()) {
            document.properties.metadata.description =
                jsonStringField(*metadata, "summary").value_or({});
        }
        document.properties.metadata.author = jsonStringField(*metadata, "author").value_or({});
        document.properties.metadata.automaticVersioningEnabled =
            jsonBooleanField(*metadata, "automatic_versioning").value_or(false);
        document.properties.metadata.programVersion =
            normalizeFrogProgramVersion(
                jsonStringField(*metadata, "program_version").value_or("0.0.0.0"));
        document.properties.metadata.tags = jsonStringArrayField(*metadata, "tags");
        document.properties.metadata.documentationUri =
            jsonStringField(*metadata, "documentation_uri").value_or({});
    }

    if (const auto *host = objectField(root.value(), "host")) {
        document.properties.host.version = (std::max)(
            1,
            static_cast<int>(jsonNumberField(*host, "version").value_or(1.0)));
        if (const auto *launch = objectField(*host, "launch")) {
            document.properties.host.launchTrigger =
                jsonStringField(*launch, "trigger").value_or("manual");
        }
        if (const auto *window = objectField(*host, "front_panel_window")) {
            auto &hostProperties = document.properties.host;
            hostProperties.windowStyle = jsonStringField(*window, "style").value_or("standard");
            if (jsonStringField(*window, "title_mode").value_or({}) == "custom") {
                const auto legacyCustomTitle = jsonStringField(*window, "custom_title").value_or({});
                if (!legacyCustomTitle.empty()) {
                    document.properties.metadata.useDocumentName = false;
                    document.properties.metadata.customProgramName = legacyCustomTitle;
                }
            }
            hostProperties.initialPosition =
                jsonStringField(*window, "initial_position").value_or("centered");
            hostProperties.minimumWidth = (std::max)(
                0,
                static_cast<int>(jsonNumberField(*window, "minimum_width").value_or(0.0)));
            hostProperties.minimumHeight = (std::max)(
                0,
                static_cast<int>(jsonNumberField(*window, "minimum_height").value_or(0.0)));
            hostProperties.allowClose = jsonBooleanField(*window, "allow_close").value_or(true);
            hostProperties.allowResize = jsonBooleanField(*window, "allow_resize").value_or(true);
            hostProperties.allowMinimize = jsonBooleanField(*window, "allow_minimize").value_or(true);
            hostProperties.menuBarVisible = jsonBooleanField(*window, "menu_bar_visible").value_or(true);
            hostProperties.toolbarVisible = jsonBooleanField(*window, "toolbar_visible").value_or(true);
            hostProperties.verticalScrollbarVisible =
                jsonBooleanField(*window, "vertical_scrollbar_visible").value_or(true);
            hostProperties.horizontalScrollbarVisible =
                jsonBooleanField(*window, "horizontal_scrollbar_visible").value_or(true);
            hostProperties.scrollToOrigin = jsonBooleanField(*window, "scroll_to_origin").value_or(false);
        }
    }

    if (const auto *policy = objectField(root.value(), "execution_policy")) {
        document.properties.executionPolicy.version = (std::max)(
            1,
            static_cast<int>(jsonNumberField(*policy, "version").value_or(1.0)));
        document.properties.executionPolicy.debuggingAllowed =
            jsonBooleanField(*policy, "debugging_allowed").value_or(true);
        document.properties.executionPolicy.priority =
            jsonStringField(*policy, "priority").value_or("normal");
        document.properties.executionPolicy.reentrancy =
            jsonStringField(*policy, "reentrancy").value_or("non_reentrant");
        document.properties.executionPolicy.executionSystem =
            jsonStringField(*policy, "execution_system").value_or("caller");
        document.properties.executionPolicy.automaticErrorHandling =
            jsonBooleanField(*policy, "automatic_error_handling").value_or(true);
        document.properties.executionPolicy.suspendWhenCalled =
            jsonBooleanField(*policy, "suspend_when_called").value_or(false);
        document.properties.executionPolicy.clearIndicatorsWhenCalled =
            jsonBooleanField(*policy, "clear_indicators_when_called").value_or(false);
    }

    if (!publicSource) if (const auto *native = objectField(*root, "native_options")) {
        const auto *version=objectField(*native,"version");int parsedVersion=0;
        const auto versionText=version?std::string_view(version->string):std::string_view{""};
        const auto parsed=std::from_chars(versionText.data(),versionText.data()+versionText.size(),parsedVersion);
        const auto target=jsonStringField(*native,"target_id");
        const auto backend=jsonStringField(*native,"backend_id");
        const auto optimization=jsonStringField(*native,"optimization_profile");
        if(native->type!=JsonValue::Type::Object || native->object.size()!=4 || !version || version->type!=JsonValue::Type::Number ||
           parsed.ec!=std::errc{} || parsed.ptr!=versionText.data()+versionText.size() ||
           !target || !backend || !optimization) {
            if(error)*error="Private native_options requires an integer version and exact target_id/backend_id/optimization_profile strings.";
            return std::nullopt;
        }
        document.properties.nativeOptions=NativeProgramOptions{parsedVersion,*target,*backend,*optimization};
    }

    const JsonValue *icon = objectField(root.value(), "icon");
    std::string_view iconPath = "/icon";
    if (icon == nullptr) {
        icon = objectField(root.value(), "file_icon");
        iconPath = "/file_icon";
    }
    if (icon != nullptr) {
        const auto iconFormat = jsonStringField(*icon, "format").value_or("svg");
        if (iconFormat == "svg" || iconFormat == "image/svg+xml") {
            const auto svg = jsonStringField(*icon, "svg");
            document.fileIconSvgText = svg.value_or(jsonStringField(*icon, "svgText").value_or({}));
            if (const auto problem = frogIconPresentationError(document.fileIconSvgText); !problem.empty()) {
                // Preserve the private reader's established string API while
                // carrying the existing shared authority's semantic refusal
                // separately. Callers never infer a category from this text.
                if (error) *error = "/icon/svg: " + std::string(problem);
                if (readDiagnostic) *readDiagnostic = {
                    FrogDocumentReadErrorKind::InvalidPresentation,
                    std::string(iconPath) + (svg ? "/svg" : "/svgText"),
                    std::string(problem)};
                return std::nullopt;
            }
        }
    }

    const JsonValue *publicInterface = objectField(root.value(), "interface");
    if (publicInterface == nullptr) {
        publicInterface = objectField(root.value(), "public_interface");
    }
    if (publicInterface != nullptr && publicInterface->sourceBegin != std::string_view::npos &&
        publicInterface->sourceEnd <= source.size()) {
        const auto declarationBytes = source.substr(publicInterface->sourceBegin,
            publicInterface->sourceEnd - publicInterface->sourceBegin);
        // The historical byte-string exception belongs only to the private
        // draft reader. Public import never inherits this compatibility rule.
        std::optional<std::string> retainedLegacyRootMap;
        if (!publicSource) {
            if (const auto *map = objectField(*root, "interface_map"); map &&
                map->sourceBegin != std::string_view::npos && map->sourceEnd <= source.size()) {
                retainedLegacyRootMap = std::string(source.substr(map->sourceBegin, map->sourceEnd - map->sourceBegin));
            }
        }
        document.declaredInterface = publicSource
            ? FrogDraftInterface::read(declarationBytes)
            : FrogDraftInterface::retainParsedDraftDeclaration(std::string(declarationBytes), std::move(retainedLegacyRootMap));
    }
    // Historical document reading retains its exact permissive aliases and
    // root.interface_map fallback. Strict projection inspection is separate.
    const auto mapSettings = retainedInterfaceMapSettings(publicInterface, &*root);
    document.interfaceBindingMapProfileId = mapSettings.profileId;
    document.interfaceMapLayoutPatternId = mapSettings.patternId;
    document.interfaceMapVisualTransforms = mapSettings.transformTokens;

    JsonValue emptyFrontPanel;
    emptyFrontPanel.type = JsonValue::Type::Object;
    const auto *frontPanel = objectField(root.value(), "frontPanel");
    if (frontPanel == nullptr) {
        frontPanel = objectField(root.value(), "front_panel");
    }
    // An absent presentation does not erase the independent interface or make
    // a historical private source unreadable. This empty view authors neither
    // widgets nor nodes; absent Diagram authority remains migration-blocked.
    // A present private panel still requires its actual widgets array below.
    const bool absentFrontPanel = frontPanel == nullptr;
    if (absentFrontPanel) frontPanel = &emptyFrontPanel;

    const JsonValue *frontPanelCanvas = objectField(*frontPanel, "canvas");
    if (frontPanelCanvas != nullptr) {
        const auto width = jsonNumberField(*frontPanelCanvas, "width");
        const auto height = jsonNumberField(*frontPanelCanvas, "height");
        if (width.has_value() && height.has_value() && width.value() > 0.0 && height.value() > 0.0) {
            document.widgets.setFrontPanelCanvasSize(Size{ width.value(), height.value() });
        }
    }

    const JsonValue *grid = objectField(*frontPanel, "grid");
    if (grid == nullptr) {
        if (frontPanelCanvas != nullptr) {
            grid = objectField(*frontPanelCanvas, "grid");
        }
    }
    if (grid != nullptr) {
        if (const auto pitch = jsonNumberField(*grid, "pitch")) {
            document.widgets.setGridPitch(pitch.value());
        }
    }

    if (const auto *ide = objectField(root.value(), "ide")) {
        if (const auto *windows = objectField(*ide, "windows"); windows &&
            jsonStringField(*windows, "coordinate_space") == "screen_pixels") {
            const auto readWindow = [&](std::string_view name)
                -> std::optional<FrogEditorWindowPlacement> {
                const auto *window = objectField(*windows, name);
                const auto *bounds = window ? objectField(*window, "bounds") : nullptr;
                if (!bounds) return std::nullopt;
                const auto x = jsonNumberField(*bounds, "x");
                const auto y = jsonNumberField(*bounds, "y");
                const auto width = jsonNumberField(*bounds, "width");
                const auto height = jsonNumberField(*bounds, "height");
                if (!x || !y || !width || !height) return std::nullopt;
                FrogEditorWindowPlacement result{
                    { *x, *y, *width, *height },
                    jsonBooleanField(*window, "maximized").value_or(false) };
                return validFrogEditorWindowPlacement(result)
                    ? std::optional{ result } : std::nullopt;
            };
            document.editorWindows.frontPanel = readWindow("front_panel");
            document.editorWindows.diagram = readWindow("diagram");
            document.editorWindows.frogSource = readWindow("frog_source");
        }
        const JsonValue *ideFrontPanel = objectField(*ide, "front_panel");
        if (ideFrontPanel == nullptr) {
            ideFrontPanel = objectField(*ide, "frontPanel");
        }
        if (ideFrontPanel != nullptr) {
            const auto opacity = static_cast<int>(jsonNumberField(*ideFrontPanel, "background_opacity").value_or(100.0));
            if (const auto backgroundColor = jsonStringField(*ideFrontPanel, "background_color")) {
                if (isTransparentColorLiteral(backgroundColor.value())) {
                    document.widgets.setIdeFrontPanelBackground(std::uint32_t{ 0x000000U }, 0);
                } else if (const auto rgb = parseRgbHex(backgroundColor.value())) {
                    document.widgets.setIdeFrontPanelBackground(rgb.value(), opacity);
                }
            }
            const auto gridOpacity = static_cast<int>(jsonNumberField(*ideFrontPanel, "grid_opacity").value_or(100.0));
            if (const auto gridColor = jsonStringField(*ideFrontPanel, "grid_color")) {
                if (isTransparentColorLiteral(gridColor.value())) {
                    document.widgets.setIdeFrontPanelGrid(std::uint32_t{ 0x000000U }, 0);
                } else if (const auto rgb = parseRgbHex(gridColor.value())) {
                    document.widgets.setIdeFrontPanelGrid(rgb.value(), gridOpacity);
                }
            }
            if (const auto gridVisible = jsonBooleanField(*ideFrontPanel, "grid_visible")) {
                document.widgets.setIdeFrontPanelGridVisible(gridVisible.value());
            }
            if (const auto snapToGrid = jsonBooleanField(*ideFrontPanel, "snap_to_grid")) {
                document.widgets.setIdeFrontPanelSnapToGrid(snapToGrid.value());
            }
            if (const auto zoomPercent = jsonNumberField(*ideFrontPanel, "zoom_percent")) {
                document.widgets.setIdeFrontPanelZoomPercent(static_cast<int>(std::lround(zoomPercent.value())));
            }
            if (const auto *viewportOrigin = objectField(*ideFrontPanel, "viewport_origin")) {
                document.widgets.setIdeFrontPanelViewportOrigin(Point{
                    jsonNumberField(*viewportOrigin, "x").value_or(0.0),
                    jsonNumberField(*viewportOrigin, "y").value_or(0.0)
                });
            }
        }

        const JsonValue *ideDiagram = objectField(*ide, "diagram");
        if (ideDiagram == nullptr) {
            ideDiagram = objectField(*ide, "block_diagram");
        }
        if (ideDiagram != nullptr) {
            const auto opacity = static_cast<int>(jsonNumberField(*ideDiagram, "background_opacity").value_or(100.0));
            if (const auto backgroundColor = jsonStringField(*ideDiagram, "background_color")) {
                if (isTransparentColorLiteral(backgroundColor.value())) {
                    document.widgets.setIdeDiagramBackground(std::uint32_t{ 0x000000U }, 0);
                } else if (const auto rgb = parseRgbHex(backgroundColor.value())) {
                    document.widgets.setIdeDiagramBackground(rgb.value(), opacity);
                }
            }
            const auto gridOpacity = static_cast<int>(jsonNumberField(*ideDiagram, "grid_opacity").value_or(100.0));
            if (const auto gridColor = jsonStringField(*ideDiagram, "grid_color")) {
                if (isTransparentColorLiteral(gridColor.value())) {
                    document.widgets.setIdeDiagramGrid(std::uint32_t{ 0x000000U }, 0);
                } else if (const auto rgb = parseRgbHex(gridColor.value())) {
                    document.widgets.setIdeDiagramGrid(rgb.value(), gridOpacity);
                }
            }
        }
    }

    JsonValue emptyWidgets;
    emptyWidgets.type = JsonValue::Type::Array;
    const auto *widgets = objectField(*frontPanel, "widgets");
    if (widgets == nullptr && (publicSource || absentFrontPanel)) widgets = &emptyWidgets;
    if (widgets == nullptr || widgets->type != JsonValue::Type::Array) {
        if (error != nullptr) {
            *error = "Missing frontPanel.widgets array.";
        }
        return std::nullopt;
    }

    int fallbackIndex = 1;
    int maxSerial = 0;
    for (const auto &widget : widgets->array) {
        if (widget.type != JsonValue::Type::Object) {
            continue;
        }
        auto object = sceneObjectFromJson(widget, fallbackIndex++,source);
        maxSerial = (std::max)(maxSerial, widgetSerialFromId(object.id));
        // An explicit placement is source data, not an insertion-time fit
        // request. Missing historical bounds retain the existing admission.
        const auto *decodedWidget = document_internal::privateWidgetValueObject(widget);
        document.widgets.addObject(std::move(object),
            decodedWidget != nullptr && jsonObjectBounds(*decodedWidget).has_value());
    }

    if (!document.widgets.validateTabMembership(error)) return std::nullopt;

    if (const auto *groups = objectField(*frontPanel, "groups");
        groups != nullptr && groups->type == JsonValue::Type::Array) {
        int fallbackGroupIndex = 1;
        for (const auto &groupValue : groups->array) {
            if (groupValue.type != JsonValue::Type::Object) {
                continue;
            }
            SceneObjectGroup group;
            group.id = jsonStringField(groupValue, "id").value_or("group-load-" + std::to_string(fallbackGroupIndex++));
            group.objectIds = jsonStringArrayField(groupValue, "object_ids");
            if (group.objectIds.empty()) {
                group.objectIds = jsonStringArrayField(groupValue, "objectIds");
            }
            static_cast<void>(document.widgets.addObjectGroup(std::move(group)));
        }
    }

    JsonValue *diagram = mutableObjectField(root.value(), "diagram");
    if (diagram == nullptr) {
        diagram = mutableObjectField(root.value(), "blockDiagram");
    }
    // Presence, including an empty array, is authored graph state. Capture it
    // before canonical region flattening materializes its derived arrays.
    const auto *authoredNodes = diagram ? objectField(*diagram, "nodes") : nullptr;
    const bool authoredDiagram = authoredNodes != nullptr;
    if (authoredNodes && authoredNodes->type != JsonValue::Type::Array) {
        if (error) *error = "Diagram nodes must be an array.";
        return std::nullopt;
    }
    int maxDiagramNodeSerial = 0;
    int maxDiagramEdgeSerial = 0;
    int maxDiagramWireFragmentSerial = 0;
    if (diagram != nullptr) {
        document_internal::flattenCanonicalStructureRegions(*diagram);
        if (const auto zoomPercent = jsonNumberField(*diagram, "zoom_percent")) {
            document.diagramZoomPercent = std::clamp(
                static_cast<int>(std::lround(zoomPercent.value())),
                30,
                150);
        }
        if (const auto *viewportOrigin = objectField(*diagram, "viewport_origin")) {
            document.diagramViewportOrigin.x =
                jsonNumberField(*viewportOrigin, "x").value_or(0.0);
            document.diagramViewportOrigin.y =
                jsonNumberField(*viewportOrigin, "y").value_or(0.0);
        }
        if (const auto *nodes = objectField(*diagram, "nodes");
            nodes != nullptr && nodes->type == JsonValue::Type::Array) {
            for (const auto &nodeValue : nodes->array) {
                if (nodeValue.type != JsonValue::Type::Object) {
                    continue;
                }
                DiagramNode node;
                bool loadedFrogCall = false;
                node.id = jsonStringField(nodeValue, "id").value_or({});
                if(const auto *link=nodeValue.member("type_definition")) {
                    std::ostringstream encoded;document_internal::appendJsonValue(encoded,*link);
                    const auto decoded=deserializeTypeDefinitionLink(encoded.str(),error);
                    if(!decoded)return std::nullopt;
                    node.typeDefinition=*decoded;
                }
                node.kind = jsonStringField(nodeValue, "kind").value_or("primitive");
                node.type = jsonStringField(nodeValue, "type").value_or({});
                node.constantOwnerNodeId = jsonStringField(nodeValue, "constant_owner_node_id").value_or("");
                node.paletteCatalogId = jsonStringField(nodeValue, "palette_catalog_id").value_or("");
                node.constantFieldId = jsonStringField(nodeValue, "constant_field_id").value_or("");
                node.constantFieldOrder = jsonUnsignedField(nodeValue, "constant_field_order").value_or(0U);
                node.clusterConstantCollapsed = jsonBooleanField(nodeValue, "cluster_constant_collapsed").value_or(false);
                if (const auto schema = jsonStringField(nodeValue, "array_element_template")) {
                    node.arrayElementTemplate = deserializeArrayElementWidget(*schema);
                    if(!node.arrayElementTemplate && schema->find("\"array_cell_value\"")!=std::string::npos) {
                        if(error)*error="Invalid compact Array cell value in Diagram node '"+node.id+"'.";
                        return std::nullopt;
                    }
                }
                if (node.arrayElementTemplate)
                    normalizeColorBoxGeometry(*node.arrayElementTemplate, kColorBoxConstantSide);
                node.arrayElementIcon = jsonBooleanField(nodeValue, "array_element_icon").value_or(false);
                node.clusterConstantAutoSizing = jsonStringField(nodeValue, "cluster_constant_autosizing").value_or("none");
                node.clusterConstantExpandedWidth = jsonNumberField(nodeValue, "cluster_expanded_width").value_or(180.0);
                node.clusterConstantExpandedHeight = jsonNumberField(nodeValue, "cluster_expanded_height").value_or(110.0);
                if(node.clusterConstantAutoSizing!="none" && node.clusterConstantAutoSizing!="fit" &&
                   node.clusterConstantAutoSizing!="horizontal" && node.clusterConstantAutoSizing!="vertical")
                    node.clusterConstantAutoSizing="none";
                // These are saved expanded dimensions, not palette defaults.
                // Size to Fit may be smaller than the ordinary cluster minimum;
                // the Error preset is intentionally narrower as well.
                const double minimumClusterWidth=node.clusterConstantAutoSizing=="fit" ? 1.0 :
                    node.type=="frog.studio.error.constant" ? kErrorConstantWidth : 70.0;
                const double minimumClusterHeight=node.clusterConstantAutoSizing=="fit" ? 1.0 : 50.0;
                node.clusterConstantExpandedWidth=std::clamp(node.clusterConstantExpandedWidth,
                    minimumClusterWidth,100000.0);
                node.clusterConstantExpandedHeight=std::clamp(node.clusterConstantExpandedHeight,
                    minimumClusterHeight,100000.0);
                if (node.kind == "structure") {
                    const auto canonicalType = document_internal::nodeTypeForCanonicalStructure(
                        jsonStringField(nodeValue, "structure_type").value_or({}));
                    if (!canonicalType.empty()) {
                        node.kind = "primitive";
                        node.type = canonicalType;
                    }
                }
                node.diagramOwnerStructureId = jsonStringField(
                    nodeValue,
                    "diagram_owner_structure_id").value_or({});
                node.diagramOwnerRegionId = jsonStringField(
                    nodeValue,
                    "diagram_owner_region_id").value_or({});
                if (node.type == "frog.studio.structures.case_structure") {
                    const auto profile = jsonStringField(nodeValue, "case_profile").value_or("");
                    if (!profile.empty() && profile != "frog.case-editing@1") {
                        if (error) *error = "Unsupported Case editing profile: " + profile + ". Open this file with a compatible reader; no regions were discarded.";
                        return std::nullopt;
                    }
                }
                node.valueType = jsonStringField(nodeValue, "value_type")
                    .value_or(jsonStringField(nodeValue, "valueType").value_or({}));
                const bool presetArrayConstantNode = node.kind == "primitive" &&
                    node.type.starts_with("frog.studio.array.other_constants.");
                // Early palette-array writers omitted value_type/array_constant.
                // Recover the declared element type, never replace a saved override.
                if (presetArrayConstantNode && node.valueType.empty()) {
                    if (const auto *entry = findFunctionCatalogEntryByDiagramNodeType(node.type)) {
                        node.valueType = functionArrayPresetElementType(*entry);
                    }
                }
                if (objectField(nodeValue,"studio_application_control") &&
                        jsonUnsignedField(*root,"draft_revision").value_or(0U)<kFrogApplicationControlDraftRevision) {
                    if (error) *error="Application Control configuration requires private draft_revision 6.";
                    return std::nullopt;
                }
                if (!document_internal::readApplicationControl(nodeValue, node, error)) return std::nullopt;
                if (const auto *configuration = objectField(nodeValue, "studio_connectivity")) {
                    if (jsonUnsignedField(*root, "draft_revision").value_or(0U) < kFrogConnectivityDraftRevision) {
                        if (error) *error = "Connectivity configuration requires private draft_revision 7.";
                        return std::nullopt;
                    }
                    node.connectivity = deserializeConnectivityConfiguration(*configuration, error);
                    if (!node.connectivity || node.type != "frog.studio." + node.connectivity->catalogId) {
                        if (error && error->empty()) *error = "Connectivity configuration does not match an available diagram node.";
                        return std::nullopt;
                    }
                }
                if (const auto *configuration = objectField(nodeValue, "studio_native_call")) {
                    if (jsonUnsignedField(*root, "draft_revision").value_or(0U) < kFrogConnectivityDraftRevision) {
                        if (error) *error = "Native Call configuration requires private draft_revision 7.";
                        return std::nullopt;
                    }
                    if (node.type != "frog.studio.connectivity.libraries.call_library_function" || node.connectivity) {
                        if (error) *error = "Native Call configuration does not match its diagram node.";
                        return std::nullopt;
                    }
                    // Native ABI defects remain editable authoring diagnostics. Only a malformed
                    // codec is refused; visual migration never repairs the argument signature.
                    node.nativeCall = nativeCallFromJson(*configuration, error);
                    if (!node.nativeCall) return std::nullopt;
                }
                const auto storedLabel = jsonStringField(nodeValue, "label");
                if(const auto *stored=nodeValue.member("studio_collection_constant")) {
                    const auto state=readCollectionAuthoringState(*stored,error);
                    const auto id=collectionFunctionId(node.type);
                    if(!state||node.kind!="primitive"||
                       (id!="collection.map.constant"&&id!="collection.set.constant")||
                       (id=="collection.map.constant")!=(state->kind==CollectionKind::Map)||
                       (!node.valueType.empty()&&node.valueType!="unknown"&&node.valueType!=collectionType(*state).canonical())) {
                        if(error&&error->empty())*error="Collection constant identity, kind and complete type disagree.";return std::nullopt;
                    }
                    node.collectionConstant=*state;node.valueType=collectionType(*state).canonical();
                }else if(isCollectionConstant(node)) {
                    CollectionAuthoringState state;state.kind=collectionFunctionId(node.type)=="collection.map.constant"?CollectionKind::Map:CollectionKind::Set;
                    if(node.valueType!="unknown"&&!node.valueType.empty()){if(error)*error="A typed Collection constant is missing its complete model state.";return std::nullopt;}
                    node.collectionConstant=std::move(state);
                }
                if(const auto *stored=nodeValue.member("studio_collection_build")) {
                    const auto state=readCollectionBuildState(*stored,error);const auto id=collectionFunctionId(node.type);
                    if(!state||(id!="collection.map.build"&&id!="collection.set.build")||
                       (state->typeWitness.known()&&state->typeWitness.kind!=(id=="collection.map.build"?binding::TypeKind::Map:binding::TypeKind::Set))) {
                        if(error&&error->empty())*error="Collection Build identity and type witness disagree.";return std::nullopt;
                    }
                    node.collectionBuild=*state;
                }
                node.indexArrayOutputs = std::clamp(
                    jsonUnsignedField(nodeValue, "index_array_outputs").value_or(1U), 1U, 32U);
                if (isRetainFirstError(node.type))
                    node.retainFirstErrorInputs=jsonUnsignedField(nodeValue,"retain_first_error_inputs");
                node.arrayEditGroups = std::clamp(
                    jsonUnsignedField(nodeValue, "array_edit_groups").value_or(1U), 1U, 32U);
                node.buildArrayInputs = std::clamp(
                    jsonUnsignedField(nodeValue, "build_array_inputs").value_or(2U), 1U, 32U);
                node.reshapeArrayDimensions = std::clamp(
                    jsonUnsignedField(nodeValue, "reshape_array_dimensions").value_or(1U), 1U, 16U);
                node.buildArrayConcatenate = jsonBooleanField(nodeValue, "build_array_concatenate").value_or(false);
                node.comparisonAggregates = jsonBooleanField(nodeValue, "comparison_aggregates").value_or(false);
                node.inRangeIncludeUpperLimit = jsonBooleanField(nodeValue, "in_range_include_upper_limit").value_or(false);
                node.inRangeIncludeLowerLimit = jsonBooleanField(nodeValue, "in_range_include_lower_limit").value_or(true);
                node.fileReadLines = jsonBooleanField(nodeValue, "file_read_lines").value_or(false);
                node.fileConvertEol = jsonBooleanField(nodeValue, "file_convert_eol").value_or(true);
                node.fileDataType = jsonStringField(nodeValue, "file_data_type").value_or("f64");
                if (node.fileDataType != "f64" && node.fileDataType != "i64" && node.fileDataType != "string") {
                    if (error) *error = "Unsupported File spreadsheet data type.";
                    return std::nullopt;
                }
                node.fileXmlArray = jsonBooleanField(nodeValue, "file_xml_array").value_or(false);
                node.numericColorBox = jsonBooleanField(nodeValue, "numeric_color_box").value_or(false) &&
                    node.valueType == "u32";
                node.booleanArraySignExtensionMode = parseBooleanArraySignExtensionMode(
                    jsonStringField(nodeValue, "boolean_array_sign_extension_mode").value_or("if_output_is_signed"));
                node.arrayFunctionTerminals = std::min(jsonUnsignedField(nodeValue, "array_function_terminals").value_or(0U),32U);
                node.stringFunctionTerminals = std::clamp(jsonUnsignedField(nodeValue, "string_function_terminals").value_or(1U),1U,32U);
                if (const auto *rows=objectField(nodeValue,"studio_event_registration_rows");
                    rows && rows->type==JsonValue::Type::Array) {
                    for(const auto &row:rows->array) {
                        if(row.type!=JsonValue::Type::Object || node.eventRegistrationRows.size()>=64U) continue;
                        node.eventRegistrationRows.push_back({jsonStringField(row,"id").value_or(""),
                            jsonStringField(row,"event_kind").value_or("user_event")});
                    }
                }
                node.clusterFunctionTerminals = std::min(jsonUnsignedField(nodeValue, "cluster_function_terminals").value_or(0U),32U);
                node.clusterFieldNames = jsonStringArrayField(nodeValue, "cluster_field_names");
                if(const auto *paths=objectField(nodeValue,"cluster_field_id_paths");
                    paths && paths->type==JsonValue::Type::Array) {
                    for(const auto &path:paths->array) {
                        if(node.clusterFieldIdPaths.size()>=32U) break;
                        std::vector<std::string> ids;
                        if(path.type==JsonValue::Type::Array) for(const auto &id:path.array) {
                            if(id.type==JsonValue::Type::String && !id.string.empty()) ids.push_back(id.string);
                        }
                        node.clusterFieldIdPaths.push_back(std::move(ids));
                    }
                }
                node.clusterShowFullNames = jsonBooleanField(nodeValue, "cluster_show_full_names").value_or(false);
                if(node.clusterFieldNames.size()>32) node.clusterFieldNames.resize(32);
                node.matrixAppendByRows = jsonBooleanField(nodeValue, "matrix_append_by_rows").value_or(false);
                node.label = storedLabel.value_or("");
                node.widgetId = jsonStringField(nodeValue, "widget")
                    .value_or(jsonStringField(nodeValue, "source_widget_id")
                        .value_or(jsonStringField(nodeValue, "sourceWidgetId").value_or("")));
                const auto labelVisibleSnake = jsonBooleanField(nodeValue, "label_visible");
                const auto labelVisibleCamel = jsonBooleanField(nodeValue, "labelVisible");
                const bool timestampConstantNode = node.kind == "primitive" &&
                    (node.type == "frog.studio.timestamp.constant" || node.type == "timestamp.constant");
                const bool numericConstantNode =
                    node.kind == "primitive" &&
                    (timestampConstantNode || node.type == "frog.studio.numeric.numeric_constant" ||
                        node.type == "numeric.numeric_constant");
                const bool numericArrayConstantNode =
                    node.kind == "primitive" &&
                    (presetArrayConstantNode ||
                     node.type == "frog.studio.numeric.numeric_array_constant" ||
                        node.type == "numeric.numeric_array_constant");
                const bool untypedArrayConstantNode =
                    node.kind == "primitive" &&
                    (node.type == "frog.studio.array.array_constant" ||
                        node.type == "array.array_constant");
                const bool enumConstantNode =
                    node.kind == "primitive" &&
                    (node.type == "frog.studio.numeric.enum_constant" ||
                        node.type == "numeric.enum_constant");
                const bool enumArrayConstantNode =
                    node.kind == "primitive" &&
                    (node.type == "frog.studio.numeric.enum_array_constant" ||
                        node.type == "numeric.enum_array_constant");
                const bool ringConstantNode =
                    node.kind == "primitive" &&
                    (node.type == "frog.studio.numeric.ring_constant" ||
                        node.type == "numeric.ring_constant");
                const bool ringArrayConstantNode =
                    node.kind == "primitive" &&
                    (node.type == "frog.studio.numeric.ring_array_constant" ||
                        node.type == "numeric.ring_array_constant");
                const bool stringConstantNode =
                    node.kind == "primitive" &&
                    (node.type == "frog.studio.string.constant" ||
                        node.type == "string.constant");
                const bool pathConstantNode =
                    node.kind == "primitive" &&
                    (node.type == "frog.studio.path.constant" ||
                        node.type == "path.constant");
                const bool stringArrayConstantNode =
                    node.kind == "primitive" &&
                    (node.type == "frog.studio.string.string_array_constant" ||
                        node.type == "string.string_array_constant" ||
                        node.type == "frog.studio.path.path_array_constant" ||
                        node.type == "path.path_array_constant");
                node.labelVisible = labelVisibleSnake.value_or(
                    labelVisibleCamel.value_or(
                        !(numericConstantNode || numericArrayConstantNode ||
                            untypedArrayConstantNode ||
                            enumConstantNode || enumArrayConstantNode ||
                            ringConstantNode || ringArrayConstantNode ||
                            stringConstantNode || pathConstantNode ||
                            stringArrayConstantNode)));
                // Older Studio files emitted label_visible=true for unnamed
                // functions that had no label renderer. Keep those diagrams
                // visually unchanged until the user explicitly shows a label.
                if (!storedLabel.has_value() &&
                    (node.kind == "frog_call" ||
                     (node.kind == "primitive" &&
                      document_internal::canonicalStructureTypeForNodeType(node.type).empty() &&
                      !(numericConstantNode || numericArrayConstantNode || untypedArrayConstantNode ||
                        enumConstantNode || enumArrayConstantNode || ringConstantNode || ringArrayConstantNode ||
                        stringConstantNode || pathConstantNode || stringArrayConstantNode)))) {
                    node.labelVisible = false;
                }
                node.invertedInputPorts = jsonStringArrayField(
                    nodeValue,
                    "inverted_input_ports");
                if (node.invertedInputPorts.empty()) {
                    node.invertedInputPorts = jsonStringArrayField(
                        nodeValue,
                        "invertedInputPorts");
                }
                if (node.type == "frog.studio.structures.feedback_node") {
                    node.feedbackNodeConfiguration = std::string(
                        normalizeFeedbackNodeConfiguration(
                            jsonStringField(nodeValue, "feedback_node_configuration")
                                .value_or(jsonStringField(
                                    nodeValue,
                                "feedbackNodeConfiguration").value_or("inverse"))));
                    if (objectField(nodeValue, "feedback_delay")) {
                        const auto delay = jsonNumberField(nodeValue, "feedback_delay");
                        node.feedbackDelay = delay && std::isfinite(*delay) && *delay >= 1.0 &&
                            *delay <= 4294967295.0 && std::floor(*delay) == *delay
                            ? static_cast<std::uint32_t>(*delay) : 0U;
                    }
                    node.feedbackInitialization = jsonStringField(nodeValue, "feedback_initialization").value_or("compile_or_load");
                    node.feedbackInitializationExplicit = jsonBooleanField(nodeValue, "feedback_initialization_explicit").value_or(false);
                    node.feedbackInitializerLoopId = jsonStringField(nodeValue, "feedback_initializer_loop_id").value_or("");
                    node.feedbackHistory = jsonBooleanField(nodeValue, "feedback_history").value_or(false);
                    node.feedbackZTransform = jsonBooleanField(nodeValue, "feedback_z_transform").value_or(false);
                }
                document_internal::deserializeStructureProperties(nodeValue, node);
                if (isDiagramStructureNodeType(node.type)) {
                    const auto alignment = jsonStringField(nodeValue, "structure_subdiagram_label_alignment");
                    node.structureSubdiagramLabelAlignment = alignment &&
                        (*alignment == "left" || *alignment == "right") ? *alignment : "center";
                }
                node.structureAutoGrow = jsonBooleanField(nodeValue, "structure_auto_grow").value_or(true);
                node.forLoopIterationVisible = jsonBooleanField(nodeValue, "for_loop_iteration_visible").value_or(true);
                node.hidden = jsonBooleanField(nodeValue, "hidden").value_or(false);
                node.constantTextBoundsAutomatic = jsonBooleanField(nodeValue, "constant_text_bounds_automatic");
                node.constantArrayCellBoundsAutomatic = jsonBooleanField(nodeValue, "constant_array_cell_bounds_automatic");
                node.locked = jsonBooleanField(nodeValue, "locked").value_or(false);
                node.groupId = jsonStringField(nodeValue, "group")
                    .value_or(jsonStringField(nodeValue, "group_id").value_or({}));
                node.text = jsonStringField(nodeValue, "text").value_or({});
                const auto *documentation = objectField(nodeValue, "doc");
                if (documentation == nullptr) {
                    documentation = objectField(nodeValue, "documentation");
                }
                if (documentation != nullptr) {
                    node.documentationSummary = jsonStringField(*documentation, "summary").value_or({});
                    node.documentationUrl = jsonStringField(*documentation, "url").value_or({});
                    node.instanceDescription = jsonStringField(*documentation, "instance_description");
                    node.instanceTip = jsonStringField(*documentation, "tip");
                }
                if (node.kind == "terminal" && !node.widgetId.empty()) {
                    node.kind = "widget_value";
                }
                if (const auto *labelOffset = objectField(nodeValue, "label_offset")) {
                    node.labelOffsetX = jsonNumberField(*labelOffset, "x").value_or(0.0);
                    node.labelOffsetY = jsonNumberField(*labelOffset, "y").value_or(0.0);
                }
                bool loadedLabelSize = false;
                if (const auto *labelSize = objectField(nodeValue, "label_size")) {
                    loadedLabelSize = true;
                    node.labelWidth = (std::max)(
                        24.0,
                        jsonNumberField(*labelSize, "width").value_or(node.labelWidth));
                    node.labelHeight = (std::max)(
                        16.0,
                        jsonNumberField(*labelSize, "height").value_or(node.labelHeight));
                }
                if (const auto labelAutoSize = jsonBooleanField(nodeValue, "label_auto_size")) {
                    node.labelAutoSize = labelAutoSize.value();
                } else {
                    // Before label_auto_size existed, 180 x 31 was the untouched default.
                    // Preserve every other legacy size as an intentional manual adjustment.
                    node.labelAutoSize = !loadedLabelSize ||
                        (std::abs(node.labelWidth - 180.0) < 0.001 &&
                         std::abs(node.labelHeight - 31.0) < 0.001);
                }
                node.labelVerticalArrangement = jsonStringField(
                    nodeValue,
                    "label_vertical_arrangement").value_or("none");
                if (node.labelVerticalArrangement != "none" &&
                    node.labelVerticalArrangement != "stacked" &&
                    node.labelVerticalArrangement != "clockwise" &&
                    node.labelVerticalArrangement != "counterclockwise") {
                    node.labelVerticalArrangement = "none";
                }
                if (const auto *labelFont = objectField(nodeValue, "label_font")) {
                    node.labelFontFamily = jsonStringField(*labelFont, "family").value_or({});
                    node.labelTextStyle = jsonStringField(*labelFont, "style").value_or("plain");
                    node.labelFontPointSize = std::clamp(
                        jsonNumberField(*labelFont, "point_size").value_or(15.0),
                        6.0,
                        144.0);
                    node.labelFontPointSizeUsesDefault = jsonBooleanField(
                        *labelFont, "use_default_size").value_or(
                            std::abs(node.labelFontPointSize - 15.0) < 0.001 ||
                            std::abs(node.labelFontPointSize - 19.5) < 0.001 ||
                            (node.type.starts_with("frog.studio.structures.") &&
                             std::abs(node.labelFontPointSize - kStructureSubdiagramLabelDefaultFontPointSize) < 0.001));
                    node.labelTextRgb = jsonUnsignedField(*labelFont, "text_rgb");
                    node.labelBackgroundRgb = jsonUnsignedField(*labelFont, "background_rgb");
                }
                if (const auto *displayFormat = objectField(nodeValue, "display_format")) {
                    node.numericRadixVisible = jsonBooleanField(*displayFormat, "radix_visible");
                    node.numericDisplayFormat = jsonStringField(*displayFormat, "kind");
                    if (const auto digits = jsonNumberField(*displayFormat, "digits")) {
                        node.numericDisplayDigits = std::clamp(
                            static_cast<int>(std::lround(digits.value())),
                            1,
                            18);
                    }
                    node.numericPrecisionType = jsonStringField(*displayFormat, "precision_type");
                    node.numericHideTrailingZeros =
                        jsonBooleanField(*displayFormat, "hide_trailing_zeros");
                    node.numericExponentMultipleOfThree =
                        jsonBooleanField(*displayFormat, "exponent_multiple_of_3");
                    if (const auto width =
                            jsonNumberField(*displayFormat, "minimum_field_width")) {
                        node.numericMinimumFieldWidth = (std::max)(
                            0,
                            static_cast<int>(std::lround(width.value())));
                    }
                    node.numericPadding = jsonStringField(*displayFormat, "padding");
                    node.numericAlignment = jsonStringField(*displayFormat, "alignment");
                    node.numericSignDisplay = jsonStringField(*displayFormat, "sign");
                    node.numericLocale = jsonStringField(*displayFormat, "locale");
                    node.numericDisplayPrefix = jsonStringField(*displayFormat, "prefix");
                    node.numericDisplaySuffix = jsonStringField(*displayFormat, "suffix");
                    node.numericRadixShowPrefix =
                        jsonBooleanField(*displayFormat, "radix_show_prefix");
                    node.numericRadixUppercase =
                        jsonBooleanField(*displayFormat, "radix_uppercase");
                    if (const auto groupSize =
                            jsonNumberField(*displayFormat, "radix_group_size")) {
                        node.numericRadixGroupSize = std::clamp(
                            static_cast<int>(std::lround(groupSize.value())),
                            0,
                            8);
                    }
                    node.numericRadixInterpretation =
                        jsonStringField(*displayFormat, "radix_interpretation");
                    node.numericCustomFormatString =
                        jsonStringField(*displayFormat, "custom_format");
                }
                if (const auto *stringConstant = objectField(nodeValue, "string_constant")) {
                    node.stringDisplayStyle =
                        jsonStringField(*stringConstant, "display_style").value_or("normal");
                    if (node.stringDisplayStyle != "normal" &&
                        node.stringDisplayStyle != "backslash_codes" &&
                        node.stringDisplayStyle != "password" &&
                        node.stringDisplayStyle != "hexadecimal") {
                        node.stringDisplayStyle = "normal";
                    }
                    node.stringDisplayStyleVisible =
                        jsonBooleanField(*stringConstant, "display_style_visible").value_or(false);
                    node.stringLimitToSingleLine =
                        jsonBooleanField(*stringConstant, "limit_to_single_line").value_or(false);
                    node.stringWrapAtWordBreaks =
                        jsonBooleanField(*stringConstant, "wrap_at_word_breaks").value_or(true);
                    node.stringUpdateWhileTyping =
                        jsonBooleanField(*stringConstant, "update_while_typing").value_or(false);
                    node.stringVerticalScrollbarVisible =
                        jsonBooleanField(*stringConstant, "vertical_scrollbar_visible").value_or(false);
                    node.stringHorizontalScrollbarVisible =
                        jsonBooleanField(*stringConstant, "horizontal_scrollbar_visible").value_or(false);
                    node.stringSizeToText =
                        jsonBooleanField(*stringConstant, "size_to_text").value_or(false);
                    node.stringTextAlignment =
                        jsonStringField(*stringConstant, "text_alignment").value_or("left");
                    if (node.stringTextAlignment != "right" && node.stringTextAlignment != "center") {
                        node.stringTextAlignment = "left";
                    }
                }
                if (const auto *pathConstant = objectField(nodeValue, "path_constant")) {
                    node.pathFolderVisible =
                        jsonBooleanField(*pathConstant, "folder_visible").value_or(false);
                    node.pathBrowseButtonVisible =
                        jsonBooleanField(*pathConstant, "browse_button_visible").value_or(false);
                    node.pathScrollbarVisible =
                        jsonBooleanField(*pathConstant, "scrollbar_visible").value_or(false);
                    node.pathOverflowMode =
                        jsonStringField(*pathConstant, "overflow_mode").value_or("abbreviate");
                    if (node.pathOverflowMode != "clip") {
                        node.pathOverflowMode = "abbreviate";
                    }
                    node.pathSizeToText =
                        jsonBooleanField(*pathConstant, "size_to_text").value_or(false);
                    node.pathBrowsePrompt =
                        jsonStringField(*pathConstant, "browse_prompt").value_or(std::string{});
                    node.pathBrowsePatternLabel =
                        jsonStringField(*pathConstant, "browse_pattern_label").value_or(std::string{});
                    node.pathBrowsePattern =
                        jsonStringField(*pathConstant, "browse_pattern").value_or("*.*");
                    node.pathBrowseSelectionMode =
                        jsonStringField(*pathConstant, "browse_selection_mode").value_or("files");
                    if (node.pathBrowseSelectionMode != "folders" &&
                        node.pathBrowseSelectionMode != "files_or_folders") {
                        node.pathBrowseSelectionMode = "files";
                    }
                    node.pathBrowseExistenceMode =
                        jsonStringField(*pathConstant, "browse_existence_mode").value_or("existing");
                    if (node.pathBrowseExistenceMode != "new" &&
                        node.pathBrowseExistenceMode != "new_or_existing") {
                        node.pathBrowseExistenceMode = "existing";
                    }
                    node.pathBrowseStartPath =
                        jsonStringField(*pathConstant, "browse_start_path").value_or(std::string{});
                    node.stringHorizontalScrollbarVisible = node.pathScrollbarVisible;
                    node.stringVerticalScrollbarVisible = false;
                    node.stringSizeToText = node.pathSizeToText;
                    if (objectField(nodeValue, "string_constant") == nullptr) {
                        node.stringLimitToSingleLine = false;
                        node.stringWrapAtWordBreaks = true;
                    }
                }
                const auto *choiceConstant = objectField(nodeValue, "enum_constant");
                if (choiceConstant == nullptr) {
                    choiceConstant = objectField(nodeValue, "ring_constant");
                }
                if (choiceConstant != nullptr) {
                    const auto *choiceEntry = findFunctionCatalogEntryByDiagramNodeType(node.type);
                    const bool stringDropdown = choiceEntry != nullptr &&
                        choiceEntry->catalogId == "string.dropdown_constant";
                    node.enumDomainId = stringDropdown ? std::string{} :
                        jsonStringField(*choiceConstant, "domain_id").value_or(
                            "frog.studio.constant." + node.id);
                    node.enumSelectedValue =
                        jsonUnsignedField(*choiceConstant, "selected_value").value_or(0U);
                    node.enumTextOverflowVisible =
                        jsonBooleanField(*choiceConstant, "text_overflow_visible").value_or(true);
                    node.enumDigitalDisplayVisible =
                        jsonBooleanField(*choiceConstant, "digital_display_visible").value_or(false);
                    node.ringAllowUndefinedValues =
                        jsonBooleanField(*choiceConstant, "allow_undefined_values").value_or(false);
                    if (const auto *items = objectField(*choiceConstant, "items");
                        items != nullptr && items->type == JsonValue::Type::Array) {
                        node.enumItems.reserve((std::min)(
                            items->array.size(),
                            std::size_t{ 4096U }));
                        for (const auto &itemValue : items->array) {
                            if (itemValue.type != JsonValue::Type::Object ||
                                node.enumItems.size() >= 4096U) {
                                continue;
                            }
                            RingItem item;
                            const auto parsedLabel =
                                jsonStringField(itemValue, "label");
                            item.label = parsedLabel.value_or({});
                            item.value = jsonUnsignedField(itemValue, "value").value_or(
                                static_cast<std::uint32_t>(node.enumItems.size()));
                            item.disabled =
                                jsonBooleanField(itemValue, "disabled").value_or(false);
                            item.id = jsonStringField(itemValue, "id").value_or({});
                            item.visible =
                                jsonBooleanField(itemValue, "visible").value_or(true);
                            if (!parsedLabel.has_value()) {
                                item.label = "Item " +
                                    std::to_string(node.enumItems.size() + 1U);
                            }
                            if (item.id.empty()) {
                                item.id = "item_" +
                                    std::to_string(node.enumItems.size() + 1U);
                            }
                            node.enumItems.push_back(std::move(item));
                        }
                        normalizeRingItemIdentities(node.enumItems);
                    }
                }
                if (presetArrayConstantNode && objectField(nodeValue, "array_constant") == nullptr) {
                    node.numericArrayCellWidth = 64.0;
                    node.numericArrayCellHeight = 28.0;
                }
                if (const auto *arrayConstant = objectField(nodeValue, "array_constant")) {
                    node.numericArrayRank = std::clamp(
                        jsonUnsignedField(*arrayConstant, "rank").value_or(1U), 1U, 16U);
                    node.numericArrayShape = jsonUnsignedArrayField(*arrayConstant, "shape");
                    node.numericArrayFirstIndices = jsonUnsignedArrayField(
                        *arrayConstant, "first_indices");
                    node.numericArrayVisibleCounts = jsonUnsignedArrayField(
                        *arrayConstant, "visible_counts");
                    node.numericArrayOneDimensionalVertical = jsonBooleanField(
                        *arrayConstant,
                        "one_dimensional_vertical").value_or(false);
                    if (const auto *cellSize = objectField(*arrayConstant, "cell_size")) {
                        node.numericArrayElementGap=std::clamp(jsonNumberField(*cellSize,"element_gap").value_or(0.0),0.0,64.0);
                        node.numericArrayCellWidth = std::clamp(
                            jsonNumberField(*cellSize, "width").value_or(node.numericArrayCellWidth),
                            node.constantArrayCellBoundsAutomatic.value_or(false) ? 1.0 : 24.0,
                            32768.0);
                        node.numericArrayCellHeight = std::clamp(
                            jsonNumberField(*cellSize, "height").value_or(node.numericArrayCellHeight),
                            node.constantArrayCellBoundsAutomatic.value_or(false) ? 1.0 : 18.0,
                            node.arrayElementTemplate ? 4096.0 : 180.0);
                    }
                    node.numericArrayValues = jsonStringArrayField(*arrayConstant, "values");
                    node.numericArrayElementLabelVisible = jsonBooleanField(
                        *arrayConstant, "element_label_visible").value_or(false);
                }
                if (const auto *font = objectField(nodeValue, "font")) {
                    node.defaultTextGeometryInitialized = jsonBooleanField(*font,"geometry_initialized").value_or(false);
                    node.fontFamily = jsonStringField(*font, "family").value_or({});
                    node.textStyle = jsonStringField(*font, "style").value_or("plain");
                    node.fontPointSize = std::clamp(
                        jsonNumberField(*font, "point_size").value_or(15.0),
                        6.0,
                        144.0);
                    node.fontPointSizeUsesDefault = jsonBooleanField(
                        *font, "use_default_size").value_or(
                            std::abs(node.fontPointSize - 15.0) < 0.001 ||
                            std::abs(node.fontPointSize - 19.5) < 0.001 ||
                            ((node.type.find("string_constant") != std::string::npos || node.type == "frog.studio.string.constant") &&
                             std::abs(node.fontPointSize - 12.0) < 0.001));
                    node.textRgb = jsonUnsignedField(*font, "text_rgb");
                    node.textBackgroundRgb = jsonUnsignedField(*font, "background_rgb");
                    if (node.kind == "comment") {
                        node.commentUseThemeTextColor = jsonBooleanField(*font, "use_theme_text_color");
                    }
                }
                if (node.kind == "comment") {
                    if (const auto *runs = objectField(nodeValue, "comment_text_runs");
                        runs && runs->type == JsonValue::Type::Array) {
                        const auto utf16Length = commentTextUtf16Length(node.text);
                        for (const auto &stored : runs->array) {
                            const auto first = jsonNumberField(stored, "start_utf16");
                            const auto length = jsonNumberField(stored, "length_utf16");
                            const auto *format = objectField(stored, "format");
                            if (!first || !length || !format || format->type != JsonValue::Type::Object ||
                                !std::isfinite(*first) || !std::isfinite(*length) ||
                                *first < 0 || *length <= 0 || std::floor(*first) != *first ||
                                std::floor(*length) != *length || *first >= static_cast<double>(utf16Length)) continue;
                            CommentTextRun run;
                            run.start = static_cast<std::size_t>(*first);
                            run.length = static_cast<std::size_t>((std::min)(*length, static_cast<double>(utf16Length)));
                            run.format = commentBaseTextFormat(node);
                            run.format.fontFamily = jsonStringField(*format, "family").value_or(run.format.fontFamily);
                            run.format.textStyle = jsonStringField(*format, "style").value_or(run.format.textStyle);
                            run.format.fontPointSize = jsonNumberField(*format, "point_size").value_or(run.format.fontPointSize);
                            run.format.textRgb = jsonUnsignedField(*format, "text_rgb");
                            run.format.useThemeTextColor = jsonBooleanField(*format, "use_theme_text_color")
                                .value_or(run.format.useThemeTextColor);
                            node.commentTextRuns.push_back(std::move(run));
                        }
                        normalizeCommentTextRuns(node, utf16Length);
                    }
                    node.commentAutoSize = jsonBooleanField(nodeValue, "comment_auto_size").value_or(false);
                    if (const auto *stored = objectField(nodeValue, "comment_attachment");
                        stored && stored->type == JsonValue::Type::Object) {
                        const auto kind = diagramCommentTargetKind(jsonStringField(*stored, "kind").value_or(""));
                        if (kind) {
                            DiagramCommentAttachment attachment;
                            attachment.kind = *kind;
                            attachment.targetId = jsonStringField(*stored, "target_id").value_or("");
                            const auto coordinate = [&](std::string_view name) {
                                return stored->member(name) ? jsonNumberField(*stored, name).value_or(
                                    std::numeric_limits<double>::quiet_NaN()) : 0.5;
                            };
                            attachment.nodeAnchor = {coordinate("node_x"), coordinate("node_y")};
                            attachment.wirePosition = coordinate("wire_position");
                            if (normalizeDiagramCommentAttachment(attachment))
                                node.commentAttachment = std::move(attachment);
                        }
                    }
                    node.textVerticalArrangement = jsonStringField(
                        nodeValue,
                        "vertical_arrangement").value_or("none");
                    if (node.textVerticalArrangement != "none" &&
                        node.textVerticalArrangement != "stacked" &&
                        node.textVerticalArrangement != "clockwise" &&
                        node.textVerticalArrangement != "counterclockwise") {
                        node.textVerticalArrangement = "none";
                    }
                }
                if (const auto *layout = objectField(nodeValue, "layout")) {
                    node.layout.x = jsonNumberField(*layout, "x").value_or(node.layout.x);
                    node.layout.y = jsonNumberField(*layout, "y").value_or(node.layout.y);
                    node.layout.width = (std::max)(1.0, jsonNumberField(*layout, "width").value_or(node.layout.width));
                    node.layout.height = (std::max)(1.0, jsonNumberField(*layout, "height").value_or(node.layout.height));
                }
                if (node.numericColorBox && node.kind == "primitive" &&
                    (node.type == "frog.studio.numeric.numeric_constant" ||
                     node.type == "frog.studio.numeric.constant"))
                    node.layout = fixedColorBoxBounds(node.layout, kColorBoxConstantSide);
                if (node.kind == "static_visual") {
                    if (const auto *visualObject = objectField(nodeValue, "visual")) {
                        StaticVisual visual;
                        loadEmbeddedStaticVisual(*visualObject, node.layout, visual);
                        node.staticVisual = std::move(visual);
                    }
                }
                if (node.kind == "frog_call") {
                    if (const auto *call = objectField(nodeValue, "call")) {
                        loadedFrogCall = true;
                        node.callDocumentPath =
                            jsonStringField(*call, "document_path").value_or({});
                        node.callDocumentTitle =
                            jsonStringField(*call, "document_title").value_or({});
                        node.callIconSvgText =
                            jsonStringField(*call, "icon_svg").value_or({});
                        if (const auto *ports = objectField(*call, "ports");
                            ports != nullptr && ports->type == JsonValue::Type::Array) {
                            node.callPorts.reserve((std::min)(
                                ports->array.size(),
                                std::size_t{ 256U }));
                            for (const auto &portValue : ports->array) {
                                if (portValue.type != JsonValue::Type::Object ||
                                    node.callPorts.size() >= 256U) {
                                    continue;
                                }
                                DiagramCallPort port;
                                port.id = jsonStringField(portValue, "id").value_or({});
                                port.sourcePortId = jsonStringField(
                                    portValue,
                                    "source_port_id").value_or(port.id);
                                port.interfaceMapSlotId = jsonStringField(
                                    portValue,
                                    "interface_map_slot").value_or({});
                                port.label = jsonStringField(portValue, "label").value_or({});
                                port.valueType = jsonStringField(
                                    portValue,
                                    "value_type").value_or("unknown");
                                port.input = jsonStringField(
                                    portValue,
                                    "direction").value_or("input") != "output";
                                port.connectionRequirement = jsonStringField(
                                    portValue,
                                    "connection_requirement").value_or("recommended");
                                if (port.connectionRequirement != "required" &&
                                    port.connectionRequirement != "optional") {
                                    port.connectionRequirement = "recommended";
                                }
                                port.normalizedX = std::clamp(
                                    jsonNumberField(portValue, "normalized_x").value_or(
                                        port.input ? 0.0 : 1.0),
                                    0.0,
                                    1.0);
                                port.normalizedY = std::clamp(
                                    jsonNumberField(portValue, "normalized_y").value_or(0.5),
                                    0.0,
                                    1.0);
                                if (port.id.empty()) {
                                    continue;
                                }
                                const bool duplicate = std::any_of(
                                    node.callPorts.begin(),
                                    node.callPorts.end(),
                                    [&](const DiagramCallPort &candidate) {
                                        return candidate.id == port.id;
                                    });
                                if (!duplicate) {
                                    node.callPorts.push_back(std::move(port));
                                }
                            }
                        }
                    }
                }
                if (numericConstantNode) {
                    if (node.valueType.empty()) {
                        node.valueType = timestampConstantNode ? "timestamp" : "u8";
                    }
                    if (node.label.empty()) {
                        node.label = timestampConstantNode ? "Timestamp Constant" : "Numeric Constant";
                    }
                }
                if (node.kind == "primitive" &&
                    (node.type == "frog.studio.numeric.enum_constant" ||
                     node.type == "numeric.enum_constant" ||
                     node.type == "frog.studio.numeric.enum_array_constant" ||
                     node.type == "numeric.enum_array_constant" ||
                     node.type == "frog.studio.numeric.ring_constant" ||
                     node.type == "numeric.ring_constant" ||
                     node.type == "frog.studio.numeric.ring_array_constant" ||
                     node.type == "numeric.ring_array_constant")) {
                    const bool ringConstant =
                        node.type == "frog.studio.numeric.ring_constant" ||
                        node.type == "numeric.ring_constant" ||
                        node.type == "frog.studio.numeric.ring_array_constant" ||
                        node.type == "numeric.ring_array_constant";
                    if (node.valueType.empty()) {
                        node.valueType = ringConstant ? "u8" : "u16";
                    }
                    const bool arrayConstant =
                        node.type == "frog.studio.numeric.enum_array_constant" ||
                        node.type == "numeric.enum_array_constant" ||
                        node.type == "frog.studio.numeric.ring_array_constant" ||
                        node.type == "numeric.ring_array_constant";
                    if (node.label.empty()) {
                        node.label = arrayConstant
                            ? ringConstant
                                ? "Ring Array Constant"
                                : "Enum Array Constant"
                            : ringConstant
                                ? "Ring Constant"
                                : "Enum Constant";
                    }
                    if (node.enumItems.empty()) {
                        node.enumItems = defaultEnumItems();
                    }
                    const auto selected = std::find_if(
                        node.enumItems.begin(),
                        node.enumItems.end(),
                        [&node](const RingItem &item) {
                            return item.value == node.enumSelectedValue && !item.disabled;
                        });
                    if (selected == node.enumItems.end()) {
                        const auto firstEnabled = std::find_if(
                            node.enumItems.begin(),
                            node.enumItems.end(),
                            [](const RingItem &item) {
                                return !item.disabled && item.visible;
                            });
                        if (firstEnabled != node.enumItems.end()) {
                            node.enumSelectedValue = firstEnabled->value;
                        }
                    }
                }
                if (node.kind == "primitive" &&
                    (node.type == "frog.studio.array.array_constant" ||
                     node.type == "array.array_constant")) {
                    node.type = "frog.studio.array.array_constant";
                    node.valueType.clear();
                    node.text.clear();
                    if (node.label.empty()) {
                        node.label = "Array Constant";
                    }
                    node.numericArrayRank = std::clamp(
                        node.numericArrayRank,
                        1U,
                        16U);
                    if (node.numericArrayRank != 1U) {
                        node.numericArrayOneDimensionalVertical = false;
                    }
                    node.numericArrayShape.resize(node.numericArrayRank, 1U);
                    if (!checkedArrayElementCount(node.numericArrayShape)) {
                        if(error)*error="Array dimensions exceed 4294967295 elements.";
                        return std::nullopt;
                    }
                    node.numericArrayFirstIndices.resize(
                        node.numericArrayRank,
                        0U);
                    node.numericArrayVisibleCounts.resize(
                        node.numericArrayRank,
                        1U);
                    normalizeDraftArrayConstantVisibleCounts(node);
                    node.numericArrayCellWidth = std::clamp(
                        node.numericArrayCellWidth,
                        node.constantArrayCellBoundsAutomatic.value_or(false) ? 1.0 : 24.0,
                        32768.0);
                    node.numericArrayCellHeight = std::clamp(
                        node.numericArrayCellHeight,
                        node.constantArrayCellBoundsAutomatic.value_or(false) ? 1.0 : 18.0,
                        180.0);
                    node.numericArrayValues.clear();
                }
                if (node.kind == "primitive" &&
                    (presetArrayConstantNode ||
                     node.type == "frog.studio.numeric.numeric_array_constant" ||
                     node.type == "numeric.numeric_array_constant" ||
                     node.type == "frog.studio.numeric.enum_array_constant" ||
                     node.type == "numeric.enum_array_constant" ||
                     node.type == "frog.studio.numeric.ring_array_constant" ||
                     node.type == "numeric.ring_array_constant" ||
                     node.type == "frog.studio.string.string_array_constant" ||
                     node.type == "string.string_array_constant" ||
                     node.type == "frog.studio.path.path_array_constant" ||
                     node.type == "path.path_array_constant")) {
                    const bool stringArray =
                        (presetArrayConstantNode && node.valueType == "string") ||
                        node.type == "frog.studio.string.string_array_constant" ||
                        node.type == "string.string_array_constant";
                    const bool pathArray =
                        (presetArrayConstantNode && node.valueType == "path") ||
                        node.type == "frog.studio.path.path_array_constant" ||
                        node.type == "path.path_array_constant";
                    if (node.valueType.empty()) {
                        node.valueType = pathArray
                            ? "path"
                            : stringArray ? "string" : "u8";
                    }
                    const bool enumArray =
                        node.type == "frog.studio.numeric.enum_array_constant" ||
                        node.type == "numeric.enum_array_constant" ||
                        node.type == "frog.studio.numeric.ring_array_constant" ||
                        node.type == "numeric.ring_array_constant";
                    if (node.label.empty() && !enumArray) {
                        node.label = pathArray
                            ? "Path Array Constant"
                            : stringArray
                                ? "String Array Constant"
                                : "Numeric Array Constant";
                    }
                    node.numericArrayRank = std::clamp(node.numericArrayRank, 1U, 16U);
                    if (node.numericArrayRank != 1U) {
                        node.numericArrayOneDimensionalVertical = false;
                    }
                    if (node.numericArrayShape.empty()) {
                        node.numericArrayShape.assign(node.numericArrayRank, 1U);
                        const auto columnDimension = node.numericArrayRank - 1U;
                        const std::uint32_t visibleColumns = node.numericArrayVisibleCounts.empty() ? 1U : node.numericArrayVisibleCounts.front();
                        const std::uint32_t firstColumn = columnDimension < node.numericArrayFirstIndices.size()
                            ? node.numericArrayFirstIndices[columnDimension] : 0U;
                        node.numericArrayShape[columnDimension] = static_cast<std::uint32_t>((std::min)(
                            kMaximumArrayElements, (std::max)(std::uint64_t{1}, std::uint64_t{firstColumn} + visibleColumns)));
                        if (node.numericArrayRank >= 2U) {
                            const auto rowDimension = node.numericArrayRank - 2U;
                            const std::uint32_t visibleRows = node.numericArrayVisibleCounts.size() >= 2U
                                ? node.numericArrayVisibleCounts[1U] : 1U;
                            const std::uint32_t firstRow = rowDimension < node.numericArrayFirstIndices.size()
                                ? node.numericArrayFirstIndices[rowDimension] : 0U;
                            node.numericArrayShape[rowDimension] = static_cast<std::uint32_t>((std::min)(
                                kMaximumArrayElements, (std::max)(std::uint64_t{1}, std::uint64_t{firstRow} + visibleRows)));
                        }
                    }
                    node.numericArrayShape.resize(node.numericArrayRank, 1U);
                    if (!checkedArrayElementCount(node.numericArrayShape)) {
                        if(error)*error="Array dimensions exceed 4294967295 elements.";
                        return std::nullopt;
                    }
                    node.numericArrayFirstIndices.resize(node.numericArrayRank, 0U);
                    node.numericArrayVisibleCounts.resize(node.numericArrayRank, 1U);
                    normalizeDraftArrayConstantVisibleCounts(node);
                    node.numericArrayCellWidth = std::clamp(node.numericArrayCellWidth, node.constantArrayCellBoundsAutomatic.value_or(false) ? 1.0 : 24.0, 32768.0);
                    node.numericArrayCellHeight = std::clamp(node.numericArrayCellHeight, node.constantArrayCellBoundsAutomatic.value_or(false) ? 1.0 : 18.0, node.arrayElementTemplate ? 4096.0 : 180.0);
                    if (node.arrayElementTemplate && isNumericColorBox(*node.arrayElementTemplate))
                        node.numericArrayCellWidth = node.numericArrayCellHeight = kColorBoxConstantSide;
                    if (node.numericArrayValues.empty() && std::none_of(node.numericArrayShape.begin(), node.numericArrayShape.end(), [](auto extent) { return extent == 0; })) {
                        node.numericArrayValues.push_back(
                            enumArray
                                ? std::to_string(node.enumSelectedValue)
                                : (stringArray || pathArray)
                                    ? node.text
                                    : node.text.empty()
                                        ? node.valueType.starts_with("cluster<") ? "{}" : "0"
                                        : node.text);
                    }
                    // Modern documents store cell values independently from
                    // the optional scalar edit cache. Reconstructing that
                    // cache would author a new `text` member on Save/Open of
                    // a freshly placed Array preset. Legacy scalar-only
                    // storage still needs the historical fallback.
                    const auto *storedArray = objectField(nodeValue, "array_constant");
                    const auto *storedValues = storedArray ? objectField(*storedArray, "values") : nullptr;
                    const bool hasStoredCellValues = storedValues != nullptr &&
                        storedValues->type == JsonValue::Type::Array;
                    if (!hasStoredCellValues && node.text.empty() && !node.numericArrayValues.empty()) {
                        node.text = node.numericArrayValues.front();
                    }
                }
                const bool primitive = node.kind == "primitive" && !node.type.empty();
                if(node.type=="frog.studio.cluster.constant" && !jsonBooleanField(nodeValue,"cluster_constant_collapsed").has_value()) {
                    // Former palette capsules had no field editor or expansion state.
                    node.layout.width=(std::max)(180.0,node.layout.width);
                    node.layout.height=(std::max)(110.0,node.layout.height);
                }
                if(primitive && node.clusterConstantCollapsed &&
                    (node.type=="frog.studio.cluster.constant" || node.type=="frog.studio.error.constant") &&
                    node.layout.width==48.0 && node.layout.height==32.0) {
                    // The old collapsed viewport shrank the standard square SVG.
                    // Preserve the expanded envelope, field positions and values.
                    const auto geometry=diagramElementGeometry(DiagramElementStandard::FixedConstant);
                    node.layout.width=geometry.naturalWidth;node.layout.height=geometry.naturalHeight;
                }
                const bool widgetValue = node.kind == "widget_value" && !node.widgetId.empty();
                const bool staticVisual = node.kind == "static_visual" && node.staticVisual.has_value();
                const bool comment = node.kind == "comment" && !node.text.empty();
                const bool frogCall = node.kind == "frog_call" && loadedFrogCall;
                if (node.id.empty() ||
                    (!primitive && !widgetValue && !staticVisual && !comment && !frogCall)) {
                    continue;
                }
                // The historical private writer omitted this presentation type
                // hint. Recover only a missing member, from the linked authored
                // source schema. Explicit empty/null/unknown/conflicting members
                // are not authorization to repair the graph. Public source is
                // strictly excluded and this is not semantic validation or FIR.
                if (!publicSource && widgetValue && objectField(nodeValue, "type") == nullptr) {
                    const auto *sourceObject = document.widgets.objectById(node.widgetId);
                    if (sourceObject && sourceObject->widget &&
                        widgetSupportsDiagramTerminal(*sourceObject->widget)) {
                        node.type = diagramTerminalValueType(*sourceObject->widget);
                        document.privateTerminalHintMigrations.push_back({
                            "private.widget_terminal.type_hint.absent.v1", node.id, node.widgetId,
                            node.type, originalPosition(nodeValue.sourceBegin), originalPosition(nodeValue.sourceEnd)});
                    }
                }
                maxDiagramNodeSerial = (std::max)(maxDiagramNodeSerial, diagramNodeSerialFromId(node.id));
                document.diagramNodes.push_back(std::move(node));
            }
        }
        if (const auto *edges = objectField(*diagram, "edges");
            edges != nullptr && edges->type == JsonValue::Type::Array) {
            for (const auto &edgeValue : edges->array) {
                if (edgeValue.type != JsonValue::Type::Object) {
                    continue;
                }
                const auto *from = objectField(edgeValue, "from");
                const auto *to = objectField(edgeValue, "to");
                if (from == nullptr || to == nullptr) {
                    continue;
                }
                DiagramEdge edge;
                edge.id = jsonStringField(edgeValue, "id").value_or({});
                edge.from.nodeId = jsonStringField(*from, "node").value_or({});
                edge.from.portId = jsonStringField(*from, "port").value_or({});
                edge.to.nodeId = jsonStringField(*to, "node").value_or({});
                edge.to.portId = jsonStringField(*to, "port").value_or({});
                edge.diagramOwnerStructureId = jsonStringField(
                    edgeValue,
                    "diagram_owner_structure_id").value_or({});
                edge.diagramOwnerRegionId = jsonStringField(
                    edgeValue,
                    "diagram_owner_region_id").value_or({});
                if (const auto *label = objectField(edgeValue, "label");
                    label != nullptr && label->type == JsonValue::Type::Object) {
                    edge.label = jsonStringField(*label, "text").value_or({});
                    constexpr std::size_t maximumWireLabelBytes = 4096U;
                    if (edge.label.size() > maximumWireLabelBytes) {
                        edge.label.resize(maximumWireLabelBytes);
                    }
                    const double normalizedPosition = jsonNumberField(
                        *label,
                        "normalized_position").value_or(0.5);
                    edge.labelNormalizedPosition = std::isfinite(normalizedPosition)
                        ? std::clamp(normalizedPosition, 0.0, 1.0)
                        : 0.5;
                    const double offsetX =
                        jsonNumberField(*label, "offset_x").value_or(0.0);
                    const double offsetY =
                        jsonNumberField(*label, "offset_y").value_or(0.0);
                    edge.labelOffsetX = std::isfinite(offsetX) ? offsetX : 0.0;
                    edge.labelOffsetY = std::isfinite(offsetY) ? offsetY : 0.0;
                }
                document_internal::canonicalizeFunctionEdge(document.diagramNodes, edge);
                if (const auto *bends = objectField(edgeValue, "bends");
                    bends != nullptr && bends->type == JsonValue::Type::Array) {
                    constexpr std::size_t maxPersistedBends = 64U;
                    edge.bends.reserve((std::min)(bends->array.size(), maxPersistedBends));
                    for (const auto &bendValue : bends->array) {
                        if (edge.bends.size() >= maxPersistedBends) {
                            break;
                        }
                        if (bendValue.type != JsonValue::Type::Object) {
                            continue;
                        }
                        const auto x = jsonNumberField(bendValue, "x");
                        const auto y = jsonNumberField(bendValue, "y");
                        if (x.has_value() && y.has_value() &&
                            std::isfinite(x.value()) && std::isfinite(y.value())) {
                            edge.bends.push_back(Point{ x.value(), y.value() });
                        }
                    }
                }
                if (edge.id.empty() || edge.from.nodeId.empty() || edge.from.portId.empty() ||
                    edge.to.nodeId.empty() || edge.to.portId.empty()) {
                    continue;
                }
                maxDiagramEdgeSerial = (std::max)(
                    maxDiagramEdgeSerial,
                    diagramEdgeSerialFromId(edge.id));
                document.diagramEdges.push_back(std::move(edge));
            }
        }
        if (const auto *fragments = objectField(*diagram, "wire_fragments");
            fragments != nullptr && fragments->type == JsonValue::Type::Array) {
            constexpr std::size_t maxPersistedFragments = 4096U;
            constexpr std::size_t maxPersistedFragmentPoints = 256U;
            document.diagramWireFragments.reserve(
                (std::min)(fragments->array.size(), maxPersistedFragments));
            for (const auto &fragmentValue : fragments->array) {
                if (document.diagramWireFragments.size() >= maxPersistedFragments) {
                    break;
                }
                if (fragmentValue.type != JsonValue::Type::Object) {
                    continue;
                }
                DiagramWireFragment fragment;
                fragment.id = jsonStringField(fragmentValue, "id").value_or({});
                fragment.detachedEdgeId = jsonStringField(fragmentValue, "detached_edge_id").value_or({});
                if (const auto *label = objectField(fragmentValue, "label")) {
                    fragment.label = jsonStringField(*label, "text").value_or({});
                    constexpr std::size_t maximumWireLabelBytes = 4096U;
                    if (fragment.label.size() > maximumWireLabelBytes) fragment.label.resize(maximumWireLabelBytes);
                    const auto position = jsonNumberField(*label, "normalized_position").value_or(0.5);
                    fragment.labelNormalizedPosition = std::isfinite(position) ? std::clamp(position, 0.0, 1.0) : 0.5;
                    const auto x = jsonNumberField(*label, "offset_x").value_or(0.0);
                    const auto y = jsonNumberField(*label, "offset_y").value_or(0.0);
                    fragment.labelOffsetX = std::isfinite(x) ? x : 0.0;
                    fragment.labelOffsetY = std::isfinite(y) ? y : 0.0;
                }
                fragment.typeHint = jsonStringField(fragmentValue, "type_hint")
                    .value_or(jsonStringField(fragmentValue, "typeHint").value_or({}));
                fragment.typeHintAuthoritative = jsonBooleanField(
                    fragmentValue,
                    "type_hint_authoritative").value_or(false);
                fragment.diagramOwnerStructureId = jsonStringField(
                    fragmentValue,
                    "diagram_owner_structure_id").value_or({});
                fragment.diagramOwnerRegionId = jsonStringField(
                    fragmentValue,
                    "diagram_owner_region_id").value_or({});
                const auto loadEndpoint = [&](std::string_view name)
                    -> std::optional<DiagramEdgeEndpoint> {
                    const auto *endpointValue = objectField(fragmentValue, name);
                    if (endpointValue == nullptr || endpointValue->type != JsonValue::Type::Object) {
                        return std::nullopt;
                    }
                    DiagramEdgeEndpoint endpoint;
                    endpoint.nodeId = jsonStringField(*endpointValue, "node").value_or({});
                    endpoint.portId = jsonStringField(*endpointValue, "port").value_or({});
                    if (endpoint.nodeId.empty() || endpoint.portId.empty()) {
                        return std::nullopt;
                    }
                    document_internal::canonicalizeFunctionEndpoint(document.diagramNodes, endpoint);
                    return endpoint;
                };
                fragment.source = loadEndpoint("source");
                fragment.sink = loadEndpoint("sink");
                // A private draft may retain invalid authoring state. Preserve
                // both diagnostic anchors without converting them into an edge;
                // readiness still rejects every fragment. Public projection must
                // keep rejecting a complete source-to-sink fragment by count.
                if (publicSource && fragment.source.has_value() && fragment.sink.has_value()) {
                    continue;
                }
                const auto *points = objectField(fragmentValue, "points");
                if (points == nullptr || points->type != JsonValue::Type::Array) {
                    continue;
                }
                fragment.points.reserve(
                    (std::min)(points->array.size(), maxPersistedFragmentPoints));
                for (const auto &pointValue : points->array) {
                    if (fragment.points.size() >= maxPersistedFragmentPoints) {
                        break;
                    }
                    if (pointValue.type != JsonValue::Type::Object) {
                        continue;
                    }
                    const auto x = jsonNumberField(pointValue, "x");
                    const auto y = jsonNumberField(pointValue, "y");
                    if (x.has_value() && y.has_value() &&
                        std::isfinite(x.value()) && std::isfinite(y.value())) {
                        fragment.points.push_back({ x.value(), y.value() });
                    }
                }
                bool orthogonal = fragment.points.size() >= 2U;
                for (std::size_t pointIndex = 0U;
                     orthogonal && pointIndex + 1U < fragment.points.size();
                     ++pointIndex) {
                    const auto &first = fragment.points[pointIndex];
                    const auto &second = fragment.points[pointIndex + 1U];
                    const bool horizontal = std::abs(first.y - second.y) <= 0.001;
                    const bool vertical = std::abs(first.x - second.x) <= 0.001;
                    orthogonal = (horizontal || vertical) &&
                        !(horizontal && vertical);
                }
                if (fragment.id.empty() || !orthogonal) {
                    continue;
                }
                maxDiagramWireFragmentSerial = (std::max)(
                    maxDiagramWireFragmentSerial,
                    diagramWireFragmentSerialFromId(fragment.id));
                document.diagramWireFragments.push_back(std::move(fragment));
            }
        }
    }

    for(const auto &object:document.widgets.objects())if(object.widget) {
        auto *widget=document.widgets.widgetById(object.id);
        if(widget && !validateStoredArrayCellValues(*widget)) {
            if(error)*error="Invalid compact Array cell value in Front Panel object '"+object.id+"'.";
            return std::nullopt;
        }
    }
    for(auto &node:document.diagramNodes) {
        if(node.arrayElementTemplate && !validateStoredArrayCellValues(*node.arrayElementTemplate)) {
            if(error)*error="Invalid compact Array cell value in Diagram node '"+node.id+"'.";
            return std::nullopt;
        }
        for(const auto &cell:node.numericArrayValues) {
            if(!validateNewArrayCellValue(node.arrayElementTemplate ? &*node.arrayElementTemplate : nullptr,cell)) {
                if(error)*error="Invalid compact Array cell value in Diagram node '"+node.id+"'.";
                return std::nullopt;
            }
        }
    }
    document.nextWidgetSerial = (std::max)(1, maxSerial + 1);
    document.nextDiagramNodeSerial = (std::max)(1, maxDiagramNodeSerial + 1);
    document.nextDiagramEdgeSerial = (std::max)(1, maxDiagramEdgeSerial + 1);
    document.nextDiagramWireFragmentSerial =
        (std::max)(1, maxDiagramWireFragmentSerial + 1);
    // Missing historical graph source is distinct from an authored empty graph.
    // Preserve the private artifact and its loaded data without manufacturing
    // executable terminals from presentation. Execution and Save need an explicit
    // migration; this read adapter cannot grant that consent.
    if (!publicSource && !authoredDiagram)
        document.privateSourceAwaitingDiagramMigration =
            std::make_shared<const std::string>(originalSource);
    if (publicGraphCounts) {
        const auto importError = document_internal::validatePublicImportProjection(*publicGraphCounts, document);
        if (!importError.empty()) {
            if (error) *error = importError;
            return std::nullopt;
        }
    }
    // A private historical projection must retain incomplete authored wiring.
    // Validation diagnoses missing endpoints; Read never repairs them by deletion.
    // Public import keeps its existing strict projection checks and path below.
    if (authoredDiagram || !publicSource) {
        return document;
    }

    std::unordered_set<std::string_view> liveDiagramNodeIds;
    liveDiagramNodeIds.reserve(document.diagramNodes.size());
    for (const auto &node : document.diagramNodes) {
        liveDiagramNodeIds.emplace(node.id);
    }
    std::erase_if(document.diagramEdges, [&liveDiagramNodeIds](const DiagramEdge &edge) {
        return !liveDiagramNodeIds.contains(edge.from.nodeId) ||
            !liveDiagramNodeIds.contains(edge.to.nodeId);
    });
    std::erase_if(
        document.diagramWireFragments,
        [&liveDiagramNodeIds](const DiagramWireFragment &fragment) {
            const bool sourceMissing = fragment.source.has_value() &&
                !liveDiagramNodeIds.contains(fragment.source->nodeId);
            const bool sinkMissing = fragment.sink.has_value() &&
                !liveDiagramNodeIds.contains(fragment.sink->nodeId);
            return sourceMissing || sinkMissing;
        });
    return document;
}


} // namespace frog::native_engine
