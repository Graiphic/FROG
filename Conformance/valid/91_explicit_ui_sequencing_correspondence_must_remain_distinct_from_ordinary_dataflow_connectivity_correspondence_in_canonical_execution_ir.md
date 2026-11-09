<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 91</h1>

<p align="center">
  <strong>Explicit UI sequencing correspondence must remain distinct from ordinary dataflow connectivity correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ui-sequencing-cannot-be-collapsed-into-ordinary-dataflow-connectivity">7. Why UI Sequencing Cannot Be Collapsed into Ordinary Dataflow Connectivity</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>91_explicit_ui_sequencing_correspondence_must_remain_distinct_from_ordinary_dataflow_connectivity_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to explicit UI sequencing participation and also preserves correspondence to ordinary dataflow connectivity, those correspondences must remain explicitly distinct and recoverable rather than being normalized into one generic edge or dependency relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a critical execution distinction:</p>

<pre><code>explicit UI sequencing
            !=
ordinary dataflow connectivity
</code></pre>

<p>Both may appear as connectivity-facing structure. They do not express the same architectural meaning.</p>

<p>The preserved rule is:</p>

<pre><code>explicit UI sequencing correspondence
            !=
ordinary dataflow connectivity correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to explicit UI sequencing participation, and</li>
  <li>correspondence to ordinary dataflow connectivity participation.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>Explicit UI sequencing correspondence</strong> answers which explicit sequencing boundary carries ordering or effect-chain participation for UI-facing interaction.</li>
  <li><strong>Ordinary dataflow connectivity correspondence</strong> answers which ordinary data dependency or connectivity path participates in value-oriented execution structure.</li>
</ul>

<p>These categories may coexist near the same local region. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both:</p>

<ul>
  <li>explicit UI sequencing participation through <code>ui_in</code> / <code>ui_out</code>, and</li>
  <li>ordinary dataflow connectivity between execution-facing objects.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves explicit UI sequencing correspondence as sequencing correspondence,</li>
  <li>canonical execution IR preserves ordinary dataflow connectivity correspondence as dataflow-connectivity correspondence,</li>
  <li>the reader can distinguish UI-side effect-order participation from ordinary value-oriented connectivity,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>explicit UI sequencing correspondence remains recoverable as sequencing correspondence,</li>
  <li>ordinary dataflow connectivity correspondence remains recoverable as ordinary connectivity correspondence,</li>
  <li>UI sequencing is not reclassified as ordinary connectivity merely because both may be represented through edge-like structure,</li>
  <li>ordinary connectivity is not used as the sole surviving explanation of explicit UI sequencing when sequencing is in scope,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two correspondence families directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>explicit UI sequencing relation
+
ordinary dataflow connectivity relation
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-ui-sequencing-cannot-be-collapsed-into-ordinary-dataflow-connectivity">7. Why UI Sequencing Cannot Be Collapsed into Ordinary Dataflow Connectivity</h2>
<p>UI sequencing cannot be collapsed into ordinary dataflow connectivity because explicit UI-side ordering is not the same architectural relation as value-oriented dataflow connectivity.</p>

<ul>
  <li>UI sequencing identifies explicit effect-order participation.</li>
  <li>Ordinary dataflow connectivity identifies ordinary dependency or value transfer participation.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>effect ordering can be mistaken for value flow,</li>
  <li>value flow can be mistaken for UI-effect sequencing,</li>
  <li>execution interpretation becomes less precise,</li>
  <li>canonical execution IR loses part of its cross-layer clarity.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>explicit UI sequencing correspondence, and</li>
  <li>ordinary dataflow connectivity correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>explicit connectivity versus inferred order,</li>
  <li>UI-object operation versus UI sequencing,</li>
  <li><code>ui_in</code> versus <code>ui_out</code>,</li>
  <li>preservation of explicit UI-side structure inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>execution-facing correspondence is not sufficiently preserved
if UI sequencing relation
is collapsed into ordinary dataflow-connectivity relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves explicit UI sequencing relation and ordinary dataflow-connectivity relation,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
