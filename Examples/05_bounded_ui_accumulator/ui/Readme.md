<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 05 UI Package</h1>

<p align="center">
  <strong>Published front-panel package and SVG realization assets for the bounded UI accumulator example</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory contains the published UI package artifacts for the <code>Examples/05_bounded_ui_accumulator/</code> bounded vertical slice.
It makes the front-panel package layer visible separately from canonical program source, execution-facing FIR and lowering artifacts, backend-family contract emission, and runtime-family consumption.
</p>

<p>
The important architectural rule is that this directory participates in the example publication corridor, but it does not redefine executable semantics.
</p>

<hr/>

<h2>Directory Shape</h2>

<pre><code>Examples/05_bounded_ui_accumulator/ui/
├── Readme.md
└── accumulator_panel.wfrog

Libraries/Realizations/Default/assets/numeric/templates/
└── numeric_rectangular.svg
</code></pre>

<hr/>

<h2>Package Role</h2>

<p>
The <code>.wfrog</code> file is the example-local published front-panel package artifact.
It owns the Example 05 panel identity, widget placement, initial presentation properties, host binding reference, and SVG asset references.
</p>

<pre><code>canonical .frog source
  -&gt; executable program meaning

accumulator_panel.wfrog
  -&gt; front-panel package publication
  -&gt; widget layout and SVG realization references

runtime
  -&gt; consumes contract + .wfrog
  -&gt; hosts UI
  -&gt; binds widget value to public input/output
</code></pre>

<hr/>

<h2>Example 05 Layout Contract</h2>

<p>
The package uses <code>panel_pixels</code> as its coordinate space.
The current Example 05 front panel is:
</p>

<pre><code>main_panel:
  width: 500
  height: 128
  canvas.grid.visible: false
  canvas.grid.pitch: 16
  canvas.grid.snap: omitted for execution view
  canvas.presentation_mode: execution

ctrl_input:
  class_ref: frog.widgets.numeric_control
  x: 32
  y: 32
  width: 96
  height: 32
  display.increment_buttons_side: right
  visual.asset_ref: asset:numeric_rectangular_svg

ind_result:
  class_ref: frog.widgets.numeric_indicator
  x: 288
  y: 32
  width: 96
  height: 32
  style.value_face.fill_color: #E8EBED
  visual.asset_ref: asset:numeric_rectangular_svg
</code></pre>

<p>
The panel uses the shared Default Numeric realization asset, <code>Libraries/Realizations/Default/assets/numeric/templates/numeric_rectangular.svg</code>.
The runtime scales that official realization into the <code>.wfrog</code> widget bounds and uses its published anchors for dynamic overlays.
</p>

<hr/>

<h2>SVG Asset Role</h2>

<p>
The SVG asset is the shared Default Numeric rectangular realization:
</p>

<ul>
  <li><code>Libraries/Realizations/Default/assets/numeric/templates/numeric_rectangular.svg</code></li>
</ul>

<p>
Each SVG includes these realization markers:
</p>

<ul>
  <li><code>caption_text</code></li>
  <li><code>placement_bounds</code></li>
  <li><code>control_body</code></li>
  <li><code>indicator_body</code></li>
  <li><code>value_face</code></li>
  <li><code>text_value</code></li>
  <li><code>increment_up</code></li>
  <li><code>increment_down</code></li>
  <li><code>focus_ring</code></li>
</ul>

<p>
The browser-host runtimes treat the SVG skin as the widget body and overlay only dynamic value and interaction surfaces at these published markers.
The native HTML numeric input and step buttons remain browser-host implementation details bound to the Default Numeric realization parts.
</p>

<hr/>

<h2>Runtime Consumption</h2>

<p>
The C++ browser-host runtime consumes this package by:
</p>

<ul>
  <li>loading the package from <code>accumulator_panel.wfrog</code>,</li>
  <li>using panel <code>width</code>, <code>height</code>, and <code>coordinate_space</code>,</li>
  <li>placing each widget using <code>x</code>, <code>y</code>, <code>width</code>, and <code>height</code>,</li>
  <li>resolving <code>visual.asset_ref</code> to an SVG asset route,</li>
  <li>using <code>placement_bounds</code>, <code>caption_text</code>, <code>value_face</code>, <code>text_value</code>, <code>increment_up</code>, and <code>increment_down</code> for placement and overlays,</li>
  <li>applying contract-driven property writes such as <code>foreground_color</code>,</li>
  <li>binding <code>ctrl_input.value</code> to <code>input_value</code>,</li>
  <li>publishing <code>result</code> to <code>ind_result.value</code>.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
This directory does not define FROG itself.
It does not define the language, FIR, lowering, backend contract law, or widget class law.
</p>

<pre><code>published UI package
    !=
semantic authority

SVG realization asset
    !=
widget class definition
</code></pre>

<p>
The correct reading posture is:
</p>

<ul>
  <li><code>main.frog</code> owns example-local executable meaning,</li>
  <li><code>accumulator_panel.wfrog</code> owns example-local front-panel publication,</li>
  <li>the SVG files support current Example 05 realization,</li>
  <li>downstream hosts and runtimes consume these artifacts without acquiring semantic authority.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
This directory contains the published front-panel package and the minimal SVG realization assets for Example 05.
It exists to make the widget-oriented package layer visible in the repository while preserving strict separation from canonical source-owned program meaning.
</p>
