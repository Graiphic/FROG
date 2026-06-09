# Example 29 - Array 1D Numeric Container Roundtrip

This example demonstrates a one-dimensional Array container whose repeated cells instantiate the Default Numeric widget realization.

Reviewed 2026-06-08.

The Array owns rank, shape, index display, viewport, scrolling, insertion/materialization, and repeated-cell layout. The Numeric element owns per-cell value editing and value display. The concrete numeric representation and default value are source-owned through `element.props.data_type.representation = u16`, `element.props.data_type.named_numeric_size = U16`, and `element.default_value = 0`; the `.wfrog` package resolves Default Array and Default Numeric assets without owning semantic defaults.

## Runtime Boundary

Example 29 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array is a container widget, not a hardcoded numeric grid.
- The front panel declares the visible 16 px placement grid through `front_panel.canvas.grid` (`visible = true`, `snap = placement_bounds`); the runtime only renders that source-owned calibration.
- Each visible cell references the Default Numeric realization through `element.template_ref` and `element.asset_ref`.
- Each visible Numeric cell declares its source-owned representation as `u16` / `U16`; the private runtime must not infer another numeric type from the SVG or host widget.
- The example validates the reviewed Numeric integration posture inside an Array: control and indicator cells both use `element.layout.fit_part = placement_bounds`.
- The Array cell is the contained Numeric aura (`96x32`) and `element.layout.padding = 0`; the Numeric body stays centered inside that aura by the Numeric realization law.
- `element.layout.boundary_policy = contained_widget_placement_bounds` declares that each Array cell respects the contained Numeric widget placement bounds. The runtime must not add Array-local fake shells or geometry-changing selection borders around the contained Numeric widgets; hover/selection belongs to the Array cell.
- New numeric elements materialize from `element.default_value = 0`.
- `style.frame.*` styles the Array element viewport container. The visible index display is a neighboring Array subcontainer with its own `style.index_display.*` surface, separated from the element viewport by source-owned geometry. `element.props.style.frame.*` styles each contained Numeric widget frame. These are intentionally separate surfaces.
- `style.index_display.step_gap` and `style.index_display.value_gap` keep the Array indexer proportions source-owned instead of hardcoded in the runtime host.
- Runtime overlays must align to Default Array `element_region` / `element_slot` parts and Default Numeric `value_face` / `text_value` / `increment_up` / `increment_down` parts.
- The control is editable. The indicator is read-only.
- No local Example SVG skin is duplicated.
