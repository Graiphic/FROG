<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Array Widget Example Reference</h1>

<p align="center">
  <strong>Current Array selection and widget-container proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The stabilized Array progression is currently demonstrated by
<code>Examples/26_array_numeric_selection_roundtrip</code> through
<code>Examples/30_array_1d_horizontal_numeric_container_roundtrip</code>.
These examples prove the Array as a source-owned collection widget, not as a
hardcoded numeric grid.
</p>

<p>
The public class law remains <code>Libraries/Widgets/Array.md</code>, and the
Default realization surface remains
<code>Libraries/Realizations/Default/Array.md</code>. This page records the
practical example evidence and the boundaries that must stay true while the
Array examples evolve.
</p>

<pre><code>array.value
  -&gt; selected element / selected index posture
  -&gt; native u16 proof kernel
  -&gt; array indicator value
  -&gt; public selected element output

array element template
  -&gt; Default Numeric widget realization
  -&gt; repeated widget-backed cells inside the Array viewport</code></pre>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<p>
The <code>.frog</code> source owns the diagram and all Array front-panel
instance data:
</p>

<ul>
  <li>Array rank: one-dimensional, two-dimensional, or three-dimensional.</li>
  <li>Shape, length, and regular materialized dimensions.</li>
  <li>Initial element values and source-owned default value for materialized numeric cells.</li>
  <li>Selected element id, selected indices, and optional selected value output.</li>
  <li>Viewport first indices, visible counts, orientation, and scrollbar posture.</li>
  <li>Index display visibility, values, position, and style.</li>
  <li>Element template binding, including <code>frog.widgets.numeric_control</code> or <code>frog.widgets.numeric_indicator</code> for the current numeric examples.</li>
  <li>Contained widget properties such as numeric representation, increment/decrement visibility, read-only posture, and default value.</li>
  <li>Labels, caption placement, layout, and instance-level visual overrides.</li>
</ul>

<p>
When numeric values are edited outside the current shape, the Array materializes
the regular collection shape needed to contain that value. New numeric cells are
initialized to <code>0</code>, matching the public Array class law.
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
      <td>Array frame</td>
      <td><code>style.frame.*</code>, <code>style.frame.padding</code></td>
      <td>Draw one uniform outer Array frame from source-owned style data.</td>
    </tr>
    <tr>
      <td>Index display</td>
      <td><code>index_display.*</code>, <code>style.index_display.*</code></td>
      <td>Render the index display as a neighboring Array subcontainer, not as part of the element viewport.</td>
    </tr>
    <tr>
      <td>Element region</td>
      <td><code>viewport.*</code>, <code>layout.cell_width</code>, <code>layout.cell_height</code></td>
      <td>Place visible cells from the Array viewport model and keep overlays aligned to the Default SVG parts.</td>
    </tr>
    <tr>
      <td>Contained Numeric widget</td>
      <td><code>element.class_id</code>, <code>element.template_ref</code>, <code>element.props.*</code></td>
      <td>Compose the Default Numeric widget inside each visible cell without replacing it with a runtime-local surrogate.</td>
    </tr>
    <tr>
      <td>Scrollbars</td>
      <td><code>scrollbars.*</code>, <code>viewport.*</code>, <code>style.scrollbar.*</code></td>
      <td>Keep scrollbar tracks, thumbs, visibility, colors, gaps, and orientation configurable.</td>
    </tr>
    <tr>
      <td>Selection</td>
      <td><code>viewport.selection_start[]</code>, <code>viewport.selection_size[]</code>, <code>style.selection.*</code></td>
      <td>Selection must not change layout dimensions or create extra per-cell shells.</td>
    </tr>
  </tbody>
</table>

<p>
The Default Array SVG publishes the Array shell and public composition parts.
The Default Numeric SVG publishes the value face, text value, increment up,
increment down, control body, and indicator body parts used by the contained
Numeric elements.
</p>

<hr/>

<h2>Widget-Backed Array Rule</h2>

<p>
The Array container may repeat another widget class as its element template.
For the current examples, the contained widget is Numeric. The contained widget
keeps its own realization, behavior, and property surface.
</p>

<p>
The Array may scroll, clip, repeat, materialize, and position contained
widgets. It must not redraw them as a simplified HTML table, and it must not
insert fake separator lines around cells when neighboring contained widgets are
already sharing their published bounds.
</p>

<p>
When <code>element.layout.padding</code> and <code>element_gap.size</code> are
zero, repeated contained widgets may touch or visually superpose adjacent
borders. This is the expected LabVIEW-like posture for the current Numeric
Array container examples.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<p>
The Array examples use package files such as:
</p>

<pre><code>Examples/26_array_numeric_selection_roundtrip/ui/array_panel.wfrog
Examples/27_array_1d_vertical_numeric_selection_roundtrip/ui/array_panel.wfrog
Examples/28_array_3d_numeric_selection_roundtrip/ui/array_panel.wfrog
Examples/29_array_1d_numeric_container_roundtrip/ui/array_panel.wfrog
Examples/30_array_1d_horizontal_numeric_container_roundtrip/ui/array_panel.wfrog</code></pre>

<p>
The <code>.wfrog</code> package resolves Default realization assets and host
capability declarations. It does not own Array values, shape, layout, selected
indices, or diagram behavior.
</p>

<ul>
  <li>Default Array package: <code>Libraries/Realizations/Default/array.default.wfrog</code></li>
  <li>Default Array shell: <code>Libraries/Realizations/Default/assets/array/templates/array_shell.svg</code></li>
  <li>Default Numeric package for widget-backed cells: <code>Libraries/Realizations/Default/numeric.default.wfrog</code></li>
  <li>Default Numeric SVG: <code>Libraries/Realizations/Default/assets/numeric/templates/numeric_rectangular.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The runtime consumes <code>.frog</code> Array instance data, <code>.wfrog</code> realization references, Default SVG assets, FIR/lowering artifacts, and the native manifest where the example is native-backed.</li>
  <li>The runtime may provide host overlays for element editing, index displays, and scrollbars, but those overlays must align to published parts and source-owned geometry.</li>
  <li>Editing a front-panel element changes command-side front-panel state. It does not execute the diagram until the example's explicit <code>Execute</code> action.</li>
  <li>Control Arrays may edit values and materialize new numeric cells when resizing is legal.</li>
  <li>Indicator Arrays remain read-only for value edits but may allow viewport inspection.</li>
  <li>No accepted Array example may rely on an HTML/CSS fallback widget or a duplicated example-local SVG skin.</li>
</ul>

<hr/>

<h2>Native Kernel Posture</h2>

<p>
The current Array examples are post-public-runtime-boundary widget progression
examples. They contain public source, FIR, lowering, Default realization
references, and native manifest proof material. Runtime implementation for
these slices continues in <code>Graiphic/FROG-Runtime</code> unless explicitly
promoted later.
</p>

<ul>
  <li>Example 26: 2D numeric selected-element proof.</li>
  <li>Example 27: 1D vertical numeric selected-element proof.</li>
  <li>Example 28: 3D numeric selected-element proof.</li>
  <li>Example 29: 1D vertical Array container with Default Numeric element widgets.</li>
  <li>Example 30: 1D horizontal Array container with Default Numeric element widgets.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
Example 30 currently publishes an accepted reference snapshot:
</p>

<pre><code>Examples/30_array_1d_horizontal_numeric_container_roundtrip/reference/</code></pre>

<p>
That directory records the accepted screenshot, public runtime snapshot,
visual contract, machine-readable visual contract, and artifact hash index.
Additional Array examples may publish equivalent <code>reference/</code>
directories after visual acceptance.
</p>

<hr/>

<h2>Validation Notes</h2>

<ul>
  <li>Index displays and Array element containers must remain visually distinct subcontainers.</li>
  <li>Array outer frame thickness must remain uniform after resize.</li>
  <li>Repeated contained widgets must preserve the contained widget's own validated default realization.</li>
  <li>Array selection must not resize cells or alter the contained widget border geometry.</li>
  <li>Scrollbar tracks and thumbs must stay configurable and aligned to the Array viewport.</li>
  <li>Control and indicator surfaces may differ in interactivity, but their geometry law must remain comparable.</li>
  <li>Browser-host screenshots are accepted evidence for the current host, not the only possible FROG host.</li>
</ul>
