<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 69</h1>

<p align="center">
  <strong>Structure family identity must remain recoverable after derivation</strong><br/>
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
  <li><a href="#why-generic-structure-presence-is-not-enough">7. Why Generic Structure Presence Is Not Enough</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>69_structure_family_identity_must_remain_recoverable_after_derivation</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> when validated meaning contains an explicit structure-family distinction, canonical execution IR must preserve recoverable structure-family identity rather than reducing all structures to one generic region container category.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects structure-family identity at the canonical execution IR layer.</p>

<p>The preserved rule is:</p>

<pre><code>explicit structure-family distinction
                -&gt;
recoverable structure-family identity after derivation
</code></pre>

<p>A structure may be execution-facing and normalized without losing the fact that it belongs to a particular structure family.</p>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>mere existence of regions or structure-like carriers, and</li>
  <li>recoverable structure-family identity.</li>
</ul>

<p>The preserved distinction is:</p>

<pre><code>generic structure presence
            !=
recoverable structure-family identity
</code></pre>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains a structure whose family identity is architecturally meaningful.</p>

<p>After derivation into canonical execution IR:</p>

<ul>
  <li>the structure remains representable as an execution-facing object,</li>
  <li>its regions and terminals may remain explicit,</li>
  <li>its family identity also remains recoverable,</li>
  <li>the reader can still distinguish which structure-family semantics are in scope without depending on runtime-private interpretation.</li>
</ul>

<p>This does not require copying the source format. It requires preserving the structure-family boundary.</p>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>the structure remains attributable as belonging to a recoverable family,</li>
  <li>family identity is not inferred only from terminal count, region count, or incidental connectivity,</li>
  <li>family identity is not delegated to runtime-private behavior,</li>
  <li>canonical execution IR remains sufficient for a conforming reader to recover the family distinction.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>structure identity
+
region/terminal preservation
+
recoverable family category
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-generic-structure-presence-is-not-enough">7. Why Generic Structure Presence Is Not Enough</h2>
<p>Generic structure presence is not enough because different structure families may carry different execution-facing obligations and interpretation boundaries.</p>

<p>If family identity is erased:</p>

<ul>
  <li>distinct structure semantics may be blurred,</li>
  <li>downstream readers may reconstruct different meaning from the same IR,</li>
  <li>conformance loses the ability to check whether derivation preserved the correct family boundary,</li>
  <li>structure interpretation risks sliding into implementation folklore.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that structure-family identity remains recoverable in canonical execution IR after derivation.</p>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-published recoverability corridor around:</p>

<ul>
  <li>region ownership,</li>
  <li>structure-boundary terminals,</li>
  <li>structure terminal roles.</li>
</ul>

<p>It adds the immediately adjacent closure:</p>

<pre><code>structure internals are not enough
the family boundary itself
must also remain recoverable
</code></pre>

<p>That keeps canonical execution IR strong enough to support later lowering and backend-facing interpretation without collapsing architectural structure categories too early.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>a structure-family distinction exists upstream,</li>
  <li>that family identity remains recoverable after derivation,</li>
  <li>the case is valid.</li>
</ul>
