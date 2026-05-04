<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Pipeline</h1>

<p align="center">
  <strong>Executable reference pipeline for the current Examples 01–05 FROG corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Pipeline</h2>

<pre><code>Examples 01–05
  .frog
    -&gt; FIR
    -&gt; lowering
    -&gt; backend contract
    -&gt; runtime acceptance
    -&gt; LLVM module / native proof
</code></pre>

<hr/>

<h2>Command</h2>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py</code></pre>

<p>
With widget validation:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py --include-widget-validator</code></pre>

<p>
With native LLVM builds:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py --include-widget-validator --include-llvm-build</code></pre>

<hr/>

<h2>Stage Order</h2>

<ol>
  <li>Artifact preflight for Examples 01–05.</li>
  <li>Optional widget-layer validation.</li>
  <li><code>.frog -&gt; FIR</code> for Examples 01–05.</li>
  <li><code>FIR -&gt; lowering</code> for Examples 01–05.</li>
  <li><code>lowering -&gt; backend contract</code>.</li>
  <li><code>contract -&gt; runtime acceptance</code>.</li>
  <li><code>lowering -&gt; LLVM module</code>.</li>
  <li>Optional native LLVM build proof.</li>
</ol>

<hr/>

<h2>Boundary</h2>

<p>
The pipeline is non-normative.
It verifies published artifacts and implementation-stage coherence.
It does not define the FROG language.
</p>
