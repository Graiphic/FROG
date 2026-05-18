<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization - Subpanel Widget</h1>

<p align="center">
  <strong>Default realization posture for the standardized Subpanel widget</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Target Class</h2>

<ul>
  <li><code>frog.widgets.subpanel</code></li>
</ul>

<hr/>

<h2>Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>subpanel_region</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Hosted Surface</h2>

<p>
The Default Subpanel realization publishes a <code>subpanel_region</code> part.
Runtime hosts must align the embedded front-panel surface to this published
part. The visible caption text is supplied by the source-owned
<code>caption.text</code> property.
</p>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/subpanel/templates/subpanel_shell.svg</code></li>
  <li><code>assets/subpanel/subobjects/subpanel_region.svg</code></li>
</ul>

<hr/>

<p>
This realization is downstream from widget class law. It publishes default
embodiment assets, part bindings, resource references, host hints, and
validation expectations. It does not redefine Subpanel semantics.
</p>
