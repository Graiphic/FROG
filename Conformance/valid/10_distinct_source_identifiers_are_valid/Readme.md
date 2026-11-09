<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Case — Distinct Source Identifiers Are Valid</h1>

<p align="center">
  <strong>Structurally valid canonical source with distinct identifiers in a uniqueness-required source scope</strong><br/>
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
This case defines a structurally valid canonical <code>.frog</code> source file in which source objects use distinct identifiers in a source family where uniqueness is required.
</p>

<p>
The case exists to show the positive structural rule for source-visible identity:
when a source family requires stable identifiers, canonical source remains valid when those identifiers are present and unambiguous.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation accepts canonical source in which:
</p>

<ul>
  <li>multiple source objects coexist in the same uniqueness-relevant scope,</li>
  <li>their identifiers are distinct,</li>
  <li>no source-level ambiguity is introduced.</li>
</ul>

<p>
It makes the following truth explicit:
</p>

<pre><code>distinct source identifiers
      -&gt;
structurally valid canonical source
</code></pre>

<p>
This case is the positive mirror of invalid duplicate-identifier rejection cases.
</p>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li><strong>Case family:</strong> valid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Case kind:</strong> canonical source acceptance with distinct identifiers in a uniqueness-required scope</li>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> valid</li>
  <li><strong>Expected meaning:</strong> established</li>
  <li><strong>Expected IR derivation:</strong> permitted to proceed according to the published downstream rules</li>
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
Accordingly, a source file is structurally valid when identifiers that must be distinct are in fact distinct.
</p>

<pre><code>source identity required
+
identifiers distinct
    ->
structural validity preserved
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated valid source artifact for this case SHOULD be a JSON-loadable <code>.frog</code> file in which multiple source objects in a uniqueness-required scope use distinct identifiers.
</p>

<p>
A representative valid shape is:
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {
    "inputs": [
      {
        "id": "in_a",
        "name": "a",
        "type": "float64"
      },
      {
        "id": "in_b",
        "name": "b",
        "type": "float64"
      }
    ],
    "outputs": []
  },
  "diagram": {
    "nodes": [],
    "edges": []
  }
}</code></pre>

<p>
In this shape, the two interface inputs occupy the same source family and relevant scope, but they remain unambiguous because their identifiers are distinct.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
Expected preservation: source-visible identity remains explicit and unambiguous
</code></pre>

<p>
A conforming implementation MUST accept the case as canonical source.
It MUST preserve the distinction between:
</p>

<ul>
  <li>multiple distinct source objects,</li>
  <li>their distinct source-visible identities.</li>
</ul>

<hr/>

<h2 id="why-this-case-must-pass">7. Why this Case Must Pass</h2>

<p>
The published source model does not merely reject duplicate identifiers.
It also positively requires that valid source identity remain explicit and unambiguous where uniqueness matters.
</p>

<p>
This case must therefore pass because:
</p>

<ul>
  <li>the required canonical sections are present,</li>
  <li>the source family under test uses explicit identifiers,</li>
  <li>those identifiers are distinct within the relevant scope,</li>
  <li>no source-level ambiguity is introduced.</li>
</ul>

<pre><code>multiple source objects
+
distinct identifiers
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
  <li>type legality beyond the fact that the objects are structurally valid,</li>
  <li>rich diagram semantics,</li>
  <li>widget participation,</li>
  <li>front-panel composition,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>source-visible identity
      ->
when required to be unique
      ->
must remain distinct and valid
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>distinct source objects,</li>
  <li>ambiguous source identity,</li>
  <li>valid canonical source identity,</li>
  <li>duplicate-identifier rejection.</li>
</ul>

<p>
It therefore protects the rule:
</p>

<pre><code>distinct ids
      -&gt;
valid explicit source identity

multiple objects
      -/-> identifier ambiguity by default
</code></pre>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source as JSON,</li>
  <li>validate required section presence,</li>
  <li>validate uniqueness requirements for the relevant source family,</li>
  <li>accept the source as structurally valid canonical source,</li>
  <li>allow later semantic and derivation stages to proceed according to the published architecture.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>reject the file merely because multiple source objects exist in the same family,</li>
  <li>collapse distinct objects into one identity slot,</li>
  <li>treat valid explicit identity as a defect.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes a focused positive truth:
</p>

<pre><code>distinct source identifiers
    ->
canonical source still valid
    ->
acceptance required
</code></pre>

<p>
It anchors the positive side of the source-identity boundary in the FROG canonical source model.
</p>
