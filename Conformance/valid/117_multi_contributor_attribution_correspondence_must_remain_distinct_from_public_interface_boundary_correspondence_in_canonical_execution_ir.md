<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 117</h1>

<p align="center">
  <strong>Multi-contributor attribution correspondence must remain distinct from public-interface-boundary correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-multi-contributor-attribution-cannot-be-collapsed-into-public-interface-boundary-correspondence">7. Why Multi-Contributor Attribution Cannot Be Collapsed into Public-Interface-Boundary Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>117_multi_contributor_attribution_correspondence_must_remain_distinct_from_public_interface_boundary_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to multi-contributor attribution and also preserves correspondence to public-interface-boundary participation, those correspondences must remain explicitly distinct and recoverable rather than being normalized into one generic externally visible source-anchor relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a critical distinction between plural source-origin explanation and public program-boundary participation:</p>

<pre><code>multi-contributor attribution
            !=
public-interface boundary participation
</code></pre>

<p>Both may be recoverable and both may point back to source-visible material. They do not express the same architectural meaning.</p>

<p>The preserved rule is:</p>

<pre><code>multi-contributor attribution correspondence
            !=
public-interface-boundary correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to multi-contributor attribution, and</li>
  <li>correspondence to public-interface-boundary participation.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>Multi-contributor attribution correspondence</strong> answers which plural set of source-side contributors materially participates in the derivation explanation of a canonical execution-facing object.</li>
  <li><strong>Public-interface-boundary correspondence</strong> answers which public ingress or egress participation remains part of the canonical execution-facing structure.</li>
</ul>

<p>These categories may coexist around one execution-facing object or one local derivation slice. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both:</p>

<ul>
  <li>execution-facing objects with recoverable multi-contributor attribution, and</li>
  <li>public-interface-boundary participation through explicit public inputs or outputs.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves multi-contributor attribution correspondence as attribution correspondence,</li>
  <li>canonical execution IR preserves public-interface-boundary correspondence as public-boundary correspondence,</li>
  <li>the reader can distinguish plural source-origin explanation from public program-boundary participation,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>multi-contributor attribution correspondence remains recoverable as attribution correspondence,</li>
  <li>public-interface-boundary correspondence remains recoverable as public-boundary correspondence,</li>
  <li>plural source-origin explanation is not reclassified as public-interface participation merely because one contributor may also be exposed through a public boundary,</li>
  <li>public-interface participation is not used as the sole surviving explanation of multi-contributor attribution when such attribution is in scope,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two correspondence families directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>multi-contributor attribution relation
+
public-interface-boundary relation
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-multi-contributor-attribution-cannot-be-collapsed-into-public-interface-boundary-correspondence">7. Why Multi-Contributor Attribution Cannot Be Collapsed into Public-Interface-Boundary Correspondence</h2>
<p>Multi-contributor attribution cannot be collapsed into public-interface-boundary correspondence because plural source-origin explanation is not the same architectural relation as public program-boundary participation.</p>

<ul>
  <li>Multi-contributor attribution identifies a materially contributing source-side set.</li>
  <li>Public-interface-boundary correspondence identifies external program-boundary participation.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>plural attribution can be mistaken for public boundary structure,</li>
  <li>public boundary structure can be mistaken for derivation ownership,</li>
  <li>derivation interpretation becomes less precise,</li>
  <li>canonical execution IR loses part of its attribution discipline.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>multi-contributor attribution correspondence, and</li>
  <li>public-interface-boundary correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>multi-contributor attribution,</li>
  <li>primary / non-primary / contributor categories,</li>
  <li>primary source attribution versus public-interface-boundary participation,</li>
  <li>execution-facing correspondence categories inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>execution-facing correspondence is not sufficiently preserved
if multi-contributor attribution relation
is collapsed into public-interface-boundary relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves multi-contributor attribution relation and public-interface-boundary relation,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
