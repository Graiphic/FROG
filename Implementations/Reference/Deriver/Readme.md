<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Deriver</h1>

<p align="center">
  <strong>Non-normative reference derivation workspace for generating FIR from supported FROG source slices</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Reference implementation workspace</a></li>
  <li><a href="../../../IR/Derivation%20rules.md">IR derivation rules</a></li>
  <li><a href="../../../IR/Execution%20IR.md">Execution IR</a></li>
  <li><a href="../../../Examples/01_pure_addition/Readme.md">Example 01</a></li>
  <li><a href="../../../Examples/05_bounded_ui_accumulator/Readme.md">Example 05</a></li>
  <li><a href="./rules/example01_fir_derivation.md">Example 01 FIR derivation rules</a></li>
  <li><a href="./rules/example05_fir_derivation.md">Example 05 FIR derivation rules</a></li>
</ul>

<hr/>

<h2>1. Purpose</h2>

<p>
This directory contains the first non-normative reference deriver for the FROG implementation workspace.
</p>

<p>
The supported derivation targets are intentionally narrow:
</p>

<ul>
  <li><code>Examples/01_pure_addition/main.frog -&gt; main.fir.json</code></li>
  <li><code>Examples/05_bounded_ui_accumulator/main.frog -&gt; main.fir.json</code></li>
</ul>

<p>
This is still not a general FROG compiler.
It is the first step from a one-slice deriver toward a rule-dispatched source-to-FIR reference stage.
</p>

<hr/>

<h2>2. Files</h2>

<ul>
  <li><code>fir_deriver.py</code> — source-graph helpers and supported FIR derivation rules.</li>
  <li><code>derive_fir.py</code> — generic CLI for supported source examples.</li>
  <li><code>derive_example05_fir.py</code> — compatibility wrapper for the frozen Example 05 check.</li>
  <li><code>rules/</code> — documented supported derivation rules.</li>
</ul>

<hr/>

<h2>3. Commands</h2>

<h3>Example 01</h3>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/01_pure_addition/main.frog \
  --expected Examples/01_pure_addition/main.fir.json \
  --check
</code></pre>

<h3>Example 05</h3>

<pre><code>python Implementations/Reference/Deriver/derive_example05_fir.py --check
</code></pre>

<hr/>

<h2>4. Boundary</h2>

<p>
This deriver is downstream from source and validation.
It does not own language semantics, widget law, runtime behavior, lowering, backend contracts, or LLVM behavior.
</p>
