# Stacked Sequence: diagnose a dependency cycle

Stacked Sequence: diagnose a dependency cycle

This graph reuses the external path from the Flat example. Stacked requires all inputs before its first frame and publishes only at its final exit, so this path is a genuine cycle. The common Frontend must reject it during FIR derivation with IR_SEQUENCE_DEPENDENCY_CYCLE, without producing executable FIR or rewriting the document. A separate test also checks the independent FIR guard on the same canonical graph.

Stacked Sequence: diagnose a dependency cycle

Mode: expected-diagnostic. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
Diagnostic: IR_SEQUENCE_DEPENDENCY_CYCLE

[Open the actual FROG document](main.frog).
