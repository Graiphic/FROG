<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

# Reference Runtime

<p>Runtime-family consumers and contract-driven acceptance checks for the non-normative FROG reference implementation.</p>
<p>FROG — Free Open Graphical Language</p>

---

## Overview

This directory coordinates runtime-family checks in the non-normative reference implementation.

The current runtime surface has two levels:

```text
Examples 01–04
  backend contract
    -> generic contract executor
    -> runtime acceptance snapshot

Example 05
  normalized backend contract + .wfrog package
    -> generic contract executor
    -> specialized runtime-family checker
    -> richer UI/state/overflow acceptance snapshot
```

## Normalized Example 05 Runtime

The specialized Python runtime and the generic contract executor now consume the normalized Example 05 contract surface:

```text
public_io
ui_bindings
execution_kernel
effects
publications
```

The legacy compatibility fields have been removed from the published Example 05 contract.

## Commands

```text
python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py
python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py --skip-specialized-example05
python Implementations/Reference/Runtime/python/execute_contract.py 3
python Implementations/Reference/Runtime/execute_reference_contract.py \
  --acceptance Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json \
  --check
```

## Boundary

This directory does not define FROG semantics.
It consumes emitted backend contracts and verifies repository-visible runtime acceptance.
