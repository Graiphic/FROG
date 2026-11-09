<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 38</h1>

<p align="center">
  <strong>Derived execution identity must not replace recoverable source attribution</strong><br/>
  FROG — Free Open Graphical Language
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#1-case-name">1. Case Name</a></li>
  <li><a href="#2-expected">2. Expected</a></li>
  <li><a href="#3-why">3. Why</a></li>
  <li><a href="#4-boundary-being-violated">4. Boundary Being Violated</a></li>
  <li><a href="#5-scenario">5. Scenario</a></li>
  <li><a href="#6-what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#7-why-derived-identity-is-not-enough">7. Why Derived Identity Is Not Enough</a></li>
  <li><a href="#8-what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</a></li>
  <li><a href="#9-expected-conformance-result">9. Expected Conformance Result</a></li>
  <li><a href="#10-rationale">10. Rationale</a></li>
  <li><a href="#11-minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="1-case-name">1. Case Name</h2>

<p>
  Case:
  <code>38_derived_execution_identity_must_not_replace_recoverable_source_attribution</code>
</p>

<hr/>

<h2 id="2-expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected reason:</strong> rejected as a preservation failure.</p>

<p>
  <strong>Expected rejection basis:</strong>
  the case attempts to treat derived execution identity as sufficient in itself,
  while recoverable attribution to validated source meaning has been lost,
  severed, or made implementation-private.
</p>

<hr/>

<h2 id="3-why">3. Why</h2>

<p>
  This case exists to reject the forbidden collapse:
</p>

<pre><code>derived execution identity
            ->
sufficient semantic ownership
</code></pre>

<p>
  Derivation may introduce execution-oriented identifiers, helper artifacts,
  normalized graph fragments, internal boundaries, or decomposition products.
</p>

<p>
  Those derived identities may be useful.
  They do not become the new owner of semantic truth merely because they exist.
</p>

<p>
  If recoverable attribution to validated source meaning is lost and replaced by
  opaque derived identity, the implementation has crossed a conformance boundary.
</p>

<hr/>

<h2 id="4-boundary-being-violated">4. Boundary Being Violated</h2>

<p>
  This case violates the normative separation between:
</p>

<ul>
  <li>recoverable attribution to validated source meaning, and</li>
  <li>derived execution identity treated as a semantic substitute.</li>
</ul>

<p>
  The invalid substitution is:
</p>

<pre><code>recoverable source attribution
            !=
opaque derived execution identity
</code></pre>

<p>
  The case attempts to let execution-facing identity replace source ownership,
  rather than remain subordinate to it.
</p>

<hr/>

<h2 id="5-scenario">5. Scenario</h2>

<p>
  A valid source program is transformed into a derived execution-facing form.
  During that transformation, the implementation introduces one or more
  execution identities such as:
</p>

<ul>
  <li>internal node identifiers,</li>
  <li>lowered fragment identifiers,</li>
  <li>expanded helper-operation identifiers,</li>
  <li>scheduler-facing region identities,</li>
  <li>runtime-oriented state or dependency identifiers.</li>
</ul>

<p>
  The conformance failure occurs when those derived identities are then treated as
  sufficient and the implementation no longer preserves recoverable attribution to:
</p>

<ul>
  <li>the relevant source construct,</li>
  <li>the validated semantic owner,</li>
  <li>the upstream structural boundary,</li>
  <li>the source-established participation or responsibility.</li>
</ul>

<p>
  In that scenario, source attribution has effectively been replaced rather than preserved.
</p>

<hr/>

<h2 id="6-what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>
  This case is invalid because it allows derivation to sever semantic traceability.
</p>

<p>
  More precisely, it is invalid because it does at least one of the following:
</p>

<ul>
  <li>treats derived execution identity as the new semantic owner,</li>
  <li>accepts irreversible loss of attribution to validated source meaning,</li>
  <li>makes explanation of execution-facing elements depend on hidden implementation knowledge,</li>
  <li>uses decomposition or normalization as a justification for attribution loss,</li>
  <li>collapses architectural recoverability into opaque runtime-facing identity.</li>
</ul>

<p>
  A conforming implementation may derive, normalize, expand, and lower.
  It must not make source-origin recovery disappear behind those operations.
</p>

<hr/>

<h2 id="7-why-derived-identity-is-not-enough">7. Why Derived Identity Is Not Enough</h2>

<p>
  Derived identity is not enough because derivation is downstream from validated meaning.
</p>

<p>
  Execution-oriented identities are permitted as implementation or IR conveniences.
  They are not allowed to become the only surviving truth surface for semantic ownership.
</p>

<p>
  If they were enough, then:
</p>

<ul>
  <li>inspection would depend on executor-private knowledge,</li>
  <li>conformance review would become harder to ground,</li>
  <li>validation explanations would become weaker,</li>
  <li>independent implementations could diverge while all claiming equivalent execution identities,</li>
  <li>the repository would lose one of its core inspectability guarantees.</li>
</ul>

<p>
  FROG cannot permit that substitution.
</p>

<hr/>

<h2 id="8-what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</h2>

<p>
  Later layers may preserve derived execution identity as operational machinery,
  but they must not preserve it as the replacement for recoverable source attribution.
</p>

<p>
  In particular, a conforming pipeline must not preserve as normative truth:
</p>

<ul>
  <li>“this derived identifier exists, therefore source attribution no longer matters,”</li>
  <li>“the execution graph can run, therefore recoverability is optional,”</li>
  <li>“lowering introduced new identities, therefore validated source ownership is no longer needed,”</li>
  <li>“runtime-facing decomposition is sufficient explanation of program meaning.”</li>
</ul>

<p>
  The rejected promotion is:
</p>

<pre><code>derived execution identity
        -/-> replacement for recoverable source attribution
</code></pre>

<hr/>

<h2 id="9-expected-conformance-result">9. Expected Conformance Result</h2>

<p>
  This case must be rejected as invalid.
</p>

<p>
  The rejection result is not:
</p>

<ul>
  <li>“derived execution identity is forbidden,”</li>
  <li>“IR normalization is forbidden,”</li>
  <li>“lowering may not introduce new identifiers.”</li>
</ul>

<p>
  The actual rejection result is:
</p>

<ul>
  <li>derived execution identity may exist,</li>
  <li>but it must not replace recoverable attribution to validated source meaning.</li>
</ul>

<hr/>

<h2 id="10-rationale">10. Rationale</h2>

<p>
  This rejection matters because FROG is explicitly trying to keep the path from source,
  meaning, derivation, and execution inspectable.
</p>

<p>
  If derived execution identity can replace source attribution, then derivation becomes a
  semantic laundering step:
</p>

<pre><code>source
  ->
validated meaning
  ->
derived identity
  ->
source ownership disappears
</code></pre>

<p>
  That is exactly the kind of architectural collapse the repository is trying to avoid.
</p>

<p>
  Independent implementations need freedom in their execution-facing representation.
  They do not need freedom to erase source-origin recovery.
</p>

<p>
  The preserved rule remains:
</p>

<pre><code>derived execution identity
            must remain subordinate to
recoverable source attribution
</code></pre>

<hr/>

<h2 id="11-minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
  A minimal conforming reading of this case is:
</p>

<ul>
  <li>the case relies on derived execution identity,</li>
  <li>recoverable attribution to validated source meaning is lost or replaced,</li>
  <li>that loss breaks preservation requirements,</li>
  <li>the case must therefore be rejected as invalid.</li>
</ul>

<p>
  The public truth asserted by this rejection is:
</p>

<pre><code>derived execution identity
            must not replace
recoverable source attribution
</code></pre>
