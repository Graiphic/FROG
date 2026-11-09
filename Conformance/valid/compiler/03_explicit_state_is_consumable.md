<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">03 — Explicit State Is Consumable</h1>

<p align="center">
  <strong>Positive compiler-corridor case for an explicit-state slice consumable by the declared native CPU LLVM-oriented corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>valid/compiler/03_explicit_state_is_consumable</code></pre>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that an explicit local-memory / delay-like slice can remain valid across the full declared compiler corridor while preserving explicit state semantics.
</p>

<p>
The protected positive statement is:
</p>

<pre><code>explicit state
   can remain
semantically preserved,
IR-derivable,
lowerable,
contract-emittable,
and backend-family consumable</code></pre>

<hr/>

<h2>Why this Case Exists</h2>

<p>
A serious compilation corridor must eventually handle explicit state.
If the corridor can compile only pure computation and structured control, it still leaves one of the most important dataflow-language distinctions unclosed.
</p>

<p>
This case therefore exists to prove that:
</p>

<ul>
  <li>explicit local memory remains explicit local memory,</li>
  <li>initialization remains explicit when semantically required,</li>
  <li>state participation is not rewritten as accidental persistence,</li>
  <li>a backend-facing route can consume state-bearing programs without semantic invention.</li>
</ul>

<hr/>

<h2>Source Scenario</h2>

<p>
The program defines:
</p>

<ul>
  <li>a typed input,</li>
  <li>an explicit local-memory or delay-like construct accepted by core FROG,</li>
  <li>an explicit initialization value where required,</li>
  <li>a typed output depending on the state-bearing execution path,</li>
  <li>no UI-runtime dependence,</li>
  <li>no unsupported external effect surface for the declared corridor.</li>
</ul>

<p>
Illustrative execution shape:
</p>

<pre><code>input ----\
          +--- delay/state --- result
initial --/</code></pre>

<hr/>

<h2>Expected Loadability</h2>

<pre><code>Expected loadability: loadable</code></pre>

<hr/>

<h2>Expected Structural Validity</h2>

<pre><code>Expected structural validity: valid</code></pre>

<hr/>

<h2>Expected Semantic Acceptance</h2>

<pre><code>Expected semantic acceptance: accepted</code></pre>

<p>
The case is accepted only because the state-bearing construct is explicit and semantically legal under the core language.
</p>

<hr/>

<h2>Expected IR Derivation</h2>

<pre><code>Expected IR derivation: derivable</code></pre>

<p>
The canonical IR must preserve:
</p>

<ul>
  <li>explicit state identity,</li>
  <li>initialization participation,</li>
  <li>state-related boundary participation where applicable,</li>
  <li>distinction between state semantics and ordinary connectivity.</li>
</ul>

<hr/>

<h2>Expected Canonical JSON IR Result</h2>

<pre><code>Expected IR schema result: schema-valid</code></pre>

<hr/>

<h2>Expected Lowering Result</h2>

<pre><code>Expected lowering result: lowerable</code></pre>

<p>
Valid lowering may include:
</p>

<ul>
  <li>materialization of explicit state cells,</li>
  <li>explicit initialization carriers,</li>
  <li>storage-class specialization,</li>
  <li>backend-facing memory commitments.</li>
</ul>

<p>
Lowering must not invent hidden persistence and must not erase explicit state semantics.
</p>

<hr/>

<h2>Expected Backend-Contract Result</h2>

<pre><code>Expected backend-contract result: emittable</code></pre>

<p>
The producer must be able to emit a contract that states the relevant state and initialization commitments explicitly enough for the declared corridor.
</p>

<hr/>

<h2>Declared Profile Scope</h2>

<pre><code>Declared profile scope: native_cpu_llvm</code></pre>

<hr/>

<h2>Declared Backend-Family Scope</h2>

<pre><code>Declared backend-family scope: LLVM-oriented native CPU consumer</code></pre>

<hr/>

<h2>Expected Backend-Family Consumption</h2>

<pre><code>Expected backend-family consumption: consumable</code></pre>

<p>
The declared backend-family consumer should be able to consume the contract while preserving the state semantics of the accepted subset.
</p>

<hr/>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>explicit state remains distinct from ordinary dataflow,</li>
  <li>explicit initialization remains distinct from inferred defaulting,</li>
  <li>state participation remains distinct from hidden backend-private persistence,</li>
  <li>backend-family consumption remains downstream from the explicit state semantics of FROG.</li>
</ul>

<hr/>

<h2>Case Classification</h2>

<pre><code>Case classification:
- positive
- compiler corridor
- explicit state
- native_cpu_llvm
- LLVM-oriented native CPU consumable</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case is the first positive explicit-state anchor for the compiler corridor.
A conforming implementation supporting the declared subset of <code>native_cpu_llvm</code> should accept, derive, lower, contract-emit, and consume this case without rewriting explicit state into hidden persistence.
</p>
