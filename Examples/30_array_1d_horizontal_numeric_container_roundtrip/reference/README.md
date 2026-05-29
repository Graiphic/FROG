# Example 30 Reference Snapshot

This directory records the accepted public reference snapshot for Example 30,
`Array 1D Horizontal Numeric Container Roundtrip`.

The snapshot documents the visible C++-accepted state of the horizontal
one-dimensional Array container example. It is evidence for the public example
dossier, not runtime source truth and not a promotion of Graiphic private
runtime implementation code into the public repository.

## Files

- `screenshot.png` - accepted browser-host visual state.
- `state.accepted.json` - accepted public runtime snapshot.
- `visual-contract.md` - human-readable appearance and interaction contract.
- `visual-contract.json` - machine-readable visual contract summary.
- `artifact-index.json` - relative artifact paths and hashes for traceability.

## Boundary

The source of truth remains:

- `../main.frog`
- `../main.fir.json`
- `../main.lowering.json`
- `../ui/array_panel.wfrog`
- `../../Implementations/Reference/LLVM/examples/30_array_1d_horizontal_numeric_container_roundtrip/native_kernel_manifest.json`
- `../../Libraries/Realizations/Default/array.default.wfrog`
- `../../Libraries/Realizations/Default/numeric.default.wfrog`

The snapshot describes what was accepted, not a generalized runtime claim.
