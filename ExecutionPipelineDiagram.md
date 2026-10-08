# End-to-End Execution Pipeline Diagram

```text
+----------------------+
| .frog                |
| canonical source     |
+----------+-----------+
           |
           v
+----------------------+
| structural validation|
+----------+-----------+
           |
           v
+----------------------+
| semantic validation  |
+----------+-----------+
           |
           v
+----------------------+
| FIR                  |
| open Execution IR    |
+------+-------+-------+
       |       |
       |       +----------------------------+
       |                                    |
       v                                    v
+----------------------+        +----------------------+
| lowering             |        | widget realization   |
| backend contract     |        | .wfrog + SVG         |
+----------+-----------+        +----------+-----------+
           |                               |
           v                               v
+----------------------+        +----------------------+
| LLVM backend         |        | UI host              |
| native artifact      |        | replaceable host     |
+----------+-----------+        +----------+-----------+
           |                               |
           +---------------+---------------+
                           |
                           v
                 +----------------------+
                 | runtime              |
                 | orchestration        |
                 | bindings             |
                 | scheduling           |
                 | diagnostics          |
                 +----------------------+
```

This diagram is a compact reading aid for the public FROG pipeline. It does not redefine FROG semantics.

## Engineering lifecycle context

This complementary view does not replace the execution pipeline above. Its
sideways relationships are artifact references, not executable dataflow edges.
Runtime, compiler and hybrid realization remain downstream of lowering and the
backend contract; the compact LLVM/UI example above is not a mandatory packaging model.

```text
External intent / requirements / test baselines
       | explicit references and reviewed authoring
       v
Canonical .frog source --------------------> Associated links and evidence
       | loadability / structural / semantic validation       ^
       v                                                       |
Validated meaning -> FIR -------------------- source mapping --+
       | lowering / backend contract                           |
       v                                                       |
Runtime / compiler / hybrid artifact ---------- build identity +
       |                                                       |
       v                                                       |
Actual execution instance ---------------- observations / runs +
                                                               |
                                                               v
                                                Versioned acceptance policy
```

The [architecture](FROG-Architecture.md#engineering-lifecycle-and-digital-thread-integration)
separates program execution, traceability/evidence and organizational authorization.
The [execution eligibility gates](Language/Execution%20eligibility.md) remain in
force. A missing external proof may block release policy without being a language
error; a successful test does not itself authorize deployment.

The [associated-document proposal](docs/proposals/engineering-traceability.md)
is non-normative. No lifecycle tool, network service or new source field is
required. Source/FIR mapping retains its existing obligations. A feedback record
may propose a source change; it does not silently modify the active target.
