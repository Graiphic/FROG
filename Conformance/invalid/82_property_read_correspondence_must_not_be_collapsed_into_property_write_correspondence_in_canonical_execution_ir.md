<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 82</h1>

<p align="center">
  <strong>Property-read correspondence must not be collapsed into property-write correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-write-correspondence-is-not-a-read-correspondence-substitute">7. Why Write Correspondence Is Not a Read-Correspondence Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>82_property_read_correspondence_must_not_be_collapsed_into_property_write_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only property-write correspondence where property-read correspondence should also remain explicitly recoverable as a distinct operation-facing relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific UI-object operation collapse:</p>

<pre><code>property-read correspondence
                -&gt;
property-write correspondence only
</code></pre>

<p>That collapse is invalid because property observation is not equivalent to property mutation.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>read-facing property relation, and</li>
  <li>write-facing property relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>a write-side relation exists
        therefore
read-side correspondence no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains property-oriented UI-object interactions including property reads and possibly property writes.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>write-side relations remain recoverable,</li>
  <li>those relations may be explicit and well-shaped,</li>
  <li>but property-read correspondence is omitted, erased, or folded into property-write correspondence,</li>
  <li>or the reader is expected to infer property observation only from write-side material or from implementation-private handling.</li>
</ul>

<p>The result is an IR that preserves some property interaction but preserves the wrong operation category set.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>property-read correspondence is omitted while property-write correspondence remains,</li>
  <li>property-read correspondence is represented as if it were merely property-write participation,</li>
  <li>read recovery depends on heuristics about later writes or about target-property names alone,</li>
  <li>write-side records are used as the sole surviving explanation of property-side interaction when a read operation is in scope,</li>
  <li>the canonical IR reader cannot distinguish whether a relation is read-facing or write-facing.</li>
</ul>

<p>Any of those outcomes breaks the required operation-category preservation.</p>

<hr/>

<h2 id="why-write-correspondence-is-not-a-read-correspondence-substitute">7. Why Write Correspondence Is Not a Read-Correspondence Substitute</h2>
<p>Write correspondence is not a read-correspondence substitute because:</p>

<ul>
  <li>a property read retrieves or observes property-side information,</li>
  <li>a property write applies or requests modification of property-side state.</li>
</ul>

<p>Even when both affect the same UI object and the same property family, they do not authorize one category to stand in for the other.</p>

<p>If such substitution were allowed:</p>

<ul>
  <li>observation and mutation would become harder to distinguish,</li>
  <li>side-effect reasoning would become less stable,</li>
  <li>property interaction semantics would become more implementation-dependent.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some property-write relations,</li>
  <li>but property-read correspondence has been wrongly collapsed into property-write correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid read/write distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>UI-object property correspondence survives
but is not correctly preserved
if read-facing correspondence
has been replaced by write-facing correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the distinction between observation-oriented and mutation-oriented UI-object property operations.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>write-side relation remains recoverable,</li>
  <li>property-read correspondence has been collapsed or lost,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
