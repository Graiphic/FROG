<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">04 — Contract-Emittable but Consumer-Rejected</h1>

<p align="center">
  <strong>Negative compiler-corridor case where the backend contract is validly emitted but the declared backend-family consumer must reject it</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/04_contract_emittable_but_consumer_rejected</code></pre>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that backend-contract emission and backend-family consumption are distinct stages.
</p>

<pre><code>backend-contract-emittable
   !=
backend-family-consumable</code></pre>

<hr/>

<h2>Source Scenario</h2>

<p>
The program is assumed to be:
</p>

<ul>
  <li>structurally valid,</li>
  <li>semantically accepted,</li>
  <li>IR-derivable,</li>
  <li>lowerable,</li>
  <li>backend-contract-emittable,</li>
  <li>but dependent on an explicit contract requirement not supported by the declared backend-family consumer.</li>
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

<h2>Expected Lowering Result</h2>

<pre><code>Expected lowering result: lowerable</code></pre>

<h2>Expected Backend-Contract Result</h2>

<pre><code>Expected backend-contract result: emittable</code></pre>

<h2>Declared Profile Scope</h2>

<pre><code>Declared profile scope: native_cpu_llvm</code></pre>

<h2>Declared Backend-Family Scope</h2>

<pre><code>Declared backend-family scope: LLVM-oriented native CPU consumer</code></pre>

<h2>Expected Backend-Family Consumption</h2>

<pre><code>Expected backend-family consumption: rejected</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: backend-family consumer intake</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the emitted backend contract is explicit and validly produced,
but the declared backend-family consumer cannot honor one or more required capabilities or commitments</code></pre>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>backend contract remains distinct from backend-family consumption,</li>
  <li>consumer rejection remains distinct from producer failure,</li>
  <li>explicit consumer incompatibility remains distinct from semantic invalidity.</li>
</ul>

<h2>Summary</h2>

<p>
This case proves that the backend contract is a real handoff boundary.
A conforming backend-family consumer must reject explicitly when it cannot honor the declared contract.
</p>
