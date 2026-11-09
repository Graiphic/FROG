<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 71</h1>

<p align="center">
  <strong>Correspondence category must remain recoverable in canonical execution IR</strong><br/>
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
  <li><a href="#why-recoverable-link-presence-is-not-enough">7. Why Recoverable Link Presence Is Not Enough</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>71_correspondence_category_must_remain_recoverable_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> when canonical execution IR contains a recoverable correspondence to source-side material, the category of that correspondence must remain explicit and recoverable, rather than reduced to an untyped generic link.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case protects the distinction between correspondence existence and correspondence category.</p>

<p>The preserved rule is:</p>

<pre><code>recoverable correspondence
+
recoverable correspondence category
                -&gt;
valid canonical execution IR
</code></pre>

<p>A link back to source-side material is not enough by itself. A conforming reader must also be able to recover what kind of correspondence is being asserted.</p>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>mere presence of a correspondence link, and</li>
  <li>recoverable classification of that correspondence.</li>
</ul>

<p>The preserved distinction is:</p>

<pre><code>generic correspondence presence
            !=
recoverable correspondence category
</code></pre>

<p>For example, a conforming reader must not be forced to guess whether a correspondence is primary, non-primary, contributory, explanatory, or otherwise category-significant where such distinctions are architecturally in scope.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program derives to canonical execution IR containing one or more explicit correspondences back to source-side anchors.</p>

<p>In the valid case:</p>

<ul>
  <li>correspondence presence remains explicit,</li>
  <li>correspondence category remains recoverable,</li>
  <li>the category is represented at the public IR layer rather than hidden in runtime-private logic,</li>
  <li>a conforming reader can distinguish what kind of relation is being asserted.</li>
</ul>

<p>The valid case does not require one universal implementation strategy. It requires public recoverability of the category boundary.</p>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true:</p>

<ul>
  <li>correspondence links remain explicit where they are part of the canonical IR posture,</li>
  <li>their categories remain recoverable,</li>
  <li>category meaning is not delegated to runtime-private interpretation,</li>
  <li>the IR remains sufficient for a conforming reader to distinguish correspondence kinds without guesswork.</li>
</ul>

<p>The preserved form is:</p>

<pre><code>explicit correspondence
+
explicit category
+
public recoverability
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-recoverable-link-presence-is-not-enough">7. Why Recoverable Link Presence Is Not Enough</h2>
<p>Recoverable link presence is not enough because different correspondence categories carry different architectural meanings.</p>

<p>If category is lost while the link remains:</p>

<ul>
  <li>primary anchoring may be confused with non-primary relation,</li>
  <li>contribution may be confused with identity,</li>
  <li>accidental omission may be confused with intentional categorization,</li>
  <li>independent implementations may apply different hidden readings to the same IR.</li>
</ul>

<p>The IR must therefore preserve not only that correspondence exists, but what category of correspondence it is.</p>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that correspondence category remains recoverable in canonical execution IR wherever correspondence is part of the public derivation explanation.</p>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case extends the already-published line around:</p>

<ul>
  <li>recoverable source attribution,</li>
  <li>intentional non-primary correspondence,</li>
  <li>recoverable source anchoring of primary execution objects.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>correspondence is not fully preserved
if category disappears
while the link alone survives
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>canonical execution IR contains recoverable correspondence,</li>
  <li>the category of that correspondence also remains recoverable,</li>
  <li>the case is valid.</li>
</ul>
