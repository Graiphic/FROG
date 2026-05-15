<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 04 — Stateful Feedback Delay</h1>

<p align="center">
  <strong>Minimal explicit-state corridor using <code>frog.core.delay</code></strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example introduces a valid feedback path through explicit local state.
It receives one public floating-point input, adds it to the previous delayed state, publishes the result, and commits the result as the next delayed state.
</p>

<p>
The feedback cycle is valid only because <code>frog.core.delay</code> provides an explicit initial value and an explicit commit boundary.
The example has no front panel participation and no UI effect; it isolates state and feedback behavior before the bounded UI accumulator in Example 05.
</p>

<hr/>

<h2>What This Example Tests</h2>

<ul>
  <li>explicit feedback through <code>frog.core.delay</code>,</li>
  <li>initial state materialization,</li>
  <li>state-current / state-next lowering posture,</li>
  <li>public input to stateful computation,</li>
  <li>public output publication from a stateful step,</li>
  <li>source-to-FIR derivation for a valid feedback graph,</li>
  <li>FIR-to-lowering projection for a stateful delay kernel,</li>
  <li>backend-contract emission for a reference stateful runtime family,</li>
  <li>generic runtime contract execution,</li>
  <li>LLVM module emission from the lowering artifact,</li>
  <li>optional native build proof when <code>clang</code> is available.</li>
</ul>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/04_stateful_feedback_delay/
├── Readme.md
├── main.frog
├── main.fir.json
└── main.lowering.json

Implementations/Reference/ContractEmitter/examples/
└── 04_stateful_feedback_delay.reference_stateful_feedback_delay.contract.json

Implementations/Reference/Runtime/acceptance/
├── example04_stateful_feedback_delay.acceptance.json
└── example04_stateful_feedback_delay.snapshot.json

Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/
├── module.ll
├── build.sh
└── expected-output.json
</code></pre>

<hr/>

<h2>Source Shape</h2>

<pre><code>interface_input(x) ----\
                       &gt; frog.core.add ---&gt; interface_output(y)
frog.core.delay.out ---/       |
        ^                      |
        \------ delay.in ------/
</code></pre>

<hr/>

<h2>Corridor</h2>

<pre><code>Examples/04_stateful_feedback_delay/main.frog
  -&gt; Examples/04_stateful_feedback_delay/main.fir.json
  -&gt; Examples/04_stateful_feedback_delay/main.lowering.json
  -&gt; Implementations/Reference/ContractEmitter/examples/04_stateful_feedback_delay.reference_stateful_feedback_delay.contract.json
  -&gt; Implementations/Reference/Runtime/acceptance/example04_stateful_feedback_delay.snapshot.json
  -&gt; Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll
</code></pre>

<hr/>

<h2>Reference Checks</h2>

<h3>Full repository-visible corridor</h3>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_10_full.py</code></pre>

<h3>Source to FIR</h3>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/04_stateful_feedback_delay/main.frog \
  --expected Examples/04_stateful_feedback_delay/main.fir.json \
  --check
</code></pre>

<h3>FIR to lowering</h3>

<pre><code>python Implementations/Reference/Lowerer/lower_fir.py \
  --fir Examples/04_stateful_feedback_delay/main.fir.json \
  --expected Examples/04_stateful_feedback_delay/main.lowering.json \
  --check
</code></pre>

<h3>Backend contract</h3>

<pre><code>python Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py \
  --example 04 \
  --check
</code></pre>

<h3>Runtime acceptance</h3>

<pre><code>python Implementations/Reference/Runtime/check_examples01_10_runtime_acceptance.py</code></pre>

<h3>LLVM module</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/04_stateful_feedback_delay/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll \
  --check
</code></pre>

<h3>Optional native LLVM build</h3>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/04_stateful_feedback_delay/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll \
  --check \
  --build \
  --example-dir Implementations/Reference/LLVM/examples/04_stateful_feedback_delay
</code></pre>

<hr/>

<h2>Expected Native Proof Behavior</h2>

<p>
The LLVM proof path emits a small native module that executes one explicit delay step and prints the public output, committed state, and status.
The precise expected native output is published in:
</p>

<pre><code>Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/expected-output.json</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This example proves the bounded explicit-state feedback path across the current reference corridor.
It does not define a general state scheduler, a complete memory model, or a production runtime.
It keeps the feedback rule inspectable before Example 05 combines explicit state with UI binding and bounded iteration.
</p>
