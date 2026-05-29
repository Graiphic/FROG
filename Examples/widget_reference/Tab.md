<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Tab Widget Example Reference</h1>

<p align="center">
  <strong>Current page-selection and page-region proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Tab progression is demonstrated by
<code>Examples/20_tab_page_selection</code>. It proves a Tab control whose
selected-page value is patched live in the browser host and published through
an explicit native string proof on <code>Execute</code>.
</p>

<p>
The Tab is a page-navigation widget and a page-region host. It selects one
named page; it is not the semantic owner of child widget values inside pages.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Page ids, page titles, selected page id, and selected index.</li>
  <li>Header placement, tab dimensions, item positions, row policy, and overflow posture.</li>
  <li>Page-label display position, size, and selected-page value.</li>
  <li>Page region layout and content records.</li>
  <li>Colors, borders, active-tab join behavior, fonts, and instance-level visual overrides.</li>
</ul>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Important parts</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>Header</td><td><code>tab_header_region</code>, <code>tab_item</code>, <code>tab_item_label</code></td></tr>
    <tr><td>Selection</td><td><code>selection_face</code>, selected-tab join edge properties</td></tr>
    <tr><td>Page label display</td><td><code>page_label_display</code>, <code>page_label_display_text</code></td></tr>
    <tr><td>Page body</td><td><code>page_region</code>, <code>page_container</code>, <code>page_content</code></td></tr>
    <tr><td>Overflow</td><td><code>overflow_button</code>, <code>overflow_menu</code>, tab scroll parts</td></tr>
  </tbody>
</table>

<p>
When the selected tab is adjacent to the page region, the selected tab should
visually join the page region without drawing a conflicting border between the
tab and its page.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/20_tab_page_selection/ui/tab_panel.wfrog</code></li>
  <li>Default Tab package: <code>Libraries/Realizations/Default/tab.default.wfrog</code></li>
  <li>Default Tab shell: <code>Libraries/Realizations/Default/assets/tab/templates/tab_shell.svg</code></li>
  <li>Default Tab subobjects: <code>Libraries/Realizations/Default/assets/tab/subobjects/*.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>Clicking a tab patches selected-page front-panel state without forcing a full page reload.</li>
  <li><code>Execute</code> publishes the selected page through the manifest-backed native string proof.</li>
  <li>The runtime must consume source-owned page/header/style data and align overlays to Default Tab parts.</li>
  <li>Header wrapping must preserve uniform row heights and must not stack rows above the main header line when width is available.</li>
  <li>Colors, borders, seam behavior, and page-label display geometry must remain configurable.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 20. When
published, it should live under <code>Examples/20_tab_page_selection/reference/</code>
and follow <a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>
