<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 06 UI Package</h1>

<p align="center">
  <strong>Published Boolean front-panel package for the bounded value roundtrip pilot</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory publishes the <code>.wfrog</code> front-panel package for <code>Examples/06_boolean_value_roundtrip/</code>.
The package contains one Boolean control and one Boolean indicator.
</p>

<p>
The package owns layout, initial presentation properties, realization references, and widget-local asset references.
Executable meaning remains owned by the canonical <code>.frog</code> source and downstream runtime acceptance artifacts.
</p>

<hr/>

<h2>Layout</h2>

<pre><code>main_panel: 420 x 150 panel_pixels
bool_input:  x=20,  y=32, width=160, height=80
bool_result: x=240, y=32, width=160, height=80
</code></pre>

<hr/>

<h2>Realization Posture</h2>

<p>
The package references the Default Boolean realization manifest:
</p>

<pre><code>Libraries/Realizations/Default/boolean.default.wfrog</code></pre>

<p>
The package also declares the concrete assets used by its widget instances:
</p>

<pre><code>asset:boolean_rectangular_svg -> Libraries/Realizations/Default/assets/boolean/templates/boolean_rectangular.svg
asset:boolean_circular_svg    -> Libraries/Realizations/Default/assets/boolean/templates/boolean_circular.svg
</code></pre>

<p>
The official Example 06 checker must render those assets or fail validation.
A hardcoded card, CSS switch, native checkbox face, or other handcrafted approximation is not a valid front-panel rendering for this example.
</p>

<hr/>

<h2>Debug Boundary</h2>

<p>
Runtime snapshots and diagnostics may remain available through debug routes such as <code>/state.json</code>.
They must not be shown as raw preformatted content in the normal user-facing front panel by default.
</p>

<hr/>

<h2>Boundary</h2>

<p>
This package is a bounded Example 06 front-panel publication.
It is not a generalized <code>.wfrog</code> renderer and does not introduce new widget classes.
</p>
