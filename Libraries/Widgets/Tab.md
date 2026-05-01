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
</ul>

<hr/>

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.tab_control</code></li>
  <li><code>frog.widgets.tab_indicator</code></li>
</ul>

<hr/>
<h2>Overview</h2><p>The Tab family defines finite page-navigation widgets. A tab is a composite selector over named pages and a visible page region; it is not a host-private layout trick.</p>

<hr/>

<h2>Page and Selection Model</h2><ul><li><code>pages[]</code></li><li><code>pages[].id</code></li><li><code>pages[].title</code></li><li><code>pages[].enabled</code></li><li><code>selection.index</code></li><li><code>selection.id</code></li><li><code>value</code></li></ul>

<hr/>

<h2>Public Parts</h2><ul><li><code>root</code></li><li><code>label</code></li><li><code>caption</code></li><li><code>frame</code></li><li><code>tab_header_region</code></li><li><code>tab_item</code></li><li><code>selection_face</code></li><li><code>page_region</code></li><li><code>focus_ring</code></li></ul>

<hr/>

<h2>Methods and Events</h2><ul><li><code>select_next()</code></li><li><code>select_previous()</code></li><li><code>set_selected_index(index)</code></li><li><code>set_selected_page(id)</code></li><li><code>focus()</code></li></ul><ul><li><code>selection_changed</code></li><li><code>value_changed</code></li><li><code>page_rendered</code></li></ul>
