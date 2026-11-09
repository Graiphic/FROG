<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Case — Connector Defines Logical Ports</h1>

<p align="center">
  <strong>Structurally invalid canonical source due to a connector attempting to define logical ports instead of projecting the public interface</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#case-purpose">2. Case Purpose</a></li>
  <li><a href="#case-classification">3. Case Classification</a></li>
  <li><a href="#normative-basis">4. Normative Basis</a></li>
  <li><a href="#input-shape">5. Input Shape</a></li>
  <li><a href="#expected-outcome">6. Expected Outcome</a></li>
  <li><a href="#why-this-case-must-fail">7. Why this Case Must Fail</a></li>
  <li><a href="#what-this-case-is-not-testing">8. What this Case Is Not Testing</a></li>
  <li><a href="#preservation-and-boundary-notes">9. Preservation and Boundary Notes</a></li>
  <li><a href="#implementation-requirements">10. Implementation Requirements</a></li>
  <li><a href="#summary">11. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines a canonical-source rejection expectation for a FROG file in which the <code>connector</code> section attempts to define logical ports rather than graphically project the public interface that is already owned by <code>interface</code>.
</p>

<p>
The file may be syntactically readable as JSON and may contain both <code>interface</code> and <code>connector</code> sections.
However, if the connector is used to introduce new logical ports, the source is not structurally valid as canonical FROG source.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation does not collapse:
</p>

<pre><code>public logical contract
      !=
graphical node-side projection
</code></pre>

<p>
The public logical contract belongs to <code>interface</code>.
The connector may project that contract graphically when the FROG is reused as a node, but it must not become a second interface-definition surface.
</p>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li><strong>Case family:</strong> invalid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Case kind:</strong> source-shape / ownership-boundary rejection</li>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> invalid</li>
  <li><strong>Expected meaning:</strong> not established</li>
  <li><strong>Expected IR derivation:</strong> must not proceed as if validated meaning existed</li>
</ul>

<hr/>

<h2 id="normative-basis">4. Normative Basis</h2>

<p>
This case is grounded in the published ownership rules of <code>Expression/</code>.
</p>

<p>
In FROG v0.1:
</p>

<ul>
  <li><code>interface</code> defines the public typed inputs and outputs of the FROG,</li>
  <li><code>connector</code> defines the graphical perimeter mapping of those interface ports when the FROG is reused as a node,</li>
  <li>the connector never defines new logical ports.</li>
</ul>

<p>
Therefore, a connector that attempts to create or own logical ports violates canonical source ownership.
</p>

<pre><code>interface
    -> owns public logical ports

connector
    -> projects existing public ports

connector creating ports
    -> ownership violation
    -> structural invalidity
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated invalid source artifact for this case SHOULD be a JSON-loadable <code>.frog</code> file in which the connector includes source content that attempts to declare new logical ports rather than only mapping already-declared interface ports.
</p>

<p>
Typical invalid patterns include a connector section that:
</p>

<ul>
  <li>introduces connector-side ports not present in <code>interface</code>,</li>
  <li>declares new logical input or output identities inside <code>connector</code>,</li>
  <li>acts as if connector-owned ports were an alternative to the public interface section.</li>
</ul>

<p>
A representative invalid intention is:
</p>

<pre><code>interface
  -> declares one public input

connector
  -> tries to declare two graphical ports
  -> one of them has no corresponding interface port
  -> connector therefore attempts to define logical port surface
</code></pre>

<p>
This is not merely a semantic mismatch.
It is a source-ownership violation in the canonical source model.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: invalid
Expected meaning: not established
Expected rejection: connector defines logical ports
</code></pre>

<p>
A conforming implementation MUST reject the case explicitly.
It MUST NOT silently reinterpret connector-defined ports as interface ports.
It MUST NOT auto-merge connector declarations into the public interface.
It MUST NOT continue as if canonical source validity had been established.
</p>

<hr/>

<h2 id="why-this-case-must-fail">7. Why this Case Must Fail</h2>

<p>
FROG explicitly separates:
</p>

<ul>
  <li>public program contract,</li>
  <li>graphical projection of that contract.</li>
</ul>

<p>
If the connector is allowed to define ports, then the source layer would gain two competing owners for the public boundary:
</p>

<ul>
  <li><code>interface</code></li>
  <li><code>connector</code></li>
</ul>

<p>
That would blur the canonical source model, create ambiguity, and make source validation depend on implementation-specific reconciliation rules.
</p>

<p>
This case must therefore fail because:
</p>

<ul>
  <li>connector is not the owner of logical boundary definition,</li>
  <li>public port identity must remain owned by <code>interface</code>,</li>
  <li>connector must remain a projection layer only.</li>
</ul>

<pre><code>connector defines new logical port
    ->
source ownership collapses
    ->
canonical source contract not satisfied
    ->
rejection required
</code></pre>

<hr/>

<h2 id="what-this-case-is-not-testing">8. What this Case Is Not Testing</h2>

<p>
This case is intentionally narrow.
It is not testing:
</p>

<ul>
  <li>type legality of correctly declared interface ports,</li>
  <li>diagram graph legality,</li>
  <li>widget participation,</li>
  <li>state semantics,</li>
  <li>IR preservation details,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one architectural boundary:
</p>

<pre><code>interface ownership
      !=
connector projection
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the following source-level distinctions:
</p>

<ul>
  <li><code>interface</code> versus <code>connector</code>,</li>
  <li>logical contract versus graphical projection,</li>
  <li>source ownership versus implementation repair.</li>
</ul>

<p>
It therefore protects the rule:
</p>

<pre><code>connector appearance
      -/-> interface ownership

graphical projection
      -/-> logical declaration
</code></pre>

<p>
This boundary matters upstream of validated meaning and far upstream of runtime realization.
It must not be repaired implicitly by tools.
</p>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source if it is syntactically readable,</li>
  <li>validate that connector entries refer only to already-declared interface ports,</li>
  <li>detect any connector-side attempt to introduce new logical ports,</li>
  <li>reject the file before claiming validated meaning exists,</li>
  <li>report the rejection as a canonical-source ownership or structural-source failure.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>treat connector declarations as a second interface-definition surface,</li>
  <li>auto-create missing interface ports from connector content,</li>
  <li>silently reconcile conflicting connector/interface ownership,</li>
  <li>derive execution-facing artifacts as if the public boundary were structurally valid.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes a critical public truth:
</p>

<pre><code>connector defines logical ports
    ->
structurally invalid canonical source
    ->
rejection required
</code></pre>

<p>
It protects one of the core source-layer separations in FROG:
</p>

<pre><code>interface
   ->
owns the public logical contract

connector
   ->
projects that contract graphically
</code></pre>

<p>
That separation must remain explicit across all conforming implementations.
</p>
