<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 80</h1>

<p align="center">
  <strong>Widget reference correspondence must not be collapsed into UI-object-operation correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-operation-correspondence-is-not-a-reference-correspondence-substitute">7. Why Operation Correspondence Is Not a Reference-Correspondence Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>80_widget_reference_correspondence_must_not_be_collapsed_into_ui_object_operation_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only UI-object-operation correspondence where widget-reference correspondence should also remain explicitly recoverable as a distinct reference-facing relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific UI-side category collapse:</p>

<pre><code>widget reference correspondence
                -&gt;
UI-object-operation correspondence only
</code></pre>

<p>That collapse is invalid because object-reference participation is not the same thing as object-operation participation.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>reference-facing widget relation, and</li>
  <li>operation-facing UI-object relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>an operation-side relation exists
        therefore
reference-side correspondence no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains widget-side material with reference-facing participation and later UI-object operations performed through that reference-facing path.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>operation-side relations remain recoverable,</li>
  <li>those relations may be well-shaped and explicit,</li>
  <li>but widget-reference correspondence is omitted, erased, or folded into operation correspondence,</li>
  <li>or the reader is expected to infer reference participation only from the existence of later operations.</li>
</ul>

<p>The result is an IR that preserves some UI-object-level activity but preserves the wrong architectural category set.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>widget-reference correspondence is omitted while UI-object-operation correspondence remains,</li>
  <li>widget-reference correspondence is represented as if it were merely operation participation,</li>
  <li>reference recovery depends on heuristics about later property reads, writes, or method calls,</li>
  <li>operation records are used as the sole surviving explanation of reference-facing participation,</li>
  <li>the canonical IR reader cannot distinguish whether a relation is object-reference-facing or object-operation-facing.</li>
</ul>

<p>Any of those outcomes breaks the required category preservation.</p>

<hr/>

<h2 id="why-operation-correspondence-is-not-a-reference-correspondence-substitute">7. Why Operation Correspondence Is Not a Reference-Correspondence Substitute</h2>
<p>Operation correspondence is not a reference-correspondence substitute because:</p>

<ul>
  <li>a reference path expresses that a widget-side object-facing relation exists,</li>
  <li>an operation path expresses that a specific UI-object interaction is performed through that relation.</li>
</ul>

<p>Even when operations depend on a reference path, they do not erase the need to preserve the reference path as its own public relation category.</p>

<p>If such substitution were allowed:</p>

<ul>
  <li>mere object-facing access could be confused with concrete interaction behavior,</li>
  <li>different operations could appear to redefine the reference relation itself,</li>
  <li>UI-object derivation semantics would become less stable across implementations.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some operation-side relations,</li>
  <li>but widget-reference correspondence has been wrongly collapsed into operation correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid reference/operation distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>UI-side relation survives
but is not correctly preserved
if reference-facing correspondence
has been replaced by operation-facing correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the language’s distinction between object reference participation and object operation participation.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>operation-side relation remains recoverable,</li>
  <li>widget-reference correspondence has been collapsed or lost,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
