<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Path Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized path widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./path.default.wfrog">Machine-readable path realization manifest</a></li>
  <li><a href="./assets/path/Readme.md">Path default realization assets</a></li>
  <li><a href="../../Widgets/Path.md">Path widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.path_control</code> and <code>frog.widgets.path_indicator</code>.
</p>

<p>
The default path realization provides one rectangular path-field embodiment with path display, optional browse button, optional path icon, optional validation marker, optional text-overflow marker, caption and label surfaces, and focus ring.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular_field</code></li>
</ul>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/path.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>path_face</code></li>
  <li><code>path_display</code></li>
  <li><code>browse_button</code></li>
  <li><code>path_icon</code></li>
  <li><code>validation_marker</code></li>
  <li><code>text_overflow_marker</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. SVG Template Asset</h2>

<ul>
  <li><code>assets/path/templates/path_rectangular_field.svg</code></li>
</ul>

<p>
The SVG template provides visual structure and part markers.
It does not own path value semantics or host browse behavior.
</p>
