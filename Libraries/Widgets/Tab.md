<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Tab Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized tab control and tab indicator widget classes</strong><br/>
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
  <li><a href="../Realizations/Default/Tab.md">Default realization — Tab</a></li>
</ul>

<hr/>

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.tab_control</code></li>
  <li><code>frog.widgets.tab_indicator</code></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
The Tab family defines finite page-navigation widgets in FROG.
A Tab widget is a composite page selector with a visible page region.
It is not a host-private layout trick, and it is not merely a decorative header strip.
</p>

<p>
The Tab family provides a public page model, page identity model, selected-page value posture, header layout posture, page-container posture, overflow and scrolling posture, optional close/reorder posture, and stable public part model.
</p>

<p>
The Tab family is a layout / selection / navigation widget family.
It is especially important for structured front panels because it gives an IDE and a runtime a standard way to model multiple named pages inside one visible region.
</p>

<hr/>

<h2>Common Family Posture</h2>

<ul>
  <li>family: page navigation / container widget family</li>
  <li>primary value: present</li>
  <li>value type: <code>frog.tab_selection</code></li>
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
  <li><code>value</code> — the selected page identity / selected-page value posture,</li>
  <li><code>pages.*</code> — page records and page state,</li>
  <li><code>selection.*</code> — currently selected page,</li>
  <li><code>headers.*</code> — tab-header view posture,</li>
  <li><code>page_region.*</code> — selected-page host posture,</li>
  <li><code>overflow.*</code> — overflow menu / tab-scroll posture,</li>
  <li><code>reorder.*</code> — optional page reordering posture,</li>
  <li><code>close.*</code> — optional close-button posture,</li>
  <li>realization-private page widget handles, host-native tab handles, virtual page caches, IDE-only page editors, and runtime-private page containers.</li>
</ul>

<hr/>

<h2><code>frog.widgets.tab_control</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.tab_control</code></li>
  <li><strong>family:</strong> <code>tab_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.tab_selection</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes for selected page where enabled</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A tab control may allow page selection, page reordering, page closing, keyboard navigation, overflow selection, and page-container focus movement when those capabilities are enabled by the active posture.
</p>

<hr/>

<h2><code>frog.widgets.tab_indicator</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.tab_indicator</code></li>
  <li><strong>family:</strong> <code>tab_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.tab_selection</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no for selected page in the standard portable indicator posture</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A tab indicator is display-oriented.
It may show the selected page and expose read-only page inspection, but portable user-originated selection mutation belongs to <code>frog.widgets.tab_control</code>.
</p>

<hr/>

<h2>Tab Value Model</h2>

<p>
The portable tab value is the selected-page posture, not the entire page content tree.
The baseline value may be represented as:
</p>

<ul>
  <li><code>value.selected_index</code></li>
  <li><code>value.selected_page_id</code></li>
</ul>

<p>
The page list itself is exposed through <code>pages.*</code> and may include page composition references.
</p>

<h3>Page model</h3>

<ul>
  <li><code>pages[].id</code> — stable page identifier,</li>
  <li><code>pages[].title</code>,</li>
  <li><code>pages[].caption</code>,</li>
  <li><code>pages[].icon</code>,</li>
  <li><code>pages[].enabled</code>,</li>
  <li><code>pages[].visible</code>,</li>
  <li><code>pages[].closable</code>,</li>
  <li><code>pages[].reorderable</code>,</li>
  <li><code>pages[].content_ref</code> — reference to the page content container or page host,</li>
  <li><code>pages[].metadata</code>.</li>
</ul>

<h3>Page identity</h3>

<p>
Page identity SHOULD be stable.
Selection, reorder, close, and page content hosting SHOULD use page ids rather than host-private tab handles.
</p>

<hr/>

<h2>Selection Model</h2>

<ul>
  <li><code>selection.index</code></li>
  <li><code>selection.id</code></li>
  <li><code>selection.previous_id</code></li>
  <li><code>selection.previous_index</code></li>
  <li><code>selection.enabled_only</code></li>
  <li><code>selection.wrap_navigation</code></li>
</ul>

<p>
Selection is the primary user-facing value of a tab control.
Changing selection changes which page is active; it does not mutate the hidden page contents.
</p>

<hr/>

<h2>Header and Page Region Model</h2>

<h3>Headers</h3>

<ul>
  <li><code>headers.placement</code> — <code>top</code>, <code>bottom</code>, <code>left</code>, or <code>right</code></li>
  <li><code>headers.item_width_policy</code> — <code>auto</code>, <code>equal</code>, <code>fixed</code>, or <code>content</code></li>
  <li><code>headers.visible</code></li>
  <li><code>headers.scrollable</code></li>
  <li><code>headers.icon_visible</code></li>
  <li><code>headers.close_button_visible</code></li>
</ul>

<h3>Page region</h3>

<ul>
  <li><code>page_region.active_page_id</code></li>
  <li><code>page_region.visible</code></li>
  <li><code>page_region.clip_content</code></li>
  <li><code>page_region.content_padding</code></li>
  <li><code>page_region.content_ref</code></li>
</ul>

<p>
The selected page region is a host surface for child widgets or page content.
It is not the semantic owner of child widget values.
</p>

<hr/>

<h2>Overflow, Reorder, and Close Model</h2>

<h3>Overflow</h3>

<ul>
  <li><code>overflow.enabled</code></li>
  <li><code>overflow.visible</code></li>
  <li><code>overflow.hidden_page_ids</code></li>
  <li><code>overflow.menu_open</code></li>
</ul>

<h3>Reorder</h3>

<ul>
  <li><code>reorder.enabled</code></li>
  <li><code>reorder.drag_page_id</code></li>
  <li><code>reorder.drop_index</code></li>
</ul>

<h3>Close</h3>

<ul>
  <li><code>close.enabled</code></li>
  <li><code>close.requested_page_id</code></li>
  <li><code>close.policy</code> — <code>deny</code>, <code>request</code>, or <code>immediate</code></li>
</ul>

<p>
Close and reorder behavior is optional and must be explicit.
A host must not silently mutate the page model through private UI behavior.
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

<h3>Tab properties</h3>

<ul>
  <li><code>pages.count</code></li>
  <li><code>pages.ids</code></li>
  <li><code>pages.titles</code></li>
  <li><code>pages.enabled</code></li>
  <li><code>pages.visible</code></li>
  <li><code>pages.icon</code></li>
  <li><code>pages.content_ref</code></li>
  <li><code>selection.*</code></li>
  <li><code>headers.*</code></li>
  <li><code>page_region.*</code></li>
  <li><code>overflow.*</code></li>
  <li><code>reorder.*</code></li>
  <li><code>close.*</code></li>
</ul>

<hr/>

<h2>Standard Methods</h2>

<ul>
  <li><code>focus()</code></li>
  <li><code>select_next()</code></li>
  <li><code>select_previous()</code></li>
  <li><code>set_selected_index(index)</code></li>
  <li><code>set_selected_page(id)</code></li>
  <li><code>add_page(page)</code></li>
  <li><code>remove_page(id)</code></li>
  <li><code>rename_page(id, title)</code></li>
  <li><code>set_page_enabled(id, enabled)</code></li>
  <li><code>set_page_visible(id, visible)</code></li>
  <li><code>move_page(id, target_index)</code></li>
  <li><code>open_overflow_menu()</code></li>
  <li><code>close_overflow_menu()</code></li>
  <li><code>request_close_page(id)</code></li>
  <li><code>set_page_content_ref(id, content_ref)</code></li>
</ul>

<hr/>

<h2>Standard Events</h2>

<ul>
  <li><code>value_changed</code></li>
  <li><code>selection_changed</code></li>
  <li><code>selected_page_changed</code></li>
  <li><code>page_rendered</code></li>
  <li><code>page_added</code></li>
  <li><code>page_removed</code></li>
  <li><code>page_renamed</code></li>
  <li><code>page_enabled_changed</code></li>
  <li><code>page_visibility_changed</code></li>
  <li><code>page_reordered</code></li>
  <li><code>overflow_opened</code></li>
  <li><code>overflow_closed</code></li>
  <li><code>close_requested</code></li>
  <li><code>tab_clicked</code></li>
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
  <li><code>page_region</code></li>
  <li><code>page_container</code></li>
  <li><code>page_content</code></li>
  <li><code>focus_ring</code></li>
</ul>

<p>
Page containers and page contents are public composition surfaces.
Host-private tab handles, page renderer caches, hidden page widgets, or IDE-only page editors are not semantic storage.
</p>

<hr/>

<h2>Diagram Interaction Posture</h2>

<p>
The Tab family supports natural value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation where legal, and event observation where legal.
</p>

<p>
Ordinary selected-page value flow should prefer <code>widget_value</code>.
Object-style access should be used for page-management, header, overflow, close, reorder, and page-region interaction.
</p>

<hr/>

<h2>Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-tab-selection <code>value</code> payloads,</li>
  <li>duplicate page identifiers,</li>
  <li>empty page lists when the active posture requires at least one page,</li>
  <li>selection ids or indexes outside the page list,</li>
  <li>selected disabled or hidden page where forbidden,</li>
  <li>invalid page content references,</li>
  <li>invalid header placement,</li>
  <li>invalid close policy,</li>
  <li>unsupported close or reorder operations on indicators,</li>
  <li>attempts to expose host-private tab handles, page renderer caches, hidden page widget handles, or IDE-only page objects as public semantic storage.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
The Tab family provides the standard finite page-navigation and page-container baseline of FROG.
It covers selected page, page identity, headers, overflow, optional close/reorder behavior, page-container composition, and stable public parts while keeping host-private tab widgets and hidden page renderers downstream from class law.
</p>
