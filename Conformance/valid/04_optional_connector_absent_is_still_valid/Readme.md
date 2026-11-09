<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Case — Optional Connector Absent Is Still Valid</h1>

<p align="center">
  <strong>Structurally valid canonical source with no <code>connector</code> section present</strong><br/>
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
This case defines a structurally valid canonical <code>.frog</code> source file in which the optional <code>connector</code> section is intentionally absent.
</p>

<p>
The case exists to make explicit that a FROG program does not require a serialized reusable-node perimeter projection in order to remain valid canonical source.
A FROG may therefore define a public interface without also serializing a connector representation. ([github.com](https://github.com/Graiphic/FROG/blob/main/Expression/Readme.md))
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation does not treat the absence of <code>connector</code> as a structural defect.
</p>

<p>
It makes the following truth explicit:
</p>

<pre><code>connector absent
      -&gt;
canonical source may still be valid
</code></pre>

<p>
This matters because <code>connector</code> is an optional graphical projection layer, not part of the minimal required canonical source contract. ([github.com](https://github.com/Graiphic/FROG/blob/main/Expression/Readme.md))
</p>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li><strong>Case family:</strong> valid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Case kind:</strong> canonical source acceptance with absent optional <code>connector</code></li>
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
  <li><code>connector</code> is optional,</li>
  <li>when present, <code>connector</code> graphically projects the public interface when the FROG is reused as a node,</li>
  <li>its absence does not invalidate the public interface or the canonical source itself.</li>
</ul>

<p>
Therefore, canonical source validity does not depend on the presence of a connector. ([github.com](https://github.com/Graiphic/FROG/blob/main/Expression/Readme.md))
</p>

<pre><code>required sections present
+
connector absent
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
  <li>no <code>connector</code> section,</li>
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
The absence of <code>connector</code> here is intentional and valid.
It does not imply that the public interface is missing or incomplete. ([github.com](https://github.com/Graiphic/FROG/blob/main/Expression/Readme.md))
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
Expected preservation: absence of serialized connector projection remains acceptable
</code></pre>

<p>
A conforming implementation MUST accept the case as canonical source.
It MUST NOT require <code>connector</code> to exist merely because the language supports reusable-node graphical projection when desired. ([github.com](https://github.com/Graiphic/FROG/blob/main/Expression/Readme.md))
</p>

<hr/>

<h2 id="why-this-case-must-pass">7. Why this Case Must Pass</h2>

<p>
The published source model explicitly separates the public logical interface from the optional graphical perimeter projection of that interface.
A FROG remains a valid canonical source even when no serialized connector is provided.
</p>

<p>
This case must therefore pass because:
</p>

<ul>
  <li>the required canonical sections are present,</li>
  <li><code>connector</code> is optional rather than mandatory,</li>
  <li>the source remains structurally complete without reusable-node graphical projection,</li>
  <li>nothing in the canonical source contract requires a connector for every FROG.</li>
</ul>

<pre><code>interface present
+
connector absent
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
  <li>connector structure when <code>connector</code> is present,</li>
  <li>connector-to-interface mapping details,</li>
  <li>front-panel composition,</li>
  <li>diagram richness,</li>
  <li>widget participation,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>optional connector
      ->
may be absent without invalidating canonical source
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>public logical interface,</li>
  <li>optional connector projection,</li>
  <li>program validity,</li>
  <li>graphical reuse enrichment.</li>
</ul>

<p>
It therefore protects the rule:
</p>

<pre><code>no serialized connector
      -/-> invalid program source

graphical node projection support
      -/-> connector mandatory in every .frog file
</code></pre>

<p>
This matters because implementations must not convert optional reusable-node presentation into a hidden canonical-source requirement. ([github.com](https://github.com/Graiphic/FROG/blob/main/Expression/Readme.md))
</p>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source as JSON,</li>
  <li>validate required section presence,</li>
  <li>recognize that no <code>connector</code> section is present,</li>
  <li>accept the source as structurally valid canonical source,</li>
  <li>allow later semantic and derivation stages to proceed according to the published architecture.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>reject the file because <code>connector</code> is absent,</li>
  <li>invent a default <code>connector</code> as a hidden structural requirement,</li>
  <li>treat absence of serialized connector projection as a canonical-source defect.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes a focused positive truth:
</p>

<pre><code>connector absent
    ->
canonical source still valid
    ->
acceptance required
</code></pre>

<p>
It anchors the rule that serialized reusable-node graphical projection remains optional in the FROG canonical source model.
</p>
