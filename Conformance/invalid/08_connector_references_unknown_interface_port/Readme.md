<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Conformance Case 08</h1>

<p align="center">
  <strong>Connector references to unknown interface ports must be rejected as canonical source structural failure</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case identity</h2>

<ul>
  <li><strong>Case ID:</strong> <code>invalid/08_connector_references_unknown_interface_port</code></li>
  <li><strong>Kind:</strong> invalid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Primary boundary:</strong> canonical source structural validity</li>
  <li><strong>Stage of expected failure:</strong> structural validation</li>
</ul>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that a canonical FROG source file MUST reject a <code>connector</code> section that references a port not declared by the public <code>interface</code>.
</p>

<p>
It exists to make one source-ownership rule publicly testable:
</p>

<pre><code>connector projection
must remain a projection
of the declared public interface
and must not invent
or guess
logical ports
</code></pre>

<p>
This is a structural-source failure.
It is not a later semantic rejection.
</p>

<hr/>

<h2>Why this case exists</h2>

<p>
In FROG, the <code>interface</code> section owns the public typed inputs and outputs of the program.
The <code>connector</code> section may graphically project that public boundary when the FROG is reused as a node, but it does not own logical port creation.
</p>

<p>
Without this rejection rule, implementations could drift toward invalid behavior such as:
</p>

<ul>
  <li>accepting a connector port reference that does not exist in <code>interface</code>,</li>
  <li>treating connector-side labels as if they created new public ports,</li>
  <li>repairing connector mistakes by silently inventing interface entries,</li>
  <li>treating graphical connector content as stronger than the declared public interface.</li>
</ul>

<p>
This case therefore protects the explicit ownership split between public interface definition and connector projection.
</p>

<hr/>

<h2>What this case tests</h2>

<ul>
  <li>that the <code>interface</code> section remains the sole owner of public logical ports,</li>
  <li>that connector content may only refer to declared interface ports,</li>
  <li>that unknown connector references are rejected during structural validation,</li>
  <li>that implementations do not silently repair connector/interface mismatch,</li>
  <li>that canonical source structural validity remains distinct from later semantic validation.</li>
</ul>

<hr/>

<h2>Construct under test</h2>

<p>
The invalid construct is:
</p>

<pre><code>a loadable canonical source file
contains a connector entry
that references a public port identifier
not declared in interface
</code></pre>

<p>
The file may still be valid JSON.
It may still contain all required top-level sections.
However, it is not structurally valid canonical source because the connector attempts to project a public boundary that does not exist.
</p>

<p>
The expected classification MUST therefore remain:
</p>

<pre><code>loadable source
   -&gt;
structurally invalid canonical source
   -&gt;
meaning not established
</code></pre>

<hr/>

<h2>Expected outcome</h2>

<ul>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> invalid</li>
  <li><strong>Expected meaning:</strong> not established</li>
  <li><strong>Expected rejection:</strong> connector references an unknown interface port</li>
</ul>

<p>
The implementation MUST reject this case before claiming validated program meaning.
</p>

<p>
It MUST NOT:
</p>

<ul>
  <li>invent a missing interface port implicitly,</li>
  <li>accept the connector reference as a tolerated graphical alias,</li>
  <li>rewrite the connector silently to match an existing port,</li>
  <li>defer the problem as if it were only a semantic issue after structural acceptance.</li>
</ul>

<hr/>

<h2>Architectural boundary being protected</h2>

<pre><code>interface
    !=
connector

logical public boundary
    !=
graphical connector projection

declared interface port
    !=
connector-side invented port

parseable JSON
    !=
structurally valid canonical source

implementation tolerance
    !=
published conformance truth
</code></pre>

<hr/>

<h2>Why the failure is structural, not semantic</h2>

<p>
This case is not about the runtime behavior of a valid connector.
It is about the fact that canonical source ownership has already been violated before semantic interpretation is entitled to proceed.
</p>

<p>
The failure belongs to the structural-validation corridor owned by <code>Expression/</code>, because it concerns consistency between the <code>interface</code> section and the <code>connector</code> section as canonical source sections.
</p>

<p>
It is therefore not a question of later execution meaning.
It is a question of source-shape correctness and source-owned cross-section consistency.
</p>

<hr/>

<h2>Conformance reading rule</h2>

<p>
A conforming implementation should classify this case as:
</p>

<pre><code>Loadability:
  yes

Structural validity:
  no

Validated meaning:
  not established

IR derivation:
  must not be claimed as conforming

Reason:
  connector references a public port not declared by interface
</code></pre>

<hr/>

<h2>Minimal fixture guidance</h2>

<p>
A minimal fixture for this case SHOULD remain small and isolate connector/interface mismatch only.
It SHOULD avoid introducing unrelated source failures that would obscure the reason for rejection.
</p>

<p>
Good fixture design:
</p>

<ul>
  <li>keep JSON syntax valid,</li>
  <li>keep all required top-level sections present,</li>
  <li>declare a small valid public interface,</li>
  <li>include a connector entry that references exactly one unknown port identifier,</li>
  <li>avoid stacking additional unrelated diagram, widget, or type errors in the same fixture.</li>
</ul>

<p>
The goal is to expose one sharply-owned structural rejection boundary:
</p>

<pre><code>connector projection
must follow
declared interface ownership
</code></pre>

<hr/>

<h2>Repository role</h2>

<p>
This case belongs in <code>Conformance/invalid/</code> because it publishes a source-shape rejection that implementations must classify consistently.
</p>

<p>
It does not redefine the source specification.
It makes the published structural boundary testable.
</p>

<pre><code>Expression/    -&gt; owns interface / connector source rules
Conformance/   -&gt; exposes the rejection publicly
Implementation -&gt; must reject accordingly
</code></pre>

<hr/>

<h2>Summary</h2>

<p>
This case states a simple public rule:
</p>

<pre><code>a connector
must not reference
an unknown interface port
</code></pre>

<p>
The rejection must occur as structural canonical-source invalidity, not as hidden repair and not as deferred semantic interpretation.
</p>
