<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Picture Widgets</h1>

<p align="center">
  <strong>Default realization posture for the standardized picture widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Target Classes</h2>

<ul>
  <li><code>frog.widgets.picture_control</code></li>
  <li><code>frog.widgets.picture_indicator</code></li>
</ul>

<hr/>

<h2>Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>picture_region</code></li>
  <li><code>overlay_region</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/picture/templates/picture_rectangular.svg</code></li>
</ul>

<hr/>

<p>
This realization is downstream from widget class law. It publishes default embodiment assets, part bindings, resource references, host hints, and validation expectations. It does not redefine widget semantics.
</p>

<p>
The Default Picture realization is not an image codec and does not read image
files. It provides the SVG-backed display grammar and public parts that a host
uses after a program has produced an image value, for example through
<code>frog.image.decode_file_rgba8</code>.
</p>
