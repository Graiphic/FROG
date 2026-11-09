<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">03 — Lowerable but Not Backend-Contract-Emittable</h1>

<p align="center">
  <strong>Negative compiler-corridor case where lowering is possible but no consumer-safe backend contract can be emitted</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/03_lowerable_but_not_backend_contract_emittable</code></pre>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that lowering eligibility and backend-contract emission are distinct stages of the published compiler corridor.
</p>

<pre><code>lowerable
   !=
backend-contract-emittable</code></pre>

<hr/>

<h2>Source Scenario</h2>

<p>
The program is assumed to be:
</p>

<ul>
  <li>structurally valid canonical FROG source,</li>
  <li>semantically accepted by FROG,</li>
  <li>derivable to canonical FROG Execution IR,</li>
  <li>lowerable under the declared corridor,</li>
  <li>but still unable to yield a consumer-safe backend contract because one or more required handoff commitments remain ambiguous or unsupported.</li>
</ul>

<hr/>

<h2>Expected Loadability</h2>

<pre><code>Expected loadability: loadable</code></pre>

<h2>Expected Structural Validity</h2>

<pre><code>Expected structural validity: valid</code></pre>

<h2>Expected Semantic Acceptance</h2>

<pre><code>Expected semantic acceptance: accepted</code></pre>

<h2>Expected IR Derivation</h2>

<pre><code>Expected IR derivation: derivable</code></pre>

<h2>Expected Canonical JSON IR Result</h2>

<pre><code>Expected IR schema result: schema-valid</code></pre>

<h2>Declared Profile Scope</h2>

<pre><code>Declared profile scope: native_cpu_llvm</code></pre>

<h2>Expected Lowering Result</h2>

<pre><code>Expected lowering result: lowerable</code></pre>

<h2>Expected Backend-Contract Result</h2>

<pre><code>Expected backend-contract result: not emittable</code></pre>

<h2>Expected Backend-Family Consumption</h2>

<pre><code>Expected backend-family consumption: not applicable</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: backend-contract emission</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
lowering succeeds,
but the producer cannot emit an explicit consumer-safe backend contract
for the declared corridor without ambiguity, hidden assumptions, or unsupported commitments</code></pre>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>lowering success remains distinct from backend-contract emission success,</li>
  <li>backend-contract emission failure remains distinct from lowering failure,</li>
  <li>producer-side handoff ambiguity remains distinct from consumer-side rejection,</li>
  <li>contract readiness remains distinct from merely having a lowered artifact.</li>
</ul>

<h2>Summary</h2>

<p>
This case proves that the backend contract is a real architectural boundary rather than a trivial by-product of lowering.
A conforming implementation must reject here rather than emit an underspecified contract.
</p>
