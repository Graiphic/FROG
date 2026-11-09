<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 90</h1>

<p align="center">
  <strong>ui_in correspondence must not be collapsed into ui_out correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ui-out-is-not-a-ui-in-substitute">7. Why ui_out Is Not a ui_in Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>90_ui_in_correspondence_must_not_be_collapsed_into_ui_out_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only <code>ui_out</code> correspondence where <code>ui_in</code> correspondence should also remain explicitly recoverable as a distinct incoming sequencing relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific UI sequencing collapse:</p>

<pre><code>ui_in correspondence
            -&gt;
ui_out correspondence only
</code></pre>

<p>That collapse is invalid because incoming sequencing participation is not equivalent to outgoing sequencing participation.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li><code>ui_in</code> relation, and</li>
  <li><code>ui_out</code> relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>a ui_out relation exists
        therefore
ui_in correspondence no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains explicit UI sequencing participation through <code>ui_in</code> and possibly <code>ui_out</code>.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li><code>ui_out</code> relations remain recoverable,</li>
  <li>those relations may be explicit and well-shaped,</li>
  <li>but <code>ui_in</code> correspondence is omitted, erased, or folded into <code>ui_out</code> correspondence,</li>
  <li>or the reader is expected to infer incoming sequencing only from outgoing sequencing structure or implementation-private handling.</li>
</ul>

<p>The result is an IR that preserves some explicit UI sequencing but preserves the wrong directional category set.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li><code>ui_in</code> correspondence is omitted while <code>ui_out</code> correspondence remains,</li>
  <li><code>ui_in</code> correspondence is represented as if it were merely <code>ui_out</code> participation,</li>
  <li>incoming sequencing recovery depends on heuristics about local ordering, edge direction, or expected UI behavior,</li>
  <li><code>ui_out</code> records are used as the sole surviving explanation of explicit UI sequencing when <code>ui_in</code> is in scope,</li>
  <li>the canonical IR reader cannot distinguish whether a relation is incoming-sequencing-facing or outgoing-sequencing-facing.</li>
</ul>

<p>Any of those outcomes breaks the required sequencing-category preservation.</p>

<hr/>

<h2 id="why-ui-out-is-not-a-ui-in-substitute">7. Why ui_out Is Not a ui_in Substitute</h2>
<p><code>ui_out</code> is not a <code>ui_in</code> substitute because:</p>

<ul>
  <li><code>ui_in</code> expresses explicit incoming sequencing participation,</li>
  <li><code>ui_out</code> expresses explicit outgoing sequencing participation.</li>
</ul>

<p>Even when both belong to the same local UI-side sequencing path, they do not authorize one category to stand in for the other.</p>

<p>If such substitution were allowed:</p>

<ul>
  <li>entry-side and exit-side sequencing would become harder to distinguish,</li>
  <li>directional UI sequencing semantics would become less stable,</li>
  <li>UI-side execution interpretation would become more implementation-dependent.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some <code>ui_out</code> relations,</li>
  <li>but <code>ui_in</code> correspondence has been wrongly collapsed into <code>ui_out</code> correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid <code>ui_in</code>/<code>ui_out</code> distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>UI sequencing correspondence survives
but is not correctly preserved
if incoming correspondence
has been replaced by outgoing correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the distinction between incoming and outgoing explicit UI sequencing participation.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li><code>ui_out</code> relation remains recoverable,</li>
  <li><code>ui_in</code> correspondence has been collapsed or lost,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
