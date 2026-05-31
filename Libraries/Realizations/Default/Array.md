<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Array Widget</h1>

<p align="center">
  <strong>Default realization posture for the composite array widget</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./array.default.wfrog">Machine-readable array realization manifest</a></li>
  <li><a href="./assets/array/Readme.md">Array default realization assets</a></li>
  <li><a href="../../Widgets/Array.md">Array widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.array</code>,
<code>frog.widgets.array_control</code>, and
<code>frog.widgets.array_indicator</code>.
</p>

<p>
The default array realization exposes a shell with slots for index display, element region, element slots, scrollbars, and optional element gap.
It does not define the semantics of the element widget class.
For rank-3 arrays, the same realization surface may expose a layer index
display while the element region renders the selected two-dimensional layer.
</p>

<p>
The published <code>frame</code> part represents the outer Array container
surface. Hosts may realize it as a dynamic overlay when the source-owned
viewport and scrollbar geometry changes, but its fill, border, and padding
remain instance properties. When the index display is visible, it remains a
neighboring Array subcontainer with its own source-owned surface and a
source-owned gap to the element viewport. The repeated element widgets keep
their own realization frames through <code>element.props.*</code>; the Array
frame must not replace or redraw those contained widgets.
</p>

<p>
The index display is an Array subobject. Arrow/value proportions, arrow gap,
and value gap are source-owned through <code>index_display.layout.*</code> and
<code>style.index_display.*</code>. A visible index display may also expose its
own source-owned subcontainer fill, border, border width, and padding so it
reads as a neighboring Array container rather than a floating control. A host
may overlay native or HTML hit areas, but those hit areas must align to the
published Default Array index display parts and must not bake spacing into
runtime code.
</p>

<p>
When the Array repeats another Default widget, the repeated cell may be fitted
to a public part of that contained realization. A Numeric control element may
consume <code>control_body</code> so the increment/decrement surfaces remain
visible; a Numeric indicator element may consume <code>indicator_body</code>
when those surfaces are hidden. The Default Array frame and padding must remain
geometrically identical across those postures.
</p>

<p>
The Default realization treats <code>viewport.visible_counts[]</code> as a
source-owned viewport property surface. In a two-dimensional Array viewport,
<code>viewport.visible_counts[0]</code> controls the visible rows and
<code>viewport.visible_counts[1]</code> controls the visible columns. The
Default realization may expose host overlays for the visible-count effect, but
the values must come from the Array instance data or from explicit
property-write flow declared by the <code>.frog</code> source.
</p>

<p>
Visible counts describe the viewport, not the allocated data shape. The Default
realization must therefore allow a viewport to show more rows or columns than
currently contain authored values, rendering those extra visible slots with the
contained element default value and disabled/read-only visual posture where
appropriate. The realization must not bake a runtime maximum equal to the
semantic Array shape.
</p>

<p>
The Default contained-widget boundary posture is border sharing. If
<code>element.layout.padding</code>, row gap, and column gap are zero, repeated
contained widgets may touch and their adjacent published frames may visually
superpose. The Default Array realization must not insert a fake separator
stroke or per-cell shell between contained widgets. The only collection border
owned by the Array is the outer frame controlled by <code>style.frame.*</code>,
plus the neighboring index-display subcontainer when it is visible.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular_viewport</code></li>
</ul>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/array.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>index_display</code></li>
  <li><code>dimension_controls</code></li>
  <li><code>element_region</code></li>
  <li><code>element_slot</code></li>
  <li><code>element_gap</code></li>
  <li><code>horizontal_scrollbar</code></li>
  <li><code>vertical_scrollbar</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. SVG Assets</h2>

<ul>
  <li><code>assets/array/templates/array_shell.svg</code></li>
  <li><code>assets/array/subobjects/index_display.svg</code></li>
  <li><code>assets/array/subobjects/element_region.svg</code></li>
  <li><code>assets/array/subobjects/element_slot.svg</code></li>
  <li><code>assets/array/subobjects/vertical_scrollbar.svg</code></li>
  <li><code>assets/array/subobjects/horizontal_scrollbar.svg</code></li>
</ul>
