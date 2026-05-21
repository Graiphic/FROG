<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Widget Layer Validator</h1>

<p align="center">
  <strong>Non-normative repository-quality validator for the FROG widget and Default realization layer</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This tool checks repository hygiene for the current widget layer.
It does not define widget semantics.
</p>

<hr/>

<h2>Checks</h2>

<ul>
  <li>widget class-law documents exist,</li>
  <li>Default realization documents exist,</li>
  <li><code>*.default.wfrog</code> manifests exist and parse as JSON,</li>
  <li>manifest resources resolve relative to the manifest directory,</li>
  <li>target classes are mentioned in the matching widget class-law document,</li>
  <li>declared SVG-bound public parts are visible through <code>data-frog-part</code> markers where possible,</li>
  <li>host-overlay public parts may be declared in <code>.wfrog</code> without requiring hidden prototype SVG markers,</li>
  <li>Default package documentation does not contain literal escaped newline artifacts.</li>
</ul>

<hr/>

<h2>Run</h2>

<pre><code>python Implementations/Reference/WidgetValidator/validate_widget_layer.py
</code></pre>
