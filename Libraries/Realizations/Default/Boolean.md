<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization - Boolean Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized boolean widgets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./boolean.default.wfrog">Machine-readable boolean realization manifest</a></li>
  <li><a href="./assets/boolean/Readme.md">Boolean default realization assets</a></li>
  <li><a href="../../Widgets/Boolean.md">Boolean widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for the standardized boolean widget classes:
</p>

<ul>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
</ul>

<p>
Boolean remains one semantic widget family. Square, rectangular, round, and
oval appearances are realization skins, not separate Boolean classes.
</p>

<hr/>

<h2>2. Published Default Variants</h2>

<ul>
  <li><code>rectangular</code> - square/rectangular LED skin.</li>
  <li><code>circular</code> - round/oval LED skin.</li>
</ul>

<pre><code>frog.widgets.boolean_control
frog.widgets.boolean_indicator
    |
    +-- frog.realizations.default.boolean.rectangular
    |
    +-- frog.realizations.default.boolean.circular
</code></pre>

<hr/>

<h2>3. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>placement_bounds</code> - non-rendered IDE/container placement aura.</li>
  <li><code>state_face</code> - Boolean SVG mapping to <code>main_body</code>, the visible true/false state surface.</li>
  <li><code>state_text</code> - optional centered On/Off text.</li>
  <li><code>focus_ring</code> - optional focus outline following <code>state_face</code>.</li>
</ul>

<p>
The default Boolean realization does not publish <code>outer_face</code>,
<code>inner_face</code>, or <code>frame</code>. Those older surfaces were removed
to keep the SVG skin semantic and minimal.
</p>

<hr/>

<h2>4. Property Binding Posture</h2>

<pre><code>boolean public property
    -> realization property binding
        -> SVG template part or host-rendered surface
</code></pre>

<p>
Representative bindings include <code>label.text</code> to <code>label</code>,
<code>caption.text</code> to <code>caption</code>,
<code>state_text.true_text</code> and <code>state_text.false_text</code> to
<code>state_text</code>, <code>style.state_face.*</code> to
<code>state_face</code>, and <code>style.focus_ring.*</code> to
<code>focus_ring</code>.
</p>

<hr/>

<h2>5. IDE and Container Geometry</h2>

<p>
The <code>placement_bounds</code> part is the Boolean placement aura. IDE
selection overlays and widget-backed Array cells may use it as the containment
surface. The aura is not the keyboard focus ring and is not a value surface.
</p>

<p>
The Default Boolean skin uses this source geometry by default:
<code>placement_bounds = 72 x 72</code>, <code>state_face = 64 x 64</code>,
with a uniform 4 source-unit <code>aura_band</code> between
<code>placement_bounds</code> and <code>main_body</code>. This matches the
Default Numeric aura standard. Resizing a rectangular, square, oval, or
circular Boolean scales that skin. The runtime may expose the
placement aura in IDE view, but it must not replace it with a hardcoded shape.
</p>

<hr/>

<h2>6. Summary</h2>

<p>
The default boolean realization keeps class law in
<code>Libraries/Widgets/Boolean.md</code>, publishes SVG template resources
under <code>assets/boolean/</code>, and binds them through
<code>boolean.default.wfrog</code>.
</p>
