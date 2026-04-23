<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

# Reference Runtime

<p>Runtime-family consumers for the non-normative FROG reference implementation.</p>
<p>FROG — Free Open Graphical Language</p>

---

## Navigation

- Parent reference implementation: [`../Readme.md`](../Readme.md)
- Runtime-family responsibilities: [`responsibilities.md`](responsibilities.md)
- Contract-consumption note: [`accept_contract_and_execute.md`](accept_contract_and_execute.md)
- Shared acceptance material: [`acceptance/Readme.md`](acceptance/Readme.md)
- Example-specific Python wrapper: [`run_slice05_contract.py`](run_slice05_contract.py)
- Python consumer: [`python/Readme.md`](python/Readme.md)
- Rust consumer: [`rust/Readme.md`](rust/Readme.md)
- C/C++ consumer: [`cpp/Readme.md`](cpp/Readme.md)
- Example corridor anchor: [`../../../Examples/05_bounded_ui_accumulator/Readme.md`](../../../Examples/05_bounded_ui_accumulator/Readme.md)
- Contract artifact family: [`../ContractEmitter/Readme.md`](../ContractEmitter/Readme.md)

## Overview

This directory is the parent coordination point for the first published FROG runtime family in the non-normative reference implementation.

Its job is narrow and downstream:

```text
canonical .frog source
  -> semantic acceptance
  -> FIR
  -> lowering
  -> backend-family contract
  -> runtime-family consumer
```

The runtime family begins after source, meaning, FIR, lowering, and backend-contract emission. It consumes published contract artifacts. It does not define the language, the widget law, the front panel, or the compiler-family corridor.

## Published runtime family

The first published runtime family is:

```text
reference_host_runtime_ui_binding
```

For the current bounded corridor, the family reads:

- one single-process host execution posture,
- one deterministic bounded execution model,
- one explicit state carrier,
- one minimal UI binding surface,
- one browser-host realization path for the first visible runtime UI slice.

The current family is intentionally small. It exists to close one inspectable corridor, not to claim general runtime closure for the whole language.

## Shared acceptance posture

The runtime family carries a shared acceptance layer under:

```text
Implementations/Reference/Runtime/acceptance/
```

That acceptance layer exists to keep the three consumer languages aligned on:

- the accepted contract family,
- the accepted `.wfrog` package shape,
- the accepted SVG asset surface,
- the accepted execution result for the bounded slice,
- the accepted browser-host UI snapshot surface.

Acceptance artifacts are shared inputs for Python, Rust, and C/C++ consumers. They are not a substitute for source, FIR, lowering, or backend-contract ownership.

## Current published repository shape

```text
Implementations/Reference/Runtime/
├── Readme.md
├── accept_contract_and_execute.md
├── acceptance/
│   ├── Readme.md
│   ├── example05_input_3.snapshot.json
│   └── example05_runtime_family.acceptance.json
├── reference_runtime.py
├── responsibilities.md
├── run_slice05_contract.py
├── python/
│   ├── Readme.md
│   ├── __init__.py
│   ├── cli.py
│   ├── execute_contract.py
│   ├── run_slice05_ui.py
│   ├── runtime_core.py
│   ├── ui_runtime.py
│   └── tests/
│       ├── __pycache__/                    [generated, not intended source]
│       ├── test_runtime_slice05.py        [acceptance-driven]
│       ├── test_runtime_ui_slice05.py     [acceptance-driven]
│       ├── test_slice05_contract.py       [legacy smoke test]
│       └── test_slice05_ui_runtime.py     [legacy smoke test]
├── rust/
│   ├── Readme.md
│   ├── Cargo.toml
│   ├── src/
│   │   ├── cli.rs
│   │   ├── contract.rs
│   │   ├── diagnostics.rs
│   │   ├── execute.rs
│   │   ├── lib.rs
│   │   ├── main.rs
│   │   ├── runtime.rs
│   │   └── ui.rs
│   └── tests/
│       ├── slice05_runtime.rs
│       └── slice05_ui.rs
└── cpp/
    ├── Readme.md
    ├── CMakeLists.txt
    ├── include/
    │   ├── contract.hpp
    │   ├── execute.hpp
    │   ├── json.hpp
    │   ├── runtime.hpp
    │   └── ui.hpp
    ├── src/
    └── tests/
        └── test_slice05.cpp
```

Generated cache and build artifacts are not part of the intended runtime-family source contract and should not be versioned. The Python tree still shows transitional smoke tests beside the acceptance-driven tests. The acceptance-driven line is the current family-level source of truth.

## First corridor this directory coordinates

The current canonical runtime slice is anchored in:

```text
Examples/05_bounded_ui_accumulator/
```

The runtime-family handoff for that slice is the published backend contract artifact:

```text
Implementations/Reference/ContractEmitter/examples/
└── 05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json
```

The family-level reading posture is therefore:

```text
Examples/05_bounded_ui_accumulator/main.frog
  -> Examples/05_bounded_ui_accumulator/main.fir.json
  -> Examples/05_bounded_ui_accumulator/main.lowering.json
  -> backend-family contract
  -> runtime-family consumer
  -> headless result and/or browser-host UI
```

## Inputs consumed by the first common runtime slice

For the current bounded slice, the runtime-family consumers share the same logical inputs:

- the emitted backend contract artifact,
- the example-local `.wfrog` package,
- the SVG assets referenced by that package,
- the shared acceptance reading posture published under `acceptance/`.

For Example 05, the package input is:

```text
Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog
```

and the referenced assets are:

```text
Examples/05_bounded_ui_accumulator/ui/assets/numeric_control.svg
Examples/05_bounded_ui_accumulator/ui/assets/numeric_indicator.svg
```

The runtime family does not take semantic authority away from `main.frog`, FIR, lowering, or the `.wfrog` package.

## Current published entry points

### Parent-level Python convenience entry points

```text
python -m Implementations.Reference.Runtime.run_slice05_contract 3
python Implementations/Reference/Runtime/python/run_slice05_ui.py
```

These remain useful example-specific entry points, but they are convenience surfaces rather than the full parent-level definition of the runtime family.

### Python consumer

```text
python -m Implementations.Reference.Runtime.python.cli run 3
python -m Implementations.Reference.Runtime.python.cli ui
python -m Implementations.Reference.Runtime.python.cli ui --host 127.0.0.1 --port 8080 --no-open-browser
```

### Rust consumer

```text
cd Implementations/Reference/Runtime/rust
cargo test
cargo run -- 3
cargo run -- ui
cargo run -- ui --host 127.0.0.1 --port 8080 --no-open-browser
```

### C/C++ consumer

```text
cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp
build/frog_runtime_cpp/frog_reference_runtime_cpp 3
build/frog_runtime_cpp/frog_reference_runtime_cpp ui
build/frog_runtime_cpp/frog_reference_runtime_cpp ui --host 127.0.0.1 --port 8080 --no-open-browser
```

## Multi-runtime posture

At the current published code state, all three consumer languages expose:

- a headless execution path for the bounded contract,
- a browser-host UI path driven by the same bounded runtime core,
- a repository-visible test or build posture,
- the same example-local contract and `.wfrog` package corridor,
- the same acceptance-driven reading posture.

That does not mean the three implementations are already identical in tooling maturity. It means the first common runtime-family slice is now visible across Python, Rust, and C/C++ at the level of published code structure, command surfaces, and shared acceptance artifacts.

## What this directory owns

This directory owns runtime-family concerns only:

- contract consumption after backend-family handoff,
- runtime-private state and scheduling mechanics,
- runtime-private success and failure reporting,
- minimal host-side UI realization for the accepted slice,
- shared acceptance alignment across the Python, Rust, and C/C++ consumers,
- coordination between the Python, Rust, and C/C++ consumers.

## What this directory does not own

This directory does not own:

- the language,
- the canonical `.frog` source model,
- semantic acceptance,
- FIR,
- lowering,
- the backend-contract boundary,
- widget-law ownership,
- compiler-family behavior,
- LLVM-native executable definition.

```text
runtime-family consumer != language definition
runtime-private structures != backend contract
browser-host UI != native compiled UI closure
```
