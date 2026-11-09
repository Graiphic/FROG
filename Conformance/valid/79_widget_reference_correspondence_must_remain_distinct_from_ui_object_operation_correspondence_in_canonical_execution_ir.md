<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 79</h1>

<p align="center">
  <strong>Widget reference correspondence must remain distinct from UI-object-operation correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-reference-correspondence-cannot-be-collapsed-into-operation-correspondence">7. Why Reference Correspondence Cannot Be Collapsed into Operation Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>79_widget_reference_correspondence_must_remain_distinct_from_ui_object_operation_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to widget-reference participation and also preserves correspondence to UI-object operations performed through that reference-facing path, those correspondences must remain explicitly distinct and recoverable rather than merged into one object-facing UI relation category.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a local but critical UI distinction:</p>

<pre><code>widget_reference
            !=
UI-object operation
</code></pre>

<p>A widget reference may enable later property reads, property writes, method invocations, or other UI-object-oriented operations. That enablement does not make the reference correspondence identical to the operation correspondence.</p>

<p>The preserved rule is:</p>

<pre><code>widget reference correspondence
            !=
UI-object-operation correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to reference-facing widget participation, and</li>
  <li>correspondence to UI-object operations that may be performed through that reference-facing participation.</li>
</ul>

<p>They are related but not equivalent.</p>

<p>They answer different questions:</p>

<ul>
  <li><strong>Widget reference correspondence</strong> answers which widget-side object reference participation is present.</li>
  <li><strong>UI-object-operation correspondence</strong> answers which object-level interaction path is exercised through that reference-capable participation.</li>
</ul>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains widget-side material with reference-facing participation.</p>

<p>That reference-facing participation may support later UI-object operations such as:</p>

<ul>
  <li>property read,</li>
  <li>property write,</li>
  <li>method invocation,</li>
  <li>other explicitly standardized object-oriented UI interactions.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves correspondence to widget-reference participation as a distinct category,</li>
  <li>canonical execution IR preserves correspondence to the UI-object operation as a distinct category where such operation is present,</li>
  <li>the reader can distinguish the object reference path from the operation path that uses it,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>widget reference correspondence remains recoverable as reference correspondence,</li>
  <li>UI-object-operation correspondence remains recoverable as operation correspondence where present,</li>
  <li>reference participation is not reclassified as operation participation merely because an operation is later applied through it,</li>
  <li>operation participation is not used as the sole surviving explanation of reference participation,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two categories directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>reference-side relation
+
operation-side relation where applicable
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-reference-correspondence-cannot-be-collapsed-into-operation-correspondence">7. Why Reference Correspondence Cannot Be Collapsed into Operation Correspondence</h2>
<p>Reference correspondence cannot be collapsed into operation correspondence because a reference path and an operation path do not express the same architectural claim.</p>

<ul>
  <li>A reference path expresses access to a widget-side object identity or object-facing handle.</li>
  <li>An operation path expresses an action or interaction performed through that object-facing path.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>mere object-reference availability can be mistaken for active UI-object manipulation,</li>
  <li>operation-specific semantics can be projected backwards onto all reference participation,</li>
  <li>different implementations may silently reconstruct different UI-object behavior from the same IR,</li>
  <li>canonical execution IR becomes less precise as a public derivation surface.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>widget reference correspondence, and</li>
  <li>UI-object-operation correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinctions around:</p>

<ul>
  <li>widget value versus widget reference,</li>
  <li>widget reference versus UI-object primitive behavior,</li>
  <li>correspondence-category preservation inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>UI-side correspondence is not sufficiently preserved
if reference-facing relation
is collapsed into operation-facing relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves widget reference relation and operation relation where applicable,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
