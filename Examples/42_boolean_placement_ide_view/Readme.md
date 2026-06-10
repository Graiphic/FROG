# Example 42 - Boolean Placement IDE View

Reviewed date: 2026-06-10

This example is an IDE-view calibration witness for the Default Boolean realization.
It shows the same Boolean widget family rendered with square/rectangular and round/oval SVG skins.

The visible IDE selection aura is derived from the `placement_bounds` SVG part.
The Boolean value surface is the `state_face` SVG part.
The optional centered `state_text` displays the true/false text labels without defining the Boolean value.

Default Boolean geometry keeps `placement_bounds` at 72 x 72 source units and
the default `state_face` at 56 x 56 source units. The visible 8 source-unit
band between them is the Boolean skin aura. IDE selection displays
`placement_bounds`; keyboard focus follows `state_face`.

This example is reviewed as a visual IDE placement witness.
Execution semantics reuse the Boolean value roundtrip corridor only to keep the C++ reference host path available.
