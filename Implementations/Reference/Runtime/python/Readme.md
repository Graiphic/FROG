# Reference Runtime (Python)

Python realization of the published `reference_host_runtime_ui_binding` family

Repository governance and publication state are centralized in [`Versioning/Readme.md`](../../../Versioning/Readme.md).

* * *

## Directory navigation

```text
Implementations/Reference/Runtime/python/
├── Readme.md
├── __init__.py
├── cli.py
├── execute_contract.py
├── run_slice05_ui.py
├── runtime_core.py
├── ui_runtime.py
└── tests/
    ├── test_runtime_slice05.py
    └── test_runtime_ui_slice05.py
```

Generated cache directories are not part of the intended source surface of this runtime and should not be versioned.

* * *

## Role

This directory contains the Python consumer for the published Example 05 runtime-family corridor.

It accepts the emitted backend contract, loads the published `.wfrog` package, resolves the referenced SVG assets, executes the bounded kernel, and can expose the panel through a minimal browser-host UI.

This directory is downstream from:

```text
main.frog
  -> main.fir.json
  -> main.lowering.json
  -> reference_host_runtime_ui_binding contract
  -> shared runtime-family acceptance
```

It is not a source parser, not a lowering stage, and not a language-definition layer.

* * *

## Current published entry points

### `cli.py`

The main entry point for this directory. It exposes two modes:

- `run` — headless contract + `.wfrog` execution,
- `ui` — browser-host UI service for the same runtime core.

Examples from the repository root:

```text
python -m Implementations.Reference.Runtime.python.cli run 3
python -m Implementations.Reference.Runtime.python.cli ui
python -m Implementations.Reference.Runtime.python.cli ui --host 127.0.0.1 --port 8080 --no-open-browser
```

### `execute_contract.py`

Headless execution entry used by tests and helper wrappers.

### `ui_runtime.py`

Browser-host runtime surface for the same bounded Example 05 corridor.

* * *

## Current published test surface

The currently published Python test surface is acceptance-driven and aligned with the shared runtime-family artifacts:

- `tests/test_runtime_slice05.py`
- `tests/test_runtime_ui_slice05.py`

These tests are expected to remain aligned with:

- `Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json`,
- `Implementations/Reference/Runtime/acceptance/example05_input_3.snapshot.json`.

* * *

## Required host-binding capabilities

The current published Python runtime expects the Example 05 host binding to require:

- `window`,
- `basic_widget_rendering`,
- `property_write`,
- `widget_value_binding`,
- `widget_reference_binding`.

Those capabilities match the currently published `.wfrog` package and the emitted backend contract for the bounded slice.

* * *

## Freeze posture

For the current published repository state, this runtime should be read as one downstream consumer of the frozen Example 05 reference corridor.

Routine maintenance may improve portability, CI, or implementation clarity, but it should preserve the shared acceptance posture and the observable Example 05 result.

* * *

## Summary

This directory is the Python runtime-family consumer for the current frozen bounded Example 05 corridor.

Its job is to consume the published contract and `.wfrog` package, not to redefine the language or the corridor itself.
