<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Ring Widget Example Reference</h1>

<p align="center">
  <strong>Current text-over-numeric finite-choice proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

Reviewed 2026-06-12.

<h2>Scope</h2>

<p>
The current Ring progression is demonstrated by
<code>Examples/21_ring_selection_roundtrip</code>. It proves a LabVIEW-like
Ring: the user sees item labels while the diagram receives the selected item's
numeric value.
</p>

<p>
Ring is distinct from Enum. A Ring maps labels to source-declared scalar values
and can carry numeric representation and undefined-value policy. Enum represents
a closed typed symbolic set. The two widgets may share a visual realization, but
they do not share the same value law.
</p>

<p>
The intended implementation direction is shared UI, separate value contract:
same compact skin, increment/decrement commands, dropdown host surface, aura, and
Array containment rules; different terminal value semantics.
</p>

<hr/>

<h2>Source-Owned Instance Data</h2>

<ul>
  <li>Ring item ids, labels, numeric values, enabled/visible posture, and order.</li>
  <li>Selected id, selected index, selected label, and selected numeric value.</li>
  <li>Data type representation, sequential-values posture, and undefined-value policy where used.</li>
  <li>Control/indicator roles, labels, layout, and instance-level style.</li>
</ul>

<hr/>

<h2>Published Parts</h2>

<table>
  <thead>
    <tr>
      <th>Part</th>
      <th>Purpose</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>root</code></td><td>SVG template root.</td></tr>
    <tr><td><code>placement_bounds</code></td><td>Invisible placement aura consumed by IDE/grid/Array placement. This is not focus or selection.</td></tr>
    <tr><td><code>label</code>, <code>caption</code></td><td>Text surfaces owned by source/widget placement.</td></tr>
    <tr><td><code>value_face</code></td><td>Visible selected-value body.</td></tr>
    <tr><td><code>value_display</code></td><td>Visible selected item label.</td></tr>
    <tr><td><code>focus_ring</code></td><td>Focus boundary around <code>value_face</code> only.</td></tr>
    <tr><td><code>spinner</code>, <code>increment_up</code>, <code>increment_down</code></td><td>Optional LabVIEW-like next/previous item command surfaces.</td></tr>
    <tr><td><code>list_panel</code>, <code>option_row</code>, <code>option_label</code>, <code>option_selection_face</code></td><td>Host popup/dropdown surface parts.</td></tr>
  </tbody>
</table>

<p>
The Default Ring compact SVG does not publish a selector face or selector arrow.
Any opened popup/dropdown is a host surface backed by the dropdown SVG skin and
anchored to <code>value_face</code>.
</p>

<hr/>

<h2>Placement Contract</h2>

<ul>
  <li>The compact SVG viewBox is <code>200 x 130</code>.</li>
  <li><code>placement_bounds</code> is <code>176 x 38</code> source units and carries the uniform 4-unit placement aura around the compact body.</li>
  <li>Array cells consume <code>placement_bounds</code> as the contained widget footprint, exactly like Default Numeric. They must not size themselves from <code>value_face</code>, <code>spinner</code>, or focus/selection overlays.</li>
  <li>Control and indicator variants share the same placement footprint. Indicator posture changes fill/style, not the cell boundary.</li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>Changing selection patches front-panel state. <code>Execute</code> publishes the selected numeric value.</li>
  <li>The runtime must not infer Ring values from text labels when explicit item values exist.</li>
  <li>Increment/decrement commands navigate visible enabled items and honor <code>display.increment_decrement_visible</code>.</li>
  <li>The focus ring covers <code>value_face</code> only and never encloses increment/decrement buttons.</li>
  <li>Disabled item posture, colors, fonts, borders, popup state, and numeric representation must come from source/default realization data.</li>
</ul>
