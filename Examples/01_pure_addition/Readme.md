<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 01 — Pure Addition</h1>

<p align="center">
  <strong>Minimal public-interface arithmetic corridor from canonical source to runtime and LLVM proof</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example is the smallest useful executable FROG program in the repository.
It receives two public floating-point inputs, applies one core arithmetic primitive, and exposes the result through one public floating-point output.
</p>

<p>
It intentionally has no front panel participation, no widget participation, no structure, and no explicit local memory.
Its purpose is to keep the pure dataflow path inspectable before UI, effect, state, and bounded-loop concerns are introduced by later examples.
</p>

<hr/>

<h2>What This Example Tests</h2>

<ul>
  <li>canonical <code>.frog</code> public input / public output structure,</li>
  <li>structural source-to-FIR derivation for a pure arithmetic graph,</li>
  <li>FIR-to-lowering projection for a pure add kernel,</li>
  <li>backend-contract emission for a reference pure dataflow arithmetic family,</li>
  <li>generic runtime contract execution,</li>
  <li>LLVM module emission from the lowering artifact,</li>
  <li>optional native build proof when <code>clang</code> is available.</li>
</ul>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/01_pure_addition/
├── Readme.md
├── main.frog
├── main.fir.json
└── main.lowering.json

Implementations/Reference/ContractEmitter/examples/
└── 01_pure_addition.reference_pure_dataflow_arithmetic.contract.json

Implementations/Reference/Runtime/acceptance/
├── example01_pure_addition.acceptance.json
└── example01_pure_addition.snapshot.json

Implementations/Reference/LLVM/examples/01_pure_addition/
├── module.ll
├── build.sh
└── expected-output.json
</code></pre>

<hr/>

<h2>Source Shape</h2>

<pre><code>interface_input(a) ---\
                       &gt; frog.core.add ---&gt; interface_output(result)
interface_input(b) ---/
</code></pre>

<hr/>

<h2>Corridor</h2>

<pre><code>Examples/01_pure_addition/main.frog
  -&gt; Examples/01_pure_addition/main.fir.json
  -&gt; Examples/01_pure_addition/main.lowering.json
  -&gt; Implementations/Reference/ContractEmitter/examples/01_pure_addition.reference_pure_dataflow_arithmetic.contract.json
  -&gt; Implementations/Reference/Runtime/acceptance/example01_pure_addition.snapshot.json
  -&gt; Implementations/Reference/LLVM/examples/01_pure_addition/module.ll
</code></pre>

<hr/>

<h2>Reference Checks</h2>

<h3>Full repository-visible corridor</h3>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_10_full.py</code></pre>

<h3>Artifact preflight</h3>

<pre><code>python Implementations/Reference/ArtifactChecks/check_examples01_10_artifacts.py</code></pre>

<h3>Source to FIR</h3>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/01_pure_addition/main.frog \
  --expected Examples/01_pure_addition/main.fir.json \
  --check
</code></pre>

<h3>FIR to lowering</h3>

<pre><code>python Implementations/Reference/Lowerer/lower_fir.py \
  --fir Examples/01_pure_addition/main.fir.json \
  --expected Examples/01_pure_addition/main.lowering.json \
  --check
</code></pre>

<h3>Backend contract</h3>

<pre><code>python Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py \
  --example 01 \
  --check
</code></pre>

<h3>Runtime acceptance</h3>

<pre><code>python Implementations/Reference/Runtime/check_examples01_10_runtime_acceptance.py</code></pre>

<h3>LLVM module</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/01_pure_addition/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/01_pure_addition/module.ll \
  --check
</code></pre>

<h3>Optional native LLVM build</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/01_pure_addition/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/01_pure_addition/module.ll \
  --check \
  --build \
  --example-dir Implementations/Reference/LLVM/examples/01_pure_addition
</code></pre>

<hr/>

<h2>Expected Native Proof Behavior</h2>

<p>
The LLVM proof path emits a small native module that computes the floating-point addition and prints the expected result and status.
The precise expected native output is published in:
</p>

<pre><code>Implementations/Reference/LLVM/examples/01_pure_addition/expected-output.json</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This example is a bounded reference corridor, not a complete compiler or runtime definition.
It proves the pure arithmetic path across the current reference workspace while remaining subordinate to the owning specification documents and centralized version-governance surface.
</p>
