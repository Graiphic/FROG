# Example 32 - Array 1D Horizontal Visible Count Property Roundtrip

This example demonstrates a horizontal one-dimensional Array container whose repeated cells instantiate the Default Numeric widget realization, plus a Numeric U8 control that drives the Array viewport visible-cell count through an explicit property-write node.

Reviewed 2026-06-09.

The Array owns rank, shape, index display, horizontal viewport, scrolling, insertion/materialization, and repeated-cell layout. The Numeric U8 control owns the requested visible-cell count as front-panel state until `Execute` consumes it and writes `numeric_array.viewport.visible_counts[1]` and `published_array.viewport.visible_counts[1]`. The Numeric element owns per-cell value editing and value display. The concrete numeric representation and default value are source-owned through `element.props.data_type.representation = u16`, `element.props.data_type.named_numeric_size = U16`, and `element.default_value = 0`; the `.wfrog` package resolves Default Array and Default Numeric assets without owning semantic defaults.

## Runtime Boundary

Example 32 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array is a container widget, not a hardcoded numeric grid.
- This is an execution example: the front panel keeps the 16 px placement grid law through `front_panel.canvas.grid` (`visible = false`, `snap = placement_bounds`) and declares `canvas.presentation_mode = execution`, so the runtime hides IDE calibration points while preserving placement geometry.
- The visible-cell count is not a runtime shortcut: `.frog` declares the Numeric U8 value node and the horizontal Array property-write nodes.
- Each visible cell references the Default Numeric realization through `element.template_ref` and `element.asset_ref`.
- The standalone `visible_cell_count` control references the same Default Numeric realization and uses `u8` / `U8` as a source-owned numeric representation.
- Each visible Numeric cell declares its source-owned representation as `u16` / `U16`; the private runtime must not infer another numeric type from the SVG or host widget.
- The example validates the reviewed Numeric integration posture inside an Array: control and indicator cells both use `element.layout.fit_part = placement_bounds`.
- The Array cell is the contained Numeric aura (`96x32`) and `element.layout.padding = 0`; the Numeric body stays centered inside that aura by the Numeric realization law.
- Numeric control cells use the white editable value face and visible increment/decrement commands; Numeric indicator cells use the Default Numeric indicator gray `#E8EBED` and hide increment/decrement commands. In the compact Numeric skin, the visible per-cell border is owned by `element.props.style.value_face.*`, not by an Array-local fake frame.
- The reviewed Numeric-backed Array palette is source-owned: Array container body `#EEF4F6`, Array frame/region/helper borders `#A8ABAE`, scrollbar/button helper faces `#DDE1E4`, Numeric control value face white, Numeric indicator value face `#E8EBED`, and selection/aura blue `#00ADEF`.
- `element.layout.boundary_policy = contained_widget_placement_bounds` declares that each Array cell respects the contained Numeric widget placement bounds. The runtime must not add Array-local fake shells or geometry-changing selection borders around the contained Numeric widgets; hover/selection belongs to the Array cell.
- New numeric elements materialize from `element.default_value = 0`.
- `style.frame.*` styles the Array element viewport container. The visible index display is a neighboring Array subcontainer with its own `style.index_display.*` surface, separated from the element viewport by source-owned geometry. Contained Numeric border/fill posture is source-owned through `element.props.style.value_face.*`.
- `style.index_display.step_gap` and `style.index_display.value_gap` keep the Array indexer proportions source-owned instead of hardcoded in the runtime host.
- Runtime overlays must align to Default Array `element_region` / `element_slot` parts and Default Numeric `value_face` / `text_value` / `increment_up` / `increment_down` parts.
- The control is editable. The indicator is read-only.
- No local Example SVG skin is duplicated.
