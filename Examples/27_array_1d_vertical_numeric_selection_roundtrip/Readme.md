# Example 27 - Array 1D Vertical Numeric Selection Roundtrip

This example demonstrates a one-dimensional vertical numeric Array control feeding a read-only Array indicator through the FROG source-to-runtime corridor.

The visible UI is owned by `main.frog`: widget ids, labels, layout, rank, length, elements, selected value, one-dimensional index display, viewport, binding, and instance-level visual overrides. The `.wfrog` package only references the Default Array realization assets and host capabilities.

## Runtime Boundary

Example 27 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array value is semantic rank 1, not a 2D matrix with one column.
- The vertical viewport displays one vector slice and one index display.
- The control is editable and selectable.
- The indicator is read-only.
- Editing a numeric element outside the current vector length materializes the vector up to that index and initializes new numeric elements to `0`, matching LabVIEW-like numeric Array behavior.
- Runtime overlays must align to the Default Array SVG-published parts.
- No local Example SVG skin is duplicated.
