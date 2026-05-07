<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Listbox Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized listbox control and listbox indicator widget classes</strong><br/>
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
  <li><a href="../Realizations/Default/Listbox.md">Default realization — Listbox</a></li>
</ul>

<hr/>

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.listbox_control</code></li>
  <li><code>frog.widgets.listbox_indicator</code></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
The Listbox family defines standardized widgets for finite visible item lists in FROG.
It provides a public item model, item identity model, single and multiple selection posture, active item posture, viewport and scrolling posture, optional icons, disabled items, hover state, search/filter posture, and stable public part model.
</p>

<p>
A Listbox widget is suitable for selecting values, choosing named options, presenting a finite list of records, navigating lightweight UI choices, and displaying selected or selectable lists in front panels.
</p>

<p>
The intrinsic Listbox baseline is not a table, tree, combo box, virtualized database view, file browser, or arbitrary item-template engine.
Those integrations may use listbox-like behavior, but they remain downstream from this widget class law unless explicitly standardized elsewhere.
</p>

<hr/>

<h2>Common Family Posture</h2>

<ul>
  <li>family: item-list selection / navigation widget family</li>
  <li>primary value: present</li>
  <li>value type: <code>frog.listbox_selection</code></li>
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
  <li><code>value</code> — selected item id, selected index, or selected item set according to selection mode,</li>
  <li><code>items.*</code> — item list and item metadata,</li>
  <li><code>selection.*</code> — current selection posture,</li>
  <li><code>active.*</code> — active item / keyboard focus posture,</li>
  <li><code>hover.*</code> — pointer hover posture,</li>
  <li><code>viewport.*</code> — visible item window and scrolling posture,</li>
  <li><code>search.*</code> and <code>filter.*</code> — optional search/filter view posture,</li>
  <li>realization-private item renderers, virtualized item handles, host-native list handles, row caches, and IDE-private item editors.</li>
</ul>

<hr/>

<h2><code>frog.widgets.listbox_control</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.listbox_control</code></li>
  <li><strong>family:</strong> <code>listbox_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.listbox_selection</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes for selection where enabled</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A listbox control may allow selection, multi-selection, keyboard navigation, scrolling, search/filter selection, item activation, and optional lightweight item mutation when those capabilities are enabled by the active posture.
</p>

<hr/>

<h2><code>frog.widgets.listbox_indicator</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.listbox_indicator</code></li>
  <li><strong>family:</strong> <code>listbox_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.listbox_selection</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no for selection in the standard portable indicator posture</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A listbox indicator is display-oriented.
It may show the current selection and support viewport inspection, but portable user-originated selection mutation belongs to <code>frog.widgets.listbox_control</code>.
</p>

<hr/>

<h2>Listbox Value Model</h2>

<p>
The portable listbox value is the selected item posture.
Depending on <code>selection.mode</code>, value may be represented as:
</p>

<ul>
  <li><code>value.selected_index</code></li>
  <li><code>value.selected_id</code></li>
  <li><code>value.selected_value</code></li>
  <li><code>value.selected_indexes</code></li>
  <li><code>value.selected_ids</code></li>
  <li><code>value.selected_values</code></li>
</ul>

<p>
The item list itself is exposed through <code>items.*</code>.
The widget does not make realization-private item rows the semantic owner of item data.
</p>

<h3>Item model</h3>

<p>
Each item may expose:
</p>

<ul>
  <li><code>items[].id</code> — stable item identifier,</li>
  <li><code>items[].label</code>,</li>
  <li><code>items[].value</code>,</li>
  <li><code>items[].icon</code>,</li>
  <li><code>items[].enabled</code>,</li>
  <li><code>items[].visible</code>,</li>
  <li><code>items[].selected</code>,</li>
  <li><code>items[].tooltip</code>,</li>
  <li><code>items[].metadata</code>.</li>
</ul>

<h3>Item identity</h3>

<p>
Item identity SHOULD be stable when selection, search, filtering, or programmatic updates are used.
Selection SHOULD prefer item ids where stable ids exist.
Index-based selection remains valid for compact lists, but it is sensitive to sorting or filtering.
</p>

<hr/>

<h2>Selection Model</h2>

<ul>
  <li><code>selection.mode</code> — <code>none</code>, <code>single</code>, <code>multiple</code>, or <code>range</code></li>
  <li><code>selection.selected_index</code></li>
  <li><code>selection.selected_id</code></li>
  <li><code>selection.selected_indexes</code></li>
  <li><code>selection.selected_ids</code></li>
  <li><code>selection.anchor_index</code></li>
  <li><code>selection.active_index</code></li>
  <li><code>selection.require_enabled_item</code></li>
</ul>

<p>
Selection is distinct from item data.
Changing selection does not mutate item labels, values, or metadata.
</p>

<hr/>

<h2>Active, Hover, and Activation Model</h2>

<ul>
  <li><code>active.index</code></li>
  <li><code>active.id</code></li>
  <li><code>hover.index</code></li>
  <li><code>hover.id</code></li>
  <li><code>activation.double_click_enabled</code></li>
  <li><code>activation.enter_key_enabled</code></li>
</ul>

<p>
The active item is the keyboard-navigation focus within the listbox.
It may differ from the selected item in multi-selection or focus-only navigation modes.
</p>

<hr/>

<h2>Viewport, Search, and Filter Model</h2>

<ul>
  <li><code>viewport.item_offset</code></li>
  <li><code>viewport.visible_item_count</code></li>
  <li><code>viewport.row_height</code></li>
  <li><code>viewport.horizontal_offset</code></li>
</ul>

<ul>
  <li><code>search.query</code></li>
  <li><code>search.matches</code></li>
  <li><code>search.active_match</code></li>
  <li><code>filter.enabled</code></li>
  <li><code>filter.expression</code></li>
  <li><code>filter.visible_item_count</code></li>
</ul>

<p>
Search and filtering are view postures unless explicitly modeled as value transformations elsewhere.
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

<h3>Listbox properties</h3>

<ul>
  <li><code>items.count</code></li>
  <li><code>items.ids</code></li>
  <li><code>items.labels</code></li>
  <li><code>items.values</code></li>
  <li><code>items.icons</code></li>
  <li><code>items.enabled</code></li>
  <li><code>items.visible</code></li>
  <li><code>selection.*</code></li>
  <li><code>active.*</code></li>
  <li><code>hover.*</code></li>
  <li><code>viewport.*</code></li>
  <li><code>search.*</code></li>
  <li><code>filter.*</code></li>
  <li><code>empty_state.*</code></li>
</ul>

<hr/>

<h2>Standard Methods</h2>

<ul>
  <li><code>focus()</code></li>
  <li><code>clear_selection()</code></li>
  <li><code>set_selection(selection)</code></li>
  <li><code>select_index(index)</code></li>
  <li><code>select_id(id)</code></li>
  <li><code>select_next()</code></li>
  <li><code>select_previous()</code></li>
  <li><code>scroll_to(index)</code></li>
  <li><code>scroll_to_id(id)</code></li>
  <li><code>set_active_index(index)</code></li>
  <li><code>set_active_id(id)</code></li>
  <li><code>add_item(item)</code></li>
  <li><code>remove_item(id)</code></li>
  <li><code>clear_items()</code></li>
  <li><code>set_item_enabled(id, enabled)</code></li>
  <li><code>set_item_visible(id, visible)</code></li>
  <li><code>find_items(query)</code></li>
  <li><code>clear_filter()</code></li>
</ul>

<hr/>

<h2>Standard Events</h2>

<ul>
  <li><code>value_changed</code></li>
  <li><code>selection_changed</code></li>
  <li><code>active_item_changed</code></li>
  <li><code>item_activated</code></li>
  <li><code>item_clicked</code></li>
  <li><code>item_double_clicked</code></li>
  <li><code>item_added</code></li>
  <li><code>item_removed</code></li>
  <li><code>item_enabled_changed</code></li>
  <li><code>item_visibility_changed</code></li>
  <li><code>viewport_changed</code></li>
  <li><code>search_changed</code></li>
  <li><code>filter_changed</code></li>
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
  <li><code>item_region</code></li>
  <li><code>item_row</code></li>
  <li><code>item_icon</code></li>
  <li><code>item_label</code></li>
  <li><code>active_item</code></li>
  <li><code>selection_face</code></li>
  <li><code>hover_face</code></li>
  <li><code>disabled_face</code></li>
  <li><code>empty_state</code></li>
  <li><code>search_highlight</code></li>
  <li><code>vertical_scrollbar</code></li>
  <li><code>horizontal_scrollbar</code></li>
  <li><code>focus_ring</code></li>
</ul>

<p>
Item rows, icons, labels, selection and hover faces are public part categories.
Realization-private row pools, item renderer handles, virtualized row caches, or host-native list handles remain outside the public class surface.
</p>

<hr/>

<h2>Diagram Interaction Posture</h2>

<p>
The Listbox family supports:
</p>

<ul>
  <li>natural value participation through <code>widget_value</code>,</li>
  <li>property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>,</li>
  <li>method invocation where legal,</li>
  <li>event observation where legal.</li>
</ul>

<p>
Ordinary selection value flow should prefer <code>widget_value</code>.
Object-style access should be used for item-management, active item, viewport, search, filter, and display posture.
</p>

<hr/>

<h2>Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-listbox-selection <code>value</code> payloads,</li>
  <li>duplicate item identifiers,</li>
  <li>selection ids outside the item list,</li>
  <li>selection indexes outside the item list,</li>
  <li>selected disabled items where forbidden,</li>
  <li>invalid active item,</li>
  <li>invalid viewport offset or visible item count,</li>
  <li>unsupported multi-selection on indicator-only classes,</li>
  <li>unsupported item mutation on indicators,</li>
  <li>attempts to expose host-native item handles, virtualized rows, row renderer handles, or IDE-private item objects as public semantic storage.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
The Listbox family provides the standard portable finite item-list selection baseline of FROG:
</p>

<ul>
  <li><code>frog.widgets.listbox_control</code></li>
  <li><code>frog.widgets.listbox_indicator</code></li>
</ul>

<p>
It covers item identity, item labels and values, single/multiple/range selection, active item, hover, viewport, search/filter posture, and a stable public part model while keeping host-native virtualization and renderer internals downstream from class law.
</p>
