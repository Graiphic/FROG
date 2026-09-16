<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — String Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized string widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./string.default.wfrog">Machine-readable string realization manifest</a></li>
  <li><a href="./assets/string/Readme.md">String default realization assets</a></li>
  <li><a href="../../Widgets/String.md">String widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for the standardized string widget classes:
</p>

<ul>
  <li><code>frog.widgets.string_control</code></li>
  <li><code>frog.widgets.string_indicator</code></li>
</ul>

<p>
It does not define string widget semantics.
The class law belongs to <code>Libraries/Widgets/String.md</code>.
This document describes how the default realization family can embody that law through SVG template resources, part bindings, state maps, and realization variants.
</p>

<p>
Latest public realization metadata review:
<time datetime="2026-06-06">2026-06-06</time>. The reviewed surface is the
Default rectangular String SVG skin, its public semantic part contract, and the
Example 07 placeholder/text_value/focus_ring browser-host launch posture.
</p>

<hr/>

<h2>2. Label and Caption</h2>

<p>
The default string realization follows the shared widget convention:
</p>

<ul>
  <li><code>label</code> realizes the structural/logical label surface when exposed.</li>
  <li><code>caption</code> realizes the front-panel user-facing caption.</li>
  <li><code>text_value</code> realizes the current string value.</li>
  <li><code>placeholder</code> realizes optional placeholder text when the value is empty.</li>
</ul>

<hr/>

<h2>3. Published Default Variant</h2>

<p>
The default string realization publishes one initial compatible variant:
</p>

<ul>
  <li><code>rectangular</code></li>
</ul>

<p>
The variant targets both string classes and is not a separate widget class.
</p>

<hr/>

<h2>4. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/string.default.wfrog</code></pre>

<p>
That manifest declares target classes, realization records, variants, SVG resources, part bindings, state maps, property bindings, anchors, and host hints.
</p>

<hr/>

<h2>5. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>text_region</code></li>
  <li><code>text_value</code></li>
  <li><code>placeholder</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>6. SVG Template Assets</h2>

<ul>
  <li><code>assets/string/templates/string_rectangular.svg</code></li>
</ul>

<hr/>

<h2>7. Property Binding Posture</h2>

<pre><code>string public property
    -> realization property binding
        -> SVG template part or host-rendered surface
</code></pre>

<p>
Representative bindings include <code>caption.text</code> to <code>caption</code>, <code>value</code> to <code>text_value</code>, <code>placeholder.text</code> to <code>placeholder</code>, <code>style.text_region.*</code> to <code>text_region</code>, and <code>style.text.*</code> to <code>text_value</code>.
</p>

<p>
The published String default realization keeps hover and text-region appearance on <code>style.text_region.*</code>.
It publishes <code>focus_ring</code> as the skin-owned keyboard-focus boundary, so hosts must use that semantic part instead of inventing a hardcoded String focus shape.
IDE hover and selection auras remain separate host overlays aligned to <code>placement_bounds</code>.
</p>

<h2>8. Rectangular SVG Geometry Contract</h2>

<p>
The rectangular String realization is a semantic SVG skin. Its published parts
and anchors define where a host places the caption, editable/display text
overlay, placeholder overlay, and stretched text field body.
</p>

<table>
  <thead>
    <tr>
      <th>Part</th>
      <th>Runtime interpretation</th>
      <th>Resize posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>root</code></td>
      <td>Complete String widget coordinate system.</td>
      <td>Scales to the front-panel widget layout rectangle.</td>
    </tr>
    <tr>
      <td><code>label</code></td>
      <td>Logical/tooling label preview; separate from the visible caption.</td>
      <td>Anchored text, hidden or replaced by host policy.</td>
    </tr>
    <tr>
      <td><code>caption</code></td>
      <td>User-facing front-panel caption.</td>
      <td>Anchored text aligned to <code>caption.anchor</code>.</td>
    </tr>
    <tr>
      <td><code>text_region</code></td>
      <td>Main editable/display field body, border, and embedded-widget crop reference.</td>
      <td>Stretch surface with uniform non-scaling stroke.</td>
    </tr>
    <tr>
      <td><code>text_value</code></td>
      <td>Dynamic current string value.</td>
      <td>Host overlay aligns to <code>text_region.left_center</code>.</td>
    </tr>
    <tr>
      <td><code>placeholder</code></td>
      <td>Optional empty-value fallback text only; never the semantic value.</td>
      <td>Host overlay aligns to <code>text_region.left_center</code> when visible.</td>
    </tr>
    <tr>
      <td><code>focus_ring</code></td>
      <td>Optional focus visualization surface.</td>
      <td>SVG-owned focus boundary toggled by host focus state.</td>
    </tr>
  </tbody>
</table>

<p>
The Default rectangular String skin intentionally has no published frame,
validation marker, overflow marker, caret, or selection surface.
Those are host or editor details unless a later realization explicitly promotes them.
The published <code>focus_ring</code> is the exception because focus shape is
owned by the SVG skin contract.
</p>
