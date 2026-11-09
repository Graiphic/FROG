<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Case — Wrong Top-Level Section Type</h1>

<p align="center">
  <strong>Structurally invalid canonical source due to a required top-level section having the wrong source shape</strong><br/>
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
This case defines a canonical-source rejection expectation for a FROG file in which a required top-level section is present but has the wrong top-level source shape.
</p>

<p>
The file may still be syntactically readable as JSON.
It may even contain the required section name.
However, if that section is not represented using the source shape required by the canonical source specification, the file is not a structurally valid canonical <code>.frog</code> source file.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation does not confuse:
</p>

<pre><code>section presence
      !=
structural validity of that section
</code></pre>

<p>
A required section is not valid merely because the key exists.
The section must also satisfy the required top-level source shape expected by the canonical source specification.
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
FROG v0.1 defines a canonical top-level source structure in which required top-level sections such as <code>metadata</code>, <code>interface</code>, and <code>diagram</code> are source objects with their own section-local structure.
</p>

<p>
A required section with the wrong top-level type or wrong top-level structural shape does not satisfy the canonical source contract merely because the key exists.
</p>

<pre><code>Required section key present
    +
wrong section shape
    ->
structural invalidity
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated invalid source artifact for this case SHOULD be a JSON-loadable <code>.frog</code> file in which a required top-level section is present but represented using the wrong top-level type.
</p>

<p>
Typical examples include:
</p>

<ul>
  <li><code>"metadata": []</code> instead of an object,</li>
  <li><code>"interface": "..."</code> instead of an object,</li>
  <li><code>"diagram": []</code> instead of an object.</li>
</ul>

<p>
A representative invalid shape is:
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "diagram": []
}</code></pre>

<p>
This file is loadable as JSON.
It contains the required <code>diagram</code> key.
But the section has the wrong top-level type and therefore fails structural validation as canonical source.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: invalid
Expected meaning: not established
Expected rejection: wrong top-level section type
</code></pre>

<p>
A conforming implementation MUST reject the case explicitly.
It MUST NOT reinterpret the malformed section as if it had the required canonical shape.
It MUST NOT silently coerce the section into validity.
It MUST NOT continue as if validated meaning had been established.
</p>

<hr/>

<h2 id="why-this-case-must-fail">7. Why this Case Must Fail</h2>

<p>
The canonical source contract is not only about naming required sections.
It is also about the structural shape of those sections.
</p>

<p>
If a required section uses the wrong top-level type, then:
</p>

<ul>
  <li>the source no longer matches the canonical source model,</li>
  <li>section-local interpretation cannot proceed on a valid canonical basis,</li>
  <li>later semantic validation has no structurally valid source section to consume.</li>
</ul>

<p>
This is therefore a structural-source failure.
It is not a semantic-runtime issue and not an implementation-specific convenience issue.
</p>

<pre><code>Section key exists
    -/-> section structurally valid

Wrong section type
    ->
canonical source contract not satisfied
    ->
no structurally valid source
</code></pre>

<hr/>

<h2 id="what-this-case-is-not-testing">8. What this Case Is Not Testing</h2>

<p>
This case is intentionally narrow.
It is not testing:
</p>

<ul>
  <li>type legality inside a valid section,</li>
  <li>primitive legality,</li>
  <li>graph legality inside a structurally valid diagram object,</li>
  <li>state semantics,</li>
  <li>IR preservation details,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>required section present
      +
correct top-level shape
      ?
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>JSON loadability,</li>
  <li>required key presence,</li>
  <li>canonical source structural validity,</li>
  <li>semantic acceptance,</li>
  <li>later execution-facing derivation.</li>
</ul>

<p>
It therefore protects the following rule:
</p>

<pre><code>required key present
      -/-> section valid

structurally invalid source
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
  <li>detect that the required section has the wrong top-level type or structural shape,</li>
  <li>reject the file before claiming validated meaning exists,</li>
  <li>report the rejection as a structural-source failure.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>treat section-key presence as sufficient for source validity,</li>
  <li>silently repair the wrong top-level section type,</li>
  <li>continue semantic validation as if canonical source validity had been established,</li>
  <li>derive execution-facing artifacts as if a valid source section existed.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes another small but critical public truth:
</p>

<pre><code>wrong top-level section type
    ->
structurally invalid canonical source
    ->
rejection required
</code></pre>

<p>
It protects the fact that canonical source validity depends on both:
</p>

<pre><code>required section presence
+
required section shape
</code></pre>

<p>
Both conditions must remain explicit across all conforming implementations.
</p>
