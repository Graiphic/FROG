<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Common Widget Properties</h1>

<p align="center">
  <strong>Shared property-node surface for standard widget classes</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document defines the shared property vocabulary used by standard FROG
widget classes when they expose object-style property access. It avoids
rediscovering the same bounds, label, caption, visibility, focus, binding,
documentation, and style concepts in every widget family.
</p>

<p>
The common property surface does not replace a widget family's class-law
document. A widget class may implement the common properties that make sense
for its role, and may add family-specific properties such as Ring
<code>items[]</code>, Tab <code>pages[]</code>, Listbox viewport state, Picture
image metadata, or Table cell access.
</p>

<hr/>

<h2>Ownership Boundary</h2>

<ul>
  <li><code>.frog</code> owns widget instances, layout, bindings, labels, captions, initial values, source-owned visual overrides, and any source-visible property values.</li>
  <li><code>.wfrog</code> owns realization package references, Default asset ids, supported classes, and host capability declarations.</li>
  <li>Default SVG assets own reusable public parts, anchors, bindings, internal geometry, and default visual behavior.</li>
  <li>Runtime hosts may provide overlays for interaction, but overlays must align to published SVG parts or anchors and consume source-owned properties.</li>
  <li>Runtime hosts must not replace this surface with hardcoded widget visuals or hidden HTML/CSS-only fallback shells.</li>
</ul>

<hr/>

<h2>Access Model</h2>

<p>
Ordinary dataflow should use <code>widget_value</code> for a widget's primary
value. Object-style access should use explicit UI interaction primitives such
as <code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>,
method invocation, and event observation.
</p>

<p>
Property writes that change front-panel state are not implicit program
execution. An execution step runs only when the source and host contract expose
an explicit execution action, for example an <code>Execute</code> action in a
browser-hosted example.
</p>

<hr/>

<h2>Common Property Families</h2>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Representative FROG surface</th>
      <th>Portable meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Bounds</td>
      <td><code>layout.x</code>, <code>layout.y</code>, <code>layout.width</code>, <code>layout.height</code></td>
      <td>Source-owned widget rectangle in the front-panel coordinate space.</td>
    </tr>
    <tr>
      <td>Position</td>
      <td><code>layout.x</code>, <code>layout.y</code></td>
      <td>Source-owned left/top placement. Hosts may expose this as a property-node alias for bounds position.</td>
    </tr>
    <tr>
      <td>Size</td>
      <td><code>layout.width</code>, <code>layout.height</code></td>
      <td>Source-owned widget size. Resizing must not change semantic value unless the widget class explicitly says so.</td>
    </tr>
    <tr>
      <td>Label</td>
      <td><code>label.visible</code>, <code>label.text</code>, <code>label.style.*</code>, <code>label.position.*</code></td>
      <td>The logical front-panel label associated with the widget instance. When visible, its anchor must align deliberately with the widget body or another published realization anchor.</td>
    </tr>
    <tr>
      <td>Caption</td>
      <td><code>caption.visible</code>, <code>caption.text</code>, <code>caption.style.*</code>, <code>caption.position.*</code></td>
      <td>Optional visible caption or local explanatory text associated with the widget instance. Caption offset must be source-owned and aligned to the intended widget part, not a runtime fallback.</td>
    </tr>
    <tr>
      <td>Documentation</td>
      <td><code>documentation.description</code>, <code>documentation.tip_strip</code></td>
      <td>Human-oriented description and tooltip-style help text. It is not a semantic value.</td>
    </tr>
    <tr>
      <td>Visibility and enabled state</td>
      <td><code>interaction.visible</code>, <code>interaction.enabled</code>, <code>interaction.read_only</code></td>
      <td>Whether the widget is rendered, user-operable, and user-mutable.</td>
    </tr>
    <tr>
      <td>Focus and tabbing</td>
      <td><code>interaction.focusable</code>, <code>interaction.focused</code>, <code>interaction.key_focus</code>, <code>interaction.skip_when_tabbing</code></td>
      <td>Host interaction posture for keyboard focus and tab traversal. Focusable SVG-backed widgets should expose a <code>focus_ring</code> part when the widget skin owns the visible focus boundary.</td>
    </tr>
    <tr>
      <td>Mouse wheel</td>
      <td><code>interaction.mouse_wheel_enabled</code></td>
      <td>Whether host mouse-wheel gestures may change or inspect widget state.</td>
    </tr>
    <tr>
      <td>Binding</td>
      <td><code>binding.data_binding_ref</code>, <code>binding.control_index</code></td>
      <td>Source-owned binding references and optional ordering/index metadata.</td>
    </tr>
    <tr>
      <td>Primary value mirror</td>
      <td><code>value.*</code> family-specific mirror</td>
      <td>Object-style mirror of the widget's natural diagram-facing value, when the widget has one.</td>
    </tr>
    <tr>
      <td>Signaling write</td>
      <td><code>value_signaling</code> family-specific event posture</td>
      <td>A value write that also requests the widget's value-changed event semantics, when supported.</td>
    </tr>
    <tr>
      <td>Style</td>
      <td><code>style.*</code>, including <code>style.focus_ring.*</code> when a widget publishes <code>focus_ring</code></td>
      <td>Source-owned visual overrides for colors, borders, thicknesses, fonts, hover, pressed, selected, disabled, and focus states.</td>
    </tr>
    <tr>
      <td>Viewport or display state</td>
      <td><code>viewport.*</code>, <code>display.*</code></td>
      <td>Instance-local display posture such as scroll offsets, popup posture, page header layout, or display formatting.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>A widget example must state which common properties it uses and which family-specific properties it adds.</li>
  <li>When a visual effect is configurable, the value must come from <code>.frog</code> instance data, a Default realization property, or a documented generic fallback for missing optional properties.</li>
  <li>Common layout and style properties must not be hidden inside runtime code.</li>
  <li>Focusable SVG-backed widgets must not rely on private runtime focus geometry. When a visible focus boundary is part of the widget skin, publish <code>focus_ring</code> and bind it through <code>interaction.focused</code> and <code>style.focus_ring.*</code>.</li>
  <li>By default, <code>focus_ring</code> follows the primary visible command/value face, not neighboring selector, spinner, popup, scrollbar, or design-time editor surfaces.</li>
  <li>Host overlays must consume published parts, anchors, and bindings from the chosen realization.</li>
  <li>Visible labels and captions must be checked against their widget body or published anchor; accidental offsets, touching labels, and per-runtime fallback placement are not valid.</li>
  <li>Family-specific documents remain responsible for value semantics, legal methods, events, and specialized property surfaces.</li>
</ul>
