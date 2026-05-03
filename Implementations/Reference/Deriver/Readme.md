<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Deriver</h1>

<p align="center">
  <strong>Non-normative reference derivation workspace for generating FIR from validated FROG source slices</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Reference implementation workspace</a></li>
  <li><a href="../../../IR/Derivation%20rules.md">IR derivation rules</a></li>
  <li><a href="../../../IR/Execution%20IR.md">Execution IR</a></li>
  <li><a href="../../../Examples/05_bounded_ui_accumulator/Readme.md">Example 05</a></li>
  <li><a href="../../../Examples/05_bounded_ui_accumulator/Freeze.md">Example 05 freeze boundary</a></li>
</ul>

<hr/>

<h2>1. Purpose</h2>

<p>
This directory contains the first non-normative reference deriver for the FROG implementation workspace.
</p>

<p>
The first target is intentionally narrow:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog
  -&gt;
Examples/05_bounded_ui_accumulator/main.fir.json
</code></pre>

<p>
This tool does not claim to be a complete FROG compiler.
It exists to make the frozen Example 05 FIR artifact reproducible from the canonical <code>.frog</code> source.
</p>

<hr/>

<h2>2. Current Scope</h2>

<ul>
  <li>loads one canonical JSON <code>.frog</code> source file,</li>
  <li>checks the minimal source shape needed for Example 05,</li>
  <li>derives the bounded FIR shape currently published for Example 05,</li>
  <li>preserves the key corridor distinctions: public interface, widget value, widget reference, property write, explicit state, structured loop, and publications,</li>
  <li>can compare the generated FIR against the published FIR artifact.</li>
</ul>

<hr/>

<h2>3. Run</h2>

<pre><code>python Implementations/Reference/Deriver/derive_example05_fir.py --check
</code></pre>

<p>
To write a generated FIR artifact elsewhere:
</p>

<pre><code>python Implementations/Reference/Deriver/derive_example05_fir.py \
  --source Examples/05_bounded_ui_accumulator/main.frog \
  --output build/generated/main.fir.json
</code></pre>

<hr/>

<h2>4. Boundary</h2>

<p>
This deriver is downstream from source and validation.
It does not own language semantics, widget law, runtime behavior, lowering, backend contracts, or LLVM behavior.
</p>

<pre><code>.frog source
  -&gt; validation / accepted meaning
  -&gt; Deriver
  -&gt; FIR
</code></pre>

<hr/>

<h2>5. Next Step</h2>

<p>
After this deriver is green for Example 05, the next stage is:
</p>

<pre><code>main.fir.json
  -&gt;
main.lowering.json
</code></pre>

<p>
That should be implemented separately under <code>Implementations/Reference/Lowerer/</code>.
</p>
