# Example 28 - Array 3D Numeric Selection Roundtrip

This example demonstrates a three-dimensional numeric Array control feeding a read-only Array indicator through the FROG source-to-runtime corridor.

The visible UI is owned by `main.frog`: widget ids, labels, layout, rank, 3D shape, layer/row/column elements, selected value, three index displays, viewport, binding, and instance-level visual overrides. The `.wfrog` package only references the Default Array realization assets and host capabilities.

## Runtime Boundary

Example 28 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array value is semantic rank 3, ordered as `[layer,row,column]`.
- The visible grid displays one 2D layer at a time.
- The control is editable and selectable.
- The indicator is read-only.
- Editing a numeric element outside the current volume materializes the full rectangular 3D volume up to `[layer,row,column]` and initializes new numeric elements to `0`, matching LabVIEW-like numeric Array behavior.
- Runtime overlays must align to the Default Array SVG-published parts.
- No local Example SVG skin is duplicated.
