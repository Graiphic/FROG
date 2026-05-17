<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Pipeline Checks</h1>

<p align="center">
  <strong>Repository-visible checks for the bounded Examples 01-10 base pipeline</strong><br/>
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
  <li><a href="../../../Examples/Readme.md">Examples index</a></li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This directory coordinates the current reproducibility checks for the published Examples 01-10 corridor.
The default repository-quality posture is to validate the widget layer before checking source derivation, lowering, backend contracts, runtime acceptance, and LLVM proof artifacts.
</p>

<p>
The Examples 01-10 pipeline is the base source/FIR/lowering/contract/runtime
pipeline. It should not be read as the current public reference runtime
boundary. The public reference runtime closure extends through Example
<code>15</code>, with Examples <code>11</code> through <code>15</code>
validated through runtime-family tests and native-kernel bridge checks.
</p>

<pre><code>widget layer validation
  -&gt; main.frog
  -&gt; main.fir.json
  -&gt; main.lowering.json
  -&gt; backend contract
  -&gt; runtime acceptance
  -&gt; LLVM module / native proof
</code></pre>

<hr/>

<h2>Primary Command</h2>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_10_full.py</code></pre>

<p>
This command includes widget-layer validation by default.
</p>

<hr/>

<h2>Example 05 Legacy Corridor Command</h2>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py</code></pre>

<p>
This command keeps a narrow Example 05-only staged check available for focused debugging.
It also includes widget-layer validation by default.
</p>

<hr/>

<h2>Options</h2>

<ul>
  <li><code>--include-llvm-build</code> — also run the LLVM native build proof. Requires <code>clang</code>.</li>
  <li><code>--skip-widget-validator</code> — skip the widget-layer validator for narrow debugging of unrelated pipeline stages.</li>
  <li><code>--skip-runtime-acceptance</code> — available on the Example 05-only pipeline to skip the contract-to-runtime acceptance stage.</li>
  <li><code>--skip-llvm</code> — available on the Example 05-only pipeline to skip the lowering-to-LLVM module reproducibility stage.</li>
</ul>

<hr/>

<h2>Widget-Layer Only Check</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --widget-layer-only</code></pre>

<p>
Use this command when modifying widget class-law documents, Default realization documents, <code>*.default.wfrog</code> manifests, SVG resources, public parts, or realization bindings.
</p>

<hr/>

<h2>Boundary</h2>

<p>
These pipeline checks prove staged artifact reproducibility for the current reference workspace.
They do not claim general compiler coverage and do not define FROG language semantics, widget semantics, runtime semantics, or native backend semantics.
</p>
