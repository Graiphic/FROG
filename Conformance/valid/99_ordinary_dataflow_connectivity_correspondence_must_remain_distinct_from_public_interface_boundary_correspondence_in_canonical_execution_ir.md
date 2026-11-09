<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 99</h1>

<p align="center">
  <strong>Ordinary dataflow connectivity correspondence must remain distinct from public-interface-boundary correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ordinary-dataflow-connectivity-cannot-be-collapsed-into-public-interface-boundary-correspondence">7. Why Ordinary Dataflow Connectivity Cannot Be Collapsed into Public-Interface-Boundary Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>99_ordinary_dataflow_connectivity_correspondence_must_remain_distinct_from_public_interface_boundary_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to ordinary dataflow connectivity and also preserves correspondence to public-interface-boundary participation, those correspondences must remain explicitly distinct and recoverable rather than being normalized into one generic ingress, egress, or graph-edge relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a critical distinction between internal graph structure and public program boundary structure:</p>

<pre><code>ordinary dataflow connectivity
            !=
public-interface-boundary participation
</code></pre>

<p>Both may appear as connectivity-facing structure. They do not express the same architectural meaning.</p>

<p>The preserved rule is:</p>

<pre><code>ordinary dataflow connectivity correspondence
            !=
public-interface-boundary correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to ordinary internal dataflow connectivity participation, and</li>
  <li>correspondence to public-interface-boundary participation.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>Ordinary dataflow connectivity correspondence</strong> answers which ordinary internal dependency or value-transfer relation participates in canonical execution structure.</li>
  <li><strong>Public-interface-boundary correspondence</strong> answers which public ingress or egress participation remains part of the canonical execution-facing structure.</li>
</ul>

<p>These categories may coexist in one program and may be connected through longer execution paths. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both:</p>

<ul>
  <li>ordinary internal dataflow connectivity between execution-facing objects, and</li>
  <li>public-interface-boundary participation through explicit public inputs or outputs.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves ordinary dataflow connectivity correspondence as ordinary connectivity correspondence,</li>
  <li>canonical execution IR preserves public-interface-boundary correspondence as public-boundary correspondence,</li>
  <li>the reader can distinguish internal graph connectivity from public program-boundary participation,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>ordinary dataflow connectivity correspondence remains recoverable as ordinary connectivity correspondence,</li>
  <li>public-interface-boundary correspondence remains recoverable as public-boundary correspondence,</li>
  <li>internal graph connectivity is not reclassified as public-interface participation merely because a path eventually touches a public port,</li>
  <li>public-interface participation is not used as the sole surviving explanation of ordinary internal connectivity when such connectivity is in scope,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two correspondence families directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>ordinary dataflow-connectivity relation
+
public-interface-boundary relation
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-ordinary-dataflow-connectivity-cannot-be-collapsed-into-public-interface-boundary-correspondence">7. Why Ordinary Dataflow Connectivity Cannot Be Collapsed into Public-Interface-Boundary Correspondence</h2>
<p>Ordinary dataflow connectivity cannot be collapsed into public-interface-boundary correspondence because internal graph connectivity is not the same architectural relation as public ingress or egress participation.</p>

<ul>
  <li>Ordinary dataflow connectivity identifies internal dependency or value transfer.</li>
  <li>Public-interface-boundary correspondence identifies external program-boundary participation.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>internal execution structure can be mistaken for public boundary structure,</li>
  <li>public boundary structure can be mistaken for ordinary internal graph structure,</li>
  <li>execution interpretation becomes less precise,</li>
  <li>canonical execution IR loses part of its boundary discipline.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>ordinary dataflow connectivity correspondence, and</li>
  <li>public-interface-boundary correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>ordinary dataflow connectivity versus explicit UI sequencing,</li>
  <li>public interface versus front panel and widget participation,</li>
  <li>public-interface-boundary correspondence versus UI-side correspondence,</li>
  <li>execution-facing correspondence categories inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>execution-facing correspondence is not sufficiently preserved
if ordinary dataflow-connectivity relation
is collapsed into public-interface-boundary relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves ordinary dataflow-connectivity relation and public-interface-boundary relation,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
