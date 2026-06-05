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
<time datetime="2026-06-05">2026-06-05</time>. The reviewed surface keeps the
closed selector SVG minimal and adds a separate dropdown list SVG skin for the
host-rendered enum item popup.
</p>

<p>
The default enum realization provides one rectangular selector embodiment with
value face, selected-item text, selector face, caption, and label surfaces.
Selector arrows remain internal details of <code>selector_face</code>. The
opened popup/dropdown is a host surface rendered from a separate semantic SVG
skin so item rows can be styled without making them public parts of the closed
selector SVG.
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
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>selector_face</code></li>
</ul>

<hr/>

<h2>5. SVG Template Assets</h2>

<ul>
  <li><code>assets/enum/templates/enum_rectangular_ring.svg</code></li>
  <li><code>assets/enum/templates/enum_dropdown_list.svg</code></li>
</ul>

<p>
The closed selector SVG provides visual structure and part markers for the
resting widget. The dropdown list SVG provides a repeated row/text skin for the
host-rendered item popup. Neither SVG owns enum value, item inventory, or
selected item semantics.
</p>

<p>
The popup geometry is declared in <code>enum.default.wfrog</code> under
<code>host_surface_bindings</code>. The Default dropdown anchors to
<code>value_face</code>, uses that part's outer width with a declared
source-width outset, centers the dropdown under the body, and explicitly
excludes <code>selector_face</code> from the measured popup width.
The runtime must interpret this contract; it must not hardcode Enum-specific
popup alignment.
</p>

<p>
The closed selector geometry keeps <code>selector_face</code> tied to
<code>value_face.right</code> by the SVG-declared gap
<code>data-frog-layout-gap-units="3.959"</code>. When the value body width is
changed, selector geometry must move by the same source delta, and the dropdown
width must continue to follow the body through the manifest's
<code>outer_width</code> rule.
</p>
