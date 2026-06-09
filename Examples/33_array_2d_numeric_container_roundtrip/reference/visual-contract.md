# Example 33 Visual Contract

Example 33 records the accepted browser-host reference surface for
Array 2D Numeric Container Roundtrip.

Reviewed 2026-06-08.

## Accepted Visible Surface

- `numeric_array`: control `frog.widgets.array_control` containing repeated Default Numeric control widgets.
- `published_array`: indicator `frog.widgets.array_indicator` containing repeated Default Numeric indicator widgets.

## Source-Owned Geometry And Style

- `.frog` owns diagram logic, widget instance ids, bindings, labels, layout,
  initial values, rank, shape, selected indices, and instance-level visual overrides.
- `.wfrog` resolves realization packages, Default asset ids, supported widget
  classes, and host capability declarations.
- Default SVG assets own reusable visual grammar, public parts, internal
  geometry, internal anchors, and default visual behavior.
- Runtime overlays may exist only to provide host interaction and must align to
  published SVG parts and source-owned instance properties.
- Selection, editing, scrolling, and execution affordances must not change the
  published widget layout dimensions.

## Accepted Container Posture

- The Array is a widget container, not a hardcoded numeric grid.
- Each visible cell references the Default Numeric realization through
  `element.template_ref` and `element.asset_ref`.
- Control cells keep Numeric increment/decrement controls visible.
- Indicator cells are read-only and use source-owned grey styling.
- Control and indicator cells both fit the contained Numeric `placement_bounds` aura (`96x32`).
- `element.layout.boundary_policy = contained_widget_placement_bounds` means
  each Array cell respects the contained Numeric widget placement bounds instead
  of receiving Array-local fake cell shells.

## Accepted Asset And Part Evidence

- Asset reference consumed: `asset:array_shell_svg`.
- Contained widget asset consumed: `asset:numeric_rectangular_svg`.
- Runtime overlays align to Default Array `index_display`, `element_region`,
  `element_slot`, and scrollbar parts.
- Contained Numeric element overlays align to Default Numeric `placement_bounds`; the visible body still uses `control_body` / `indicator_body`, `value_face`, `text_value`, `increment_up`, and `increment_down` internally.

## Accepted Data Posture

- The accepted state is stored in `state.accepted.json`.
- Public outputs, UI widget values, native manifest status, consumed assets,
  selected element id, selected indices, and published Array records are
  inspectable without exposing private runtime implementation code.

## Inspection Notes

This snapshot was captured from the C++ Graiphic private runtime browser host
for a post-public-runtime-boundary widget progression example. It is evidence
for the example and widget realization surface, not a claim that every possible
runtime host behavior has been generalized.
