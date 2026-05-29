<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Frame Widget Example Reference</h1>

<p align="center">
  <strong>Current standalone rectangle support-widget proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Frame progression is demonstrated by
<code>Examples/25_frame_grouping_board</code>. It proves the standalone
<code>frog.widgets.frame</code> class as a simple visual support rectangle
with background and uniform border styling.
</p>

<p>
Frame is not a Subpanel, not a Tab page, not a value widget, and not a hidden
owner of child widgets placed visually inside its bounds.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Frame widget id, position, size, and z-order.</li>
  <li>Background visibility and fill color.</li>
  <li>Border visibility, border color, and one uniform border-width value.</li>
  <li>Optional companion Label widgets when text is needed.</li>
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
    <tr><td><code>background</code></td><td><code>style.background.*</code></td><td>Apply visible/fill values from source/default data.</td></tr>
    <tr><td><code>border</code></td><td><code>style.border.*</code></td><td>Keep one continuous rectangular border with uniform non-scaling stroke width.</td></tr>
  </tbody>
</table>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/25_frame_grouping_board/ui/frame_panel.wfrog</code></li>
  <li>Default Frame package: <code>Libraries/Realizations/Default/frame.default.wfrog</code></li>
  <li>Default Frame SVG: <code>Libraries/Realizations/Default/assets/frame/templates/frame_rectangular.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The runtime consumes source-owned frame geometry and style.</li>
  <li>Resizing must not create uneven border thickness.</li>
  <li>Frame must not acquire value, scrolling, caption, method, or child-hosting behavior.</li>
  <li>No HTML-only frame shell or duplicated example-local Frame SVG is valid when the Default realization exists.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 25. When
published, it should live under <code>Examples/25_frame_grouping_board/reference/</code>
and follow <a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>
