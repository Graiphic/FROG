# Example 12 UI Realization Package

This folder contains the Example 12 `.wfrog` package for the bounded Button `switch_when_released` acceptance slice.

The package intentionally references the shared Default Button and Boolean realization assets:

- `Libraries/Realizations/Default/assets/button/templates/button_rectangular.svg`
- `Libraries/Realizations/Default/assets/boolean/templates/boolean_circular.svg`

It does not duplicate SVG skins under `Examples`. Widget instance layout, labels, initial values, bindings, and per-instance visual overrides remain owned by `Examples/12_button_switch_when_released/main.frog`.
