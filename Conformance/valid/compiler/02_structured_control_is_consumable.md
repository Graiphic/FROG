<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">02 — Structured Control Is Consumable</h1>

<p align="center">
  <strong>Positive compiler-corridor case for a structured-control slice consumable by the declared native CPU LLVM-oriented corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>valid/compiler/02_structured_control_is_consumable</code></pre>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that a bounded structured-control slice can remain valid across the full declared compiler corridor without losing structured semantics.
</p>

<p>
The protected positive statement is:
</p>

<pre><code>structured control
   can remain
semantically preserved,
IR-derivable,
lowerable,
contract-emittable,
and backend-family consumable</code></pre>

<hr/>

<h2>Why this Case Exists</h2>

<p>
A real compiler corridor cannot stop at pure arithmetic forever.
It must demonstrate that at least a conservative structured-control slice can survive the full corridor without being flattened into semantic ambiguity.
</p>

<p>
This case therefore exists to prove that:
</p>

<ul>
  <li>structured control remains structured control,</li>
  <li>regions and boundary roles remain meaningful through the corridor,</li>
  <li>lowering may specialize control representation without destroying control semantics.</li>
</ul>

<hr/>

<h2>Source Scenario</h2>

<p>
The program defines:
</p>

<ul>
  <li>a typed selector or loop-driving input,</li>
  <li>a bounded structured-control family accepted by core FROG,</li>
  <li>a small pure body inside the structure,</li>
  <li>a typed output reflecting the structured result,</li>
  <li>no UI-runtime dependence,</li>
  <li>no unsupported effect surface for the declared corridor.</li>
</ul>

<p>
Illustrative execution shape:
</p>

<pre><code>selector
   |
   v
 case
 ├── region A: pure arithmetic path
 └── region B: alternate pure arithmetic path
   |
   v
 result</code></pre>

<p>
A loop-shaped variant is also conceptually acceptable if the implementation declares it within the supported corridor subset.
</p>

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
The structured form is assumed to be semantically accepted by the core language for the claimed case shape.
</p>

<hr/>

<h2>Expected IR Derivation</h2>

<pre><code>Expected IR derivation: derivable</code></pre>

<p>
The canonical IR must preserve:
</p>

<ul>
  <li>structure-family identity,</li>
  <li>region ownership,</li>
  <li>boundary participation,</li>
  <li>structured terminal or equivalent role distinctions where applicable,</li>
  <li>ordinary internal computation inside the structure.</li>
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
  <li>control normalization toward CFG-friendly forms,</li>
  <li>explicit region entry and exit handling,</li>
  <li>backend-facing structured control preparation.</li>
</ul>

<p>
Lowering must remain faithful and must not erase the structured-control semantics that still matter to the corridor.
</p>

<hr/>

<h2>Expected Backend-Contract Result</h2>

<pre><code>Expected backend-contract result: emittable</code></pre>

<p>
The producer must be able to emit a consumer-safe contract that preserves the structured-control commitments relevant to the declared corridor.
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
The declared backend-family consumer should be able to consume the contract while preserving the control semantics through its own downstream mechanisms.
</p>

<hr/>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>structured control remains distinct from ordinary flat connectivity,</li>
  <li>region ownership remains recoverable until the permitted downstream boundary,</li>
  <li>boundary participation remains distinct from ordinary connectivity,</li>
  <li>backend-family consumption remains downstream from structured FROG semantics rather than redefining them.</li>
</ul>

<hr/>

<h2>Case Classification</h2>

<pre><code>Case classification:
- positive
- compiler corridor
- structured control
- native_cpu_llvm
- LLVM-oriented native CPU consumable</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case is the first positive structured-control anchor for the compiler corridor.
A conforming implementation supporting the declared subset of <code>native_cpu_llvm</code> should accept, derive, lower, contract-emit, and consume this case without collapsing structured semantics.
</p>
