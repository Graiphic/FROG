# Example 26 Visual Contract

Example 26 records the accepted browser-host reference surface for
`Array 2D Numeric Selection Roundtrip`.

This is a deprecated historical non-widget-composed Array development snapshot
as of 2026-06-04. It is preserved for traceability and regression context, but
it is not the final runtime rendering target for Array. The final rendering
direction is the widget-composed Array container posture introduced by
Examples 29-37.

## Accepted Visible Surface

- `numeric_array`: control `frog.widgets.array_control`.
- `published_array`: indicator `frog.widgets.array_indicator`.

## Source-Owned Geometry And Style

- `.frog` owns diagram logic, widget instance ids, bindings, labels, layout,
  initial values, and instance-level visual overrides.
- `.wfrog` resolves realization packages, Default asset ids, supported widget
  classes, and host capability declarations.
- Default SVG assets own reusable visual grammar, public parts, internal
  geometry, internal anchors, and default visual behavior.
- Runtime overlays may exist only to provide host interaction and must align to
  published SVG parts and source-owned instance properties.
- Selection, editing, scrolling, and execution affordances must not change the
  published widget layout dimensions.

## Accepted Asset And Part Evidence

- Asset reference consumed: `asset:array_shell_svg`.


## Accepted Data Posture

- The accepted state is stored in `state.accepted.json`.
- Public outputs, UI widget values, native manifest status, and consumed assets
  are inspectable without exposing private runtime implementation code.

## Inspection Notes

This snapshot was captured from the C++ Graiphic private runtime browser host
for a post-public-runtime-boundary widget progression example. It is evidence
for the example and widget realization surface, not a claim that every possible
runtime host behavior has been generalized.
