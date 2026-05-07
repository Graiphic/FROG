<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Tree Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized tree control and tree indicator widget classes</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Widgets index</a></li>
  <li><a href="../../Expression/Widget.md">Expression widget instances</a></li>
  <li><a href="../../Expression/Widget%20interaction.md">Expression widget interaction</a></li>
  <li><a href="../../Expression/Widget%20realization.md">Expression widget realization</a></li>
  <li><a href="../../Expression/Widget%20package%20(.wfrog).md">Widget package publication</a></li>
  <li><a href="../../Libraries/UI.md">Executable UI primitives</a></li>
  <li><a href="../Realizations/Default/Tree.md">Default realization — Tree</a></li>
</ul>

<hr/>

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.tree_control</code></li>
  <li><code>frog.widgets.tree_indicator</code></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
The Tree family defines standardized widgets for finite hierarchical node structures in FROG.
It provides a public tree value model, node identity model, expansion model, selection model, viewport model, optional editing and drag posture, checkbox posture, icon posture, and stable public part model.
</p>

<p>
A Tree widget is suitable for hierarchical data display, project navigation, configuration trees, object browsers, inspection trees, hierarchical results, file-like navigation, and structured runtime UI selection.
</p>

<p>
The intrinsic Tree baseline is not a filesystem API, database browser, object-reflection API, scene graph, or IDE-private navigator.
Those integrations may use the Tree family, but they remain downstream from the widget class law.
</p>

<hr/>

<h2>Common Family Posture</h2>

<ul>
  <li>family: hierarchical selection / navigation widget family</li>
  <li>primary value: present</li>
  <li>value type: <code>frog.tree</code></li>
  <li>public value-facing surface: yes</li>
  <li>object-style access surface: yes</li>
  <li>primary value mirror property: <code>value</code></li>
  <li>common label property: <code>label.text</code></li>
  <li>common caption property: <code>caption.text</code></li>
  <li>common visibility property: <code>interaction.visible</code></li>
</ul>

<p>
The family separates:
</p>

<ul>
  <li><code>value</code> — the class-owned tree payload,</li>
  <li><code>nodes.*</code> — node data, identity, children, and display posture,</li>
  <li><code>selection.*</code> — selected node posture,</li>
  <li><code>expansion.*</code> — expanded / collapsed node posture,</li>
  <li><code>viewport.*</code> — visible scroll window and indentation posture,</li>
  <li><code>editing.*</code> — optional node editing posture,</li>
  <li><code>check.*</code> — optional checkbox or check-state posture,</li>
  <li><code>drag.*</code> and <code>drop.*</code> — optional drag/drop posture,</li>
  <li>realization-private row pools, renderer caches, virtualized node handles, host-native tree handles, and IDE-private node objects.</li>
</ul>

<hr/>

<h2><code>frog.widgets.tree_control</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.tree_control</code></li>
  <li><strong>family:</strong> <code>tree_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.tree</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes where editing, selection, expansion, or check-state surfaces are exposed</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A tree control may allow node selection, expansion/collapse, editing labels, reordering nodes, toggling check state, and drag/drop when those capabilities are enabled by the active class posture or profile.
The intrinsic class does not require every host to support full drag/drop or virtualized editing.
</p>

<hr/>

<h2><code>frog.widgets.tree_indicator</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.tree_indicator</code></li>
  <li><strong>family:</strong> <code>tree_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.tree</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no for tree value in the standard portable posture</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A tree indicator is display-oriented.
It may still support viewport changes, node expansion for inspection, and selection readout when those interactions do not mutate the tree value.
</p>

<hr/>

<h2>Tree Value Model</h2>

<p>
The canonical portable tree value is a structured finite hierarchy.
The baseline recognizes:
</p>

<table>
  <thead>
    <tr>
      <th>Field</th>
      <th>Meaning</th>
      <th>Baseline status</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>nodes</code></td><td>Node table or recursive node list.</td><td>required</td></tr>
    <tr><td><code>root_ids</code></td><td>Top-level node identifiers.</td><td>required when nodes are flat-indexed</td></tr>
    <tr><td><code>selected_node_ids</code></td><td>Optional current selection mirror.</td><td>optional</td></tr>
    <tr><td><code>expanded_node_ids</code></td><td>Optional expansion mirror.</td><td>optional</td></tr>
    <tr><td><code>metadata</code></td><td>Optional tree-level metadata.</td><td>optional</td></tr>
  </tbody>
</table>

<h3>Node model</h3>

<p>
Each node may expose:
</p>

<ul>
  <li><code>nodes[].id</code> — stable node identifier,</li>
  <li><code>nodes[].parent_id</code> — optional parent identifier,</li>
  <li><code>nodes[].children</code> or <code>nodes[].child_ids</code>,</li>
  <li><code>nodes[].label</code>,</li>
  <li><code>nodes[].value</code>,</li>
  <li><code>nodes[].icon</code>,</li>
  <li><code>nodes[].depth</code> where derived or declared,</li>
  <li><code>nodes[].path</code>,</li>
  <li><code>nodes[].expanded</code>,</li>
  <li><code>nodes[].selectable</code>,</li>
  <li><code>nodes[].editable</code>,</li>
  <li><code>nodes[].checked</code> when check state is exposed,</li>
  <li><code>nodes[].visible</code>,</li>
  <li><code>nodes[].metadata</code>.</li>
</ul>

<h3>Node identity</h3>

<p>
Node identity SHOULD be stable when the tree participates in selection, expansion, check state, or editing.
Selection and expansion SHOULD be expressed by node id or node path, not by host-private row handles.
</p>

<h3>Node paths</h3>

<p>
A node path is a stable public path-like address through the hierarchy.
Path syntax is realization-independent.
It may be represented as an array of node ids or as a canonicalized path string where allowed by the active profile.
</p>

<hr/>

<h2>Selection Model</h2>

<ul>
  <li><code>selection.mode</code> — <code>none</code>, <code>single_node</code>, <code>multi_node</code>, or <code>range</code></li>
  <li><code>selection.selected_node_ids</code></li>
  <li><code>selection.active_node_id</code></li>
  <li><code>selection.anchor_node_id</code></li>
  <li><code>selection.selected_paths</code></li>
</ul>

<p>
Selection is distinct from tree value.
Changing selection does not mutate node payloads unless an explicit edit or method mutates tree data.
</p>

<hr/>

<h2>Expansion Model</h2>

<ul>
  <li><code>expansion.expanded_node_ids</code></li>
  <li><code>expansion.collapsed_node_ids</code></li>
  <li><code>expansion.auto_expand_on_search</code></li>
  <li><code>expansion.default_state</code> — <code>expanded</code> or <code>collapsed</code></li>
</ul>

<p>
Expansion is a view posture.
It does not mutate the underlying hierarchy unless explicitly treated as part of the value by a profile or program.
</p>

<hr/>

<h2>Check State Model</h2>

<p>
Tree widgets may expose node check states when useful for configuration trees or multi-selection workflows.
</p>

<ul>
  <li><code>check.enabled</code></li>
  <li><code>check.mode</code> — <code>none</code>, <code>boolean</code>, or <code>tri_state</code></li>
  <li><code>check.checked_node_ids</code></li>
  <li><code>check.indeterminate_node_ids</code></li>
  <li><code>nodes[].checked</code></li>
</ul>

<p>
Tri-state check behavior is optional and belongs to the Tree check surface.
It does not redefine scalar boolean widgets.
</p>

<hr/>

<h2>Viewport and Search Model</h2>

<ul>
  <li><code>viewport.node_offset</code></li>
  <li><code>viewport.visible_node_count</code></li>
  <li><code>viewport.row_height</code></li>
  <li><code>viewport.indent_width</code></li>
  <li><code>viewport.horizontal_offset</code></li>
</ul>

<ul>
  <li><code>search.query</code></li>
  <li><code>search.matches</code></li>
  <li><code>search.active_match</code></li>
  <li><code>filter.expression</code></li>
  <li><code>filter.visible_node_count</code></li>
</ul>

<p>
Search and filtering are view postures unless explicitly modeled as value transformations elsewhere.
</p>

<hr/>

<h2>Editing and Drag/Drop Model</h2>

<ul>
  <li><code>editing.enabled</code></li>
  <li><code>editing.active_node_id</code></li>
  <li><code>editing.pending_label</code></li>
  <li><code>editing.commit_policy</code></li>
</ul>

<ul>
  <li><code>drag.enabled</code></li>
  <li><code>drag.active_node_ids</code></li>
  <li><code>drop.enabled</code></li>
  <li><code>drop.target_node_id</code></li>
  <li><code>drop.position</code> — <code>before</code>, <code>inside</code>, or <code>after</code></li>
</ul>

<p>
Drag/drop is optional.
Hosts that do not support portable drag/drop may still support selection and expansion.
</p>

<hr/>

<h2>Standard Properties</h2>

<h3>Common properties</h3>

<ul>
  <li><code>value</code></li>
  <li><code>label.text</code></li>
  <li><code>label.visible</code></li>
  <li><code>caption.text</code></li>
  <li><code>caption.visible</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>interaction.enabled</code></li>
</ul>

<h3>Tree properties</h3>

<ul>
  <li><code>nodes.count</code></li>
  <li><code>nodes.root_ids</code></li>
  <li><code>nodes.children</code></li>
  <li><code>nodes.label</code></li>
  <li><code>nodes.value</code></li>
  <li><code>nodes.icon</code></li>
  <li><code>nodes.path</code></li>
  <li><code>nodes.depth</code></li>
  <li><code>nodes.visible</code></li>
</ul>

<h3>Interaction properties</h3>

<ul>
  <li><code>selection.*</code></li>
  <li><code>expansion.*</code></li>
  <li><code>check.*</code></li>
  <li><code>viewport.*</code></li>
  <li><code>search.*</code></li>
  <li><code>filter.*</code></li>
  <li><code>editing.*</code></li>
  <li><code>drag.*</code></li>
  <li><code>drop.*</code></li>
</ul>

<hr/>

<h2>Standard Methods</h2>

<ul>
  <li><code>focus()</code></li>
  <li><code>clear_selection()</code></li>
  <li><code>set_selection(selection)</code></li>
  <li><code>select_node(node_id)</code></li>
  <li><code>select_path(path)</code></li>
  <li><code>expand_node(node_id)</code></li>
  <li><code>collapse_node(node_id)</code></li>
  <li><code>toggle_node(node_id)</code></li>
  <li><code>expand_all()</code></li>
  <li><code>collapse_all()</code></li>
  <li><code>scroll_to_node(node_id)</code></li>
  <li><code>scroll_to_path(path)</code></li>
  <li><code>set_node_label(node_id, label)</code></li>
  <li><code>set_node_value(node_id, value)</code></li>
  <li><code>append_child(parent_id, node)</code></li>
  <li><code>insert_before(reference_node_id, node)</code></li>
  <li><code>remove_node(node_id)</code></li>
  <li><code>move_node(node_id, target_parent_id, position)</code></li>
  <li><code>set_checked(node_id, checked)</code></li>
  <li><code>find_nodes(query)</code></li>
  <li><code>clear_filter()</code></li>
</ul>

<hr/>

<h2>Standard Events</h2>

<ul>
  <li><code>value_changed</code></li>
  <li><code>node_added</code></li>
  <li><code>node_removed</code></li>
  <li><code>node_moved</code></li>
  <li><code>node_label_changed</code></li>
  <li><code>node_value_changed</code></li>
  <li><code>selection_changed</code></li>
  <li><code>expansion_changed</code></li>
  <li><code>node_expanded</code></li>
  <li><code>node_collapsed</code></li>
  <li><code>check_changed</code></li>
  <li><code>viewport_changed</code></li>
  <li><code>search_changed</code></li>
  <li><code>filter_changed</code></li>
  <li><code>edit_started</code></li>
  <li><code>edit_committed</code></li>
  <li><code>edit_cancelled</code></li>
  <li><code>node_clicked</code></li>
  <li><code>node_double_clicked</code></li>
  <li><code>drag_started</code></li>
  <li><code>drop_completed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2>Public Parts</h2>

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

<p>
Node rows, expanders, connectors, checkbox faces, icons, labels, and selection surfaces are public part categories.
Virtualized node renderers, host-native row handles, recycled item views, or IDE-private node references remain realization-private.
</p>

<hr/>

<h2>Diagram Interaction Posture</h2>

<p>
The Tree family supports:
</p>

<ul>
  <li>natural value participation through <code>widget_value</code>,</li>
  <li>property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>,</li>
  <li>method invocation where legal,</li>
  <li>event observation where legal.</li>
</ul>

<p>
Ordinary tree dataflow should prefer <code>widget_value</code>.
Object-style access should be used for selection, expansion, check state, node editing, viewport, search, filter, or drag/drop interaction.
</p>

<hr/>

<h2>Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-tree <code>value</code> payloads,</li>
  <li>duplicate node identifiers,</li>
  <li>missing root nodes,</li>
  <li>parent-child cycles,</li>
  <li>children that reference unknown node ids,</li>
  <li>selection ids outside the tree,</li>
  <li>expansion ids outside the tree,</li>
  <li>check-state ids outside the tree,</li>
  <li>invalid node paths,</li>
  <li>invalid viewport offsets,</li>
  <li>unsupported edit or drag/drop operations on indicators,</li>
  <li>attempts to treat realization-only row handles, virtualized node handles, host-native tree handles, or IDE-private node objects as public semantic storage.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
The Tree family provides the standard portable finite hierarchical data baseline of FROG:
</p>

<ul>
  <li><code>frog.widgets.tree_control</code></li>
  <li><code>frog.widgets.tree_indicator</code></li>
</ul>

<p>
It covers node identity, hierarchy, selection, expansion, optional check state, viewport, editing, drag/drop posture, and a stable part model while keeping host-native virtualization and renderer internals downstream from class law.
</p>
