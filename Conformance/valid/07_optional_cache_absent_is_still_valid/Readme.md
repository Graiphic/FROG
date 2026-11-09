<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Case — Optional Cache Section Absent Is Still Valid</h1>

<p align="center">
  <strong>Structurally valid canonical source with no <code>cache</code> section present</strong><br/>
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
This case defines a structurally valid canonical <code>.frog</code> source file in which the optional <code>cache</code> section is intentionally absent.
</p>

<p>
The case exists to make explicit that a FROG program does not require serialized non-authoritative tooling cache data in order to remain valid canonical source.
A FROG may therefore define a complete canonical program source without embedding any cache artifact content.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation does not treat the absence of <code>cache</code> as a structural defect.
</p>

<p>
It makes the following truth explicit:
</p>

<pre><code>cache absent
      -&gt;
canonical source may still be valid
</code></pre>

<p>
This matters because <code>cache</code> is an optional non-authoritative tooling convenience, not part of the minimal required canonical source contract.
</p>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li><strong>Case family:</strong> valid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Case kind:</strong> canonical source acceptance with absent optional <code>cache</code></li>
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
For FROG v0.1:
</p>

<ul>
  <li><code>spec_version</code>, <code>metadata</code>, <code>interface</code>, and <code>diagram</code> are required,</li>
  <li><code>cache</code> is optional,</li>
  <li>when present, <code>cache</code> contains derived non-authoritative tooling data,</li>
  <li>its absence does not invalidate the canonical source itself.</li>
</ul>

<p>
Therefore, canonical source validity does not depend on the presence of a cache section.
</p>

<pre><code>required sections present
+
cache absent
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
  <li>no <code>cache</code> section,</li>
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
The absence of <code>cache</code> here is intentional and valid.
It does not imply that the source is incomplete or that toolchains are missing required canonical information.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
Expected preservation: absence of serialized cache content remains acceptable
</code></pre>

<p>
A conforming implementation MUST accept the case as canonical source.
It MUST NOT require <code>cache</code> to exist merely because tooling may persist derived convenience data when desired.
</p>

<hr/>

<h2 id="why-this-case-must-pass">7. Why this Case Must Pass</h2>

<p>
The published source model separates authoritative canonical source from optional non-authoritative derived tooling content.
The <code>cache</code> section is not part of the minimal program-defining source contract.
</p>

<p>
This case must therefore pass because:
</p>

<ul>
  <li>the required canonical sections are present,</li>
  <li><code>cache</code> is optional rather than mandatory,</li>
  <li>the source remains structurally complete without tooling cache content,</li>
  <li>nothing in the canonical source contract requires every FROG to embed derived non-authoritative data.</li>
</ul>

<pre><code>program source valid
+
cache omitted by choice
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
  <li>the structure of <code>cache</code> when present,</li>
  <li>tool-specific caching behavior,</li>
  <li>front-panel composition,</li>
  <li>connector projection,</li>
  <li>diagram richness,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>optional cache section
      ->
may be absent without invalidating canonical source
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>authoritative program-defining source,</li>
  <li>optional non-authoritative derived tooling cache,</li>
  <li>program validity,</li>
  <li>tooling acceleration content.</li>
</ul>

<p>
It therefore protects the rule:
</p>

<pre><code>no serialized cache section
      -/-> invalid program source

tooling cache support
      -/-> cache mandatory in every .frog file
</code></pre>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source as JSON,</li>
  <li>validate required section presence,</li>
  <li>recognize that no <code>cache</code> section is present,</li>
  <li>accept the source as structurally valid canonical source,</li>
  <li>allow later semantic and derivation stages to proceed according to the published architecture.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>reject the file because <code>cache</code> is absent,</li>
  <li>invent a default <code>cache</code> as a hidden structural requirement,</li>
  <li>treat absence of serialized cache content as a canonical-source defect.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes a focused positive truth:
</p>

<pre><code>cache absent
    ->
canonical source still valid
    ->
acceptance required
</code></pre>

<p>
It anchors the rule that serialized non-authoritative cache content remains optional in the FROG canonical source model.
</p>
