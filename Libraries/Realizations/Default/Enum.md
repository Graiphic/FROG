<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Enum Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized enum widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./enum.default.wfrog">Machine-readable enum realization manifest</a></li>
  <li><a href="./assets/enum/Readme.md">Enum default realization assets</a></li>
  <li><a href="../../Widgets/Enum.md">Enum widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.enum_control</code> and <code>frog.widgets.enum_indicator</code>.
</p>

<p>
Latest public realization metadata review:
<time datetime="2026-06-12">2026-06-12</time>. The reviewed surface keeps the
closed value SVG minimal and keeps a separate dropdown list SVG skin for any
host-rendered enum item popup.
</p>

<p>
The default enum realization provides one compact rectangular value embodiment
with value face, selected-item text, optional increment/decrement command parts,
caption, and label surfaces. The closed value skin does not publish a selector
face or arrow. Any opened popup/dropdown remains a host surface rendered from a
separate semantic SVG skin so item rows can be styled without making them public
parts of the closed value SVG.
</p>

<p>
This visual posture intentionally matches the Default Ring compact posture.
Hosts may share internal rendering and layout code for the two realizations, but
must keep Enum value semantics separate from Ring value semantics.
</p>

<p>
The accepted closed-selector and dropdown appearance is published as reusable
<code>default_widget_properties</code> in <code>enum.default.wfrog</code>.
Hosts apply those defaults by widget class and role before any source-owned
<code>.frog</code> instance overrides.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular_ring</code></li>
</ul>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/enum.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>focus_ring</code></li>
  <li><code>spinner</code> when increment/decrement is shown</li>
  <li><code>increment_up</code> when increment/decrement is shown</li>
  <li><code>increment_down</code> when increment/decrement is shown</li>
</ul>

<p>
<code>placement_bounds</code> is the invisible placement aura consumed by IDE,
grid, and Array containment policies. The Default compact Enum aura is
<code>176 x 38</code> source units inside the <code>200 x 130</code> SVG
viewBox, with a uniform 4-unit band around the compact body. It is not focus,
selection, a dropdown row, or a visible decoration.
</p>

<hr/>

<h2>5. SVG Template Assets</h2>

<ul>
  <li><code>assets/enum/templates/enum_rectangular_ring.svg</code></li>
  <li><code>assets/enum/templates/enum_dropdown_list.svg</code></li>
</ul>

<p>
The closed value SVG provides visual structure and part markers for the
resting widget. The dropdown list SVG provides a repeated row/text skin for the
host-rendered item popup. Neither SVG owns enum value, item inventory, or
selected item semantics.
</p>

<p>
The popup geometry is declared in <code>enum.default.wfrog</code> under
<code>host_surface_bindings</code>. The Default dropdown anchors to
<code>value_face</code>, uses that part's outer width with a declared
source-width outset, and centers the dropdown under the body. The runtime must
interpret this contract; it must not hardcode Enum-specific popup alignment.
</p>

<p>
When a Default Enum widget is embedded in an Array cell, the cell footprint is
<code>placement_bounds</code>. The Array must not infer cell size from
<code>value_face</code>, <code>spinner</code>, <code>increment_up</code>, or
<code>increment_down</code>.
</p>

<p>
Inside Array containment, the opened Enum item list is hosted by the Array as a
floating overlay surface above the viewport. It still consumes the dropdown SVG
skin and host-surface binding declared here; it is not allowed to be clipped by
the repeated cell box. Increment/decrement hover and pressed visual states must
continue to style the same semantic button parts, and the Default posture uses
<code>data_entry.increment_wrap=true</code> for circular next/previous item
stepping.
</p>
