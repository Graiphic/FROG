# Example 32 Visual Contract

Example 32 records the accepted browser-host reference surface for
Array 1D Horizontal Visible Count Property Roundtrip.

## Accepted Visible Surface

- `visible_item_count`: Numeric U8 control that requests the horizontal Array visible-cell count.
- `numeric_array`: control `frog.widgets.array_control`.
- `published_array`: indicator `frog.widgets.array_indicator`.

## Source-Owned Geometry And Style

- `.frog` owns diagram logic, widget instance ids, bindings, labels, layout,
  initial values, property-write nodes, and instance-level visual overrides.
- `.wfrog` resolves realization packages, Default asset ids, supported widget
  classes, and host capability declarations.
- Default SVG assets own reusable visual grammar, public parts, internal
  geometry, internal anchors, and default visual behavior.
- Runtime overlays may exist only to provide host interaction and must align to
  published SVG parts and source-owned instance properties.
- Selection, editing, scrolling, and execution affordances must not change the
  published widget layout dimensions.

## Accepted Property-Write Posture

- The visible-count Numeric control can be edited without changing the Array viewport.
- `Execute` consumes the Numeric U8 value and writes
  `numeric_array.viewport.visible_counts[1]` and
  `published_array.viewport.visible_counts[1]`.
- Visible counts are viewport display properties. They may exceed the currently
  materialized semantic shape without forcing data materialization.
- The Array viewport update is diagram-owned behavior, not an immediate
  design-time host resize.
- This example is execution-mode evidence. IDE live-edit behavior belongs to
  a separate design-time host boundary.

## Accepted Asset And Part Evidence

- Asset reference consumed: `asset:array_shell_svg`.
- Contained widget asset consumed: `asset:numeric_rectangular_svg`.
- Runtime overlays align to Default Array `index_display`, `element_region`,
  `element_slot`, and horizontal scrollbar parts.
- Contained Numeric element overlays align to Default Numeric `value_face`,
  `text_value`, `increment_up`, and `increment_down` parts.

## Accepted Data Posture

- The accepted state is stored in `state.accepted.json`.
- Public outputs, UI widget values, native manifest status, and consumed assets
  are inspectable without exposing private runtime implementation code.

## Inspection Notes

This snapshot was captured from the C++ Graiphic private runtime browser host
for a post-public-runtime-boundary widget progression example. It is evidence
for the example and widget realization surface, not a claim that every possible
runtime host behavior has been generalized.
