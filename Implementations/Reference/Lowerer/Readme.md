<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Lowerer</h1>

<p align="center">
  <strong>Non-normative reference lowering workspace for projecting FIR into lowered backend-oriented forms</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory contains the first non-normative reference lowerer for the FROG implementation workspace.
</p>

<p>
The supported lowering targets are intentionally bounded:
</p>

<ul>
  <li><code>Examples/01_pure_addition/main.fir.json -&gt; main.lowering.json</code></li>
  <li><code>Examples/02_ui_value_roundtrip/main.fir.json -&gt; main.lowering.json</code></li>
  <li><code>Examples/03_ui_property_write/main.fir.json -&gt; main.lowering.json</code></li>
  <li><code>Examples/04_stateful_feedback_delay/main.fir.json -&gt; main.lowering.json</code></li>
  <li><code>Examples/05_bounded_ui_accumulator/main.fir.json -&gt; main.lowering.json</code></li>
</ul>

<hr/>

<h2>Commands</h2>

<pre><code>python Implementations/Reference/Lowerer/lower_fir.py \
  --fir Examples/01_pure_addition/main.fir.json \
  --expected Examples/01_pure_addition/main.lowering.json \
  --check
</code></pre>

<pre><code>python Implementations/Reference/Lowerer/lower_example05_fir.py --check
</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
The lowerer is downstream from FIR and upstream from backend contracts, runtime consumers, and compiler-family consumers.
It does not define FROG semantics.
</p>
