# Example 07 - String Placeholder Roundtrip

Example 07 is a bounded runtime/UI-binding pilot for a String widget roundtrip across the current C++, Python, and Rust reference runtimes.

Reviewed 2026-06-04: the example starts with an empty String control so the published `placeholder` part is visible, while `text_value` remains the real editable value surface.

It publishes a simple String value flow:

```text
string control text_value -> public input -> LLVM-produced string kernel -> public output -> string indicator text_value
```

The visible front panel must consume `ui/string_panel.wfrog` and the Default String realization asset.
It must not publish a local duplicated SVG or pass validation through a handcrafted HTML/CSS fallback.

The String caption placement, hover colors, and text-region styling are instance properties in the `.wfrog` package.
The launch-state placeholder text is `Type your text here`; it is presentation-only and disappears as soon as the control has a value.
The Default String realization intentionally publishes no `frame`, `focus_ring`, caret, selection, validation marker, or overflow marker for this example surface.
