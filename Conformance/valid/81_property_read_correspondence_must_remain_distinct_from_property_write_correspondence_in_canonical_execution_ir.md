<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 81</h1>

<p align="center">
  <strong>Property-read correspondence must remain distinct from property-write correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-read-correspondence-cannot-be-collapsed-into-write-correspondence">7. Why Read Correspondence Cannot Be Collapsed into Write Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>81_property_read_correspondence_must_remain_distinct_from_property_write_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to UI-object property operations, property-read correspondence and property-write correspondence must remain explicitly distinct and recoverable rather than normalized into one generic property-access relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a basic but essential UI-object operation distinction:</p>

<pre><code>property read
            !=
property write
</code></pre>

<p>Both operations may target the same UI object and even the same property name. That does not make them architecturally identical.</p>

<p>The preserved rule is:</p>

<pre><code>property-read correspondence
            !=
property-write correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to property-read participation, and</li>
  <li>correspondence to property-write participation.</li>
</ul>

<p>They are related but not equivalent.</p>

<p>They answer different questions:</p>

<ul>
  <li><strong>Property-read correspondence</strong> answers which operation obtains property-side information from a UI object.</li>
  <li><strong>Property-write correspondence</strong> answers which operation applies property-side modification or assignment to a UI object.</li>
</ul>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains UI-object interactions using property-oriented operations.</p>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves property-read correspondence as read correspondence,</li>
  <li>canonical execution IR preserves property-write correspondence as write correspondence where present,</li>
  <li>the reader can distinguish information retrieval from property mutation,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<p>This does not require one specific low-level representation. It requires preservation of the operation-category boundary.</p>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>property-read correspondence remains recoverable as read correspondence,</li>
  <li>property-write correspondence remains recoverable as write correspondence where present,</li>
  <li>read participation is not reclassified as write participation merely because both target related UI-side material,</li>
  <li>write participation is not used as the sole surviving explanation of property-side interaction when a read operation is also present,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish read and write categories directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>property-read relation
+
property-write relation where applicable
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-read-correspondence-cannot-be-collapsed-into-write-correspondence">7. Why Read Correspondence Cannot Be Collapsed into Write Correspondence</h2>
<p>Read correspondence cannot be collapsed into write correspondence because information retrieval and state mutation are not the same architectural action.</p>

<ul>
  <li>A property read observes or retrieves property-side information.</li>
  <li>A property write applies or requests property-side modification.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>observation can be mistaken for mutation,</li>
  <li>mutation can be mistaken for observation,</li>
  <li>sequencing and effect interpretation become less precise,</li>
  <li>different implementations may silently reconstruct different UI-object behavior from the same IR.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>property-read correspondence, and</li>
  <li>property-write correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>widget reference versus UI-object operation,</li>
  <li>different correspondence categories inside canonical execution IR,</li>
  <li>preservation of UI-side execution-facing meaning without collapsing operation families.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>UI-object-operation correspondence is not sufficiently preserved
if read-facing relation
is collapsed into write-facing relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves property-read relation and property-write relation where applicable,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
