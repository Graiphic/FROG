<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference FIR Derivation Rule Families</h1>

<p align="center">
  <strong>Bounded source-pattern families used by the non-normative reference deriver</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory documents the bounded source-pattern rules currently recognized by the reference deriver.
The purpose is to keep the <code>.frog -&gt; FIR</code> corridor explicit while avoiding the weaker posture where derivation is selected only by an example identifier.
</p>

<p>
These documents are non-normative reference-implementation material.
They do not replace <code>IR/Derivation rules.md</code>, <code>IR/Execution IR.md</code>, <code>Expression/</code>, or <code>Language/</code>.
</p>

<hr/>

<h2>Current Rule-Selection Posture</h2>

<pre><code>canonical .frog source
  -&gt; source structure inspection
  -&gt; bounded source-pattern recognition
  -&gt; exactly one matching derivation rule
  -&gt; published FIR artifact
</code></pre>

<p>
The reference deriver attempts all registered rules and accepts a source only when exactly one rule matches.
The <code>metadata.name</code> field may be reported in diagnostics, but it is not the authority used to select the derivation rule.
</p>

<hr/>

<h2>Current Rule Families</h2>

<table>
  <thead>
    <tr>
      <th>Rule family</th>
      <th>Current rule id</th>
      <th>Published example</th>
      <th>Recognized source shape</th>
      <th>Derived FIR unit kind</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Pure public-interface arithmetic</td>
      <td><code>pure_public_addition</code></td>
      <td><code>01_pure_addition</code></td>
      <td>Two public <code>f64</code> inputs connected to one <code>frog.core.add</code> primitive and one public <code>f64</code> output.</td>
      <td><code>pure_dataflow_arithmetic_unit</code></td>
    </tr>
    <tr>
      <td>Natural widget-value dataflow</td>
      <td><code>ui_value_roundtrip</code></td>
      <td><code>02_ui_value_roundtrip</code></td>
      <td>Two control <code>widget_value</code> sources connected to <code>frog.core.add</code>, with result published to one indicator <code>widget_value</code>.</td>
      <td><code>ui_value_roundtrip_unit</code></td>
    </tr>
    <tr>
      <td>Object-style UI property effect</td>
      <td><code>ui_property_write</code></td>
      <td><code>03_ui_property_write</code></td>
      <td>One public string input and one widget reference feeding one <code>frog.ui.property_write</code> effect.</td>
      <td><code>ui_property_write_unit</code></td>
    </tr>
    <tr>
      <td>Explicit delay-backed state</td>
      <td><code>stateful_feedback_delay</code></td>
      <td><code>04_stateful_feedback_delay</code></td>
      <td>One explicit <code>frog.core.delay</code> state carrier with initial value, feeding a valid feedback add step.</td>
      <td><code>stateful_feedback_delay_unit</code></td>
    </tr>
    <tr>
      <td>Bounded UI accumulator</td>
      <td><code>bounded_ui_accumulator</code></td>
      <td><code>05_bounded_ui_accumulator</code></td>
      <td>One public <code>u16</code> input, one public <code>u16</code> output, widget-value bindings, property writes, bounded loop, and explicit accumulator state.</td>
      <td><code>bounded_stateful_ui_unit</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Family Boundaries</h2>

<h3>Pure public-interface arithmetic</h3>

<p>
This family derives public-boundary arithmetic graphs without UI participation or explicit state.
It preserves public inputs, public outputs, primitive identity, directed dependency structure, and publication to the public output boundary.
</p>

<h3>Natural widget-value dataflow</h3>

<p>
This family derives ordinary value participation by front-panel widgets.
It preserves the distinction between <code>widget_value</code> participation and public interface participation.
It does not treat widget values as public interface ports.
</p>

<h3>Object-style UI property effect</h3>

<p>
This family derives object-style UI interactions.
It preserves the distinction between <code>widget_reference</code> participation and the actual UI primitive operation, such as <code>frog.ui.property_write</code>.
</p>

<h3>Explicit delay-backed state</h3>

<p>
This family derives a valid feedback cycle only when an explicit local-memory carrier provides the required initial state and commit boundary.
It must not infer hidden state from a cycle that lacks such a carrier.
</p>

<h3>Bounded UI accumulator</h3>

<p>
This family composes the earlier concerns in a bounded form: public IO, widget-value binding, widget-reference property writes, bounded loop posture, and explicit accumulator state.
It is the richest current source-to-FIR derivation slice, but it remains deliberately narrow.
</p>

<hr/>

<h2>Preconditions Shared by the Current Rules</h2>

<ul>
  <li>The source must be loadable JSON.</li>
  <li>The source must expose <code>metadata</code>, <code>interface</code>, and <code>diagram</code> objects as required by the current examples.</li>
  <li>The <code>diagram.nodes</code> and <code>diagram.edges</code> collections must be arrays.</li>
  <li>Node identifiers must be unique strings.</li>
  <li>Edge endpoints must expose string <code>node</code> and <code>port</code> members.</li>
  <li>Each recognized rule must find its required nodes, edges, public interface shape, widget-value participants, property-write participants, or explicit state carriers.</li>
</ul>

<hr/>

<h2>Failure Modes</h2>

<ul>
  <li><strong>No matching rule:</strong> derivation fails with diagnostics from all attempted rules.</li>
  <li><strong>Multiple matching rules:</strong> derivation fails as ambiguous.</li>
  <li><strong>Malformed source structure:</strong> derivation fails before a FIR artifact is produced.</li>
  <li><strong>Unsupported source family:</strong> derivation fails explicitly rather than guessing a FIR shape.</li>
</ul>

<hr/>

<h2>Out of Scope</h2>

<ul>
  <li>general FROG compiler completeness,</li>
  <li>automatic derivation for arbitrary graph shapes,</li>
  <li>general type inference,</li>
  <li>general structured-control lowering,</li>
  <li>general UI host semantics,</li>
  <li>general widget realization consumption,</li>
  <li>runtime scheduling,</li>
  <li>LLVM emission,</li>
  <li>backend contract generation.</li>
</ul>

<hr/>

<h2>Next Stabilization Direction</h2>

<p>
The next safe improvement is to keep the current rule families but progressively factor repeated concepts out of example-shaped functions:
</p>

<pre><code>recognized source family
  -&gt; shared extraction helpers
  -&gt; shared FIR construction helpers
  -&gt; narrower example-specific constants
</code></pre>

<p>
Only after that step should the reference implementation attempt broader derivation behavior.
</p>
