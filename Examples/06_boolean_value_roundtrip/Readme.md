<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 06 - Boolean Value Roundtrip</h1>

<p align="center">
  <strong>Bounded scalar widget pilot for Boolean control-to-indicator value binding</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
Example 06 publishes one Boolean control and one Boolean indicator.
The runtime behavior is intentionally minimal:
<code>bool_input.value</code> is copied to <code>bool_result.value</code> and
to the public output <code>result</code>.
</p>

<p>
Reviewed <time datetime="2026-06-11">2026-06-11</time>: the Boolean example now
uses the minimal Default Boolean SVG contract:
<code>root</code>, <code>label</code>, <code>caption</code>,
<code>placement_bounds</code>, <code>state_face</code>,
<code>state_text</code>, and <code>focus_ring</code>.
</p>

<hr/>

<h2>Rendering Requirement</h2>

<p>
Example 06 must not pass validation with a handcrafted HTML approximation of a
Boolean widget. The visible front panel must consume the <code>.wfrog</code>
layout and the Default Boolean SVG assets declared by the package.
</p>

<pre><code>bool_input.visual.asset_ref  = asset:boolean_rectangular_svg
bool_result.visual.asset_ref = asset:boolean_circular_svg</code></pre>

<p>
The square/rectangular and round/oval appearances are skins of the same Boolean
widget family, not separate widget classes.
</p>

<hr/>

<h2>Front Panel</h2>

<pre><code>main_panel: 360 x 160 panel_pixels
bool_input:  x=56,  y=56, width=72, height=72, bounds_ref=placement_bounds
bool_result: x=224, y=56, width=72, height=72, bounds_ref=placement_bounds
</code></pre>

<hr/>

<h2>Reference Snapshot</h2>

<p>
The accepted C++ browser-host reference package is published under
<a href="./reference/">reference/</a>. It contains the accepted screenshot,
runtime state snapshot, visual contract, and artifact hash index for the source,
lowering, native manifest, and Default Boolean realization assets.
</p>

<p>
The layout coordinates refer to each widget's <code>placement_bounds</code>
aura. The runtime may render the caption outside that aura, but the value body
is still fitted through the source-owned Boolean SVG skin.
</p>

<hr/>

<h2>Runtime Shape</h2>

<pre><code>bool_input.value
  -> public_input.input_value
  -> Execute
  -> public_output.result
  -> bool_result.value
</code></pre>

<p>
Clicking the Boolean control changes the local control value only. The diagram
is executed when the host posts the current <code>input_value</code> through
the explicit <code>Execute</code> action. The published C++ validation keeps
the source path visible as <code>.frog -> FIR -> lowering -> reference
runtime</code>, with the native Boolean LLVM bridge covered by the Slice06
LLVM test.
</p>

<p>
For the published acceptance cases, <code>input_value = false</code> produces
<code>result = false</code>, and <code>input_value = true</code> produces
<code>result = true</code>.
</p>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/06_boolean_value_roundtrip/
├── Readme.md
├── main.frog
├── main.fir.json
├── main.lowering.json
└── ui/
    ├── Readme.md
    └── boolean_panel.wfrog
</code></pre>
