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

<p>
At the current published state, this directory exposes a first repository-visible LLVM-oriented closure dossier for the canonical Example 05 corridor, and a reference helper that can regenerate the published <code>module.ll</code> from the published lowered artifact.
</p>

<hr/>

<h2>Architectural Boundary</h2>

<pre><code>canonical .frog source
      |
      v
validated meaning
      |
      v
FIR / execution-facing representation
      |
      v
lowering
      |
      v
compiler-facing lowered kernel
      |
      v
LLVM-oriented downstream path
      |
      v
native executable
</code></pre>

<p>
The current LLVM path is intentionally a pure bounded-kernel proof.
It does not compile the rendered front panel natively.
UI and widget effects remain runtime-mediated in the current architecture.
</p>

<hr/>

<h2>Current Reproducibility Check</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check</code></pre>

<p>
This command reads:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.lowering.json</code></pre>

<p>
and checks that the emitted LLVM module matches:
</p>

<pre><code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll</code></pre>

<hr/>

<h2>Native Build Proof</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check --build</code></pre>

<p>
The native build proof requires <code>clang</code>.
The build checks inputs <code>0</code>, <code>3</code>, and <code>7</code>.
</p>

<hr/>

<h2>Current Fidelity Improvement</h2>

<p>
The current module is emitted from the lowered loop shape:
</p>

<ul>
  <li><code>initial_state = 0</code>,</li>
  <li><code>state_type = u16</code>,</li>
  <li><code>iteration_count</code> read from <code>main.lowering.json</code>,</li>
  <li><code>state_next = state_current + input_value</code>,</li>
  <li><code>state_current &lt;- state_next after each iteration</code>.</li>
</ul>

<p>
This is more faithful than the earlier closed-form multiplication proof while still remaining a narrow Example 05 native kernel proof.
</p>

<hr/>

<h2>Summary</h2>

<p>
The current LLVM-oriented path verifies:
</p>

<pre><code>main.lowering.json -&gt; loop-shaped module.ll -&gt; native proof</code></pre>

<p>
The next depth step would be to add native overflow rejection parity for the runtime-family <code>u16</code> overflow policy.
</p>
