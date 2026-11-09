<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Case — Minimal Canonical FROG</h1>

<p align="center">
  <strong>Structurally valid minimal canonical source with the required top-level sections only</strong><br/>
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
This case defines the smallest structurally valid canonical <code>.frog</code> source file for FROG v0.1.
</p>

<p>
It contains only the required top-level sections:
</p>

<ul>
  <li><code>spec_version</code></li>
  <li><code>metadata</code></li>
  <li><code>interface</code></li>
  <li><code>diagram</code></li>
</ul>

<p>
It intentionally omits all optional sections.
The purpose is to establish a minimal positive anchor for canonical source validity.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation accepts the minimal canonical source form defined by <code>Expression/</code>.
</p>

<p>
It makes the following truth explicit:
</p>

<pre><code>minimal required canonical source
      ->
structurally valid canonical source
</code></pre>

<p>
This is the positive mirror of invalid cases such as:
</p>

<ul>
  <li>missing required section,</li>
  <li>wrong top-level section type,</li>
  <li>invalid top-level root shape.</li>
</ul>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li><strong>Case family:</strong> valid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Case kind:</strong> minimal canonical source acceptance</li>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> valid</li>
  <li><strong>Expected meaning:</strong> established at the minimal accepted source level</li>
  <li><strong>Expected IR derivation:</strong> permitted to proceed according to the published downstream rules</li>
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
Optional sections such as <code>connector</code>, <code>front_panel</code>, <code>icon</code>, <code>ide</code>, and <code>cache</code> MAY be absent.
A conforming implementation must therefore accept a source file that contains the required minimal structure and no more.
</p>

<pre><code>required sections present
+
top-level root shape valid
+
required section shapes valid
    ->
minimal canonical source accepted
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated valid source artifact for this case SHOULD be a JSON-loadable <code>.frog</code> file with:
</p>

<ul>
  <li>a valid top-level object root,</li>
  <li>all required sections present,</li>
  <li>no optional sections required,</li>
  <li>a structurally valid but intentionally minimal <code>interface</code>,</li>
  <li>a structurally valid but intentionally minimal <code>diagram</code>.</li>
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
This shape is intentionally simple.
Its purpose is not expressive richness, but canonical-source validity.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
Expected preservation: minimal canonical source remains explicit as source
</code></pre>

<p>
A conforming implementation MUST accept the case as canonical source.
It MUST NOT reject the file merely because optional sections are absent.
It MUST NOT require front-panel, connector, icon, IDE, or cache sections for minimal canonical validity.
</p>

<hr/>

<h2 id="why-this-case-must-pass">7. Why this Case Must Pass</h2>

<p>
The canonical source specification defines a minimal required source contract.
If that contract is satisfied, the file is a structurally valid canonical FROG Expression.
</p>

<p>
This case must therefore pass because:
</p>

<ul>
  <li>the top-level root has the required canonical object shape,</li>
  <li>the required top-level sections are present,</li>
  <li>the optional sections are correctly omitted rather than malformed,</li>
  <li>nothing in the source violates section ownership or structural-source rules.</li>
</ul>

<pre><code>canonical root object
+
required section presence
+
minimal valid section structure
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
  <li>rich diagram semantics,</li>
  <li>primitive-library behavior,</li>
  <li>widget participation,</li>
  <li>front-panel composition,</li>
  <li>structure legality beyond the minimal diagram form,</li>
  <li>state semantics,</li>
  <li>IR preservation details beyond the fact that derivation may proceed,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>minimal canonical source contract
      ->
accepted as valid canonical source
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>required sections,</li>
  <li>optional sections,</li>
  <li>minimal canonical source,</li>
  <li>enriched canonical source.</li>
</ul>

<p>
It therefore protects the following rule:
</p>

<pre><code>optional section absent
      -/-> source invalid

minimal canonical source
      -/-> incomplete by default
</code></pre>

<p>
This matters because conformance must make clear not only what fails, but also what the published specification requires implementations to accept.
</p>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source as JSON,</li>
  <li>recognize the top-level canonical object shape,</li>
  <li>validate required section presence,</li>
  <li>accept the source as structurally valid canonical source,</li>
  <li>allow later semantic and derivation stages to proceed according to the published architecture.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>reject the file for lacking optional sections,</li>
  <li>require a <code>front_panel</code> for basic canonical validity,</li>
  <li>require a <code>connector</code> for basic canonical validity,</li>
  <li>treat minimality as a structural defect.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes the smallest positive public truth for canonical source:
</p>

<pre><code>required canonical sections present
    ->
minimal canonical source valid
    ->
acceptance required
</code></pre>

<p>
It anchors the positive side of the source-shape conformance surface and gives the repository a minimal acceptance baseline for all conforming implementations.
</p>
