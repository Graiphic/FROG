<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Label Widget Example Reference</h1>

<p align="center">
  <strong>Current standalone support-text widget proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Label progression is demonstrated by
<code>Examples/17_label_typography_board</code>. It proves that a Label is a
real front-panel support widget, not baked SVG text and not merely the common
<code>label.*</code> property of another widget.
</p>

<p>
The visible support text is owned by <code>text.value</code>. Position, size,
alignment, wrapping, typography, background, and frame styling are source-owned
instance data or Default realization data.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Label widget ids and layout rectangles.</li>
  <li><code>text.value</code>, wrapping, alignment, and text posture.</li>
  <li>Font family, size, weight, style, color, and decoration.</li>
  <li>Background visibility, fill color, frame visibility, frame color, and frame width.</li>
  <li>Example scene composition and explanatory support text.</li>
</ul>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Part</th>
      <th>Property surface</th>
      <th>Runtime obligation</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>background</code></td><td><code>style.background.*</code></td><td>Apply fill and visibility from source/default data.</td></tr>
    <tr><td><code>frame</code></td><td><code>style.frame.*</code></td><td>Keep one uniform border thickness under resize.</td></tr>
    <tr><td><code>text_surface</code></td><td><code>text.*</code>, <code>style.text.*</code></td><td>Render live text, not SVG-baked text.</td></tr>
    <tr><td><code>focus_ring</code></td><td><code>style.focus_ring.*</code></td><td>Expose focus posture when enabled by host/source policy.</td></tr>
  </tbody>
</table>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/17_label_typography_board/ui/label_panel.wfrog</code></li>
  <li>Default Label package: <code>Libraries/Realizations/Default/label.default.wfrog</code></li>
  <li>Default Label SVG: <code>Libraries/Realizations/Default/assets/label/templates/label_rectangular.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The runtime consumes Label instances from <code>.frog</code> and the Default Label realization from <code>.wfrog</code>.</li>
  <li>Host text overlays may be used, but they must align to <code>text_surface</code> and consume source-owned typography.</li>
  <li>Labels must be visually aligned to their source-owned widget rectangles.</li>
  <li>The runtime must not introduce an HTML-only fallback label shell or duplicate the Default Label SVG under the example.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 17. When
published, it should live under <code>Examples/17_label_typography_board/reference/</code>
and follow <a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>
