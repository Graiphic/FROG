<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">LLVM Closure Example — 05 Bounded UI Accumulator</h1>

<p align="center">
  <strong>First native LLVM corridor for the canonical bounded UI accumulator slice</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory publishes the first LLVM-native closure material for the canonical <code>05_bounded_ui_accumulator</code> example.
Its role is not to define a general-purpose LLVM backend.
Its role is to make the first native corridor repository-visible, executable, and reproducible from the published lowering artifact.
</p>

<hr/>

<h2>Generation Check</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check</code></pre>

<hr/>

<h2>Build and Run</h2>

<pre><code>cd Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator
bash build.sh
</code></pre>

<p>
The build script checks:
</p>

<ul>
  <li><code>input 0 -&gt; 0</code></li>
  <li><code>input 3 -&gt; 15</code></li>
  <li><code>input 7 -&gt; 35</code></li>
  <li><code>input 20000 -&gt; status=error</code></li>
</ul>

<hr/>

<h2>Lowering Fidelity</h2>

<p>
The LLVM kernel is emitted as an explicit loop, not as a closed-form shortcut.
</p>

<pre><code>state_current = 0
i = 0

while i &lt; iteration_count:
    sum = state_current + input_value
    if sum &gt; 65535:
        reject
    state_current = sum
    i = i + 1

return state_current
</code></pre>

<hr/>

<h2>Overflow Parity</h2>

<p>
The native proof now mirrors the current runtime-family overflow policy for the bounded <code>u16</code> slice:
</p>

<pre><code>status=error
error=final_state must remain in the u16 domain.
</code></pre>

<p>
This keeps the native proof aligned with the runtime acceptance case for <code>input_value = 20000</code>.
</p>

<hr/>

<h2>Boundary</h2>

<p>
This directory is downstream from canonical source, FIR, lowering, and runtime-family discussion.
It is a native proof corridor, not a language-definition directory.
</p>

<pre><code>first native example closure
    !=
fully generalized production LLVM backend
</code></pre>

<p>
The current proof compiles the bounded arithmetic kernel.
It does not compile the rendered front panel natively.
</p>
