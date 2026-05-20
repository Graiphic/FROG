<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization - Ring Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized ring widgets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Target Classes</h2>

<ul>
  <li><code>frog.widgets.ring_control</code></li>
  <li><code>frog.widgets.ring_indicator</code></li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This default realization publishes a compact selector embodiment for the FROG
Ring widget family. It provides a rectangular value face, selected-item display,
selector affordance, popup anchor surface, option row surfaces, and focus
surface.
</p>

<p>
This realization is downstream from widget class law. It does not define Ring
value semantics, item identity, numeric item values, FIR semantics, backend
lowering, runtime-private host handles, or IDE-private item editing behavior.
</p>

<hr/>

<h2>Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>selector_face</code></li>
  <li><code>selector_arrow</code></li>
  <li><code>popup_layer</code></li>
  <li><code>option_row</code></li>
  <li><code>option_label</code></li>
  <li><code>option_selection_face</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/ring/templates/ring_rectangular.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>value_face</code> anchors the selected value body.</li>
  <li><code>value_display</code> anchors selected item text.</li>
  <li><code>selector_face</code> and <code>selector_arrow</code> anchor the open-selector affordance.</li>
  <li><code>popup_layer</code> anchors a host overlay for visible options.</li>
  <li><code>option_row</code>, <code>option_label</code>, and <code>option_selection_face</code> publish option-row grammar.</li>
</ul>

<p>
A runtime may use a host-native popup or select surface when it preserves the
public part alignment, value posture, item inventory, and style-property
bindings.
</p>

<p>
The LabVIEW-like Ring posture is text over numeric value: <code>items[].label</code>
is visible text, <code>items[].value</code> is the diagram-facing scalar, and
<code>value.selected_id</code> is only stable UI identity. Display format,
digital-display visibility, increment/decrement visibility, disabled item
posture, data-entry limits, and undefined-value policy remain source-owned
properties consumed through this realization.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>All declared public parts must exist in widget class law.</li>
  <li>Declared SVG resources must resolve relative to this package.</li>
  <li>The declared SVG resources should expose all declared <code>data-frog-part</code> markers.</li>
  <li>Resources must not become semantic owners of Ring item values.</li>
  <li>Host-native selector handles must remain realization-private.</li>
</ul>
