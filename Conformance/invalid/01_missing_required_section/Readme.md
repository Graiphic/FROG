<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Case — Missing Required Section</h1>

<p align="center">
  <strong>Structurally invalid canonical source due to a missing required top-level section</strong><br/>
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
This case defines a canonical-source rejection expectation for a FROG file that is loadable as JSON but is missing at least one required top-level section.
</p>

<p>
The purpose of this case is to make the <strong>structural validity boundary</strong> publicly testable.
The file may still be syntactically readable as JSON, but it is not a structurally valid canonical <code>.frog</code> source file.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation does not collapse the following distinctions:
</p>

<pre><code>loadable source
      !=
structurally valid canonical source

structurally invalid source
      !=
semantically rejected source
</code></pre>

<p>
A missing required section is a source-shape failure.
It must be rejected at the canonical source-validation stage.
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
This case is grounded in the published canonical-source requirements of <code>Expression/</code>.
</p>

<p>
For FROG v0.1, a canonical <code>.frog</code> source file MUST contain the required top-level sections:
</p>

<ul>
  <li><code>spec_version</code></li>
  <li><code>metadata</code></li>
  <li><code>interface</code></li>
  <li><code>diagram</code></li>
</ul>

<p>
If one of these required sections is absent, the source is not a structurally valid canonical FROG Expression.
</p>

<pre><code>Required source sections
    ->
missing section
    ->
structural invalidity
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated invalid source artifact for this case SHOULD be a JSON-loadable <code>.frog</code> file that omits one required top-level section.
</p>

<p>
Typical examples include omission of:
</p>

<ul>
  <li><code>metadata</code></li>
  <li><code>interface</code></li>
  <li><code>diagram</code></li>
</ul>

<p>
A representative invalid shape is:
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {}
}</code></pre>

<p>
This file is loadable as JSON, but it is missing the required <code>diagram</code> section.
It must therefore fail structural validation as canonical source.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: invalid
Expected meaning: not established
Expected rejection: missing required top-level section
</code></pre>

<p>
A conforming implementation MUST reject the case explicitly.
It MUST NOT silently invent the missing section.
It MUST NOT silently repair the file into a structurally valid FROG.
It MUST NOT continue as if validated meaning had been established.
</p>

<hr/>

<h2 id="why-this-case-must-fail">7. Why this Case Must Fail</h2>

<p>
A canonical <code>.frog</code> source file has a minimum required source contract.
That contract is part of the source specification itself.
</p>

<p>
If a required section is missing, then:
</p>

<ul>
  <li>the source is incomplete as canonical program representation,</li>
  <li>the required source structure has not been satisfied,</li>
  <li>later semantic validation has no valid canonical source basis to work from.</li>
</ul>

<p>
This is not a runtime concern.
It is not an implementation convenience concern.
It is not an optimization choice.
It is a structural-source failure.
</p>

<pre><code>Missing required section
    ->
canonical source contract not satisfied
    ->
no structurally valid source
    ->
no validated meaning
</code></pre>

<hr/>

<h2 id="what-this-case-is-not-testing">8. What this Case Is Not Testing</h2>

<p>
This case is intentionally narrow.
It is not testing:
</p>

<ul>
  <li>type legality,</li>
  <li>primitive legality,</li>
  <li>structure semantics,</li>
  <li>state semantics,</li>
  <li>IR preservation details,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>required canonical source shape
      ->
present or absent
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>JSON parseability,</li>
  <li>canonical source structural validity,</li>
  <li>semantic acceptance,</li>
  <li>later execution-facing derivation.</li>
</ul>

<p>
It therefore protects the following rule:
</p>

<pre><code>syntactically loadable
      -/-> canonical source valid

canonical source invalid
      -/-> semantic validation success

semantic validation absent
      -/-> IR derivation as if meaning existed
</code></pre>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source if it is syntactically readable,</li>
  <li>perform source-structure validation,</li>
  <li>detect the missing required top-level section,</li>
  <li>reject the file before claiming validated meaning exists,</li>
  <li>report the rejection as a structural-source failure rather than as a runtime failure.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>auto-create the missing required section silently,</li>
  <li>treat the file as semantically valid anyway,</li>
  <li>derive execution-facing artifacts as if canonical source validity had been established,</li>
  <li>blur this rejection into an unrelated implementation-private error category.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes a simple but critical public truth:
</p>

<pre><code>missing required source section
    ->
structurally invalid canonical source
    ->
rejection required
</code></pre>

<p>
It is one of the smallest possible conformance cases, but it protects a major architectural boundary:
</p>

<pre><code>loadable source
   -&gt;
structurally valid canonical source
   -&gt;
validated meaning
</code></pre>

<p>
That boundary must remain explicit across all conforming implementations.
</p>
