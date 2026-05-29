<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Decorations Widget Example Reference</h1>

<p align="center">
  <strong>Current flat-box and line decoration proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Decoration progression is demonstrated by
<code>Examples/18_decoration_board</code>. It proves three non-value support
widgets: <code>frog.widgets.flat_box</code>,
<code>frog.widgets.vertical_line</code>, and
<code>frog.widgets.horizontal_line</code>.
</p>

<p>
Decorations structure the visible front panel. They do not own diagram values
and do not execute logic.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Decoration class id, widget id, position, size, and z-order.</li>
  <li>Flat-box fill color, border color, and border width.</li>
  <li>Line color, line width, orientation, and endpoints.</li>
  <li>Optional visibility and support-widget posture.</li>
</ul>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Class</th>
      <th>Default SVG asset</th>
      <th>Published parts</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>frog.widgets.flat_box</code></td><td><code>flat_box.svg</code></td><td><code>root</code>, <code>shape</code></td></tr>
    <tr><td><code>frog.widgets.horizontal_line</code></td><td><code>horizontal_line.svg</code></td><td><code>root</code>, <code>line</code></td></tr>
    <tr><td><code>frog.widgets.vertical_line</code></td><td><code>vertical_line.svg</code></td><td><code>root</code>, <code>line</code></td></tr>
  </tbody>
</table>

<p>
Stroke width is a single source-owned visual property for each line or box
border. The runtime must preserve uniform thickness when resized.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/18_decoration_board/ui/decoration_panel.wfrog</code></li>
  <li>Default Decorations package: <code>Libraries/Realizations/Default/decorations.default.wfrog</code></li>
  <li>Default SVG assets: <code>Libraries/Realizations/Default/assets/decorations/templates/*.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The runtime consumes source-owned layout/style and Default SVG public parts.</li>
  <li>The visible board must contain one flat box, one vertical line, and one horizontal line as independent support widgets.</li>
  <li>No Label widget, HTML-only shell, or duplicated example-local decoration SVG is valid as a substitute.</li>
  <li>The native proof only validates the execution corridor; it does not assign scalar value semantics to decorations.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 18. When
published, it should live under <code>Examples/18_decoration_board/reference/</code>
and follow <a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>
