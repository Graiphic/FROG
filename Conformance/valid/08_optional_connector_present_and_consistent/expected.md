<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Expected Outcome — Valid Conformance Case 08</h1>

<p align="center">
  <strong>Optional connector presence is valid when it remains consistent with the declared public interface</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Expected classification</h2>

<ul>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> valid</li>
  <li><strong>Expected meaning:</strong> established or eligible to be established for the represented slice</li>
  <li><strong>Expected preservation:</strong> connector remains a graphical projection of the declared public interface and does not create new logical ports</li>
</ul>

<hr/>

<h2>Interpretation</h2>

<p>
This fixture is valid JSON and includes the required top-level canonical source sections needed for a minimal FROG source file.
It also includes the optional <code>connector</code> section.
</p>

<p>
The case is valid because the connector remains consistent with the declared public interface.
Its connector-side entries project already-declared interface ports and do not invent new logical ports.
</p>

<p>
This means the case passes the structural-validation corridor for the tested boundary.
If no additional semantic violation exists in the fixture, validated meaning is established or may proceed to establishment for the represented slice.
</p>

<hr/>

<h2>Stage reading</h2>

<pre><code>loadability
   -&gt; yes

structural validity
   -&gt; yes

validated meaning
   -&gt; established or eligible to be established for the represented slice

downstream derivation
   -&gt; permitted if the remaining represented slice is otherwise valid
</code></pre>

<hr/>

<h2>Boundary being enforced</h2>

<pre><code>interface
    !=
connector

logical public boundary
    !=
graphical connector projection

optional connector presence
    !=
permission to invent new ports

valid connector projection
    ==
projection of declared interface ownership
</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case must be classified as valid for the tested boundary because the optional <code>connector</code> section is present and consistent with the declared public interface.
The connector is accepted as projection only, not as a second source of logical-port truth.
</p>
