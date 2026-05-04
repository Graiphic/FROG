<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 04 — Stateful Feedback Delay</h1>

<p align="center">
  <strong>Minimal valid feedback example using frog.core.delay with explicit initial state</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/04_stateful_feedback_delay/
├── Readme.md
├── main.frog
├── main.fir.json
└── main.lowering.json
</code></pre>

<hr/>

<h2>Source Shape</h2>

<pre><code>interface_input(x) ----\
                       &gt; frog.core.add ---&gt; interface_output(y)
frog.core.delay.out ---/       |
        ^                      |
        \------ delay.in ------/
</code></pre>

<hr/>

<h2>Reference Checks</h2>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py --source Examples/04_stateful_feedback_delay/main.frog --expected Examples/04_stateful_feedback_delay/main.fir.json --check
python Implementations/Reference/Lowerer/lower_fir.py --fir Examples/04_stateful_feedback_delay/main.fir.json --expected Examples/04_stateful_feedback_delay/main.lowering.json --check
</code></pre>

<hr/>

<h2>Boundary</h2>

<p>This example is an explicit-state derivation and lowering slice. It does not currently claim backend contract, runtime acceptance, or LLVM closure.</p>
