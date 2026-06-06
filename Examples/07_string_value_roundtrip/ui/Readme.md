# Example 07 String Front Panel

This folder owns the Example 07 `.wfrog` front-panel package.

The package references the Default String realization and does not publish a local duplicated SVG.

The package owns the Example 07 instance-level caption anchors and hover styling.
The runtime must apply those properties to the Default String realization instead of hardcoding label placement or interaction colors.

Reviewed 2026-06-06. The normal String surface publishes `focus_ring` so keyboard/browser focus geometry stays source-owned by the SVG skin. Browser hosts must not invent a different untracked CSS focus shape.
