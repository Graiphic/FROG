#pragma once

#include "frog_native_engine/FrogSourceDocument.hpp"

#include "FrogDocumentJsonValue.hpp"

#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <unordered_set>

namespace frog::native_engine::document_internal {

// This is an import-loss guard, not the public language's structural/semantic
// validator. Studio's private draft model cannot represent every public form.
[[nodiscard]] inline std::string publicImportError(std::string_view path, std::string_view reason) {
    return "Unsupported public FROG import at " + std::string(path) + ": " + std::string(reason) +
        ". No document was imported or converted; use a reader that preserves this source.";
}

[[nodiscard]] inline bool hasPublicImportContent(const JsonValue &value) {
    if (value.type == JsonValue::Type::Null) return false;
    if (value.type == JsonValue::Type::Array) return !value.array.empty();
    if (value.type == JsonValue::Type::Object) return !value.object.empty();
    return true;
}

[[nodiscard]] inline std::string validatePublicImportDiagram(const JsonValue &diagram, const std::string &path) {
    if (diagram.type != JsonValue::Type::Object) return publicImportError(path, "expected an object");
    constexpr std::array knownFields{"nodes", "edges", "wire_fragments", "zoom_percent", "viewport_origin"};
    for (const auto &[name, value] : diagram.object) {
        if (std::find(knownFields.begin(), knownFields.end(), name) == knownFields.end() && hasPublicImportContent(value))
            return publicImportError(std::string(path).append(".").append(name), "this diagram section is not retained by Studio");
    }
    for (const auto field : {"nodes", "edges", "wire_fragments"}) {
        if (const auto *values = objectField(diagram, field); values && values->type != JsonValue::Type::Array)
            return publicImportError(path + "." + field, "expected an array");
    }
    if (const auto *nodes = objectField(diagram, "nodes")) {
        for (const auto &node : nodes->array) {
            const auto id = jsonStringField(node, "id").value_or("");
            const auto nodePath = std::string(path).append(".nodes[").append(id).append("]");
            if (node.type != JsonValue::Type::Object || id.empty())
                return publicImportError(nodePath, "a node requires a nonempty ID and object shape");
            const auto kind = jsonStringField(node, "kind").value_or("primitive");
            if (kind != "primitive" && kind != "structure" && kind != "widget_value" &&
                kind != "terminal" && kind != "frog_call" && kind != "comment" && kind != "static_visual")
                return publicImportError(nodePath + ".kind", "node kind '" + kind + "' is not retained by Studio");
            // These carry executable meaning in public/profile-defined nodes;
            // the current loader has no corresponding authoritative model field.
            for (const auto field : {"interface_port", "ref", "params", "parameters", "const", "value",
                     "literal", "state", "initial", "ports", "bindings", "binding", "implementation",
                     "inputs", "outputs", "execution", "config"}) {
                if (objectField(node, field)) return publicImportError(nodePath + "." + field, "node semantics are not retained by Studio");
            }
            for (const auto field : {"kind", "type", "value_type", "valueType"}) {
                if (const auto *value = objectField(node, field); value && value->type != JsonValue::Type::String)
                    return publicImportError(nodePath + "." + field, "only string-form node types are retained by Studio");
            }
            if (const auto *profile = objectField(node, "binding_profile"); profile &&
                (profile->type != JsonValue::Type::String || profile->string != "frog.typed-binding@1"))
                return publicImportError(nodePath + ".binding_profile", "this binding profile is not supported by Studio");
            if (kind == "structure") {
                const auto structureType = jsonStringField(node, "structure_type").value_or("");
                if (nodeTypeForCanonicalStructure(structureType).empty())
                    return publicImportError(nodePath + ".structure_type", "this structure family is not retained by Studio");
                if (const auto *regions = objectField(node, "regions")) {
                    if (regions->type != JsonValue::Type::Array) return publicImportError(nodePath + ".regions", "expected an array");
                    for (const auto &region : regions->array) {
                        if (region.type != JsonValue::Type::Object) return publicImportError(nodePath + ".regions", "expected region objects");
                        if (const auto *body = objectField(region, "diagram")) {
                            auto issue = validatePublicImportDiagram(*body, nodePath + ".regions.diagram");
                            if (!issue.empty()) return issue;
                        }
                    }
                }
            }
        }
    }
    if (const auto *edges = objectField(diagram, "edges")) {
        for (const auto &edge : edges->array) {
            for (const auto field : {"type", "value_type", "coercion", "conversion", "binding"}) {
                if (objectField(edge, field)) return publicImportError(path + ".edges." + field, "edge semantics are not retained by Studio");
            }
        }
    }
    if (const auto *fragments = objectField(diagram, "wire_fragments")) {
        for (const auto &fragment : fragments->array) {
            for (const auto field : {"source", "sink"}) {
                const auto *endpoint = objectField(fragment, field);
                if (endpoint && endpoint->type != JsonValue::Type::Null &&
                    (endpoint->type != JsonValue::Type::Object || jsonStringField(*endpoint, "node").value_or("").empty() ||
                     jsonStringField(*endpoint, "port").value_or("").empty()))
                    return publicImportError(path + ".wire_fragments." + field, "loading would discard a wire endpoint");
            }
        }
    }
    return {};
}

[[nodiscard]] inline std::string validatePublicImportEnvelope(const JsonValue &root) {
    const auto *version = objectField(root, "spec_version");
    if (!version || !isCurrentPublicFrogSourceVersion(*version))
        return publicImportError("spec_version", "only source version 0.1 (string or number) is supported");
    // Legacy UI-only imports also use spec_version. Keep their established
    // envelope aliases, without treating them as canonical public validation.
    constexpr std::array knownSections{"spec_version", "metadata", "interface", "diagram", "front_panel",
        "icon", "host", "execution_policy", "ide", "cache", "title", "document", "frontPanel",
        "blockDiagram", "block_diagram", "public_interface", "interface_map", "file_icon", "draft_revision"};
    for (const auto &[name, value] : root.object) {
        if (std::find(knownSections.begin(), knownSections.end(), name) == knownSections.end() && hasPublicImportContent(value))
            return publicImportError(name, "this source section is not retained by Studio");
    }
    for (const auto field : {"interface", "public_interface"}) {
        const auto *interface = objectField(root, field);
        if (!interface) continue;
        if (interface->type != JsonValue::Type::Object) return publicImportError(field, "expected an object");
        for (const auto &[name, value] : interface->object) {
            if (name != "inputs" && name != "outputs" && name != "map" && hasPublicImportContent(value))
                return publicImportError(std::string(field) + "." + name, "this interface section is not retained by Studio");
        }
        for (const auto direction : {"inputs", "outputs"}) {
            const auto *ports = objectField(*interface, direction);
            if (ports && (ports->type != JsonValue::Type::Array || !ports->array.empty()))
                return publicImportError(std::string(field) + "." + direction,
                    "independent public interface declarations are not retained by Studio");
        }
    }
    for (const auto field : {"front_panel", "frontPanel"}) {
        if (const auto *panel = objectField(root, field)) {
            if (panel->type != JsonValue::Type::Object) return publicImportError(field, "expected an object");
            if (const auto *packages = objectField(*panel, "package_refs"); packages && hasPublicImportContent(*packages))
                return publicImportError(std::string(field) + ".package_refs", "package references are not retained by Studio");
            if (const auto *widgets = objectField(*panel, "widgets")) {
                if (widgets->type != JsonValue::Type::Array)
                    return publicImportError(std::string(field) + ".widgets", "expected an array");
                for (const auto &widget : widgets->array) {
                    const auto widgetPath = std::string(field) + ".widgets[" +
                        jsonStringField(widget, "id").value_or("") + "]";
                    if (widget.type != JsonValue::Type::Object)
                        return publicImportError(widgetPath, "expected a widget object");
                    // The public widget identifier, parts and initial value are
                    // independent authoring data. The private class_ref reader
                    // does not decode these forms; retaining graph counts alone
                    // would still admit a projection with missing widgets/data.
                    if (objectField(widget, "widget"))
                        return publicImportError(widgetPath + ".widget",
                            "public widget class declarations are not retained by Studio's private projection");
                    if (const auto *parts = objectField(widget, "parts"); parts && hasPublicImportContent(*parts))
                        return publicImportError(widgetPath + ".parts",
                            "public widget part declarations are not retained by Studio's private projection");
                    if (const auto *props = objectField(widget, "props"); props && objectField(*props, "default_value"))
                        return publicImportError(widgetPath + ".props.default_value",
                            "public widget default values are not retained by Studio's private projection");
                }
            }
        }
    }
    if (objectField(root, "frontPanel")) {
        if (const auto *alias = objectField(root, "front_panel"); alias && hasPublicImportContent(*alias))
            return publicImportError("front_panel", "multiple panel authorities would discard one source section");
    }
    for (const auto field : {"diagram", "blockDiagram"}) {
        if (const auto *diagram = objectField(root, field)) {
            auto issue = validatePublicImportDiagram(*diagram, field);
            if (!issue.empty()) return issue;
        }
    }
    if (objectField(root, "diagram")) {
        if (const auto *alias = objectField(root, "blockDiagram"); alias && hasPublicImportContent(*alias))
            return publicImportError("blockDiagram", "multiple graph authorities would discard one source section");
    }
    if (const auto *legacyDiagram = objectField(root, "block_diagram")) {
        for (const auto field : {"nodes", "edges", "wire_fragments"}) {
            if (const auto *items = objectField(*legacyDiagram, field); items && hasPublicImportContent(*items))
                return publicImportError(std::string("block_diagram.") + field, "this legacy graph spelling is not retained by Studio");
        }
    }
    return {};
}

struct PublicImportGraphCounts {
    std::size_t nodes = 0U;
    std::size_t edges = 0U;
    std::size_t wireFragments = 0U;
};

inline void countPublicImportDiagram(const JsonValue &diagram, PublicImportGraphCounts &counts) {
    if (const auto *nodes = objectField(diagram, "nodes")) {
        counts.nodes += nodes->array.size();
        for (const auto &node : nodes->array) {
            if (const auto *regions = objectField(node, "regions")) {
                for (const auto &region : regions->array) {
                    if (const auto *body = objectField(region, "diagram")) countPublicImportDiagram(*body, counts);
                }
            }
        }
    }
    if (const auto *edges = objectField(diagram, "edges")) counts.edges += edges->array.size();
    if (const auto *fragments = objectField(diagram, "wire_fragments")) counts.wireFragments += fragments->array.size();
}

[[nodiscard]] inline std::optional<PublicImportGraphCounts> publicImportGraphCounts(const JsonValue &root) {
    const auto *diagram = objectField(root, "diagram");
    if (!diagram) diagram = objectField(root, "blockDiagram");
    if (!diagram) return std::nullopt; // Legacy UI-only imports intentionally have no graph.
    PublicImportGraphCounts counts;
    countPublicImportDiagram(*diagram, counts);
    return counts;
}

[[nodiscard]] inline std::string validatePublicImportProjection(
    const PublicImportGraphCounts &counts, const FrogDraftData &document) {
    if (counts.nodes != document.diagramNodes.size())
        return publicImportError("diagram.nodes", "loading would discard or synthesize nodes");
    if (counts.edges != document.diagramEdges.size())
        return publicImportError("diagram.edges", "loading would discard edges");
    if (counts.wireFragments != document.diagramWireFragments.size())
        return publicImportError("diagram.wire_fragments", "loading would discard wire fragments");
    std::unordered_set<std::string> nodeIds;
    nodeIds.reserve(document.diagramNodes.size());
    for (const auto &node : document.diagramNodes) {
        if (!nodeIds.insert(node.id).second)
            return publicImportError("diagram.nodes", "scoped or duplicate IDs cannot be represented unambiguously by Studio's flat graph");
    }
    for (const auto &edge : document.diagramEdges) {
        if (!nodeIds.contains(edge.from.nodeId) || !nodeIds.contains(edge.to.nodeId))
            return publicImportError("diagram.edges[" + edge.id + "]", "loading would discard a missing-node connection");
    }
    for (const auto &fragment : document.diagramWireFragments) {
        if ((fragment.source && !nodeIds.contains(fragment.source->nodeId)) ||
            (fragment.sink && !nodeIds.contains(fragment.sink->nodeId)))
            return publicImportError("diagram.wire_fragments[" + fragment.id + "]", "loading would discard a missing-node connection");
    }
    return {};
}

} // namespace frog::native_engine::document_internal
