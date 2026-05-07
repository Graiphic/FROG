<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 02 — UI Value Roundtrip</h1>

<p align="center">
  <strong>Minimal front-panel value corridor using <code>widget_value</code> nodes</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example introduces natural front-panel value participation.
It reads two numeric control values through <code>widget_value</code>, applies one arithmetic primitive, and publishes the result back through an indicator <code>widget_value</code> surface.
</p>

<p>
It intentionally has no public interface IO, no object-style widget reference, no explicit UI property write, no structured control, and no explicit local state.
Its role is to keep the ordinary widget-value dataflow path separate from later object-style and stateful examples.
</p>

<hr/>

<h2>What This Example Tests</h2>

<ul>
  <li>inline front-panel source participation,</li>
  <li>natural <code>widget_value</code> control reads,</li>
  <li>natural <code>widget_value</code> indicator publication,</li>
  <li>source-to-FIR derivation with widget value bindings,</li>
  <li>FIR-to-lowering projection for a UI value roundtrip kernel,</li>
  <li>backend-contract emission for a reference UI value runtime family,</li>
  <li>generic runtime contract execution,</li>
  <li>LLVM module emission from the lowering artifact,</li>
  <li>optional native build proof when <code>clang</code> is available.</li>
</ul>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/02_ui_value_roundtrip/
├── Readme.md
├── main.frog
├── main.fir.json
└── main.lowering.json

Implementations/Reference/ContractEmitter/examples/
└── 02_ui_value_roundtrip.reference_ui_value_roundtrip.contract.json

Implementations/Reference/Runtime/acceptance/
├── example02_ui_value_roundtrip.acceptance.json
└── example02_ui_value_roundtrip.snapshot.json

Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/
├── module.ll
├── build.sh
└── expected-output.json
</code></pre>

<hr/>

<h2>Source Shape</h2>

<pre><code>widget_value(ctrl_a) ---\
                         &gt; frog.core.add ---&gt; widget_value(ind_result)
widget_value(ctrl_b) ---/
</code></pre>

<hr/>

<h2>Corridor</h2>

<pre><code>Examples/02_ui_value_roundtrip/main.frog
  -&gt; Examples/02_ui_value_roundtrip/main.fir.json
  -&gt; Examples/02_ui_value_roundtrip/main.lowering.json
  -&gt; Implementations/Reference/ContractEmitter/examples/02_ui_value_roundtrip.reference_ui_value_roundtrip.contract.json
  -&gt; Implementations/Reference/Runtime/acceptance/example02_ui_value_roundtrip.snapshot.json
  -&gt; Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll
</code></pre>

<hr/>

<h2>Reference Checks</h2>

<h3>Full repository-visible corridor</h3>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py</code></pre>

<h3>Source to FIR</h3>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/02_ui_value_roundtrip/main.frog \
  --expected Examples/02_ui_value_roundtrip/main.fir.json \
  --check
</code></pre>

<h3>FIR to lowering</h3>

<pre><code>python Implementations/Reference/Lowerer/lower_fir.py \
  --fir Examples/02_ui_value_roundtrip/main.fir.json \
  --expected Examples/02_ui_value_roundtrip/main.lowering.json \
  --check
</code></pre>

<h3>Backend contract</h3>

<pre><code>python Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py \
  --example 02 \
  --check
</code></pre>

<h3>Runtime acceptance</h3>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py</code></pre>

<h3>LLVM module</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/02_ui_value_roundtrip/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll \
  --check
</code></pre>

<h3>Optional native LLVM build</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/02_ui_value_roundtrip/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll \
  --check \
  --build \
  --example-dir Implementations/Reference/LLVM/examples/02_ui_value_roundtrip
</code></pre>

<hr/>

<h2>Expected Native Proof Behavior</h2>

<p>
The LLVM proof path emits a small native module that computes the widget-value addition and prints the resulting indicator value and status.
The precise expected native output is published in:
</p>

<pre><code>Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/expected-output.json</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This example proves natural widget-value participation across the current reference corridor.
It does not define widget class law, Default realization law, host UI rendering, or a complete UI runtime.
Those surfaces remain owned by the relevant specification and realization layers.
</p>
