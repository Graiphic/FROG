<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Lowerer</h1>

<p align="center">
  <strong>Non-normative reference lowering workspace for projecting FIR into lowered backend-oriented forms</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Reference implementation workspace</a></li>
  <li><a href="../../../IR/Lowering.md">IR lowering boundary</a></li>
  <li><a href="../../../IR/Derivation%20rules.md">IR derivation rules</a></li>
  <li><a href="../../../Examples/05_bounded_ui_accumulator/Readme.md">Example 05</a></li>
  <li><a href="../../../Examples/05_bounded_ui_accumulator/Freeze.md">Example 05 freeze boundary</a></li>
  <li><a href="../ContractEmitter/Readme.md">ContractEmitter</a></li>
</ul>

<hr/>

<h2>1. Purpose</h2>

<p>
This directory contains the first non-normative reference lowerer for the FROG implementation workspace.
</p>

<p>
The first target is intentionally narrow:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.fir.json
  -&gt;
Examples/05_bounded_ui_accumulator/main.lowering.json
</code></pre>

<p>
This tool does not claim to be a complete lowering engine.
It exists to make the frozen Example 05 lowering artifact reproducible from the published FIR.
</p>

<hr/>

<h2>2. Current Scope</h2>

<ul>
  <li>loads one published FIR artifact,</li>
  <li>checks the minimal FIR shape needed for Example 05,</li>
  <li>projects the bounded stateful UI unit into the current lowered unit shape,</li>
  <li>preserves public IO, UI bindings, reference writes, explicit state, loop count, iteration body, commit rule, and final publications,</li>
  <li>can compare the generated lowering against the published lowering artifact.</li>
</ul>

<hr/>

<h2>3. Run</h2>

<pre><code>python Implementations/Reference/Lowerer/lower_example05_fir.py --check
</code></pre>

<p>
To write a generated lowering artifact elsewhere:
</p>

<pre><code>python Implementations/Reference/Lowerer/lower_example05_fir.py \
  --fir Examples/05_bounded_ui_accumulator/main.fir.json \
  --output build/generated/main.lowering.json
</code></pre>

<hr/>

<h2>4. Boundary</h2>

<p>
This lowerer is downstream from FIR and upstream from backend contract emission.
It does not own language semantics, FIR law, backend contracts, runtime behavior, or LLVM behavior.
</p>

<pre><code>FIR
  -&gt; Lowerer
  -&gt; lowered backend-oriented form
  -&gt; ContractEmitter / compiler-family consumer
</code></pre>

<hr/>

<h2>5. Next Step</h2>

<p>
After this lowerer is green for Example 05, the next stage is:
</p>

<pre><code>main.lowering.json
  -&gt;
reference_host_runtime_ui_binding.contract.json
</code></pre>

<p>
That path should remain owned by <code>Implementations/Reference/ContractEmitter/</code>.
</p>
