# Example 43 UI Package

Reviewed 2026-06-11.

`array_panel.wfrog` resolves the Default Array, Default Numeric, and Default
Boolean realizations used by the Boolean Array IDE placement view.

The source owns the contained Boolean cell choice:

- control Array cells use `asset:boolean_rectangular_svg`,
- indicator Array cells use `asset:boolean_circular_svg`,
- both cells fit `placement_bounds`,
- both cells use `contained_widget_placement_bounds` as the Array cell boundary.

The `.wfrog` package only resolves assets and host capabilities. It does not
choose square versus round cells and does not define Array execution behavior.
