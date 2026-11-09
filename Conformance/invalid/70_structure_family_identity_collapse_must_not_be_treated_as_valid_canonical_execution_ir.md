<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 70</h1>

<p align="center">
  <strong>Structure family identity collapse must not be treated as valid canonical execution IR</strong><br/>
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
  <li><a href="#why-region-and-terminal-survival-are-not-enough">7. Why Region and Terminal Survival Are Not Enough</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>70_structure_family_identity_collapse_must_not_be_treated_as_valid_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves some structure-shaped carriers, but no longer preserves recoverable structure-family identity.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects reduction of distinct structure families into one generic structure bucket.</p>

<p>The rejected collapse is:</p>

<pre><code>recoverable structure-family distinction
                -&gt;
generic structure container only
</code></pre>

<p>That collapse is architecturally invalid when family identity remains part of the execution-facing meaning that must survive derivation.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>recoverable structure-family identity, and</li>
  <li>generic region-bearing structure presence.</li>
</ul>

<p>The rejected substitution is:</p>

<pre><code>regions and terminals still exist
        therefore
family identity no longer matters
</code></pre>

<p>That inference is invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains an explicit structure family.</p>

<p>During canonical execution IR derivation:</p>

<ul>
  <li>the structure remains present as some generic execution-facing container,</li>
  <li>regions may remain present,</li>
  <li>terminals may remain present,</li>
  <li>but the IR no longer preserves which structure family the object belongs to,</li>
  <li>or that distinction can be reconstructed only indirectly from implementation-specific behavior.</li>
</ul>

<p>The resulting IR has preserved fragments of structure but lost the family boundary.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>different structure families are normalized into one undifferentiated category,</li>
  <li>family identity is omitted while regions and terminals are kept,</li>
  <li>family recovery depends only on heuristics such as terminal arrangement or dynamic behavior,</li>
  <li>runtime-private interpretation is required to know which structure family is present,</li>
  <li>the canonical IR reader cannot recover the family distinction directly from the public IR layer.</li>
</ul>

<p>Any of those outcomes breaks structure-family preservation.</p>

<hr/>

<h2 id="why-region-and-terminal-survival-are-not-enough">7. Why Region and Terminal Survival Are Not Enough</h2>
<p>Region and terminal survival are not enough because they preserve only part of the structure boundary.</p>

<p>Without family identity:</p>

<ul>
  <li>distinct structure categories may be confused,</li>
  <li>downstream interpretation becomes less deterministic at the architectural level,</li>
  <li>conformance can no longer verify that the correct family semantics were preserved,</li>
  <li>generic IR shape may hide semantic collapse.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>structure internals survive
        therefore
structure family is preserved
</code></pre>

<p>That shortcut is not acceptable.</p>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still contain structure-shaped objects,</li>
  <li>but structure-family identity has been collapsed.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid structure-family recoverability case.</p>

<p>It closes the next local gap in the repository’s IR recoverability corridor:</p>

<pre><code>region recovery
+
terminal recovery
+
role recovery
do not by themselves guarantee
family recovery
</code></pre>

<p>Canonical execution IR must remain strong enough to preserve not only local structure internals but also the family boundary those internals belong to.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>a structure-family distinction exists upstream,</li>
  <li>the IR preserves generic structure presence but loses family identity,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
