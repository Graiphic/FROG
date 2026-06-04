<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Widget Example Reference Notes</h1>

<p align="center">
  <strong>Implementation notes for the currently stabilized FROG widget examples</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
These notes describe the current repository-proven widget examples.
They are not the normative widget class law.
The class law remains under <code>Libraries/Widgets/</code>, the Default realization law remains under
<code>Libraries/Realizations/Default/</code>, and the numbered examples remain executable proof slices.
</p>

<p>
Each page records the practical contract that must stay true while the examples evolve:
</p>

<ul>
  <li>the <code>.frog</code> source owns diagram logic, front-panel instances, layout, binding, initial values, and instance-level visual customization;</li>
  <li>the example <code>.wfrog</code> package owns realization references, Default SVG asset references, and host capability requirements;</li>
  <li>the runtime consumes the <code>.frog</code> instance data, <code>.wfrog</code> package, Default SVG public parts, and native manifest when the slice is native-backed;</li>
  <li>no accepted widget example may rely on a hardcoded HTML/CSS fallback or duplicated local SVG skin.</li>
</ul>

<hr/>

<h2>Current Stabilized Widgets</h2>

<table>
  <thead>
    <tr>
      <th>Widget family</th>
      <th>Example</th>
      <th>Reference note</th>
      <th>Default realization package</th>
      <th>Default SVG asset</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Numeric</td>
      <td><code>05_bounded_ui_accumulator</code></td>
      <td><a href="./Numeric.md">Numeric.md</a></td>
      <td><code>Libraries/Realizations/Default/numeric.default.wfrog</code></td>
      <td><code>assets/numeric/templates/numeric_rectangular.svg</code></td>
    </tr>
    <tr>
      <td>Boolean</td>
      <td><code>06_boolean_value_roundtrip</code></td>
      <td><a href="./Boolean.md">Boolean.md</a></td>
      <td><code>Libraries/Realizations/Default/boolean.default.wfrog</code></td>
      <td><code>assets/boolean/templates/boolean_rectangular.svg</code>, <code>boolean_circular.svg</code></td>
    </tr>
    <tr>
      <td>String<br/><small>Reviewed 2026-06-04</small></td>
      <td><code>07_string_value_roundtrip</code></td>
      <td><a href="./String.md">String.md</a></td>
      <td><code>Libraries/Realizations/Default/string.default.wfrog</code></td>
      <td><code>assets/string/templates/string_rectangular.svg</code></td>
    </tr>
    <tr>
      <td>Enum</td>
      <td><code>08_enum_value_roundtrip</code></td>
      <td><a href="./Enum.md">Enum.md</a></td>
      <td><code>Libraries/Realizations/Default/enum.default.wfrog</code></td>
      <td><code>assets/enum/templates/enum_rectangular_ring.svg</code></td>
    </tr>
    <tr>
      <td>Path</td>
      <td><code>09_path_value_roundtrip</code></td>
      <td><a href="./Path.md">Path.md</a></td>
      <td><code>Libraries/Realizations/Default/path.default.wfrog</code></td>
      <td><code>assets/path/templates/path_rectangular_field.svg</code></td>
    </tr>
    <tr>
      <td>Button</td>
      <td><code>10_button_press_to_boolean</code></td>
      <td><a href="./Button.md">Button.md</a></td>
      <td><code>Libraries/Realizations/Default/button.default.wfrog</code></td>
      <td><code>assets/button/templates/button_rectangular.svg</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Post-Boundary Private Runtime Widget Notes</h2>

<p>
The following repository-visible widget notes document later specification-facing examples.
They are validated through Graiphic private runtime slices and do not expand the current public reference runtime
closure unless that boundary is explicitly promoted later.
</p>

<table>
  <thead>
    <tr>
      <th>Widget family</th>
      <th>Example</th>
      <th>Reference note</th>
      <th>Default realization package</th>
      <th>Default SVG asset</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Picture</td>
      <td><code>16_picture_logo_jpeg</code></td>
      <td><a href="./Picture.md">Picture.md</a></td>
      <td><code>Libraries/Realizations/Default/picture.default.wfrog</code></td>
      <td><code>assets/picture/templates/picture_rectangular.svg</code></td>
    </tr>
    <tr>
      <td>Label</td>
      <td><code>17_label_typography_board</code></td>
      <td><a href="./Label.md">Label.md</a></td>
      <td><code>Libraries/Realizations/Default/label.default.wfrog</code></td>
      <td><code>assets/label/templates/label_rectangular.svg</code></td>
    </tr>
    <tr>
      <td>Decorations</td>
      <td><code>18_decoration_board</code></td>
      <td><a href="./Decorations.md">Decorations.md</a></td>
      <td><code>Libraries/Realizations/Default/decorations.default.wfrog</code></td>
      <td><code>assets/decorations/templates/flat_box.svg</code>, <code>horizontal_line.svg</code>, <code>vertical_line.svg</code></td>
    </tr>
    <tr>
      <td>Subpanel</td>
      <td><code>19_subpanel_embedded_front_panel</code></td>
      <td><a href="./Subpanel.md">Subpanel.md</a></td>
      <td><code>Libraries/Realizations/Default/subpanel.default.wfrog</code></td>
      <td><code>assets/subpanel/templates/subpanel_shell.svg</code></td>
    </tr>
    <tr>
      <td>Tab</td>
      <td><code>20_tab_page_selection</code></td>
      <td><a href="./Tab.md">Tab.md</a></td>
      <td><code>Libraries/Realizations/Default/tab.default.wfrog</code></td>
      <td><code>assets/tab/templates/tab_shell.svg</code> + <code>assets/tab/subobjects/*.svg</code></td>
    </tr>
    <tr>
      <td>Ring</td>
      <td><code>21_ring_selection_roundtrip</code></td>
      <td><a href="./Ring.md">Ring.md</a></td>
      <td><code>Libraries/Realizations/Default/ring.default.wfrog</code></td>
      <td><code>assets/ring/templates/ring_rectangular.svg</code></td>
    </tr>
    <tr>
      <td>Listbox</td>
      <td><code>22_listbox_selection_roundtrip</code></td>
      <td><a href="./Listbox.md">Listbox.md</a></td>
      <td><code>Libraries/Realizations/Default/listbox.default.wfrog</code></td>
      <td><code>assets/listbox/templates/listbox_shell.svg</code> + <code>assets/listbox/subobjects/*.svg</code></td>
    </tr>
    <tr>
      <td>Table</td>
      <td><code>23_table_cell_selection_roundtrip</code></td>
      <td><a href="./Table.md">Table.md</a></td>
      <td><code>Libraries/Realizations/Default/table.default.wfrog</code></td>
      <td><code>assets/table/templates/table_shell.svg</code> + <code>assets/table/subobjects/*.svg</code></td>
    </tr>
    <tr>
      <td>Tree</td>
      <td><code>24_tree_node_selection_roundtrip</code></td>
      <td><a href="./Tree.md">Tree.md</a></td>
      <td><code>Libraries/Realizations/Default/tree.default.wfrog</code></td>
      <td><code>assets/tree/templates/tree_shell.svg</code> + <code>assets/tree/item_symbols/*.svg</code></td>
    </tr>
    <tr>
      <td>Frame</td>
      <td><code>25_frame_grouping_board</code></td>
      <td><a href="./Frame.md">Frame.md</a></td>
      <td><code>Libraries/Realizations/Default/frame.default.wfrog</code></td>
      <td><code>assets/frame/templates/frame_rectangular.svg</code></td>
    </tr>
    <tr>
      <td>Array</td>
      <td><code>26_array_numeric_selection_roundtrip</code> through <code>28_array_3d_numeric_selection_roundtrip</code><br/><small>deprecated historical rigid milestones</small><br/><code>29_array_1d_numeric_container_roundtrip</code> and later<br/><small>current widget-composed container direction</small></td>
      <td><a href="./Array.md">Array.md</a></td>
      <td><code>Libraries/Realizations/Default/array.default.wfrog</code></td>
      <td><code>assets/array/templates/array_shell.svg</code> + contained widget assets such as <code>assets/numeric/templates/numeric_rectangular.svg</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Runtime Parity Snapshot</h2>

<p>
The stabilized widget examples are expected to run through the same reference-runtime family in every implemented
language. The runtime may be written in C++, Python, or Rust, but it must consume the same source-owned front-panel
instance data, the same <code>.wfrog</code> realization package, and the same native manifest truth when the slice is
native-backed.
</p>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Widget family</th>
      <th>C++</th>
      <th>Python</th>
      <th>Rust</th>
      <th>Execution posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>05</code></td>
      <td>Numeric</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>06</code></td>
      <td>Boolean</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>07</code></td>
      <td>String</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>08</code></td>
      <td>Enum</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>09</code></td>
      <td>Path</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>10</code></td>
      <td>Button</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>16</code></td>
      <td>Picture</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + frog.image provider + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>17</code></td>
      <td>Label</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + source-owned support text + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>18</code></td>
      <td>Decorations</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + source-owned support geometry + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>19</code></td>
      <td>Subpanel</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + hosted child .frog front panel + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>20</code></td>
      <td>Tab</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + selected-page string proof + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>21</code></td>
      <td>Ring</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + selected numeric value proof + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>22</code></td>
      <td>Listbox</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + selected numeric value proof + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>23</code></td>
      <td>Table</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + selected table cell proof + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>24</code></td>
      <td>Tree</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native kernel manifest + explicit frog.tree.records dataflow + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>25</code></td>
      <td>Frame</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td>validated private slice</td>
      <td><code>LLVM native manifest + source-owned support frame geometry + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>26-28</code></td>
      <td>Array</td>
      <td>deprecated historical slice</td>
      <td>deprecated historical slice</td>
      <td>deprecated historical slice</td>
      <td><code>LLVM native kernel manifest + rigid/non-widget-composed Array shape/viewport/element data + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>29-37</code></td>
      <td>Array</td>
      <td>validated private slices</td>
      <td>validated where explicitly recorded</td>
      <td>validated where explicitly recorded</td>
      <td><code>LLVM native kernel manifest + source-owned Array container data + contained Default widget realizations + .frog/.wfrog UI</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Reference Snapshot Coverage</h2>

<p>
Widget reference snapshots are accepted-state evidence. They are published only
after the visible example state has been accepted and captured according to
<a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Widget family</th>
      <th>Public snapshot status</th>
      <th>Reference path</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>16</code></td><td>Picture</td><td>pending accepted public snapshot</td><td><code>Examples/16_picture_logo_jpeg/reference/</code></td></tr>
    <tr><td><code>17</code></td><td>Label</td><td>pending accepted public snapshot</td><td><code>Examples/17_label_typography_board/reference/</code></td></tr>
    <tr><td><code>18</code></td><td>Decorations</td><td>pending accepted public snapshot</td><td><code>Examples/18_decoration_board/reference/</code></td></tr>
    <tr><td><code>19</code></td><td>Subpanel</td><td>pending accepted public snapshot</td><td><code>Examples/19_subpanel_embedded_front_panel/reference/</code></td></tr>
    <tr><td><code>20</code></td><td>Tab</td><td>pending accepted public snapshot</td><td><code>Examples/20_tab_page_selection/reference/</code></td></tr>
    <tr><td><code>21</code></td><td>Ring</td><td>pending accepted public snapshot</td><td><code>Examples/21_ring_selection_roundtrip/reference/</code></td></tr>
    <tr><td><code>22</code></td><td>Listbox</td><td>pending accepted public snapshot</td><td><code>Examples/22_listbox_selection_roundtrip/reference/</code></td></tr>
    <tr><td><code>23</code></td><td>Table</td><td>pending accepted public snapshot</td><td><code>Examples/23_table_cell_selection_roundtrip/reference/</code></td></tr>
    <tr><td><code>24</code></td><td>Tree</td><td>pending accepted public snapshot</td><td><code>Examples/24_tree_node_selection_roundtrip/reference/</code></td></tr>
    <tr><td><code>25</code></td><td>Frame</td><td>pending accepted public snapshot</td><td><code>Examples/25_frame_grouping_board/reference/</code></td></tr>
    <tr><td><code>30</code></td><td>Array</td><td><a href="../30_array_1d_horizontal_numeric_container_roundtrip/reference/README.md">accepted public snapshot published</a></td><td><code>Examples/30_array_1d_horizontal_numeric_container_roundtrip/reference/</code></td></tr>
  </tbody>
</table>

<p>
Pending rows are deliberate: the repository should not publish placeholder
screenshots or claim accepted visual evidence before the browser-host state is
captured and reviewed.
</p>

<hr/>

<h2>Ownership Boundary</h2>

<table>
  <thead>
    <tr>
      <th>Concern</th>
      <th>Owner</th>
      <th>Validation expectation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Diagram algorithm</td>
      <td><code>.frog</code></td>
      <td>Derives to FIR, lowers, emits native kernel, and produces the expected runtime output.</td>
    </tr>
    <tr>
      <td>Front-panel widget instances</td>
      <td><code>.frog</code></td>
      <td>Owns widget id, class, role, layout, binding, initial value, label/caption placement, and instance style.</td>
    </tr>
    <tr>
      <td>Published widget realization</td>
      <td><code>.wfrog</code></td>
      <td>Points to the Default realization package and declares which SVG asset ids the runtime must resolve.</td>
    </tr>
    <tr>
      <td>Visual geometry hooks</td>
      <td>Default SVG asset</td>
      <td>Exposes stable <code>data-frog-part</code>, <code>data-frog-anchor</code>, and <code>data-frog-bind</code> surfaces.</td>
    </tr>
    <tr>
      <td>Execution bridge</td>
      <td>Runtime + native manifest</td>
      <td>The runtime loads a manifest-declared ABI artifact and does not become LLVM-only.</td>
    </tr>
    <tr>
      <td>Visible UI host overlays</td>
      <td>Runtime</td>
      <td>May provide host input overlays, but must align them to published SVG parts and source-owned instance data.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Public Boundary</h2>

<p>
Widget examples in this directory remain framed by
<a href="../widget_public_boundary.md">the public widget example boundary</a>.
The public evidence corridor is:
</p>

<pre><code>.frog diagram + front-panel instances
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM kernel + native manifest
  -&gt; runtime C++ / Python / Rust
  -&gt; .wfrog realization package
  -&gt; Default SVG asset
  -&gt; faithful visible UI
</code></pre>

<p>
The public repository documents this ownership split and its repository-visible
evidence. Graiphic internal acceptance recipes and manual review procedures are
not published as part of these reference notes.
</p>
