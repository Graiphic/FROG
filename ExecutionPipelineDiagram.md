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
