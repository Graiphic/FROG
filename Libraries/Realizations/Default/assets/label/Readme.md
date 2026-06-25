<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Label Realization Assets</h1>

<p align="center">
  <strong>SVG template asset for the default label realization variant</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../Label.md">Default label realization posture</a></li>
  <li><a href="../../label.default.wfrog">Machine-readable label realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/Label.md">Label widget class law</a></li>
</ul>

<hr/>

<h2>1. Published Asset</h2>

<ul>
  <li><code>templates/label_rectangular.svg</code> — rectangular default label realization template.</li>
</ul>

<hr/>

<p>
The published rectangular template is intentionally minimal. A Label is static
support text, so the SVG publishes only the placement aura and the semantic
text surface. The live text remains owned by <code>.frog</code>
<code>text.value</code>; SVG preview text is never semantic content.
</p>

<p>
The <code>placement_bounds</code> part is the IDE/runtime placement aura for
the widget. The default aura band is 4 px. Selection and hover visuals are
runtime/IDE overlays aligned to <code>placement_bounds</code>, not SVG-baked
focus rings. If a visible rectangle is needed around text, use a decoration or
frame widget with a Label instead of adding public Label skin parts.
</p>

<hr/>

<h2>2. Required Template Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code></li>
  <li><code>text_surface</code></li>
</ul>
