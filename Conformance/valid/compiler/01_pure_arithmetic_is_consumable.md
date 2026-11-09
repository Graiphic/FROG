<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">01 — Pure Arithmetic Is Consumable</h1>

<p align="center">
  <strong>Positive compiler-corridor case for a pure arithmetic slice consumable by the declared native CPU LLVM-oriented corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>valid/compiler/01_pure_arithmetic_is_consumable</code></pre>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that a small pure arithmetic FROG slice can pass through the full declared compiler corridor:
</p>

<pre><code>.frog
   -&gt;
structural validity
   -&gt;
semantic acceptance
   -&gt;
canonical Execution IR derivation
   -&gt;
canonical JSON IR validity
   -&gt;
lowering eligibility
   -&gt;
backend-contract eligibility
   -&gt;
declared backend-family consumability</code></pre>

<p>
This is the foundational positive case for the first conservative <code>native_cpu_llvm</code> corridor.
</p>

<hr/>

<h2>Why this Case Exists</h2>

<p>
A serious compiler corridor should begin with a slice that is:
</p>

<ul>
  <li>small,</li>
  <li>pure,</li>
  <li>typed,</li>
  <li>free of UI-runtime dependence,</li>
  <li>free of ambiguous effect surfaces,</li>
  <li>naturally compatible with native compilation.</li>
</ul>

<p>
Pure arithmetic is therefore the natural first positive anchor.
</p>

<hr/>

<h2>Source Scenario</h2>

<p>
The program defines:
</p>

<ul>
  <li>two numeric inputs,</li>
  <li>one pure arithmetic primitive chain,</li>
  <li>one numeric output,</li>
  <li>no explicit local memory,</li>
  <li>no UI participation,</li>
  <li>no interop boundary,</li>
  <li>no deployment-specific behavior.</li>
</ul>

<p>
Illustrative execution shape:
</p>

<pre><code>a ----\
       +--- add ---\
b ----/            +--- multiply_by_constant --- result
constant ----------/</code></pre>

<hr/>

<h2>Expected Loadability</h2>

<pre><code>Expected loadability: loadable</code></pre>

<hr/>

<h2>Expected Structural Validity</h2>

<pre><code>Expected structural validity: valid</code></pre>

<p>
The source is expected to be valid canonical FROG source.
</p>

<hr/>

<h2>Expected Semantic Acceptance</h2>

<pre><code>Expected semantic acceptance: accepted</code></pre>

<p>
The program is semantically valid because it only uses:
</p>

<ul>
  <li>typed inputs and outputs,</li>
  <li>pure arithmetic primitives,</li>
  <li>ordinary explicit dataflow connectivity.</li>
</ul>

<hr/>

<h2>Expected IR Derivation</h2>

<pre><code>Expected IR derivation: derivable</code></pre>

<p>
The program must derive to canonical FROG Execution IR without ambiguity.
</p>

<p>
The derived IR must preserve:
</p>

<ul>
  <li>primary execution-object attribution,</li>
  <li>ordinary dataflow connectivity,</li>
  <li>typed execution-facing ports or equivalent terminal carriers,</li>
  <li>distinction between inputs, computation objects, and output participation.</li>
</ul>

<hr/>

<h2>Expected Canonical JSON IR Result</h2>

<pre><code>Expected IR schema result: schema-valid</code></pre>

<p>
The canonical JSON IR for this case must be structurally valid under the published IR schema for the claimed version.
</p>

<hr/>

<h2>Expected Lowering Result</h2>

<pre><code>Expected lowering result: lowerable</code></pre>

<p>
A conforming implementation claiming the declared corridor must be able to lower this case faithfully.
</p>

<p>
Typical valid lowering directions include:
</p>

<ul>
  <li>SSA-friendly pure computation preparation,</li>
  <li>typed native arithmetic lowering,</li>
  <li>explicit backend-facing data representation commitment.</li>
</ul>

<hr/>

<h2>Expected Backend-Contract Result</h2>

<pre><code>Expected backend-contract result: emittable</code></pre>

<p>
A backend contract must be emittable without hidden assumptions because:
</p>

<ul>
  <li>the case is pure,</li>
  <li>the case has no explicit state,</li>
  <li>the case has no UI-service dependence,</li>
  <li>the case has no unresolved interop boundary.</li>
</ul>

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
The declared backend-family consumer should be able to consume the contract for this case without semantic reinvention.
</p>

<hr/>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>ordinary connectivity remains ordinary connectivity,</li>
  <li>primary source attribution remains recoverable,</li>
  <li>canonical Execution IR remains distinct from lowered form,</li>
  <li>backend contract remains distinct from private realization,</li>
  <li>compiler-family consumption remains downstream from FROG IR.</li>
</ul>

<hr/>

<h2>Case Classification</h2>

<pre><code>Case classification:
- positive
- compiler corridor
- pure computation
- native_cpu_llvm
- LLVM-oriented native CPU consumable</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case is the foundational positive compiler-corridor anchor for the first conservative native compiled route.
A conforming implementation supporting <code>native_cpu_llvm</code> should accept, derive, lower, contract-emit, and consume this case successfully.
</p>
