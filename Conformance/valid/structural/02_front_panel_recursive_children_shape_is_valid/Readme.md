<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Structural Case — Recursive Front-Panel Children Shape Is Valid</h1>

<p align="center">
  <strong>Structurally valid canonical source with recursive widget children under a container-shaped front-panel widget instance</strong><br/>
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
  <li><a href="#why-this-case-must-pass">8. Why This Case Must Pass</a></li>
  <li><a href="#what-this-case-is-not-testing">9. What This Case Is Not Testing</a></li>
  <li><a href="#summary">10. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines a structurally valid canonical <code>.frog</code> source file whose front panel contains a recursive widget tree through <code>children</code>.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that recursive widget-instance shape is accepted structurally when <code>children</code> is an array of widget instances.
</p>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li>Case family: valid / structural</li>
  <li>Primary owner: <code>Expression/</code></li>
  <li>Expected loadability: loadable</li>
  <li>Expected structural validity: valid</li>
  <li>Expected meaning: established</li>
</ul>

<hr/>

<h2 id="normative-basis">4. Normative Basis</h2>

<p>
This case is grounded in the published source-shape rule that widget instances may recursively contain <code>children</code>, and that <code>children</code> is an array when present.
</p>

<hr/>

<h2 id="associated-source-artifact">5. Associated Source Artifact</h2>

<p>
The source artifact associated with this case is:
</p>

<pre><code>Conformance/valid/structural/02_front_panel_recursive_children_shape_is_valid/case.frog
</code></pre>

<p>
That artifact is the authoritative concrete input for this case. This README explains why it must be accepted.
</p>

<hr/>

<h2 id="input-shape">6. Input Shape</h2>

<p>
The associated source artifact contains:
</p>

<ul>
  <li>a present <code>front_panel.widgets</code> array,</li>
  <li>a parent widget instance,</li>
  <li>a <code>children</code> array,</li>
  <li>a nested child widget instance with structurally required fields.</li>
</ul>

<hr/>

<h2 id="expected-outcome">7. Expected Outcome</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
</code></pre>

<hr/>

<h2 id="why-this-case-must-pass">8. Why This Case Must Pass</h2>

<p>
This case must pass because the recursive shape itself is structurally valid.
</p>

<p>
It does not require the schema layer to prove container legality semantically. It only requires acceptance of the published recursive source shape.
</p>

<hr/>

<h2 id="what-this-case-is-not-testing">9. What This Case Is Not Testing</h2>

<ul>
  <li>whether the parent widget class is semantically permitted to contain children,</li>
  <li>class-side member legality,</li>
  <li>diagram participation,</li>
  <li>runtime UI behavior.</li>
</ul>

<hr/>

<h2 id="summary">10. Summary</h2>

<p>
This is a narrow structural acceptance case for recursive <code>children</code> shape in front-panel widget serialization.
</p>
