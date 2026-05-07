<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Listbox Widgets</h1>

<p align="center">
  <strong>Default realization posture for the standardized listbox widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Target Classes</h2>

<ul>
  <li><code>frog.widgets.listbox_control</code></li>
  <li><code>frog.widgets.listbox_indicator</code></li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This default realization publishes a portable listbox embodiment for the FROG Listbox widget family.
It provides a shell, item region, item rows, item icons, item labels, active item, selection face, hover face, disabled face, empty state, search highlight, scrollbars, and focus surface.
</p>

<p>
This realization is downstream from widget class law.
It does not define listbox value semantics, host-native list handles, item renderer lifecycle, or virtualized row storage.
</p>

<hr/>

<h2>Public Parts Realized</h2>

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

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/listbox/templates/listbox_shell.svg</code></li>
  <li><code>assets/listbox/subobjects/item_region.svg</code></li>
  <li><code>assets/listbox/subobjects/item_row.svg</code></li>
  <li><code>assets/listbox/subobjects/selection_face.svg</code></li>
  <li><code>assets/listbox/subobjects/active_item.svg</code></li>
  <li><code>assets/listbox/subobjects/hover_face.svg</code></li>
  <li><code>assets/listbox/subobjects/disabled_face.svg</code></li>
  <li><code>assets/listbox/subobjects/empty_state.svg</code></li>
  <li><code>assets/listbox/subobjects/search_highlight.svg</code></li>
  <li><code>assets/listbox/subobjects/vertical_scrollbar.svg</code></li>
  <li><code>assets/listbox/subobjects/horizontal_scrollbar.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>item_region</code> anchors the visible item list.</li>
  <li><code>item_row</code> anchors each visible item row.</li>
  <li><code>item_icon</code> and <code>item_label</code> anchor optional icon and label rendering.</li>
  <li><code>selection_face</code> renders selected items.</li>
  <li><code>active_item</code> renders keyboard-navigation focus.</li>
  <li><code>hover_face</code> renders pointer hover.</li>
  <li><code>disabled_face</code> renders disabled-item posture.</li>
  <li><code>empty_state</code> renders empty list posture.</li>
  <li><code>search_highlight</code> renders lightweight search match emphasis.</li>
</ul>

<p>
A runtime may replace the SVG-backed list with a host-native virtual list when it preserves the public part model and property/method/event surfaces.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>All declared public parts must exist in widget class law.</li>
  <li>Declared SVG resources must resolve relative to this package.</li>
  <li>The declared SVG resources should expose all declared <code>data-frog-part</code> markers.</li>
  <li>Resources must not become semantic owners of listbox value.</li>
  <li>Host-native item handles and virtualized row handles must remain realization-private.</li>
</ul>
