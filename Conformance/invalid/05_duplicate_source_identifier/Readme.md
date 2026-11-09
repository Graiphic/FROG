<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Case — Duplicate Source Identifier</h1>

<p align="center">
  <strong>Structurally invalid canonical source due to duplicate identifiers where source-level uniqueness is required</strong><br/>
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
This case defines a canonical-source rejection expectation for a FROG file that duplicates an identifier in a source family where source-level uniqueness is required.
</p>

<p>
The file may be syntactically readable as JSON.
It may also contain all required top-level sections.
However, if two source objects that must remain distinct reuse the same identifier within the same relevant ownership scope, the file is not a structurally valid canonical <code>.frog</code> source file.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation does not collapse:
</p>

<pre><code>multiple source objects
      !=
one reusable identifier slot
</code></pre>

<p>
Canonical source must remain structurally checkable and unambiguous.
If source-level identity is required to be unique within a given scope, duplication is a source-shape failure.
</p>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li><strong>Case family:</strong> invalid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Case kind:</strong> source-shape / structural-validity rejection</li>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> invalid</li>
  <li><strong>Expected meaning:</strong> not established</li>
  <li><strong>Expected IR derivation:</strong> must not proceed as if validated meaning existed</li>
</ul>

<hr/>

<h2 id="normative-basis">4. Normative Basis</h2>

<p>
This case is grounded in the published source-level identity discipline of <code>Expression/</code>.
</p>

<p>
Canonical source relies on explicit source objects and explicit source references.
Where a source family requires stable identity, that identity must remain unambiguous within the relevant source scope.
</p>

<p>
Typical source families where uniqueness may be required include:
</p>

<ul>
  <li>interface ports,</li>
  <li>diagram nodes,</li>
  <li>diagram edges where identifiers are used,</li>
  <li>widget instances,</li>
  <li>other explicit source-owned objects with source-visible identity.</li>
</ul>

<p>
If two source objects that must be distinct share the same identifier in the same required scope, canonical source validity fails before later semantic interpretation.
</p>

<pre><code>source identity required
    +
identifier reused illegally
    ->
structural invalidity
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated invalid source artifact for this case SHOULD be a JSON-loadable <code>.frog</code> file in which two source objects reuse the same identifier where uniqueness is required.
</p>

<p>
Typical invalid patterns include:
</p>

<ul>
  <li>two interface inputs with the same <code>id</code>,</li>
  <li>two diagram nodes with the same <code>id</code>,</li>
  <li>two widgets with the same <code>id</code> in the same front-panel source scope.</li>
</ul>

<p>
A representative invalid intention is:
</p>

<pre><code>interface
  -> declares two input ports
  -> both use the same source identifier
</code></pre>

<p>
This creates an ambiguous canonical source boundary and must fail as structural-source validation.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: invalid
Expected meaning: not established
Expected rejection: duplicate source identifier
</code></pre>

<p>
A conforming implementation MUST reject the case explicitly.
It MUST NOT silently rename one of the source objects.
It MUST NOT silently merge duplicated objects.
It MUST NOT continue as if canonical source validity had been established.
</p>

<hr/>

<h2 id="why-this-case-must-fail">7. Why this Case Must Fail</h2>

<p>
Canonical source depends on explicit, recoverable source identity.
That identity supports source-level references, validation, preservation, and later-stage recoverability.
</p>

<p>
If duplicate identifiers were tolerated where uniqueness is required, then:
</p>

<ul>
  <li>source references would become ambiguous,</li>
  <li>different tools could resolve the ambiguity differently,</li>
  <li>canonical source would stop being reliably checkable and portable.</li>
</ul>

<p>
This case must therefore fail because:
</p>

<ul>
  <li>identifier uniqueness is part of source structural discipline where required,</li>
  <li>implementation repair must not replace source validity,</li>
  <li>later semantic interpretation must not start from ambiguous source identity.</li>
</ul>

<pre><code>duplicate identifier in uniqueness-required scope
    ->
ambiguous source identity
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
  <li>type legality of otherwise distinct objects,</li>
  <li>primitive legality,</li>
  <li>graph execution semantics,</li>
  <li>state semantics,</li>
  <li>IR preservation details,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>source-visible identity
      ->
must remain unambiguous where uniqueness is required
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the following source-layer distinctions:
</p>

<ul>
  <li>distinct source objects versus duplicated source identifiers,</li>
  <li>canonical source identity versus implementation repair,</li>
  <li>source validity versus later-stage interpretation convenience.</li>
</ul>

<p>
It therefore protects the rule:
</p>

<pre><code>duplicate id
      -/-> harmless formatting issue

ambiguous source identity
      -/-> acceptable canonical source
</code></pre>

<p>
This boundary matters before semantic validation and before any IR derivation.
It must not be repaired implicitly by tools.
</p>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source if it is syntactically readable,</li>
  <li>validate uniqueness requirements for the relevant source family,</li>
  <li>detect duplicate identifiers in a uniqueness-required scope,</li>
  <li>reject the file before claiming validated meaning exists,</li>
  <li>report the rejection as a structural-source failure.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>auto-rename duplicated source identifiers silently,</li>
  <li>merge duplicated source objects implicitly,</li>
  <li>continue semantic validation as if source identity were unambiguous,</li>
  <li>derive execution-facing artifacts as if structurally valid source had been established.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes a basic but important public truth:
</p>

<pre><code>duplicate source identifier
    ->
structurally invalid canonical source
    ->
rejection required
</code></pre>

<p>
It protects a key principle of the FROG source layer:
</p>

<pre><code>explicit source identity
   -&gt;
must remain unambiguous
   -&gt;
must remain structurally checkable
</code></pre>

<p>
That principle must remain explicit across all conforming implementations.
</p>
