<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Enum Widget Example Reference</h1>

<p align="center">
  <strong>Current Example 08 Enum control and indicator proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

Reviewed 2026-06-12.

<h2>Scope</h2>

<p>
The stabilized Enum slice is <code>Examples/08_enum_value_roundtrip</code>.
It proves one <code>frog.widgets.enum_control</code> and one
<code>frog.widgets.enum_indicator</code> over the <code>example08.mode</code>
enum domain.
</p>

<pre><code>mode_input.value
  -&gt; mode_result.value
  -&gt; public result_mode
</code></pre>

<p>
Enum represents a closed typed symbolic set. It is distinct from Ring even when
the two widgets share a compact visual realization: Enum publishes item identity
from a named enum domain, while Ring maps visible strings to scalar numeric
values.
</p>

<p>
The intended implementation direction is shared UI, separate value contract:
same compact skin, increment/decrement commands, dropdown host surface, aura, and
Array containment rules; different terminal value semantics.
</p>

<hr/>

<h2>Source-Owned Instance Data</h2>

<ul>
  <li><code>Examples/08_enum_value_roundtrip/main.frog</code></li>
  <li>control widget: <code>mode_input</code>, <code>frog.widgets.enum_control</code></li>
  <li>indicator widget: <code>mode_result</code>, <code>frog.widgets.enum_indicator</code></li>
  <li>enum item ids, display text, numeric values, enabled posture, selected value, caption placement, widget layout, and style overrides live in <code>.frog</code></li>
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
    <tr><td><code>value_display</code></td><td>Visible selected item text.</td></tr>
    <tr><td><code>focus_ring</code></td><td>Focus boundary around <code>value_face</code> only.</td></tr>
    <tr><td><code>spinner</code>, <code>increment_up</code>, <code>increment_down</code></td><td>Optional LabVIEW-like next/previous item command surfaces.</td></tr>
  </tbody>
</table>

<p>
The Default Enum compact SVG does not publish a selector face or selector arrow.
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
  <li>The selected value must resolve to a declared enum item unless the source explicitly allows undefined values.</li>
  <li>Increment/decrement commands navigate the declared item order and honor <code>display.increment_buttons_visible</code>.</li>
  <li>The focus ring covers <code>value_face</code> only and never encloses increment/decrement buttons.</li>
  <li>The runtime must reject hidden hand-built selector shells or local duplicated SVG skins for this example.</li>
</ul>
