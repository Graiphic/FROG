# Example 43 - Boolean Array IDE Placement View

Reviewed 2026-06-10.

This example is the IDE/review companion for Example 38. It keeps the same
Boolean Array source-owned contract while turning the placement grid on so the
Array container, its index display, and the contained Boolean cells can be
visually inspected.

The control Array uses square Default Boolean cells through
`asset:boolean_rectangular_svg`.

The indicator Array uses round Default Boolean cells through
`asset:boolean_circular_svg`.

Both arrays use `element.layout.fit_part = placement_bounds` and
`element.layout.boundary_policy = contained_widget_placement_bounds`. The
Array cell therefore represents the contained Boolean placement aura; the C++
runtime must not invent a separate hardcoded Boolean cell shell.

## IDE Surface

- `canvas.presentation_mode = ide_view`
- `canvas.grid.visible = true`
- `canvas.grid.pitch = 16`
- `canvas.grid.snap = placement_bounds`
- `canvas.grid.width_quantization = grid_pitch_multiple`

The grid is a visual review aid, not a different execution semantics. Source
members in `.frog` still own placement, cell dimensions, Boolean skin choice,
state colors, text, and Array viewport behavior.

## Runtime Boundary

The C++ reference host runs this view with the same native Boolean Array
identity corridor as Example 38. This example does not introduce a new Boolean
kernel; it exists to validate Array + Boolean widget integration under an IDE
placement view.
