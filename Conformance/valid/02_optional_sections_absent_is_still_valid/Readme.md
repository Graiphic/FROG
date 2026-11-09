<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Case — Optional Sections Absent Is Still Valid</h1>

<p align="center">
  <strong>Structurally valid canonical source with optional top-level sections intentionally omitted</strong><br/>
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
  <li><a href="#why-this-case-must-pass">7. Why this Case Must Pass</a></li>
  <li><a href="#what-this-case-is-not-testing">8. What this Case Is Not Testing</a></li>
  <li><a href="#preservation-and-boundary-notes">9. Preservation and Boundary Notes</a></li>
  <li><a href="#implementation-requirements">10. Implementation Requirements</a></li>
  <li><a href="#summary">11. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines a structurally valid canonical <code>.frog</code> source file in which the optional top-level sections are intentionally absent.
</p>

<p>
The file contains the required canonical source sections only:
</p>

<ul>
  <li><code>spec_version</code></li>
  <li><code>metadata</code></li>
  <li><code>interface</code></li>
  <li><code>diagram</code></li>
</ul>

<p>
It intentionally omits:
</p>

<ul>
  <li><code>connector</code></li>
  <li><code>front_panel</code></li>
  <li><code>icon</code></li>
  <li><code>ide</code></li>
  <li><code>cache</code></li>
</ul>

<p>
The purpose of the case is to make explicit that optional-section absence is compatible with canonical source validity.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation accepts canonical source that omits optional top-level sections when they are not needed.
</p>

<p>
It makes the following truth explicit:
</p>

<pre><code>optional section absent
      -&gt;
still valid canonical source
</code></pre>

<p>
This case is important because conformance must not only reject malformed or incomplete source.
It must also publish what implementations are required to accept.
</p>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li><strong>Case family:</strong> valid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Case kind:</strong> canonical source acceptance with optional-section absence</li>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> valid</li>
  <li><strong>Expected meaning:</strong> established</li>
  <li><strong>Expected IR derivation:</strong> permitted to proceed according to the published downstream rules</li>
</ul>

<hr/>

<h2 id="normative-basis">4. Normative Basis</h2>

<p>
This case is grounded in the published canonical-source rules of <code>Expression/</code>.
</p>

<p>
For FROG v0.1, a canonical <code>.frog</code> source file MUST contain:
</p>

<ul>
  <li><code>spec_version</code></li>
  <li><code>metadata</code></li>
  <li><code>interface</code></li>
  <li><code>diagram</code></li>
</ul>

<p>
A canonical <code>.frog</code> source file MAY additionally contain:
</p>

<ul>
  <li><code>connector</code></li>
  <li><code>front_panel</code></li>
  <li><code>icon</code></li>
  <li><code>ide</code></li>
  <li><code>cache</code></li>
</ul>

<p>
Therefore, omission of optional sections is not a defect.
It is part of the published canonical source contract. :contentReference[oaicite:1]{index=1}
</p>

<pre><code>required sections present
+
optional sections omitted
    ->
canonical source still valid
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated valid source artifact for this case SHOULD be a JSON-loadable <code>.frog</code> file with:
</p>

<ul>
  <li>a valid top-level object root,</li>
  <li>all required sections present,</li>
  <li>all optional top-level sections absent,</li>
  <li>a valid minimal <code>interface</code>,</li>
  <li>a valid minimal <code>diagram</code>.</li>
</ul>

<p>
A representative valid shape is:
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {
    "inputs": [],
    "outputs": []
  },
  "diagram": {
    "nodes": [],
    "edges": []
  }
}</code></pre>

<p>
This source is not incomplete merely because optional sections are absent.
It is a valid canonical source file by design. :contentReference[oaicite:2]{index=2}
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
Expected preservation: optional-section absence remains acceptable
</code></pre>

<p>
A conforming implementation MUST accept the case as canonical source.
It MUST NOT reject the file merely because optional sections are omitted.
It MUST NOT require UI, connector, icon, IDE, or cache content for canonical source validity. :contentReference[oaicite:3]{index=3}
</p>

<hr/>

<h2 id="why-this-case-must-pass">7. Why this Case Must Pass</h2>

<p>
The published source model distinguishes between required and optional sections.
That distinction must remain observable in conformance.
</p>

<p>
This case must therefore pass because:
</p>

<ul>
  <li>the top-level root has the required canonical object shape,</li>
  <li>all required sections are present,</li>
  <li>the omitted sections are explicitly optional rather than mandatory,</li>
  <li>nothing in the source violates canonical source ownership or structural-source rules.</li>
</ul>

<pre><code>required sections satisfied
+
optional sections absent by choice
    ->
acceptance required
</code></pre>

<hr/>

<h2 id="what-this-case-is-not-testing">8. What this Case Is Not Testing</h2>

<p>
This case is intentionally narrow.
It is not testing:
</p>

<ul>
  <li>connector behavior when a connector is present,</li>
  <li>front-panel composition,</li>
  <li>icon structure,</li>
  <li>IDE preference content,</li>
  <li>cache content,</li>
  <li>rich diagram semantics,</li>
  <li>primitive-library behavior,</li>
  <li>state semantics,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>optional top-level sections
      ->
may be absent without invalidating canonical source
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>required sections,</li>
  <li>optional sections,</li>
  <li>minimal valid source,</li>
  <li>enriched valid source.</li>
</ul>

<p>
It therefore protects the following rule:
</p>

<pre><code>optional absence
      -/-> structural failure

more sections
      -/-> more canonical by default
</code></pre>

<p>
This matters because implementations must not treat optional top-level enrichments as hidden requirements. :contentReference[oaicite:4]{index=4}
</p>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source as JSON,</li>
  <li>validate the canonical top-level root,</li>
  <li>validate required section presence,</li>
  <li>recognize that omitted optional sections do not invalidate canonical source,</li>
  <li>allow later semantic and derivation stages to proceed according to the published architecture.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>reject the file because <code>connector</code> is absent,</li>
  <li>reject the file because <code>front_panel</code> is absent,</li>
  <li>reject the file because <code>icon</code>, <code>ide</code>, or <code>cache</code> are absent,</li>
  <li>treat omission of optional sections as a structural defect.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes a simple positive truth:
</p>

<pre><code>optional sections absent
    ->
canonical source still valid
    ->
acceptance required
</code></pre>

<p>
It anchors the positive conformance rule that optional top-level enrichments remain optional in published FROG canonical source.
</p>
