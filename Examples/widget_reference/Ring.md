<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Ring Widget Example Reference</h1>

<p align="center">
  <strong>Current text-over-numeric finite-choice proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Ring progression is demonstrated by
<code>Examples/21_ring_selection_roundtrip</code>. It proves a LabVIEW-like
Ring: the user sees item labels, while the diagram receives the selected
item's numeric value.
</p>

<p>
Ring is distinct from Enum. A Ring maps labels to source-declared values and
can allow numeric representations and undefined-value policy. Enum represents
a closed typed symbolic set.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Ring item ids, labels, numeric values, enabled/visible posture, and order.</li>
  <li>Selected id, selected index, selected label, and selected numeric value.</li>
  <li>Data type representation, sequential-values posture, and undefined-value policy where used.</li>
  <li>Control/indicator roles, labels, layout, and instance-level style.</li>
</ul>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Part</th>
      <th>Purpose</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>value_face</code></td><td>Main Ring face.</td></tr>
    <tr><td><code>value_display</code></td><td>Visible selected label surface.</td></tr>
    <tr><td><code>selector_face</code>, <code>selector_arrow</code></td><td>Compact selector affordance.</td></tr>
    <tr><td><code>popup_layer</code>, <code>option_row</code>, <code>option_label</code></td><td>Host-rendered option popup aligned to published parts.</td></tr>
    <tr><td><code>option_selection_face</code></td><td>Popup selection state.</td></tr>
  </tbody>
</table>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/21_ring_selection_roundtrip/ui/ring_panel.wfrog</code></li>
  <li>Default Ring package: <code>Libraries/Realizations/Default/ring.default.wfrog</code></li>
  <li>Default Ring SVG: <code>Libraries/Realizations/Default/assets/ring/templates/ring_rectangular.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>Changing selection patches front-panel state. <code>Execute</code> publishes the selected numeric value.</li>
  <li>The runtime must not infer Ring values from text labels when explicit item values exist.</li>
  <li>Popup and selector overlays must align to the published Default Ring parts.</li>
  <li>Disabled item posture, colors, fonts, borders, popup state, and numeric representation must come from source/default realization data.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 21. When
published, it should live under <code>Examples/21_ring_selection_roundtrip/reference/</code>
and follow <a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>
