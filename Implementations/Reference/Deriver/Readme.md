<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Deriver</h1>

<p align="center">
  <strong>Non-normative reference derivation workspace for generating FIR from supported FROG source slices</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory contains the first non-normative reference deriver for the FROG implementation workspace.
</p>

<p>
The supported source-to-FIR targets now cover the published numbered examples from <code>01</code> through <code>05</code>.
</p>

<hr/>

<h2>Commands</h2>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/01_pure_addition/main.frog \
  --expected Examples/01_pure_addition/main.fir.json \
  --check
</code></pre>

<pre><code>python Implementations/Reference/Deriver/derive_example05_fir.py --check
</code></pre>

<hr/>

<h2>Supported Rule Files</h2>

<ul>
  <li><a href="./rules/example01_fir_derivation.md">Example 01 FIR derivation rules</a></li>
  <li><a href="./rules/example02_fir_derivation.md">Example 02 FIR derivation rules</a></li>
  <li><a href="./rules/example03_fir_derivation.md">Example 03 FIR derivation rules</a></li>
  <li><a href="./rules/example04_fir_derivation.md">Example 04 FIR derivation rules</a></li>
  <li><a href="./rules/example05_fir_derivation.md">Example 05 FIR derivation rules</a></li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
This deriver is downstream from source and validation.
It does not own language semantics, widget law, runtime behavior, lowering, backend contracts, or LLVM behavior.
</p>
