# Example 33 - Array 2D Numeric Container Roundtrip

This example demonstrates a two-dimensional Array container whose repeated cells instantiate the Default Numeric widget realization.

Reviewed 2026-06-09.

The Array owns rank, matrix shape, index displays, viewport, scrolling, insertion/materialization posture, and repeated-cell layout. The Numeric element owns per-cell value editing and value display. Control cells keep increment/decrement controls visible; indicator cells are read-only and use source-owned grey styling to show that they are not editable.

## Runtime Boundary

Example 33 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array is a container widget, not a hardcoded numeric grid.
- This is an execution example: the front panel keeps the 16 px placement grid law through `front_panel.canvas.grid` (`visible = false`, `snap = placement_bounds`) and declares `canvas.presentation_mode = execution`, so the runtime hides IDE calibration points while preserving placement geometry.
- Each visible cell references the Default Numeric realization through `element.template_ref` and `element.asset_ref`.
- The same Array widget is used for rank 1 and rank 2; this example sets `dimensions.rank = 2` and `dimensions.shape` in the .frog source.
- Two index display rows are source-owned through `index_display.rank = 2` and `index_display.values`.
- Control and indicator cells both use `element.layout.fit_part = placement_bounds`; the Array cell is the contained Numeric aura (`96x32`). Controls keep increment/decrement controls visible with a white editable value face; indicators are read-only, hide increment/decrement controls, and use the Default Numeric indicator gray `#E8EBED`. In the compact Numeric skin, the visible per-cell border is owned by `element.props.style.value_face.*`, not by an Array-local fake frame.
- The reviewed Numeric-backed Array palette is source-owned: Array container body `#EEF4F6`, Array frame/region/helper borders `#A8ABAE`, scrollbar/button helper faces `#DDE1E4`, Numeric control value face white, Numeric indicator value face `#E8EBED`, and selection/aura blue `#00ADEF`.
- `element.layout.boundary_policy = contained_widget_placement_bounds` declares that each Array cell respects the contained Numeric widget placement bounds. The runtime must not add Array-local fake cell shells around the contained Numeric widgets; hover/selection belongs to the Array cell.
- Runtime overlays must align to Default Array `element_region` / `element_slot` parts and Default Numeric `value_face` / `text_value` / `increment_up` / `increment_down` parts.
- No local Example SVG skin is duplicated.
