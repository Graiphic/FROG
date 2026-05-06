<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Table Widgets</h1>

<p align="center">
  <strong>Default realization posture for the standardized table widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Target Classes</h2>

<ul>
  <li><code>frog.widgets.table_control</code></li>
  <li><code>frog.widgets.table_indicator</code></li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This default realization publishes a portable table embodiment for the FROG Table widget family.
It provides a shell, header surfaces, row and cell surfaces, selection and edit overlays, sort and resize affordances, scrollbars, and focus surface.
</p>

<p>
This realization is downstream from widget class law.
It does not define table value semantics, editing semantics, sorting semantics, filtering semantics, or host-native virtualization semantics.
</p>

<hr/>

<h2>Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>header_region</code></li>
  <li><code>column_header_region</code></li>
  <li><code>row_header_region</code></li>
  <li><code>grid_region</code></li>
  <li><code>body_region</code></li>
  <li><code>row</code></li>
  <li><code>cell</code></li>
  <li><code>active_cell</code></li>
  <li><code>selection_face</code></li>
  <li><code>edit_overlay</code></li>
  <li><code>sort_indicator</code></li>
  <li><code>resize_handle</code></li>
  <li><code>vertical_scrollbar</code></li>
  <li><code>horizontal_scrollbar</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/table/templates/table_shell.svg</code></li>
  <li><code>assets/table/subobjects/header_region.svg</code></li>
  <li><code>assets/table/subobjects/grid_region.svg</code></li>
  <li><code>assets/table/subobjects/cell.svg</code></li>
  <li><code>assets/table/subobjects/selection_face.svg</code></li>
  <li><code>assets/table/subobjects/active_cell.svg</code></li>
  <li><code>assets/table/subobjects/edit_overlay.svg</code></li>
  <li><code>assets/table/subobjects/sort_indicator.svg</code></li>
  <li><code>assets/table/subobjects/resize_handle.svg</code></li>
  <li><code>assets/table/subobjects/vertical_scrollbar.svg</code></li>
  <li><code>assets/table/subobjects/horizontal_scrollbar.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>header_region</code> anchors the visible table headers.</li>
  <li><code>column_header_region</code> anchors column header cells.</li>
  <li><code>row_header_region</code> anchors row index or row id cells.</li>
  <li><code>grid_region</code> anchors the visible grid.</li>
  <li><code>body_region</code> anchors rows and cells.</li>
  <li><code>selection_face</code> renders selection.</li>
  <li><code>active_cell</code> renders focus within the grid.</li>
  <li><code>edit_overlay</code> anchors edit controls where legal.</li>
  <li><code>sort_indicator</code> and <code>resize_handle</code> anchor header affordances.</li>
</ul>

<p>
A runtime may replace the SVG-backed grid with a host-native virtual grid when it preserves the published part model and public property/method/event surfaces.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>All declared public parts must exist in widget class law.</li>
  <li>Declared SVG resources must resolve relative to this package.</li>
  <li>The declared SVG resources should expose all declared <code>data-frog-part</code> markers.</li>
  <li>Resources must not become semantic owners of table value.</li>
  <li>Host-native virtualization must not expose private row, cell, or renderer handles as public members.</li>
</ul>
