<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Validation</h1>

<p align="center">
  <strong>Validation checklist for the published Default widget realization layer</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document defines the repository-visible validation posture for the Default widget realization layer.
It is intentionally downstream from widget class law.
</p>

<p>
The goal is to keep the published widget layer coherent as a standard-facing surface rather than a loose set of unrelated documents.
Validation therefore focuses on repository hygiene, publication alignment, manifest integrity, resource resolution, public-part consistency, and realization/class-law boundaries.
</p>

<hr/>

<h2>Validation Targets</h2>

<p>
Each published widget family should provide:
</p>

<ul>
  <li>a widget class-law document under <code>Libraries/Widgets/</code>,</li>
  <li>a Default realization document under <code>Libraries/Realizations/Default/</code>,</li>
  <li>a machine-readable manifest <code>*.default.wfrog</code>,</li>
  <li>resolvable SVG template or subobject resources where the manifest declares them,</li>
  <li>part bindings that refer to public widget parts,</li>
  <li>target classes that appear in the corresponding widget class-law document,</li>
  <li>role posture that distinguishes control, indicator, command, support, layout, container, or structured families,</li>
  <li>clear realization-private boundaries for host-native handles, renderer caches, virtualized rows/nodes/items, canvas surfaces, GPU surfaces, and IDE-private editing structures.</li>
</ul>

<hr/>

<h2>Machine-Checkable Expectations</h2>

<ul>
  <li>All manifests must be valid JSON.</li>
  <li>Each manifest must include <code>format</code>, <code>kind</code>, <code>package</code>, <code>targets</code>, <code>resources</code>, and <code>validation_expectations</code>.</li>
  <li>Each manifest should include <code>publication_role</code>, <code>version_governance_ref</code>, <code>ownership_boundary</code>, <code>exports</code>, <code>realizations</code>, <code>part_bindings</code>, and <code>host_hints</code>.</li>
  <li>Each declared target class must be listed in the corresponding validator family definition and must appear in the owning widget class-law document.</li>
  <li>Each target must declare an explicit role.</li>
  <li>Generic target roles such as <code>widget</code> should not be used in published Default manifests.</li>
  <li>Each declared resource path must resolve relative to the manifest file.</li>
  <li>Resource paths must not escape the <code>Libraries/Realizations/Default/</code> directory.</li>
  <li>Each exported resource identifier should correspond to a declared resource.</li>
  <li>Each composition slot or nested slot should reference a declared resource.</li>
  <li>Each part binding should refer to a supported public part.</li>
  <li>Each event binding source part should refer to a supported public part.</li>
  <li>Each resource <code>target_part</code> or <code>target_parts</code> entry should refer to a supported public part.</li>
  <li>Each declared supported part should appear in the corresponding widget class-law document.</li>
  <li>SVG resources should expose declared public parts through <code>data-frog-part</code> markers when applicable.</li>
  <li>SVG shell resources may expose composition slots through <code>data-frog-slot</code> markers.</li>
  <li>SVG-backed resources should have a reviewable semantic skin contract covering required parts, optional parts, anchors, bindings, slots, dynamic text/value regions, resize policy, solidary groups, repeatable regions, and clip regions.</li>
  <li>Runtime-facing overlays should be alignable from published SVG parts or anchors; a Default realization should not require private example-specific coordinates to render accepted widget surfaces.</li>
  <li>Any visible border, frame, selection outline, cell outline, scrollbar track, or equivalent stroke-like surface should preserve one uniform published thickness after resize; SVG-backed resources should use non-scaling strokes or an equivalent realization strategy when a single width property controls the surface.</li>
  <li>Bordered fill surfaces should not leave unintended inner gaps. If a value face, text region, button face, state face, or equivalent part is meant to touch its surrounding border, the SVG geometry should meet the stroke inner edge or extend under a transparent / stroke-only frame.</li>
  <li>Documentation files should not contain literal escaped newline artifacts in rendered lists.</li>
</ul>

<hr/>

<h2>Composite Widget Expectations</h2>

<p>
Composite widgets may use shell resources, subobject assets, slots, nested slots, layer-order records, property bindings, method bindings, event bindings, state maps, and host-native replacement hints.
</p>

<p>
The current complex families that should remain especially protected are:
</p>

<ul>
  <li><code>frog.widgets.waveform_chart</code></li>
  <li><code>frog.widgets.listbox_control</code> and <code>frog.widgets.listbox_indicator</code></li>
  <li><code>frog.widgets.tab_control</code> and <code>frog.widgets.tab_indicator</code></li>
  <li><code>frog.widgets.tree_control</code> and <code>frog.widgets.tree_indicator</code></li>
  <li><code>frog.widgets.table_control</code> and <code>frog.widgets.table_indicator</code></li>
</ul>

<p>
For those families, validation should pay particular attention to public parts, SVG marker consistency, resource references, method/event surfaces, virtualized-host boundaries, and host-native replacement rules.
</p>

<hr/>

<h2>Reference Validator</h2>

<p>
The non-normative reference validator lives under:
</p>

<pre><code>Implementations/Reference/WidgetValidator/validate_widget_layer.py</code></pre>

<p>
Run it from the repository root:
</p>

<pre><code>python Implementations/Reference/WidgetValidator/validate_widget_layer.py</code></pre>

<p>
That validator is a repository-quality tool.
It does not define widget semantics, realization semantics, FROG source law, FIR law, lowering law, runtime law, or host rendering law.
</p>

<hr/>

<h2>Expected Validation Boundary</h2>

<p>
The validator may reject repository hygiene errors such as missing manifests, invalid JSON, missing required keys, unresolved resources, class mismatches, invalid part references, resource references that escape the Default directory, or malformed manifest shape.
</p>

<p>
The validator may warn about weaker publication posture such as missing optional descriptive keys, incomplete SVG markers, generic roles, or abstract parts not represented directly by SVG resources.
</p>

<p>
The validator must not decide whether a runtime implementation is visually attractive, performant, or complete.
It only protects the repository-visible contract between widget class law, Default realization manifests, resources, and documentation.
</p>
