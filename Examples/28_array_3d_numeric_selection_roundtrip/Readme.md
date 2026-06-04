# Example 28 - Array 3D Numeric Selection Roundtrip

This example demonstrates a three-dimensional numeric Array control feeding a read-only Array indicator through the FROG source-to-runtime corridor.

> **Deprecated historical milestone. Reviewed 2026-06-04.**
> This rigid/non-widget-composed Array example is retained for regression and traceability only. It is superseded by the widget-composed Array container examples starting at Example `29`, where Array repeats contained Default widget realizations.

The visible UI is owned by `main.frog`: widget ids, labels, layout, rank, 3D shape, layer/row/column elements, selected value, three index displays, viewport, binding, and instance-level visual overrides. The `.wfrog` package only references the Default Array realization assets and host capabilities.

## Runtime Boundary

Example 28 is a deprecated post-public-boundary widget progression milestone. Graiphic private runtimes may keep validating this behavior for regression context, but it is not the final Array rendering model.

## Validation Notes

- The Array value is semantic rank 3, ordered as `[layer,row,column]`.
- The visible grid displays one 2D layer at a time.
- The control is editable and selectable.
- The indicator is read-only.
- Editing a numeric element outside the current volume materializes the full rectangular 3D volume up to `[layer,row,column]` and initializes new numeric elements to `0`, matching LabVIEW-like numeric Array behavior.
- Runtime overlays must align to the Default Array SVG-published parts.
- No local Example SVG skin is duplicated.
