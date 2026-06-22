# Example 47 Reference Screenshot

This directory publishes the screenshot-only public reference for Example 47,
the Static Image widget placement example.

## Evidence

- `screenshot.png`: captured from the private C++ browser-host runtime on
  `2026-06-20` and re-opened for visual inspection on `2026-06-22`.
- Visual check: the screenshot shows two `frog.widgets.image_static` instances
  sharing the same source-owned SVG media asset through `media_ref`, with one
  selected placement aura and one resized layout-driven instance.

## Boundary

Image Static is static imported or pasted media. It is not the Picture widget,
and this reference does not define dataflow-produced image output semantics.
The screenshot is public human comparison evidence only; richer runtime
evidence belongs in the private context/runtime repositories.
