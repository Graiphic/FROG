<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Runtime Acceptance Artifacts</h1>

<p align="center">
  <strong>Shared non-normative acceptance material for Examples 01–05 runtime closure</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Acceptance Surface</h2>

<ul>
  <li><code>example01_pure_addition.acceptance.json</code></li>
  <li><code>example02_ui_value_roundtrip.acceptance.json</code></li>
  <li><code>example03_ui_property_write.acceptance.json</code></li>
  <li><code>example04_stateful_feedback_delay.acceptance.json</code></li>
  <li><code>example05_runtime_family.acceptance.json</code></li>
</ul>

<hr/>

<h2>Execution Model</h2>

<p>
Examples <code>01</code> through <code>04</code> are checked through the generic reference contract executor:
</p>

<pre><code>contract JSON
  -> unit.kind
  -> generic executor
  -> observed snapshot
  -> compare with published snapshot
</code></pre>

<p>
Example <code>05</code> remains on the richer specialized runtime-family checker because it also covers front-panel package loading, UI bindings, and bounded <code>u16</code> overflow rejection.
</p>

<hr/>

<h2>Check</h2>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py</code></pre>

<p>
These checks are non-normative repository-quality checks.
They do not define FROG semantics.
</p>
