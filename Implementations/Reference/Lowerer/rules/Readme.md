<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference FIR-to-Lowering Rule Families</h1>

<p align="center">
  <strong>Bounded FIR-unit-kind lowering families used by the non-normative reference lowerer</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory documents the bounded FIR-to-lowering rule families currently recognized by the reference lowerer.
The purpose is to keep the <code>FIR -&gt; lowering</code> corridor explicit while avoiding the weaker posture where lowering is selected only by an example identifier.
</p>

<p>
These documents are non-normative reference-implementation material.
They do not replace <code>IR/Lowering.md</code>, <code>IR/Backend contract.md</code>, <code>IR/Execution IR.md</code>, or the canonical source and language layers.
</p>

<hr/>

<h2>Current Rule-Selection Posture</h2>

<pre><code>FIR artifact
  -&gt; artifact_kind == frog_fir_unit
  -&gt; exactly one FIR unit
  -&gt; FIR units[0].kind
  -&gt; matching bounded lowering rule
  -&gt; lowered_unit.kind
  -&gt; backend / runtime / LLVM-compatible lowered surface
</code></pre>

<p>
The reference lowerer dispatches by <code>units[0].kind</code>.
The source example identifier remains preserved in <code>source_ref</code>, but it is no longer the lowering-rule authority.
</p>

<hr/>

<h2>Current Lowering Rule Families</h2>

<table>
  <thead>
    <tr>
      <th>Lowering family</th>
      <th>Current rule id</th>
      <th>Input FIR unit kind</th>
      <th>Output lowered unit kind</th>
      <th>Backend-family target</th>
      <th>Compiler-family target</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Pure public-interface arithmetic</td>
      <td><code>lower_pure_dataflow_arithmetic</code></td>
      <td><code>pure_dataflow_arithmetic_unit</code></td>
      <td><code>pure_addition_kernel</code></td>
      <td><code>reference_pure_dataflow_arithmetic</code></td>
      <td><code>llvm_oriented_native_path</code></td>
    </tr>
    <tr>
      <td>Natural widget-value dataflow</td>
      <td><code>lower_ui_value_roundtrip</code></td>
      <td><code>ui_value_roundtrip_unit</code></td>
      <td><code>ui_value_roundtrip_kernel</code></td>
      <td><code>reference_ui_value_roundtrip</code></td>
      <td>None currently declared</td>
    </tr>
    <tr>
      <td>Object-style UI property effect</td>
      <td><code>lower_ui_property_write</code></td>
      <td><code>ui_property_write_unit</code></td>
      <td><code>ui_property_write_effect_unit</code></td>
      <td><code>reference_ui_property_write</code></td>
      <td>None currently declared</td>
    </tr>
    <tr>
      <td>Explicit delay-backed state</td>
      <td><code>lower_stateful_feedback_delay</code></td>
      <td><code>stateful_feedback_delay_unit</code></td>
      <td><code>stateful_feedback_delay_kernel</code></td>
      <td><code>reference_stateful_feedback_delay</code></td>
      <td><code>llvm_oriented_native_path</code></td>
    </tr>
    <tr>
      <td>Bounded stateful UI</td>
      <td><code>lower_bounded_stateful_ui</code></td>
      <td><code>bounded_stateful_ui_unit</code></td>
      <td><code>bounded_accumulator_kernel_with_ui_bindings</code></td>
      <td><code>reference_host_runtime_ui_binding</code></td>
      <td><code>llvm_oriented_native_path</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Shared Lowering Invariants</h2>

<ul>
  <li>The input artifact must declare <code>artifact_kind = frog_fir_unit</code>.</li>
  <li>The input artifact must contain exactly one FIR unit.</li>
  <li>The FIR unit must expose a string <code>kind</code>.</li>
  <li>The FIR unit kind must match exactly one registered lowering rule.</li>
  <li>The output artifact must declare <code>artifact_kind = frog_lowered_unit</code>.</li>
  <li>The output artifact must carry <code>artifact_governance_ref</code> pointing to centralized version governance.</li>
  <li>The output artifact must preserve <code>source_ref</code>.</li>
  <li>The output artifact must carry <code>fir_ref</code> pointing back to the input FIR artifact and unit id.</li>
  <li>The output artifact must contain exactly one lowered unit for the current bounded corridor.</li>
  <li>The lowered unit kind must match the registered rule's declared output kind.</li>
</ul>

<hr/>

<h2>Family Boundaries</h2>

<h3>Pure public-interface arithmetic</h3>

<p>
This family projects a pure FIR arithmetic unit into a compact executable kernel with public IO, one add operation, and one final publication.
It is consumable by the generic runtime contract executor and the LLVM-oriented native proof path.
</p>

<h3>Natural widget-value dataflow</h3>

<p>
This family preserves widget-value bindings while projecting the computation into a compact UI-value kernel.
It keeps widget-value participation distinct from public interface IO.
</p>

<h3>Object-style UI property effect</h3>

<p>
This family projects an object-style UI effect into an explicit lowered effect unit.
It keeps <code>widget_reference</code> support distinct from <code>frog.ui.property_write</code> execution effects.
</p>

<h3>Explicit delay-backed state</h3>

<p>
This family projects explicit delay-backed state into a stateful lowered kernel with initial state, state type, step body, commit rule, and final publication.
It must not infer state where the FIR does not provide explicit state.
</p>

<h3>Bounded stateful UI</h3>

<p>
This family projects the bounded accumulator FIR into a lowered unit that can be consumed by the reference host-runtime UI-binding backend and the LLVM-oriented native proof path.
It preserves public IO, widget bindings, explicit state, iteration count, iteration body, commit rule, and final publications.
</p>

<hr/>

<h2>Failure Modes</h2>

<ul>
  <li><strong>Invalid artifact kind:</strong> lowering fails before rule selection.</li>
  <li><strong>Missing or non-string FIR unit kind:</strong> lowering fails before rule selection.</li>
  <li><strong>Unsupported FIR unit kind:</strong> lowering fails with the list of supported unit kinds.</li>
  <li><strong>Ambiguous FIR unit kind:</strong> lowering fails if multiple registered rules claim the same FIR unit kind.</li>
  <li><strong>Malformed FIR unit structure:</strong> the selected rule fails before producing a lowered artifact.</li>
  <li><strong>Lowered kind mismatch:</strong> lowering fails if a rule produces a lowered unit kind different from the registered output kind.</li>
</ul>

<hr/>

<h2>Out of Scope</h2>

<ul>
  <li>general lowering for arbitrary FIR units,</li>
  <li>backend-specific optimization,</li>
  <li>runtime scheduling,</li>
  <li>native ABI definition,</li>
  <li>general UI host lowering,</li>
  <li>general widget realization lowering,</li>
  <li>LLVM code generation itself,</li>
  <li>backend-contract generation itself.</li>
</ul>

<hr/>

<h2>Next Stabilization Direction</h2>

<p>
The next safe improvement is to keep the current rule families but progressively factor repeated lowering concepts out of example-shaped functions:
</p>

<pre><code>FIR unit kind
  -&gt; shared validation helpers
  -&gt; shared lowered artifact construction helpers
  -&gt; narrower family-specific kernel projection
</code></pre>

<p>
Only after that step should the reference implementation attempt broader lowering behavior.
</p>
