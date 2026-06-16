<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Button Realization Assets</h1>

<p align="center">
  <strong>SVG template and state resources for the default button realization</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../Button.md">Default button realization posture</a></li>
  <li><a href="../../button.default.wfrog">Machine-readable button realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/Button.md">Button widget class law</a></li>
</ul>

<hr/>

<h2>1. Published Assets</h2>

<ul>
  <li><code>templates/button_rectangular.svg</code> — parameterized rectangular button template.</li>
</ul>

<hr/>

<h2>2. Required Template Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>placement_bounds</code></li>
  <li><code>button_face</code></li>
  <li><code>state_text</code></li>
</ul>

<p>
The rectangular template also publishes the placement anchors <code>label.anchor</code>, <code>caption.anchor</code>, <code>button_face.bounds</code>, and <code>state_text.center</code>.
Its default visible skin is a single bordered rectangular <code>button_face</code> centered inside the non-visible <code>placement_bounds</code> aura.
Button examples should consume these Default assets through a <code>.wfrog</code> package reference instead of copying local SVG skins.
</p>
