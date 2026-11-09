<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">03 — Contract-Emittable but Consumer-Rejected</h1>

<p align="center">
  <strong>Negative compiler-corridor case where the backend contract is validly emitted but the declared backend-family consumer must reject it</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/03_contract_emittable_but_consumer_rejected</code></pre>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that backend-contract emission and backend-family consumption are distinct stages.
</p>

<p>
The protected distinction is:
</p>

<pre><code>backend-contract-emittable
   !=
backend-family-consumable</code></pre>

<hr/>

<h2>Why this Case Exists</h2>

<p>
A serious handoff architecture requires explicit consumer rejection when declared assumptions cannot be honored.
</p>

<p>
Without this distinction, implementations tend to:
</p>

<ul>
  <li>silently reinterpret contracts,</li>
  <li>ignore required capabilities,</li>
  <li>pretend that contract emission already guarantees realizability everywhere.</li>
</ul>

<p>
This case prevents that drift.
</p>

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

<p>
Illustrative causes may include:
</p>

<ul>
  <li>unsupported contract-version semantics,</li>
  <li>unsupported required capability,</li>
  <li>unsupported lowered representation commitment,</li>
  <li>another explicit consumer-side incompatibility.</li>
</ul>

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

<hr/>

<h2>Expected Canonical JSON IR Result</h2>

<pre><code>Expected IR schema result: schema-valid</code></pre>

<hr/>

<h2>Expected Lowering Result</h2>

<pre><code>Expected lowering result: lowerable</code></pre>

<hr/>

<h2>Expected Backend-Contract Result</h2>

<pre><code>Expected backend-contract result: emittable</code></pre>

<p>
The producer side of the corridor is expected to succeed.
</p>

<hr/>

<h2>Declared Profile Scope</h2>

<pre><code>Declared profile scope: native_cpu_llvm</code></pre>

<hr/>

<h2>Declared Backend-Family Scope</h2>

<pre><code>Declared backend-family scope: LLVM-oriented native CPU consumer</code></pre>

<hr/>

<h2>Expected Backend-Family Consumption</h2>

<pre><code>Expected backend-family consumption: rejected</code></pre>

<hr/>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: backend-family consumer intake</code></pre>

<hr/>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the emitted backend contract is explicit and validly produced,
but the declared backend-family consumer cannot honor one or more required capabilities or commitments</code></pre>

<hr/>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>backend contract remains distinct from backend-family consumption,</li>
  <li>consumer rejection remains distinct from producer failure,</li>
  <li>explicit consumer incompatibility remains distinct from semantic invalidity.</li>
</ul>

<hr/>

<h2>Case Classification</h2>

<pre><code>Case classification:
- negative
- compiler corridor
- backend-contract-emittable
- consumer-rejected
- native_cpu_llvm boundary case</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case proves that the backend contract is a real handoff boundary.
A conforming backend-family consumer must reject explicitly when it cannot honor the declared contract, instead of silently reinterpreting it.
</p>
