<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 84</h1>

<p align="center">
  <strong>Method-invocation correspondence must not be collapsed into property-write correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-property-write-is-not-a-method-invocation-substitute">7. Why Property Write Is Not a Method-Invocation Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>84_method_invocation_correspondence_must_not_be_collapsed_into_property_write_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only property-write correspondence where method-invocation correspondence should also remain explicitly recoverable as a distinct operation-facing relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific UI-object operation collapse:</p>

<pre><code>method-invocation correspondence
                -&gt;
property-write correspondence only
</code></pre>

<p>That collapse is invalid because invoking named behavior is not equivalent to assigning property state.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>method-invocation relation, and</li>
  <li>property-write relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>a property-write relation exists
        therefore
method-invocation correspondence no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains UI-object interactions including method invocation and possibly property write.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>property-write relations remain recoverable,</li>
  <li>those relations may be explicit and well-shaped,</li>
  <li>but method-invocation correspondence is omitted, erased, or folded into property-write correspondence,</li>
  <li>or the reader is expected to infer behavior invocation only from property mutation patterns or implementation-private handling.</li>
</ul>

<p>The result is an IR that preserves some UI-object mutation-facing material but preserves the wrong operation category set.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>method-invocation correspondence is omitted while property-write correspondence remains,</li>
  <li>method-invocation correspondence is represented as if it were merely property-write participation,</li>
  <li>invocation recovery depends on heuristics about property names, write targets, or expected UI effects,</li>
  <li>property-write records are used as the sole surviving explanation of a method call when invocation is in scope,</li>
  <li>the canonical IR reader cannot distinguish whether a relation is method-invocation-facing or property-write-facing.</li>
</ul>

<p>Any of those outcomes breaks the required operation-category preservation.</p>

<hr/>

<h2 id="why-property-write-is-not-a-method-invocation-substitute">7. Why Property Write Is Not a Method-Invocation Substitute</h2>
<p>Property write is not a method-invocation substitute because:</p>

<ul>
  <li>a method invocation triggers or requests named behavior,</li>
  <li>a property write applies or requests state modification at a property boundary.</li>
</ul>

<p>Even when both may produce visible UI changes, they do not authorize one category to stand in for the other.</p>

<p>If such substitution were allowed:</p>

<ul>
  <li>behavior-oriented calls would become harder to distinguish from assignment-oriented updates,</li>
  <li>effect reasoning would become less stable,</li>
  <li>UI-object operation semantics would become more implementation-dependent.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some property-write relations,</li>
  <li>but method-invocation correspondence has been wrongly collapsed into property-write correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid method/property-write distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>UI-object operation correspondence survives
but is not correctly preserved
if behavior-invocation correspondence
has been replaced by property-write correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the distinction between behavior-oriented UI-object invocation and property-oriented state mutation.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>property-write relation remains recoverable,</li>
  <li>method-invocation correspondence has been collapsed or lost,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
