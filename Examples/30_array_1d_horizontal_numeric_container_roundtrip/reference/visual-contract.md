# Example 30 Visual Contract

Example 30 validates a horizontal one-dimensional Array container whose visible
cells are Default Numeric widget instances.

## Accepted Visible Surface

- One horizontal Array control labeled `Horizontal Array container`.
- One horizontal Array indicator labeled `Horizontal Array indicator`.
- Both surfaces show a separate Array index display on the left and an Array
  element viewport on the right.
- The Array index display is a separate subcontainer with stacked increment and
  decrement buttons plus an index value face.
- The control Array element viewport contains repeated Numeric control widgets.
- The indicator Array element viewport contains repeated Numeric indicator
  widgets.
- Numeric cells share adjacent bounds; the Array does not draw additional local
  separator shells around contained Numeric widgets.
- The control exposes Numeric increment/decrement faces inside each visible
  cell.
- The indicator omits Numeric increment/decrement faces and remains read-only.
- Horizontal scrollbars remain inside the Array element viewport frame.

## Source-Owned Geometry And Style

- `.frog` owns instance position, label text, label placement, visible element
  count, index display value, viewport offset, selected element, element
  template binding, and per-instance visual overrides.
- `.wfrog` resolves the Default Array and Default Numeric realization assets.
- Default Array publishes the index display, element region, element slot, and
  scrollbar parts.
- Default Numeric publishes value face, text value, increment up, increment
  down, control body, and indicator body parts.
- Selection must not change layout dimensions.
- Repeated Numeric widget boundaries superpose; there is no fake Array-local
  HTML/CSS cell shell.

## Accepted Data Posture

- The control starts with numeric values `10, 20, 30, 40, 50, 60, 70, 80, 90,
  100`.
- The accepted selected element id is `i2`, with selected value `30`.
- The visible viewport starts at column offset `0`.
- New elements materialize from the source-owned default value `0`.

## Inspection Notes

This snapshot was accepted as a post-public-runtime-boundary widget progression
example. Runtime source for this slice remains private in `Graiphic/FROG-Runtime`.
