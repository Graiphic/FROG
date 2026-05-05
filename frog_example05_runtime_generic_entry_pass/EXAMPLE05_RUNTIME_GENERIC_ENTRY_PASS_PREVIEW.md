# Example 05 runtime generic entry pass

This pack implements Step 5.1/5.2:

```text
Example 05
  contract JSON + .wfrog support artifact
    -> generic contract executor
    -> same published runtime snapshot
```

What changes:

- adds `bounded_executable_ui_unit` support to `Runtime/contract_executor.py`;
- keeps Examples 01–4 generic executor behavior;
- keeps the specialized Example 05 runtime checker active in parallel;
- updates `execute_reference_contract.py` so it can load `wfrog_path` from acceptance artifacts;
- updates `check_examples01_05_runtime_acceptance.py` to check Example 05 generically and then specialized by default;
- adds tests for Example 05 generic snapshot parity and overflow rejection.


## Files

- `Implementations/Reference/Runtime/Readme.md`
- `Implementations/Reference/Runtime/acceptance/Readme.md`
- `Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py`
- `Implementations/Reference/Runtime/contract_executor.py`
- `Implementations/Reference/Runtime/execute_reference_contract.py`
- `Implementations/Reference/Runtime/tests/test_contract_executor.py`
- `Implementations/Reference/Runtime/tests/test_examples01_05_runtime_acceptance.py`
- `Implementations/Reference/checks.md`
