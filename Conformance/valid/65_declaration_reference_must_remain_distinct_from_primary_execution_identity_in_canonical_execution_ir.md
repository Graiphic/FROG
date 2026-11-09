<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 65</h1>

<p align="center">
  <strong>Declaration reference must remain distinct from primary execution identity in canonical execution IR</strong><br/>
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
  <li><a href="#why-reference-and-identity-are-not-the-same">7. Why Reference and Identity Are Not the Same</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>65_declaration_reference_must_remain_distinct_from_primary_execution_identity_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> a declaration reference may remain recoverable for attribution, correspondence, or explanatory purposes, but it must remain distinct from the primary execution identity of the canonical execution object itself.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case preserves a specific identity boundary inside canonical execution IR.</p>

<p>The preserved rule is:</p>

<pre><code>declaration reference
            !=
primary execution identity
</code></pre>

<p>A canonical execution object may refer back to a declaration-side anchor, but that reference does not become the object’s execution identity.</p>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>the identity of the execution-facing object that exists in canonical execution IR, and</li>
  <li>the declaration-side object or reference from which part of its meaning, attribution, or correspondence may be derived.</li>
</ul>

<p>Both may be recoverable. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated source program contains a declaration-side object that participates in the derivation of a canonical execution-facing object.</p>

<p>In the valid case:</p>

<ul>
  <li>the declaration reference remains recoverable,</li>
  <li>the execution-facing object has its own primary identity,</li>
  <li>the IR makes both roles explicit,</li>
  <li>a conforming reader can distinguish “what this execution object is” from “which declaration-side origin it references.”</li>
</ul>

<p>This preserves both attribution clarity and execution-level identity clarity.</p>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>the execution object has its own recoverable primary identity,</li>
  <li>declaration reference remains recoverable where applicable,</li>
  <li>reference does not replace identity,</li>
  <li>identity does not erase reference,</li>
  <li>the canonical IR reader can distinguish the two roles without runtime-private reconstruction.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>execution object identity
+
declaration-side reference
+
explicit distinction
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-reference-and-identity-are-not-the-same">7. Why Reference and Identity Are Not the Same</h2>
<p>Reference and identity are not the same because they answer different architectural questions.</p>

<ul>
  <li>Primary execution identity answers: “Which canonical execution-facing object is this?”</li>
  <li>Declaration reference answers: “Which declaration-side source anchor does this object refer back to?”</li>
</ul>

<p>If those are collapsed:</p>

<ul>
  <li>identity tracking becomes ambiguous,</li>
  <li>derivation explanation becomes weaker,</li>
  <li>later correspondence handling becomes harder to validate,</li>
  <li>different implementations may hide different assumptions behind the same IR shape.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means the IR successfully preserves:</p>

<ul>
  <li>primary execution identity,</li>
  <li>recoverable declaration reference where applicable,</li>
  <li>the explicit distinction between those two categories.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case continues the repository’s identity-discipline line by preventing a new local collapse.</p>

<p>It sits naturally after:</p>

<ul>
  <li>intentional non-primary correspondence,</li>
  <li>recoverable source anchoring,</li>
  <li>multi-contributor attribution.</li>
</ul>

<p>It adds the next closure:</p>

<pre><code>reference recoverability
does not authorize
identity collapse
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>a canonical execution object has its own identity,</li>
  <li>it also retains a recoverable declaration reference,</li>
  <li>those two remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
