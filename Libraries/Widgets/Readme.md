<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Widgets</h1>

<p align="center">
  <strong>Repository-visible standard widget class-law index</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory defines the public class law for the standard FROG widget families.
Widget class-law documents define portable class identity, role posture, primary value posture, public properties, public methods, public events, public parts, validation expectations, and the boundary between semantic widget behavior and downstream realization choices.
</p>

<p>
The widget class-law layer is upstream from Default realizations, SVG assets, host-native renderers, runtime consumers, and IDE-specific authoring behavior.
A realization may provide templates, subobjects, slots, bindings, and host hints, but it must not redefine the widget class law.
</p>

<hr/>

<h2>Common Property Surface</h2>

<p>
Shared widget properties such as bounds, position, size, label, caption,
documentation, visibility, enabled state, focus, tabbing, binding, primary
value mirrors, display posture, viewport posture, and source-owned styling are
defined once in <a href="./CommonProperties.md"><code>CommonProperties.md</code></a>.
Family documents add the specialized value, method, event, and property
surfaces that are specific to each widget class.
</p>

<hr/>

<h2>Current Widget Baseline</h2>

<table>
  <thead>
    <tr>
      <th>Document</th>
      <th>Family</th>
      <th>Classes</th>
      <th>Category</th>
      <th>Role posture</th>
      <th>Current class-law posture</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><a href="./Boolean.md"><code>Boolean.md</code></a></td><td>Boolean</td><td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td><td>core value</td><td>control / indicator</td><td>published scalar baseline</td></tr>
    <tr><td><a href="./String.md"><code>String.md</code></a></td><td>String</td><td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td><td>core value</td><td>control / indicator</td><td>published scalar baseline</td></tr>
    <tr><td><a href="./Button.md"><code>Button.md</code></a></td><td>Button</td><td><code>frog.widgets.button</code></td><td>action</td><td>command</td><td>published action baseline</td></tr>
    <tr><td><a href="./Numeric.md"><code>Numeric.md</code></a></td><td>Numeric</td><td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td><td>core value</td><td>control / indicator</td><td>published scalar baseline</td></tr>
    <tr><td><a href="./Enum.md"><code>Enum.md</code></a></td><td>Enum</td><td><code>frog.widgets.enum_control</code>, <code>frog.widgets.enum_indicator</code></td><td>core value</td><td>control / indicator</td><td>published scalar baseline</td></tr>
    <tr><td><a href="./Ring.md"><code>Ring.md</code></a></td><td>Ring</td><td><code>frog.widgets.ring_control</code>, <code>frog.widgets.ring_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>published compact finite-choice baseline</td></tr>
    <tr><td><a href="./Path.md"><code>Path.md</code></a></td><td>Path</td><td><code>frog.widgets.path_control</code>, <code>frog.widgets.path_indicator</code></td><td>core value</td><td>control / indicator</td><td>published scalar baseline</td></tr>
    <tr><td><a href="./Picture.md"><code>Picture.md</code></a></td><td>Picture</td><td><code>frog.widgets.picture_control</code>, <code>frog.widgets.picture_indicator</code></td><td>visual data</td><td>control / indicator</td><td>published image / viewport baseline</td></tr>
    <tr><td><a href="./Chart.md"><code>Chart.md</code></a></td><td>Waveform Chart</td><td><code>frog.widgets.waveform_chart</code></td><td>visual data</td><td>indicator</td><td>published composite chart baseline</td></tr>
    <tr><td><a href="./Listbox.md"><code>Listbox.md</code></a></td><td>Listbox</td><td><code>frog.widgets.listbox_control</code>, <code>frog.widgets.listbox_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>published item-list baseline</td></tr>
    <tr><td><a href="./Tab.md"><code>Tab.md</code></a></td><td>Tab</td><td><code>frog.widgets.tab_control</code>, <code>frog.widgets.tab_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>published page-navigation baseline</td></tr>
    <tr><td><a href="./Tree.md"><code>Tree.md</code></a></td><td>Tree</td><td><code>frog.widgets.tree_control</code>, <code>frog.widgets.tree_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>published hierarchical-data baseline</td></tr>
    <tr><td><a href="./Table.md"><code>Table.md</code></a></td><td>Table</td><td><code>frog.widgets.table_control</code>, <code>frog.widgets.table_indicator</code></td><td>structured data</td><td>control / indicator</td><td>published tabular-data baseline</td></tr>
    <tr><td><a href="./Array.md"><code>Array.md</code></a></td><td>Array</td><td><code>frog.widgets.array</code>, <code>frog.widgets.array_control</code>, <code>frog.widgets.array_indicator</code></td><td>structured data</td><td>structured collection</td><td>published homogeneous collection baseline</td></tr>
    <tr><td><a href="./Cluster.md"><code>Cluster.md</code></a></td><td>Cluster</td><td><code>frog.widgets.cluster</code></td><td>structured data</td><td>structured aggregate</td><td>published heterogeneous aggregate baseline</td></tr>
    <tr><td><a href="./Label.md"><code>Label.md</code></a></td><td>Label</td><td><code>frog.widgets.label</code></td><td>support</td><td>support</td><td>published support baseline</td></tr>
    <tr><td><a href="./Frame.md"><code>Frame.md</code></a></td><td>Frame</td><td><code>frog.widgets.frame</code></td><td>support</td><td>support</td><td>published support baseline</td></tr>
    <tr><td><a href="./Decorations.md"><code>Decorations.md</code></a></td><td>Decorations</td><td><code>frog.widgets.flat_box</code>, <code>frog.widgets.horizontal_line</code>, <code>frog.widgets.vertical_line</code></td><td>support</td><td>decorative support</td><td>published intentionally non-value baseline</td></tr>
    <tr><td><a href="./Splitter.md"><code>Splitter.md</code></a></td><td>Splitter</td><td><code>frog.widgets.horizontal_splitter</code>, <code>frog.widgets.vertical_splitter</code></td><td>layout</td><td>layout</td><td>published layout baseline</td></tr>
    <tr><td><a href="./Subpanel.md"><code>Subpanel.md</code></a></td><td>Subpanel</td><td><code>frog.widgets.subpanel</code></td><td>layout / embedded front panel</td><td>subpanel host</td><td>published embedded-front-panel baseline</td></tr>
  </tbody>
</table>

<hr/>

<h2>Deepened Complex Widget Families</h2>

<p>
The current published widget layer includes detailed class-law baselines for the principal complex front-panel families:
</p>

<ul>
  <li><strong>Picture</strong> — image payload, pixel metadata, viewport, display posture, overlay and pointer surfaces.</li>
  <li><strong>Ring</strong> — compact finite-choice selection with a selected item id/value and popup selector surface.</li>
  <li><strong>Waveform Chart</strong> — retained sample history, plots, scales, legends, cursors, palette, scrollbars, overlays, and plot-image layers.</li>
  <li><strong>Listbox</strong> — item identity, selection, active item, hover, viewport, search/filter posture, and item public parts.</li>
  <li><strong>Tab</strong> — page identity, selected-page value, header posture, page region, overflow, close/reorder posture, and page composition surfaces.</li>
  <li><strong>Tree</strong> — finite hierarchy, node identity, selection, expansion, check state, viewport, editing, drag/drop posture, and node public parts.</li>
  <li><strong>Table</strong> — flat tabular value, column schema, row identity, cell access, selection, viewport, editing, sorting, filtering, and grid public parts.</li>
</ul>

<hr/>

<h2>Architecture Rule</h2>

<ul>
  <li>Widget documents define public widget class law.</li>
  <li>Default realization documents and manifests define default embodiment.</li>
  <li>SVG assets are templates or subobject resources, not semantic owners.</li>
  <li>Runtime implementations consume these surfaces; they do not define the standard.</li>
  <li>Host-native widgets may replace SVG-backed rendering when they preserve public parts, value posture, property/method/event surfaces, and realization-private boundaries.</li>
</ul>

<hr/>

<h2>Interaction Rule</h2>

<p>
Standard widgets may participate in diagrams through natural value access and object-style interaction surfaces.
Ordinary dataflow should use <code>widget_value</code> for the widget's primary value.
Object-style access through <code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, method invocation, and event observation should be used for interaction surfaces such as viewport, selection, editing, focus, page management, cursor posture, history management, and host-facing display posture.
The shared property vocabulary is documented in <a href="./CommonProperties.md"><code>CommonProperties.md</code></a>.
</p>

<hr/>

<h2>Collection and Structure Rule</h2>

<ul>
  <li><code>frog.widgets.array</code> is the homogeneous collection class; <code>frog.widgets.array_control</code> and <code>frog.widgets.array_indicator</code> are the interactive and read-only front-panel forms.</li>
  <li><code>frog.widgets.cluster</code> is the heterogeneous named aggregate widget.</li>
  <li><code>frog.widgets.table_control</code> and <code>frog.widgets.table_indicator</code> are visible flat tabular widgets.</li>
  <li>Tree, Tab, Listbox, Table, Picture, Chart, and Subpanel are not generic host containers for arbitrary private state; their public surfaces are bounded by their class-law documents.</li>
</ul>

<hr/>

<h2>Default Realization Relationship</h2>

<p>
The corresponding official Default realization family is published under:
</p>

<pre><code>Libraries/Realizations/Default/</code></pre>

<p>
Each standard widget family should have a matching Default realization document and a machine-readable <code>*.default.wfrog</code> manifest when the Default family publishes an embodiment for it.
The reference WidgetValidator protects this repository-level alignment, but the validator remains non-normative.
</p>
