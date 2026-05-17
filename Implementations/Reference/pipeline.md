<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Pipeline</h1>

<p align="center">
  <strong>Executable pipeline for the Examples 01-15 public reference runtime closure</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Pipeline</h2>

<pre><code>Examples 01-15
  widget layer validation
    -&gt; .frog
    -&gt; FIR
    -&gt; lowering
    -&gt; backend contract
    -&gt; runtime acceptance
    -&gt; LLVM module / native proof
</code></pre>

<p>
The widget-layer validator is included by default because the current Examples 01-15 corridor depends on a stable published UI/widget layer, especially through <code>widget_value</code>, <code>widget_reference</code>, <code>frog.ui.property_write</code>, the Example 05 front-panel package, and the Example 06-15 Default realization packages.
</p>

<p>
This named pipeline keeps the public reference runtime closure reproducible at
the source/FIR/lowering/contract/LLVM boundary. The historical runtime
acceptance stage still covers Examples <code>01</code> through <code>10</code>;
Examples <code>11</code> through <code>15</code> are additionally covered by
runtime-family tests and native-kernel bridge checks documented under
<code>Implementations/Reference/Runtime/</code>.
</p>

<hr/>

<h2>Command</h2>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_10_full.py</code></pre>

<p>
With native LLVM builds:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_10_full.py --include-llvm-build</code></pre>

<p>
To validate only the widget layer through the workspace entry point:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py --widget-layer-only</code></pre>

<p>
To skip widget validation for narrow debugging of unrelated stages:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_10_full.py --skip-widget-validator</code></pre>

<hr/>

<h2>Stage Order</h2>

<ol>
  <li>Artifact preflight for Examples 01-10.</li>
  <li>Widget-layer validation.</li>
  <li><code>.frog -&gt; FIR</code> for Examples 01-15.</li>
  <li><code>FIR -&gt; lowering</code> for Examples 01-15.</li>
  <li><code>lowering -&gt; backend contract</code> for Examples 01-04 through the shared contract emitter.</li>
  <li><code>lowering -&gt; backend contract</code> for Examples 05-15 through the host-runtime UI-binding contract emitter.</li>
  <li><code>contract -&gt; runtime acceptance</code> for the historical Examples 01-10 acceptance surface.</li>
  <li><code>lowering -&gt; LLVM module</code> for Examples 01-15.</li>
  <li>Optional native LLVM build proof.</li>
</ol>

<hr/>

<h2>Current Rule-Handoff Discipline</h2>

<pre><code>.frog source pattern
  -&gt; FIR unit.kind
  -&gt; lowered_unit.kind
  -&gt; backend/runtime/LLVM consumers
</code></pre>

<p>
The reference pipeline protects this handoff discipline by checking source-to-FIR derivation, FIR-to-lowering projection, backend-contract emission, runtime acceptance, and lowered-unit-kind LLVM module emission.
</p>

<hr/>

<h2>Widget-Layer Validation Scope</h2>

<p>
The widget-layer validation stage checks repository hygiene for:
</p>

<ul>
  <li>widget class-law documents under <code>Libraries/Widgets/</code>,</li>
  <li>Default realization documents under <code>Libraries/Realizations/Default/</code>,</li>
  <li><code>*.default.wfrog</code> manifests,</li>
  <li>target class references,</li>
  <li>resource references,</li>
  <li>SVG <code>data-frog-part</code> markers,</li>
  <li>public-part alignment,</li>
  <li>part / property / method / event binding posture,</li>
  <li>composition resource references,</li>
  <li>host-native replacement boundaries.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
The pipeline is non-normative.
It verifies published artifacts and implementation-stage coherence.
It does not define the FROG language, widget semantics, realization semantics, runtime semantics, backend contract semantics, or compiler semantics.
</p>
