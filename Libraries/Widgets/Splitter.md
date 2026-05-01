<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Splitter Widgets</h1>

<p align="center">
  <strong>Normative baseline for horizontal and vertical splitter layout widgets</strong><br/>
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
  <li><code>frog.widgets.horizontal_splitter</code></li>
  <li><code>frog.widgets.vertical_splitter</code></li>
</ul>

<hr/>
<h2>Overview</h2><p>Splitter widgets define draggable layout separators between two regions. They are layout widgets, not computational values.</p>

<hr/>

<h2>Property Surface</h2><ul><li><code>orientation</code></li><li><code>position</code></li><li><code>minimum_before</code></li><li><code>minimum_after</code></li><li><code>collapsed_before</code></li><li><code>collapsed_after</code></li><li><code>interaction.draggable</code></li><li><code>style.handle.*</code></li></ul>

<hr/>

<h2>Public Parts</h2><ul><li><code>root</code></li><li><code>separator</code></li><li><code>handle</code></li><li><code>focus_ring</code></li></ul>
