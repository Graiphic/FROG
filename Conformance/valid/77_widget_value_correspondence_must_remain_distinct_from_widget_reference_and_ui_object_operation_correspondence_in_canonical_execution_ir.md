<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 77</h1>

<p align="center">
  <strong>Widget value correspondence must remain distinct from widget reference and UI-object-operation correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-value-correspondence-cannot-be-collapsed-into-reference-or-ui-operation-correspondence">7. Why Value Correspondence Cannot Be Collapsed into Reference or UI-Operation Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>77_widget_value_correspondence_must_remain_distinct_from_widget_reference_and_ui_object_operation_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence for widget value participation and also preserves correspondence for widget reference or UI-object operation participation, those correspondences must remain explicitly distinct rather than being normalized into one generic UI relation class.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a core distinction inside interaction-facing participation:</p>

<pre><code>widget_value
            !=
widget_reference
            !=
UI-object operation correspondence
</code></pre>

<p>Those categories may be related, but they are not interchangeable.</p>

<p>The preserved rule is:</p>

<pre><code>widget value correspondence
            !=
widget reference correspondence
            !=
UI-object-operation correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between three different relation families:</p>

<ul>
  <li>correspondence to widget value participation,</li>
  <li>correspondence to widget reference participation,</li>
  <li>correspondence to UI-object operations such as property-oriented or method-oriented participation.</li>
</ul>

<p>They may coexist in one program and even around related UI-side material. They must remain distinguishable.</p>

<p>They answer different questions:</p>

<ul>
  <li><strong>Widget value correspondence</strong> answers which value-facing UI participation is in scope.</li>
  <li><strong>Widget reference correspondence</strong> answers which reference-facing UI object participation is in scope.</li>
  <li><strong>UI-object-operation correspondence</strong> answers which UI-object interaction path is being exercised.</li>
</ul>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains interaction-facing material in which:</p>

<ul>
  <li>a widget contributes a value-facing path,</li>
  <li>the same or another widget also contributes a reference-facing path,</li>
  <li>UI-object-oriented operations may be performed through that reference-facing path.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves value-side correspondence as value-side correspondence,</li>
  <li>reference-side correspondence remains reference-side correspondence,</li>
  <li>UI-object-operation correspondence remains operation correspondence,</li>
  <li>the reader can distinguish value participation from object-reference participation and from object-operation participation without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>widget value correspondence remains recoverable as a distinct category,</li>
  <li>widget reference correspondence remains recoverable as a distinct category where present,</li>
  <li>UI-object-operation correspondence remains recoverable as a distinct category where present,</li>
  <li>value-side relations do not become object-reference relations merely because they involve the same widget-side material,</li>
  <li>reference-side relations do not become UI-operation relations merely because operations are later performed through them,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the three categories directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>value-side relation
+
reference-side relation where applicable
+
UI-operation relation where applicable
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-value-correspondence-cannot-be-collapsed-into-reference-or-ui-operation-correspondence">7. Why Value Correspondence Cannot Be Collapsed into Reference or UI-Operation Correspondence</h2>
<p>Widget value correspondence cannot be collapsed into reference or UI-operation correspondence because they belong to different execution-facing and interaction-facing meanings.</p>

<p>If they are blurred together:</p>

<ul>
  <li>value flow can be mistaken for object-level control,</li>
  <li>reference participation can be mistaken for value participation,</li>
  <li>property or method interaction can be mistaken for ordinary widget value exchange,</li>
  <li>independent implementations may silently reconstruct different UI semantics from the same IR.</li>
</ul>

<p>The architecture therefore requires these categories to remain separately recoverable.</p>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>widget value correspondence,</li>
  <li>widget reference correspondence,</li>
  <li>UI-object-operation correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinctions around:</p>

<ul>
  <li>widget value versus widget reference,</li>
  <li>UI reference participation versus UI-object primitive operation,</li>
  <li>source-side correspondence category preservation in canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>UI-side correspondence is not sufficiently preserved
if value-facing relation
is collapsed into reference-facing or operation-facing relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves widget value relation and other UI-side relations where applicable,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
