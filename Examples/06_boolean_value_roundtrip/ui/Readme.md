<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 06 UI Package</h1>

<p align="center">
  <strong>Published Boolean front-panel package for the bounded value roundtrip pilot</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory publishes the <code>.wfrog</code> front-panel package for
<code>Examples/06_boolean_value_roundtrip/</code>. The package contains one
Boolean control and one Boolean indicator.
</p>

<p>
Reviewed <time datetime="2026-06-11">2026-06-11</time>.
</p>

<hr/>

<h2>Layout</h2>

<pre><code>main_panel: 360 x 160 panel_pixels
bool_input:  x=56,  y=56, width=72, height=72, bounds_ref=placement_bounds
bool_result: x=224, y=56, width=72, height=72, bounds_ref=placement_bounds
</code></pre>

<hr/>

<h2>Realization Posture</h2>

<p>
The package references the Default Boolean realization manifest:
</p>

<pre><code>Libraries/Realizations/Default/boolean.default.wfrog</code></pre>

<p>
The visible widget bodies must use the declared SVG asset references:
</p>

<pre><code>bool_input.visual.asset_ref  = asset:boolean_rectangular_svg
bool_result.visual.asset_ref = asset:boolean_circular_svg</code></pre>

<p>
The public SVG skin parts are <code>root</code>, <code>label</code>,
<code>caption</code>, <code>placement_bounds</code>, <code>state_face</code>,
<code>state_text</code>, and <code>focus_ring</code>.
</p>

<hr/>

<h2>Boundary</h2>

<p>
This package is a bounded Example 06 front-panel publication. It is not a
generalized <code>.wfrog</code> renderer and does not introduce new Boolean
widget classes.
</p>
