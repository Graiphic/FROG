#include "FrogDocumentJsonValue.hpp"
#include "frog_native_engine/TimedLoopConfiguration.hpp"
#include "frog_native_engine/StructureTunnelMode.hpp"
#include "FrogDocumentConditionalDisable.hpp"

#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "frog_native_engine/DiagramStructureShelfCatalog.hpp"
#include "frog_native_engine/InPlaceStructure.hpp"
#include "PortableText.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <unordered_set>
#include <utility>

namespace frog::native_engine::document_internal {
namespace {

JsonValue jsonStringValue(std::string_view text) {
    JsonValue value;
    value.type = JsonValue::Type::String;
    value.string.assign(text);
    return value;
}
std::uint64_t inPlaceSerialField(const JsonValue &value,std::string_view name,std::uint64_t fallback) {
    const auto text=jsonStringField(value,name);
    if(!text)return fallback;
    std::uint64_t result=0;const auto parsed=std::from_chars(text->data(),text->data()+text->size(),result);
    return parsed.ec==std::errc{} && parsed.ptr==text->data()+text->size()?result:fallback;
}

void collectCanonicalDiagramScope(
    const JsonValue &diagram,
    std::string_view ownerStructureId,
    std::string_view ownerRegionId,
    std::vector<JsonValue> &nodes,
    std::vector<JsonValue> &edges,
    std::vector<JsonValue> &wireFragments) {
    if (const auto *scopeNodes = objectField(diagram, "nodes");
        scopeNodes != nullptr && scopeNodes->type == JsonValue::Type::Array) {
        for (const auto &sourceNode : scopeNodes->array) {
            if (sourceNode.type != JsonValue::Type::Object) {
                continue;
            }
            JsonValue node = sourceNode;
            if (!ownerStructureId.empty()) {
                node.object["diagram_owner_structure_id"] =
                    jsonStringValue(ownerStructureId);
                node.object["diagram_owner_region_id"] =
                    jsonStringValue(ownerRegionId);
            }
            const std::string nodeId =
                jsonStringField(node, "id").value_or(std::string{});
            const std::string structureType =
                jsonStringField(node, "structure_type").value_or(std::string{});
            const bool canonicalStructure =
                jsonStringField(node, "kind").value_or(std::string{}) == "structure" &&
                (structureType == "case" || structureType == "for_loop" ||
                 structureType == "while_loop" ||
                 structureType == "timed_loop" ||
                 structureType == "event_structure" ||
                 structureType == "disabled_structure" ||
                 structureType == "conditional_disable_structure" ||
                 structureType == "sequence_structure" || structureType == "in_place_element_structure");
            nodes.push_back(node);
            if (!canonicalStructure || nodeId.empty()) {
                continue;
            }
            const auto *regions = objectField(sourceNode, "regions");
            if (regions == nullptr || regions->type != JsonValue::Type::Array) {
                continue;
            }
            const auto collectRegion = [&](const JsonValue &region) {
                if (region.type != JsonValue::Type::Object) {
                    return;
                }
                std::string regionId =
                    jsonStringField(region, "id").value_or(std::string{});
                if (structureType == "disabled_structure") {
                    if (jsonBooleanField(region, "studio_placeholder").value_or(false)) return;
                    regionId = jsonStringField(region, "studio_region_id").value_or(regionId);
                }
                if (structureType == "case") {
                    if (const auto match = jsonBooleanField(region, "match")) {
                    if (regionId.empty()) regionId = match.value()
                        ? std::string{ kCaseStructureTrueRegionId }
                        : std::string{ kCaseStructureFalseRegionId };
                    }
                } else if (structureType == "for_loop") {
                    regionId = std::string{ kForLoopBodyRegionId };
                } else if (structureType == "while_loop") {
                    regionId = std::string{ kWhileLoopBodyRegionId };
                } else if (structureType == "timed_loop") {
                    regionId = std::string{ kTimedLoopBodyRegionId };
                }
                const auto *regionDiagram = objectField(region, "diagram");
                if (regionId.empty() || regionDiagram == nullptr) {
                    return;
                }
                collectCanonicalDiagramScope(
                    *regionDiagram,
                    nodeId,
                    regionId,
                    nodes,
                    edges,
                    wireFragments);
            };
            for (const auto &region : regions->array) collectRegion(region);
            if (structureType == "disabled_structure") {
                if (const auto *extra = objectField(sourceNode, "studio_disabled_extra_regions");
                    extra && extra->type == JsonValue::Type::Array)
                    for (const auto &region : extra->array) collectRegion(region);
            }
        }
    }

    const auto collectOwnedValues = [=](
        std::string_view fieldName,
        std::vector<JsonValue> &target) {
        const auto *values = objectField(diagram, fieldName);
        if (values == nullptr || values->type != JsonValue::Type::Array) {
            return;
        }
        for (const auto &sourceValue : values->array) {
            if (sourceValue.type != JsonValue::Type::Object) {
                continue;
            }
            JsonValue value = sourceValue;
            if (!ownerStructureId.empty()) {
                value.object["diagram_owner_structure_id"] =
                    jsonStringValue(ownerStructureId);
                value.object["diagram_owner_region_id"] =
                    jsonStringValue(ownerRegionId);
            }
            target.push_back(std::move(value));
        }
    };
    collectOwnedValues("edges", edges);
    collectOwnedValues("wire_fragments", wireFragments);
}

} // namespace

const JsonValue *objectField(
    const JsonValue &value,
    std::string_view name) noexcept {
    if (value.type != JsonValue::Type::Object) {
        return nullptr;
    }
    const auto found = value.object.find(std::string(name));
    return found == value.object.end() ? nullptr : &found->second;
}

JsonValue *mutableObjectField(
    JsonValue &value,
    std::string_view name) {
    if (value.type != JsonValue::Type::Object) {
        return nullptr;
    }
    const auto found = value.object.find(std::string(name));
    return found == value.object.end() ? nullptr : &found->second;
}

std::optional<std::string> jsonStringField(
    const JsonValue &value,
    std::string_view name) {
    const auto *field = objectField(value, name);
    if (field == nullptr || field->type != JsonValue::Type::String) {
        return std::nullopt;
    }
    return field->string;
}

std::optional<double> jsonNumberField(
    const JsonValue &value,
    std::string_view name) {
    const auto *field = objectField(value, name);
    if (field == nullptr || field->type != JsonValue::Type::Number) {
        return std::nullopt;
    }
    return field->number;
}

std::optional<bool> jsonBooleanField(
    const JsonValue &value,
    std::string_view name) {
    const auto *field = objectField(value, name);
    if (field == nullptr || field->type != JsonValue::Type::Boolean) {
        return std::nullopt;
    }
    return field->boolean;
}

std::vector<std::string> jsonStringArrayField(
    const JsonValue &value,
    std::string_view name) {
    const auto *field = objectField(value, name);
    if (field == nullptr || field->type != JsonValue::Type::Array) {
        return {};
    }

    std::vector<std::string> strings;
    strings.reserve(field->array.size());
    for (const auto &item : field->array) {
        if (item.type == JsonValue::Type::String) {
            strings.push_back(item.string);
        }
    }
    return strings;
}

std::vector<std::uint32_t> jsonUnsignedArrayField(
    const JsonValue &value,
    std::string_view name) {
    const auto *field = objectField(value, name);
    if (field == nullptr || field->type != JsonValue::Type::Array) {
        return {};
    }

    std::vector<std::uint32_t> values;
    values.reserve(field->array.size());
    for (const auto &item : field->array) {
        if (item.type != JsonValue::Type::Number ||
            item.number < 0.0 ||
            item.number > static_cast<double>((std::numeric_limits<std::uint32_t>::max)())) {
            continue;
        }
        values.push_back(static_cast<std::uint32_t>(std::llround(item.number)));
    }
    return values;
}

std::optional<std::uint32_t> jsonUnsignedField(
    const JsonValue &value,
    std::string_view name) {
    const auto number = jsonNumberField(value, name);
    if (!number.has_value() || number.value() < 0.0) {
        return std::nullopt;
    }
    return static_cast<std::uint32_t>(number.value());
}

std::optional<double> parseCssPixelValue(std::string_view text) {
    const auto trimmed = std::string(
        portable_internal::trimAsciiWhitespace(
            text,
            portable_internal::kJsonWhitespace));
    if (trimmed.empty()) {
        return std::nullopt;
    }
    char *end = nullptr;
    const double value = std::strtod(trimmed.c_str(), &end);
    if (end == trimmed.c_str()) {
        return std::nullopt;
    }
    while (*end != '\0' && std::isspace(static_cast<unsigned char>(*end)) != 0) {
        ++end;
    }
    if (*end != '\0' && std::string_view(end) != "px") {
        return std::nullopt;
    }
    return value;
}

std::optional<double> jsonPixelField(
    const JsonValue &value,
    std::string_view name) {
    if (const auto number = jsonNumberField(value, name)) {
        return number;
    }
    if (const auto text = jsonStringField(value, name)) {
        return parseCssPixelValue(text.value());
    }
    return std::nullopt;
}

void flattenCanonicalStructureRegions(JsonValue &diagram) {
    if (diagram.type != JsonValue::Type::Object) {
        return;
    }
    std::vector<JsonValue> nodes;
    std::vector<JsonValue> edges;
    std::vector<JsonValue> wireFragments;
    collectCanonicalDiagramScope(diagram, {}, {}, nodes, edges, wireFragments);

    JsonValue flattenedNodes;
    flattenedNodes.type = JsonValue::Type::Array;
    flattenedNodes.array = std::move(nodes);
    diagram.object["nodes"] = std::move(flattenedNodes);

    JsonValue flattenedEdges;
    flattenedEdges.type = JsonValue::Type::Array;
    flattenedEdges.array = std::move(edges);
    diagram.object["edges"] = std::move(flattenedEdges);

    JsonValue flattenedFragments;
    flattenedFragments.type = JsonValue::Type::Array;
    flattenedFragments.array = std::move(wireFragments);
    diagram.object["wire_fragments"] = std::move(flattenedFragments);
}

std::string_view canonicalStructureTypeForNodeType(std::string_view nodeType) {
    const auto family = diagramStructureFamilyForNodeType(nodeType);
    return family.has_value()
        ? diagramStructureCanonicalType(*family)
        : std::string_view{};
}

std::string_view nodeTypeForCanonicalStructure(std::string_view structureType) {
    const auto family = diagramStructureFamilyForCanonicalType(structureType);
    return family.has_value()
        ? diagramStructureNodeType(*family)
        : std::string_view{};
}

void deserializeStructureTunnels(const JsonValue &nodeValue, DiagramNode &node) {
    const auto historyIndex = [](const JsonValue &value) -> std::size_t {
        if (!objectField(value, "shift_register_history_index")) return 0;
        // Preserve malformed persisted indices as invalid state for readiness;
        // reopening must not silently turn a fractional/negative slot into t-1.
        const auto number=jsonNumberField(value,"shift_register_history_index");
        if(!number || !std::isfinite(*number) || *number<0 ||
            *number>std::numeric_limits<std::uint32_t>::max() || std::floor(*number)!=*number)
            return std::numeric_limits<std::uint32_t>::max();
        return static_cast<std::size_t>(*number);
    };
    const JsonValue *tunnels = objectField(nodeValue, "structure_tunnels");
    if (tunnels == nullptr) {
        tunnels = objectField(nodeValue, "case_structure_tunnels");
    }
    std::vector<DiagramCaseStructureTunnel> authoringTunnels;
    if (tunnels != nullptr && tunnels->type == JsonValue::Type::Array) {
        std::unordered_set<std::string> tunnelIds;
        for (const auto &tunnelValue : tunnels->array) {
            if (tunnelValue.type != JsonValue::Type::Object ||
                authoringTunnels.size() >= 128U) {
                continue;
            }
            DiagramCaseStructureTunnel tunnel;
            tunnel.id = jsonStringField(tunnelValue, "id").value_or({});
            if (tunnel.id.empty() || !tunnelIds.insert(tunnel.id).second) {
                continue;
            }
            tunnel.valueType =
                jsonStringField(tunnelValue, "value_type").value_or("unknown");
            tunnel.side = std::string(normalizeCaseStructureTunnelSide(
                jsonStringField(tunnelValue, "side").value_or("left")));
            tunnel.normalizedPosition = clampCaseStructureTunnelNormalizedPosition(
                jsonNumberField(tunnelValue, "normalized_position").value_or(0.5));
            tunnel.outputFromStructure =
                jsonBooleanField(tunnelValue, "output_from_structure").value_or(false);
            tunnel.shiftRegisterPairId = jsonStringField(
                tunnelValue, "shift_register_pair_id").value_or("");
            tunnel.shiftRegisterHistoryIndex = historyIndex(tunnelValue);
            tunnel.useDefaultIfUnwired = jsonBooleanField(
                tunnelValue, "use_default_if_unwired").value_or(false);
            tunnel.mode = parseDiagramTunnelMode(jsonStringField(
                tunnelValue, "tunnel_mode").value_or("last_value"));
            tunnel.indexingPolicy=tunnelValue.member("indexing_policy")&&tunnelValue.member("indexing_policy")->type!=JsonValue::Type::String?DiagramTunnelIndexingPolicy::Invalid:parseDiagramTunnelIndexingPolicy(jsonStringField(tunnelValue,"indexing_policy").value_or("leading_dimension"));
            tunnel.conditional = jsonBooleanField(tunnelValue, "conditional").value_or(false);
            tunnel.sequenceFrameId = jsonStringField(tunnelValue, "studio_sequence_frame").value_or("");
            tunnel.sequenceOuterFrameId = jsonStringField(tunnelValue, "studio_sequence_outer_frame").value_or("");
            tunnel.sequenceLegacyFrameId = jsonStringField(tunnelValue, "studio_sequence_legacy_frame").value_or("");
            tunnel.sequenceLegacyEdgeId = jsonStringField(tunnelValue, "studio_sequence_legacy_edge").value_or("");
            // Keep even an invalid editor association: reopening is not a
            // silent repair. Case validation reports its conflict or missing ID.
            tunnel.linkedInputTunnelId = jsonStringField(tunnelValue, "linked_input_tunnel_id").value_or("");
            authoringTunnels.push_back(std::move(tunnel));
        }
    }

    // The public boundary is authoritative for id, type and direction.
    // structure_tunnels (and its legacy case_structure_tunnels spelling) is
    // non-semantic authoring metadata: it may only restore wall placement.
    const auto *boundary = objectField(nodeValue, "boundary");
    bool canonicalBoundaryDeclared = false;
    std::unordered_set<std::string> canonicalIds;
    const auto loadBoundary = [&](std::string_view name, bool outputBoundary) {
        if (boundary == nullptr || boundary->type != JsonValue::Type::Object) {
            return;
        }
        const auto *values = objectField(*boundary, name);
        if (values == nullptr || values->type != JsonValue::Type::Array) {
            return;
        }
        canonicalBoundaryDeclared = true;
        std::size_t ordinal = 0U;
        for (const auto &value : values->array) {
            if (value.type != JsonValue::Type::Object ||
                node.caseStructureTunnels.size() >= 128U) {
                continue;
            }
            DiagramCaseStructureTunnel tunnel;
            tunnel.id = jsonStringField(value, "id").value_or({});
            if (tunnel.id.empty() || !canonicalIds.insert(tunnel.id).second) {
                continue;
            }
            tunnel.valueType = jsonStringField(value, "type").value_or("unknown");
            tunnel.shiftRegisterPairId = jsonStringField(
                value, "shift_register_pair_id").value_or("");
            tunnel.shiftRegisterHistoryIndex = historyIndex(value);
            tunnel.useDefaultIfUnwired = outputBoundary && jsonBooleanField(
                value, "use_default_if_unwired").value_or(false);
            // Semantic boundary wins even when authoring metadata disagrees.
            tunnel.mode = parseDiagramTunnelMode(jsonStringField(
                value, "tunnel_mode").value_or("last_value"));
            tunnel.indexingPolicy=value.member("indexing_policy")&&value.member("indexing_policy")->type!=JsonValue::Type::String?DiagramTunnelIndexingPolicy::Invalid:parseDiagramTunnelIndexingPolicy(jsonStringField(value,"indexing_policy").value_or("leading_dimension"));
            tunnel.conditional = jsonBooleanField(value, "conditional").value_or(false);
            tunnel.side = outputBoundary ? "right" : "left";
            tunnel.normalizedPosition = clampCaseStructureTunnelNormalizedPosition(
                0.25 + (0.18 * static_cast<double>(ordinal++)));
            tunnel.outputFromStructure = outputBoundary;
            const auto authoring = std::find_if(
                authoringTunnels.begin(),
                authoringTunnels.end(),
                [&tunnel](const DiagramCaseStructureTunnel &candidate) {
                    return candidate.id == tunnel.id;
                });
            if (authoring != authoringTunnels.end()) {
                tunnel.side = authoring->side;
                tunnel.sequenceFrameId = authoring->sequenceFrameId;
                tunnel.sequenceOuterFrameId = authoring->sequenceOuterFrameId;
                tunnel.sequenceLegacyFrameId = authoring->sequenceLegacyFrameId;
                tunnel.sequenceLegacyEdgeId = authoring->sequenceLegacyEdgeId;
                tunnel.normalizedPosition = authoring->normalizedPosition;
                if (tunnel.shiftRegisterPairId.empty()) {
                    tunnel.shiftRegisterPairId =
                        authoring->shiftRegisterPairId;
                }
                if (!objectField(value, "shift_register_history_index"))
                    tunnel.shiftRegisterHistoryIndex = authoring->shiftRegisterHistoryIndex;
                if (!tunnel.useDefaultIfUnwired) {
                    tunnel.useDefaultIfUnwired =
                        authoring->useDefaultIfUnwired;
                }
                tunnel.linkedInputTunnelId = authoring->linkedInputTunnelId;
            }
            node.caseStructureTunnels.push_back(std::move(tunnel));
        }
    };
    loadBoundary("inputs", false);
    loadBoundary("outputs", true);
    if (canonicalBoundaryDeclared) {
        return;
    }

    // Compatibility with documents written before boundary became mandatory.
    node.caseStructureTunnels = std::move(authoringTunnels);
}

void deserializeStructureProperties(
    const JsonValue &nodeValue,
    DiagramNode &node) {
    if (node.type != "frog.studio.structures.case_structure" &&
        node.type != "frog.studio.structures.for_loop" &&
        node.type != "frog.studio.structures.while_loop" &&
        node.type != "frog.studio.structures.timed_loop" &&
        node.type != "frog.studio.structures.event_structure" &&
        node.type != "frog.studio.structures.disabled_structure" &&
        node.type != "frog.studio.structures.sequence_structure" &&
        node.type != "frog.studio.structures.in_place_element_structure" &&
        node.type !=
            "frog.studio.structures.conditional_disable_structure") {
        return;
    }
    deserializeStructureTunnels(nodeValue, node);
    if(node.type=="frog.studio.structures.in_place_element_structure") {
        node.inPlaceSemanticProfile=jsonStringField(nodeValue,"studio_in_place_profile").value_or(std::string(kInPlaceSemanticProfile));
        node.inPlaceNextPairSerial=inPlaceSerialField(nodeValue,"studio_in_place_next_pair_serial",0);
        // Studio-private text metadata; old drafts without this field remain readable.
        node.forLoopSubdiagramLabelText = jsonStringField(
            nodeValue, "studio_subdiagram_label_text").value_or("");
        // The In Place writer uses the shared presentation fields. Accept
        // the legacy Case keys only when those shared fields were absent.
        node.structureSubdiagramLabelVisible = jsonBooleanField(
            nodeValue, "structure_subdiagram_label_visible").value_or(
                jsonBooleanField(nodeValue, "case_structure_subdiagram_label_visible").value_or(false));
        node.structureSubdiagramLabelHeight = clampStructureSubdiagramLabelHeight(
            jsonNumberField(nodeValue, "structure_subdiagram_label_height").value_or(
                jsonNumberField(nodeValue, "case_structure_subdiagram_label_height").value_or(
                    kStructureSubdiagramLabelDefaultHeight)));
        node.structureSubdiagramLabelBorderRgb = jsonUnsignedField(
            nodeValue, "structure_subdiagram_label_border_rgb");
        if (!objectField(nodeValue, "label_font"))
            node.labelFontPointSize = kStructureSubdiagramLabelDefaultFontPointSize;
        if(const auto *pairs=objectField(nodeValue,"studio_in_place_pairs");pairs && pairs->type==JsonValue::Type::Array)
            for(const auto &value:pairs->array) {
                DiagramInPlacePair p;p.id=jsonStringField(value,"id").value_or("");p.kind=jsonStringField(value,"kind").value_or("");
                p.y=jsonNumberField(value,"y").value_or(48);
                const auto dimension=jsonNumberField(value,"split_dimension").value_or(0);
                p.splitDimension=dimension>=0 && dimension<=std::numeric_limits<std::uint32_t>::max() && std::floor(dimension)==dimension
                    ?static_cast<std::uint32_t>(dimension):std::numeric_limits<std::uint32_t>::max();
                p.markAsModifier=jsonBooleanField(value,"mark_as_modifier").value_or(false);
                p.allowParallelReadOnly=jsonBooleanField(value,"allow_parallel_read_only").value_or(false);
                p.nextAccessSerial=inPlaceSerialField(value,"next_access_serial",1);p.accesses.clear();
                if(const auto *accesses=objectField(value,"accesses");accesses && accesses->type==JsonValue::Type::Array)
                    for(const auto &item:accesses->array) {
                        DiagramInPlaceAccess a;a.id=jsonStringField(item,"id").value_or("");
                        if(const auto *path=objectField(item,"field_path");path && path->type==JsonValue::Type::Array)
                            for(const auto &part:path->array) if(part.type==JsonValue::Type::String) a.fieldPath.push_back(part.string);
                        p.accesses.push_back(std::move(a));
                    }
                node.inPlacePairs.push_back(std::move(p));
            }
        seedInPlaceIdentityCounters(node);
        return; // Do not replace the shared appearance with unrelated Case defaults.
    }
    if (node.type == "frog.studio.structures.sequence_structure") {
        node.sequenceFrameNumbersVisible=jsonBooleanField(nodeValue,"studio_sequence_frame_numbers_visible").value_or(true);
        node.structureSubdiagramLabelVisible=jsonBooleanField(nodeValue,"structure_subdiagram_label_visible").value_or(false);
        node.structureSubdiagramLabelHeight=clampStructureSubdiagramLabelHeight(
            jsonNumberField(nodeValue,"structure_subdiagram_label_height").value_or(kStructureSubdiagramLabelDefaultHeight));
        node.structureSubdiagramLabelBorderRgb=jsonUnsignedField(nodeValue,"structure_subdiagram_label_border_rgb");
        node.sequenceSemanticProfile=jsonStringField(nodeValue,"studio_sequence_profile").value_or("flat_sequence");
        node.sequenceSelectedFrameId=jsonStringField(nodeValue,"studio_sequence_selected_frame").value_or("frame_0");
        if (const auto *regions=objectField(nodeValue,"regions"); regions && regions->type==JsonValue::Type::Array) {
            node.sequenceFrames.clear();
            for (const auto &region:regions->array)
                node.sequenceFrames.push_back({jsonStringField(region,"id").value_or(""),
                    jsonStringField(region,"studio_title").value_or(""),
                    jsonNumberField(region,"studio_width").value_or(240.0),
                    jsonStringField(region,"studio_subdiagram_label_text").value_or("")});
        }
        return; // Do not overwrite the sequence label settings with legacy Case defaults.
    }
    if (node.type == "frog.studio.structures.case_structure") {
        if (const auto *terminals = objectField(
                nodeValue, "structure_terminals")) {
            if (const auto *selector = objectField(*terminals, "selector")) {
                node.caseStructureSelectorValueType = jsonStringField(
                    *selector, "type").value_or("bool");
                if (const auto *policy = objectField(
                        *selector, "match_policy")) {
                    node.caseStructureStringCaseInsensitive =
                        jsonStringField(*policy, "case_sensitivity")
                            .value_or("sensitive") == "insensitive";
                }
                if (const auto *items = objectField(*selector, "enum_items");
                    items != nullptr && items->type == JsonValue::Type::Array) {
                    for (std::size_t index = 0U;
                         index < items->array.size(); ++index) {
                        const auto &itemValue = items->array[index];
                        if (itemValue.type != JsonValue::Type::Object) {
                            continue;
                        }
                        RingItem item;
                        item.id = jsonStringField(itemValue, "item_id")
                            .value_or("item_" + std::to_string(index + 1U));
                        item.label = jsonStringField(itemValue, "display_name")
                            .value_or(item.id);
                        item.value = static_cast<std::uint32_t>((std::max)(
                            0.0,
                            jsonNumberField(itemValue, "ordinal")
                                .value_or(static_cast<double>(index))));
                        // Private Case item presentation uses the same absent
                        // defaults as RingItem; accept only actual JSON booleans.
                        item.disabled = jsonBooleanField(itemValue, "disabled").value_or(false);
                        item.visible = jsonBooleanField(itemValue, "visible").value_or(true);
                        node.caseStructureSelectorEnumItems.push_back(
                            std::move(item));
                    }
                }
            }
        }

        if (const auto *regions = objectField(nodeValue, "regions");
            regions != nullptr && regions->type == JsonValue::Type::Array) {
            for (const auto &regionValue : regions->array) {
                if (regionValue.type != JsonValue::Type::Object) {
                    continue;
                }
                DiagramCaseStructureRegion region;
                region.id = jsonStringField(regionValue, "id").value_or({});
                region.isDefault = jsonBooleanField(
                    regionValue, "default").value_or(false);
                region.title = jsonStringField(regionValue, "title").value_or("");
                region.draftText = jsonStringField(regionValue, "draft_text");
                if (const auto legacyMatch = jsonBooleanField(
                        regionValue, "match")) {
                    if (region.id.empty()) region.id = legacyMatch.value()
                        ? std::string{ kCaseStructureTrueRegionId }
                        : std::string{ kCaseStructureFalseRegionId };
                    region.matches.push_back(DiagramCaseStructureMatcher{
                        "boolean_exact",
                        legacyMatch.value() ? "true" : "false" });
                } else if (const auto legacyString = jsonStringField(regionValue, "match")) {
                    region.matches.push_back({"string_exact", *legacyString});
                } else if (const auto *matches = objectField(
                               regionValue, "matches");
                           matches != nullptr &&
                           matches->type == JsonValue::Type::Array) {
                    for (const auto &matcherValue : matches->array) {
                        if (matcherValue.type != JsonValue::Type::Object) {
                            continue;
                        }
                        DiagramCaseStructureMatcher matcher;
                        matcher.kind = jsonStringField(matcherValue, "kind")
                            .value_or({});
                        if (matcher.kind == "enum_item") {
                            matcher.value = jsonStringField(
                                matcherValue, "item").value_or({});
                        } else if (const auto text = jsonStringField(
                                       matcherValue, "value")) {
                            matcher.value = text.value();
                        } else if (const auto boolean = jsonBooleanField(
                                       matcherValue, "value")) {
                            matcher.value = boolean.value() ? "true" : "false";
                        } else if (const auto *numberValue = objectField(
                                       matcherValue, "value");
                                   numberValue != nullptr &&
                                   numberValue->type == JsonValue::Type::Number) {
                            matcher.value = !numberValue->string.empty()
                                ? numberValue->string
                                : std::to_string(static_cast<std::int64_t>(
                                      std::llround(numberValue->number)));
                        }
                        matcher.lower = jsonStringField(matcherValue, "lower");
                        matcher.upper = jsonStringField(matcherValue, "upper");
                        region.matches.push_back(std::move(matcher));
                    }
                }
                if (!region.id.empty()) {
                    if (!jsonStringField(regionValue, "title"))
                        region.title = jsonStringField(nodeValue,
                            region.id == kCaseStructureTrueRegionId ? "case_structure_true_subdiagram_label_text" :
                            region.id == kCaseStructureFalseRegionId ? "case_structure_false_subdiagram_label_text" : "")
                            .value_or("");
                    node.caseStructureRegions.push_back(std::move(region));
                }
            }
        }
        node.caseStructureVisibleRegionId = jsonStringField(
            nodeValue, "case_structure_visible_region_id").value_or({});
        const auto radix = static_cast<int>(jsonNumberField(nodeValue, "case_structure_radix").value_or(10));
        node.caseStructureRadix = radix == 2 || radix == 8 || radix == 16 ? radix : 10;
    }
    if (node.type == "frog.studio.structures.for_loop") {
        node.forLoopConditionalTerminal = jsonBooleanField(nodeValue,
            "for_loop_conditional_terminal").value_or(false);
        node.whileLoopStopIfTrue = jsonBooleanField(nodeValue,
            "for_loop_stop_if_true").value_or(true);
        node.whileLoopConditionNormalizedX = clampWhileLoopTerminalNormalizedPosition(
            jsonNumberField(nodeValue, "for_loop_condition_normalized_x").value_or(0.92));
        node.whileLoopConditionNormalizedY = clampWhileLoopTerminalNormalizedPosition(
            jsonNumberField(nodeValue, "for_loop_condition_normalized_y").value_or(0.90));
        if (objectField(nodeValue, "label_font") == nullptr) {
            node.labelFontPointSize =
                kStructureSubdiagramLabelDefaultFontPointSize;
        }
        node.forLoopIterationNormalizedX =
            clampForLoopTerminalNormalizedPosition(jsonNumberField(
                nodeValue, "for_loop_iteration_normalized_x").value_or(0.02));
        node.forLoopIterationNormalizedY =
            clampForLoopTerminalNormalizedPosition(jsonNumberField(
                nodeValue, "for_loop_iteration_normalized_y").value_or(0.98));
        node.structureSubdiagramLabelVisible = jsonBooleanField(
            nodeValue, "for_loop_subdiagram_label_visible").value_or(false);
        node.forLoopSubdiagramLabelText = jsonStringField(
            nodeValue, "for_loop_subdiagram_label_text").value_or("");
        node.structureSubdiagramLabelHeight =
            clampStructureSubdiagramLabelHeight(jsonNumberField(
                nodeValue, "for_loop_subdiagram_label_height").value_or(
                    kStructureSubdiagramLabelDefaultHeight));
        node.structureSubdiagramLabelBorderRgb = jsonUnsignedField(
            nodeValue, "structure_subdiagram_label_border_rgb");
        node.forLoopBackgroundRgb =
            jsonUnsignedField(nodeValue, "for_loop_background_rgb");
        node.forLoopBackgroundOpacityPercent = std::clamp(static_cast<int>(
            jsonNumberField(nodeValue, "for_loop_background_opacity_percent")
                .value_or(100)), 0, 100);
        node.forLoopSubframeRgb =
            jsonUnsignedField(nodeValue, "for_loop_subframe_rgb");
        node.forLoopSubframeOpacityPercent = std::clamp(static_cast<int>(
            jsonNumberField(nodeValue, "for_loop_subframe_opacity_percent")
                .value_or(100)), 0, 100);
        return;
    }
    if (node.type == "frog.studio.structures.while_loop") {
        if (objectField(nodeValue, "label_font") == nullptr) {
            node.labelFontPointSize =
                kStructureSubdiagramLabelDefaultFontPointSize;
        }
        node.whileLoopIterationNormalizedX =
            clampWhileLoopTerminalNormalizedPosition(jsonNumberField(
                nodeValue, "while_loop_iteration_normalized_x").value_or(0.08));
        node.whileLoopIterationNormalizedY =
            clampWhileLoopTerminalNormalizedPosition(jsonNumberField(
                nodeValue, "while_loop_iteration_normalized_y").value_or(0.90));
        node.whileLoopConditionNormalizedX =
            clampWhileLoopTerminalNormalizedPosition(jsonNumberField(
                nodeValue, "while_loop_condition_normalized_x").value_or(0.92));
        node.whileLoopConditionNormalizedY =
            clampWhileLoopTerminalNormalizedPosition(jsonNumberField(
                nodeValue, "while_loop_condition_normalized_y").value_or(0.90));
        node.whileLoopIterationVisible = jsonBooleanField(
            nodeValue, "while_loop_iteration_visible").value_or(true);
        node.whileLoopConditionVisible = jsonBooleanField(
            nodeValue, "while_loop_condition_visible").value_or(true);
        node.whileLoopStopIfTrue = jsonBooleanField(
            nodeValue, "while_loop_stop_if_true").value_or(true);
        node.structureSubdiagramLabelVisible = jsonBooleanField(
            nodeValue, "while_loop_subdiagram_label_visible").value_or(false);
        node.forLoopSubdiagramLabelText = jsonStringField(
            nodeValue, "while_loop_subdiagram_label_text").value_or("");
        node.structureSubdiagramLabelHeight =
            clampStructureSubdiagramLabelHeight(jsonNumberField(
                nodeValue, "while_loop_subdiagram_label_height").value_or(
                    kStructureSubdiagramLabelDefaultHeight));
        node.structureSubdiagramLabelBorderRgb = jsonUnsignedField(
            nodeValue, "structure_subdiagram_label_border_rgb");
        node.whileLoopBackgroundRgb =
            jsonUnsignedField(nodeValue, "while_loop_background_rgb");
        node.whileLoopBackgroundOpacityPercent = std::clamp(static_cast<int>(
            jsonNumberField(nodeValue, "while_loop_background_opacity_percent")
                .value_or(100)), 0, 100);
        node.whileLoopSubframeRgb =
            jsonUnsignedField(nodeValue, "while_loop_subframe_rgb");
        node.whileLoopSubframeOpacityPercent = std::clamp(static_cast<int>(
            jsonNumberField(nodeValue, "while_loop_subframe_opacity_percent")
                .value_or(100)), 0, 100);
        return;
    }
    if (node.type == "frog.studio.structures.timed_loop") {
        node.structureSubdiagramLabelVisible = jsonBooleanField(
            nodeValue, "structure_subdiagram_label_visible").value_or(false);
        node.structureSubdiagramLabelHeight = clampStructureSubdiagramLabelHeight(
            jsonNumberField(nodeValue, "structure_subdiagram_label_height")
                .value_or(kStructureSubdiagramLabelDefaultHeight));
        node.structureSubdiagramLabelBorderRgb = jsonUnsignedField(
            nodeValue, "structure_subdiagram_label_border_rgb");
        node.forLoopSubdiagramLabelText = jsonStringField(
            nodeValue, "timed_loop_subdiagram_label_text").value_or("");
        if (!objectField(nodeValue, "label_font"))
            node.labelFontPointSize = kStructureSubdiagramLabelDefaultFontPointSize;
        const auto position = [&](std::string_view name, double fallback) {
            return clampWhileLoopTerminalNormalizedPosition(
                jsonNumberField(nodeValue, name).value_or(fallback));
        };
        const auto rows = [&](std::string_view name, std::uint32_t fallback,
                              std::uint32_t maximum) {
            return clampTimedLoopVisibleRowCount(static_cast<std::uint32_t>((std::max)(
                1.0, jsonNumberField(nodeValue, name).value_or(fallback))), maximum);
        };
        node.timedLoopIterationNormalizedX = position("timed_loop_iteration_normalized_x", 0.08);
        node.timedLoopIterationNormalizedY = position("timed_loop_iteration_normalized_y", 0.90);
        node.timedLoopConditionNormalizedX = position("timed_loop_condition_normalized_x", 0.92);
        node.timedLoopConditionNormalizedY = position("timed_loop_condition_normalized_y", 0.90);
        const bool legacyInitialRows = jsonNumberField(
            nodeValue, "timed_loop_initial_visible_row_count").has_value();
        const bool legacyFeedbackRows = jsonNumberField(
            nodeValue, "timed_loop_feedback_visible_row_count").has_value();
        const bool legacyUpdateRows = jsonNumberField(
            nodeValue, "timed_loop_update_visible_row_count").has_value();
        const bool legacyResultRows = jsonNumberField(
            nodeValue, "timed_loop_result_visible_row_count").has_value();
        node.timedLoopInitialVisibleRowCount = rows(
            "timed_loop_initial_visible_row_count", 4U,
            diagramTimedLoopMaximumVisibleRows(
                DiagramTimedLoopTimingNodeRole::Initial));
        node.timedLoopFeedbackVisibleRowCount = rows(
            "timed_loop_feedback_visible_row_count", 1U,
            diagramTimedLoopMaximumVisibleRows(
                DiagramTimedLoopTimingNodeRole::Feedback));
        node.timedLoopUpdateVisibleRowCount = rows(
            "timed_loop_update_visible_row_count", 1U,
            diagramTimedLoopMaximumVisibleRows(
                DiagramTimedLoopTimingNodeRole::Update));
        node.timedLoopResultVisibleRowCount = rows(
            "timed_loop_result_visible_row_count", 1U,
            diagramTimedLoopMaximumVisibleRows(
                DiagramTimedLoopTimingNodeRole::Result));
        const auto loadFields = [&](std::string_view name,
                                    std::vector<std::string> &target,
                                    bool legacyCount,
                                    std::uint32_t count,
                                    std::span<const DiagramStructureShelfFieldSpec>
                                        legacyOrder) {
            auto stored = jsonStringArrayField(nodeValue, name);
            if (!stored.empty()) {
                target = std::move(stored);
            } else if (legacyCount) {
                target.clear();
                for (const auto &field : legacyOrder) {
                    if (target.size() >= count) break;
                    target.emplace_back(field.id);
                }
            }
            if (target.empty()) target.emplace_back(legacyOrder.front().id);
        };
        loadFields("timed_loop_initial_fields", node.timedLoopInitialFields,
            legacyInitialRows, node.timedLoopInitialVisibleRowCount,
            diagramTimedLoopTimingFields(DiagramTimedLoopTimingNodeRole::Initial));
        loadFields("timed_loop_feedback_fields", node.timedLoopFeedbackFields,
            legacyFeedbackRows, node.timedLoopFeedbackVisibleRowCount,
            diagramTimedLoopTimingFields(DiagramTimedLoopTimingNodeRole::Feedback));
        loadFields("timed_loop_update_fields", node.timedLoopUpdateFields,
            legacyUpdateRows, node.timedLoopUpdateVisibleRowCount,
            diagramTimedLoopTimingFields(DiagramTimedLoopTimingNodeRole::Update));
        loadFields("timed_loop_result_fields", node.timedLoopResultFields,
            legacyResultRows, node.timedLoopResultVisibleRowCount,
            diagramTimedLoopTimingFields(DiagramTimedLoopTimingNodeRole::Result));
        node.timedLoopInitialVisibleRowCount = static_cast<std::uint32_t>(
            (std::min)(node.timedLoopInitialFields.size(),
                diagramTimedLoopTimingFields(
                    DiagramTimedLoopTimingNodeRole::Initial).size()));
        node.timedLoopFeedbackVisibleRowCount = static_cast<std::uint32_t>(
            (std::min)(node.timedLoopFeedbackFields.size(),
                diagramTimedLoopTimingFields(
                    DiagramTimedLoopTimingNodeRole::Feedback).size()));
        node.timedLoopUpdateVisibleRowCount = static_cast<std::uint32_t>(
            (std::min)(node.timedLoopUpdateFields.size(),
                diagramTimedLoopTimingFields(
                    DiagramTimedLoopTimingNodeRole::Update).size()));
        node.timedLoopResultVisibleRowCount = static_cast<std::uint32_t>(
            (std::min)(node.timedLoopResultFields.size(),
                diagramTimedLoopTimingFields(
                    DiagramTimedLoopTimingNodeRole::Result).size()));
        node.timedLoopInitialTimingNodeNormalizedY =
            clampEventStructureDataNodeNormalizedY(jsonNumberField(nodeValue,
                "timed_loop_initial_timing_node_normalized_y").value_or(0.0));
        node.timedLoopFeedbackTimingNodeNormalizedY =
            clampEventStructureDataNodeNormalizedY(jsonNumberField(nodeValue,
                "timed_loop_feedback_timing_node_normalized_y").value_or(0.0));
        node.timedLoopUpdateTimingNodeNormalizedY =
            clampEventStructureDataNodeNormalizedY(jsonNumberField(nodeValue,
                "timed_loop_update_timing_node_normalized_y").value_or(0.0));
        node.timedLoopResultTimingNodeNormalizedY =
            clampEventStructureDataNodeNormalizedY(jsonNumberField(nodeValue,
                "timed_loop_result_timing_node_normalized_y").value_or(0.0));
        node.timedLoopBackgroundRgb = jsonUnsignedField(nodeValue, "timed_loop_background_rgb");
        node.timedLoopBackgroundOpacityPercent = std::clamp(static_cast<int>(jsonNumberField(
            nodeValue, "timed_loop_background_opacity_percent").value_or(100)), 0, 100);
        node.timedLoopSubframeRgb = jsonUnsignedField(nodeValue, "timed_loop_subframe_rgb");
        node.timedLoopSubframeOpacityPercent = std::clamp(static_cast<int>(jsonNumberField(
            nodeValue, "timed_loop_subframe_opacity_percent").value_or(100)), 0, 100);
        if(const auto *schedule=objectField(nodeValue,"timed_loop_schedule"))
            node.timedLoopSchedule=timed_loop::readSchedule(*schedule);
        if(const auto *options=objectField(nodeValue,"studio_timed_loop_options"))
            node.timedLoopOptions=timed_loop::readOptions(*options);
        return;
    }
    if (node.type == "frog.studio.structures.event_structure") {
        node.eventDynamicTerminalsVisible = jsonBooleanField(
            nodeValue, "studio_event_dynamic_terminals_visible").value_or(false);
        if (const auto position=jsonNumberField(nodeValue,"studio_event_dynamic_terminals_normalized_y"))
            node.eventDynamicTerminalsNormalizedY=clampEventStructureDataNodeNormalizedY(*position);
        node.structureSubdiagramLabelVisible = jsonBooleanField(
            nodeValue, "structure_subdiagram_label_visible").value_or(false);
        node.structureSubdiagramLabelHeight = clampStructureSubdiagramLabelHeight(jsonNumberField(
            nodeValue, "structure_subdiagram_label_height").value_or(kStructureSubdiagramLabelDefaultHeight));
        node.structureSubdiagramLabelBorderRgb = jsonUnsignedField(nodeValue, "structure_subdiagram_label_border_rgb");
        if (!objectField(nodeValue, "label_font")) node.labelFontPointSize = kStructureSubdiagramLabelDefaultFontPointSize;
        node.eventStructureVisibleCaseIndex = static_cast<std::size_t>((std::max)(
            0.0, jsonNumberField(nodeValue,
                "event_structure_visible_case_index").value_or(0.0)));
        node.eventStructureDataNodeNormalizedY =
            clampEventStructureDataNodeNormalizedY(jsonNumberField(
                nodeValue, "event_structure_data_node_normalized_y").value_or(0.5));
        node.eventStructureResponseNodeNormalizedY =
            clampEventStructureDataNodeNormalizedY(jsonNumberField(
                nodeValue, "studio_event_response_node_normalized_y").value_or(0.5));
        node.eventStructureBackgroundRgb =
            jsonUnsignedField(nodeValue, "event_structure_background_rgb");
        node.eventStructureBackgroundOpacityPercent = std::clamp(static_cast<int>(
            jsonNumberField(nodeValue, "event_structure_background_opacity_percent")
                .value_or(100)), 0, 100);
        node.eventStructureSubframeRgb =
            jsonUnsignedField(nodeValue, "event_structure_subframe_rgb");
        node.eventStructureSubframeOpacityPercent = std::clamp(static_cast<int>(
            jsonNumberField(nodeValue, "event_structure_subframe_opacity_percent")
                .value_or(100)), 0, 100);
        node.eventStructureCases.clear();
        if (const auto *regions = objectField(nodeValue, "regions");
            regions != nullptr && regions->type == JsonValue::Type::Array) {
            for (const auto &region : regions->array) {
                if (region.type != JsonValue::Type::Object ||
                    node.eventStructureCases.size() >= 128U) {
                    continue;
                }
                DiagramEventCase eventCase;
                eventCase.id = jsonStringField(region, "id").value_or({});
                if (eventCase.id.empty()) continue;
                eventCase.label = jsonStringField(region, "event_label")
                    .value_or(eventCase.id);
                eventCase.subdiagramLabelText = jsonStringField(region, "subdiagram_label_text").value_or("");
                if (const auto y = jsonNumberField(region, "studio_event_data_node_normalized_y"))
                    eventCase.eventDataNodeNormalizedY = clampEventStructureDataNodeNormalizedY(*y);
                eventCase.eventDataNodeVisible = jsonBooleanField(region, "studio_event_data_node_visible").value_or(true);
                if (const auto *event = objectField(region, "event");
                    event != nullptr && event->type == JsonValue::Type::Object) {
                    eventCase.eventKind = jsonStringField(*event, "kind")
                        .value_or(eventCase.id == kEventStructureTimeoutRegionId
                            ? "timeout"
                            : "user");
                    eventCase.eventSource =
                        jsonStringField(*event, "source").value_or({});
                    if (const auto *bindings = objectField(*event, "studio_bindings");
                        bindings && bindings->type == JsonValue::Type::Array) {
                        for (const auto &binding : bindings->array) {
                            if (binding.type != JsonValue::Type::Object) continue;
                            eventCase.eventBindings.push_back({
                                jsonStringField(binding, "kind").value_or(""),
                                jsonStringField(binding, "source").value_or(""),
                                jsonStringField(binding, "source_scope").value_or("")});
                        }
                        if (!eventCase.eventBindings.empty()) {
                            eventCase.eventKind = eventCase.eventBindings.front().kind;
                            eventCase.eventSource = eventCase.eventBindings.front().source;
                        }
                    }
                } else {
                    // Legacy Event Structure documents only carried a region
                    // id and label. Keep them readable while all new writes
                    // emit the public event descriptor.
                    eventCase.eventKind =
                        eventCase.id == kEventStructureTimeoutRegionId
                            ? "timeout"
                            : "user";
                }
                eventCase.virtualKeyEnum = jsonStringField(region, "studio_event_vkey_encoding").value_or("") == "enum-v1";
                eventCase.eventDataFields = jsonStringArrayField(
                    region, "event_data_fields");
                if (eventCase.eventDataFields.empty()) {
                    for (const auto &field : diagramEventStructureDataFields()) {
                        eventCase.eventDataFields.emplace_back(field.id);
                    }
                }
                // Presence and [] are different authoring states. Preserve
                // unknown/duplicate IDs as file data; the resolver diagnoses
                // unavailable or repeated response selections explicitly.
                if (const auto *selection = objectField(region, "studio_event_response_fields");
                    selection && selection->type == JsonValue::Type::Array)
                    eventCase.eventResponseFields = jsonStringArrayField(
                        region, "studio_event_response_fields");
                node.eventStructureCases.push_back(std::move(eventCase));
            }
        }
        if (node.eventStructureCases.empty()) {
            node.eventStructureCases.push_back(DiagramEventCase{});
        }
        node.eventStructureVisibleCaseIndex = (std::min)(
            node.eventStructureVisibleCaseIndex,
            node.eventStructureCases.size() - 1U);
        return;
    }
    if (node.type == "frog.studio.structures.disabled_structure") {
        node.structureSubdiagramLabelVisible = jsonBooleanField(
            nodeValue, "structure_subdiagram_label_visible").value_or(false);
        node.structureSubdiagramLabelHeight = clampStructureSubdiagramLabelHeight(jsonNumberField(
            nodeValue, "structure_subdiagram_label_height").value_or(kStructureSubdiagramLabelDefaultHeight));
        node.structureSubdiagramLabelBorderRgb = jsonUnsignedField(nodeValue, "structure_subdiagram_label_border_rgb");
        if (!objectField(nodeValue, "label_font")) node.labelFontPointSize = kStructureSubdiagramLabelDefaultFontPointSize;
        node.disabledStructureEnabledVisible = jsonBooleanField(
            nodeValue, "disabled_structure_enabled_visible").value_or(true);
        node.disabledStructureVisibleRegionId = jsonStringField(nodeValue,
            "disabled_structure_visible_region_id").value_or(std::string{});
        if (const auto *regions = objectField(nodeValue, "studio_disabled_regions");
            regions && regions->type == JsonValue::Type::Array) {
            std::unordered_set<std::string> ids;
            bool enabledSeen = false;
            for (const auto &region : regions->array) {
                auto id = jsonStringField(region, "id").value_or(std::string{});
                if (id.empty() || !ids.insert(id).second) continue;
                const bool enabled = jsonBooleanField(region, "enabled").value_or(false) && !enabledSeen;
                enabledSeen = enabledSeen || enabled;
                node.disabledStructureRegions.push_back({std::move(id), enabled,
                    jsonStringField(region, "subdiagram_label_text").value_or("")});
            }
        }
        node.disabledStructureBackgroundRgb =
            jsonUnsignedField(nodeValue, "disabled_structure_background_rgb");
        node.disabledStructureBackgroundOpacityPercent = std::clamp(
            static_cast<int>(jsonNumberField(nodeValue,
                "disabled_structure_background_opacity_percent").value_or(100)),
            0, 100);
        node.disabledStructureSubframeRgb =
            jsonUnsignedField(nodeValue, "disabled_structure_subframe_rgb");
        node.disabledStructureSubframeOpacityPercent = std::clamp(
            static_cast<int>(jsonNumberField(nodeValue,
                "disabled_structure_subframe_opacity_percent").value_or(100)),
            0, 100);
        node.disabledStructureVeilRgb =
            jsonUnsignedField(nodeValue, "disabled_structure_veil_rgb");
        node.disabledStructureVeilOpacityPercent = std::clamp(
            static_cast<int>(jsonNumberField(nodeValue,
                "disabled_structure_veil_opacity_percent").value_or(50)),
            0, 100);
        return;
    }
    if (node.type ==
        "frog.studio.structures.conditional_disable_structure") {
        node.structureSubdiagramLabelVisible = jsonBooleanField(
            nodeValue, "structure_subdiagram_label_visible").value_or(false);
        node.structureSubdiagramLabelHeight = clampStructureSubdiagramLabelHeight(jsonNumberField(
            nodeValue, "structure_subdiagram_label_height").value_or(kStructureSubdiagramLabelDefaultHeight));
        node.structureSubdiagramLabelBorderRgb = jsonUnsignedField(nodeValue, "structure_subdiagram_label_border_rgb");
        if (!objectField(nodeValue, "label_font")) node.labelFontPointSize = kStructureSubdiagramLabelDefaultFontPointSize;
        readConditionalDisableCases(nodeValue, node);
        node.conditionalDisableVisibleCaseIndex = (std::min)(
            static_cast<std::size_t>((std::max)(0.0,
                jsonNumberField(nodeValue,
                    "conditional_disable_visible_case_index")
                        .value_or(0.0))),
            node.conditionalDisableCases.empty() ? 0U : node.conditionalDisableCases.size() - 1U);
        node.conditionalDisableBackgroundRgb = jsonUnsignedField(
            nodeValue, "conditional_disable_background_rgb");
        node.conditionalDisableBackgroundOpacityPercent = std::clamp(
            static_cast<int>(jsonNumberField(nodeValue,
                "conditional_disable_background_opacity_percent")
                    .value_or(100)),
            0, 100);
        node.conditionalDisableSubframeRgb = jsonUnsignedField(
            nodeValue, "conditional_disable_subframe_rgb");
        node.conditionalDisableSubframeOpacityPercent = std::clamp(
            static_cast<int>(jsonNumberField(nodeValue,
                "conditional_disable_subframe_opacity_percent")
                    .value_or(100)),
            0, 100);
        node.conditionalDisableVeilRgb = jsonUnsignedField(
            nodeValue, "conditional_disable_veil_rgb");
        node.conditionalDisableVeilOpacityPercent = std::clamp(
            static_cast<int>(jsonNumberField(nodeValue,
                "conditional_disable_veil_opacity_percent")
                    .value_or(30)),
            0, 100);
        return;
    }
    if (objectField(nodeValue, "label_font") == nullptr) {
        node.labelFontPointSize =
            kStructureSubdiagramLabelDefaultFontPointSize;
    }
    node.caseStructureTrueVisible = jsonBooleanField(
        nodeValue,
        "case_structure_true_visible").value_or(
            jsonBooleanField(nodeValue, "caseStructureTrueVisible").value_or(true));
    if (node.caseStructureVisibleRegionId.empty()) {
        node.caseStructureVisibleRegionId = node.caseStructureTrueVisible
            ? std::string{ kCaseStructureTrueRegionId }
            : std::string{ kCaseStructureFalseRegionId };
    } else {
        node.caseStructureTrueVisible =
            node.caseStructureVisibleRegionId == kCaseStructureTrueRegionId;
    }
    node.caseStructureSelectorNormalizedY =
        clampCaseStructureSelectorNormalizedY(
            jsonNumberField(
                nodeValue,
                "case_structure_selector_normalized_y").value_or(
                    jsonNumberField(
                        nodeValue,
                        "caseStructureSelectorNormalizedY").value_or(0.5)));
    node.caseStructureManualOrder = jsonBooleanField(nodeValue, "case_structure_manual_order").value_or(false);
    node.structureSubdiagramLabelVisible = jsonBooleanField(
        nodeValue, "case_structure_subdiagram_label_visible").value_or(false);
    node.caseStructureTrueSubdiagramLabelText = jsonStringField(
        nodeValue, "case_structure_true_subdiagram_label_text").value_or("");
    node.caseStructureFalseSubdiagramLabelText = jsonStringField(
        nodeValue, "case_structure_false_subdiagram_label_text").value_or("");
    node.structureSubdiagramLabelHeight =
        clampStructureSubdiagramLabelHeight(jsonNumberField(
            nodeValue, "case_structure_subdiagram_label_height").value_or(
                kStructureSubdiagramLabelDefaultHeight));
    node.structureSubdiagramLabelBorderRgb = jsonUnsignedField(
        nodeValue, "structure_subdiagram_label_border_rgb");
    node.caseStructureTrueBackgroundRgb =
        jsonUnsignedField(nodeValue, "case_structure_true_background_rgb");
    node.caseStructureTrueBackgroundOpacityPercent = std::clamp(
        static_cast<int>(jsonNumberField(
            nodeValue,
            "case_structure_true_background_opacity_percent").value_or(100)),
        0,
        100);
    node.caseStructureTrueSubframeRgb =
        jsonUnsignedField(nodeValue, "case_structure_true_subframe_rgb");
    node.caseStructureTrueSubframeOpacityPercent = std::clamp(
        static_cast<int>(jsonNumberField(
            nodeValue,
            "case_structure_true_subframe_opacity_percent").value_or(100)),
        0,
        100);
    node.caseStructureFalseBackgroundRgb =
        jsonUnsignedField(nodeValue, "case_structure_false_background_rgb");
    node.caseStructureFalseBackgroundOpacityPercent = std::clamp(
        static_cast<int>(jsonNumberField(
            nodeValue,
            "case_structure_false_background_opacity_percent").value_or(100)),
        0,
        100);
    node.caseStructureFalseSubframeRgb =
        jsonUnsignedField(nodeValue, "case_structure_false_subframe_rgb");
    node.caseStructureFalseSubframeOpacityPercent = std::clamp(
        static_cast<int>(jsonNumberField(
            nodeValue,
            "case_structure_false_subframe_opacity_percent").value_or(100)),
        0,
        100);
}

} // namespace frog::native_engine::document_internal
