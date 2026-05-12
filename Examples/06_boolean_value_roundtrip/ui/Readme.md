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
The package owns layout, initial presentation properties, realization references, and asset references.
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
The visible widget bodies must use the declared SVG asset references:
</p>

<pre><code>bool_input.visual.asset_ref  = asset:boolean_rectangular_svg
bool_result.visual.asset_ref = asset:boolean_circular_svg</code></pre>

<p>
The rendered user-facing front panel must not replace these assets with a handcrafted CSS-only Boolean card or toggle.
Fallback rendering may be useful for diagnostics, but it cannot pass official Example 06 validation.
</p>

<hr/>

<h2>Text Ownership</h2>

<p>
For this bounded pilot, the SVG templates expose the public text parts but hide template text by default.
The host overlay owns the visible caption and state text, preventing duplicated text between SVG template text and HTML overlays.
</p>

<hr/>

<h2>Boundary</h2>

<p>
This package is a bounded Example 06 front-panel publication.
It is not a generalized <code>.wfrog</code> renderer and does not introduce new widget classes.
</p>
