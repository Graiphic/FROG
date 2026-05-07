<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realizations</h1>

<p align="center">
  <strong>Official default realization family for standardized FROG widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Realizations index</a></li>
  <li><a href="../../Widgets/Readme.md">Standard widgets</a></li>
  <li><a href="./Package.md">Default realization package</a></li>
  <li><a href="./Validation.md">Default realization validation checklist</a></li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This directory publishes the official <code>Default</code> realization family for the current standard FROG widget layer.
A Default realization gives a portable embodiment posture for a widget class family through documentation, machine-readable <code>*.default.wfrog</code> manifests, SVG templates, subobject resources, part bindings, property bindings, method bindings, event bindings, composition records, host hints, and validation expectations.
</p>

<p>
Default realizations are downstream from widget class law.
They do not define FROG source semantics, FIR semantics, lowering semantics, runtime semantics, or host-private rendering behavior.
They provide a repository-visible default embodiment surface that runtimes and hosts can consume, replace, or adapt while preserving the public widget contract.
</p>

<hr/>

<h2>Current Default Realization Families</h2>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Manifest</th>
      <th>Target classes</th>
      <th>Category</th>
      <th>Runtime role posture</th>
      <th>Current realization posture</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><a href="./Boolean.md">Boolean</a></td><td><code>boolean.default.wfrog</code></td><td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td><td>core value</td><td>control / indicator</td><td>scalar SVG-backed baseline</td></tr>
    <tr><td><a href="./String.md">String</a></td><td><code>string.default.wfrog</code></td><td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td><td>core value</td><td>control / indicator</td><td>scalar SVG-backed baseline</td></tr>
    <tr><td><a href="./Button.md">Button</a></td><td><code>button.default.wfrog</code></td><td><code>frog.widgets.button</code></td><td>action</td><td>command</td><td>action SVG-backed baseline</td></tr>
    <tr><td><a href="./Numeric.md">Numeric</a></td><td><code>numeric.default.wfrog</code></td><td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td><td>core value</td><td>control / indicator</td><td>scalar SVG-backed baseline</td></tr>
    <tr><td><a href="./Enum.md">Enum</a></td><td><code>enum.default.wfrog</code></td><td><code>frog.widgets.enum_control</code>, <code>frog.widgets.enum_indicator</code></td><td>core value</td><td>control / indicator</td><td>scalar SVG-backed baseline</td></tr>
    <tr><td><a href="./Path.md">Path</a></td><td><code>path.default.wfrog</code></td><td><code>frog.widgets.path_control</code>, <code>frog.widgets.path_indicator</code></td><td>core value</td><td>control / indicator</td><td>scalar SVG-backed baseline</td></tr>
    <tr><td><a href="./Picture.md">Picture</a></td><td><code>picture.default.wfrog</code></td><td><code>frog.widgets.picture_control</code>, <code>frog.widgets.picture_indicator</code></td><td>visual data</td><td>control / indicator</td><td>rectangular image-surface baseline</td></tr>
    <tr><td><a href="./Chart.md">Waveform Chart</a></td><td><code>chart.default.wfrog</code></td><td><code>frog.widgets.waveform_chart</code></td><td>visual data</td><td>indicator</td><td>composite chart realization with shell and subobjects</td></tr>
    <tr><td><a href="./Listbox.md">Listbox</a></td><td><code>listbox.default.wfrog</code></td><td><code>frog.widgets.listbox_control</code>, <code>frog.widgets.listbox_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>composite item-list realization with virtual-list host allowance</td></tr>
    <tr><td><a href="./Tab.md">Tab</a></td><td><code>tab.default.wfrog</code></td><td><code>frog.widgets.tab_control</code>, <code>frog.widgets.tab_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>composite page-navigation realization with page-host surfaces</td></tr>
    <tr><td><a href="./Tree.md">Tree</a></td><td><code>tree.default.wfrog</code></td><td><code>frog.widgets.tree_control</code>, <code>frog.widgets.tree_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>composite hierarchical realization with virtual-tree host allowance</td></tr>
    <tr><td><a href="./Table.md">Table</a></td><td><code>table.default.wfrog</code></td><td><code>frog.widgets.table_control</code>, <code>frog.widgets.table_indicator</code></td><td>structured data</td><td>control / indicator</td><td>composite tabular realization with virtual-grid host allowance</td></tr>
    <tr><td><a href="./Array.md">Array</a></td><td><code>array.default.wfrog</code></td><td><code>frog.widgets.array</code></td><td>structured data</td><td>structured collection</td><td>composite collection baseline</td></tr>
    <tr><td><a href="./Cluster.md">Cluster</a></td><td><code>cluster.default.wfrog</code></td><td><code>frog.widgets.cluster</code></td><td>structured data</td><td>structured aggregate</td><td>composite aggregate baseline</td></tr>
    <tr><td><a href="./Label.md">Label</a></td><td><code>label.default.wfrog</code></td><td><code>frog.widgets.label</code></td><td>support</td><td>support</td><td>support SVG-backed baseline</td></tr>
    <tr><td><a href="./Frame.md">Frame</a></td><td><code>frame.default.wfrog</code></td><td><code>frog.widgets.frame</code></td><td>support</td><td>support</td><td>support SVG-backed baseline</td></tr>
    <tr><td><a href="./Decorations.md">Decorations</a></td><td><code>decorations.default.wfrog</code></td><td><code>frog.widgets.flat_box</code>, <code>frog.widgets.horizontal_line</code>, <code>frog.widgets.vertical_line</code></td><td>support</td><td>decorative support</td><td>decorative non-value baseline</td></tr>
    <tr><td><a href="./Splitter.md">Splitter</a></td><td><code>splitter.default.wfrog</code></td><td><code>frog.widgets.horizontal_splitter</code>, <code>frog.widgets.vertical_splitter</code></td><td>layout</td><td>layout</td><td>layout realization baseline</td></tr>
    <tr><td><a href="./Panel.md">Panel/SubPanel</a></td><td><code>panel.default.wfrog</code></td><td><code>frog.widgets.panel</code>, <code>frog.widgets.subpanel</code></td><td>layout / container</td><td>container</td><td>container realization baseline</td></tr>
  </tbody>
</table>

<hr/>

<h2>Realization Boundary</h2>

<p>
The Default family owns realization-side publication only.
It does not define FROG language semantics, widget class law, FIR, lowering, backend contracts, or runtime-private behavior.
</p>

<p>
A conforming host may render a Default realization using SVG templates, subobject composition, native widgets, canvas surfaces, GPU-backed plot surfaces, or a mixed strategy, provided that the published widget class law and public part model remain preserved.
</p>

<pre><code>Libraries/Widgets/
  -> public widget class law

Libraries/Realizations/Default/*.md
  -> default realization explanation

Libraries/Realizations/Default/*.default.wfrog
  -> machine-readable realization manifests

Libraries/Realizations/Default/assets/
  -> SVG templates and subobject resources

Implementations/Reference/WidgetValidator/
  -> non-normative repository hygiene checks
</code></pre>

<hr/>

<h2>Simple and Composite Realizations</h2>

<ul>
  <li>Simple scalar or support widgets may use one primary SVG template.</li>
  <li>Composite widgets may use a shell plus subobject assets.</li>
  <li>Composite widgets may declare slots, nested slots, layer order, property bindings, method bindings, event bindings, host hints, and fallback behavior.</li>
  <li>Assets are templates or realization resources; they are not semantic owners of widget values.</li>
  <li>Host-native replacement is allowed only when the public class-law surface remains preserved.</li>
</ul>

<hr/>

<h2>Current Composite Default Families</h2>

<p>
The current Default layer includes deepened composite realization surfaces for:
</p>

<ul>
  <li><strong>Waveform Chart</strong> — shell, plot area, grid, scales, legends, cursor surfaces, palette, scrollbars, plot-image layers, annotations, and thresholds.</li>
  <li><strong>Listbox</strong> — shell, item region, item rows, icons, labels, selection, active item, hover, disabled face, empty state, search highlight, and scrollbars.</li>
  <li><strong>Tab</strong> — shell, header region, tab items, selected face, close / overflow / scroll affordances, page region, page container, and page content surfaces.</li>
  <li><strong>Tree</strong> — shell, node region, node rows, hierarchy surfaces, expanders, checkbox faces, node icons, labels, selection, editing, drag/drop, and scrollbars.</li>
  <li><strong>Table</strong> — shell, header regions, grid region, body region, rows, cells, active cell, selection face, edit overlay, sort indicator, resize handle, and scrollbars.</li>
</ul>

<hr/>

<h2>Validation Posture</h2>

<p>
Default realization publication is protected by the non-normative reference validator:
</p>

<pre><code>python Implementations/Reference/WidgetValidator/validate_widget_layer.py</code></pre>

<p>
The validator checks repository hygiene: class-law documents, Default documents, manifests, target classes, resource references, SVG part markers, role posture, composition references, and public-part alignment.
It does not define widget semantics.
</p>
