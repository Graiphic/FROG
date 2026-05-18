<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 19 - Subpanel Embedded Front Panel</h1>

<p align="center">
  <strong>Subpanel host region with a referenced child .frog front panel</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This example validates the Subpanel concept as a post-boundary widget
progression example. The parent <code>.frog</code> owns one
<code>frog.widgets.subpanel</code> instance and a reference to a child
<code>.frog</code>. The child <code>.frog</code> owns the visible widgets
displayed inside the Subpanel.
</p>

<hr/>

<h2>Boundary</h2>

<ul>
  <li><code>.frog</code> owns the parent Subpanel instance, layout, caption, hosted front-panel reference, style overrides, and child source reference.</li>
  <li>The embedded child <code>.frog</code> owns its own Label and Boolean widget instances.</li>
  <li><code>.wfrog</code> resolves the Default Subpanel realization package and SVG asset.</li>
  <li>The Default Subpanel SVG publishes <code>root</code>, <code>label</code>, <code>caption</code>, <code>frame</code>, <code>subpanel_region</code>, and <code>focus_ring</code> parts.</li>
  <li>The runtime must align the embedded child surface to the published <code>subpanel_region</code> part.</li>
</ul>

<hr/>

<h2>Runtime Status</h2>

<p>
Example 19 is outside the current public reference runtime closure. Runtime
implementation continues in <code>Graiphic/FROG-Runtime</code> unless this
example is explicitly promoted later.
</p>
