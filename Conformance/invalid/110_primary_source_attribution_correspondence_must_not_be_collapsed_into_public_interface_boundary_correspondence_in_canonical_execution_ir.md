<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 110</h1>

<p align="center">
  <strong>Primary source attribution correspondence must not be collapsed into public-interface-boundary correspondence in canonical execution IR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-violated">4. Boundary Being Violated</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#why-public-interface-boundary-correspondence-is-not-a-primary-source-attribution-substitute">7. Why Public-Interface-Boundary Correspondence Is Not a Primary Source-Attribution Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>110_primary_source_attribution_correspondence_must_not_be_collapsed_into_public_interface_boundary_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only public-interface-boundary correspondence where primary source attribution correspondence should also remain explicitly recoverable as a distinct source-origin relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific derivation-side collapse:</p>

<pre><code>primary source-attribution correspondence
                -&gt;
public-interface-boundary correspondence only
</code></pre>

<p>That collapse is invalid because primary source attribution is not equivalent to public-interface-boundary participation.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>primary source-attribution relation, and</li>
  <li>public-interface-boundary relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>a public-interface-boundary relation exists
        therefore
primary source-attribution correspondence no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains both execution-facing objects with primary source attribution and public-interface-boundary participation.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>public-interface-boundary relations remain recoverable,</li>
  <li>those relations may be explicit and well-shaped,</li>
  <li>but primary source attribution correspondence is omitted, erased, or folded into public-interface-boundary correspondence,</li>
  <li>or the reader is expected to infer source-origin attribution only from public program-boundary material or implementation-private handling.</li>
</ul>

<p>The result is an IR that preserves some public-boundary structure but preserves the wrong architectural category set.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>primary source attribution correspondence is omitted while public-interface-boundary correspondence remains,</li>
  <li>primary source attribution correspondence is represented as if it were merely public-interface participation,</li>
  <li>attribution recovery depends on heuristics about public ports, exposed program boundaries, or expected interface behavior,</li>
  <li>public-interface-boundary records are used as the sole surviving explanation of primary source attribution when such attribution is in scope,</li>
  <li>the canonical IR reader cannot distinguish whether a relation is source-attribution-facing or public-boundary-facing.</li>
</ul>

<p>Any of those outcomes breaks the required correspondence preservation.</p>

<hr/>

<h2 id="why-public-interface-boundary-correspondence-is-not-a-primary-source-attribution-substitute">7. Why Public-Interface-Boundary Correspondence Is Not a Primary Source-Attribution Substitute</h2>
<p>Public-interface-boundary correspondence is not a primary source-attribution substitute because:</p>

<ul>
  <li>primary source attribution correspondence identifies the main source-side origin of an execution-facing object,</li>
  <li>public-interface-boundary correspondence identifies public ingress or egress participation.</li>
</ul>

<p>Even when a source-side object is also related to a public boundary, those two categories do not authorize one to stand in for the other.</p>

<p>If such substitution were allowed:</p>

<ul>
  <li>source-origin ownership and public boundary structure would become harder to distinguish,</li>
  <li>derivation interpretation would become less stable,</li>
  <li>canonical execution IR would become more dependent on implementation-specific reconstruction.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some public-interface-boundary relations,</li>
  <li>but primary source attribution correspondence has been wrongly collapsed into public-interface-boundary correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid primary-source-attribution/public-interface-boundary distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>execution-facing correspondence survives
but is not correctly preserved
if primary source-attribution correspondence
has been replaced by public-interface-boundary correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the distinction between source-origin attribution and public program-boundary participation.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>public-interface-boundary relation remains recoverable,</li>
  <li>primary source-attribution correspondence has been collapsed or lost,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
