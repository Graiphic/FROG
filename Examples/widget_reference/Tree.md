<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Tree Widget Example Reference</h1>

<p align="center">
  <strong>Current Tree control and Tree indicator proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The stabilized Tree slice is <code>Examples/24_tree_node_selection_roundtrip</code>.
It proves a <code>frog.widgets.tree_control</code> command surface driving a read-only
<code>frog.widgets.tree_indicator</code> through a native manifest-backed selected-node value and an explicit
<code>frog.tree.records</code> diagram dataflow.
</p>

<pre><code>system_tree.value.selected_node_value
  -&gt; native u16 proof kernel
  -&gt; published_tree.value.selected_node_value
  -&gt; public selected_node_value

system_tree.nodes.*
  -&gt; frog.tree.records dataflow
  -&gt; published_tree.nodes.*
  -&gt; public published_tree_records</code></pre>

<p>
This page is a practical example note, not the normative Tree class law.
The normative class surface remains <code>Libraries/Widgets/Tree.md</code>, and the Default realization surface remains
<code>Libraries/Realizations/Default/Tree.md</code>.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<p>
The <code>.frog</code> source owns the diagram and all front-panel instance data:
</p>

<ul>
  <li><code>Examples/24_tree_node_selection_roundtrip/main.frog</code></li>
  <li>Tree control instance: <code>system_tree</code>, <code>frog.widgets.tree_control</code></li>
  <li>Tree indicator instance: <code>published_tree</code>, <code>frog.widgets.tree_indicator</code></li>
  <li>node ids, tags, labels, hierarchy, numeric values, check posture, item icon enum values, and editable attributes</li>
  <li>column ids, labels, value references, widths, visibility, and editability</li>
  <li>selection state: selected node id, selected column id, selected cell id, and selected numeric node value</li>
  <li>expansion state, scroll offsets, visible item posture, row height, header height, indentation, and layout</li>
  <li>instance-level style for labels, headers, rows, active cells, selection, scrollbars, item icon slots, expanders, and optional checkboxes</li>
</ul>

<p>
The Tree example deliberately contains more rows and columns than the visible viewport so both vertical and horizontal
scrollbar behavior can be inspected.
</p>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Current source properties</th>
      <th>Runtime obligation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Tree body</td>
      <td><code>style.node_region.*</code>, <code>viewport.*</code>, <code>display.*</code></td>
      <td>Render the body through the Default Tree <code>node_region</code> part and source-owned viewport posture.</td>
    </tr>
    <tr>
      <td>Rows and cells</td>
      <td><code>nodes.*</code>, <code>columns.*</code>, <code>style.node_row.*</code>, <code>style.active_cell.*</code></td>
      <td>Use source-owned node and column records; do not hardcode row labels, values, widths, or selection behavior.</td>
    </tr>
    <tr>
      <td>Expand/collapse control</td>
      <td><code>expansion.*</code>, <code>style.expander.*</code></td>
      <td>Expose one plus/minus control per expandable parent node; keep the glyph centered and independent from item icons.</td>
    </tr>
    <tr>
      <td>Item icon slot</td>
      <td><code>nodes.icon</code>, <code>style.node_icon_slot.*</code></td>
      <td>Resolve item symbols through the <code>.wfrog</code> item-symbol registry and consume Default SVG assets.</td>
    </tr>
    <tr>
      <td>Optional checkbox</td>
      <td><code>nodes.checked</code>, <code>display.checkboxes_visible</code>, <code>style.checkbox.*</code></td>
      <td>Keep checkbox styling separate from expanders and item icons.</td>
    </tr>
    <tr>
      <td>Scrollbars</td>
      <td><code>viewport.*</code>, <code>display.*scrollbar_visible</code>, <code>style.scrollbar.*</code></td>
      <td>Keep vertical and horizontal scrollbar tracks and thumbs configurable, centered, and aligned to published SVG parts.</td>
    </tr>
  </tbody>
</table>

<p>
The Default Tree SVG shell publishes static geometry parts. Dynamic row, cell, selection, expander, checkbox, icon, edit,
and scrollbar overlays are host-rendered from source-owned data and the Default realization contract.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<p>
The example package is:
</p>

<pre><code>Examples/24_tree_node_selection_roundtrip/ui/tree_panel.wfrog</code></pre>

<p>
It owns only realization references, asset references, item-symbol registry entries, and host capability declarations.
</p>

<ul>
  <li>Default Tree package: <code>Libraries/Realizations/Default/tree.default.wfrog</code></li>
  <li>Tree shell SVG: <code>Libraries/Realizations/Default/assets/tree/templates/tree_shell.svg</code></li>
  <li>Item symbol SVG assets: <code>assets/tree/item_symbols/check.svg</code>, <code>folder_closed.svg</code>, <code>folder_open.svg</code>, <code>document.svg</code>, and <code>leaf.svg</code></li>
  <li>Example asset ids consumed by runtimes: <code>tree_shell_svg</code> and <code>tree_item_symbol_*</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The runtime consumes <code>.frog</code> node, column, selection, expansion, viewport, and style records.</li>
  <li>The runtime resolves the Tree shell and item-symbol SVG assets from <code>.wfrog</code>; no runtime-private icon drawings are valid.</li>
  <li>Changing selection or expansion patches front-panel state only. It does not execute the diagram.</li>
  <li>Editable Tree cells preserve command-side edits until <code>Execute</code>.</li>
  <li><code>Execute</code> consumes the current source-modeled state, runs the manifest-backed native selected-node kernel, and publishes the declared Tree records flow.</li>
  <li>The Tree indicator remains read-only for value edits, but may allow viewport, expansion, and selection inspection when those interactions do not mutate the public tree payload.</li>
  <li>The runtime must reject fallback markers that replace the Tree with a hardcoded HTML-only shell.</li>
</ul>

<hr/>

<h2>Native Kernel Posture</h2>

<ul>
  <li>native manifest: <code>Implementations/Reference/LLVM/examples/24_tree_node_selection_roundtrip/native_kernel_manifest.json</code></li>
  <li>native ABI: <code>frog_u16_to_result_status_outptr</code></li>
  <li>lowering unit: <code>Examples/24_tree_node_selection_roundtrip/main.lowering.json</code></li>
  <li>The native proof kernel handles the selected numeric node value. The full Tree record payload is carried by declared diagram dataflow, not by hidden UI synchronization.</li>
  <li>Example 24 is accepted at the same C++/Python/Rust private-runtime source level; this does not claim generalized runtime completeness beyond the validated slice.</li>
</ul>

<hr/>

<h2>Resizable Host Posture</h2>

<p>
The Default Tree shell is SVG-backed and uses a <code>viewBox</code>, so a conforming IDE or runtime host can scale the widget
without bitmap pixelation. Scaling is not automatic layout correctness by itself: the host must recompute overlays for
rows, cells, scrollbars, expanders, checkboxes, item icons, labels, and edit surfaces from the published SVG parts and
the source-owned layout/style properties.
</p>

<p>
The current browser host uses CSS pixel coordinates as a concrete inspection surface. Another host, such as a Qt,
native desktop, or GPU-backed host, may render the same Tree if it preserves the <code>.frog</code> instance model,
<code>.wfrog</code> asset resolution, Default SVG part contract, and public property/method/event behavior.
</p>

<hr/>

<h2>Validation Notes</h2>

<ul>
  <li>The Tree must be manually inspected in the browser host before acceptance.</li>
  <li>Vertical and horizontal scrollbars must be smooth, aligned, centered, and configurable.</li>
  <li>Every selectable cell must show a uniform selection border width.</li>
  <li>Labels must be visually separated from the widget body and aligned with the source-owned layout.</li>
  <li>Item icons must remain static display enum values; expanding or collapsing a node must not mutate the item icon.</li>
  <li>There must be no duplicated example-local Tree SVG skin when the Default Tree realization exists.</li>
</ul>
