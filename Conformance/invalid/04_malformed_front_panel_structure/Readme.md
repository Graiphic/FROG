<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Case — Malformed Front Panel Structure</h1>

<p align="center">
  <strong>Structurally invalid canonical source due to a malformed <code>front_panel</code> section</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#case-purpose">2. Case Purpose</a></li>
  <li><a href="#case-classification">3. Case Classification</a></li>
  <li><a href="#normative-basis">4. Normative Basis</a></li>
  <li><a href="#associated-source-artifact">5. Associated Source Artifact</a></li>
  <li><a href="#input-shape">6. Input Shape</a></li>
  <li><a href="#expected-outcome">7. Expected Outcome</a></li>
  <li><a href="#why-this-case-must-fail">8. Why this Case Must Fail</a></li>
  <li><a href="#what-this-case-is-not-testing">9. What this Case Is Not Testing</a></li>
  <li><a href="#preservation-and-boundary-notes">10. Preservation and Boundary Notes</a></li>
  <li><a href="#implementation-requirements">11. Implementation Requirements</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines a canonical-source rejection expectation for a FROG file in which the optional <code>front_panel</code> section is present but malformed at the source-structure level.
</p>

<p>
The section is optional. However, once present, it MUST satisfy the canonical source-shape rules that apply to front-panel composition and widget containment. If the section has the wrong source form, the file is not a structurally valid canonical <code>.frog</code> source file.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation does not collapse:
</p>

<pre><code>optional section
      !=
structurally unconstrained section
</code></pre>

<p>
An optional section may be absent. But if it is present, it must still satisfy the source-structure rules owned by <code>Expression/</code>.
</p>

<p>
This case also exists to verify that a conforming implementation does not blur:
</p>

<pre><code>front_panel composition
      !=
widget instance model
      !=
widget class contract
      !=
diagram-side widget interaction
</code></pre>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li>Case family: invalid</li>
  <li>Primary owner: <code>Expression/</code></li>
  <li>Case kind: source-shape / structural-validity rejection</li>
  <li>Expected loadability: loadable</li>
  <li>Expected structural validity: invalid</li>
  <li>Expected meaning: not established</li>
  <li>Expected IR derivation: must not proceed as if validated meaning existed</li>
</ul>

<hr/>

<h2 id="normative-basis">4. Normative Basis</h2>

<p>
This case is grounded in the published canonical-source requirements of <code>Expression/</code>.
</p>

<p>
In FROG v0.1:
</p>

<ul>
  <li><code>front_panel</code> is an optional top-level source section,</li>
  <li>when present, it defines front-panel composition and widget-side source content,</li>
  <li>its presence does not make it free-form or implementation-private,</li>
  <li>widget instances remain governed at instance level by <code>Widget.md</code>,</li>
  <li>class-side member legality remains owned by <code>Widget class contract.md</code>,</li>
  <li>diagram-side executable widget interaction remains owned by <code>Widget interaction.md</code>,</li>
  <li>all executable linkage related to front-panel widgets must still pass through the <code>diagram</code>.</li>
</ul>

<p>
Therefore, a malformed <code>front_panel</code> section is a structural-source failure.
</p>

<pre><code>optional front_panel absent
    -&gt; allowed

optional front_panel present and well-formed
    -&gt; allowed

optional front_panel present but malformed
    -&gt; structural invalidity
</code></pre>

<hr/>

<h2 id="associated-source-artifact">5. Associated Source Artifact</h2>

<p>
The source artifact associated with this case is:
</p>

<pre><code>Conformance/invalid/04_malformed_front_panel_structure/case.frog
</code></pre>

<p>
That artifact is the authoritative concrete input for this case. This README explains why it must be rejected.
</p>

<hr/>

<h2 id="input-shape">6. Input Shape</h2>

<p>
The associated invalid source artifact is a JSON-loadable <code>.frog</code> file in which <code>front_panel</code> exists but is malformed as source.
</p>

<p>
In the current associated <code>case.frog</code>, the malformed shape is explicit:
</p>

<pre><code>"front_panel": [
  {
    "id": "widget_1",
    "role": "control",
    "widget": "frog.ui.standard.numeric_control"
  }
]
</code></pre>

<p>
That is invalid because <code>front_panel</code> must be an object when present, not an array.
</p>

<p>
More generally, typical invalid patterns include:
</p>

<ul>
  <li><code>front_panel</code> represented with the wrong top-level shape,</li>
  <li>widget collections represented with the wrong container type,</li>
  <li>widget entries missing required structural identity fields,</li>
  <li>widget containment encoded in a shape not permitted by the source model,</li>
  <li>mixing executable linkage directly into <code>front_panel</code> instead of expressing it through the <code>diagram</code>,</li>
  <li>treating front-panel composition as if it declared class-side widget legality.</li>
</ul>

<p>
This is not merely a UI-runtime issue. It is a canonical-source failure.
</p>

<hr/>

<h2 id="expected-outcome">7. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: invalid
Expected meaning: not established
Expected rejection: malformed front_panel structure
</code></pre>

<p>
A conforming implementation MUST reject the associated <code>case.frog</code> explicitly. It MUST NOT silently normalize malformed front-panel source into validity. It MUST NOT treat malformed UI source as harmless decoration. It MUST NOT continue as if validated meaning had been established.
</p>

<hr/>

<h2 id="why-this-case-must-fail">8. Why this Case Must Fail</h2>

<p>
The FROG source model allows optional sections, but optionality does not remove structural discipline.
</p>

<p>
If a malformed <code>front_panel</code> were tolerated as canonical source, then:
</p>

<ul>
  <li>source-level UI composition would become implementation-defined,</li>
  <li>tooling could silently reinterpret broken source in incompatible ways,</li>
  <li>canonical source validity would no longer be publicly checkable.</li>
</ul>

<p>
This case must therefore fail because:
</p>

<ul>
  <li><code>front_panel</code> remains part of canonical source when present,</li>
  <li>canonical source must stay structurally checkable,</li>
  <li>front-panel composition must remain distinct from the executable graph,</li>
  <li>source composition must remain distinct from class-side widget member legality,</li>
  <li>source composition must remain distinct from executable widget interaction behavior.</li>
</ul>

<pre><code>optional section
+
wrong source shape
    -&gt;
structural rejection required
</code></pre>

<hr/>

<h2 id="what-this-case-is-not-testing">9. What this Case Is Not Testing</h2>

<p>
This case is intentionally narrow. It is not testing:
</p>

<ul>
  <li>whether a widget class is semantically supported,</li>
  <li>whether a widget member is legally readable or writable,</li>
  <li>whether a method is invocable,</li>
  <li>diagram-side widget interaction semantics,</li>
  <li>runtime UI realization.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>front_panel present
      +
wrong source shape
      -&gt;
must be rejected structurally
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">10. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>optional section presence,</li>
  <li>structural validity,</li>
  <li>widget instance serialization,</li>
  <li>class-side widget contract,</li>
  <li>diagram-side widget interaction,</li>
  <li>diagram execution ownership.</li>
</ul>

<p>
It therefore protects the rules:
</p>

<pre><code>optional
  -/-> free-form

front_panel
  -/-> second diagram

front_panel
  -/-> class-side widget contract

front_panel
  -/-> executable widget interaction layer
</code></pre>

<hr/>

<h2 id="implementation-requirements">11. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source as JSON,</li>
  <li>detect that <code>front_panel</code> is structurally malformed,</li>
  <li>reject the source before semantic validation is considered established,</li>
  <li>report the rejection as a structural-source failure.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>silently repair malformed front-panel source into validity,</li>
  <li>reinterpret malformed widget-source shape as acceptable implementation-private UI state,</li>
  <li>treat malformed UI source as semantically harmless decoration,</li>
  <li>continue as if a valid widget instance model had already been established,</li>
  <li>continue as if class-side widget member legality or executable widget interaction had already become applicable.</li>
</ul>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
This case establishes a focused negative truth:
</p>

<pre><code>optional front_panel
+
malformed source shape
    -&gt;
structural rejection required
</code></pre>

<p>
It anchors the negative side of the front-panel structural boundary in the FROG canonical source model while keeping composition, widget instance model, class contract, and executable widget interaction explicitly distinct.
</p>
