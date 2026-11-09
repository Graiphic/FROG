<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 83</h1>

<p align="center">
  <strong>Method-invocation correspondence must remain distinct from property-write correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-method-invocation-cannot-be-collapsed-into-property-write">7. Why Method Invocation Cannot Be Collapsed into Property Write</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>83_method_invocation_correspondence_must_remain_distinct_from_property_write_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to UI-object method invocation and also preserves correspondence to property-write participation, those correspondences must remain explicitly distinct and recoverable rather than being normalized into one generic mutation-facing UI relation class.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a critical distinction inside UI-object interaction:</p>

<pre><code>method invocation
            !=
property write
</code></pre>

<p>Both may affect the same UI object. Both may even look operationally imperative. That does not make them architecturally identical.</p>

<p>The preserved rule is:</p>

<pre><code>method-invocation correspondence
            !=
property-write correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to method-invocation participation, and</li>
  <li>correspondence to property-write participation.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>Method-invocation correspondence</strong> answers which operation invokes behavior through a named UI-object method.</li>
  <li><strong>Property-write correspondence</strong> answers which operation applies value-side modification to a named UI-object property.</li>
</ul>

<p>These categories may coexist. They must not collapse into one generic “UI mutation” relation.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains UI-object interactions using both:</p>

<ul>
  <li>method invocation, and</li>
  <li>property write.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves method-invocation correspondence as method-invocation correspondence,</li>
  <li>canonical execution IR preserves property-write correspondence as property-write correspondence where present,</li>
  <li>the reader can distinguish behavior invocation from property assignment or update,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>method-invocation correspondence remains recoverable as method-invocation correspondence,</li>
  <li>property-write correspondence remains recoverable as property-write correspondence where present,</li>
  <li>invocation participation is not reclassified as property-write participation merely because both may cause observable UI changes,</li>
  <li>property-write participation is not used as the sole surviving explanation of a method call when method invocation is in scope,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two categories directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>method-invocation relation
+
property-write relation where applicable
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-method-invocation-cannot-be-collapsed-into-property-write">7. Why Method Invocation Cannot Be Collapsed into Property Write</h2>
<p>Method invocation cannot be collapsed into property write because invoking behavior is not the same architectural action as assigning a property value.</p>

<ul>
  <li>A method invocation triggers or requests named UI-object behavior.</li>
  <li>A property write applies or requests property-state modification.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>behavior invocation can be mistaken for simple value assignment,</li>
  <li>property mutation can be mistaken for behavior-oriented call semantics,</li>
  <li>effect interpretation becomes less precise,</li>
  <li>different implementations may silently reconstruct different UI-object behavior from the same IR.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>method-invocation correspondence, and</li>
  <li>property-write correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>widget reference versus UI-object operation,</li>
  <li>property read versus property write,</li>
  <li>preservation of UI-object operation families inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>UI-object-operation correspondence is not sufficiently preserved
if behavior-invocation relation
is collapsed into property-write relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves method-invocation relation and property-write relation where applicable,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
