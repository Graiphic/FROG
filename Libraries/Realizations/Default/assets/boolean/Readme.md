<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Boolean Realization Assets</h1>

<p align="center">
  <strong>SVG template assets for the default boolean realization variants</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../Boolean.md">Default boolean realization posture</a></li>
  <li><a href="../../boolean.default.wfrog">Machine-readable boolean realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/Boolean.md">Boolean widget class law</a></li>
</ul>

<hr/>

<h2>1. Purpose</h2>

<p>
This directory contains realization assets for the default boolean realization family.
</p>

<p>
The assets here do not define the boolean widget classes.
The boolean class law belongs to <code>Libraries/Widgets/Boolean.md</code>.
These files are SVG template resources consumed by the default realization manifest.
</p>

<hr/>

<h2>2. Published Assets</h2>

<ul>
  <li><code>templates/boolean_rectangular.svg</code> — rectangular default realization variant.</li>
  <li><code>templates/boolean_circular.svg</code> — circular default realization variant.</li>
</ul>

<hr/>

<h2>3. Realization Role</h2>

<p>
Both assets target:
</p>

<ul>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
</ul>

<p>
They are realization variants, not separate widget classes.
</p>

<hr/>

<h2>4. Required Template Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>outer_face</code></li>
  <li><code>inner_face</code></li>
  <li><code>state_text</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. Ownership Boundary</h2>

<p>
The assets may define geometry, template markers, SVG groups, CSS variables, and fallback visual defaults.
They must not redefine the meaning of <code>value</code>, the control-versus-indicator distinction, or the public boolean property, method, and event inventory.
</p>
