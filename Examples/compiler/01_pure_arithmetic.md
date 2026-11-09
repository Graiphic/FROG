<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 01 — Pure Arithmetic</h1>

<p align="center">
  <strong>Illustrative pure computation slice for the conservative native compilation corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This example illustrates the smallest natural positive slice of the first native compilation corridor:
a pure typed arithmetic graph.
</p>

<hr/>

<h2>Illustrative Shape</h2>

<pre><code>a ----\
       +--- add ---\
b ----/            +--- multiply_by_constant --- result
constant ----------/</code></pre>

<hr/>

<h2>Why this Example Matters</h2>

<p>
This example is:
</p>

<ul>
  <li>pure,</li>
  <li>typed,</li>
  <li>free of UI-runtime dependence,</li>
  <li>free of explicit state,</li>
  <li>naturally compatible with native compilation.</li>
</ul>

<p>
It is therefore the first natural illustrative slice for the positive <code>native_cpu_llvm</code> corridor.
</p>

<hr/>

<h2>Architectural Reading</h2>

<ul>
  <li>Expression: canonical typed graph structure</li>
  <li>Language: pure arithmetic meaning</li>
  <li>IR: primary arithmetic execution objects and ordinary connectivity</li>
  <li>Lowering: SSA-friendly or equivalent pure computation preparation</li>
  <li>Backend contract: explicit native-CPU-friendly computation handoff</li>
</ul>

<hr/>

<h2>Related Conformance Case</h2>

<pre><code>Conformance/valid/compiler/01_pure_arithmetic_is_consumable</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This example is the illustrative mirror of the first positive compiler-corridor anchor.
</p>
