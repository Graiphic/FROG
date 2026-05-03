<h1>Reference Contract Emitter</h1>

<p>Backend-contract emission stage for the non-normative FROG reference implementation.</p>

<p>
  Repository governance and publication state are centralized in
  <a href="../../../Versioning/Readme.md"><code>Versioning/Readme.md</code></a>.
</p>

<hr/>

<h2>Directory navigation</h2>

<pre><code>Implementations/Reference/ContractEmitter/
├── Readme.md
├── __init__.py
├── emit_backend_contract.md
├── emit_backend_contract.py
├── reference_contract_emitter.py
├── responsibilities.md
├── acceptance/
│   └── Readme.md
├── tests/
│   └── test_example05_contract_emission.py
└── examples/
    └── 05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json</code></pre>

<p>
Generated cache directories may appear beside these files in the published tree.
They are not part of the intended source surface of this stage.
</p>

<hr/>

<h2>Role</h2>

<p>
This directory emits backend-family handoff artifacts from published execution-facing artifacts.
For the current published slice, the active target is a single backend family:
</p>

<pre><code>reference_host_runtime_ui_binding</code></pre>

<p>
The emitted contract is downstream from canonical source, FIR, lowering, and the published front-panel package.
It exists to make runtime-family consumption explicit.
It must not redefine source semantics.
</p>

<hr/>

<h2>Current bounded scope</h2>

<p>
The current emitter is intentionally narrow.
It is designed for the Example 05 corridor documented in
<a href="../../../Examples/05_bounded_ui_accumulator/Readme.md"><code>Examples/05_bounded_ui_accumulator/</code></a>.
</p>

<p>
The current code path accepts the published lowering artifact for that slice and emits one contract artifact for the same slice.
It is <strong>not</strong> a general lowering-to-contract compiler for arbitrary programs.
</p>

<hr/>

<h2>Published input and output</h2>

<h3>Input</h3>

<pre><code>Examples/05_bounded_ui_accumulator/main.lowering.json</code></pre>

<h3>Output</h3>

<pre><code>Implementations/Reference/ContractEmitter/examples/
└── 05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json</code></pre>

<hr/>

<h2>Reproducibility check</h2>

<p>
The current acceptance check is:
</p>

<pre><code>python -m Implementations.Reference.ContractEmitter.reference_contract_emitter --check</code></pre>

<p>
This command emits the contract in memory from the published lowering artifact and compares it with the published contract artifact.
Formatting differences are ignored because comparison is performed over canonical JSON data.
</p>

<hr/>

<h2>Emit to an output file</h2>

<pre><code>python -m Implementations.Reference.ContractEmitter.reference_contract_emitter \
  --lowering Examples/05_bounded_ui_accumulator/main.lowering.json \
  --output Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json</code></pre>

<p>
Useful check override:
</p>

<pre><code>python -m Implementations.Reference.ContractEmitter.reference_contract_emitter \
  --lowering Examples/05_bounded_ui_accumulator/main.lowering.json \
  --expected Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json \
  --check</code></pre>

<hr/>

<h2>What the current emitter preserves</h2>

<ul>
  <li>backend family <code>reference_host_runtime_ui_binding</code>,</li>
  <li>source, FIR, lowering, and <code>.wfrog</code> references used to derive the contract,</li>
  <li>one executable unit named <code>main</code>,</li>
  <li>one public input <code>input_value : u16</code>,</li>
  <li>one public output <code>result : u16</code>,</li>
  <li>one control widget binding to <code>ctrl_input</code>,</li>
  <li>one indicator widget binding to <code>ind_result</code>,</li>
  <li>widget-reference support for <code>foreground_color</code> on both widgets,</li>
  <li>two emitted <code>frog.ui.property_write</code> operations for <code>foreground_color</code>,</li>
  <li>one explicit state model based on <code>frog.core.delay</code>,</li>
  <li>one bounded execution model with exactly five iterations,</li>
  <li>one final result publication to both public output and indicator.</li>
</ul>

<hr/>

<h2>Ownership boundary</h2>

<ul>
  <li>This directory <strong>owns</strong> the emission of backend-family handoff artifacts for the current reference slice.</li>
  <li>This directory does <strong>not own</strong> source semantics, FIR semantics, lowering semantics, widget semantics, runtime implementation, or compiler-family implementation.</li>
</ul>

<p>
The contract emitter is a downstream publication stage.
If the emitted contract diverges from the canonical source-owned and execution-facing artifacts, the upstream artifacts win.
</p>

<hr/>

<h2>Relationship to Example 05 and the runtime family</h2>

<pre><code>main.frog
  -&gt; main.fir.json
  -&gt; main.lowering.json
  -&gt; emitted backend contract
  -&gt; runtime-family consumer</code></pre>

<hr/>

<h2>Non-goals</h2>

<ul>
  <li>General backend-contract emission for arbitrary programs.</li>
  <li>Ownership of the front-panel package.</li>
  <li>Ownership of runtime behavior.</li>
  <li>Ownership of LLVM or compiler-family lowering.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
Read this directory as the narrow but explicit handoff stage that turns the published Example 05 lowering artifact into the published
<code>reference_host_runtime_ui_binding</code> contract artifact consumed by the current Python, Rust, and C/C++ runtimes.
</p>
