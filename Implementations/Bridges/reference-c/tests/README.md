The public corpus is `../examples/corpus.json`. It contains six positive
Source 0.1 documents and twelve finite activation oracles: two core Add,
two exact six-scalar/wrap64, four bounded Array32 and four static For cases.
For counts are 0, 3 and 32; the second For3 activation crosses the signed
64-bit boundary. Both negative Array indices produce an attributed fault
with no partial output publication. Two further public Sources are valid for
the common Frontend but must be refused by the C bridge before compilation:
an Enum full schema with a physical U32 carrier, and the real wrap64 subtraction
operation outside the finite Add-only profile. Their exact attributed refusal
codes, object IDs, Source pointers and input/FIR hashes are in `negative_sources`.

The expected JSON files contain mathematical literals written independently
of either backend. They cover every root output port and every root indicator
input `value`. For exposes only its shift-register result port; iteration,
count-value and history-read ports remain internal. I64/U64 observations must
be integer JSON tokens. The comparator never converts those carriers to float.
F64 comparison uses IEEE binary64 bits, and Array comparison checks its exact
rank-one shape, all elements and the maximum32 bound. Human diagnostic message
text must be nonempty; its code, object identity and Source pointer are exact.

The inputs use `frog.bridge.kernel.inputs` version0.1 and may override declared
widget controls only. The outputs use `frog.bridge.kernel.outputs` version0.1.
Array values retain `{ "shape": [N], "elements": [...] }` and canonical
`array<f64>` type. No graph evaluator or replacement Frontend is in these tests.

Comparison command:

```text
python -B tests/check_outputs.py --expected examples/expected/scalar-default.json --actual C-output.json --compare LLVM-output.json
```

`frontend-probe/` preserves actual public CLI derivations, source maps,
diagnostics and API3 provenance for all eight final Sources. The probe requested Source
family `frog.source` version0.1, FIR version0.1 and separate Source/FIR document
IDs declared in the corpus. All eight final derivations returned success with empty
diagnostics. APPDATA, LOCALAPPDATA, TEMP and TMP were isolated under the probe
directory. This evidence demonstrates Source/FIR admission only. Neither the C
bridge nor LLVM execution or compilation has been run for this corpus here.
One earlier unlisted nominal probe omitted the explicit composed-scope profile
and was rejected by the Frontend. Its exact Source and diagnostics remain in
`frontend-probe/unsupported-nominal-u32.source-rejected.frog` and the matching
diagnostics file. It is not a successful bridge-refusal witness. The final
nominal document is distinct and retains the original public Enum schema on
both the FIR object and port.

The implementation scope excludes Case, While, Event, Record, String, references,
NativeCall and providers. The public For boundary is the existing explicit
shift-register contract; no artificial Case FIR or backend-specific Source
rewrite is used. Compiler configuration, bridge framing, build and execution
qualification remain owned by the integrator.
