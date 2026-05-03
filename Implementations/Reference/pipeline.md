<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Pipeline</h1>

<p align="center">
  <strong>First executable reference pipeline for the non-normative FROG reference implementation workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
The current reference pipeline makes the frozen Example 05 corridor reproducible across staged artifacts, runtime acceptance, and a first LLVM module emission check.
</p>

<pre><code>canonical .frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; backend contract
  -&gt; runtime-family acceptance
  -&gt; LLVM module check
</code></pre>

<p>
The reference pipeline remains non-normative.
It consumes published artifacts and does not define FROG.
</p>

<hr/>

<h2>Current Check</h2>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py</code></pre>

<p>
The default check runs:
</p>

<ul>
  <li><code>Deriver/derive_example05_fir.py --check</code></li>
  <li><code>Lowerer/lower_example05_fir.py --check</code></li>
  <li><code>ContractEmitter/reference_contract_emitter.py --check</code></li>
  <li><code>Runtime/check_example05_runtime_acceptance.py</code></li>
  <li><code>LLVM/tools/emit_llvm_module.py --check</code></li>
</ul>

<hr/>

<h2>Options</h2>

<ul>
  <li><code>--include-widget-validator</code> — run widget-layer validation first.</li>
  <li><code>--skip-runtime-acceptance</code> — stop before runtime acceptance.</li>
  <li><code>--skip-llvm</code> — stop before LLVM module emission.</li>
  <li><code>--include-llvm-build</code> — also run the native <code>build.sh</code> check; requires <code>clang</code>.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
The LLVM check verifies that <code>main.lowering.json</code> can reproduce the published <code>module.ll</code>.
It does not claim that FROG IR is LLVM IR, and it does not compile the rendered front panel natively.
</p>

<hr/>

<h2>Summary</h2>

<p>
The current pipeline now verifies:
</p>

<pre><code>.frog -&gt; FIR -&gt; lowering -&gt; backend contract -&gt; runtime acceptance -&gt; LLVM module</code></pre>

<p>
The next step is not to add more stages, but to execute the full pipeline and fix any concrete failures.
</p>
