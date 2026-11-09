<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Case — Invalid Top-Level Root Shape</h1>

<p align="center">
  <strong>Structurally invalid canonical source due to the top-level JSON root not being the required source object</strong><br/>
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
This case defines a canonical-source rejection expectation for a FROG file whose JSON content is loadable but whose top-level root is not the required canonical source object.
</p>

<p>
A canonical <code>.frog</code> source file is not merely “some JSON”.
It must be a top-level source object with the canonical FROG section structure.
If the top-level root is instead an array, scalar, or another non-object JSON form, the file is not a structurally valid canonical FROG Expression.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation does not collapse:
</p>

<pre><code>JSON-loadable content
      !=
canonical top-level source object
</code></pre>

<p>
The top-level source root is part of canonical source shape.
It is not an implementation detail.
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
FROG v0.1 defines a canonical top-level source structure in which the <code>.frog</code> file is represented as a top-level object containing required and optional source sections.
A non-object top-level root does not satisfy that source contract.
</p>

<pre><code>canonical .frog source
    ->
top-level object

top-level non-object
    ->
structural invalidity
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated invalid source artifact for this case SHOULD be a syntactically valid JSON file whose top-level root is not an object.
</p>

<p>
Typical invalid examples include:
</p>

<ul>
  <li>a top-level array,</li>
  <li>a top-level string,</li>
  <li>a top-level number,</li>
  <li>a top-level boolean.</li>
</ul>

<p>
A representative invalid shape is:
</p>

<pre><code>[
  {
    "spec_version": "0.1"
  }
]</code></pre>

<p>
This content is valid JSON.
It is not a valid canonical FROG source root.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: invalid
Expected meaning: not established
Expected rejection: invalid top-level root shape
</code></pre>

<p>
A conforming implementation MUST reject the case explicitly.
It MUST NOT reinterpret the non-object root as if it were a valid canonical source object.
It MUST NOT continue as if validated meaning had been established.
</p>

<hr/>

<h2 id="why-this-case-must-fail">7. Why this Case Must Fail</h2>

<p>
The top-level source root is the entry point of the canonical source contract.
If that entry point is malformed, the canonical source model has not even been established.
</p>

<p>
This case must therefore fail because:
</p>

<ul>
  <li>the canonical source root is structurally wrong,</li>
  <li>top-level section interpretation cannot begin on a valid basis,</li>
  <li>later semantic stages have no canonical source object to consume.</li>
</ul>

<hr/>

<h2 id="what-this-case-is-not-testing">8. What this Case Is Not Testing</h2>

<p>
This case is not testing:
</p>

<ul>
  <li>required-section presence inside a valid root object,</li>
  <li>section-local structure,</li>
  <li>type legality,</li>
  <li>primitive legality,</li>
  <li>runtime behavior.</li>
</ul>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case protects the distinction between:
</p>

<ul>
  <li>generic JSON content,</li>
  <li>canonical FROG source shape.</li>
</ul>

<pre><code>loadable JSON
      -/-> canonical source object
</code></pre>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation SHOULD:
</p>

<ul>
  <li>load the JSON content if syntactically valid,</li>
  <li>check that the top-level root is the required canonical source object shape,</li>
  <li>reject the file before semantic validation begins.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>auto-wrap or reinterpret the root silently,</li>
  <li>invent a canonical root object around the provided content,</li>
  <li>derive execution-facing artifacts as if source validity had been established.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes:
</p>

<pre><code>non-object top-level root
    ->
structurally invalid canonical source
    ->
rejection required
</code></pre>
