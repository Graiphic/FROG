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

<h2>Published Files</h2>

<pre><code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/
├── Readme.md
├── main.fir.json
├── main.lowering.json
├── module.ll
├── build.sh
└── expected-output.json
</code></pre>

<hr/>

<h2>Generation Check</h2>

<p>
The published <code>module.ll</code> can be regenerated and checked from the upstream lowered artifact:
</p>

<pre><code>python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check</code></pre>

<hr/>

<h2>Build and Run</h2>

<pre><code>cd Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator
bash build.sh
</code></pre>

<p>
The build script checks the published native module for inputs <code>0</code>, <code>3</code>, and <code>7</code>.
For input <code>3</code>, the expected observable result is:
</p>

<pre><code>final_state=15
public_output=15
status=ok
</code></pre>

<hr/>

<h2>Lowering Fidelity</h2>

<p>
The LLVM kernel is now emitted as an explicit loop rather than as the closed-form shortcut <code>input_value * 5</code>.
</p>

<p>
The generated function mirrors the lowered kernel shape:
</p>

<pre><code>state_current = 0
i = 0

while i &lt; iteration_count:
    state_next = state_current + input_value
    state_current = state_next
    i = i + 1

return state_current
</code></pre>

<p>
This preserves the visible loop and commit posture of <code>main.lowering.json</code> while remaining a narrow proof path.
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
