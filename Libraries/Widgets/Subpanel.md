<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Subpanel Widget</h1>

<p align="center">
  <strong>Normative baseline for embedding another front-panel surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
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

<h2>Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.subpanel</code></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
A Subpanel is a front-panel host region that displays another FROG front-panel
surface inside the current front panel. It is the LabVIEW-like concept used
when a parent interface embeds the visible front panel of another program,
component, or compiled instance.
</p>

<p>
FROG does not define a standalone <code>frog.widgets.panel</code> widget as a
default widget family. Simple visual grouping belongs to Decorations, page
selection belongs to Tab, and embedding another front panel belongs to Subpanel.
Existing root <code>front_panel</code> metadata may still carry a
<code>class_ref</code> for historical schema continuity, but that root surface
is not a user-placed Panel widget.
</p>

<hr/>

<h2>Property Surface</h2>

<ul>
  <li><code>caption.text</code></li>
  <li><code>caption.visible</code></li>
  <li><code>caption.anchor.*</code></li>
  <li><code>caption.align.horizontal</code></li>
  <li><code>caption.style.*</code></li>
  <li><code>hosted_front_panel_ref</code></li>
  <li><code>hosted_front_panel_package_ref</code></li>
  <li><code>hosted_front_panel.instance_id</code></li>
  <li><code>layout.clip_overflow</code></li>
  <li><code>style.frame.*</code></li>
  <li><code>style.host_region.*</code></li>
</ul>

<hr/>

<h2>Public Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>subpanel_region</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Ownership Boundary</h2>

<ul>
  <li>The parent <code>.frog</code> owns the Subpanel widget instance, layout, caption, hosted front-panel reference, and visual overrides.</li>
  <li>The hosted <code>.frog</code> owns its own diagram, front-panel widgets, layout, bindings, values, and visual overrides.</li>
  <li>The <code>.wfrog</code> package owns the Default Subpanel realization reference, Default asset ids, supported classes, and host capability declarations.</li>
  <li>The runtime may host the referenced front panel, but it must align the hosted surface to the published <code>subpanel_region</code> part.</li>
</ul>
