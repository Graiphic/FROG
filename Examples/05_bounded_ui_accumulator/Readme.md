<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 05 — Bounded UI Accumulator</h1>

<p align="center">
  <strong>Primary applicative corridor combining UI binding, property writes, bounded iteration, explicit state, runtime acceptance, and LLVM proof</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example is the primary applicative vertical slice of the current repository state.
It combines standardized numeric widgets, a published front-panel package, natural <code>widget_value</code> binding, object-style <code>frog.ui.property_write</code> effects, bounded iteration, explicit accumulator state, backend-contract emission, runtime-family acceptance, and an LLVM-oriented native proof path.
</p>

<p>
For input <code>3</code>, the bounded accumulator executes five iterations and publishes the expected result <code>15</code>.
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
  <li>optional native LLVM build proof when <code>clang</code> is available.</li>
</ul>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/05_bounded_ui_accumulator/
├── Readme.md
├── Freeze.md
├── main.frog
├── main.fir.json
├── main.lowering.json
└── ui/
    └── accumulator_panel.wfrog

Implementations/Reference/ContractEmitter/examples/
└── 05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json

Implementations/Reference/Runtime/acceptance/
├── example05_runtime_family.acceptance.json
└── example05_input_3.snapshot.json

Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/
├── module.ll
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
Its widget posture aligns with the standardized numeric widget family:
</p>

<ul>
  <li><code>ctrl_input</code> / <code>input_value</code> is a <code>frog.widgets.numeric_control</code>,</li>
  <li><code>ind_result</code> / <code>result_value</code> is a <code>frog.widgets.numeric_indicator</code>.</li>
</ul>

<p>
The widget package owns the front-panel publication and realization-facing host details.
The executable semantics remain carried through canonical source, FIR, lowering, backend contract, runtime acceptance, and LLVM proof artifacts.
</p>

<hr/>

<h2>Corridor</h2>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog
  -&gt; Examples/05_bounded_ui_accumulator/main.fir.json
  -&gt; Examples/05_bounded_ui_accumulator/main.lowering.json
  -&gt; Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json
  -&gt; Implementations/Reference/Runtime/acceptance/example05_input_3.snapshot.json
  -&gt; Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll
</code></pre>

<hr/>

<h2>Execution Shape</h2>

<pre><code>input_value = 3
initial_state = 0
iteration_count = 5

for each iteration:
  state_next = state_current + input_value
  state_current &lt;- state_next

result = state_current
result_value = state_current
</code></pre>

<p>
Expected published result:
</p>

<pre><code>3 * 5 = 15</code></pre>

<hr/>

<h2>Reference Checks</h2>

<h3>Full repository-visible corridor</h3>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py</code></pre>

<h3>Source to FIR</h3>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/05_bounded_ui_accumulator/main.frog \
  --expected Examples/05_bounded_ui_accumulator/main.fir.json \
  --check
</code></pre>

<h3>FIR to lowering</h3>

<pre><code>python Implementations/Reference/Lowerer/lower_fir.py \
  --fir Examples/05_bounded_ui_accumulator/main.fir.json \
  --expected Examples/05_bounded_ui_accumulator/main.lowering.json \
  --check
</code></pre>

<h3>Backend contract</h3>

<pre><code>python -m Implementations.Reference.ContractEmitter.reference_contract_emitter \
  --lowering Examples/05_bounded_ui_accumulator/main.lowering.json \
  --expected Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json \
  --check
</code></pre>

<h3>Runtime acceptance</h3>

<pre><code>python Implementations/Reference/Runtime/check_example05_runtime_acceptance.py</code></pre>

<p>
The shared Examples 01–05 runtime check also includes Example 05:
</p>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py</code></pre>

<h3>LLVM module</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/05_bounded_ui_accumulator/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll \
  --check
</code></pre>

<h3>Optional native LLVM build</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/05_bounded_ui_accumulator/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll \
  --check \
  --build \
  --example-dir Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator
</code></pre>

<hr/>

<h2>Expected Runtime Snapshot</h2>

<p>
The published runtime snapshot for input <code>3</code> is:
</p>

<pre><code>Implementations/Reference/Runtime/acceptance/example05_input_3.snapshot.json</code></pre>

<p>
The published native proof expected output is:
</p>

<pre><code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/expected-output.json</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This example is the richest current applicative corridor, but it is still bounded.
It does not define a general UI host, a complete widget renderer, a general loop compiler, a production runtime, or a complete native backend.
It proves a coherent path through the currently published repository surfaces while remaining subordinate to the owning specification documents and centralized version-governance surface.
</p>
