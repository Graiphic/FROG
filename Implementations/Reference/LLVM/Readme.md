<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference LLVM-Oriented Path</h1>

<p align="center">
  <strong>Downstream compiler-family proof path for native executable closure in the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory contains the current LLVM-oriented proof path for the FROG reference implementation workspace.
It exists to prove that selected lowered artifacts can be consumed by a native compiler-family path without making LLVM the definition of FROG semantics.
</p>

<p>
The correct current path is:
</p>

<pre><code>.frog
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM module / native proof
</code></pre>

<p>
LLVM consumes lowering artifacts.
It does not consume canonical source or FIR directly.
</p>

<hr/>

<h2>Current LLVM Example Surface</h2>

<ul>
  <li><code>examples/01_pure_addition/</code></li>
  <li><code>examples/02_ui_value_roundtrip/</code></li>
  <li><code>examples/03_ui_property_write/</code></li>
  <li><code>examples/04_stateful_feedback_delay/</code></li>
  <li><code>examples/05_bounded_ui_accumulator/</code></li>
</ul>

<hr/>

<h2>Generic Lowering-to-LLVM Entry Point</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/01_pure_addition/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/01_pure_addition/module.ll \
  --check
</code></pre>

<p>
The generic emitter dispatches by <code>lowered_units[0].kind</code>.
</p>

<hr/>

<h2>Runtime Kernel Bridge Relationship</h2>

<p>
The runtime kernel bridge direction is documented in <a href="../Runtime/KernelBridge.md"><code>Implementations/Reference/Runtime/KernelBridge.md</code></a>.
That bridge keeps the runtime compiler-agnostic: the runtime hosts execution and UI, while compiler-family paths produce artifacts that can be consumed through explicit manifests and stable ABI surfaces.
</p>

<p>
For the first native bridge milestone, LLVM may produce the Example 05 native kernel artifact and its manifest.
That does not make LLVM a runtime dependency.
LLVM remains a backend-family producer of native-oriented material, and the runtime consumes the resulting manifest-defined artifact.
</p>

<pre><code>lowering
  -&gt; LLVM-oriented backend path
       -&gt; native kernel artifact
       -&gt; native kernel manifest

runtime
  -&gt; manifest-declared kernel
  -&gt; ABI call
  -&gt; diagnostics and snapshots
</code></pre>

<hr/>

<h2>Rule Family Documentation</h2>

<ul>
  <li><a href="./rules/Readme.md">Reference lowering-to-LLVM rule-family index</a></li>
</ul>

<hr/>

<h2>Supported Lowered Unit Kinds</h2>

<table>
  <thead>
    <tr>
      <th>Lowered unit kind</th>
      <th>Published example</th>
      <th>Proof posture</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>pure_addition_kernel</code></td><td><code>01_pure_addition</code></td><td>Native proof for pure <code>f64</code> addition.</td></tr>
    <tr><td><code>ui_value_roundtrip_kernel</code></td><td><code>02_ui_value_roundtrip</code></td><td>Native proof payload for widget-value arithmetic.</td></tr>
    <tr><td><code>ui_property_write_effect_unit</code></td><td><code>03_ui_property_write</code></td><td>Native proof payload for explicit UI property effect.</td></tr>
    <tr><td><code>stateful_feedback_delay_kernel</code></td><td><code>04_stateful_feedback_delay</code></td><td>Native proof for one explicit delay-backed state step.</td></tr>
    <tr><td><code>bounded_accumulator_kernel_with_ui_bindings</code></td><td><code>05_bounded_ui_accumulator</code></td><td>Native proof for bounded <code>u16</code> accumulation with overflow rejection.</td></tr>
  </tbody>
</table>

<hr/>

<h2>Compatibility Wrappers</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_examples01_04_llvm_modules.py --check
python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check
</code></pre>

<p>
These wrappers are kept for continuity, but both now delegate to the generic lowered-unit-kind emitter.
</p>

<hr/>

<h2>Native Build</h2>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py --include-llvm-build</code></pre>

<p>
The native build path requires <code>clang</code>.
It verifies the published native proof dossiers for Examples 01–05 when the toolchain is available.
</p>

<hr/>

<h2>Boundary</h2>

<p>
LLVM remains downstream from FROG.
These examples are native proof dossiers, not a generalized production backend.
The LLVM-oriented path must not redefine canonical source, validated meaning, FIR, lowering law, backend contracts, runtime behavior, widget law, or UI host behavior.
</p>

<p>
When the native kernel bridge is implemented, LLVM should remain a compiler-family producer of manifest-declared kernel artifacts rather than becoming part of the runtime definition.
</p>
