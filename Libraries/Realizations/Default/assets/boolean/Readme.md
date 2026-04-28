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

<pre><code>frog.widgets.boolean_control
frog.widgets.boolean_indicator
    |
    +-- realization.variant = rectangular
    |
    +-- realization.variant = circular
</code></pre>

<hr/>

<h2>4. Required Template Markers</h2>

<p>
The SVG templates should expose stable markers for these public parts:
</p>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>outer_face</code></li>
  <li><code>inner_face</code></li>
  <li><code>state_text</code></li>
  <li><code>focus_ring</code></li>
</ul>

<p>
These markers allow an IDE or runtime to bind public boolean properties to realization surfaces without inferring semantics from raw geometry.
</p>

<hr/>

<h2>5. Ownership Boundary</h2>

<p>
The assets may define geometry, template markers, SVG groups, CSS variables, and fallback visual defaults.
They must not redefine:
</p>

<ul>
  <li>the meaning of <code>value</code>,</li>
  <li>the control-versus-indicator distinction,</li>
  <li>the public boolean property inventory,</li>
  <li>the public boolean method inventory,</li>
  <li>the public boolean event inventory.</li>
</ul>

<hr/>

<h2>6. Summary</h2>

<p>
This directory provides the first default SVG template resources for boolean widgets.
The machine-readable manifest that binds those assets to the default realization family is:
</p>

<pre><code>Libraries/Realizations/Default/boolean.default.wfrog</code></pre>
