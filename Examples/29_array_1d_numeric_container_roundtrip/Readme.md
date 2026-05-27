# Example 29 - Array 1D Numeric Container Roundtrip

This example demonstrates a one-dimensional Array container whose repeated cells instantiate the Default Numeric widget realization.

The Array owns rank, shape, index display, viewport, scrolling, insertion/materialization, and repeated-cell layout. The Numeric element owns per-cell value editing and value display. The concrete numeric representation and default value are source-owned through `element.props.data_type.representation = u16`, `element.props.data_type.named_numeric_size = U16`, and `element.default_value = 0`; the `.wfrog` package resolves Default Array and Default Numeric assets without owning semantic defaults.

## Runtime Boundary

Example 29 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array is a container widget, not a hardcoded numeric grid.
- Each visible cell references the Default Numeric realization through `element.template_ref` and `element.asset_ref`.
- Each visible Numeric cell declares its source-owned representation as `u16` / `U16`; the private runtime must not infer another numeric type from the SVG or host widget.
- Control cells keep the Numeric increment/decrement buttons visible through `element.props.display.increment_buttons_visible = true`; indicator cells keep them hidden because they are read-only.
- New numeric elements materialize from `element.default_value = 0`.
- Runtime overlays must align to Default Array `element_region` / `element_slot` parts and Default Numeric `value_face` / `text_value` / `increment_up` / `increment_down` parts.
- The control is editable. The indicator is read-only.
- No local Example SVG skin is duplicated.
