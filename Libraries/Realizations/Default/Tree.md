<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Tree Widgets</h1>

<p align="center">
  <strong>Default realization posture for the standardized tree widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Target Classes</h2>

<ul>
  <li><code>frog.widgets.tree_control</code></li>
  <li><code>frog.widgets.tree_indicator</code></li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This default realization publishes a portable tree embodiment for the FROG Tree widget family.
It provides a shell, node region, rows, indent and connector surfaces, expanders, optional checkbox faces, node icons, labels, selection, active-node, edit, drag/drop, scrollbars, and focus surface.
</p>

<p>
This realization is downstream from widget class law.
It does not define tree value semantics, editing semantics, check-state propagation, filesystem behavior, object-browser behavior, or host-native virtualization semantics.
</p>

<hr/>

<h2>Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>node_region</code></li>
  <li><code>node_row</code></li>
  <li><code>node_indent</code></li>
  <li><code>node_connector</code></li>
  <li><code>expander_face</code></li>
  <li><code>checkbox_face</code></li>
  <li><code>node_icon</code></li>
  <li><code>node_label</code></li>
  <li><code>active_node</code></li>
  <li><code>selection_face</code></li>
  <li><code>edit_overlay</code></li>
  <li><code>drag_feedback</code></li>
  <li><code>drop_target</code></li>
  <li><code>vertical_scrollbar</code></li>
  <li><code>horizontal_scrollbar</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/tree/templates/tree_shell.svg</code></li>
  <li><code>assets/tree/subobjects/node_region.svg</code></li>
  <li><code>assets/tree/subobjects/node_row.svg</code></li>
  <li><code>assets/tree/subobjects/selection_face.svg</code></li>
  <li><code>assets/tree/subobjects/active_node.svg</code></li>
  <li><code>assets/tree/subobjects/expander_face.svg</code></li>
  <li><code>assets/tree/subobjects/checkbox_face.svg</code></li>
  <li><code>assets/tree/subobjects/node_icon.svg</code></li>
  <li><code>assets/tree/subobjects/edit_overlay.svg</code></li>
  <li><code>assets/tree/subobjects/drag_feedback.svg</code></li>
  <li><code>assets/tree/subobjects/drop_target.svg</code></li>
  <li><code>assets/tree/subobjects/vertical_scrollbar.svg</code></li>
  <li><code>assets/tree/subobjects/horizontal_scrollbar.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>node_region</code> anchors the visible tree body.</li>
  <li><code>node_row</code> anchors visible node rows.</li>
  <li><code>node_indent</code> and <code>node_connector</code> anchor hierarchy display.</li>
  <li><code>expander_face</code> anchors expand/collapse affordances.</li>
  <li><code>checkbox_face</code> anchors optional check-state display.</li>
  <li><code>node_icon</code> anchors optional icon display.</li>
  <li><code>node_label</code> anchors node text.</li>
  <li><code>selection_face</code> and <code>active_node</code> anchor selection and keyboard focus.</li>
  <li><code>edit_overlay</code> anchors node-label editing.</li>
  <li><code>drag_feedback</code> and <code>drop_target</code> anchor optional drag/drop posture.</li>
</ul>

<p>
A runtime may replace the SVG-backed tree with a host-native virtual tree when it preserves the published part model and public property/method/event surfaces.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>All declared public parts must exist in widget class law.</li>
  <li>Declared SVG resources must resolve relative to this package.</li>
  <li>The declared SVG resources should expose all declared <code>data-frog-part</code> markers.</li>
  <li>Resources must not become semantic owners of tree value.</li>
  <li>Host-native virtualization must not expose private node or row handles as public members.</li>
</ul>
