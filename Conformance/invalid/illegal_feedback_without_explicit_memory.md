<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Conformance Case — Invalid: Illegal Feedback Without Explicit Memory</h1>

<p align="center">
  Invalid conformance case for combinational feedback in FROG v0.1<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#status">2. Expected Status</a></li>
  <li><a href="#intended-anti-pattern">3. Intended Anti-Pattern</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-invalid">5. Why this Case Is Invalid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#why-derivation-is-forbidden">7. Why Derivation Is Forbidden</a></li>
  <li><a href="#why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr/>

<h2 id="case-overview">1. Case Overview</h2>

<p>
This invalid case covers a directed feedback cycle that contains no explicit local-memory primitive.
</p>

<p>
Typical anti-pattern:
</p>

<pre><code>a(result) ----&gt; b(input)
b(result) ----&gt; a(input)
</code></pre>

<p>
with only ordinary primitives and no <code>frog.core.delay</code> or equivalent explicit local-memory primitive.
</p>

<hr/>

<h2 id="status">2. Expected Status</h2>

<p><strong>Expected: invalid</strong></p>

<hr/>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
Conceptually, the invalid shape is any directed cycle with no explicit semantic memory inside the cycle.
A self-feedback edge with no local-memory primitive is also invalid.
</p>

<hr/>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>cycle legality,</li>
  <li>explicit local-memory requirements,</li>
  <li>semantic determinism of feedback,</li>
  <li>rejection of hidden scheduler-based “repair”.</li>
</ul>

<hr/>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because a pure dataflow cycle with no explicit local memory does not define valid executable behavior by itself.
In base FROG v0.1, every directed cycle must contain at least one explicit local-memory primitive.
</p>

<p>
Only explicit local memory can make the feedback meaning well-defined.
Layout, intention, or tool convenience cannot make such a cycle valid.
</p>

<hr/>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator should reject the case.
It should report that:
</p>

<ul>
  <li>a directed cycle exists,</li>
  <li>the cycle contains no explicit local-memory primitive,</li>
  <li>the graph is therefore an invalid combinational cycle.</li>
</ul>

<p>
If the case attempts to use <code>frog.core.delay</code> but omits the mandatory <code>initial</code> field,
it should also be rejected,
because v0.1 requires deterministic initial state for every delay node.
</p>

<hr/>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>
Execution IR derivation is forbidden for this invalid case because validation has not established a valid feedback meaning.
There is no conforming basis for deriving:
</p>

<ul>
  <li>a valid explicit state object,</li>
  <li>a valid stateful cycle,</li>
  <li>or a valid backend contract carrying explicit state semantics.</li>
</ul>

<hr/>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>
A conforming toolchain must not silently “repair” this case by:
</p>

<ul>
  <li>inserting hidden memory,</li>
  <li>inventing a hidden scheduler-private buffer,</li>
  <li>reclassifying the cycle as valid through runtime convention alone.</li>
</ul>

<p>
That would violate the published rule that only explicit local memory makes feedback portable and semantically well defined.
</p>

<hr/>

<h2 id="summary">9. Summary</h2>

<p>
This case must be rejected because a cycle without explicit local memory is invalid in FROG v0.1.
A conforming implementation must reject the graph rather than legalizing it through hidden memory or scheduler-private interpretation.
</p>
