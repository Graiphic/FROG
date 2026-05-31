# Example 32 Reference Snapshot

This directory records the accepted public reference snapshot for Example 32,
Array 1D Horizontal Visible Count Property Roundtrip.

The example is a horizontal 1D Array container whose visible cell count is
driven by a Numeric U8 front-panel control through explicit horizontal Array
viewport property-write nodes.

The snapshot is browser-host evidence for the public example dossier. It is not
runtime source truth, it does not publish Graiphic private runtime internals, and
it does not redefine the public FROG specification.

<p align="center">
  <img src="./screenshot.svg" alt="Accepted Example 32 Array 1D Horizontal Visible Count Property Roundtrip snapshot" width="700" />
</p>

## Reference Snapshot Links

- [Accepted screenshot](./screenshot.svg)
- [Accepted state JSON](./state.accepted.json)
- [Visual contract](./visual-contract.md)
- [Machine-readable visual contract](./visual-contract.json)
- [Artifact hash index](./artifact-index.json)

## Files

- `screenshot.svg` - accepted public visual reference for the browser-host checkpoint.
- `state.accepted.json` - accepted public runtime snapshot.
- `visual-contract.md` - human-readable appearance and interaction contract.
- `visual-contract.json` - machine-readable visual contract summary.
- `artifact-index.json` - relative artifact paths and hashes for traceability.

## Boundary

The source of truth remains the `.frog` source, FIR/lowering artifacts, `.wfrog`
realization references, Default SVG realization assets, and native manifest
artifacts listed in `artifact-index.json`. The snapshot describes what was
accepted for this example; it is not a generalized runtime completeness claim.

The visible-count Numeric control is front-panel state until `Execute` consumes
it. The Array viewport horizontal visible count changes only through the
declared property-write flow from `.frog` to FIR/lowering/native manifest/runtime.
