<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Pipeline</h1>

<p align="center">
  <strong>First executable reference pipeline for the non-normative FROG reference implementation workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#current-closure-target">2. Current Closure Target</a></li>
  <li><a href="#published-staged-corridor">3. Published Staged Corridor</a></li>
  <li><a href="#reproducibility-check">4. Reproducibility Check</a></li>
  <li><a href="#stage-boundaries">5. Stage Boundaries</a></li>
  <li><a href="#runtime-acceptance">6. Runtime Acceptance</a></li>
  <li><a href="#runtime-and-llvm-boundary">7. Runtime and LLVM Boundary</a></li>
  <li><a href="#summary">8. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document describes the first executable reference pipeline of the non-normative FROG reference implementation workspace.
</p>

<p>
The current goal is to make the frozen Example 05 corridor reproducible across staged artifacts and runtime acceptance.
</p>

<pre><code>canonical .frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; backend contract
  -&gt; runtime-family acceptance
  -&gt; LLVM-oriented native proof path
</code></pre>

<p>
The reference pipeline remains non-normative.
It consumes the published specification layers and published example artifacts.
It does not define FROG.
</p>

<hr/>

<h2 id="current-closure-target">2. Current Closure Target</h2>

<p>
The current closure target is:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/</code></pre>

<p>
That slice combines front-panel value participation, widget-reference participation, property writes, bounded structured control, explicit local state, public output publication, a backend contract, runtime consumers, and a first LLVM-oriented native proof path.
</p>

<hr/>

<h2 id="published-staged-corridor">3. Published Staged Corridor</h2>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog
  -&gt;
Examples/05_bounded_ui_accumulator/main.fir.json
  -&gt;
Examples/05_bounded_ui_accumulator/main.lowering.json
  -&gt;
Implementations/Reference/ContractEmitter/examples/
  05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json
  -&gt;
Implementations/Reference/Runtime/acceptance/
</code></pre>

<p>
The first four stages are now covered by reference checks:
</p>

<ul>
  <li><code>Deriver/derive_example05_fir.py --check</code></li>
  <li><code>Lowerer/lower_example05_fir.py --check</code></li>
  <li><code>ContractEmitter/reference_contract_emitter.py --check</code></li>
  <li><code>Runtime/check_example05_runtime_acceptance.py</code></li>
</ul>

<hr/>

<h2 id="reproducibility-check">4. Reproducibility Check</h2>

<p>
The current combined check is:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py</code></pre>

<p>
To include the widget layer validation before the staged artifact checks:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py --include-widget-validator</code></pre>

<p>
To stop before runtime acceptance:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py --skip-runtime-acceptance</code></pre>

<hr/>

<h2 id="stage-boundaries">5. Stage Boundaries</h2>

<ul>
  <li><strong>Deriver</strong> consumes canonical source and reproduces the published FIR for Example 05.</li>
  <li><strong>Lowerer</strong> consumes FIR and reproduces the published lowering for Example 05.</li>
  <li><strong>ContractEmitter</strong> consumes lowering and reproduces the published backend contract for Example 05.</li>
  <li><strong>Runtime</strong> consumes the backend contract and published <code>.wfrog</code> package and checks observable acceptance.</li>
  <li><strong>LLVM</strong> consumes a downstream compiler-oriented projection of the bounded kernel.</li>
</ul>

<p>
No stage should silently repair missing semantics from a failed predecessor.
</p>

<hr/>

<h2 id="runtime-acceptance">6. Runtime Acceptance</h2>

<p>
The runtime acceptance stage checks:
</p>

<ul>
  <li>the published runtime acceptance artifact,</li>
  <li>the published backend contract,</li>
  <li>the published Example 05 <code>.wfrog</code> package,</li>
  <li>the headless result for input <code>3</code>,</li>
  <li>the published runtime snapshot,</li>
  <li>the overflow rejection for input <code>20000</code>.</li>
</ul>

<hr/>

<h2 id="runtime-and-llvm-boundary">7. Runtime and LLVM Boundary</h2>

<p>
The runtime-family path and LLVM-oriented path are related but distinct.
</p>

<pre><code>contract
  -&gt; runtime-family consumer
  -&gt; browser-host UI and headless acceptance

lowered kernel
  -&gt; LLVM-oriented native proof
  -&gt; bounded native accumulator executable
</code></pre>

<p>
The current LLVM proof does not compile the full UI natively.
The recommended strategy remains to compile the pure bounded kernel first while keeping UI effects runtime-mediated.
</p>

<hr/>

<h2 id="summary">8. Summary</h2>

<p>
The current reference pipeline makes the frozen Example 05 corridor reproducible through:
</p>

<pre><code>.frog -&gt; FIR -&gt; lowering -&gt; backend contract -&gt; runtime acceptance</code></pre>

<p>
The next closure step is to prepare a generated LLVM-kernel path from the lowered kernel.
</p>
