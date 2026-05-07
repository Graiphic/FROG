<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Lowerer</h1>

<p align="center">
  <strong>Non-normative reference lowering workspace for projecting FIR into lowered backend-oriented forms</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory contains the current non-normative reference lowerer for the FROG implementation workspace.
It exists to make the published <code>FIR -&gt; lowering</code> corridor executable and checkable for the bounded Examples 01–05 surface without claiming general lowering completeness.
</p>

<p>
The lowerer is downstream from FIR and upstream from backend contracts, runtime consumers, and compiler-family consumers.
It does not define FROG source semantics, validated meaning, backend contract law, runtime behavior, or LLVM behavior.
</p>

<hr/>

<h2>Current Lowering Posture</h2>

<pre><code>FIR artifact
  -&gt; artifact_kind check
  -&gt; exactly one FIR unit
  -&gt; FIR units[0].kind
  -&gt; bounded FIR-to-lowering rule
  -&gt; lowered_unit.kind
  -&gt; backend / runtime / LLVM-compatible lowered surface
</code></pre>

<p>
Rule selection is based on the FIR unit kind rather than the source example identifier.
The <code>source_ref</code> remains preserved for traceability, but it is not the lowering-rule authority.
</p>

<hr/>

<h2>Supported Lowering Targets</h2>

<ul>
  <li><code>Examples/01_pure_addition/main.fir.json -&gt; main.lowering.json</code></li>
  <li><code>Examples/02_ui_value_roundtrip/main.fir.json -&gt; main.lowering.json</code></li>
  <li><code>Examples/03_ui_property_write/main.fir.json -&gt; main.lowering.json</code></li>
  <li><code>Examples/04_stateful_feedback_delay/main.fir.json -&gt; main.lowering.json</code></li>
  <li><code>Examples/05_bounded_ui_accumulator/main.fir.json -&gt; main.lowering.json</code></li>
</ul>

<hr/>

<h2>Commands</h2>

<h3>Generic FIR-to-lowering check</h3>

<pre><code>python Implementations/Reference/Lowerer/lower_fir.py \
  --fir Examples/01_pure_addition/main.fir.json \
  --expected Examples/01_pure_addition/main.lowering.json \
  --check
</code></pre>

<h3>Example 05 legacy helper</h3>

<pre><code>python Implementations/Reference/Lowerer/lower_example05_fir.py --check</code></pre>

<p>
The legacy helper remains available for focused Example 05 debugging.
The generic <code>lower_fir.py</code> entry point is the preferred path for Examples 01–05.
</p>

<hr/>

<h2>Rule Family Documentation</h2>

<ul>
  <li><a href="./rules/Readme.md">Reference FIR-to-lowering rule-family index</a></li>
</ul>

<hr/>

<h2>Current Family-to-Lowered Mapping</h2>

<table>
  <thead>
    <tr>
      <th>FIR unit kind</th>
      <th>Lowered unit kind</th>
      <th>Backend-family target</th>
      <th>Compiler-family target</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>pure_dataflow_arithmetic_unit</code></td><td><code>pure_addition_kernel</code></td><td><code>reference_pure_dataflow_arithmetic</code></td><td><code>llvm_oriented_native_path</code></td></tr>
    <tr><td><code>ui_value_roundtrip_unit</code></td><td><code>ui_value_roundtrip_kernel</code></td><td><code>reference_ui_value_roundtrip</code></td><td>None currently declared</td></tr>
    <tr><td><code>ui_property_write_unit</code></td><td><code>ui_property_write_effect_unit</code></td><td><code>reference_ui_property_write</code></td><td>None currently declared</td></tr>
    <tr><td><code>stateful_feedback_delay_unit</code></td><td><code>stateful_feedback_delay_kernel</code></td><td><code>reference_stateful_feedback_delay</code></td><td><code>llvm_oriented_native_path</code></td></tr>
    <tr><td><code>bounded_stateful_ui_unit</code></td><td><code>bounded_accumulator_kernel_with_ui_bindings</code></td><td><code>reference_host_runtime_ui_binding</code></td><td><code>llvm_oriented_native_path</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>
The lowerer is a reference implementation tool that protects the current published FIR-to-lowering corridor.
It does not define source law, semantic law, FIR law, backend contract law, runtime behavior, or compiler-family behavior.
</p>
