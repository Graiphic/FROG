<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Tab Widgets</h1>

<p align="center">
  <strong>Default realization posture for the standardized tab widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Target Classes</h2>

<ul>
  <li><code>frog.widgets.tab_control</code></li>
  <li><code>frog.widgets.tab_indicator</code></li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This default realization publishes a portable tab embodiment for the FROG Tab widget family.
It provides a shell, header region, tab items, icon and label surfaces, selection face, optional close button, overflow button/menu, tab scroll buttons, movable page label display, page region, page container, page content surface, and focus surface.
</p>

<p>
This realization is downstream from widget class law.
It does not define page-content semantics, host-native tab controls, hidden page lifecycle, page editor behavior, or child-widget value ownership.
</p>

<hr/>

<h2>Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>tab_header_region</code></li>
  <li><code>tab_item</code></li>
  <li><code>tab_item_label</code></li>
  <li><code>tab_item_icon</code></li>
  <li><code>tab_close_button</code></li>
  <li><code>selection_face</code></li>
  <li><code>overflow_button</code></li>
  <li><code>overflow_menu</code></li>
  <li><code>tab_scroll_previous</code></li>
  <li><code>tab_scroll_next</code></li>
  <li><code>page_label_display</code></li>
  <li><code>page_label_display_face</code></li>
  <li><code>page_label_display_text</code></li>
  <li><code>page_label_display_previous</code></li>
  <li><code>page_label_display_next</code></li>
  <li><code>page_region</code></li>
  <li><code>page_container</code></li>
  <li><code>page_content</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/tab/templates/tab_shell.svg</code></li>
  <li><code>assets/tab/subobjects/tab_header_region.svg</code></li>
  <li><code>assets/tab/subobjects/tab_item.svg</code></li>
  <li><code>assets/tab/subobjects/selection_face.svg</code></li>
  <li><code>assets/tab/subobjects/page_region.svg</code></li>
  <li><code>assets/tab/subobjects/tab_close_button.svg</code></li>
  <li><code>assets/tab/subobjects/overflow_button.svg</code></li>
  <li><code>assets/tab/subobjects/overflow_menu.svg</code></li>
  <li><code>assets/tab/subobjects/tab_scroll_previous.svg</code></li>
  <li><code>assets/tab/subobjects/tab_scroll_next.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>tab_header_region</code> anchors the tab strip.</li>
  <li><code>tab_item</code> anchors each visible page selector.</li>
  <li><code>tab_item_icon</code> and <code>tab_item_label</code> anchor optional icon and text surfaces.</li>
  <li><code>selection_face</code> renders selected-page state.</li>
  <li><code>overflow_button</code> and <code>overflow_menu</code> anchor hidden-page navigation.</li>
  <li><code>tab_scroll_previous</code> and <code>tab_scroll_next</code> anchor scrollable tab strips.</li>
  <li><code>page_label_display</code> anchors a movable page selector surface.</li>
  <li><code>page_region</code>, <code>page_container</code>, and <code>page_content</code> anchor the selected page host.</li>
</ul>

<p>
The Default Tab realization declares tab item positioning, selected-row
placement, selected-tab join styling, selected-tab join overlap, selected-row
join styling, item sizing, item gaps, row gaps, multi-row wrapping, and overflow
posture as configurable surfaces. A
runtime or IDE may adapt those values from the <code>.frog</code> instance and
the <code>.wfrog</code> declaration; it must not replace them with a private
fixed or overlapping tab strip. Multi-row tab strips are valid only when the
source-owned layout or constrained-width overflow posture requires them; a
single-row layout that fits the published <code>tab_header_region</code> must
remain single-row.
</p>

<p>
A runtime may replace the SVG-backed tab with a host-native tab control when it preserves the public part model and property/method/event surfaces.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>All declared public parts must exist in widget class law.</li>
  <li>Declared SVG resources must resolve relative to this package.</li>
  <li>The declared SVG resources should expose all declared <code>data-frog-part</code> markers.</li>
  <li>Resources must not become semantic owners of page value or child-widget values.</li>
  <li>Host-native tab handles and hidden page renderer handles must remain realization-private.</li>
</ul>
