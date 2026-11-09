<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">02 — Lowerable but Not Backend-Contract-Emittable</h1>

<p align="center">
  <strong>Negative compiler-corridor case where lowering is possible but no consumer-safe backend contract can be emitted</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/02_lowerable_but_not_backend_contract_emittable</code></pre>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that lowering eligibility and backend-contract emission are distinct stages of the published compiler corridor.
</p>

<p>
The protected distinction is:
</p>

<pre><code>lowerable
   !=
backend-contract-emittable</code></pre>

<hr/>

<h2>Why this Case Exists</h2>

<p>
A serious backend handoff architecture requires an explicit consumer-facing contract.
It is therefore not enough that a producer can transform canonical open IR into a lowered form.
The producer must also be able to state the downstream assumptions explicitly and safely.
</p>

<p>
This case exists to prevent the following drift:
</p>

<pre><code>lowered artifact exists
   -/-&gt;
safe consumer-facing handoff exists</code></pre>

<p>
Without this distinction, implementations would tend to:
</p>

<ul>
  <li>emit partially specified contracts,</li>
  <li>rely on hidden producer/consumer conventions,</li>
  <li>push ambiguity into the backend consumer,</li>
  <li>pretend that a lowered form is automatically consumable.</li>
</ul>

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

<p>
Illustrative reasons include:
</p>

<ul>
  <li>the lowered form depends on assumptions that are not yet expressible in the published backend contract surface,</li>
  <li>required capability declarations cannot be stated precisely enough,</li>
  <li>consumer-relevant storage, effect, or call-surface commitments remain underspecified,</li>
  <li>the producer would need hidden conventions to complete the handoff.</li>
</ul>

<p>
This case is therefore not a lowering failure.
It is a backend-contract-emission failure.
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

<hr/>

<h2>Expected IR Derivation</h2>

<pre><code>Expected IR derivation: derivable</code></pre>

<p>
The case must derive to canonical Execution IR without loss of required distinctions.
</p>

<hr/>

<h2>Expected Canonical JSON IR Result</h2>

<pre><code>Expected IR schema result: schema-valid</code></pre>

<hr/>

<h2>Declared Profile Scope</h2>

<pre><code>Declared profile scope: native_cpu_llvm</code></pre>

<hr/>

<h2>Expected Lowering Result</h2>

<pre><code>Expected lowering result: lowerable</code></pre>

<p>
The producer is assumed to be able to form a semantically faithful lowered representation.
</p>

<hr/>

<h2>Expected Backend-Contract Result</h2>

<pre><code>Expected backend-contract result: not emittable</code></pre>

<p>
The failure occurs because the producer cannot yet emit a consumer-safe handoff without hidden assumptions or underspecified commitments.
</p>

<hr/>

<h2>Expected Backend-Family Consumption</h2>

<pre><code>Expected backend-family consumption: not applicable</code></pre>

<p>
No declared backend-family consumer should be asked to consume the case once the contract-emission boundary has already failed.
</p>

<hr/>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: backend-contract emission</code></pre>

<hr/>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
lowering succeeds,
but the producer cannot emit an explicit consumer-safe backend contract
for the declared corridor without ambiguity, hidden assumptions, or unsupported commitments</code></pre>

<hr/>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>lowering success remains distinct from backend-contract emission success,</li>
  <li>backend-contract emission failure remains distinct from lowering failure,</li>
  <li>producer-side handoff ambiguity remains distinct from consumer-side rejection,</li>
  <li>contract readiness remains distinct from merely having a lowered artifact.</li>
</ul>

<hr/>

<h2>Case Classification</h2>

<pre><code>Case classification:
- negative
- compiler corridor
- lowerable
- backend-contract-not-emittable
- native_cpu_llvm boundary case</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case proves that the backend contract is a real architectural boundary rather than a trivial by-product of lowering.
A conforming implementation must reject here rather than emit an underspecified contract and force the consumer to guess the missing commitments.
</p>
