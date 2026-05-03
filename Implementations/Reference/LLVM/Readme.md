<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference LLVM-Oriented Path</h1>

<p align="center">
  <strong>Downstream compiler-family posture for native executable closure in the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory defines the downstream compiler-family posture for LLVM-oriented native executable closure in the non-normative FROG reference implementation.
LLVM remains downstream from FROG.
It is one possible compiler-family consumer of lowered FROG artifacts.
It is not the definition of the language, not the owner of FIR, and not the owner of widget semantics.
</p>

<hr/>

<h2>Current Reproducibility Check</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check</code></pre>

<hr/>

<h2>Native Build Proof</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check --build</code></pre>

<p>
The native build proof requires <code>clang</code>.
The build checks normal cases and the current <code>u16</code> overflow rejection case.
</p>

<hr/>

<h2>Current Mapping</h2>

<p>
The current module is emitted from the lowered loop shape:
</p>

<ul>
  <li><code>initial_state = 0</code>,</li>
  <li><code>state_type = u16</code>,</li>
  <li><code>iteration_count</code> read from <code>main.lowering.json</code>,</li>
  <li><code>state_next = state_current + input_value</code>,</li>
  <li><code>state_current &lt;- state_next after each iteration</code>,</li>
  <li>native rejection when an intermediate state would leave the <code>u16</code> domain.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
The current LLVM path is intentionally a pure bounded-kernel proof.
It does not compile the rendered front panel natively.
UI and widget effects remain runtime-mediated in the current architecture.
</p>

<hr/>

<h2>Summary</h2>

<p>
The current LLVM-oriented path verifies:
</p>

<pre><code>main.lowering.json -&gt; loop-shaped module.ll -&gt; native proof with u16 overflow rejection</code></pre>
