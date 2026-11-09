<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 95</h1>

<p align="center">
  <strong>Explicit UI sequencing correspondence must remain distinct from explicit state-boundary correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ui-sequencing-cannot-be-collapsed-into-explicit-state-boundary-correspondence">7. Why UI Sequencing Cannot Be Collapsed into Explicit State-Boundary Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>95_explicit_ui_sequencing_correspondence_must_remain_distinct_from_explicit_state_boundary_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to explicit UI sequencing participation and also preserves correspondence to explicit state-boundary participation, those correspondences must remain explicitly distinct and recoverable rather than being normalized into one generic effect-order or persistence-facing relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a critical boundary between two different execution-facing structures:</p>

<pre><code>explicit UI sequencing
            !=
explicit state-boundary participation
</code></pre>

<p>Both may influence ordering and observable behavior. They do not express the same architectural meaning.</p>

<p>The preserved rule is:</p>

<pre><code>explicit UI sequencing correspondence
            !=
explicit state-boundary correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to explicit UI sequencing participation, and</li>
  <li>correspondence to explicit state-boundary participation.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>Explicit UI sequencing correspondence</strong> answers which explicit UI-side sequencing boundary carries effect ordering or effect chaining.</li>
  <li><strong>Explicit state-boundary correspondence</strong> answers which explicit state-related read, write, snapshot, version, merge, commit, or otherwise state-boundary participation remains part of the canonical execution-facing structure.</li>
</ul>

<p>These categories may coexist in one program and even near the same local execution area. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both:</p>

<ul>
  <li>explicit UI sequencing participation through <code>ui_in</code> / <code>ui_out</code>, and</li>
  <li>explicit state-boundary participation through state-related execution structure.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves explicit UI sequencing correspondence as sequencing correspondence,</li>
  <li>canonical execution IR preserves explicit state-boundary correspondence as state-boundary correspondence,</li>
  <li>the reader can distinguish UI-side effect-order participation from explicit state-boundary participation,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>explicit UI sequencing correspondence remains recoverable as sequencing correspondence,</li>
  <li>explicit state-boundary correspondence remains recoverable as state-boundary correspondence,</li>
  <li>UI sequencing is not reclassified as state-boundary participation merely because both may influence execution order or visible effects,</li>
  <li>state-boundary participation is not used as the sole surviving explanation of explicit UI sequencing when sequencing is in scope,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two correspondence families directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>explicit UI sequencing relation
+
explicit state-boundary relation
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-ui-sequencing-cannot-be-collapsed-into-explicit-state-boundary-correspondence">7. Why UI Sequencing Cannot Be Collapsed into Explicit State-Boundary Correspondence</h2>
<p>UI sequencing cannot be collapsed into explicit state-boundary correspondence because explicit UI-side ordering is not the same architectural relation as explicit state participation.</p>

<ul>
  <li>UI sequencing identifies explicit effect-order participation around UI-facing interaction.</li>
  <li>State-boundary correspondence identifies explicit state-related participation in execution semantics.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>UI-side effect ordering can be mistaken for state semantics,</li>
  <li>state semantics can be mistaken for UI-specific sequencing,</li>
  <li>execution interpretation becomes less precise,</li>
  <li>canonical execution IR loses part of its boundary discipline.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>explicit UI sequencing correspondence, and</li>
  <li>explicit state-boundary correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>explicit state boundaries versus inferred runtime behavior,</li>
  <li>UI-object operation versus UI sequencing,</li>
  <li>explicit UI sequencing versus ordinary dataflow connectivity,</li>
  <li>explicit UI sequencing versus control-structure boundary participation.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>execution-facing correspondence is not sufficiently preserved
if UI sequencing relation
is collapsed into explicit state-boundary relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves explicit UI sequencing relation and explicit state-boundary relation,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
