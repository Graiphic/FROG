<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 89</h1>

<p align="center">
  <strong>ui_in correspondence must remain distinct from ui_out correspondence in canonical execution IR</strong><br/>
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
  <li><a href="#why-ui-in-cannot-be-collapsed-into-ui-out">7. Why ui_in Cannot Be Collapsed into ui_out</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>89_ui_in_correspondence_must_remain_distinct_from_ui_out_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> where canonical execution IR preserves correspondence to explicit UI sequencing participation, <code>ui_in</code> correspondence and <code>ui_out</code> correspondence must remain explicitly distinct and recoverable rather than being normalized into one generic UI-ordering relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects a basic sequencing distinction:</p>

<pre><code>ui_in
   !=
ui_out
</code></pre>

<p>Both belong to explicit UI sequencing participation. They do not express the same architectural role.</p>

<p>The preserved rule is:</p>

<pre><code>ui_in correspondence
            !=
ui_out correspondence
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>correspondence to <code>ui_in</code> participation, and</li>
  <li>correspondence to <code>ui_out</code> participation.</li>
</ul>

<p>They answer different questions:</p>

<ul>
  <li><strong>ui_in correspondence</strong> answers which explicit incoming UI sequencing boundary participates in the operation-facing path.</li>
  <li><strong>ui_out correspondence</strong> answers which explicit outgoing UI sequencing boundary participates in the operation-facing path.</li>
</ul>

<p>These categories may coexist on the same UI-facing object or operation. They must not be conflated.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains explicit UI sequencing participation through <code>ui_in</code> and <code>ui_out</code>.</p>

<p>In the valid case:</p>

<ul>
  <li>canonical execution IR preserves <code>ui_in</code> correspondence as <code>ui_in</code> correspondence,</li>
  <li>canonical execution IR preserves <code>ui_out</code> correspondence as <code>ui_out</code> correspondence where present,</li>
  <li>the reader can distinguish incoming sequencing participation from outgoing sequencing participation,</li>
  <li>that distinction remains recoverable without runtime-private reconstruction.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li><code>ui_in</code> correspondence remains recoverable as <code>ui_in</code> correspondence,</li>
  <li><code>ui_out</code> correspondence remains recoverable as <code>ui_out</code> correspondence where present,</li>
  <li>incoming sequencing participation is not reclassified as outgoing sequencing participation,</li>
  <li>outgoing sequencing participation is not used as the sole surviving explanation of explicit UI sequencing when <code>ui_in</code> is in scope,</li>
  <li>the canonical execution IR remains sufficient for a conforming reader to distinguish the two categories directly.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>ui_in relation
+
ui_out relation where applicable
+
explicit distinction between them
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-ui-in-cannot-be-collapsed-into-ui-out">7. Why ui_in Cannot Be Collapsed into ui_out</h2>
<p><code>ui_in</code> cannot be collapsed into <code>ui_out</code> because incoming sequencing participation is not the same architectural role as outgoing sequencing participation.</p>

<ul>
  <li><code>ui_in</code> expresses explicit incoming UI-side sequencing.</li>
  <li><code>ui_out</code> expresses explicit outgoing UI-side sequencing.</li>
</ul>

<p>If those categories are blurred together:</p>

<ul>
  <li>entry-side sequencing can be mistaken for exit-side sequencing,</li>
  <li>effect-chain interpretation becomes less precise,</li>
  <li>UI-side ordering structure becomes harder to compare across implementations,</li>
  <li>canonical execution IR loses part of its sequencing clarity.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that canonical execution IR preserves a recoverable and explicit distinction between:</p>

<ul>
  <li><code>ui_in</code> correspondence, and</li>
  <li><code>ui_out</code> correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-established distinction line around:</p>

<ul>
  <li>UI-object operation versus UI sequencing,</li>
  <li>different UI-side correspondence categories,</li>
  <li>preservation of explicit UI sequencing structure inside canonical execution IR.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>UI sequencing correspondence is not sufficiently preserved
if incoming relation
is collapsed into outgoing relation
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR preserves <code>ui_in</code> relation and <code>ui_out</code> relation where applicable,</li>
  <li>those relations remain explicitly distinct,</li>
  <li>the case is valid.</li>
</ul>
