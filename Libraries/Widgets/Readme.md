<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Widgets</h1>

<p align="center">
  <strong>Repository-visible standard widget family index</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Widget Baseline</h2>

<table>
  <thead>
    <tr><th>Document</th><th>Family</th><th>Classes</th><th>Category</th><th>Role posture</th><th>Status</th></tr>
  </thead>
  <tbody>
    <tr><td><a href="./Boolean.md"><code>Boolean.md</code></a></td><td>Boolean</td><td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td><a href="./String.md"><code>String.md</code></a></td><td>String</td><td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td><a href="./Button.md"><code>Button.md</code></a></td><td>Button</td><td><code>frog.widgets.button</code></td><td>action</td><td>command</td><td>published baseline</td></tr>
    <tr><td><a href="./Numeric.md"><code>Numeric.md</code></a></td><td>Numeric</td><td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td><a href="./Enum.md"><code>Enum.md</code></a></td><td>Enum</td><td><code>frog.widgets.enum_control</code>, <code>frog.widgets.enum_indicator</code></td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td><a href="./Path.md"><code>Path.md</code></a></td><td>Path</td><td><code>frog.widgets.path_control</code>, <code>frog.widgets.path_indicator</code></td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td><a href="./Picture.md"><code>Picture.md</code></a></td><td>Picture</td><td><code>frog.widgets.picture_control</code>, <code>frog.widgets.picture_indicator</code></td><td>visual data</td><td>control / indicator</td><td>published baseline, needs later qualitative deepening</td></tr>
    <tr><td><a href="./Chart.md"><code>Chart.md</code></a></td><td>Waveform Chart</td><td><code>frog.widgets.waveform_chart</code></td><td>visual data</td><td>indicator</td><td>published composite baseline</td></tr>
    <tr><td><a href="./Listbox.md"><code>Listbox.md</code></a></td><td>Listbox</td><td><code>frog.widgets.listbox_control</code>, <code>frog.widgets.listbox_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>published baseline, needs later qualitative deepening</td></tr>
    <tr><td><a href="./Tab.md"><code>Tab.md</code></a></td><td>Tab</td><td><code>frog.widgets.tab_control</code>, <code>frog.widgets.tab_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>published composite baseline, needs later qualitative deepening</td></tr>
    <tr><td><a href="./Tree.md"><code>Tree.md</code></a></td><td>Tree</td><td><code>frog.widgets.tree_control</code>, <code>frog.widgets.tree_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>published baseline, needs later qualitative deepening</td></tr>
    <tr><td><a href="./Table.md"><code>Table.md</code></a></td><td>Table</td><td><code>frog.widgets.table_control</code>, <code>frog.widgets.table_indicator</code></td><td>structured data</td><td>control / indicator</td><td>published baseline, needs later qualitative deepening</td></tr>
    <tr><td><a href="./Array.md"><code>Array.md</code></a></td><td>Array</td><td><code>frog.widgets.array</code></td><td>structured data</td><td>structured collection</td><td>published composite baseline</td></tr>
    <tr><td><a href="./Cluster.md"><code>Cluster.md</code></a></td><td>Cluster</td><td><code>frog.widgets.cluster</code></td><td>structured data</td><td>structured aggregate</td><td>published composite baseline</td></tr>
    <tr><td><a href="./Label.md"><code>Label.md</code></a></td><td>Label</td><td><code>frog.widgets.label</code></td><td>support</td><td>support</td><td>published baseline</td></tr>
    <tr><td><a href="./Frame.md"><code>Frame.md</code></a></td><td>Frame</td><td><code>frog.widgets.frame</code></td><td>support</td><td>support</td><td>published baseline</td></tr>
    <tr><td><a href="./Decorations.md"><code>Decorations.md</code></a></td><td>Decorations</td><td><code>frog.widgets.flat_box</code>, <code>frog.widgets.horizontal_line</code>, <code>frog.widgets.vertical_line</code></td><td>support</td><td>decorative support</td><td>published baseline, intentionally non-value</td></tr>
    <tr><td><a href="./Splitter.md"><code>Splitter.md</code></a></td><td>Splitter</td><td><code>frog.widgets.horizontal_splitter</code>, <code>frog.widgets.vertical_splitter</code></td><td>layout</td><td>layout</td><td>published baseline</td></tr>
    <tr><td><a href="./Panel.md"><code>Panel.md</code></a></td><td>Panel/SubPanel</td><td><code>frog.widgets.panel</code>, <code>frog.widgets.subpanel</code></td><td>layout / container</td><td>container</td><td>published baseline</td></tr>
  </tbody>
</table>

<hr/>

<h2>Architecture Rule</h2>

<ul>
  <li>Widget documents define public widget class law.</li>
  <li>Default realization documents and manifests define default embodiment.</li>
  <li>SVG assets are templates or subobject resources, not semantic owners.</li>
  <li>Runtime implementations consume these surfaces; they do not define the standard.</li>
</ul>

<hr/>

<h2>Collection and Structure Rule</h2>

<ul>
  <li><code>frog.widgets.array</code> is the homogeneous collection widget.</li>
  <li><code>frog.widgets.cluster</code> is the heterogeneous named aggregate widget.</li>
  <li><code>frog.widgets.table_control</code> and <code>frog.widgets.table_indicator</code> are visible flat tabular widgets.</li>
</ul>
