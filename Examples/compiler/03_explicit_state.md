<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 03 — Explicit State</h1>

<p align="center">
  <strong>Illustrative explicit-state slice for the conservative native compilation corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example illustrates a bounded explicit-state slice in which local memory remains explicit across the compiler corridor.
</p>

<hr/>

<h2>Illustrative Shape</h2>

<pre><code>input ----\
          +--- delay/state --- result
initial --/</code></pre>

<hr/>

<h2>Why this Example Matters</h2>

<p>
A serious compilation corridor must eventually handle explicit local memory without rewriting it as hidden persistence.
This example therefore closes the first minimal state-bearing slice of the conservative corridor.
</p>

<hr/>

<h2>Architectural Reading</h2>

<ul>
  <li>Expression: explicit state-bearing construct</li>
  <li>Language: explicit local-memory legality and initialization meaning</li>
  <li>IR: explicit state participation and initialization carriers</li>
  <li>Lowering: explicit state-cell or equivalent faithful memory lowering</li>
  <li>Backend contract: explicit state and initialization commitments for the consumer</li>
</ul>

<hr/>

<h2>Related Conformance Case</h2>

<pre><code>Conformance/valid/compiler/03_explicit_state_is_consumable</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This example is the illustrative mirror of the first explicit-state positive compiler-corridor anchor.
</p>
