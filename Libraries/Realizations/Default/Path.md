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
The default path realization provides one rectangular path-field embodiment with path display, optional browse button, optional browse text, optional path icon, caption and label surfaces.
</p>

<p>
Latest public realization review:
<time datetime="2026-06-03">2026-06-03</time>. The reviewed surface is the
Default rectangular Path SVG skin and Example 09 C++ browser-host rendering.
The review publishes realization metadata and example evidence, not Graiphic
runtime implementation code.
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
  <li><code>path_face</code></li>
  <li><code>path_display</code></li>
  <li><code>browse_button</code></li>
  <li><code>browse_button_text</code></li>
  <li><code>path_icon</code></li>
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

<h2>6. Rectangular Field Geometry Contract</h2>

<p>
The rectangular field realization is interpreted through semantic SVG parts.
Hosts should treat the SVG as the source of visual geometry for overlays and
hit targets, not as an inert background image.
</p>

<ul>
  <li><code>root</code> scales to the front-panel widget layout rectangle.</li>
  <li><code>caption</code> is an anchored caption surface; runtime text overlays align to <code>caption.anchor</code>.</li>
  <li><code>path_face</code> is the main stretch surface and must keep one uniform border thickness after resize.</li>
  <li><code>path_icon</code> is optional and indivisible; its folder proportions must be preserved when the widget is resized.</li>
  <li><code>path_display</code> is the dynamic path text region; the host editable/display overlay aligns to <code>path_display.left_center</code>.</li>
  <li>When <code>display.icon_visible</code> is false, the path text region begins at the <code>path_face</code> inset rather than leaving a dead icon gap.</li>
  <li><code>browse_button</code> is an optional right-pinned hit target for controls; indicators hide it without redefining the widget value.</li>
  <li><code>browse_button_text</code> moves with the browse button and stays centered within it.</li>
</ul>

<p>
For embedded Path renderings, such as Path elements inside Array cells, the
host may crop to <code>path_face</code>. In that posture, caption and browse affordance
surfaces are not part of the cell value face, while the Path value semantics and
the <code>path_display</code> text region remain active.
</p>

<h2>7. Reuse Posture</h2>

<p>
The Default Path realization is the reusable public baseline for path controls
and indicators. Public examples that need a path value should reference this
realization and its published SVG asset instead of duplicating local path SVGs.
Example-specific placement, labels, initial values, binding ids, browse
filters, and visual overrides remain source-owned instance data.
</p>
