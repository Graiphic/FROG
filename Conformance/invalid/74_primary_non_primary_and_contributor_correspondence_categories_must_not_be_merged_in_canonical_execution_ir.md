<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 74</h1>

<p align="center">
  <strong>Primary, non-primary, and contributor correspondence categories must not be merged in canonical execution IR</strong><br/>
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
  <li><a href="#why-category-merging-is-not-benign">7. Why Category Merging Is Not Benign</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>74_primary_non_primary_and_contributor_correspondence_categories_must_not_be_merged_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR merges primary, non-primary, and contributor correspondences into one ambiguous or undifferentiated relation class.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects correspondence-category merging.</p>

<p>The rejected collapse is:</p>

<pre><code>primary / non-primary / contributor
                -&gt;
one generic ambiguous relation
</code></pre>

<p>That collapse is invalid because it erases distinctions the surrounding IR cases rely on for recoverability and attribution discipline.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>identity anchoring,</li>
  <li>additional explicit relation, and</li>
  <li>material contribution.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>all source-side relations are present
        therefore
their different categories no longer matter
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A canonical execution-facing object has multiple source-side relations that should remain explicitly categorized.</p>

<p>During IR construction:</p>

<ul>
  <li>the relations remain present,</li>
  <li>their targets may still be recoverable,</li>
  <li>but the IR merges them into one common relation category,</li>
  <li>or the distinction can be recovered only by external conventions or implementation-private heuristics.</li>
</ul>

<p>The result is an IR that still contains relations, but no longer preserves their architectural differences.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>primary and non-primary relations are collapsed into the same undifferentiated class,</li>
  <li>contributor relations are represented as if they were equivalent to primary anchoring,</li>
  <li>non-primary relations are represented as if they were merely redundant contributors,</li>
  <li>category meaning must be guessed from context rather than preserved explicitly,</li>
  <li>the public IR reader can no longer distinguish the three categories reliably.</li>
</ul>

<p>Any of those situations breaks canonical execution IR preservation.</p>

<hr/>

<h2 id="why-category-merging-is-not-benign">7. Why Category Merging Is Not Benign</h2>
<p>Category merging is not benign because it weakens the exact part of the IR that explains how execution-facing objects remain tied to source-side meaning.</p>

<p>Once the categories are merged:</p>

<ul>
  <li>identity recoverability becomes weaker,</li>
  <li>intentional non-primary relation becomes less checkable,</li>
  <li>multi-contributor attribution becomes easier to hide,</li>
  <li>different implementations may silently re-interpret the same IR differently.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>relations may still be present in canonical execution IR,</li>
  <li>but category merging makes the IR invalid.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid category-distinction case.</p>

<p>It closes the next local gap:</p>

<pre><code>multiple relations survive
but are not properly preserved
if their categories are merged
</code></pre>

<p>That keeps canonical execution IR aligned with the stronger attribution and identity discipline already established by the surrounding cases. </p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>primary, non-primary, and contributor relations are all in scope,</li>
  <li>the IR merges them into one ambiguous category,</li>
  <li>that merge is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
