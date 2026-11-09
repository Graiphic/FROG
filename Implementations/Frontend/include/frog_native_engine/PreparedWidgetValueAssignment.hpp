#pragma once

#include "BindingType.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace frog::native_engine::graph { struct Dataset; }
namespace frog::native_engine::data_operations::values {

// PRIVATE authoring value transfer, not a document format or execution IR.
// Preparation owns schema validation and decoding. No widget, layout, window,
// document, codec callback, default metadata or presentation cache enters here.
enum class FieldPolicy { LogicalOrder, FieldIdentity };
[[nodiscard]] bool compatibleTypes(const binding::TypeRef &,const binding::TypeRef &,FieldPolicy);
enum class Container { Leaf, Cluster, Array };
enum class CellKind { Number, String, Boolean, Composed };
struct Scalar {
    // Preserve the exact lexeme and the independent legacy cache/presence.
    std::optional<std::string> initialValue;
    std::optional<double> numericValue;
    std::optional<bool> booleanValue;
    std::optional<std::string> supportTextValue;
    // Opaque symbolic-value index owned by the document preparation adapter.
    // The pure transfer never interprets a target or stores a resource handle.
    std::optional<std::size_t> referenceEnvelope;
    std::optional<std::size_t> collectionEnvelope;
    // A complete typed XY value owned by the private-document adapter.
    std::optional<std::size_t> xyEnvelope;
};
struct GraphValue {
    std::shared_ptr<const graph::Dataset> samples;
    bool acceptedByValueOwner=false;
    // An imported unsupported Graph remains an uninterpreted source envelope
    // retained at the document boundary. The calculation cannot read it.
    std::optional<std::size_t> retainedEnvelope;
};
struct Cell {
    std::vector<std::uint32_t> indices;
    CellKind kind=CellKind::Number;
    bool defined=true,enabled=true;
    std::string scalarLexeme;
    std::optional<std::size_t> composedValue;
};
struct Node {
    binding::TypeRef type;
    Container container=Container::Leaf;
    bool graphFamily=false;
    Scalar scalar;
    std::optional<GraphValue> graph;
    bool assignGraph=false;
    // Cluster children are in logical order; Array has its inherited value.
    std::vector<std::size_t> children;
    std::vector<std::string> fieldIds;
    std::vector<std::uint32_t> shape;
    std::vector<Cell> cells;
};
struct Tree {
    std::vector<Node> nodes;
    std::size_t root=0;
};
// Detached, iterative calculation. Rejection publishes nothing. Private
// canonical before/after comparison remains the adapter's no-op authority;
// comparing normalized values here must not rewrite historical cell bytes.
[[nodiscard]] std::optional<Tree> assign(const Tree &target,const Tree &source,FieldPolicy);
} // namespace frog::native_engine::data_operations::values
