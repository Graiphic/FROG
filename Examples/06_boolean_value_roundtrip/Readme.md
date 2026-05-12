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
This example now has a bounded C++ native-kernel closure.
It keeps the standard runtime/UI-binding path, adds an LLVM-produced Boolean kernel artifact behind an explicit manifest, and does not broaden the Deriver, Lowerer, or ContractEmitter rule families.
</p>

<hr/>

<h2>Rendering Requirement</h2>

<p>
Example 06 must not pass validation with a handcrafted HTML approximation of a Boolean widget.
The visible front panel must consume the <code>.wfrog</code> layout, the Default Boolean realization manifest, and the SVG assets declared by the package.
</p>

<p>
The control and indicator widgets declare these asset references:
</p>

<pre><code>bool_input.visual.asset_ref  = asset:boolean_rectangular_svg
bool_result.visual.asset_ref = asset:boolean_circular_svg</code></pre>

<p>
The official checker fails if these assets are not rendered, if the Default Boolean realization manifest does not target the expected classes, if the referenced asset files are missing, or if raw snapshot/debug output is visible in the normal front panel.
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
├── example06_input_false.snapshot.json
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
  <li><code>bool_input</code> — <code>frog.widgets.boolean_control</code>, role <code>control</code>, value type <code>bool</code>.</li>
  <li><code>bool_result</code> — <code>frog.widgets.boolean_indicator</code>, role <code>indicator</code>, value type <code>bool</code>.</li>
</ul>

<p>
The front panel uses <code>panel_pixels</code> as its layout coordinate space.
The published runtime check treats the <code>.wfrog</code> package as layout authority and checks that the rendered front panel does not expose the runtime snapshot by default.
</p>

<hr/>

<h2>Runtime Shape</h2>

<pre><code>bool_input.value
  -> public_input.input_value
  -> public_output.result
  -> bool_result.value
</code></pre>

<p>
For the published acceptance cases, <code>input_value = false</code> produces <code>result = false</code>, and <code>input_value = true</code> produces <code>result = true</code>.
The visible Boolean control is rendered as a clickable control body; clicking it posts the opposite Boolean value.
</p>

<hr/>

<h2>Check</h2>

<pre><code>python Implementations/Reference/Runtime/check_example06_boolean_value_roundtrip.py</code></pre>

<p>
This check is intentionally bounded.
It validates the source artifact, FIR artifact, lowering artifact, backend contract artifact, <code>.wfrog</code> package, Boolean widget class-law reference, Default Boolean realization manifest reference, rendered front-panel posture, asset usage, toggle behavior, expected runtime snapshots, and the published LLVM native-kernel manifest surface.
</p>

<hr/>

<h2>Boundary</h2>

<p>
Example 06 does not claim a generalized scalar-widget runtime, a generalized <code>.wfrog</code> renderer, or a generated Deriver/Lowerer/ContractEmitter corridor.
It is the first small pilot for scalar widget coverage and remains subordinate to the published widget class law, the default realization manifest, and the centralized version-governance surface.
</p>
