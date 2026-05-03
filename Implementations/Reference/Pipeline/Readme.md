<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Pipeline Checks</h1>

<p align="center">
  <strong>Repository-visible checks for the bounded Example 05 reference pipeline</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Reference implementation workspace</a></li>
  <li><a href="../checks.md">Reference workspace checks</a></li>
  <li><a href="../pipeline.md">Reference pipeline overview</a></li>
  <li><a href="../Deriver/Readme.md">Reference Deriver</a></li>
  <li><a href="../Lowerer/Readme.md">Reference Lowerer</a></li>
  <li><a href="../ContractEmitter/Readme.md">Reference ContractEmitter</a></li>
  <li><a href="../Runtime/Readme.md">Reference Runtime</a></li>
  <li><a href="../LLVM/Readme.md">Reference LLVM-oriented path</a></li>
  <li><a href="../../../Examples/05_bounded_ui_accumulator/Freeze.md">Example 05 freeze boundary</a></li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This directory coordinates the current reproducibility checks for the frozen Example 05 corridor.
</p>

<pre><code>main.frog
  -&gt; main.fir.json
  -&gt; main.lowering.json
  -&gt; reference_host_runtime_ui_binding contract
  -&gt; runtime acceptance
  -&gt; LLVM module check
</code></pre>

<hr/>

<h2>Command</h2>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py</code></pre>

<hr/>

<h2>Options</h2>

<ul>
  <li><code>--include-widget-validator</code> — run WidgetValidator before the Example 05 artifact pipeline checks.</li>
  <li><code>--skip-runtime-acceptance</code> — skip the contract-to-runtime acceptance stage.</li>
  <li><code>--skip-llvm</code> — skip the lowering-to-LLVM module reproducibility stage.</li>
  <li><code>--include-llvm-build</code> — also run the LLVM native build proof. Requires <code>clang</code>.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
This pipeline check proves staged artifact reproducibility for Example 05.
It does not claim general compiler coverage.
</p>
