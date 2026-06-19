<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Listbox Widget Example Reference</h1>

<p align="center">
  <strong>Current finite-list selected-value proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Listbox progression is demonstrated by
<code>Examples/22_listbox_selection_roundtrip</code>. It proves a finite item
list where user selection patches front-panel state, and <code>Execute</code>
publishes the selected numeric value through the native proof corridor.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Item ids, labels, values, enabled/visible posture, icon posture, and order.</li>
  <li>Selection mode, selected id/index/value, active item, and hover posture when modeled.</li>
  <li>Viewport first item, visible count, scrollbar posture, and auto-reveal behavior.</li>
  <li>Control/indicator roles, labels, layout, font, colors, borders, and selection styling.</li>
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
    <tr><td>Items</td><td><code>item_region</code>, <code>item_row</code>, <code>item_icon</code>, <code>item_label</code></td></tr>
    <tr><td>Placement/body</td><td><code>placement_bounds</code>, <code>main_body</code>, <code>item_region</code></td></tr>
    <tr><td>States</td><td><code>selection_face</code>, <code>hover_face</code>, <code>disabled_face</code></td></tr>
    <tr><td>Viewport</td><td><code>vertical_scrollbar</code>, <code>horizontal_scrollbar</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/22_listbox_selection_roundtrip/ui/listbox_panel.wfrog</code></li>
  <li>Default Listbox package: <code>Libraries/Realizations/Default/listbox.default.wfrog</code></li>
  <li>Default shell: <code>Libraries/Realizations/Default/assets/listbox/templates/listbox_shell.svg</code></li>
  <li>Default subobjects: <code>Libraries/Realizations/Default/assets/listbox/subobjects/*.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>Selection changes must be live front-panel state patches, not full page reloads.</li>
  <li><code>Execute</code> consumes selected value state and updates the read-only Listbox indicator through the native proof.</li>
  <li>Indicator scrollbars may inspect viewport state without mutating value.</li>
  <li>The runtime must consume item records and published Default Listbox parts, not a hardcoded host list.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 22. When
published, it should live under <code>Examples/22_listbox_selection_roundtrip/reference/</code>
and follow <a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>
