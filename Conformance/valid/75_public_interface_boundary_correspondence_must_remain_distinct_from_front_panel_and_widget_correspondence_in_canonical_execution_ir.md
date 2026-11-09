<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 75</h1>

<p align="center">
  <strong>Public interface boundary correspondence must remain distinct from front-panel and widget correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ui-correspondence-cannot-replace-interface-boundary-correspondence">7. Why UI Correspondence Cannot Replace Interface-Boundary Correspondence</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>75_public_interface_boundary_correspondence_must_remain_distinct_from_front_panel_and_widget_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR contains recoverable correspondence for public interface boundary participation and also recoverable correspondence for front-panel or widget-side material, those correspondences must remain explicitly distinct and must not be collapsed into one UI-facing relation class.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a core architectural boundary that already exists upstream:</p>

<pre><code>public interface
            !=
front panel
            !=
widget participation
</code></pre>

<p>That same distinction must remain readable at the canonical execution IR layer when correspondence records or recoverable source-side relations are present.</p>

<p>The preserved rule is:</p>

<pre><code>public interface boundary correspondence
            !=
front-panel correspondence
            !=
widget correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between three different source-side relation families:</p>

<ul>
  <li>public interface boundary correspondence,</li>
  <li>front-panel correspondence,</li>
  <li>widget correspondence.</li>
</ul>

<p>They may coexist. They must not be conflated.</p>

<p>Public interface boundary correspondence answers:</p>
<ul>
  <li>which source-side public interface boundary the execution-facing object remains tied to.</li>
</ul>

<p>Front-panel or widget correspondence answers different questions, such as:</p>
<ul>
  <li>which interaction-facing surface, representation, or widget-side object is involved.</li>
</ul>

<p>Those are not the same architectural role.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both:</p>

<ul>
  <li>a public interface boundary that participates in canonical execution meaning, and</li>
  <li>front-panel or widget-side material that may also participate in interaction-facing or UI-adjacent aspects of the source model.</li>
</ul>

<p>In the valid case:</p>

<ul>
  <li>the canonical execution IR may preserve correspondence to both sides where applicable,</li>
  <li>public interface boundary correspondence remains explicitly identifiable as interface-boundary correspondence,</li>
  <li>front-panel or widget-side correspondence remains explicitly identifiable as UI-side correspondence,</li>
  <li>the reader can distinguish execution-boundary anchoring from UI-side relation without runtime-private inference.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>public interface boundary correspondence remains recoverable as a distinct category,</li>
  <li>front-panel or widget-side correspondence remains recoverable as a distinct category where present,</li>
  <li>UI-side relations do not replace interface-boundary anchoring,</li>
  <li>interface-boundary anchoring does not erase UI-side relation where that relation is also explicitly preserved,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to tell which relation belongs to which architectural layer.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>execution-facing object
+
recoverable public interface boundary relation
+
recoverable UI-side relation where applicable
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-ui-correspondence-cannot-replace-interface-boundary-correspondence">7. Why UI Correspondence Cannot Replace Interface-Boundary Correspondence</h2>
<p>UI correspondence cannot replace interface-boundary correspondence because public interface belongs to the execution-facing public boundary of the program, whereas front panel and widgets belong to interaction-facing material.</p>

<p>If those are blurred together:</p>

<ul>
  <li>execution boundary meaning becomes less precise,</li>
  <li>public interface anchoring can be mistaken for UI presence,</li>
  <li>widget-side relation can be overinterpreted as semantic boundary participation,</li>
  <li>independent implementations may silently diverge in how they reconstruct boundary ownership.</li>
</ul>

<p>The architecture therefore requires category-level separation across these correspondence families.</p>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li>public interface boundary correspondence, and</li>
  <li>front-panel or widget-side correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-published architectural line that keeps:</p>

<ul>
  <li>public interface distinct from widget participation,</li>
  <li>front-panel presence distinct from execution meaning,</li>
  <li>widget_reference distinct from widget_value,</li>
  <li>identity and correspondence categories distinct inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>source-side correspondence is not sufficiently preserved
if execution-boundary anchoring
is collapsed into UI-side correspondence
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves both public-interface-boundary relation and UI-side relation where applicable,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
