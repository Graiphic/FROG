<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 87</h1>

<p align="center">
  <strong>UI-object-operation correspondence must remain distinct from ui_in and ui_out sequencing correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ui-object-operation-cannot-be-collapsed-into-ui-sequencing">7. Why UI-Object Operation Cannot Be Collapsed into UI Sequencing</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>87_ui_object_operation_correspondence_must_remain_distinct_from_ui_in_and_ui_out_sequencing_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to UI-object operations and also preserves correspondence to explicit <code>ui_in</code> or <code>ui_out</code> sequencing participation, those correspondences must remain explicitly distinct and recoverable rather than being normalized into one generic UI-effect relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a critical distinction inside UI-facing execution structure:</p>

<pre><code>UI-object operation
            !=
ui_in / ui_out sequencing
</code></pre>

<p>A UI-object operation may be sequenced by explicit UI-side ordering carriers. That relationship does not make the operation correspondence identical to the sequencing correspondence.</p>

<p>The preserved rule is:</p>

<pre><code>UI-object-operation correspondence
            !=
ui_in / ui_out sequencing correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to UI-object operation participation, and</li>
  <li>correspondence to explicit UI sequencing participation through <code>ui_in</code> or <code>ui_out</code>.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>UI-object-operation correspondence</strong> answers which UI-facing object interaction is being performed.</li>
  <li><strong>ui_in / ui_out sequencing correspondence</strong> answers which explicit sequencing boundary participates in ordering or effect chaining around UI-facing interaction.</li>
</ul>

<p>These categories may coexist around the same local UI region. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains explicit UI-object interaction together with explicit <code>ui_in</code> and <code>ui_out</code> sequencing participation.</p>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves correspondence to the UI-object operation as operation correspondence,</li>
  <li>canonical execution IR preserves correspondence to <code>ui_in</code> and <code>ui_out</code> sequencing participation as sequencing correspondence where present,</li>
  <li>the reader can distinguish what operation is being performed from what explicit sequencing boundary orders or carries that interaction,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>UI-object-operation correspondence remains recoverable as operation correspondence,</li>
  <li><code>ui_in</code> / <code>ui_out</code> sequencing correspondence remains recoverable as sequencing correspondence where present,</li>
  <li>operation participation is not reclassified as sequencing participation merely because explicit ordering exists around it,</li>
  <li>sequencing participation is not used as the sole surviving explanation of the UI-object operation itself,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two correspondence families directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>UI-object-operation relation
+
ui_in / ui_out sequencing relation where applicable
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-ui-object-operation-cannot-be-collapsed-into-ui-sequencing">7. Why UI-Object Operation Cannot Be Collapsed into UI Sequencing</h2>
<p>UI-object operation cannot be collapsed into UI sequencing because doing something to or through a UI object is not the same architectural act as expressing explicit sequencing around UI effects.</p>

<ul>
  <li>An operation relation identifies the UI-facing action or interaction.</li>
  <li>A sequencing relation identifies explicit ordering or effect-boundary participation.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>an operation can be mistaken for a pure ordering carrier,</li>
  <li>an ordering carrier can be mistaken for the operation itself,</li>
  <li>effect interpretation becomes less precise,</li>
  <li>different implementations may silently reconstruct different UI-side execution structure from the same IR.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>UI-object-operation correspondence, and</li>
  <li><code>ui_in</code> / <code>ui_out</code> sequencing correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>widget value versus widget reference,</li>
  <li>widget reference versus UI-object operation,</li>
  <li>read versus write,</li>
  <li>property access versus method invocation,</li>
  <li>preservation of UI-side categories inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>UI-side correspondence is not sufficiently preserved
if operation-facing relation
is collapsed into ui-sequencing relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves UI-object-operation relation and <code>ui_in</code> / <code>ui_out</code> sequencing relation where applicable,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
