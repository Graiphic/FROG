<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Validation</h1>

<p align="center">
  <strong>Validation checklist for the published Default widget realization layer</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document defines the repository-visible validation posture for the Default widget realization layer.
It is intentionally downstream from widget class law.
</p>

<hr/>

<h2>Validation Targets</h2>

<p>
Each published widget family should provide:
</p>

<ul>
  <li>a widget class-law document under <code>Libraries/Widgets/</code>,</li>
  <li>a Default realization document under <code>Libraries/Realizations/Default/</code>,</li>
  <li>a machine-readable manifest <code>*.default.wfrog</code>,</li>
  <li>resolvable SVG template or subobject resources where the manifest declares them,</li>
  <li>part bindings that refer to public widget parts,</li>
  <li>target classes that appear in the corresponding widget class-law document.</li>
</ul>

<hr/>

<h2>Machine-Checkable Expectations</h2>

<ul>
  <li>All manifests must be valid JSON.</li>
  <li>Each manifest must include <code>format</code>, <code>kind</code>, <code>package</code>, <code>targets</code>, <code>resources</code>, and <code>validation_expectations</code>.</li>
  <li>Each declared resource path must resolve relative to the manifest file.</li>
  <li>SVG resources should expose declared public parts through <code>data-frog-part</code> markers when applicable.</li>
  <li>Documentation files should not contain literal escaped newline artifacts such as <code>\n</code> in rendered lists.</li>
  <li>Runtime role posture should be explicit enough to distinguish control, indicator, support, layout, container, and structured families.</li>
</ul>

<hr/>

<h2>Reference Validator</h2>

<p>
A non-normative reference validator may live under:
</p>

<pre><code>Implementations/Reference/WidgetValidator/</code></pre>

<p>
That validator should remain a repository-quality tool.
It does not define widget semantics.
</p>
