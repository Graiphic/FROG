<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Table Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized table control and table indicator widget classes</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Widgets index</a></li>
  <li><a href="../../Expression/Widget.md">Expression widget instances</a></li>
  <li><a href="../../Expression/Widget%20interaction.md">Expression widget interaction</a></li>
  <li><a href="../../Expression/Widget%20realization.md">Expression widget realization</a></li>
  <li><a href="../../Expression/Widget%20package%20(.wfrog).md">Widget package publication</a></li>
  <li><a href="../../Libraries/UI.md">Executable UI primitives</a></li>
  <li><a href="../Realizations/Default/Table.md">Default realization — Table</a></li>
</ul>

<hr/>

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.table_control</code></li>
  <li><code>frog.widgets.table_indicator</code></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
The Table family defines standardized widgets for flat row-and-column tabular data in FROG.
It provides a public value model, column schema, row identity posture, selection model, viewport model, editing posture, sorting and filtering posture, and stable public part model.
</p>

<p>
A Table widget is not merely a decorative grid.
It is a structured data widget suitable for measurement tables, parameter grids, logs, tabular inspection output, editable tabular inputs, and structured front-panel displays.
</p>

<p>
The intrinsic Table baseline intentionally remains flat.
Tree tables, hierarchical tables, pivot tables, spreadsheets with formula languages, database views, and virtualized host-grid engines are downstream profiles or host realizations unless explicitly standardized elsewhere.
</p>

<hr/>

<h2>Common Family Posture</h2>

<ul>
  <li>family: structured tabular data widget family</li>
  <li>primary value: present</li>
  <li>value type: <code>frog.table</code></li>
  <li>public value-facing surface: yes</li>
  <li>object-style access surface: yes</li>
  <li>primary value mirror property: <code>value</code></li>
  <li>common label property: <code>label.text</code></li>
  <li>common caption property: <code>caption.text</code></li>
  <li>common visibility property: <code>interaction.visible</code></li>
</ul>

<p>
The family separates:
</p>

<ul>
  <li><code>value</code> — the class-owned table payload,</li>
  <li><code>columns.*</code> — column schema and headers,</li>
  <li><code>rows.*</code> — row identity and row-count posture,</li>
  <li><code>cells.*</code> — cell-level access and edit surfaces,</li>
  <li><code>selection.*</code> — current table selection,</li>
  <li><code>viewport.*</code> — visible scroll window,</li>
  <li><code>sort.*</code> and <code>filter.*</code> — optional presentation / view postures,</li>
  <li>realization-private row pools, cell renderers, virtualization caches, host handles, and editing controls.</li>
</ul>

<hr/>

<h2><code>frog.widgets.table_control</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.table_control</code></li>
  <li><strong>family:</strong> <code>table_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.table</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes when editing is enabled</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A table control may allow cell editing, row selection, column selection, resizing, scrolling, sorting, and copy/paste-like host interactions when those surfaces are exposed by the active posture.
The intrinsic class does not require every host to implement spreadsheet-grade editing.
</p>

<hr/>

<h2><code>frog.widgets.table_indicator</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.table_indicator</code></li>
  <li><strong>family:</strong> <code>table_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.table</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no for table value in the standard portable posture</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A table indicator is display-oriented.
It may still support viewport changes, selection readout, copy selection, sorting view changes, or inspection gestures when those do not mutate the table value.
</p>

<hr/>

<h2>Table Value Model</h2>

<p>
The canonical portable table value is a structured object:
</p>

<table>
  <thead>
    <tr>
      <th>Field</th>
      <th>Meaning</th>
      <th>Baseline status</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>columns</code></td><td>Column schema list.</td><td>required</td></tr>
    <tr><td><code>rows</code></td><td>Row records or row vectors.</td><td>required</td></tr>
    <tr><td><code>cells</code></td><td>Optional explicit cell matrix representation.</td><td>optional alternative</td></tr>
    <tr><td><code>row_ids</code></td><td>Stable row identifiers when exposed.</td><td>optional</td></tr>
    <tr><td><code>metadata</code></td><td>Optional table-level metadata.</td><td>optional</td></tr>
  </tbody>
</table>

<h3>Column model</h3>

<p>
Each column may expose:
</p>

<ul>
  <li><code>columns[].id</code></li>
  <li><code>columns[].header</code></li>
  <li><code>columns[].type</code></li>
  <li><code>columns[].width</code></li>
  <li><code>columns[].alignment</code></li>
  <li><code>columns[].format</code></li>
  <li><code>columns[].editable</code></li>
  <li><code>columns[].visible</code></li>
</ul>

<h3>Cell model</h3>

<p>
Each cell value belongs to one row and one column.
Portable cell values may contain scalar values such as booleans, strings, numeric values, enum values, paths, timestamps, or profile-supported scalar payloads.
A cell may also carry optional presentation metadata such as format, alignment, validation state, or read-only state when exposed by the active profile.
</p>

<h3>Row model</h3>

<p>
Rows may be represented as arrays indexed by column order or as objects keyed by column id.
When <code>row_ids</code> are present, selection and editing surfaces SHOULD preserve those ids across sorting and filtering.
</p>

<hr/>

<h2>Selection Model</h2>

<p>
The Table family supports bounded table selection.
</p>

<ul>
  <li><code>selection.mode</code> — <code>none</code>, <code>cell</code>, <code>row</code>, <code>column</code>, <code>range</code>, or <code>multi_range</code></li>
  <li><code>selection.active_cell</code></li>
  <li><code>selection.anchor_cell</code></li>
  <li><code>selection.ranges</code></li>
  <li><code>selection.selected_rows</code></li>
  <li><code>selection.selected_columns</code></li>
</ul>

<p>
Selection is distinct from value.
Changing selection does not mutate table data unless a method or edit operation explicitly mutates table data.
</p>

<hr/>

<h2>Editing Model</h2>

<ul>
  <li><code>editing.enabled</code></li>
  <li><code>editing.active_cell</code></li>
  <li><code>editing.commit_policy</code> — <code>on_enter</code>, <code>on_focus_lost</code>, <code>explicit</code></li>
  <li><code>editing.validation_policy</code></li>
  <li><code>editing.pending_value</code></li>
</ul>

<p>
Editing is legal for <code>frog.widgets.table_control</code> when enabled.
Editing table data through a table indicator is not part of the portable intrinsic posture.
</p>

<hr/>

<h2>Viewport, Sort, and Filter Model</h2>

<h3>Viewport</h3>

<ul>
  <li><code>viewport.row_offset</code></li>
  <li><code>viewport.column_offset</code></li>
  <li><code>viewport.visible_rows</code></li>
  <li><code>viewport.visible_columns</code></li>
  <li><code>viewport.row_height</code></li>
  <li><code>viewport.default_column_width</code></li>
</ul>

<h3>Sorting</h3>

<ul>
  <li><code>sort.enabled</code></li>
  <li><code>sort.keys</code></li>
  <li><code>sort.direction</code> — <code>ascending</code> or <code>descending</code></li>
</ul>

<h3>Filtering</h3>

<ul>
  <li><code>filter.enabled</code></li>
  <li><code>filter.expression</code></li>
  <li><code>filter.visible_row_count</code></li>
</ul>

<p>
Sorting and filtering are view postures unless explicitly exposed as value transformations by another operation.
</p>

<hr/>

<h2>Standard Properties</h2>

<h3>Common properties</h3>

<ul>
  <li><code>value</code></li>
  <li><code>label.text</code></li>
  <li><code>label.visible</code></li>
  <li><code>caption.text</code></li>
  <li><code>caption.visible</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>interaction.enabled</code></li>
</ul>

<h3>Table structure properties</h3>

<ul>
  <li><code>rows.count</code></li>
  <li><code>columns.count</code></li>
  <li><code>columns.headers</code></li>
  <li><code>columns.schema</code></li>
  <li><code>cells.value</code></li>
  <li><code>cells.format</code></li>
  <li><code>cells.read_only</code></li>
</ul>

<h3>Visible item properties</h3>

<p>
Table realizations expose LabVIEW-like visible-item posture as source-owned properties.
The runtime may render host overlays, scroll tracks, headers, index displays, and selection faces, but it must consume these properties and the published Default Table parts instead of baking a table shell into runtime code.
</p>

<ul>
  <li><code>display.index_display_visible</code></li>
  <li><code>display.vertical_scrollbar_visible</code></li>
  <li><code>display.horizontal_scrollbar_visible</code></li>
  <li><code>display.row_headers_visible</code></li>
  <li><code>display.column_headers_visible</code></li>
  <li><code>display.vertical_lines_visible</code></li>
  <li><code>display.horizontal_lines_visible</code></li>
  <li><code>display.symbols_visible</code></li>
</ul>

<p>
Row and column header text maps to <code>rows[].label</code> and <code>columns[].label</code>.
Selection color maps to <code>style.selection_face.*</code>.
The active cell maps to <code>selection.active_cell.*</code>.
Scrollbar colors, borders, and thumb geometry map to <code>style.scrollbar.*</code>.
</p>

<h3>Interaction properties</h3>

<ul>
  <li><code>selection.*</code></li>
  <li><code>interaction.selection_scrolling_enabled</code></li>
  <li><code>headers.editable</code></li>
  <li><code>columns.separators_movable</code></li>
  <li><code>rows.autosize_height</code></li>
  <li><code>input.multi_line_enabled</code></li>
  <li><code>scrolling.smooth_enabled</code></li>
  <li><code>editing.*</code></li>
  <li><code>viewport.*</code></li>
  <li><code>sort.*</code></li>
  <li><code>filter.*</code></li>
</ul>

<p>
If a runtime does not yet implement an advanced operation such as editable headers, moveable separators, or multi-line editing, it must report the limitation explicitly.
It must not silently replace the source-owned property model with hardcoded runtime behavior.
</p>

<hr/>

<h2>Standard Methods</h2>

<ul>
  <li><code>focus()</code></li>
  <li><code>clear_selection()</code></li>
  <li><code>set_selection(selection)</code></li>
  <li><code>select_cell(row, column)</code></li>
  <li><code>select_row(row)</code></li>
  <li><code>select_column(column)</code></li>
  <li><code>scroll_to(row, column)</code></li>
  <li><code>scroll_to_row(row)</code></li>
  <li><code>scroll_to_column(column)</code></li>
  <li><code>set_cell_value(row, column, value)</code></li>
  <li><code>get_cell_value(row, column)</code></li>
  <li><code>insert_row(index, row)</code></li>
  <li><code>remove_row(index)</code></li>
  <li><code>append_row(row)</code></li>
  <li><code>clear_rows()</code></li>
  <li><code>resize_column(column, width)</code></li>
  <li><code>sort_by(column, direction)</code></li>
  <li><code>clear_sort()</code></li>
  <li><code>apply_filter(filter)</code></li>
  <li><code>clear_filter()</code></li>
</ul>

<hr/>

<h2>Standard Events</h2>

<ul>
  <li><code>value_changed</code></li>
  <li><code>cell_value_changed</code></li>
  <li><code>selection_changed</code></li>
  <li><code>viewport_changed</code></li>
  <li><code>sort_changed</code></li>
  <li><code>filter_changed</code></li>
  <li><code>row_inserted</code></li>
  <li><code>row_removed</code></li>
  <li><code>column_resized</code></li>
  <li><code>edit_started</code></li>
  <li><code>edit_committed</code></li>
  <li><code>edit_cancelled</code></li>
  <li><code>cell_clicked</code></li>
  <li><code>header_clicked</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2>Public Parts</h2>

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

<p>
Rows, cells, and selection faces are public part categories.
Realization-private row pools, recycled cell renderers, virtualization caches, host-grid handles, or editor controls are not semantic storage.
</p>

<hr/>

<h2>Diagram Interaction Posture</h2>

<p>
The Table family supports:
</p>

<ul>
  <li>natural value participation through <code>widget_value</code>,</li>
  <li>property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>,</li>
  <li>method invocation where legal,</li>
  <li>event observation where legal.</li>
</ul>

<p>
Ordinary table dataflow should prefer <code>widget_value</code>.
Object-style access should be used for selection, viewport, editing, sorting, filtering, and cell-level interaction.
</p>

<hr/>

<h2>Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-table <code>value</code> payloads,</li>
  <li>inconsistent row and column dimensions,</li>
  <li>column schema entries without stable identifiers,</li>
  <li>cell values incompatible with declared column type,</li>
  <li>selection ranges outside table bounds,</li>
  <li>invalid active cell,</li>
  <li>invalid viewport offset or visible window size,</li>
  <li>duplicate column identifiers,</li>
  <li>unsupported edit operations on indicators,</li>
  <li>attempts to treat realization-only row handles, cell renderers, virtualized row caches, or host-native grid handles as public semantic storage.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
The Table family provides the standard portable flat tabular data baseline of FROG:
</p>

<ul>
  <li><code>frog.widgets.table_control</code></li>
  <li><code>frog.widgets.table_indicator</code></li>
</ul>

<p>
It covers table value, schema, rows, cells, selection, viewport, editing, sorting, filtering, and a stable part model while keeping host-native virtualization and renderer internals downstream from class law.
</p>
