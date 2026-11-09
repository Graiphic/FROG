<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Conformance Case — 04 Stateful Feedback with Explicit Delay</h1>

<p align="center">
  Valid conformance case for explicit local-memory feedback in FROG v0.1<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#status">2. Expected Status</a></li>
  <li><a href="#source-target">3. Source Target</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-valid">5. Why this Case Is Valid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#expected-derivation-preservation">7. Expected Derivation Preservation</a></li>
  <li><a href="#expected-backend-contract-preservation">8. Expected Backend Contract Preservation</a></li>
  <li><a href="#what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#summary">10. Summary</a></li>
</ul>

<hr/>

<h2 id="case-overview">1. Case Overview</h2>

<p>
This case covers a minimal valid feedback graph with:
</p>

<ul>
  <li>one public input,</li>
  <li>one arithmetic primitive,</li>
  <li>one explicit local-memory primitive of type <code>frog.core.delay</code>,</li>
  <li>one public output,</li>
  <li>one feedback cycle.</li>
</ul>

<p>
The cycle is valid only because explicit local memory is present.
</p>

<hr/>

<h2 id="status">2. Expected Status</h2>

<p><strong>Expected: valid</strong></p>

<hr/>

<h2 id="source-target">3. Source Target</h2>

<p>
Expected source target:
</p>

<pre><code>Examples/04_stateful_feedback_delay/main.frog</code></pre>

<hr/>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>public interface participation,</li>
  <li>ordinary primitive execution,</li>
  <li>explicit local-memory primitive usage,</li>
  <li>required deterministic initialization,</li>
  <li>cycle legality through explicit memory,</li>
  <li>state-preserving derivation,</li>
  <li>state-preserving backend handoff.</li>
</ul>

<hr/>

<h2 id="why-this-case-is-valid">5. Why this Case Is Valid</h2>

<p>
This case is valid because:
</p>

<ul>
  <li>the feedback cycle contains an explicit local-memory primitive,</li>
  <li>the delay node provides the required initial state,</li>
  <li>the carried values are type-compatible,</li>
  <li>the resulting recurrence is explicit and attributable.</li>
</ul>

<p>
Without the explicit delay, the same feedback would be invalid.
</p>

<hr/>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator should accept the case.
It should confirm at minimum that:
</p>

<ul>
  <li>the delay primitive reference is valid,</li>
  <li>the required <code>initial</code> field is present,</li>
  <li>the initial value is type-compatible with the carried state,</li>
  <li>the cycle is legal because explicit local memory is present,</li>
  <li>the graph does not rely on hidden implicit memory.</li>
</ul>

<hr/>

<h2 id="expected-derivation-preservation">7. Expected Derivation Preservation</h2>

<p>
Execution IR derivation should preserve explicitly:
</p>

<ul>
  <li>primitive execution identity for <code>frog.core.add</code>,</li>
  <li>explicit local-memory execution identity for <code>frog.core.delay</code>,</li>
  <li>the explicit initial-state value,</li>
  <li>the feedback connectivity,</li>
  <li>the public output relation to the stateful computation.</li>
</ul>

<p>
The derivation must not reinterpret this case as:
</p>

<ul>
  <li>a pure combinational graph,</li>
  <li>a cycle legalized by hidden runtime memory insertion,</li>
  <li>a backend-private state mechanism with no attributable semantic state object.</li>
</ul>

<hr/>

<h2 id="expected-backend-contract-preservation">8. Expected Backend Contract Preservation</h2>

<p>
If this case is lowered and emitted as a backend contract, the contract should still make clear that:
</p>

<ul>
  <li>explicit local memory is semantically required,</li>
  <li>the carried state has a deterministic initial value,</li>
  <li>the feedback path is stateful rather than combinational,</li>
  <li>the consumer must preserve explicit state semantics.</li>
</ul>

<hr/>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<ul>
  <li>The case must not be rejected as an illegal cycle once explicit delay is present and valid.</li>
  <li>The case must not be accepted by erasing the explicit memory object during derivation.</li>
  <li>The case must not be lowered into a contract that loses the fact that valid recurrence depends on explicit local memory.</li>
</ul>

<hr/>

<h2 id="summary">10. Summary</h2>

<p>
This is the baseline valid conformance case for explicit local-memory feedback in FROG v0.1.
A conforming toolchain should accept it,
preserve explicit memory through derivation,
and carry explicit state semantics into backend handoff without semantic laundering.
</p>
