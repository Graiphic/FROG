# Example 07 - String Hello World Roundtrip

Example 07 is a bounded runtime/UI-binding pilot for a String widget roundtrip across the current C++, Python, and Rust reference runtimes.

It publishes a simple `hello world` flow:

```text
string control value -> public input -> LLVM-produced string kernel -> public output -> string indicator value
```

The visible front panel must consume `ui/string_panel.wfrog` and the Default String realization asset.
It must not publish a local duplicated SVG or pass validation through a handcrafted HTML/CSS fallback.

The String caption placement, hover colors, frame colors, and text-region styling are instance properties in the `.wfrog` package.
The Default String realization intentionally does not publish a `focus_ring` part for this example surface.
