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
</ul>

<hr/>

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.tree_control</code></li>
  <li><code>frog.widgets.tree_indicator</code></li>
</ul>

<hr/>
<h2>Overview</h2><p>The Tree family defines standardized widgets for finite hierarchical node structure. It provides a bounded class law and keeps virtualization, host-native rendering, and advanced editing downstream from the public widget contract.</p>

<hr/>

<h2>Core Value Model</h2><ul><li><code>value</code></li><li><code>selection.*</code> where selection is supported</li><li><code>viewport.*</code> where scrolling or visible windows are supported</li><li><code>items[]</code>, <code>rows</code>, <code>columns</code>, or <code>nodes[]</code> according to the family.</li></ul>

<hr/>

<h2>Public Parts</h2><ul><li><code>root</code></li><li><code>label</code></li><li><code>caption</code></li><li><code>frame</code></li><li><code>node_region</code></li><li><code>node_row</code></li><li><code>selection_face</code></li><li><code>expander_face</code></li><li><code>vertical_scrollbar</code></li><li><code>focus_ring</code></li></ul>

<hr/>

<h2>Methods and Events</h2><ul><li><code>focus()</code></li><li><code>clear_selection()</code></li><li><code>set_selection(...)</code></li><li><code>scroll_to(...)</code></li></ul><ul><li><code>selection_changed</code></li><li><code>value_changed</code></li><li><code>viewport_changed</code></li></ul>

<hr/>

<h2>Validation Expectations</h2><p>Validators SHOULD diagnose inconsistent selection, invalid viewport posture, unsupported class members, and attempts to treat realization-only rows, cells, or node handles as semantic storage.</p>
