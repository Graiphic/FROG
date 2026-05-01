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
This document describes the default realization posture for <code>frog.widgets.array</code>.
</p>

<p>
The default array realization exposes a shell with slots for index display, element region, element slots, scrollbars, and optional element gap.
It does not define the semantics of the element widget class.
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
