<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Table Widget Example Reference</h1>

<p align="center">
  <strong>Current flat table selected-cell proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Table progression is demonstrated by
<code>Examples/23_table_cell_selection_roundtrip</code>. It proves a flat
row-and-column data widget with source-owned rows, columns, cell values,
selection, viewport, and editable-cell posture.
</p>

<p>
Table is not Tree, Array, or Spreadsheet. It is a structured flat tabular data
widget whose value and selection posture are public FROG widget state.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Column ids, labels, widths, visibility, and cell value references.</li>
  <li>Row count, row ids when present, and cell values.</li>
  <li>Selected row, selected column, active cell, selection mode, and edit posture.</li>
  <li>Header visibility, row/column header posture, scrollbar posture, viewport offset, and visible window.</li>
  <li>Colors, borders, grid lines, active-cell style, selection style, and typography.</li>
</ul>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Important parts</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>Frame and headers</td><td><code>frame</code>, <code>header_region</code>, <code>column_header_region</code>, <code>row_header_region</code></td></tr>
    <tr><td>Grid</td><td><code>grid_region</code>, <code>body_region</code>, <code>row</code>, <code>cell</code></td></tr>
    <tr><td>Interaction</td><td><code>active_cell</code>, <code>selection_face</code>, <code>edit_overlay</code>, <code>resize_handle</code></td></tr>
    <tr><td>Scrolling</td><td><code>vertical_scrollbar</code>, <code>horizontal_scrollbar</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/23_table_cell_selection_roundtrip/ui/table_panel.wfrog</code></li>
  <li>Default Table package: <code>Libraries/Realizations/Default/table.default.wfrog</code></li>
  <li>Default shell: <code>Libraries/Realizations/Default/assets/table/templates/table_shell.svg</code></li>
  <li>Default subobjects: <code>Libraries/Realizations/Default/assets/table/subobjects/*.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>Selecting a cell patches front-panel selection state without executing the diagram.</li>
  <li>Editable control cells preserve command-side edits until <code>Execute</code>.</li>
  <li><code>Execute</code> consumes the selected numeric cell value and updates the read-only Table indicator through the native proof.</li>
  <li>Scrollbars must be source-owned, smooth in the browser host, and aligned to published parts.</li>
  <li>The runtime must not replace Table with a hardcoded HTML table whose rows, columns, or styling live outside <code>.frog/.wfrog</code>.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 23. When
published, it should live under <code>Examples/23_table_cell_selection_roundtrip/reference/</code>
and follow <a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>
