<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 73</h1>

<p align="center">
  <strong>Primary, non-primary, and contributor correspondence categories must remain distinct in canonical execution IR</strong><br/>
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
  <li><a href="#why-category-separation-matters">7. Why Category Separation Matters</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>73_primary_non_primary_and_contributor_correspondence_categories_must_remain_distinct_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR uses correspondence categories such as primary, non-primary, and contributor, those categories must remain explicitly distinct and recoverable rather than normalized into one ambiguous relation class.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case strengthens the category-preservation rule by requiring local category separation.</p>

<p>The preserved rule is:</p>

<pre><code>primary correspondence
!= non-primary correspondence
!= contributor correspondence
</code></pre>

<p>These are not merely stylistic labels. They support different architectural readings.</p>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between three locally important correspondence categories:</p>

<ul>
  <li><strong>primary</strong> — the correspondence that anchors the execution object’s main recoverable source-side relation,</li>
  <li><strong>non-primary</strong> — an explicit additional relation that is not the object’s primary execution anchor,</li>
  <li><strong>contributor</strong> — a source-side relation indicating material contribution without collapsing into primary identity.</li>
</ul>

<p>The preserved distinction is:</p>

<pre><code>primary
non-primary
contributor

must remain distinct
</code></pre>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program derives to canonical execution IR containing one execution-facing object with multiple recoverable source-side relations.</p>

<p>In the valid case:</p>

<ul>
  <li>one relation remains explicitly primary,</li>
  <li>other relations remain explicitly non-primary or contributor where appropriate,</li>
  <li>the categories are not merged,</li>
  <li>the reader can distinguish identity anchoring from additional relation and from contribution.</li>
</ul>

<p>The valid case therefore preserves not just multiple links, but the internal discipline among those links.</p>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>primary correspondence remains identifiable as primary,</li>
  <li>non-primary correspondence remains identifiable as non-primary,</li>
  <li>contributor relation remains identifiable as contributor,</li>
  <li>none of these categories is left to inference from surrounding context alone,</li>
  <li>the canonical IR itself remains sufficient to recover the distinction.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>multiple explicit correspondences
+
explicit category separation
+
recoverable public reading
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-category-separation-matters">7. Why Category Separation Matters</h2>
<p>Category separation matters because each category supports a different architectural claim.</p>

<ul>
  <li><strong>Primary</strong> supports recoverable identity anchoring.</li>
  <li><strong>Non-primary</strong> supports explicit additional relation without identity substitution.</li>
  <li><strong>Contributor</strong> supports explicit material participation without forcing identity collapse.</li>
</ul>

<p>If those categories blur together, then attribution, identity, and derivation explanation all become weaker.</p>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction among primary, non-primary, and contributor correspondence categories where they are in scope.</p>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case is the natural local strengthening of:</p>

<ul>
  <li>intentional non-primary correspondence,</li>
  <li>multi-contributor attribution,</li>
  <li>declaration reference versus primary execution identity,</li>
  <li>recoverable correspondence category.</li>
</ul>

<p>It closes the next coherent step:</p>

<pre><code>preserving categories individually
is not enough
unless their distinctions remain explicit
when they coexist
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR contains primary, non-primary, and contributor relations in scope,</li>
  <li>those categories remain explicitly distinct and recoverable,</li>
  <li>the case is valid.</li>
</ul>
