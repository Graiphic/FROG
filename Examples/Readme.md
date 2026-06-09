<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Examples</h1>

<p align="center">
  <strong>Executable example dossiers for the published FROG specification</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory contains the repository-visible numbered example progression.
Examples are not semantic owners.
They are published slices used to inspect how source, FIR, lowering, contracts, runtimes, and native proof paths relate.
</p>

<p>
New examples should follow the
<a href="./example_dossier_standard.md">FROG Example Dossier Standard</a>.
That document defines the shared public organization for widget, operator,
structure, library, compiler, and conformance examples without publishing
Graiphic's internal validation recipe.
Accepted examples may also publish a small
<a href="./example_reference_snapshot_standard.md">Example Reference Snapshot</a>
that records the accepted screenshot, public runtime snapshot, visual contract,
and source/artifact hash index without duplicating source truth.
</p>

<hr/>

<h2>Public Runtime Boundary</h2>

<p>
Examples <code>01</code> through <code>15</code> define the current public reference runtime closure.
</p>

<p>
Examples beyond this boundary may remain repository-visible as specification-facing, widget-facing, conformance-facing, or design-progression examples.
They should not be interpreted as a commitment to publish Graiphic's production runtime implementation for those examples.
</p>

<p>
Production-grade runtime development for future FROG execution surfaces continues in Graiphic proprietary repositories, including <code>Graiphic/FROG-Runtime</code>, while preserving the public contracts required for independent implementations.
</p>

<h2>Browser-Hosted Inspection Surface</h2>

<p>
When a browser-hosted example is used for validation, the visible page should expose a collapsed-by-default source and execution artifact inspector.
Opening the inspector should show formatted in-page content, while raw routes remain available for direct checks.
The expected inspection surface includes <code>state.json</code>, the owning <code>main.frog</code>, the directly consumed example <code>.wfrog</code> package, and FIR/lowering artifacts when those artifacts exist.
</p>

<p>
This browser presentation is a validation host convention, not the FROG language runtime identity.
Other front-panel hosts may expose the same source, realization, FIR, lowering, manifest, and diagnostic contracts through an equivalent inspection surface.
</p>

<p>
When a widget-facing example is accepted, a <code>reference/</code> directory
may preserve the visible browser-host state and visual contract as public
evidence. The snapshot is subordinate to the owning source and artifacts; it is
not a private runtime recipe and it does not redefine the example's validation
boundary.
</p>

<hr/>

<h2>Current Example Progression</h2>

<p>
Last worked dates use ISO 8601 format and record the latest known public example-dossier work date.
They are traceability metadata only; they are not FROG language versions and they do not redefine the validation boundary.
</p>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Last worked</th>
      <th>Focus</th>
      <th>Starts at</th>
      <th>Current repository status</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>01_pure_addition</code></td>
      <td><time datetime="2026-05-15">2026-05-15</time></td>
      <td>Pure public-interface arithmetic</td>
      <td><code>main.frog</code></td>
      <td><code>FIR → lowering → contract → runtime → LLVM</code></td>
    </tr>
    <tr>
      <td><code>02_ui_value_roundtrip</code></td>
      <td><time datetime="2026-05-15">2026-05-15</time></td>
      <td>Natural widget-value participation</td>
      <td><code>main.frog</code> with inline front panel</td>
      <td><code>FIR → lowering → contract → runtime → LLVM</code></td>
    </tr>
    <tr>
      <td><code>03_ui_property_write</code></td>
      <td><time datetime="2026-05-15">2026-05-15</time></td>
      <td>Object-style widget reference and property write</td>
      <td><code>main.frog</code> with inline front panel</td>
      <td><code>FIR → lowering → contract → runtime → LLVM</code></td>
    </tr>
    <tr>
      <td><code>04_stateful_feedback_delay</code></td>
      <td><time datetime="2026-05-15">2026-05-15</time></td>
      <td>Explicit state and feedback through <code>frog.core.delay</code></td>
      <td><code>main.frog</code></td>
      <td><code>FIR → lowering → contract → runtime → LLVM</code></td>
    </tr>
    <tr>
      <td><code>05_bounded_ui_accumulator</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Full bounded UI accumulator corridor</td>
      <td><code>main.frog</code> + <code>ui/accumulator_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → runtime acceptance → LLVM module/native proof</code></td>
    </tr>
    <tr>
      <td><code>06_boolean_value_roundtrip</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Boolean control/indicator value roundtrip</td>
      <td><code>main.frog</code> + <code>ui/boolean_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → LLVM native manifest → C++/Python/Rust runtime bridge → Default Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>07_string_value_roundtrip</code></td>
      <td><time datetime="2026-06-04">2026-06-04</time></td>
      <td>String control/indicator placeholder launch-state roundtrip</td>
      <td><code>main.frog</code> + <code>ui/string_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → LLVM native manifest → C++/Python/Rust runtime bridge → Default String realization</code></td>
    </tr>
    <tr>
      <td><code>08_enum_value_roundtrip</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Enum control/indicator mode roundtrip</td>
      <td><code>main.frog</code> + <code>ui/enum_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → LLVM native manifest → C++/Python/Rust runtime bridge → Default Enum realization</code></td>
    </tr>
    <tr>
      <td><code>09_path_value_roundtrip</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Path control/indicator value roundtrip</td>
      <td><code>main.frog</code> + <code>ui/path_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → LLVM native manifest → C++/Python/Rust runtime bridge → Default Path realization</code></td>
    </tr>
    <tr>
      <td><code>10_button_press_to_boolean</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Button <code>switch_until_released</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>11_button_switch_when_pressed</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Button <code>switch_when_pressed</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>12_button_switch_when_released</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Button <code>switch_when_released</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>13_button_latch_when_pressed</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Button <code>latch_when_pressed</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>14_button_latch_when_released</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Button <code>latch_when_released</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>15_button_latch_until_released</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Button <code>latch_until_released</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>16_picture_logo_jpeg</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Path-selected PNG/JPEG/BMP decoded by <code>frog.image.decode_file_rgba8</code> and displayed by a Picture indicator</td>
      <td><code>main.frog</code> + <code>ui/picture_panel.wfrog</code> + <code>assets/frog_logo.jpg</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>17_label_typography_board</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Standalone Label support widgets with source-owned typography, frames, backgrounds, and annotation text</td>
      <td><code>main.frog</code> + <code>ui/label_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>18_decoration_board</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Standalone Decoration support widgets with one source-owned flat box, one horizontal line, and one vertical line</td>
      <td><code>main.frog</code> + <code>ui/decoration_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>19_subpanel_embedded_front_panel</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Subpanel host region displaying a referenced child <code>.frog</code> front panel</td>
      <td><code>main.frog</code> + <code>embedded/embedded_status.frog</code> + <code>ui/subpanel_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>20_tab_page_selection</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Tab control page selection with wrapped page headers and a movable page label display</td>
      <td><code>main.frog</code> + <code>ui/tab_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>21_ring_selection_roundtrip</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Ring numeric selected-value roundtrip with a read-only Ring indicator</td>
      <td><code>main.frog</code> + <code>ui/ring_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>22_listbox_selection_roundtrip</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Listbox numeric selected-value roundtrip with a read-only Listbox indicator</td>
      <td><code>main.frog</code> + <code>ui/listbox_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>23_table_cell_selection_roundtrip</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Table numeric selected-cell roundtrip with a read-only Table indicator</td>
      <td><code>main.frog</code> + <code>ui/table_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>24_tree_node_selection_roundtrip</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Tree numeric selected-node roundtrip with parent/child expansion, editable cells, and scrollable columns/rows</td>
      <td><code>main.frog</code> + <code>ui/tree_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>25_frame_grouping_board</code></td>
      <td><time datetime="2026-05-30">2026-05-30</time></td>
      <td>Standalone simple Frame support rectangles with source-owned size, background color, border color, and uniform border thickness</td>
      <td><code>main.frog</code> + <code>ui/frame_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>26_array_numeric_selection_roundtrip</code></td>
      <td><time datetime="2026-06-04">2026-06-04</time></td>
      <td>Deprecated historical 2D rigid Array numeric selected-element milestone retained for regression and traceability</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Deprecated historical milestone: non-widget-composed Array, superseded by widget-composed Array container examples starting at <code>29</code></td>
    </tr>
    <tr>
      <td><code>27_array_1d_vertical_numeric_selection_roundtrip</code></td>
      <td><time datetime="2026-06-04">2026-06-04</time></td>
      <td>Deprecated historical 1D vertical rigid Array numeric selected-element milestone retained for regression and traceability</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Deprecated historical milestone: non-widget-composed Array, superseded by widget-composed Array container examples starting at <code>29</code></td>
    </tr>
    <tr>
      <td><code>28_array_3d_numeric_selection_roundtrip</code></td>
      <td><time datetime="2026-06-04">2026-06-04</time></td>
      <td>Deprecated historical 3D rigid Array numeric selected-element milestone retained for regression and traceability</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Deprecated historical milestone: non-widget-composed Array, superseded by widget-composed Array container examples starting at <code>29</code></td>
    </tr>
    <tr>
      <td><code>29_array_1d_numeric_container_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>1D Array container with cells realized as Default Numeric widget instances and a read-only Array/Numeric indicator</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>30_array_1d_horizontal_numeric_container_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>1D horizontal Array container with cells realized as Default Numeric widget instances and a read-only horizontal Array/Numeric indicator</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>31_array_1d_visible_count_property_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>1D Array container whose visible cell count is driven by a Numeric U8 property-write control consumed on Execute</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>32_array_1d_horizontal_visible_count_property_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>1D horizontal Array container whose visible column count is driven by a Numeric U8 property-write control consumed on Execute</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>33_array_2d_numeric_container_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>2D Array container with cells realized as Default Numeric widget instances and a read-only 2D Array/Numeric indicator</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>34_array_2d_visible_counts_property_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>2D Array container whose visible row and column counts are driven by Numeric U8 property-write controls consumed on Execute, including visible counts beyond the current materialized shape</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>35_array_dimension_count_property_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>Numeric Array container whose active N-dimensional rank and visible row/column counts are driven by Numeric U8 property-write controls consumed on Execute</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>36_array_string_container_dimension_count_roundtrip</code></td>
      <td><time datetime="2026-06-02">2026-06-02</time></td>
      <td>Array container whose cells are Default String widgets while N-dimensional rank and visible row/column counts remain driven by Numeric U8 property-write controls consumed on Execute</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>37_array_path_container_dimension_count_roundtrip</code></td>
      <td><time datetime="2026-06-02">2026-06-02</time></td>
      <td>Array container whose cells are Default Path widgets while N-dimensional rank and visible row/column counts remain driven by Numeric U8 property-write controls consumed on Execute</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>38_array_boolean_container_dimension_count_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>Array container whose cells are Default Boolean widgets while N-dimensional rank and visible row/column counts remain driven by Numeric U8 property-write controls consumed on Execute</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>39_array_enum_container_dimension_count_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>Array container whose cells are Default Enum widgets while N-dimensional rank and visible row/column counts remain driven by Numeric U8 property-write controls consumed on Execute</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>40_array_ring_container_dimension_count_roundtrip</code></td>
      <td><time datetime="2026-06-07">2026-06-07</time></td>
      <td>Array container whose cells are Default Ring widgets while N-dimensional rank and visible row/column counts remain driven by Numeric U8 property-write controls consumed on Execute</td>
      <td><code>main.frog</code> + <code>ui/array_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>41_numeric_placement_ide_view</code></td>
      <td><time datetime="2026-06-08">2026-06-08</time></td>
      <td>IDE-view Numeric placement witness showing the 16px grid, placement aura, label aura, and right/left/hidden increment-decrement postures</td>
      <td><code>main.frog</code> + <code>ui/numeric_ide_panel.wfrog</code></td>
      <td>C++ browser-host calibration surface only: Example 05 remains the executable Numeric accumulator slice</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Progression Logic</h2>

<pre><code>01  pure computation
    |
    v
02  widget_value natural UI value path
    |
    v
03  widget_reference + frog.ui.property_write object-style UI path
    |
    v
04  explicit state and valid feedback through delay
    |
    v
05  combined bounded UI accumulator corridor
    |
    v
06  Boolean .wfrog realization and native-kernel runtime bridge
    |
    v
07  String .wfrog realization and native-kernel runtime bridge
    |
    v
08  Enum .wfrog realization and native-kernel runtime bridge
    |
    v
09  Path .wfrog realization and native-kernel runtime bridge
    |
    v
10-15  Button mechanical actions in the public reference runtime closure
    |
    v
16   Path -> frog.image.decode_file_rgba8 -> Picture as a post-boundary widget progression example
    |
    v
17   standalone Label support text as a post-boundary widget progression example
    |
    v
18   standalone Decorations as a post-boundary widget progression example
    |
    v
19   Subpanel embedding another .frog front panel as a post-boundary widget progression example
    |
    v
20   Tab selected-page value and page region as a post-boundary widget progression example
    |
    v
21   Ring numeric selected-value as a post-boundary widget progression example
    |
    v
22   Listbox numeric selected-value as a post-boundary widget progression example
    |
    v
23   Table numeric selected-cell value as a post-boundary widget progression example
    |
    v
24   Tree numeric selected-node value as a post-boundary widget progression example
    |
    v
25   standalone Frame support grouping surface as a post-boundary widget progression example
    |
    v
26   deprecated historical 2D rigid Array numeric selected-element milestone
    |
    v
27   deprecated historical 1D vertical rigid Array numeric selected-element milestone
    |
    v
28   deprecated historical 3D rigid Array numeric selected-element milestone
    |
    v
29   1D Array container with Default Numeric element widgets as a post-boundary widget progression example
    |
    v
30   1D horizontal Array container with Default Numeric element widgets as a post-boundary widget progression example
    |
    v
31   1D vertical Array visible-count property write consumed on Execute
    |
    v
32   1D horizontal Array visible-count property write consumed on Execute
    |
    v
33   2D Array container with Default Numeric element widgets as a post-boundary widget progression example
    |
    v
34   2D Array visible row/column property writes consumed on Execute
    |
    v
35+  future examples continue runtime implementation in Graiphic/FROG-Runtime unless explicitly promoted
</code></pre>

<hr/>

<h2>Directory Shape</h2>

<pre><code>Examples/
├── 01_pure_addition/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   └── main.lowering.json
├── 02_ui_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   └── main.lowering.json
├── 03_ui_property_write/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   └── main.lowering.json
├── 04_stateful_feedback_delay/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   └── main.lowering.json
├── 05_bounded_ui_accumulator/
│   ├── Readme.md
│   ├── Freeze.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── accumulator_panel.wfrog
├── 06_boolean_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── boolean_panel.wfrog
├── 07_string_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── string_panel.wfrog
├── 08_enum_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── enum_panel.wfrog
├── 09_path_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── path_panel.wfrog
├── 10_button_press_to_boolean/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── button_panel.wfrog
├── 11_button_switch_when_pressed/
│   └── same Button example shape
├── 12_button_switch_when_released/
│   └── same Button example shape
├── 13_button_latch_when_pressed/
│   └── same Button example shape
├── 14_button_latch_when_released/
│   └── same Button example shape
├── 15_button_latch_until_released/
│   └── same Button example shape
├── 16_picture_logo_jpeg/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    ├── assets/
    │   └── frog_logo.jpg
    └── ui/
        └── picture_panel.wfrog
├── 17_label_typography_board/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── label_panel.wfrog
├── 18_decoration_board/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── decoration_panel.wfrog
├── 19_subpanel_embedded_front_panel/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    ├── embedded/
    │   ├── embedded_status.frog
    │   └── ui/
    │       └── embedded_status_panel.wfrog
    └── ui/
        └── subpanel_panel.wfrog
├── 20_tab_page_selection/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── tab_panel.wfrog
├── 21_ring_selection_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── ring_panel.wfrog
├── 22_listbox_selection_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── listbox_panel.wfrog
├── 23_table_cell_selection_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── table_panel.wfrog
├── 24_tree_node_selection_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── tree_panel.wfrog
└── 25_frame_grouping_board/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── frame_panel.wfrog
└── 26_array_numeric_selection_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
└── 27_array_1d_vertical_numeric_selection_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
└── 28_array_3d_numeric_selection_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
└── 29_array_1d_numeric_container_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
└── 30_array_1d_horizontal_numeric_container_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
└── 31_array_1d_visible_count_property_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
└── 32_array_1d_horizontal_visible_count_property_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
└── 33_array_2d_numeric_container_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
└── 34_array_2d_visible_counts_property_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── array_panel.wfrog
</code></pre>

<p>
Examples <code>10</code> through <code>15</code> use the same published shape:
each Button mechanical-action slice contains <code>Readme.md</code>,
<code>main.frog</code>, FIR/lowering artifacts, and
<code>ui/button_panel.wfrog</code> referencing the Default Button and Boolean
realization assets.
</p>

<p>
Example <code>16</code> starts the repository-visible post-boundary widget
progression. It contains public source, FIR/lowering shape, a
<code>.wfrog</code> package referencing the Default Path and Default Picture
realizations, and a default FROG logo image path decoded through the
<code>frog.image.decode_file_rgba8</code> standard library contract. It does not
add public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>17</code> continues the repository-visible post-boundary widget
progression with standalone Label support text. It contains public source,
FIR/lowering shape, a <code>.wfrog</code> package referencing the Default Label
realization, and a small native manifest proof output. It does not add public
runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>18</code> continues the repository-visible post-boundary widget
progression with standalone Decoration support widgets. It contains public
source, FIR/lowering shape, a <code>.wfrog</code> package referencing the Default
Decorations realization, and a small native manifest proof output.
It does not add public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>19</code> continues the repository-visible post-boundary widget
progression with a Subpanel host region. It contains a parent public source,
a referenced child <code>.frog</code> front panel, FIR/lowering shape, a
<code>.wfrog</code> package referencing the Default Subpanel realization, and a
small native manifest proof output. The parent <code>.frog</code> owns the
Subpanel instance and child reference; the child <code>.frog</code> owns the
widgets displayed inside the Subpanel. It does not add public runtime source
beyond the Example 15 closure.
</p>

<p>
Example <code>20</code> continues the repository-visible post-boundary widget
progression with a Tab control. It contains public source, FIR/lowering shape,
a <code>.wfrog</code> package referencing the Default Tab realization, and a
native manifest string proof output. The <code>.frog</code>
owns page records, selected page value, layout, style, and visible page
content. Its host capability declarations include selected-page state patching
as a realization-facing browser interaction. It does not add public runtime
source beyond the Example 15 closure.
</p>

<p>
Example <code>21</code> continues the repository-visible post-boundary widget
progression with a Ring control and read-only Ring indicator. It contains
public source, FIR/lowering shape, a <code>.wfrog</code> package referencing the
Default Ring realization, and a native manifest <code>u16</code> proof output.
The <code>.frog</code> owns item ids, labels, numeric values, selected item state,
layout, and style overrides. It does not add public runtime source beyond the
Example 15 closure.
</p>

<p>
Example <code>22</code> continues the repository-visible post-boundary widget
progression with a Listbox control and read-only Listbox indicator. It contains
public source, FIR/lowering shape, a <code>.wfrog</code> package referencing the
Default Listbox realization, and a native manifest <code>u16</code> proof output.
The <code>.frog</code> owns item ids, labels, numeric values, selected item state,
viewport, scrollbar posture, layout, and style overrides. It does not add
public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>23</code> continues the repository-visible post-boundary widget
progression with a Table control and read-only Table indicator. It contains
public source, FIR/lowering shape, a <code>.wfrog</code> package referencing the
Default Table realization, and a native manifest <code>u16</code> proof output.
The <code>.frog</code> owns the table schema, rows, cells, selected cell state,
editable-cell posture, horizontal and vertical scrollbar posture, viewport,
layout, and style overrides. It does not add public runtime source beyond the
Example 15 closure.
</p>

<p>
Example <code>24</code> continues the repository-visible post-boundary widget
progression with a Tree control and read-only Tree indicator. It contains
public source, FIR/lowering shape, a <code>.wfrog</code> package referencing the
Default Tree realization, and a native manifest <code>u16</code> proof output.
The <code>.frog</code> owns tree records, hierarchy, item symbols, selected cell
state, viewport, layout, and style overrides. It does not add public runtime
source beyond the Example 15 closure.
</p>

<p>
Example <code>25</code> continues the repository-visible post-boundary widget
progression with standalone simple Frame support rectangles. It contains public source,
FIR/lowering shape, a <code>.wfrog</code> package referencing the Default Frame
realization, and a native manifest bool proof output. The <code>.frog</code>
owns Frame instance layout, background visibility/color, border visibility/color,
and one uniform border thickness value per instance. Frame does not own labels,
captions, methods, scrollbars, child <code>.frog</code> references, or value flow. It
does not add public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>26</code> continues the repository-visible post-boundary widget
progression with a 2D Array control and read-only Array indicator. It contains
public source, FIR/lowering shape, a <code>.wfrog</code> package referencing the
Default Array realization, and a native manifest <code>u16</code> proof output.
The <code>.frog</code> owns Array rank, shape, element records, selected element,
index display values, viewport, scrollbars, layout, and style overrides. It does
not add public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>27</code> continues the Array progression with a 1D vertical Array
control and read-only Array indicator. It contains public source,
FIR/lowering shape, a <code>.wfrog</code> package referencing the same Default
Array realization, and a native manifest <code>u16</code> proof output. The
<code>.frog</code> owns rank <code>1</code>, vector length, element records,
one index display, selected element, vertical viewport, scrollbar posture,
layout, and style overrides. It does not add public runtime source beyond the
Example 15 closure.
</p>

<p>
Example <code>28</code> continues the Array progression with a 3D numeric Array
control and read-only Array indicator. It contains public source,
FIR/lowering shape, a <code>.wfrog</code> package referencing the same Default
Array realization, and a native manifest <code>u16</code> proof output. The
<code>.frog</code> owns rank <code>3</code>, semantic shape, layer/row/column
element records, three index displays, selected element, viewport posture,
layout, and style overrides. The visible grid displays one 2D layer at a time;
editing outside the current numeric volume materializes the rectangular
<code>[layer,row,column]</code> volume with new cells initialized to <code>0</code>.
It does not add public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>29</code> continues the Array progression by separating the Array
container from its contained element widget. It contains a rank-1 Array control
whose visible cells are source-owned Default Numeric widget instances, plus a
read-only Array indicator using the Default Numeric indicator template. The
<code>.frog</code> owns the Array rank, vector length, selected element, viewport,
materialization posture, element default value, contained Numeric class/template
binding, and instance-level style overrides. The <code>.wfrog</code> package resolves
both Default Array and Default Numeric realization assets. It does not add
public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>30</code> is the horizontal counterpart to Example <code>29</code>.
It contains a rank-1 horizontal Array control whose visible cells are
source-owned Default Numeric widget instances, plus a read-only horizontal
Array indicator using the Default Numeric indicator template. The
<code>.frog</code> owns orientation, viewport state, horizontal scrollbar
posture, contained Numeric element properties, and the declared native
selected-value flow. It does not add public runtime source beyond the
Example 15 closure.
</p>

<p>
Example <code>31</code> adds an execution-mode property-write slice for the
vertical Array container. A Numeric <code>U8</code> control publishes the
requested visible cell count. The diagram consumes that value on
<code>Execute</code> and writes <code>viewport.visible_counts[0]</code> on both
the command Array and read-only indicator. Direct Numeric edits remain
front-panel state until the explicit execution flow consumes them.
</p>

<p>
Example <code>32</code> applies the same execution-mode property-write posture
to the horizontal Array container. A Numeric <code>U8</code> control writes
<code>viewport.visible_counts[1]</code> on <code>Execute</code>, so the example
validates the column-count property path without turning IDE/design-time live
resizing into runtime behavior. Python/Rust parity remains a later private
runtime task after C++ acceptance.
</p>

<p>
Example <code>33</code> extends the Array-as-container posture to rank
<code>2</code>. It contains a 2D Array control whose visible cells are
source-owned Default Numeric widget instances, plus a read-only 2D
Array/Numeric indicator. The <code>.frog</code> owns rank, shape, row/column
index displays, viewport posture, contained Numeric element binding, selection,
and instance-level visual overrides. It does not add public runtime source
beyond the Example 15 closure.
</p>

<hr/>

<h2>Minimum Local Setup</h2>

<p>
Run example validation commands from the repository root.
A fresh clone needs the following local tools before the numbered example corridor can be fully checked:
</p>

<table>
  <thead>
    <tr>
      <th>Tool</th>
      <th>Required for</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Python</code> + <code>pip</code></td>
      <td>Reference workspace checks, FIR derivation, lowering, contracts, LLVM emission checks, and Python runtime tests.</td>
    </tr>
    <tr>
      <td><code>pytest</code></td>
      <td>Full reference test surface.</td>
    </tr>
    <tr>
      <td><code>CMake</code> + a C++ toolchain</td>
      <td>C++ runtime-family validation for the current widget examples.</td>
    </tr>
    <tr>
      <td><code>Cargo</code> / Rust toolchain</td>
      <td>Rust runtime-family validation for the current widget examples.</td>
    </tr>
    <tr>
      <td><code>clang</code></td>
      <td>Optional LLVM native build and native-kernel bridge checks.</td>
    </tr>
  </tbody>
</table>

<p>
Minimal setup for the Python-backed reference checks:
</p>

<pre><code>python -m pip install pytest
python Implementations/Reference/check_reference_workspace.py --include-pytest</code></pre>

<p>
Runtime-family validation adds the current C++ and Rust surfaces:
</p>

<pre><code>cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp --output-on-failure

cargo test --manifest-path Implementations/Reference/Runtime/rust/Cargo.toml</code></pre>

<p>
Widget examples are framed by the
<a href="./widget_public_boundary.md">Widget Example Public Boundary</a>.
That document records the public source / realization / runtime-consumer
ownership split without publishing Graiphic's internal widget acceptance recipe,
manual visual QA procedure, or private runtime roadmap.
</p>

<p>
All example families are organized by the
<a href="./example_dossier_standard.md">FROG Example Dossier Standard</a>.
Use the widget boundary document as the additional public boundary only when an
example has a front-panel widget or Default realization surface.
</p>

<p>
The currently stabilized widget example surfaces are inventoried in
<a href="./widget_reference/Readme.md">Widget Example Reference Notes</a>.
Those notes document the proven Numeric, Boolean, String, Enum, Path, and Button example surfaces,
including which properties are owned by <code>.frog</code>, which realization packages and SVG assets are consumed through
<code>.wfrog</code>, and which public runtime checks support the published example evidence.
</p>

<hr/>

<h2>Reference Checks</h2>

<p>
The current reference workspace check across the numbered examples is:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py</code></pre>

<p>
To include pytest coverage and optional native proof checks where the local toolchains are available:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-pytest
python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge
python Implementations/Reference/check_reference_workspace.py --include-llvm-build</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
Examples remain subordinate to the owning specification documents.
They illustrate and verify published corridors.
They do not define FROG semantics.
</p>

