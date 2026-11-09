<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 120</h1>

<p align="center">
  <strong>Multi-contributor attribution correspondence must not be collapsed into ordinary dataflow-connectivity correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ordinary-dataflow-connectivity-correspondence-is-not-a-multi-contributor-attribution-substitute">7. Why Ordinary Dataflow Connectivity Correspondence Is Not a Multi-Contributor Attribution Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>120_multi_contributor_attribution_correspondence_must_not_be_collapsed_into_ordinary_dataflow_connectivity_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only ordinary dataflow-connectivity correspondence where multi-contributor attribution correspondence should also remain explicitly recoverable as a distinct plural source-origin relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific derivation-side collapse:</p>

<pre><code>multi-contributor attribution correspondence
                -&gt;
ordinary dataflow-connectivity correspondence only
</code></pre>

<p>That collapse is invalid because plural source-origin attribution is not equivalent to ordinary dataflow connectivity.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>multi-contributor attribution relation, and</li>
  <li>ordinary dataflow-connectivity relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>an ordinary dataflow-connectivity relation exists
        therefore
multi-contributor attribution correspondence no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both execution-facing objects with multi-contributor attribution and ordinary internal dataflow connectivity.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>ordinary dataflow-connectivity relations remain recoverable,</li>
  <li>those relations may be explicit and well-shaped,</li>
  <li>but multi-contributor attribution correspondence is omitted, erased, or folded into ordinary dataflow-connectivity correspondence,</li>
  <li>or the reader is expected to infer plural source-origin attribution only from ordinary graph connectivity or implementation-private handling.</li>
</ul>

<p>The result is an IR that preserves some internal graph structure but preserves the wrong architectural category set.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>multi-contributor attribution correspondence is omitted while ordinary dataflow-connectivity correspondence remains,</li>
  <li>multi-contributor attribution correspondence is represented as if it were merely ordinary connectivity,</li>
  <li>plural attribution recovery depends on heuristics about graph edges, adjacency, or expected dataflow behavior,</li>
  <li>ordinary dataflow-connectivity records are used as the sole surviving explanation of multi-contributor attribution when such attribution is in scope,</li>
  <li>the canonical IR reader cannot distinguish whether a relation is plural-attribution-facing or ordinary-connectivity-facing.</li>
</ul>

<p>Any of those outcomes breaks the required correspondence preservation.</p>

<hr/>

<h2 id="why-ordinary-dataflow-connectivity-correspondence-is-not-a-multi-contributor-attribution-substitute">7. Why Ordinary Dataflow Connectivity Correspondence Is Not a Multi-Contributor Attribution Substitute</h2>
<p>Ordinary dataflow-connectivity correspondence is not a multi-contributor attribution substitute because:</p>

<ul>
  <li>multi-contributor attribution correspondence identifies a materially contributing source-side set,</li>
  <li>ordinary dataflow-connectivity correspondence identifies ordinary internal dependency or value transfer.</li>
</ul>

<p>Even when both may appear around the same execution-facing object, they do not authorize one category to stand in for the other.</p>

<p>If such substitution were allowed:</p>

<ul>
  <li>plural source-origin ownership and graph structure would become harder to distinguish,</li>
  <li>derivation interpretation would become less stable,</li>
  <li>canonical execution IR would become more dependent on implementation-specific reconstruction.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some ordinary dataflow-connectivity relations,</li>
  <li>but multi-contributor attribution correspondence has been wrongly collapsed into ordinary dataflow-connectivity correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid multi-contributor-attribution/ordinary-dataflow-connectivity distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>execution-facing correspondence survives
but is not correctly preserved
if multi-contributor attribution correspondence
has been replaced by ordinary dataflow-connectivity correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the distinction between plural source-origin attribution and ordinary internal graph connectivity.</p>
