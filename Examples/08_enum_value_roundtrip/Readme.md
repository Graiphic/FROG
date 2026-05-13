# Example 08 - Enum Value Roundtrip

Example 08 is a bounded C++-first runtime/UI-binding pilot for an Enum widget roundtrip.

The diagram copies `mode_input.value` to `mode_result.value` through the public `mode_value -> result_mode` surface. The `.frog` file owns the front-panel instance, including widget position and per-instance style parameters. The visible front panel must consume `ui/enum_panel.wfrog` and the shared Default Enum realization asset; a hardcoded HTML-only replacement is not a valid implementation.
