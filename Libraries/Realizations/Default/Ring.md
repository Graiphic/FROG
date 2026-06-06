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
selector affordance, optional increment/decrement command, and a dropdown
host-surface skin for option rows.
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
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>selector_face</code></li>
  <li><code>spinner</code></li>
  <li><code>increment_up</code></li>
  <li><code>increment_down</code></li>
  <li><code>list_panel</code></li>
  <li><code>option_row</code></li>
  <li><code>option_label</code></li>
  <li><code>option_selection_face</code></li>
</ul>

<p>
The visible arrow inside the selector is a realization detail owned by
<code>selector_face</code>. It is named in SVG as a detail so hosts can style it,
but it is not a public semantic part.
</p>

<p>
The optional increment/decrement command is public when shown. The
<code>spinner</code> group is controlled by
<code>display.increment_decrement_visible</code>, and the
<code>increment_up</code> / <code>increment_down</code> parts invoke the
LabVIEW-like next/previous item command without replacing the selector.
</p>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/ring/templates/ring_rectangular.svg</code></li>
  <li><code>assets/ring/templates/ring_dropdown_list.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>value_face</code> anchors the selected value body.</li>
  <li><code>value_display</code> anchors selected item text and consumes Default-supplied or source-overridden text style members such as <code>style.value_display.vertical_offset</code>.</li>
  <li><code>selector_face</code> anchors the open-selector affordance.</li>
  <li><code>spinner</code>, <code>increment_up</code>, and <code>increment_down</code> anchor the optional next/previous item command.</li>
  <li><code>list_panel</code> anchors the dropdown host overlay for visible options.</li>
  <li><code>option_row</code>, <code>option_label</code>, and <code>option_selection_face</code> publish option-row grammar.</li>
</ul>

<p>
A runtime may use a host-native popup or select surface when it preserves the
public part alignment, value posture, item inventory, and style-property
bindings.
</p>

<p>
The accepted Ring appearance is published as reusable
<code>default_widget_properties</code> in <code>ring.default.wfrog</code>.
That includes the one-pixel value-face border, value text vertical offset,
selector styling, popup styling, and dropdown asset posture. Hosts apply these
defaults by widget class and role before any source-owned <code>.frog</code>
instance overrides.
</p>

<p>
The dropdown host surface is declared in the manifest. Its width is anchored to
<code>value_face</code> only, not the selector face, so changing the main value
body width changes the dropdown width by the same amount.
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
