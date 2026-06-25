<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Label Widget</h1>

<p align="center">
  <strong>Default realization posture for the standalone label support widget</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./label.default.wfrog">Machine-readable label realization manifest</a></li>
  <li><a href="./assets/label/Readme.md">Label default realization assets</a></li>
  <li><a href="../../Widgets/Label.md">Label widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.label</code>.
</p>

<p>
The class law belongs to <code>Libraries/Widgets/Label.md</code>.
This realization defines how the Default family can embody the label through SVG-backed or host-native text rendering while preserving the distinction between <code>label.text</code> and <code>text.value</code>.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular</code></li>
</ul>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/label.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code></li>
  <li><code>text_surface</code></li>
</ul>

<hr/>

<h2>5. SVG Template Asset</h2>

<ul>
  <li><code>assets/label/templates/label_rectangular.svg</code></li>
</ul>

<p>
The SVG template provides geometry and anchors for the placement aura and text
surface. It does not own <code>text.value</code>. The published
<code>placement_bounds</code> part is the IDE/runtime placement aura; selection
and hover visuals are overlays aligned to that part, not a Label
<code>focus_ring</code>.
</p>

<p>
The Default Label is deliberately text-only. If a front-panel author needs a
visible rectangle, background, callout, or grouping surface, they should combine
the Label with a decoration/frame widget rather than expanding the Label skin
contract.
</p>
