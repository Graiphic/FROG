<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization &mdash; Tree Widgets</h1>

<p align="center">
  <strong>Default realization posture for the standardized tree widgets</strong><br/>
  <em>FROG &mdash; Free Open Graphical Language</em>
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
This default realization publishes the portable Tree geometry needed by runtimes and IDE hosts.
It intentionally keeps the SVG asset small: the shell publishes only stable anchors and geometry surfaces.
Rows, hierarchy affordances, active node or cell selection, editing, drag/drop feedback, and scrollbar thumbs are runtime host overlays generated from <code>.frog</code> instance data and this <code>.wfrog</code> realization contract.
</p>

<p>
This realization is downstream from widget class law.
It does not define tree value semantics, editing semantics, check-state propagation, filesystem behavior, object-browser behavior, or host-native virtualization semantics.
It also does not keep hidden prototype visuals in the SVG; unused visual fragments must be removed rather than disabled by runtime CSS.
</p>

<hr/>

<h2>Static Shell Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>node_region</code></li>
  <li><code>vertical_scrollbar</code></li>
  <li><code>horizontal_scrollbar</code></li>
</ul>

<hr/>

<h2>Dynamic Overlay Parts</h2>

<p>
The following public parts are realized by host overlays aligned to the static shell geometry:
</p>

<ul>
  <li><code>column_headers</code></li>
  <li><code>node_row</code></li>
  <li><code>node_indent</code></li>
  <li><code>node_connector</code></li>
  <li><code>expander_face</code></li>
  <li><code>checkbox_face</code></li>
  <li><code>node_icon</code></li>
  <li><code>node_label</code></li>
  <li><code>active_node</code></li>
  <li><code>active_cell</code></li>
  <li><code>selection_face</code></li>
  <li><code>edit_overlay</code></li>
  <li><code>drag_feedback</code></li>
  <li><code>drop_target</code></li>
</ul>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/tree/templates/tree_shell.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>node_region</code> anchors the visible tree body.</li>
  <li><code>column_headers</code> is generated as a host overlay. Its visibility and fixed/scrolling posture are controlled by <code>.frog</code> display properties.</li>
  <li><code>vertical_scrollbar</code> anchors the vertical scrollbar host overlay.</li>
  <li><code>horizontal_scrollbar</code> anchors the horizontal scrollbar host overlay.</li>
  <li><code>label</code> and <code>caption</code> expose movable text anchors owned by the <code>.frog</code> instance.</li>
  <li>Tree rows, text, expand/collapse controls, selection, edits, and scroll thumbs are generated overlays, not hidden SVG artwork.</li>
  <li><code>node_icon</code> consumes realization-published item-symbol SVG assets and <code>style.node_icon_slot.*</code> for the item icon slot.</li>
  <li><code>expander_face</code> consumes <code>style.expander.*</code>; it must not share the item icon slot style.</li>
  <li><code>checkbox_face</code> consumes <code>style.checkbox.*</code>; it must not share the expander or item icon slot style.</li>
</ul>

<p>
A runtime may replace the SVG-backed tree with a host-native virtual tree when it preserves the published part model and public property/method/event surfaces.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>All declared public parts must exist in widget class law.</li>
  <li>The declared SVG resource must resolve relative to this package.</li>
  <li>The shell SVG must expose only static anchor and geometry <code>data-frog-part</code> markers.</li>
  <li>Dynamic Tree parts must be generated as host overlays aligned to the static geometry markers.</li>
  <li>Resources must not become semantic owners of tree value.</li>
  <li>Item icon slots, expanders, and checkbox faces must remain independently styleable through <code>style.node_icon_slot.*</code>, <code>style.expander.*</code>, and <code>style.checkbox.*</code>.</li>
  <li>Host-native virtualization must not expose private node or row handles as public members.</li>
  <li>Unused prototype visuals must be removed from the SVG and <code>.wfrog</code>, not hidden in the runtime.</li>
</ul>
