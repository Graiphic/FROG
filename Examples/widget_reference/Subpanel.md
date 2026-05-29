<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Subpanel Widget Example Reference</h1>

<p align="center">
  <strong>Current embedded front-panel proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Subpanel progression is demonstrated by
<code>Examples/19_subpanel_embedded_front_panel</code>. It proves a parent
front panel that hosts a referenced child <code>.frog</code> front panel inside
a source-owned Subpanel region.
</p>

<p>
Subpanel replaces the earlier vague "Panel" idea. A Subpanel hosts another
front panel; a simple visual rectangle is Frame or Decoration, and page
selection is Tab.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Parent Subpanel widget id, layout rectangle, caption, and style.</li>
  <li>Hosted front-panel reference and hosted package reference.</li>
  <li>Child <code>.frog</code> diagram, front-panel widgets, layout, values, and bindings.</li>
  <li>Parent-to-child host posture, without transferring child ownership to the parent runtime shell.</li>
</ul>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Part</th>
      <th>Purpose</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>frame</code></td><td>Subpanel frame and visual host boundary.</td></tr>
    <tr><td><code>caption</code></td><td>Source-owned caption placement and text styling.</td></tr>
    <tr><td><code>subpanel_region</code></td><td>Host alignment region for the referenced child front panel.</td></tr>
    <tr><td><code>focus_ring</code></td><td>Optional focus posture.</td></tr>
  </tbody>
</table>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/19_subpanel_embedded_front_panel/ui/subpanel_panel.wfrog</code></li>
  <li>Child package: <code>Examples/19_subpanel_embedded_front_panel/embedded/ui/embedded_status_panel.wfrog</code></li>
  <li>Default Subpanel package: <code>Libraries/Realizations/Default/subpanel.default.wfrog</code></li>
  <li>Default Subpanel SVG: <code>Libraries/Realizations/Default/assets/subpanel/templates/subpanel_shell.svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The runtime resolves and renders the child front panel inside the published <code>subpanel_region</code>.</li>
  <li>The parent Subpanel must not flatten the child into a duplicated local SVG or hardcoded HTML block.</li>
  <li>The child <code>.frog</code> remains the owner of its own widget values, layout, and bindings.</li>
  <li>The native manifest proof remains an execution corridor; it does not redefine child-host semantics.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 19. When
published, it should live under
<code>Examples/19_subpanel_embedded_front_panel/reference/</code> and follow
<a href="../example_reference_snapshot_standard.md">the reference snapshot standard</a>.
</p>
