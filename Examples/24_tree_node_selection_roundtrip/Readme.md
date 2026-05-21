# Example 24 - Tree Node Selection Roundtrip

Example 24 is a post-boundary Tree widget progression example.

The diagram copies `system_tree.value.selected_node_value` to `published_tree.value.selected_node_value` and to the public `selected_node_value` output. It also declares `system_tree.nodes.* -> published_tree.nodes.*` as a `frog.tree.records` dataflow so editable Tree cell content is published by `Execute` through the example graph, not by an implicit UI synchronization. The `.frog` file owns the finite hierarchy, node ids, node tags, labels, numeric node values, editable record data, expansion state, selected node, layout, visible-item posture, behavior flags, and instance-level style. The `.wfrog` package references the Default Tree realization and SVG asset. A hardcoded HTML-only replacement is not a valid implementation.

## Runtime Status

This example is C++-first in the private Graiphic runtime. Python and Rust parity must wait until the C++ visual and behavioral pass is accepted.

## Expected Behavior

- Selecting a node patches front-panel state without executing the diagram.
- The Tree exposes one LabVIEW-like expand/collapse control per parent node; optional checkboxes and node icons are disabled in this example by `.frog` display properties.
- Column headers are visible and fixed in this example; both visibility and fixed/scrolling posture are `.frog` display properties.
- The source data deliberately includes more rows and columns than the viewport so vertical and horizontal scrollbars can be inspected.
- `Execute` runs the manifest-backed native `u16` proof kernel.
- The read-only Tree indicator updates its selected numeric node from the native execution result and its editable record display from the declared `frog.tree.records` diagram dataflow.
- Node label, tag, value, visible items, selection, expansion, hierarchy lines, scrollbars, colors, border widths, font, and hit areas remain source/realization-owned.

## Files

- `main.frog` - source-owned Tree instances, hierarchy data, selection state, layout, bindings, and visual overrides.
- `main.fir.json` - FIR-facing value and UI binding summary.
- `main.lowering.json` - lowering intent toward the native manifest corridor.
- `ui/tree_panel.wfrog` - Example-local realization package that points to the Default Tree package and SVG asset.
- `../../Implementations/Reference/LLVM/examples/24_tree_node_selection_roundtrip/native_kernel_manifest.json` - native kernel manifest consumed by the private runtime.
