<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 23 - Table Cell Selection Roundtrip</h1>

<p align="center">
  <strong>Table numeric selected-cell value through Default SVG assets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This example introduces a dedicated Table widget after the current public
reference runtime closure. It keeps the public source, FIR/lowering,
realization package, Default SVG asset reference, and native manifest surface
visible while Graiphic runtime implementation continues privately.
</p>

<p>
The front panel contains one <code>frog.widgets.table_control</code> and one
<code>frog.widgets.table_indicator</code>. The control lets the user select one
source-owned numeric table cell. The diagram-facing value is the selected
cell's numeric <code>u16</code> value. The explicit Execute action publishes
that value through the manifest-backed native proof and updates the read-only
Table indicator.
</p>

<hr/>

<h2>Source / Realization Boundary</h2>

<ul>
  <li><code>.frog</code> owns the Table instances, column schema, row ids, cell values, selected-cell state, labels, captions, layout, bindings, visible-item toggles, interaction posture, and instance-level style overrides.</li>
  <li><code>.wfrog</code> references the Default Table realization package, SVG asset, and host capability declarations.</li>
  <li>The Default Table SVG asset publishes <code>frame</code>, <code>header_region</code>, <code>column_header_region</code>, <code>row_header_region</code>, <code>grid_region</code>, <code>cell</code>, <code>selection_face</code>, <code>active_cell</code>, <code>edit_overlay</code>, <code>vertical_scrollbar</code>, <code>horizontal_scrollbar</code>, <code>caption</code>, <code>label</code>, and <code>focus_ring</code> parts.</li>
  <li>The runtime consumes the manifest-backed native <code>u16</code> artifact and binds host overlays to the published SVG parts.</li>
</ul>

<hr/>

<h2>Native Proof</h2>

<p>
The LLVM/native artifact is intentionally a numeric identity proof:
</p>

<pre><code>selected_cell_value = cell_value</code></pre>

<p>
The Table widget owns table UI state and cell records. The diagram sees the
selected numeric cell value, while the runtime maps that value back to the
matching source-owned table cell for display.
</p>

<hr/>

<h2>Runtime Status</h2>

<p>
This example is repository-visible public specification material beyond the
current public reference runtime closure. Its private implementation is aligned
across the C++, Python, and Rust families in <code>Graiphic/FROG-Runtime</code>
after C++ visual and behavioral acceptance.
</p>
