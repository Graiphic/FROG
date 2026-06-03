<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default String Realization Assets</h1>

<p align="center">
  <strong>SVG template assets for the default string realization variant</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../String.md">Default string realization posture</a></li>
  <li><a href="../../string.default.wfrog">Machine-readable string realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/String.md">String widget class law</a></li>
</ul>

<hr/>

<h2>1. Purpose</h2>

<p>
This directory contains realization assets for the default string realization family.
</p>

<p>
The assets here do not define the string widget classes.
The string class law belongs to <code>Libraries/Widgets/String.md</code>.
These files are SVG template resources consumed by the default realization manifest.
</p>

<hr/>

<h2>2. Published Assets</h2>

<ul>
  <li><code>templates/string_rectangular.svg</code> — rectangular default realization variant.</li>
</ul>

<hr/>

<h2>3. Required Template Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>text_region</code></li>
  <li><code>text_value</code></li>
  <li><code>placeholder</code></li>
</ul>

<p>
The String rectangular template intentionally has no <code>focus_ring</code> marker.
Interactive hover and text-region styling are carried through the manifest bindings and instance <code>.wfrog</code> properties.
</p>

<hr/>

<h2>4. Semantic SVG Contract</h2>

<p>
The SVG root publishes <code>data-frog-skin-contract="semantic_svg_skin_v1"</code>
and a <code>metadata</code> block named
<code>frog_semantic_skin_contract</code>. A host should discover String geometry
from these public markers instead of guessing private coordinates.
</p>

<table>
  <thead>
    <tr>
      <th>Part</th>
      <th>Member/style surface</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>root</code></td>
      <td>widget coordinate space</td>
      <td>Complete rectangular String skin.</td>
    </tr>
    <tr>
      <td><code>label</code></td>
      <td><code>label.text</code></td>
      <td>Logical/tooling label, separate from the visible caption.</td>
    </tr>
    <tr>
      <td><code>caption</code></td>
      <td><code>caption.text</code>, <code>caption.visible</code>, <code>caption.style.*</code></td>
      <td>Visible front-panel caption.</td>
    </tr>
    <tr>
      <td><code>frame</code></td>
      <td><code>style.frame.*</code></td>
      <td>Outer rectangular field border and background.</td>
    </tr>
    <tr>
      <td><code>text_region</code></td>
      <td><code>style.text_region.*</code></td>
      <td>Editable/display field body and crop reference for embedded String cells.</td>
    </tr>
    <tr>
      <td><code>text_value</code></td>
      <td><code>value</code>, <code>text.*</code>, <code>style.text.*</code></td>
      <td>Current string value rendered by the host overlay.</td>
    </tr>
    <tr>
      <td><code>placeholder</code></td>
      <td><code>placeholder.text</code>, <code>placeholder.visible</code>, <code>placeholder.style.*</code></td>
      <td>Optional empty-value fallback text; never the value itself.</td>
    </tr>
  </tbody>
</table>

<p>
The Default String SVG must not publish decorative validation markers,
overflow markers, hardcoded focus rings, caret surfaces, or selection surfaces.
Those remain host/editor internals unless a future realization explicitly
standardizes them.
</p>
