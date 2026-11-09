<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 115</h1>

<p align="center">
  <strong>Primary source attribution correspondence must remain distinct from explicit state-boundary correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-primary-source-attribution-cannot-be-collapsed-into-explicit-state-boundary-correspondence">7. Why Primary Source Attribution Cannot Be Collapsed into Explicit State-Boundary Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>115_primary_source_attribution_correspondence_must_remain_distinct_from_explicit_state_boundary_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to primary source attribution and also preserves correspondence to explicit state-boundary participation, those correspondences must remain explicitly distinct and recoverable rather than being normalized into one generic source-owned persistence or state-facing relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a critical distinction between source-origin explanation and internal state-owned execution semantics:</p>

<pre><code>primary source attribution
            !=
explicit state-boundary participation
</code></pre>

<p>Both may refer back to source-visible material and both may affect observable execution behavior. They do not express the same architectural meaning.</p>

<p>The preserved rule is:</p>

<pre><code>primary source attribution correspondence
            !=
explicit state-boundary correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to primary source attribution, and</li>
  <li>correspondence to explicit state-boundary participation.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>Primary source attribution correspondence</strong> answers which source-side origin serves as the primary attribution anchor for a canonical execution-facing object.</li>
  <li><strong>Explicit state-boundary correspondence</strong> answers which explicit state-related participation remains part of canonical execution semantics.</li>
</ul>

<p>These categories may coexist around one execution-facing object or one local derivation slice. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both:</p>

<ul>
  <li>execution-facing objects with recoverable primary source attribution, and</li>
  <li>explicit state-boundary participation through state-related execution structure.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves primary source attribution correspondence as attribution correspondence,</li>
  <li>canonical execution IR preserves explicit state-boundary correspondence as state-boundary correspondence,</li>
  <li>the reader can distinguish source-origin anchoring from state-owned execution participation,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>primary source attribution correspondence remains recoverable as attribution correspondence,</li>
  <li>explicit state-boundary correspondence remains recoverable as state-boundary correspondence,</li>
  <li>source-origin anchoring is not reclassified as state-boundary participation merely because the attributed object participates in explicit state behavior,</li>
  <li>explicit state-boundary participation is not used as the sole surviving explanation of primary source attribution when such attribution is in scope,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two correspondence families directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>primary source-attribution relation
+
explicit state-boundary relation
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-primary-source-attribution-cannot-be-collapsed-into-explicit-state-boundary-correspondence">7. Why Primary Source Attribution Cannot Be Collapsed into Explicit State-Boundary Correspondence</h2>
<p>Primary source attribution cannot be collapsed into explicit state-boundary correspondence because source-origin explanation is not the same architectural relation as state-owned execution participation.</p>

<ul>
  <li>Primary source attribution identifies the main source-side origin of an execution-facing object.</li>
  <li>Explicit state-boundary correspondence identifies state-related execution semantics.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>source-origin explanation can be mistaken for state-boundary semantics,</li>
  <li>state-boundary semantics can be mistaken for attribution ownership,</li>
  <li>derivation interpretation becomes less precise,</li>
  <li>canonical execution IR loses part of its attribution discipline.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>primary source attribution correspondence, and</li>
  <li>explicit state-boundary correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>recoverable source attribution,</li>
  <li>primary execution identity and declaration reference,</li>
  <li>explicit state-boundary correspondence versus public-interface and ordinary-connectivity correspondence,</li>
  <li>execution-facing correspondence categories inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>execution-facing correspondence is not sufficiently preserved
if primary source-attribution relation
is collapsed into explicit state-boundary relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves primary source-attribution relation and explicit state-boundary relation,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
