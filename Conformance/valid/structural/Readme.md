<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Valid Structural Conformance Cases</h1>

<p align="center">
  <strong>Positive structural-source acceptance cases for canonical <code>.frog</code> programs</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose-of-this-subtree">2. Purpose of this Subtree</a></li>
  <li><a href="#ownership-boundary">3. Ownership Boundary</a></li>
  <li><a href="#how-to-read-these-cases">4. How to Read These Cases</a></li>
  <li><a href="#published-cases">5. Published Cases</a></li>
  <li><a href="#expected-outcomes">6. Expected Outcomes</a></li>
  <li><a href="#what-these-cases-do-not-prove">7. What These Cases Do Not Prove</a></li>
  <li><a href="#relation-with-the-rest-of-conformance">8. Relation with the Rest of Conformance</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This subtree groups valid structural conformance cases for canonical <code>.frog</code> source.
</p>

<p>
The cases in this subtree are intentionally narrow. They verify that a source artifact is:
</p>

<ul>
  <li>loadable as canonical JSON source,</li>
  <li>structurally valid as canonical <code>.frog</code> source under the published <code>Expression/</code> ownership boundary,</li>
  <li>accepted before later semantic, IR, lowering, backend, or runtime-specific questions are considered.</li>
</ul>

<p>
These cases are therefore about source-shape truth, not full language closure.
</p>

<hr/>

<h2 id="purpose-of-this-subtree">2. Purpose of this Subtree</h2>

<p>
This subtree exists to make focused structural acceptance rules public and testable without overloading the historical top-level <code>Conformance/valid/</code> block.
</p>

<p>
It is appropriate when:
</p>

<ul>
  <li>the relevant law is already published in <code>Expression/</code>,</li>
  <li>the expected result is structural acceptance,</li>
  <li>the case does not need to prove a richer semantic, IR, or executable property.</li>
</ul>

<p>
Conceptually:
</p>

<pre><code>loadable source
+
structurally valid canonical shape
    -&gt;
accepted here
</code></pre>

<hr/>

<h2 id="ownership-boundary">3. Ownership Boundary</h2>

<p>
This subtree does not invent new rules.
</p>

<p>
Its cases map back to published source ownership, especially:
</p>

<ul>
  <li><code>Expression/Readme.md</code>,</li>
  <li><code>Expression/Schema.md</code>,</li>
  <li><code>Expression/schema/frog.schema.json</code>,</li>
  <li>the relevant section-owning documents such as <code>Front panel.md</code> and <code>Widget.md</code>.</li>
</ul>

<p>
These cases therefore test:
</p>

<ul>
  <li>source shape,</li>
  <li>required-versus-optional section discipline,</li>
  <li>selected section-local structural rules.</li>
</ul>

<p>
They do not own semantic meaning, IR law, or runtime truth.
</p>

<hr/>

<h2 id="how-to-read-these-cases">4. How to Read These Cases</h2>

<p>
Each case directory SHOULD contain:
</p>

<ul>
  <li>a <code>Readme.md</code> that explains the boundary being tested,</li>
  <li>a <code>case.frog</code> artifact that provides the concrete canonical-source input.</li>
</ul>

<p>
The <code>case.frog</code> file is the authoritative concrete input.
The local README explains why that input must be accepted structurally.
</p>

<hr/>

<h2 id="published-cases">5. Published Cases</h2>

<p>
The initial published structural valid cases are:
</p>

<ul>
  <li><code>01_front_panel_canvas_widgets_and_ui_libraries</code></li>
  <li><code>02_front_panel_recursive_children_shape_is_valid</code></li>
</ul>

<p>
Together, they expose two useful positive source-shape truths:
</p>

<ul>
  <li>the conservative published <code>front_panel</code> object shape is accepted,</li>
  <li>recursive <code>children</code> shape is accepted structurally when serialized correctly.</li>
</ul>

<hr/>

<h2 id="expected-outcomes">6. Expected Outcomes</h2>

<p>
The expected outcome class for this subtree is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
Expected downstream progress: permitted to proceed according to later published rules
</code></pre>

<p>
This does not mean a case in this subtree proves full semantic richness.
It only means structural acceptance must occur at the correct stage.
</p>

<hr/>

<h2 id="what-these-cases-do-not-prove">7. What These Cases Do Not Prove</h2>

<p>
Cases in this subtree do not automatically prove:
</p>

<ul>
  <li>widget class legality,</li>
  <li>role/class compatibility,</li>
  <li>type legality beyond source-shape acceptance,</li>
  <li>diagram-side widget interaction legality,</li>
  <li>IR construction correctness,</li>
  <li>runtime behavior.</li>
</ul>

<p>
Those belong to later validation layers or other conformance families.
</p>

<hr/>

<h2 id="relation-with-the-rest-of-conformance">8. Relation with the Rest of Conformance</h2>

<p>
This subtree should be read as structured growth under the broader positive conformance surface:
</p>

<pre><code>Conformance/valid/
    ├── historical top-level anchors
    ├── compiler/
    ├── executable/
    └── structural/
</code></pre>

<p>
It does not replace the top-level valid anchors.
It provides a clearer home for focused structural-source growth.
</p>

<hr/>

<h2 id="summary">9. Summary</h2>

<p>
This subtree groups positive structural conformance cases for canonical source.
</p>

<p>
It exists to make source-shape acceptance explicit, testable, and attributable without confusing:
</p>

<ul>
  <li>structural validity,</li>
  <li>semantic acceptance,</li>
  <li>IR correctness,</li>
  <li>runtime truth.</li>
</ul>
