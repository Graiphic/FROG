# Example 05 — Bounded UI Accumulator

Canonical bounded corridor from `.frog` source to UI package, FIR, lowering, runtime-family consumption, and first downstream LLVM proof

FROG — Free Open Graphical Language

* * *

## Navigation

- Examples index: [`../Readme.md`](../Readme.md)
- Freeze boundary: [`Freeze.md`](Freeze.md)
- Example UI package: [`ui/Readme.md`](ui/Readme.md)
- Runtime-family parent: [`../../Implementations/Reference/Runtime/Readme.md`](../../Implementations/Reference/Runtime/Readme.md)
- Shared runtime-family acceptance: [`../../Implementations/Reference/Runtime/acceptance/Readme.md`](../../Implementations/Reference/Runtime/acceptance/Readme.md)
- Python runtime consumer: [`../../Implementations/Reference/Runtime/python/Readme.md`](../../Implementations/Reference/Runtime/python/Readme.md)
- Rust runtime consumer: [`../../Implementations/Reference/Runtime/rust/Readme.md`](../../Implementations/Reference/Runtime/rust/Readme.md)
- C/C++ runtime consumer: [`../../Implementations/Reference/Runtime/cpp/Readme.md`](../../Implementations/Reference/Runtime/cpp/Readme.md)
- Contract handoff family: [`../../Implementations/Reference/ContractEmitter/Readme.md`](../../Implementations/Reference/ContractEmitter/Readme.md)
- LLVM proof corridor: [`../../Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/Readme.md`](../../Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/Readme.md)

* * *

## Overview

This example is the canonical bounded vertical slice for the current published FROG corridor.

It is the small but serious reference case used to read one connected chain:

```text
main.frog
  -> ui/accumulator_panel.wfrog
  -> main.fir.json
  -> main.lowering.json
  -> backend-family contract
  -> shared runtime-family acceptance
  -> runtime-family consumers
  -> first downstream LLVM proof
```

This example exists to prove corridor integrity, not to model a large application.

* * *

## Current maintenance posture

This example should now be read as the current frozen bounded reference slice for the published runtime-family corridor.

The freeze boundary is documented in [`Freeze.md`](Freeze.md).

That freeze does not mean that FROG is complete.
It means that the repository should stop silently reworking the same basic slice and instead grow the next closure fronts around a stable published anchor.

* * *

## Published files

```text
Examples/05_bounded_ui_accumulator/
├── Readme.md
├── Freeze.md
├── front_panel.objects.json
├── main.fir.json
├── main.frog
├── main.lowering.json
└── ui/
    ├── Readme.md
    ├── accumulator_panel.wfrog
    └── assets/
        ├── numeric_control.svg
        └── numeric_indicator.svg
```

* * *

## Why this example matters

This slice is the first published repository-visible corridor that combines:

- front-panel participation,
- widget-value participation,
- widget-reference participation,
- bounded structured control,
- explicit local state,
- public output publication,
- emitted backend-family handoff,
- shared runtime-family acceptance,
- multi-language runtime-family consumers,
- and a first narrow LLVM-native proof corridor.

That combination makes it the right anchor for corridor stabilization.

* * *

## Boundary

This example is still bounded.

It does not claim:

- full generalized widget closure,
- full generalized runtime symmetry for all future examples,
- full automatic backend-family generation,
- or full native rendered front-panel closure.

Its purpose is narrower:
make one serious source-to-execution corridor visible, inspectable, executable, and stable.

* * *

## Summary

Use Example 05 as the current frozen bounded reference slice.

Preserve it.
Test against it.
Grow the next widget and example fronts around it rather than repeatedly redefining it.
