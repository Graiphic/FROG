# Example 08 - Enum Value Roundtrip

Example 08 is a bounded public reference runtime/UI-binding example for an Enum widget roundtrip, validated against the C++ reference runtime for this widget review.

Reviewed 2026-06-12: the example consumes the Default Enum compact value SVG, the optional increment/decrement command parts, and the dropdown list SVG skin so the opened item popup is not a hardcoded HTML-only visual surface. The dropdown host-surface layout is declared in `ui/enum_panel.wfrog`: it anchors to `value_face` and follows that part's outer width without any public selector-face part.

The accepted Enum visual posture is now a reusable default: `ui/enum_panel.wfrog` declares `default_widget_properties` for the control and indicator roles, and the C++ runtime merges those defaults before any `.frog` instance-level overrides.

The accepted control posture also exposes the LabVIEW-like increment/decrement command as semantic skin parts: `spinner`, `increment_up`, and `increment_down`. `display.increment_buttons_visible` controls whether that command is shown. The reviewed Default posture sets `data_entry.increment_wrap=true`, so stepping past the first or last enabled item wraps circularly through the declared item order; a source may explicitly set the property to `false` when it wants clamping instead.

Frozen 2026-06-12 geometry rule: `value_face` drives the dropdown width. If the compact value body width changes by a rendered pixel delta, the dropdown width must change by the same rendered pixel delta through the host-surface `value_face` inclusion rule.

The diagram copies `mode_input.value` to `mode_result.value` through the public `mode_value -> result_mode` surface. The `.frog` file owns the front-panel instance, including widget position and intentional per-instance style overrides. The visible front panel must consume `ui/enum_panel.wfrog` and the shared Default Enum realization assets/defaults; a hardcoded HTML-only replacement is not a valid implementation.
