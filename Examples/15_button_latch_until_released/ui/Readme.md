# Example 15 UI Realization Package

This folder contains the Example 15 `.wfrog` package for the bounded Button `latch_until_released` acceptance slice.

The package intentionally references the shared Default Button and Boolean realization assets:

- `Libraries/Realizations/Default/assets/button/templates/button_rectangular.svg`
- `Libraries/Realizations/Default/assets/boolean/templates/boolean_circular.svg`

It does not duplicate SVG skins under `Examples`. Widget instance layout, labels, initial values, bindings, and per-instance visual overrides remain owned by `Examples/15_button_latch_until_released/main.frog`.

