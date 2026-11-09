<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Case — Optional Front Panel Present and Structurally Valid</h1>

<p align="center">
  <strong>Structurally valid canonical source with a present <code>front_panel</code> section that remains within published source ownership boundaries</strong><br/>
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
  <li><a href="#why-this-case-must-pass">8. Why this Case Must Pass</a></li>
  <li><a href="#what-this-case-is-not-testing">9. What this Case Is Not Testing</a></li>
  <li><a href="#preservation-and-boundary-notes">10. Preservation and Boundary Notes</a></li>
  <li><a href="#implementation-requirements">11. Implementation Requirements</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines a structurally valid canonical <code>.frog</code> source file in which the optional <code>front_panel</code> section is present and structurally well-formed.
</p>

<p>
The case exists to show the positive structural rule for front-panel presence: a <code>front_panel</code> is valid when it remains a source-level UI composition surface and does not attempt to replace the executable authority of the <code>diagram</code>, the public boundary owned by <code>interface</code>, the widget instance model owned by <code>Widget.md</code>, the class-side widget contract owned by <code>Widget class contract.md</code>, or the diagram-side widget interaction model owned by <code>Widget interaction.md</code>.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation accepts a source file where:
</p>

<ul>
  <li><code>front_panel</code> is present,</li>
  <li>its top-level shape is structurally valid,</li>
  <li>its widget-side content remains inside the published front-panel and widget-instance source family,</li>
  <li>it does not encode executable graph ownership that belongs to <code>diagram</code>,</li>
  <li>it does not collapse class-side widget member legality into front-panel composition.</li>
</ul>

<p>
It makes the following truth explicit:
</p>

<pre><code>front_panel present
+
front_panel structurally valid
      -&gt;
canonical source valid
</code></pre>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li>Case family: valid</li>
  <li>Primary owner: <code>Expression/</code></li>
  <li>Case kind: canonical source acceptance with present structurally valid <code>front_panel</code></li>
  <li>Expected loadability: loadable</li>
  <li>Expected structural validity: valid</li>
  <li>Expected meaning: established</li>
  <li>Expected IR derivation: permitted to proceed according to the published downstream rules</li>
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
  <li><code>front_panel</code> is optional,</li>
  <li>when present, it defines the user-facing interaction layer through widget instances, layout, style, and related UI composition data,</li>
  <li><code>front_panel</code> does not define the public API,</li>
  <li><code>front_panel</code> does not replace the executable graph,</li>
  <li><code>front_panel</code> does not own the full widget class contract,</li>
  <li><code>front_panel</code> does not own the full executable widget interaction model,</li>
  <li>any executable relationship involving front-panel behavior must still pass through <code>diagram</code>.</li>
</ul>

<p>
Therefore, a front panel is structurally valid when it remains within those source ownership boundaries.
</p>

<pre><code>front_panel
    -&gt; optional UI composition source

Widget.md
    -&gt; widget instance model

Widget class contract.md
    -&gt; class-side member legality

Widget interaction.md
    -&gt; diagram-side executable widget access

diagram
    -&gt; authoritative executable source

front_panel present and well-formed
    -&gt; canonical source valid
</code></pre>

<hr/>

<h2 id="associated-source-artifact">5. Associated Source Artifact</h2>

<p>
The source artifact associated with this case is:
</p>

<pre><code>Conformance/valid/09_optional_front_panel_present_and_structurally_valid/case.frog
</code></pre>

<p>
That artifact is the authoritative concrete input for this case. This README explains why it must be accepted.
</p>

<hr/>

<h2 id="input-shape">6. Input Shape</h2>

<p>
The associated valid source artifact is a JSON-loadable <code>.frog</code> file with:
</p>

<ul>
  <li>a valid top-level object root,</li>
  <li>all required sections present,</li>
  <li>a present <code>front_panel</code> section,</li>
  <li>a structurally valid front-panel widget collection under <code>front_panel.widgets</code>,</li>
  <li>no executable edge ownership misplaced into <code>front_panel</code>,</li>
  <li>a valid minimal <code>diagram</code>.</li>
</ul>

<p>
The associated <code>case.frog</code> uses the following representative shape:
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {
    "name": "optional_front_panel_present_and_structurally_valid",
    "description": "Valid conformance case: front_panel is present and structurally valid.",
    "author": "FROG Conformance",
    "version": "0.1.0",
    "tags": [
      "conformance",
      "valid",
      "front-panel",
      "canonical-source",
      "ui-composition"
    ]
  },
  "interface": {
    "inputs": [],
    "outputs": []
  },
  "diagram": {
    "nodes": [],
    "edges": []
  },
  "front_panel": {
    "widgets": [
      {
        "id": "widget_1",
        "role": "control",
        "widget": "frog.ui.standard.numeric_control"
      }
    ]
  }
}
</code></pre>

<p>
In this shape, the front panel stays in its source role as UI composition. It does not attempt to own executable graph content, class-side widget legality, or executable widget interaction behavior.
</p>

<hr/>

<h2 id="expected-outcome">7. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
Expected preservation: front_panel remains distinct from diagram, interface, widget class contract, and widget interaction ownership
</code></pre>

<p>
A conforming implementation MUST accept the associated <code>case.frog</code> as canonical source. It MUST preserve the distinction between:
</p>

<ul>
  <li><code>front_panel</code> as optional UI composition source,</li>
  <li><code>diagram</code> as authoritative executable source,</li>
  <li><code>interface</code> as public logical boundary,</li>
  <li><code>Widget.md</code> as widget instance model ownership,</li>
  <li><code>Widget class contract.md</code> as class-side member legality ownership,</li>
  <li><code>Widget interaction.md</code> as diagram-side executable widget-access ownership.</li>
</ul>

<hr/>

<h2 id="why-this-case-must-pass">8. Why this Case Must Pass</h2>

<p>
The published source model does not forbid front-panel presence. It defines how front-panel presence remains valid.
</p>

<p>
This case must therefore pass because:
</p>

<ul>
  <li>the required canonical sections are present,</li>
  <li><code>front_panel</code> is allowed to be present,</li>
  <li>the front panel remains structurally well-formed,</li>
  <li>the associated widget instance uses the structurally coherent instance fields <code>id</code>, <code>role</code>, and <code>widget</code>,</li>
  <li>the front panel does not attempt to take ownership of executable graph content, public interface ownership, class-side member legality, or executable widget interaction ownership.</li>
</ul>

<pre><code>front_panel present
+
front_panel structurally valid
+
diagram ownership preserved
+
class-side widget legality not collapsed into composition
    -&gt;
acceptance required
</code></pre>

<hr/>

<h2 id="what-this-case-is-not-testing">9. What this Case Is Not Testing</h2>

<p>
This case is intentionally narrow. It is not testing:
</p>

<ul>
  <li>full widget semantics,</li>
  <li>widget class legality,</li>
  <li><code>widget_value</code> versus <code>widget_reference</code> semantic behavior,</li>
  <li>diagram-side widget interaction,</li>
  <li>runtime UI realization,</li>
  <li>backend-family handling of UI participation.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>front_panel present
      -&gt;
must remain structurally valid
      -&gt;
must remain in its source ownership role
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">10. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>front-panel composition,</li>
  <li>widget instance serialization,</li>
  <li>class-side member legality,</li>
  <li>diagram-side widget interaction,</li>
  <li>public interface definition,</li>
  <li>diagram execution content,</li>
  <li>valid UI-source presence,</li>
  <li>ownership collapse.</li>
</ul>

<p>
It therefore protects the rules:
</p>

<pre><code>front_panel present
      -/-> executable authority

UI composition source
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
  <li>validate required section presence,</li>
  <li>validate the structural shape of <code>front_panel</code>,</li>
  <li>accept the associated <code>case.frog</code> as structurally valid canonical source,</li>
  <li>allow later semantic and derivation stages to proceed according to the published architecture.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>reject the file merely because <code>front_panel</code> is present,</li>
  <li>reinterpret <code>front_panel</code> as executable graph ownership,</li>
  <li>collapse front-panel composition into public-interface ownership,</li>
  <li>treat widget instance serialization as if it were the full class-side widget contract,</li>
  <li>treat front-panel composition as if it directly defined executable widget interaction semantics.</li>
</ul>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
This case establishes a focused positive truth:
</p>

<pre><code>front_panel present and structurally valid
    -&gt;
canonical source still valid
    -&gt;
acceptance required
</code></pre>

<p>
It anchors the positive side of the front-panel structural boundary in the FROG canonical source model while keeping composition, widget instance model, class contract, and executable widget interaction explicitly distinct.
</p>
