<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 01 — Pure Addition</h1>

<p align="center">
  <strong>Minimal public-interface arithmetic example for the first pure source-to-FIR derivation slice</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example is the smallest useful executable FROG program in the repository.
It receives two public floating-point inputs, applies one core arithmetic primitive, and exposes the result through one public floating-point output.
</p>

<p>
It intentionally has no front panel participation, no widget participation, no structure, and no explicit local memory.
</p>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/01_pure_addition/
├── Readme.md
├── main.frog
└── main.fir.json
</code></pre>

<hr/>

<h2>Source Shape</h2>

<pre><code>interface_input(a) ---\
                       &gt; frog.core.add ---&gt; interface_output(result)
interface_input(b) ---/
</code></pre>

<hr/>

<h2>FIR Derivation</h2>

<p>
This example is now a published reference derivation target:
</p>

<pre><code>Examples/01_pure_addition/main.frog
  -&gt;
Examples/01_pure_addition/main.fir.json
</code></pre>

<p>
The check command is:
</p>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/01_pure_addition/main.frog \
  --expected Examples/01_pure_addition/main.fir.json \
  --check
</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This example is a pure derivation slice.
It does not currently claim published lowering, backend contract, runtime acceptance, or LLVM closure.
Those stages remain focused on Example 05 for now.
</p>
