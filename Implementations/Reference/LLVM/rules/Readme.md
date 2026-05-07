<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Lowering-to-LLVM Rule Families</h1>

<p align="center">
  <strong>Bounded lowered-unit-kind LLVM proof emitters used by the non-normative reference workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory documents the bounded lowering-to-LLVM rule families currently recognized by the reference LLVM-oriented path.
The purpose is to keep the <code>lowering -&gt; LLVM</code> corridor explicit while preserving the architectural rule that LLVM consumes lowering artifacts, not FIR or canonical source directly.
</p>

<p>
These documents are non-normative reference-implementation material.
They do not replace <code>IR/Lowering.md</code>, <code>IR/Backend contract.md</code>, or any future production compiler backend specification.
</p>

<hr/>

<h2>Current Emission Posture</h2>

<pre><code>lowering artifact
  -&gt; artifact_kind == frog_lowered_unit
  -&gt; exactly one lowered unit
  -&gt; lowered_units[0].kind
  -&gt; bounded LLVM emitter
  -&gt; module.ll
  -&gt; optional native build proof
  -&gt; expected-output.json
</code></pre>

<p>
The reference LLVM emitter dispatches by <code>lowered_units[0].kind</code>.
The source example identifier remains preserved in <code>source_ref</code>, but it is not the LLVM-emitter authority.
</p>

<hr/>

<h2>Current LLVM Emitter Families</h2>

<table>
  <thead>
    <tr>
      <th>Emitter family</th>
      <th>Lowered unit kind</th>
      <th>Published example</th>
      <th>Native proof posture</th>
      <th>Current limitation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Pure floating-point addition</td>
      <td><code>pure_addition_kernel</code></td>
      <td><code>01_pure_addition</code></td>
      <td>Emits a native proof function that computes <code>f64 a + b</code> and prints result/status.</td>
      <td>Only the published two-input addition shape is supported.</td>
    </tr>
    <tr>
      <td>Widget-value arithmetic proof</td>
      <td><code>ui_value_roundtrip_kernel</code></td>
      <td><code>02_ui_value_roundtrip</code></td>
      <td>Emits a native proof that treats widget values as proof inputs and prints the indicator value/status.</td>
      <td>No native UI host is emitted; widget participation is represented as proof payload.</td>
    </tr>
    <tr>
      <td>UI property-effect proof</td>
      <td><code>ui_property_write_effect_unit</code></td>
      <td><code>03_ui_property_write</code></td>
      <td>Emits a native proof that prints the affected widget, member, value, and status.</td>
      <td>No native UI mutation host is emitted; the effect is represented as proof payload.</td>
    </tr>
    <tr>
      <td>Explicit delay step proof</td>
      <td><code>stateful_feedback_delay_kernel</code></td>
      <td><code>04_stateful_feedback_delay</code></td>
      <td>Emits a native proof for one explicit delay-backed state step.</td>
      <td>Only the published <code>delay_1</code> shape is supported.</td>
    </tr>
    <tr>
      <td>Bounded accumulator proof</td>
      <td><code>bounded_accumulator_kernel_with_ui_bindings</code></td>
      <td><code>05_bounded_ui_accumulator</code></td>
      <td>Emits a native bounded accumulator proof with <code>u16</code> overflow rejection.</td>
      <td>UI binding is proof payload; no native front-panel renderer is emitted.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Shared LLVM Emission Invariants</h2>

<ul>
  <li>The input artifact must declare <code>artifact_kind = frog_lowered_unit</code>.</li>
  <li>The input artifact must contain exactly one lowered unit.</li>
  <li>The lowered unit must expose a string <code>kind</code>.</li>
  <li>The lowered unit kind must match a registered LLVM emitter.</li>
  <li>The emitted module is a proof artifact, not a production backend artifact.</li>
  <li>The emitted module must be reproducible against the published <code>module.ll</code> for the corresponding example.</li>
  <li>The optional native build must be checked against <code>expected-output.json</code> when the build toolchain is available.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
The LLVM-oriented path is downstream from lowering.
It must not consume canonical source or FIR directly, must not redefine FROG semantics, and must not claim general production compiler coverage.
</p>

<pre><code>.frog
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM proof
</code></pre>

<hr/>

<h2>Out of Scope</h2>

<ul>
  <li>general LLVM backend completeness,</li>
  <li>general graph scheduling,</li>
  <li>native UI host generation,</li>
  <li>widget realization rendering,</li>
  <li>arbitrary type lowering,</li>
  <li>production ABI definition,</li>
  <li>optimization passes,</li>
  <li>multi-unit linking.</li>
</ul>

<hr/>

<h2>Next Stabilization Direction</h2>

<p>
The next safe improvement is to keep the current emitters but progressively separate reusable proof-emission helpers from example-shaped emitter functions:
</p>

<pre><code>lowered_unit.kind
  -&gt; shared lowered-kernel validation
  -&gt; shared LLVM text construction helpers
  -&gt; family-specific proof module emission
</code></pre>
