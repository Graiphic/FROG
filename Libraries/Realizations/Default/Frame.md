<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Frame Realization</h1>

<p align="center">
  <strong>Default rectangular realization for the simple Frame support widget</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Default realization index</a></li>
  <li><a href="./frame.default.wfrog">Machine-readable Frame realization manifest</a></li>
  <li><a href="./assets/frame/Readme.md">Frame default realization assets</a></li>
  <li><a href="../../Widgets/Frame.md">Frame widget class law</a></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
The Default Frame realization embodies <code>frog.widgets.frame</code> as a simple rectangular SVG-backed support object.
It publishes only background and border geometry.
</p>

<p>
It deliberately does not publish caption, content region, scrollbar, focus-ring, method, or child-front-panel behavior.
Those concepts belong to other widgets or to ordinary front-panel composition.
</p>

<hr/>

<h2>Manifest</h2>

<pre><code>Libraries/Realizations/Default/frame.default.wfrog</code></pre>

<p>
The manifest defines the Default Frame package, the rectangular variant, the SVG resource, default visual values,
and the property bindings for background and border style.
</p>

<hr/>

<h2>Published Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>background</code></li>
  <li><code>border</code></li>
</ul>

<hr/>

<h2>Default Asset</h2>

<ul>
  <li><code>assets/frame/templates/frame_rectangular.svg</code></li>
</ul>

<p>
The SVG border uses <code>vector-effect="non-scaling-stroke"</code> so the border remains one uniform pixel-based
thickness when the host scales the template to the <code>.frog</code> layout rectangle.
</p>

<hr/>

<h2>Style Bindings</h2>

<ul>
  <li><code>style.background.visible</code></li>
  <li><code>style.background.fill_color</code></li>
  <li><code>style.border.visible</code></li>
  <li><code>style.border.color</code></li>
  <li><code>style.border.width</code></li>
</ul>

<p>
The runtime may apply these bindings as CSS variables, native drawing attributes, or host-native style values.
It must not hardcode Frame visuals.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>The Default Frame SVG must resolve from the <code>.wfrog</code> package.</li>
  <li>The visible Frame must consume the published <code>background</code> and <code>border</code> parts.</li>
  <li>Border thickness must be uniform on all sides at every size.</li>
  <li>The border must be a continuous rectangular stroke, with no dashed, dotted, textured, or side-specific styling.</li>
  <li>Background color, border color, border visibility, and border thickness must be source/default-realization configurable.</li>
  <li>No example-local duplicate SVG skin is valid while this Default realization exists.</li>
</ul>
