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
    <tr><th>Document</th><th>Family</th><th>Classes</th><th>Role</th></tr>
  </thead>
  <tbody>
    <tr><td><a href="./Boolean.md"><code>Boolean.md</code></a></td><td>Boolean</td><td><code>frog.widgets.boolean_control / frog.widgets.boolean_indicator</code></td><td>True/false control and display</td></tr>
    <tr><td><a href="./String.md"><code>String.md</code></a></td><td>String</td><td><code>frog.widgets.string_control / frog.widgets.string_indicator</code></td><td>Text control and display</td></tr>
    <tr><td><a href="./Button.md"><code>Button.md</code></a></td><td>Button</td><td><code>frog.widgets.button</code></td><td>Command and mechanical-action button</td></tr>
    <tr><td><a href="./Numeric.md"><code>Numeric.md</code></a></td><td>Numeric</td><td><code>frog.widgets.numeric_control / frog.widgets.numeric_indicator</code></td><td>Numeric control and display</td></tr>
    <tr><td><a href="./Enum.md"><code>Enum.md</code></a></td><td>Enum</td><td><code>frog.widgets.enum_control / frog.widgets.enum_indicator</code></td><td>Finite named-value selection</td></tr>
    <tr><td><a href="./Path.md"><code>Path.md</code></a></td><td>Path</td><td><code>frog.widgets.path_control / frog.widgets.path_indicator</code></td><td>File/folder path value</td></tr>
    <tr><td><a href="./Picture.md"><code>Picture.md</code></a></td><td>Picture</td><td><code>frog.widgets.picture_control / frog.widgets.picture_indicator</code></td><td>Image/RGB/drawable display</td></tr>
    <tr><td><a href="./Chart.md"><code>Chart.md</code></a></td><td>Waveform Chart</td><td><code>frog.widgets.waveform_chart</code></td><td>History-oriented plot display</td></tr>
    <tr><td><a href="./Listbox.md"><code>Listbox.md</code></a></td><td>Listbox</td><td><code>frog.widgets.listbox_control / frog.widgets.listbox_indicator</code></td><td>Persistent visible item selection</td></tr>
    <tr><td><a href="./Table.md"><code>Table.md</code></a></td><td>Table</td><td><code>frog.widgets.table_control / frog.widgets.table_indicator</code></td><td>Flat tabular data</td></tr>
    <tr><td><a href="./Tree.md"><code>Tree.md</code></a></td><td>Tree</td><td><code>frog.widgets.tree_control / frog.widgets.tree_indicator</code></td><td>Hierarchical node data</td></tr>
    <tr><td><a href="./Label.md"><code>Label.md</code></a></td><td>Label</td><td><code>frog.widgets.label</code></td><td>Standalone support text</td></tr>
    <tr><td><a href="./Frame.md"><code>Frame.md</code></a></td><td>Frame</td><td><code>frog.widgets.frame</code></td><td>Grouping frame</td></tr>
    <tr><td><a href="./Decorations.md"><code>Decorations.md</code></a></td><td>Decorations</td><td><code>flat_box / horizontal_line / vertical_line</code></td><td>Visual decorations</td></tr>
    <tr><td><a href="./Splitter.md"><code>Splitter.md</code></a></td><td>Splitter</td><td><code>horizontal_splitter / vertical_splitter</code></td><td>Resizable layout separator</td></tr>
    <tr><td><a href="./Panel.md"><code>Panel.md</code></a></td><td>Panel/SubPanel</td><td><code>panel / subpanel</code></td><td>Composition regions</td></tr>
    <tr><td><a href="./Array.md"><code>Array.md</code></a></td><td>Array</td><td><code>frog.widgets.array</code></td><td>Homogeneous collection</td></tr>
    <tr><td><a href="./Cluster.md"><code>Cluster.md</code></a></td><td>Cluster</td><td><code>frog.widgets.cluster</code></td><td>Heterogeneous named aggregate</td></tr>
  </tbody>
</table>

<hr/>

<h2>Architecture Rule</h2>

<ul>
  <li>Widget documents define public class law.</li>
  <li>Default realization documents and manifests define default embodiment.</li>
  <li>SVG assets are templates or subobject resources, not semantic owners.</li>
  <li>Runtime implementations consume these surfaces; they do not define the standard.</li>
</ul>
