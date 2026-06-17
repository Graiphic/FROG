# Example 44 - Button Array Dimension Count Property Roundtrip

Reviewed 2026-06-17.

This example demonstrates an N-dimensional Array container whose dimension count is controlled by a Numeric U8 control through explicit diagram property writes. Two additional Numeric U8 controls continue to command visible row and column counts for the active Array view.

The Array owns the N-dimensional rank posture, source-owned explicit/default shape profiles, one index display per active dimension, viewport, scrolling, insertion/materialization posture, and repeated-cell layout. Rank 1 renders as one visible axis; rank 2 renders as a matrix; rank 3 or higher renders the last two dimensions as the visible matrix and exposes every leading dimension through stacked index displays. The Numeric U8 controls are front-panel values until Execute consumes them and writes the Array rank/display/viewport properties. The Button element owns per-cell true/false state, ON/OFF state text, and Button mechanical behavior through the Default Button realization.

The control and indicator Arrays both use rectangular Default Button cells through `asset:button_rectangular_svg`.
Both use `element.layout.fit_part = placement_bounds` and `element.layout.boundary_policy = contained_widget_placement_bounds`, so each Array cell is the contained Button placement aura rather than a runtime-invented shell. The Button `button_face` maps to the common `main_body` vocabulary; the repeated Array cell is sized to the Button `placement_bounds` aura.

## Runtime Boundary

Example 44 is a post-public-boundary widget progression example. Graiphic private runtimes validate this behavior unless the example is explicitly promoted later as public reference runtime material.

## Validation Notes

- The Array is a container widget, not a hardcoded numeric grid.
- The dimension count is a source-owned Numeric U8 control that writes `dimensions.rank` and the matching `index_display.rank` mirror on Execute.
- This slice commands the widget's active Array dimension count across source-owned explicit profiles and a default N-dimensional leading-singleton/trailing-matrix profile, not a fixed 1D/2D/3D-only switch.
- Ranks above 2 keep the last two dimensions as the visible matrix and expose every leading dimension through stacked index displays.
- The index-display container must wrap the active number of index displays using the source-owned index-display box size, gap, border, and padding.
- Execute wires the row and column controls through explicit property-write nodes to `viewport.visible_counts[0]` and `viewport.visible_counts[1]`.
- In rank 1, the visible column count is intentionally collapsed to one visual column; in rank 2 or higher, row/column counts control the visible projection of the last two semantic dimensions.
- Visible counts are viewport display properties. They may exceed the currently materialized semantic shape without forcing data materialization.
- Each visible cell references the Default Button realization through `element.template_ref` and source-owned `element.asset_ref`.
- Runtime overlays must align to Default Array `index_display`, `element_region`, `element_slot`, and scrollbar parts. Button cells fit the Default Button `placement_bounds` aura and render the visible value through `button_face` plus `state_text`.
- No local Example SVG skin is duplicated.
- Runtime-family validation for this post-boundary example must preserve the same source-owned N-dimensional rank, index-display wrapping, final-two-dimensions projection, visible row/column command semantics, and Button cell roundtrip across the private hosts that implement this slice.

## Public Capture

The public screenshot is pending. Runtime validation for this slice was
performed against the C++ private browser host; the public repository should
only receive a screenshot after a non-black, visually checked capture is
available.
