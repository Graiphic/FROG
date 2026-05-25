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

<h2>Purpose</h2>

<p>
This document defines the repository-visible package posture for the official <code>Default</code> widget realization family.
The package posture describes how Default realization manifests, documents, resources, SVG assets, public part bindings, property bindings, method bindings, event bindings, composition slots, host hints, and validation expectations are published together.
</p>

<p>
The package posture is not widget class law.
It is the machine-readable publication layer that allows hosts, validators, tooling, examples, and future UI runtimes to consume a coherent default embodiment of standardized widgets without confusing realization resources with semantic ownership.
</p>

<hr/>

<h2>Active Default Manifests</h2>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Manifest</th>
      <th>Target classes</th>
      <th>Category</th>
      <th>Runtime role posture</th>
      <th>Manifest posture</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><a href="./Boolean.md">Boolean</a></td><td><code>boolean.default.wfrog</code></td><td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td><td>core value</td><td>control / indicator</td><td>single-family scalar package</td></tr>
    <tr><td><a href="./String.md">String</a></td><td><code>string.default.wfrog</code></td><td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td><td>core value</td><td>control / indicator</td><td>single-family scalar package</td></tr>
    <tr><td><a href="./Button.md">Button</a></td><td><code>button.default.wfrog</code></td><td><code>frog.widgets.button</code></td><td>action</td><td>command</td><td>single-family action package</td></tr>
    <tr><td><a href="./Numeric.md">Numeric</a></td><td><code>numeric.default.wfrog</code></td><td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td><td>core value</td><td>control / indicator</td><td>single-family scalar package</td></tr>
    <tr><td><a href="./Enum.md">Enum</a></td><td><code>enum.default.wfrog</code></td><td><code>frog.widgets.enum_control</code>, <code>frog.widgets.enum_indicator</code></td><td>core value</td><td>control / indicator</td><td>single-family scalar package</td></tr>
    <tr><td><a href="./Path.md">Path</a></td><td><code>path.default.wfrog</code></td><td><code>frog.widgets.path_control</code>, <code>frog.widgets.path_indicator</code></td><td>core value</td><td>control / indicator</td><td>single-family scalar package</td></tr>
    <tr><td><a href="./Picture.md">Picture</a></td><td><code>picture.default.wfrog</code></td><td><code>frog.widgets.picture_control</code>, <code>frog.widgets.picture_indicator</code></td><td>visual data</td><td>control / indicator</td><td>image-surface package</td></tr>
    <tr><td><a href="./Chart.md">Waveform Chart</a></td><td><code>chart.default.wfrog</code></td><td><code>frog.widgets.waveform_chart</code></td><td>visual data</td><td>indicator</td><td>composite package with shell, subobjects, slots, bindings, and host-native plot allowance</td></tr>
    <tr><td><a href="./Listbox.md">Listbox</a></td><td><code>listbox.default.wfrog</code></td><td><code>frog.widgets.listbox_control</code>, <code>frog.widgets.listbox_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>composite package with item rows, selection surfaces, viewport, and virtual-list allowance</td></tr>
    <tr><td><a href="./Tab.md">Tab</a></td><td><code>tab.default.wfrog</code></td><td><code>frog.widgets.tab_control</code>, <code>frog.widgets.tab_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>composite package with page-navigation and page-hosting surfaces</td></tr>
    <tr><td><a href="./Tree.md">Tree</a></td><td><code>tree.default.wfrog</code></td><td><code>frog.widgets.tree_control</code>, <code>frog.widgets.tree_indicator</code></td><td>selection / navigation</td><td>control / indicator</td><td>composite package with node rows, hierarchy surfaces, check state, editing, drag/drop, and virtual-tree allowance</td></tr>
    <tr><td><a href="./Table.md">Table</a></td><td><code>table.default.wfrog</code></td><td><code>frog.widgets.table_control</code>, <code>frog.widgets.table_indicator</code></td><td>structured data</td><td>control / indicator</td><td>composite package with headers, grid, cells, editing, sorting, filtering, and virtual-grid allowance</td></tr>
    <tr><td><a href="./Array.md">Array</a></td><td><code>array.default.wfrog</code></td><td><code>frog.widgets.array</code>, <code>frog.widgets.array_control</code>, <code>frog.widgets.array_indicator</code></td><td>structured data</td><td>structured collection</td><td>structured collection package</td></tr>
    <tr><td><a href="./Cluster.md">Cluster</a></td><td><code>cluster.default.wfrog</code></td><td><code>frog.widgets.cluster</code></td><td>structured data</td><td>structured aggregate</td><td>structured aggregate package</td></tr>
    <tr><td><a href="./Label.md">Label</a></td><td><code>label.default.wfrog</code></td><td><code>frog.widgets.label</code></td><td>support</td><td>support</td><td>support package</td></tr>
    <tr><td><a href="./Frame.md">Frame</a></td><td><code>frame.default.wfrog</code></td><td><code>frog.widgets.frame</code></td><td>support</td><td>support</td><td>support package</td></tr>
    <tr><td><a href="./Decorations.md">Decorations</a></td><td><code>decorations.default.wfrog</code></td><td><code>frog.widgets.flat_box</code>, <code>frog.widgets.horizontal_line</code>, <code>frog.widgets.vertical_line</code></td><td>support</td><td>decorative support</td><td>decorative package</td></tr>
    <tr><td><a href="./Splitter.md">Splitter</a></td><td><code>splitter.default.wfrog</code></td><td><code>frog.widgets.horizontal_splitter</code>, <code>frog.widgets.vertical_splitter</code></td><td>layout</td><td>layout</td><td>layout package</td></tr>
    <tr><td><a href="./Subpanel.md">Subpanel</a></td><td><code>subpanel.default.wfrog</code></td><td><code>frog.widgets.subpanel</code></td><td>layout / embedded front panel</td><td>subpanel host</td><td>embedded-front-panel package</td></tr>
  </tbody>
</table>

<hr/>

<h2>Manifest Shape</h2>

<p>
A Default realization manifest should provide the following machine-readable surfaces:
</p>

<ul>
  <li><code>format</code> — package file format identity.</li>
  <li><code>kind</code> — realization-library identity.</li>
  <li><code>publication_role</code> — repository-facing publication role.</li>
  <li><code>version_governance_ref</code> — reference to centralized version-governance documentation.</li>
  <li><code>package</code> — package identity, namespace, category, role posture, and summary.</li>
  <li><code>ownership_boundary</code> — what the realization owns and does not own.</li>
  <li><code>targets</code> — public widget classes targeted by the realization.</li>
  <li><code>exports</code> — exported realization and resource identifiers.</li>
  <li><code>realizations</code> — realization records, supported parts, fallback rules, and family posture.</li>
  <li><code>resources</code> — SVG templates, subobject resources, and host-consumable resources.</li>
  <li><code>part_bindings</code> — public part names mapped to selectors or host surfaces.</li>
  <li><code>property_bindings</code> — property/member surfaces mapped to realization operations.</li>
  <li><code>method_bindings</code> — method surfaces mapped to realization operations, when present.</li>
  <li><code>event_bindings</code> — event surfaces mapped to source parts, when present.</li>
  <li><code>state_maps</code> — visual or interaction state posture, when present.</li>
  <li><code>composition</code> — shell, layer order, slots, and nested slots for composite families, when present.</li>
  <li><code>host_hints</code> — rendering and host-native replacement posture.</li>
  <li><code>validation_expectations</code> — repository-visible validation expectations.</li>
</ul>

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

Implementations/Reference/WidgetValidator/
    - non-normative repository hygiene validation

runtime implementation
    - interpretation of the published package
</code></pre>

<p>
Default manifests publish realization resources, public part bindings, state maps, property bindings, method bindings, event bindings, anchors, host hints, and validation expectations.
They do not redefine public widget class law.
</p>

<hr/>

<h2>Public Part and Resource Rule</h2>

<ul>
  <li>Every declared public part should correspond to a public part defined by the owning widget class-law document.</li>
  <li>Every declared resource path should resolve relative to the manifest file.</li>
  <li>SVG resources should expose relevant public parts through <code>data-frog-part</code> markers when those parts are represented in the SVG template.</li>
  <li>Composite shell resources may expose slots through <code>data-frog-slot</code> markers.</li>
  <li>A resource may serve multiple public parts when the manifest explicitly declares that relationship.</li>
  <li>Realization-private renderer handles, host-native handles, virtualized rows, virtualized nodes, canvas handles, GPU surfaces, and IDE-private editors must not become public semantic storage.</li>
</ul>

<hr/>

<h2>Host-Native Replacement Rule</h2>

<p>
A host may replace SVG-backed rendering with host-native widgets or optimized surfaces when the replacement preserves:
</p>

<ul>
  <li>target widget class identity,</li>
  <li>primary value posture,</li>
  <li>public part model,</li>
  <li>public property surface,</li>
  <li>public method surface,</li>
  <li>public event surface,</li>
  <li>realization-private boundary,</li>
  <li>validation expectations that apply to the published package.</li>
</ul>

<p>
This is especially important for Table, Tree, Listbox, Tab, Picture, and Waveform Chart, where serious hosts may prefer native virtual grids, native virtual trees, native lists, native tab controls, image surfaces, canvas layers, or GPU-backed plot surfaces.
</p>
