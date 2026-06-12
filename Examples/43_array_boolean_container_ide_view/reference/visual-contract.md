# Example 43 Visual Contract

Example 43 records the accepted browser-host reference surface for `Boolean Array IDE Placement View`.

## Accepted Visible Surface

- Captured from the C++ browser host on 2026-06-11 after real browser visual validation.
- The snapshot is public evidence only; source truth remains the owning `.frog`, `.wfrog`, FIR/lowering, Default realization packages, and native/host manifests.
- Array shell uses the accepted LabVIEW NXG-like neutral palette: container body `#EEF4F6`, helper/index background `#ffffff`, borders and scrollbar thumb `#A8ABAE`, helper button fill `#DDE1E4`, and selection/aura blue `#00ADEF`.
- Array cells represent the contained Boolean placement aura. The runtime must not invent a hardcoded Boolean cell shell.
- Control cells use the square Default Boolean skin; indicator cells use the round Default Boolean skin.
- Boolean cells retain the accepted 72 x 72 placement aura and 64 x 64 state face with a uniform 4 source-unit band.
- Numeric property helper controls use the accepted Default Numeric 96 x 32 posture and are not bespoke mini controls.
- In IDE mode, all front-panel widgets are selectable: the three Numeric U8 property controls expose the Default Numeric placement aura, and both Array widgets expose a split Array placement aura.
- The Array placement aura is composed of two regions: one around the Array body/frame/scrollbars, without the index display, and one around the index display.
- Selecting an Array cell disables the front-panel Array aura and shows only the cell aura.
- Hover preview and selected state are distinct: hover uses a lighter blue with a heavier stroke, while selected uses confirmed `#00ADEF` with a thinner stroke. Clicking outside IDE-selectable widgets clears the current selection.
- The IDE interaction palette is configurable through `canvas` style tokens in
  `.frog`: `ide.style.aura.*`, `ide.style.label.*`, and
  `ide.style.cell.*`. These tokens feed runtime CSS variables, so changing the
  aura colors or stroke widths does not require changing C++.

## Accepted Data Posture

- The accepted public state is stored in `state.accepted.json`.
- Public outputs, UI widget values, diagnostics, consumed assets, and source-owned visual properties remain inspectable without exposing private runtime implementation code.

## Inspection Notes

This snapshot was captured from the C++ Graiphic private runtime browser host for a repository-visible widget progression example. It is evidence for the example and widget realization surface, not a claim that every possible runtime host behavior has been generalized.
