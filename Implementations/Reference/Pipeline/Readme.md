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
  <li><a href="../pipeline.md">Reference pipeline overview</a></li>
  <li><a href="../Deriver/Readme.md">Reference Deriver</a></li>
  <li><a href="../Lowerer/Readme.md">Reference Lowerer</a></li>
  <li><a href="../ContractEmitter/Readme.md">Reference ContractEmitter</a></li>
  <li><a href="../../../Examples/05_bounded_ui_accumulator/Freeze.md">Example 05 freeze boundary</a></li>
</ul>

<hr/>

<h2>1. Purpose</h2>

<p>
This directory coordinates the current reproducibility checks for the frozen Example 05 corridor.
</p>

<p>
The goal is to verify the published staged chain:
</p>

<pre><code>main.frog
  -&gt; main.fir.json
  -&gt; main.lowering.json
  -&gt; reference_host_runtime_ui_binding contract
</code></pre>

<p>
This is non-normative implementation workspace material.
It does not define the FROG language.
</p>

<hr/>

<h2>2. Current Check</h2>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py</code></pre>

<p>
The check runs, in order:
</p>

<ul>
  <li><code>Deriver/derive_example05_fir.py --check</code></li>
  <li><code>Lowerer/lower_example05_fir.py --check</code></li>
  <li><code>ContractEmitter/reference_contract_emitter.py --check</code></li>
</ul>

<hr/>

<h2>3. Optional Widget-Layer Check</h2>

<p>
The pipeline check can also run the widget-layer validator first:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py --include-widget-validator</code></pre>

<hr/>

<h2>4. Boundary</h2>

<p>
This pipeline check proves staged artifact reproducibility for Example 05.
It does not claim general compiler coverage.
</p>
