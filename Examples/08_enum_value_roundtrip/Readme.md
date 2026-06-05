# Example 08 - Enum Value Roundtrip

Example 08 is a bounded public reference runtime/UI-binding example for an Enum widget roundtrip, validated against the C++ reference runtime for this widget review.

Reviewed 2026-06-05: the example consumes the Default Enum closed selector SVG and the dropdown list SVG skin so the opened item popup is not a hardcoded HTML-only visual surface. The dropdown host-surface layout is declared in `ui/enum_panel.wfrog`: it anchors to `value_face`, follows that part's outer width with a declared source-width outset, centers the popup under the body, and excludes `selector_face` from width measurement.

The accepted Enum visual posture is now a reusable default: `ui/enum_panel.wfrog` declares `default_widget_properties` for the control and indicator roles, and the C++ runtime merges those defaults before any `.frog` instance-level overrides.

Frozen 2026-06-05 geometry rule: `value_face` drives the dropdown width and the selector spacing. If the closed value body width changes by a rendered pixel delta, the dropdown width must change by the same rendered pixel delta through the `outer_width` rule, and `selector_face` must move by the same source delta to keep its declared `3.959` source-unit gap from `value_face.right`.

The diagram copies `mode_input.value` to `mode_result.value` through the public `mode_value -> result_mode` surface. The `.frog` file owns the front-panel instance, including widget position and intentional per-instance style overrides. The visible front panel must consume `ui/enum_panel.wfrog` and the shared Default Enum realization assets/defaults; a hardcoded HTML-only replacement is not a valid implementation.
