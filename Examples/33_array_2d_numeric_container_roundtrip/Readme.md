# Example 33 - Array 2D Numeric Container Roundtrip

This example demonstrates a two-dimensional Array container whose repeated cells instantiate the Default Numeric widget realization.

The Array owns rank, matrix shape, index displays, viewport, scrolling, insertion/materialization posture, and repeated-cell layout. The Numeric element owns per-cell value editing and value display. Control cells keep increment/decrement controls visible; indicator cells are read-only and use source-owned grey styling to show that they are not editable.

## Runtime Boundary

Example 33 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array is a container widget, not a hardcoded numeric grid.
- Each visible cell references the Default Numeric realization through `element.template_ref` and `element.asset_ref`.
- The same Array widget is used for rank 1 and rank 2; this example sets `dimensions.rank = 2` and `dimensions.shape` in the .frog source.
- Two index display rows are source-owned through `index_display.rank = 2` and `index_display.values`.
- Control cells use `element.layout.fit_part = control_body` with `element.props.display.increment_buttons_visible = true`.
- Indicator cells use `element.layout.fit_part = indicator_body`, `interaction.read_only = true`, and source-owned grey style values.
- `element.layout.boundary_policy = superpose_adjacent_widget_bounds` declares that repeated Numeric cells share their adjacent published bounds. The runtime must not add Array-local fake cell shells around the contained Numeric widgets.
- Runtime overlays must align to Default Array `element_region` / `element_slot` parts and Default Numeric `value_face` / `text_value` / `increment_up` / `increment_down` parts.
- No local Example SVG skin is duplicated.
