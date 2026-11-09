<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 72</h1>

<p align="center">
  <strong>Correspondence category loss must not be treated as valid canonical execution IR</strong><br/>
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
  <li><a href="#why-an-untyped-link-is-not-enough">7. Why an Untyped Link Is Not Enough</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>72_correspondence_category_loss_must_not_be_treated_as_valid_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves the existence of correspondence links but does not preserve recoverable correspondence category.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects reduction of typed correspondence into generic untyped linkage.</p>

<p>The rejected collapse is:</p>

<pre><code>recoverable correspondence category
                -&gt;
generic correspondence link only
</code></pre>

<p>That collapse is invalid because canonical execution IR must preserve category-level derivation meaning where correspondence categories are part of the public architecture.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>recoverable correspondence presence, and</li>
  <li>recoverable correspondence category.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>a link still exists
        therefore
its category no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program derives to canonical execution IR containing explicit correspondence links to source-side anchors.</p>

<p>During IR construction:</p>

<ul>
  <li>the correspondence links remain present,</li>
  <li>the links may still point to valid anchors,</li>
  <li>but the IR no longer preserves what kind of correspondence each link represents,</li>
  <li>or category can only be reconstructed from external conventions or implementation-private behavior.</li>
</ul>

<p>The result is an IR that preserves some linkage but loses the classification needed for conformance-grade reading.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>typed correspondence is replaced by generic untyped linkage,</li>
  <li>distinct correspondence categories are merged into one ambiguous class,</li>
  <li>category is omitted while link targets remain,</li>
  <li>category recovery depends on runtime-private interpretation or undocumented heuristics,</li>
  <li>the public IR reader can no longer determine whether a given relation is primary, non-primary, contributory, or otherwise category-distinct where that distinction is required.</li>
</ul>

<p>Any of those situations breaks preservation.</p>

<hr/>

<h2 id="why-an-untyped-link-is-not-enough">7. Why an Untyped Link Is Not Enough</h2>
<p>An untyped link is not enough because different correspondence categories support different architectural readings.</p>

<p>Without category:</p>

<ul>
  <li>identity and attribution boundaries blur,</li>
  <li>intentional non-primary relation can be confused with accidental loss,</li>
  <li>contributor plurality can be hidden,</li>
  <li>independent implementations may silently diverge in how they interpret the same IR linkage.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>links may still be present in IR,</li>
  <li>but category loss makes the canonical execution IR invalid.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid correspondence-category preservation case.</p>

<p>It closes a precise local gap:</p>

<pre><code>correspondence survives
but is not fully preserved
if category has disappeared
</code></pre>

<p>That keeps canonical execution IR explainable at the same level of precision as the surrounding attribution and identity cases.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>correspondence links remain in canonical execution IR,</li>
  <li>their categories are no longer recoverable,</li>
  <li>the resulting IR is architecturally invalid,</li>
  <li>the case must be rejected.</li>
</ul>
