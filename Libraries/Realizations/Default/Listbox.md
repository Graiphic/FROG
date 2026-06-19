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
  <li><code>placement_bounds</code></li>
  <li><code>main_body</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>item_region</code></li>
  <li><code>item_row</code></li>
  <li><code>item_icon</code></li>
  <li><code>item_label</code></li>
  <li><code>selection_face</code></li>
  <li><code>hover_face</code></li>
  <li><code>disabled_face</code></li>
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
  <li><code>assets/listbox/subobjects/hover_face.svg</code></li>
  <li><code>assets/listbox/subobjects/disabled_face.svg</code></li>
  <li><code>assets/listbox/subobjects/vertical_scrollbar.svg</code></li>
  <li><code>assets/listbox/subobjects/horizontal_scrollbar.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>placement_bounds</code> is the Listbox aura used for placement and IDE/container selection.</li>
  <li><code>main_body</code> groups the visible Listbox body, item region, and scrollbars inside the aura.</li>
  <li><code>item_region</code> anchors the visible item list.</li>
  <li><code>item_row</code> anchors each visible item row.</li>
  <li><code>item_icon</code> and <code>item_label</code> anchor optional icon and label rendering.</li>
  <li><code>selection_face</code> renders selected items.</li>
  <li><code>hover_face</code> renders pointer hover.</li>
  <li><code>disabled_face</code> renders disabled-item posture.</li>
  <li><code>vertical_scrollbar</code> renders source-configured viewport posture. The default thumb length policy is the visible-item ratio with a source-owned minimum height and source-owned inner padding.</li>
</ul>

<p>
A runtime may layer host-managed interactive surfaces over the SVG template when those surfaces align to published parts and consume source-owned properties. The Default Listbox realization is not valid when its SVG resource is missing, and a host-native or HTML/CSS-only replacement must not be treated as the Default widget realization.
</p>

<p>
The accepted default visual posture is declared through <code>default_widget_properties</code> in <code>listbox.default.wfrog</code>. Runtime hosts consume those defaults and source-owned overrides; they must not keep accepted Listbox colors, row height, scrollbar styling, symbol styling, selection styling, or focus styling as private runtime constants.
</p>

<p>
Viewport state such as top row, item offset, and reveal-selected behavior is instance-local front-panel state. It must not flow from a control to an indicator unless the source explicitly models that property flow.
</p>

<p>
The scrollbar is part of the default realization, not a runtime fallback shell. Its visibility comes from <code>display.vertical_scrollbar_visible</code>, and its colors, borders, radius, hover/pressed states, thumb minimum height, and inner padding come from <code>style.scrollbar.*</code>. A read-only Listbox indicator may still expose an interactive scrollbar because scrolling only changes its local viewport, not the value published by the diagram.
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
