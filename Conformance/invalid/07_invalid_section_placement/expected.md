<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Expected Outcome — Invalid Conformance Case 07</h1>

<p align="center">
  <strong>Invalid section placement must fail during structural validation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Expected classification</h2>

<ul>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> invalid</li>
  <li><strong>Expected meaning:</strong> not established</li>
  <li><strong>Expected rejection reason:</strong> source object placed in the wrong top-level section family</li>
</ul>

<hr/>

<h2>Interpretation</h2>

<p>
This fixture is valid JSON and includes the required top-level canonical source sections needed for a minimal FROG source file.
The failure is therefore not a parsing failure and not a missing-root-section failure.
</p>

<p>
The failure occurs because a widget declaration is placed under <code>diagram</code> instead of under the appropriate front-panel-owned source area.
This violates canonical source section ownership and must be rejected as a structural-source failure.
</p>

<p>
Meaning must not be established for this case.
Downstream derivation, lowering, backend-facing handoff, or runtime-side execution must not be claimed as conforming outcomes for this fixture.
</p>

<hr/>

<h2>Stage reading</h2>

<pre><code>loadability
   -> yes

structural validity
   -> no

validated meaning
   -> not established

downstream derivation
   -> must not proceed as a conforming path
</code></pre>

<hr/>

<h2>Boundary being enforced</h2>

<pre><code>parseable JSON
    !=
structurally valid canonical source

front_panel-owned source content
    !=
diagram-owned source content

implementation tolerance
    !=
published conformance acceptance
</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case must be classified as a structural rejection caused by invalid section placement.
It must not be reclassified as a semantic rejection and must not be silently repaired.
</p>
