<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 88</h1>

<p align="center">
  <strong>UI-object-operation correspondence must not be collapsed into ui_in and ui_out sequencing correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ui-sequencing-is-not-a-ui-object-operation-substitute">7. Why UI Sequencing Is Not a UI-Object-Operation Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>88_ui_object_operation_correspondence_must_not_be_collapsed_into_ui_in_and_ui_out_sequencing_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only <code>ui_in</code> or <code>ui_out</code> sequencing correspondence where UI-object-operation correspondence should also remain explicitly recoverable as a distinct operation-facing relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific UI-side category collapse:</p>

<pre><code>UI-object-operation correspondence
                -&gt;
ui_in / ui_out sequencing correspondence only
</code></pre>

<p>That collapse is invalid because explicit sequencing around UI effects is not equivalent to the UI-object interaction itself.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>UI-object-operation relation, and</li>
  <li><code>ui_in</code> / <code>ui_out</code> sequencing relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>a ui-sequencing relation exists
        therefore
UI-object-operation correspondence no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains explicit UI-object interaction together with explicit <code>ui_in</code> and <code>ui_out</code> sequencing participation.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>sequencing-side relations remain recoverable,</li>
  <li>those relations may be explicit and well-shaped,</li>
  <li>but UI-object-operation correspondence is omitted, erased, or folded into sequencing correspondence,</li>
  <li>or the reader is expected to infer the UI-object interaction only from effect-ordering carriers or from implementation-private handling.</li>
</ul>

<p>The result is an IR that preserves some explicit UI ordering but preserves the wrong architectural category set.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>UI-object-operation correspondence is omitted while <code>ui_in</code> / <code>ui_out</code> sequencing correspondence remains,</li>
  <li>UI-object-operation correspondence is represented as if it were merely sequencing participation,</li>
  <li>operation recovery depends on heuristics about UI effect order, sequencing carriers, or expected behavior,</li>
  <li>sequencing records are used as the sole surviving explanation of the UI-object operation when such operation is in scope,</li>
  <li>the canonical IR reader cannot distinguish whether a relation is operation-facing or sequencing-facing.</li>
</ul>

<p>Any of those outcomes breaks the required category preservation.</p>

<hr/>

<h2 id="why-ui-sequencing-is-not-a-ui-object-operation-substitute">7. Why UI Sequencing Is Not a UI-Object-Operation Substitute</h2>
<p>UI sequencing is not a UI-object-operation substitute because:</p>

<ul>
  <li>a UI-object operation identifies what UI-facing interaction is performed,</li>
  <li><code>ui_in</code> / <code>ui_out</code> sequencing identifies how explicit ordering or effect chaining is carried around that interaction.</li>
</ul>

<p>Even when both appear in the same local path, they do not authorize one category to stand in for the other.</p>

<p>If such substitution were allowed:</p>

<ul>
  <li>ordering and action would become harder to distinguish,</li>
  <li>effect reasoning would become less stable,</li>
  <li>UI-side execution interpretation would become more implementation-dependent.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some <code>ui_in</code> / <code>ui_out</code> sequencing relations,</li>
  <li>but UI-object-operation correspondence has been wrongly collapsed into sequencing correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid operation/sequencing distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>UI-side correspondence survives
but is not correctly preserved
if operation-facing correspondence
has been replaced by ui-sequencing correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the distinction between UI action and explicit UI sequencing.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li><code>ui_in</code> / <code>ui_out</code> sequencing relation remains recoverable,</li>
  <li>UI-object-operation correspondence has been collapsed or lost,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
