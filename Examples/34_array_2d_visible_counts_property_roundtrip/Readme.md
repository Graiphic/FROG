# Example 34 - Array 2D Visible Counts Property Roundtrip

This example demonstrates a two-dimensional Array container whose visible row and column counts are controlled by two Numeric U8 controls through explicit diagram property writes.

Reviewed 2026-06-08.

The Array owns rank, matrix shape, index displays, viewport, scrolling, insertion/materialization posture, and repeated-cell layout. The Numeric U8 controls are front-panel values until Execute consumes them and writes the Array viewport properties. The Numeric element owns per-cell value editing and value display.

## Runtime Boundary

Example 34 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array is a container widget, not a hardcoded numeric grid.
- The front panel declares the visible 16 px placement grid through `front_panel.canvas.grid` (`visible = true`, `snap = placement_bounds`); the runtime only renders that source-owned calibration.
- The visible row and visible column counts are source-owned Numeric U8 controls.
- Execute wires the controls through explicit property-write nodes to `viewport.visible_counts[0]` and `viewport.visible_counts[1]`.
- Visible counts are viewport display properties. They may exceed the currently materialized semantic shape without forcing data materialization.
- Each visible cell references the Default Numeric realization through `element.template_ref` and `element.asset_ref`.
- The same Array widget is used for rank 1 and rank 2; this example sets `dimensions.rank = 2` and `dimensions.shape` in the .frog source.
- Runtime overlays must align to Default Array `index_display`, `element_region`, `element_slot`, and scrollbar parts, plus Default Numeric value/increment parts.
- No local Example SVG skin is duplicated.

## Reference Snapshot

The accepted C++ browser-host reference package is published under
[`reference/`](./reference/README.md). It contains the accepted screenshot,
runtime state snapshot, visual contract, and artifact hash index for the source,
lowering, native manifest, and Default realization assets.
