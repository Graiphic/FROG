<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 03 — UI Property Write</h1>

<p align="center">
  <strong>Minimal object-style widget interaction corridor using <code>widget_reference</code> and <code>frog.ui.property_write</code></strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example introduces object-style UI interaction.
It receives one public string input, obtains a widget reference, and writes the input value to a public widget member through <code>frog.ui.property_write</code>.
</p>

<p>
It intentionally separates object-style widget interaction from natural <code>widget_value</code> dataflow.
The example has no arithmetic value publication, no explicit state, and no structured control.
Its purpose is to keep UI effects visible as explicit effects rather than implicit side effects hidden inside a host runtime.
</p>

<hr/>

<h2>What This Example Tests</h2>

<ul>
  <li>public string input participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li>explicit <code>frog.ui.property_write</code> effect modeling,</li>
  <li>source-to-FIR derivation of a UI property-write unit,</li>
  <li>FIR-to-lowering projection for a UI effect unit,</li>
  <li>backend-contract emission for a reference UI property-write runtime family,</li>
  <li>generic runtime contract execution with observed UI effect state,</li>
  <li>LLVM module emission from the lowering artifact,</li>
  <li>optional native proof when <code>clang</code> is available.</li>
</ul>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/03_ui_property_write/
├── Readme.md
├── main.frog
├── main.fir.json
└── main.lowering.json

Implementations/Reference/ContractEmitter/examples/
└── 03_ui_property_write.reference_ui_property_write.contract.json

Implementations/Reference/Runtime/acceptance/
├── example03_ui_property_write.acceptance.json
└── example03_ui_property_write.snapshot.json

Implementations/Reference/LLVM/examples/03_ui_property_write/
├── module.ll
├── build.sh
└── expected-output.json
</code></pre>

<hr/>

<h2>Source Shape</h2>

<pre><code>interface_input(status) --- value ---\
                                     frog.ui.property_write(label.text)
widget_reference(ctrl_gain) -- ref -/
</code></pre>

<hr/>

<h2>Corridor</h2>

<pre><code>Examples/03_ui_property_write/main.frog
  -&gt; Examples/03_ui_property_write/main.fir.json
  -&gt; Examples/03_ui_property_write/main.lowering.json
  -&gt; Implementations/Reference/ContractEmitter/examples/03_ui_property_write.reference_ui_property_write.contract.json
  -&gt; Implementations/Reference/Runtime/acceptance/example03_ui_property_write.snapshot.json
  -&gt; Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll
</code></pre>

<hr/>

<h2>Reference Checks</h2>

<h3>Full repository-visible corridor</h3>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py</code></pre>

<h3>Source to FIR</h3>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/03_ui_property_write/main.frog \
  --expected Examples/03_ui_property_write/main.fir.json \
  --check
</code></pre>

<h3>FIR to lowering</h3>

<pre><code>python Implementations/Reference/Lowerer/lower_fir.py \
  --fir Examples/03_ui_property_write/main.fir.json \
  --expected Examples/03_ui_property_write/main.lowering.json \
  --check
</code></pre>

<h3>Backend contract</h3>

<pre><code>python Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py \
  --example 03 \
  --check
</code></pre>

<h3>Runtime acceptance</h3>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py</code></pre>

<h3>LLVM module</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/03_ui_property_write/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll \
  --check
</code></pre>

<h3>Optional native LLVM build</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/03_ui_property_write/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll \
  --check \
  --build \
  --example-dir Implementations/Reference/LLVM/examples/03_ui_property_write
</code></pre>

<hr/>

<h2>Expected Native Proof Behavior</h2>

<p>
The LLVM proof path emits a small native module that prints the affected widget, member, value, and status.
The precise expected native output is published in:
</p>

<pre><code>Implementations/Reference/LLVM/examples/03_ui_property_write/expected-output.json</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This example proves explicit object-style UI effect modeling across the current reference corridor.
It does not define host UI rendering, widget class law, or a complete UI effects runtime.
It only protects the visible handoff from canonical source to FIR, lowering, backend contract, runtime acceptance, and LLVM proof payload.
</p>
