<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Enum Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized enum widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./enum.default.wfrog">Machine-readable enum realization manifest</a></li>
  <li><a href="./assets/enum/Readme.md">Enum default realization assets</a></li>
  <li><a href="../../Widgets/Enum.md">Enum widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.enum_control</code> and <code>frog.widgets.enum_indicator</code>.
</p>

<p>
The default enum realization provides one rectangular ring/selector embodiment with value display, selector face, optional increment/decrement buttons, optional digital display, and optional text-overflow marker.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular_ring</code></li>
</ul>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/enum.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>selector_face</code></li>
  <li><code>increment_up</code></li>
  <li><code>increment_down</code></li>
  <li><code>digital_display</code></li>
  <li><code>text_overflow_marker</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. SVG Template Asset</h2>

<ul>
  <li><code>assets/enum/templates/enum_rectangular_ring.svg</code></li>
</ul>

<p>
The SVG template provides visual structure and part markers.
It does not own enum value, item inventory, or selected item semantics.
</p>
