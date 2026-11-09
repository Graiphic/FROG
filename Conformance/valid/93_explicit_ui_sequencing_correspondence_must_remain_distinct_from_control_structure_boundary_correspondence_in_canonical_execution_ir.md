<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 93</h1>

<p align="center">
  <strong>Explicit UI sequencing correspondence must remain distinct from control-structure boundary correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ui-sequencing-cannot-be-collapsed-into-control-structure-boundary-correspondence">7. Why UI Sequencing Cannot Be Collapsed into Control-Structure Boundary Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>93_explicit_ui_sequencing_correspondence_must_remain_distinct_from_control_structure_boundary_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to explicit UI sequencing participation and also preserves correspondence to ordinary control-structure boundary participation, those correspondences must remain explicitly distinct and recoverable rather than being normalized into one generic ordering or boundary relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a critical boundary between two different kinds of execution-facing structure:</p>

<pre><code>explicit UI sequencing
            !=
control-structure boundary participation
</code></pre>

<p>Both may constrain ordering or region transitions. They do not express the same architectural meaning.</p>

<p>The preserved rule is:</p>

<pre><code>explicit UI sequencing correspondence
            !=
control-structure boundary correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to explicit UI sequencing participation, and</li>
  <li>correspondence to ordinary control-structure boundary participation.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>Explicit UI sequencing correspondence</strong> answers which explicit UI-side sequencing boundary carries effect ordering or effect chaining.</li>
  <li><strong>Control-structure boundary correspondence</strong> answers which ordinary structure-boundary participation belongs to the execution semantics of a control structure such as region entry, region exit, selector-side boundary handling, or tunnel-like boundary participation.</li>
</ul>

<p>These categories may coexist in one program and even in nearby local regions. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both:</p>

<ul>
  <li>explicit UI sequencing participation through <code>ui_in</code> / <code>ui_out</code>, and</li>
  <li>ordinary control-structure boundary participation through a structure with explicit regions or terminals.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves explicit UI sequencing correspondence as sequencing correspondence,</li>
  <li>canonical execution IR preserves control-structure boundary correspondence as structure-boundary correspondence,</li>
  <li>the reader can distinguish UI-side effect-order participation from ordinary structure-boundary participation,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>explicit UI sequencing correspondence remains recoverable as sequencing correspondence,</li>
  <li>control-structure boundary correspondence remains recoverable as structure-boundary correspondence,</li>
  <li>UI sequencing is not reclassified as ordinary control-structure boundary participation merely because both may affect ordering or execution shape,</li>
  <li>control-structure boundary participation is not used as the sole surviving explanation of explicit UI sequencing when sequencing is in scope,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two correspondence families directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>explicit UI sequencing relation
+
control-structure boundary relation
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-ui-sequencing-cannot-be-collapsed-into-control-structure-boundary-correspondence">7. Why UI Sequencing Cannot Be Collapsed into Control-Structure Boundary Correspondence</h2>
<p>UI sequencing cannot be collapsed into control-structure boundary correspondence because explicit UI-side ordering is not the same architectural relation as ordinary structure-boundary participation.</p>

<ul>
  <li>UI sequencing identifies explicit effect-order participation around UI-facing interaction.</li>
  <li>Control-structure boundary correspondence identifies ordinary structure-owned entry, exit, or crossing participation inside general execution semantics.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>UI-side effect ordering can be mistaken for ordinary structure semantics,</li>
  <li>ordinary structure semantics can be mistaken for UI-specific sequencing,</li>
  <li>execution interpretation becomes less precise,</li>
  <li>canonical execution IR loses part of its layer discipline.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>explicit UI sequencing correspondence, and</li>
  <li>control-structure boundary correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>explicit structure boundaries versus layout grouping or nesting,</li>
  <li>explicit structure terminals versus inferred region crossing,</li>
  <li>UI-object operation versus UI sequencing,</li>
  <li>explicit UI sequencing versus ordinary dataflow connectivity.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>execution-facing correspondence is not sufficiently preserved
if UI sequencing relation
is collapsed into control-structure boundary relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves explicit UI sequencing relation and control-structure boundary relation,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
