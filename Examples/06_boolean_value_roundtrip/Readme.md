<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 06 — Boolean Value Roundtrip</h1>

<p align="center">
  <strong>Bounded scalar widget pilot for boolean control-to-indicator value binding</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
Example 06 is the first conservative scalar-widget pilot after the Examples 01–05 executable corridor.
It publishes one boolean control on the left side of the front panel and one boolean indicator on the right side.
The runtime behavior is intentionally minimal: <code>bool_input.value</code> is copied to <code>bool_result.value</code> and to the public output <code>result</code>.
</p>

<p>
This example is runtime/UI-binding only at this stage.
It does not add LLVM coverage, does not broaden the Deriver, Lowerer, or ContractEmitter rule families, and does not weaken Example 05 native-kernel coverage.
</p>

<p>
Example 06 must not pass official validation through a handcrafted HTML approximation.
The visible front panel must be derived from <code>boolean_panel.wfrog</code>, the Boolean widget class law, the Default Boolean realization manifest, and the SVG assets referenced by each widget instance.
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

Implementations/Reference/ContractEmitter/examples/
└── 06_boolean_value_roundtrip.reference_host_runtime_ui_binding.contract.json

Implementations/Reference/Runtime/acceptance/
├── example06_boolean_value_roundtrip.acceptance.json
└── example06_input_true.snapshot.json

Implementations/Reference/Runtime/
└── check_example06_boolean_value_roundtrip.py
</code></pre>

<hr/>

<h2>Front Panel</h2>

<p>
The front panel package is published at:
</p>

<pre><code>Examples/06_boolean_value_roundtrip/ui/boolean_panel.wfrog</code></pre>

<p>
It contains exactly two widget instances:
</p>

<ul>
  <li><code>bool_input</code> — <code>frog.widgets.boolean_control</code>, role <code>control</code>, value type <code>bool</code>, visual asset <code>asset:boolean_rectangular_svg</code>.</li>
  <li><code>bool_result</code> — <code>frog.widgets.boolean_indicator</code>, role <code>indicator</code>, value type <code>bool</code>, visual asset <code>asset:boolean_circular_svg</code>.</li>
</ul>

<p>
The front panel uses <code>panel_pixels</code> as its layout coordinate space.
The checker treats the <code>.wfrog</code> package as layout authority and fails if any declared <code>visual.asset_ref</code> is not rendered by the visible front panel.
</p>

<pre><code>main_panel: 420 x 150 panel_pixels
bool_input:  x=20,  y=32, width=160, height=80
bool_result: x=240, y=32, width=160, height=80
</code></pre>

<hr/>

<h2>Realization Assets</h2>

<p>
The <code>.wfrog</code> package references the Default Boolean realization manifest:
</p>

<pre><code>Libraries/Realizations/Default/boolean.default.wfrog</code></pre>

<p>
The rendered front panel must visibly use the declared SVG assets:
</p>

<pre><code>/asset/boolean_rectangular_svg
/asset/boolean_circular_svg</code></pre>

<p>
The runtime snapshot may provide execution and debug data, but it is not a replacement for the front-panel rendering source.
Raw snapshot output must not appear in the normal front panel by default.
</p>

<hr/>

<h2>Runtime Shape</h2>

<pre><code>bool_input.value
  -> public_input.input_value
  -> public_output.result
  -> bool_result.value
</code></pre>

<p>
For the published acceptance case, <code>input_value = true</code> produces <code>result = true</code> and <code>bool_result.value = true</code>.
</p>

<hr/>

<h2>Check</h2>

<pre><code>python Implementations/Reference/Runtime/check_example06_boolean_value_roundtrip.py</code></pre>

<p>
This check is intentionally bounded.
It validates the source artifact, FIR artifact, lowering artifact, backend contract artifact, <code>.wfrog</code> package, Boolean widget class-law reference, Default Boolean realization manifest reference, realization asset existence, rendered front-panel asset routes, and expected runtime snapshot.
</p>

<hr/>

<h2>Boundary</h2>

<p>
Example 06 does not claim a generated full source-to-FIR-to-lowering corridor yet.
It does not claim LLVM coverage.
It does not claim support by the general C++ runtime.
It is a bounded runtime/UI-binding pilot fixture for the Boolean widget family, subordinate to the published widget class law, the default realization manifest, and the centralized version-governance surface.
</p>
