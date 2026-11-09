<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 66</h1>

<p align="center">
  <strong>Declaration reference must not be treated as primary execution identity in canonical execution IR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-violated">4. Boundary Being Violated</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#why-reference-cannot-replace-identity">7. Why Reference Cannot Replace Identity</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>66_declaration_reference_must_not_be_treated_as_primary_execution_identity_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> a declaration reference is used as if it were the execution-facing object’s primary identity.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a category mistake in canonical execution IR.</p>

<p>The rejected substitution is:</p>

<pre><code>declaration reference
            -/-> primary execution identity
</code></pre>

<p>A reference to a declaration-side source anchor may remain valuable and recoverable. It must not become the execution object’s own identity.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the boundary between:</p>

<ul>
  <li>execution-layer identity, and</li>
  <li>upstream declaration-layer reference.</li>
</ul>

<p>The failure occurs when the IR stops distinguishing:</p>

<pre><code>what the execution object is
                from
what declaration-side object it refers to
</code></pre>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A canonical execution-facing object is derived from validated meaning and linked to a declaration-side anchor.</p>

<p>In the invalid case:</p>

<ul>
  <li>the declaration-side reference is preserved,</li>
  <li>but the execution object no longer has a recoverable identity distinct from that declaration reference,</li>
  <li>or the IR explicitly reuses the declaration reference as execution identity,</li>
  <li>or the distinction can only be reconstructed by runtime-private interpretation.</li>
</ul>

<p>The result is an execution IR that has lost one of its own identity boundaries.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>the declaration anchor identifier is reused as the execution object’s primary identity category,</li>
  <li>the IR provides only a declaration reference and no distinct execution identity,</li>
  <li>identity must be guessed from correspondence records rather than represented explicitly,</li>
  <li>the execution layer is reduced to a declaration replay rather than its own canonical representation.</li>
</ul>

<p>Any of those outcomes breaks the canonical execution IR boundary.</p>

<hr/>

<h2 id="why-reference-cannot-replace-identity">7. Why Reference Cannot Replace Identity</h2>
<p>Reference cannot replace identity because canonical execution IR is not merely a copy of declaration-side source structure.</p>

<p>It is a derived execution-facing representation.</p>

<p>Therefore it requires:</p>

<ul>
  <li>its own execution-facing object identities, and</li>
  <li>its own explicit links back to declaration-side origins where relevant.</li>
</ul>

<p>If reference replaces identity, then:</p>

<ul>
  <li>execution-level comparison becomes weaker,</li>
  <li>identity recoverability becomes inconsistent,</li>
  <li>downstream correspondence handling becomes harder to validate,</li>
  <li>the IR risks collapsing back into source folklore.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>the source may be valid,</li>
  <li>semantic meaning may be established,</li>
  <li>the IR may even appear well-formed,</li>
  <li>but declaration reference has wrongly replaced primary execution identity.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection closes the opposite side of the valid case.</p>

<p>The repository already protects against hidden identity loss and against loss of source anchoring. This case adds the missing adjacent protection:</p>

<pre><code>recoverable reference
does not justify
execution identity collapse
</code></pre>

<p>That keeps canonical execution IR inspectable as its own layer rather than a blurred shadow of declarations.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>a declaration reference exists,</li>
  <li>it is used as if it were the execution object’s identity,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
