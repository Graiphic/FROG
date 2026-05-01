<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Panel Widgets</h1>

<p align="center">
  <strong>Normative baseline for panel and subpanel composition widgets</strong><br/>
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
  <li><code>frog.widgets.panel</code></li>
  <li><code>frog.widgets.subpanel</code></li>
</ul>

<hr/>
<h2>Overview</h2><p>Panel and SubPanel widgets define front-panel composition regions. They may host child widgets or a referenced front-panel surface, but they do not own the semantics of child widgets.</p>

<hr/>

<h2>Classes</h2><ul><li><code>frog.widgets.panel</code></li><li><code>frog.widgets.subpanel</code></li></ul>

<hr/>

<h2>Property Surface</h2><ul><li><code>content_region</code></li><li><code>hosted_front_panel_ref</code></li><li><code>loaded_content_ref</code></li><li><code>layout.clip_overflow</code></li><li><code>layout.scroll_enabled</code></li><li><code>style.frame.*</code></li><li><code>style.background.*</code></li></ul>

<hr/>

<h2>Public Parts</h2><ul><li><code>root</code></li><li><code>label</code></li><li><code>caption</code></li><li><code>frame</code></li><li><code>content_region</code></li><li><code>subpanel_region</code></li><li><code>focus_ring</code></li></ul>
