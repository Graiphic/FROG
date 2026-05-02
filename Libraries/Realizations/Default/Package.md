<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Package</h1>

<p align="center">
  <strong>Machine-readable publication posture for the official <code>Default</code> widget realization family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Realizations index</a></li>
  <li><a href="../../Widgets/Readme.md">Standard widgets</a></li>
  <li><a href="./Readme.md">Default realization family overview</a></li>
  <li><a href="./Validation.md">Default realization validation checklist</a></li>
</ul>

<hr/>

<h2>Active Default Manifests</h2>

<table>
  <thead>
    <tr><th>Family</th><th>Manifest</th><th>Target classes</th><th>Category</th><th>Runtime role posture</th></tr>
  </thead>
  <tbody>
    <tr><td><a href="./Boolean.md">Boolean</a></td><td><code>boolean.default.wfrog</code></td><td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td><td>core value</td><td>control / indicator</td></tr>
    <tr><td><a href="./String.md">String</a></td><td><code>string.default.wfrog</code></td><td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td><td>core value</td><td>control / indicator</td></tr>
    <tr><td><a href="./Button.md">Button</a></td><td><code>button.default.wfrog</code></td><td><code>frog.widgets.button</code></td><td>action</td><td>command</td></tr>
    <tr><td><a href="./Numeric.md">Numeric</a></td><td><code>numeric.default.wfrog</code></td><td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td><td>core value</td><td>control / indicator</td></tr>
    <tr><td><a href="./Enum.md">Enum</a></td><td><code>enum.default.wfrog</code></td><td><code>frog.widgets.enum_control</code>, <code>frog.widgets.enum_indicator</code></td><td>core value</td><td>control / indicator</td></tr>
    <tr><td><a href="./Path.md">Path</a></td><td><code>path.default.wfrog</code></td><td><code>frog.widgets.path_control</code>, <code>frog.widgets.path_indicator</code></td><td>core value</td><td>control / indicator</td></tr>
    <tr><td><a href="./Picture.md">Picture</a></td><td><code>picture.default.wfrog</code></td><td><code>frog.widgets.picture_control</code>, <code>frog.widgets.picture_indicator</code></td><td>visual data</td><td>control / indicator</td></tr>
    <tr><td><a href="./Chart.md">Waveform Chart</a></td><td><code>chart.default.wfrog</code></td><td><code>frog.widgets.waveform_chart</code></td><td>visual data</td><td>indicator</td></tr>
    <tr><td><a href="./Listbox.md">Listbox</a></td><td><code>listbox.default.wfrog</code></td><td><code>frog.widgets.listbox_control</code>, <code>frog.widgets.listbox_indicator</code></td><td>selection / navigation</td><td>control / indicator</td></tr>
    <tr><td><a href="./Tab.md">Tab</a></td><td><code>tab.default.wfrog</code></td><td><code>frog.widgets.tab_control</code>, <code>frog.widgets.tab_indicator</code></td><td>selection / navigation</td><td>control / indicator</td></tr>
    <tr><td><a href="./Tree.md">Tree</a></td><td><code>tree.default.wfrog</code></td><td><code>frog.widgets.tree_control</code>, <code>frog.widgets.tree_indicator</code></td><td>selection / navigation</td><td>control / indicator</td></tr>
    <tr><td><a href="./Table.md">Table</a></td><td><code>table.default.wfrog</code></td><td><code>frog.widgets.table_control</code>, <code>frog.widgets.table_indicator</code></td><td>structured data</td><td>control / indicator</td></tr>
    <tr><td><a href="./Array.md">Array</a></td><td><code>array.default.wfrog</code></td><td><code>frog.widgets.array</code></td><td>structured data</td><td>structured collection</td></tr>
    <tr><td><a href="./Cluster.md">Cluster</a></td><td><code>cluster.default.wfrog</code></td><td><code>frog.widgets.cluster</code></td><td>structured data</td><td>structured aggregate</td></tr>
    <tr><td><a href="./Label.md">Label</a></td><td><code>label.default.wfrog</code></td><td><code>frog.widgets.label</code></td><td>support</td><td>support</td></tr>
    <tr><td><a href="./Frame.md">Frame</a></td><td><code>frame.default.wfrog</code></td><td><code>frog.widgets.frame</code></td><td>support</td><td>support</td></tr>
    <tr><td><a href="./Decorations.md">Decorations</a></td><td><code>decorations.default.wfrog</code></td><td><code>frog.widgets.flat_box</code>, <code>frog.widgets.horizontal_line</code>, <code>frog.widgets.vertical_line</code></td><td>support</td><td>decorative support</td></tr>
    <tr><td><a href="./Splitter.md">Splitter</a></td><td><code>splitter.default.wfrog</code></td><td><code>frog.widgets.horizontal_splitter</code>, <code>frog.widgets.vertical_splitter</code></td><td>layout</td><td>layout</td></tr>
    <tr><td><a href="./Panel.md">Panel/SubPanel</a></td><td><code>panel.default.wfrog</code></td><td><code>frog.widgets.panel</code>, <code>frog.widgets.subpanel</code></td><td>layout / container</td><td>container</td></tr>
  </tbody>
</table>

<hr/>

<h2>Ownership Boundary</h2>

<pre><code>Libraries/Widgets/
    - public widget class law

Libraries/Realizations/Default/
    - default realization posture

*.default.wfrog
    - machine-readable realization manifests

assets/
    - SVG templates and subobject resources

runtime implementation
    - interpretation of the published package
</code></pre>

<p>
Default manifests publish realization resources, part bindings, state maps, property bindings, anchors, host hints, and validation expectations.
They do not redefine public widget class law.
</p>
