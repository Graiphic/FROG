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
The package owns layout, initial presentation properties, and references to the default Boolean realization assets.
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
The package references Boolean default realization resources from <code>Libraries/Realizations/Default/boolean.default.wfrog</code>.
These realization resources support the visible body of the widgets, but they do not own Boolean class semantics.
</p>

<hr/>

<h2>Boundary</h2>

<p>
This package is a bounded Example 06 front-panel publication.
It is not a generalized <code>.wfrog</code> renderer and does not introduce new widget classes.
</p>
