#pragma once

#include "frog_native_engine/FrogDraftData.hpp"

#include <algorithm>
#include <cmath>
#include <optional>
#include <span>
#include <string_view>

namespace frog::native_engine {

inline std::string_view diagramCommentTargetKindToken(DiagramCommentTargetKind kind) noexcept {
    switch (kind) {
    case DiagramCommentTargetKind::Node: return "node";
    case DiagramCommentTargetKind::Wire: return "wire";
    case DiagramCommentTargetKind::WireFragment: return "wire_fragment";
    }
    return {};
}

inline std::optional<DiagramCommentTargetKind> diagramCommentTargetKind(std::string_view token) noexcept {
    if (token == "node") return DiagramCommentTargetKind::Node;
    if (token == "wire") return DiagramCommentTargetKind::Wire;
    if (token == "wire_fragment") return DiagramCommentTargetKind::WireFragment;
    return std::nullopt;
}

inline bool normalizeDiagramCommentAttachment(DiagramCommentAttachment &attachment) noexcept {
    if (attachment.targetId.empty() || diagramCommentTargetKindToken(attachment.kind).empty() ||
        !std::isfinite(attachment.nodeAnchor.x) || !std::isfinite(attachment.nodeAnchor.y) ||
        !std::isfinite(attachment.wirePosition)) return false;
    attachment.nodeAnchor.x = std::clamp(attachment.nodeAnchor.x, 0.0, 1.0);
    attachment.nodeAnchor.y = std::clamp(attachment.nodeAnchor.y, 0.0, 1.0);
    attachment.wirePosition = std::clamp(attachment.wirePosition, 0.0, 1.0);
    return true;
}

inline bool diagramCommentAttachmentTargetExists(const DiagramCommentAttachment &attachment,
    std::span<const DiagramNode> nodes, std::span<const DiagramEdge> edges,
    std::span<const DiagramWireFragment> fragments) noexcept {
    switch (attachment.kind) {
    case DiagramCommentTargetKind::Node:
        return std::any_of(nodes.begin(), nodes.end(), [&](const auto &node) { return node.id == attachment.targetId; });
    case DiagramCommentTargetKind::Wire:
        return std::any_of(edges.begin(), edges.end(), [&](const auto &edge) { return edge.id == attachment.targetId; });
    case DiagramCommentTargetKind::WireFragment:
        return std::any_of(fragments.begin(), fragments.end(), [&](const auto &fragment) { return fragment.id == attachment.targetId; });
    }
    return false;
}

// Run on a complete model at a transaction boundary, never during painting or
// halfway through copying/restoring a graph. Text and its authored frame stay.
inline std::size_t detachMissingDiagramCommentAttachments(std::span<DiagramNode> nodes,
    std::span<const DiagramEdge> edges, std::span<const DiagramWireFragment> fragments) noexcept {
    std::size_t detached = 0;
    for (auto &node : nodes) {
        if (!node.commentAttachment) continue;
        const auto &attachment = *node.commentAttachment;
        if (node.kind != "comment" ||
            (attachment.kind == DiagramCommentTargetKind::Node && attachment.targetId == node.id) ||
            !diagramCommentAttachmentTargetExists(attachment, nodes, edges, fragments)) {
            node.commentAttachment.reset();
            ++detached;
        }
    }
    return detached;
}

// A detached clipboard selection may refer only to targets copied with it.
// Never keep a reference back to the source document, even in same-doc paste.
template<class NodeIds, class EdgeIds, class FragmentIds>
void remapDiagramCommentAttachment(DiagramNode &node, const NodeIds &nodes,
    const EdgeIds &edges, const FragmentIds &fragments) {
    if (!node.commentAttachment) return;
    if (node.kind != "comment") { node.commentAttachment.reset(); return; }
    const auto remap = [&](const auto &ids) {
        const auto found = ids.find(node.commentAttachment->targetId);
        if (found == ids.end()) node.commentAttachment.reset();
        else node.commentAttachment->targetId = found->second;
    };
    switch (node.commentAttachment->kind) {
    case DiagramCommentTargetKind::Node: remap(nodes); break;
    case DiagramCommentTargetKind::Wire: remap(edges); break;
    case DiagramCommentTargetKind::WireFragment: remap(fragments); break;
    default: node.commentAttachment.reset(); break;
    }
}

} // namespace frog::native_engine
