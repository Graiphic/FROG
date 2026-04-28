<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Numeric Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized numeric widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./numeric.default.wfrog">Machine-readable numeric realization manifest</a></li>
  <li><a href="./assets/numeric/Readme.md">Numeric default realization assets</a></li>
  <li><a href="../../Widgets/Numeric.md">Numeric widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for standardized numeric widgets:
</p>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
</ul>

<p>
It does not define numeric widget semantics.
The class law belongs to <code>Libraries/Widgets/Numeric.md</code>.
This document describes how the default realization family can embody that law through SVG template resources, part bindings, state maps, and host hints.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular</code></li>
</ul>

<p>
The rectangular variant is the initial default numeric realization variant.
</p>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/numeric.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>value_face</code></li>
  <li><code>text_value</code></li>
  <li><code>spinner</code></li>
  <li><code>increment_up</code></li>
  <li><code>increment_down</code></li>
  <li><code>radix_badge</code></li>
  <li><code>unit_label</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. SVG Template Asset</h2>

<ul>
  <li><code>assets/numeric/templates/numeric_rectangular.svg</code></li>
</ul>

<p>
The template is intended to be interpreted by an IDE or runtime through published part markers and property bindings.
</p>

<hr/>

<h2>6. Summary</h2>

<p>
The default numeric realization provides one inspectable SVG-backed baseline for numeric controls and indicators while keeping the class law in <code>Libraries/Widgets/Numeric.md</code>.
</p>
