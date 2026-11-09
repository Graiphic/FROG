<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Examples — Compiler Corridor</h1>

<p align="center">
  <strong>Illustrative positive slices for the first conservative native compilation corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#scope">2. Scope</a></li>
  <li><a href="#relation-with-conformance">3. Relation with Conformance</a></li>
  <li><a href="#published-example-order">4. Published Example Order</a></li>
  <li><a href="#summary">5. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This subdirectory provides illustrative positive slices for the first conservative native compilation corridor.
</p>

<p>
These examples are intended to mirror the first named positive compiler-corridor conformance cases without replacing them.
</p>

<hr/>

<h2 id="scope">2. Scope</h2>

<p>
The initial positive compiler-corridor mirror is intentionally conservative and targets the first declared profile corridor:
</p>

<pre><code>native_cpu_llvm</code></pre>

<p>
The examples are:
</p>

<pre><code>01_pure_arithmetic.md
02_structured_control.md
03_explicit_state.md</code></pre>

<hr/>

<h2 id="relation-with-conformance">3. Relation with Conformance</h2>

<p>
These examples mirror the corresponding positive conformance cases:
</p>

<pre><code>Examples/compiler/01_pure_arithmetic.md
   &lt;-&gt;
Conformance/valid/compiler/01_pure_arithmetic_is_consumable

Examples/compiler/02_structured_control.md
   &lt;-&gt;
Conformance/valid/compiler/02_structured_control_is_consumable

Examples/compiler/03_explicit_state.md
   &lt;-&gt;
Conformance/valid/compiler/03_explicit_state_is_consumable</code></pre>

<p>
The example illustrates.
The conformance case asserts the staged truth surface.
</p>

<hr/>

<h2 id="published-example-order">4. Published Example Order</h2>

<p>
The canonical order is:
</p>

<pre><code>01_pure_arithmetic.md
02_structured_control.md
03_explicit_state.md</code></pre>

<p>
This mirrors the same closure order used by the positive compiler corridor:
</p>

<pre><code>pure computation
   -&gt;
structured control
   -&gt;
explicit state</code></pre>

<hr/>

<h2 id="summary">5. Summary</h2>

<p>
This subdirectory provides the first bounded example mirror of the positive compiler corridor for <code>native_cpu_llvm</code>.
</p>
