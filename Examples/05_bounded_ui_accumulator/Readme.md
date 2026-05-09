<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 05 — Bounded UI Accumulator</h1>

<p align="center">
  <strong>Bounded LabVIEW-like corridor combining front panel, diagram lowering, native kernel, runtime bridge, and snapshot publication</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example is the primary applicative vertical slice of the current repository state.
It combines standardized numeric widgets, a published front-panel package, natural <code>widget_value</code> binding, object-style <code>frog.ui.property_write</code> effects, bounded iteration, explicit accumulator state, backend-contract emission, runtime-family acceptance, and an LLVM-produced native-kernel bridge.
</p>

<p>
For input <code>3</code>, the bounded accumulator executes five iterations and publishes the expected result <code>15</code>.
</p>

<hr/>

<h2>Bounded LabVIEW-Like Closure</h2>

<p>
The Example 05 closure is intentionally bounded and explicit:
</p>

<pre><code>.frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM-oriented backend artifact kernel.ll
  -&gt; native kernel manifest

.wfrog front panel
  -&gt; panel_pixels layout
  -&gt; numeric SVG skins
  -&gt; label_anchor / value_anchor / value_box overlays

C++ runtime
  -&gt; loads contract and .wfrog
  -&gt; loads native kernel manifest
  -&gt; binds ctrl_input.value to input_value
  -&gt; calls frog_example05_run(...)
  -&gt; publishes result to public output and ind_result.value
  -&gt; emits runtime snapshot and state.json
</code></pre>

<p>
This is a bounded proof that FROG can execute a compiled visual-program kernel through a hosted front-panel runtime.
It is not a claim of a generalized LabVIEW runtime, arbitrary diagram compiler, or complete <code>.wfrog</code> renderer.
</p>

<hr/>

<h2>What This Example Tests</h2>

<ul>
  <li>canonical <code>.frog</code> source with public input / output,</li>
  <li>external front-panel package publication through <code>ui/accumulator_panel.wfrog</code>,</li>
  <li>standardized numeric widget class references,</li>
  <li>natural <code>widget_value</code> participation for input and output widgets,</li>
  <li>object-style <code>frog.ui.property_write</code> effects through widget references,</li>
  <li>bounded loop execution posture,</li>
  <li>explicit accumulator state carried through delay-like semantics,</li>
  <li>source-to-FIR derivation for a bounded stateful UI unit,</li>
  <li>FIR-to-lowering projection for a bounded accumulator kernel with UI bindings,</li>
  <li>backend-contract emission for the reference host runtime UI binding family,</li>
  <li>runtime acceptance with a published snapshot,</li>
  <li>LLVM module emission from the lowering artifact,</li>
  <li>native-kernel manifest publication,</li>
  <li>C++ runtime bridge invocation through the manifest-declared ABI,</li>
  <li>C++ browser-host consumption of the <code>.wfrog</code> panel layout and SVG widget skins.</li>
</ul>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/05_bounded_ui_accumulator/
├── Readme.md
├── Freeze.md
├── main.frog
├── main.fir.json
├── main.lowering.json
├── front_panel.objects.json
└── ui/
    ├── accumulator_panel.wfrog
    └── assets/
        ├── numeric_control.svg
        └── numeric_indicator.svg

Implementations/Reference/ContractEmitter/examples/
└── 05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json

Implementations/Reference/Runtime/acceptance/
├── example05_runtime_family.acceptance.json
└── example05_input_3.snapshot.json

Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/
├── module.ll
├── kernel.ll
├── native_kernel_manifest.json
├── build.sh
└── expected-output.json
</code></pre>

<hr/>

<h2>Front Panel Package</h2>

<p>
The front-panel package is published at:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog</code></pre>

<p>
It declares:
</p>

<ul>
  <li><code>main_panel</code> as a <code>500x170</code> panel in <code>panel_pixels</code>,</li>
  <li><code>ctrl_input</code> as a <code>frog.widgets.numeric_control</code> at <code>x=20</code>, <code>y=24</code>, <code>width=220</code>, <code>height=88</code>,</li>
  <li><code>ind_result</code> as a <code>frog.widgets.numeric_indicator</code> at <code>x=260</code>, <code>y=24</code>, <code>width=220</code>, <code>height=88</code>,</li>
  <li>SVG asset references for both numeric widgets.</li>
</ul>

<p>
The widget package owns the front-panel publication and realization-facing host details.
The executable semantics remain carried through canonical source, FIR, lowering, backend contract, runtime acceptance, and native-kernel bridge artifacts.
</p>

<hr/>

<h2>Execution Shape</h2>

<pre><code>input_value = 3
initial_state = 0
iteration_count = 5

for each iteration:
  state_next = state_current + input_value
  state_current &lt;- state_next

result = state_current
ind_result.value = state_current
</code></pre>

<p>Expected published result:</p>

<pre><code>3 * 5 = 15</code></pre>

<hr/>

<h2>Reference Checks</h2>

<h3>Full repository-visible corridor</h3>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py</code></pre>

<h3>Runtime acceptance</h3>

<pre><code>python Implementations/Reference/Runtime/check_example05_runtime_acceptance.py
python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py</code></pre>

<h3>LLVM module and native proof</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/05_bounded_ui_accumulator/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll \
  --check

python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/05_bounded_ui_accumulator/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll \
  --check \
  --build \
  --example-dir Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator</code></pre>

<h3>Native-kernel runtime bridge</h3>

<pre><code>python Implementations/Reference/Runtime/check_example05_native_kernel_bridge.py
python Implementations/Reference/Runtime/check_example05_cpp_native_kernel_bridge.py
python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This example is the richest current applicative corridor, but it is still bounded.
It does not define a general UI host, a complete widget renderer, a general loop compiler, a production runtime, a complete native backend, or a generalized LabVIEW-compatible runtime.
</p>

<p>
It proves a coherent path through the currently published repository surfaces while preserving the runtime/compiler boundary:
LLVM produces a native kernel artifact and manifest; the runtime consumes a manifest-declared ABI.
</p>
