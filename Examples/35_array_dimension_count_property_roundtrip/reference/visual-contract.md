# Example 35 Visual Contract

Example 35 records the accepted browser-host reference surface for `Numeric Array Dimension Count Property Roundtrip`.

## Accepted Visible Surface

- Captured from the C++ browser host on 2026-06-11 after real browser visual validation.
- The snapshot is public evidence only; source truth remains the owning `.frog`, `.wfrog`, FIR/lowering, Default realization packages, and native/host manifests.
- Array shell uses the accepted LabVIEW NXG-like neutral palette: container body `#EEF4F6`, helper/index background `#ffffff`, borders and scrollbar thumb `#A8ABAE`, helper button fill `#DDE1E4`, and selection/aura blue `#00ADEF`.
- Visible cells instantiate the Default Numeric widget; they are not hardcoded array cells.
- Array cell bounds equal the contained Numeric placement aura. The cell must not add local padding or fake borders around the contained Numeric widget.
- Numeric control cells use the white editable value face and visible increment/decrement controls; Numeric indicator cells use the accepted indicator gray `#E8EBED` and hide increment/decrement controls.
- Property helper controls, when present, are Default Numeric U8 widgets using the accepted 96 x 32 placement-bounds posture.

## Accepted Data Posture

- The accepted public state is stored in `state.accepted.json`.
- Public outputs, UI widget values, diagnostics, consumed assets, and source-owned visual properties remain inspectable without exposing private runtime implementation code.

## Inspection Notes

This snapshot was captured from the C++ Graiphic private runtime browser host for a repository-visible widget progression example. It is evidence for the example and widget realization surface, not a claim that every possible runtime host behavior has been generalized.
