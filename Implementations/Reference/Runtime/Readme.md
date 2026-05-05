<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

# Reference Runtime

<p>Runtime-family consumers and contract-driven acceptance checks for the non-normative FROG reference implementation.</p>
<p>FROG — Free Open Graphical Language</p>

---

## Navigation

- Parent reference implementation: [`../Readme.md`](../Readme.md)
- Shared acceptance material: [`acceptance/Readme.md`](acceptance/Readme.md)
- Generic contract executor: [`contract_executor.py`](contract_executor.py)
- Contract execution CLI: [`execute_reference_contract.py`](execute_reference_contract.py)
- Examples 01–05 runtime check: [`check_examples01_05_runtime_acceptance.py`](check_examples01_05_runtime_acceptance.py)
- Example 05 specialized runtime-family check: [`check_example05_runtime_acceptance.py`](check_example05_runtime_acceptance.py)
- Python consumer: [`python/Readme.md`](python/Readme.md)
- Rust consumer: [`rust/Readme.md`](rust/Readme.md)
- C/C++ consumer: [`cpp/Readme.md`](cpp/Readme.md)

## Overview

This directory is the parent coordination point for runtime-family checks in the non-normative reference implementation.

The current runtime surface has two levels:

```text
Examples 01–04
  backend contract
    -> generic contract executor
    -> runtime acceptance snapshot

Example 05
  backend contract + .wfrog package
    -> specialized runtime-family acceptance checker
    -> richer UI/state/overflow acceptance snapshot
```

## Generic contract executor

The generic executor lives in:

```text
Implementations/Reference/Runtime/contract_executor.py
```

It is driven by backend-contract unit kind rather than by example id.

Supported unit kinds:

```text
pure_addition_kernel
ui_value_roundtrip_kernel
ui_property_write_effect_unit
stateful_feedback_delay_kernel
```

The goal is to move runtime acceptance for simple slices from:

```text
if example_id == ...
```

toward:

```text
contract JSON -> unit.kind -> executor -> snapshot
```

## Commands

Check Examples 01–05 runtime acceptance:

```text
python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py
```

Check one simple acceptance through the generic contract executor:

```text
python Implementations/Reference/Runtime/execute_reference_contract.py \
  --acceptance Implementations/Reference/Runtime/acceptance/example01_pure_addition.acceptance.json \
  --check
```

## Boundary

This directory does not define FROG semantics.
It consumes emitted backend contracts and verifies repository-visible runtime acceptance.
