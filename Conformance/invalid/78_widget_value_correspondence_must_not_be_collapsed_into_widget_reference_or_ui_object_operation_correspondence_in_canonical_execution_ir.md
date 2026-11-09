<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 78</h1>

<p align="center">
  <strong>Widget value correspondence must not be collapsed into widget reference or UI-object-operation correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-reference-or-ui-operation-relation-is-not-a-value-correspondence-substitute">7. Why Reference or UI-Operation Relation Is Not a Value-Correspondence Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>78_widget_value_correspondence_must_not_be_collapsed_into_widget_reference_or_ui_object_operation_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only widget reference or UI-object-operation correspondence where widget value correspondence should also remain explicitly recoverable as a distinct value-facing relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific UI-side category collapse:</p>

<pre><code>widget value correspondence
                -&gt;
widget reference or UI-object-operation correspondence only
</code></pre>

<p>That collapse is invalid because value participation is not the same thing as reference participation and is not the same thing as object-operation participation.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>value-facing widget relation,</li>
  <li>reference-facing widget relation,</li>
  <li>UI-object-operation relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>a reference-side or UI-operation relation exists
        therefore
value-side correspondence no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains widget-side material with both value-facing and reference-facing participation, potentially together with UI-object operations.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>reference-side or operation-side relations remain recoverable,</li>
  <li>those relations may be well-shaped and explicit,</li>
  <li>but widget value correspondence is omitted, erased, or folded into one of the other categories,</li>
  <li>or the reader is expected to infer value participation from reference or UI-operation patterns.</li>
</ul>

<p>The result is an IR that preserves some UI-side correspondence but preserves the wrong architectural category set.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>widget value correspondence is omitted while widget reference correspondence remains,</li>
  <li>widget value correspondence is represented as if it were merely reference-side participation,</li>
  <li>widget value correspondence is represented only through UI-object operation records,</li>
  <li>value recovery depends on heuristics about later UI operations or runtime-private handling,</li>
  <li>the canonical IR reader cannot distinguish whether a relation is value-facing or object-reference / object-operation-facing.</li>
</ul>

<p>Any of those outcomes breaks the required category preservation.</p>

<hr/>

<h2 id="why-reference-or-ui-operation-relation-is-not-a-value-correspondence-substitute">7. Why Reference or UI-Operation Relation Is Not a Value-Correspondence Substitute</h2>
<p>Reference or UI-operation relation is not a value-correspondence substitute because:</p>

<ul>
  <li>a value path expresses value-facing participation,</li>
  <li>a reference path expresses object-facing participation,</li>
  <li>a UI-object operation expresses interaction with object-level behavior.</li>
</ul>

<p>Even when those paths are related, they do not authorize one category to stand in for another.</p>

<p>If such substitution were allowed:</p>

<ul>
  <li>ordinary widget value behavior could be overinterpreted as object-level manipulation,</li>
  <li>UI-object operations could be mistaken for value exchange,</li>
  <li>value semantics would become less stable across implementations.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some UI-side relations,</li>
  <li>but widget value correspondence has been wrongly collapsed into reference-side or operation-side correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid value/reference/operation distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>UI-side relation survives
but is not correctly preserved
if value-facing correspondence
has been replaced by reference-facing or operation-facing correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the language’s UI distinction discipline instead of blurring it during derivation.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>reference-side or operation-side relation remains recoverable,</li>
  <li>widget value correspondence has been collapsed or lost,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
