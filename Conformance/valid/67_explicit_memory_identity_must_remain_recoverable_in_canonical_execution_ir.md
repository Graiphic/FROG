<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 67</h1>

<p align="center">
  <strong>Explicit memory identity must remain recoverable in canonical execution IR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-preserved">4. Boundary Being Preserved</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-must-remain-true">6. What Must Remain True</a></li>
  <li><a href="#why-explicit-memory-cannot-be-left-implicit">7. Why Explicit Memory Cannot Be Left Implicit</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>67_explicit_memory_identity_must_remain_recoverable_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where validated program meaning contains explicit memory identity, canonical execution IR must preserve that memory identity as recoverable public execution-facing structure rather than reducing it to anonymous persistence behavior.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects the explicit-memory rule at the canonical execution IR layer.</p>

<p>The preserved rule is:</p>

<pre><code>explicit memory in validated meaning
                -&gt;
recoverable explicit memory identity in canonical execution IR
</code></pre>

<p>Memory must remain identifiable as memory, not just as a runtime effect or persistence outcome.</p>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>explicit memory identity, and</li>
  <li>mere emergent or inferred persistence behavior.</li>
</ul>

<p>The preserved distinction is:</p>

<pre><code>explicit memory identity
            !=
"something persistent seems to happen"
</code></pre>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains explicit memory participation that is part of the accepted language meaning.</p>

<p>In the valid case:</p>

<ul>
  <li>memory identity remains recoverable in canonical execution IR,</li>
  <li>the reader can tell that a specific execution-facing memory entity exists,</li>
  <li>the memory identity is not replaced by a generic stateful effect,</li>
  <li>the memory boundary remains above runtime-private realization.</li>
</ul>

<p>The case is about explicit identity preservation, not about one particular runtime representation.</p>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>memory participation remains explicit,</li>
  <li>memory identity remains recoverable,</li>
  <li>identity is not reduced to scheduler behavior or persistence folklore,</li>
  <li>the canonical execution IR still exposes memory as part of public execution-facing structure.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>explicit memory identity
+
recoverable execution-facing carrier
+
distinctness from runtime-private storage
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-explicit-memory-cannot-be-left-implicit">7. Why Explicit Memory Cannot Be Left Implicit</h2>
<p>Explicit memory cannot be left implicit because FROG already rejects inferred persistence as a substitute for explicit state or memory meaning.</p>

<p>If memory identity is lost during derivation:</p>

<ul>
  <li>different implementations may materialize different hidden storage assumptions,</li>
  <li>the canonical execution IR becomes less inspectable,</li>
  <li>conformance loses the ability to check whether the same memory entity is being preserved,</li>
  <li>runtime-private realization starts replacing open architectural truth.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that explicit memory identity survives derivation into canonical execution IR as recoverable public structure.</p>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the repository’s already-published discipline around:</p>

<ul>
  <li>explicit state versus inferred persistence,</li>
  <li>explicit initialization, read timing, write visibility, snapshot, version, merge, and commit boundaries,</li>
  <li>recoverability of structure and terminal meaning at IR level.</li>
</ul>

<p>It adds the next missing identity-focused closure:</p>

<pre><code>explicit memory
must remain
explicitly identifiable
after derivation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>explicit memory identity exists in validated meaning,</li>
  <li>that identity remains recoverable in canonical execution IR,</li>
  <li>the case is valid.</li>
</ul>
