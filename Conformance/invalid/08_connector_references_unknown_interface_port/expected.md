<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Expected Outcome — Invalid Conformance Case 08</h1>

<p align="center">
  <strong>Connector references to unknown interface ports must fail during structural validation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Expected classification</h2>

<ul>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> invalid</li>
  <li><strong>Expected meaning:</strong> not established</li>
  <li><strong>Expected rejection reason:</strong> connector references a public port not declared by <code>interface</code></li>
</ul>

<hr/>

<h2>Interpretation</h2>

<p>
This fixture is valid JSON and includes the required top-level canonical source sections needed for a minimal FROG source file.
The failure is therefore not a parsing failure and not a missing-root-section failure.
</p>

<p>
The failure occurs because the <code>connector</code> section references a public port identifier that is not declared by the <code>interface</code> section.
This violates canonical source ownership and must be rejected as a structural-source failure.
</p>

<p>
Meaning must not be established for this case.
Downstream derivation, lowering, backend-facing handoff, or runtime-side execution must not be claimed as conforming outcomes for this fixture.
</p>

<hr/>

<h2>Stage reading</h2>

<pre><code>loadability
   -&gt; yes

structural validity
   -&gt; no

validated meaning
   -&gt; not established

downstream derivation
   -&gt; must not proceed as a conforming path
</code></pre>

<hr/>

<h2>Boundary being enforced</h2>

<pre><code>interface
    !=
connector

declared public port
    !=
connector-side invented port

parseable JSON
    !=
structurally valid canonical source

implementation tolerance
    !=
published conformance acceptance
</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case must be classified as a structural rejection caused by a connector reference to an unknown interface port.
It must not be reclassified as a semantic rejection and must not be silently repaired.
</p>
