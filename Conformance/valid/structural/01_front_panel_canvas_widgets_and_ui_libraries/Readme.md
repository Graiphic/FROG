<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Structural Case — Front Panel Canvas, Widgets, and UI Libraries</h1>

<p align="center">
  <strong>Structurally valid canonical source with an explicit <code>front_panel</code> object using the conservative published front-panel shape</strong><br/>
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
This case defines a structurally valid canonical <code>.frog</code> source file whose <code>front_panel</code> section uses the published conservative structure:
</p>

<ul>
  <li><code>canvas</code> as an object,</li>
  <li><code>widgets</code> as an array,</li>
  <li><code>ui_libraries</code> as an array.</li>
</ul>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to verify that a conforming implementation accepts a source file where the front panel remains within the explicit structural ownership published by <code>Expression/Front panel.md</code> and the conservative machine-checkable posture published by <code>Expression/Schema.md</code> and <code>Expression/schema/frog.schema.json</code>.
</p>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li>Case family: valid / structural</li>
  <li>Primary owner: <code>Expression/</code></li>
  <li>Expected loadability: loadable</li>
  <li>Expected structural validity: valid</li>
  <li>Expected meaning: established</li>
  <li>Expected IR derivation: permitted to proceed according to later published rules</li>
</ul>

<hr/>

<h2 id="normative-basis">4. Normative Basis</h2>

<p>
This case is grounded in the published canonical-source rules that:
</p>

<ul>
  <li><code>front_panel</code> is optional,</li>
  <li>when present it is a source-owned UI composition section,</li>
  <li><code>canvas</code> is an object when present,</li>
  <li><code>widgets</code> is an array when present,</li>
  <li><code>ui_libraries</code> is an array when present.</li>
</ul>

<hr/>

<h2 id="associated-source-artifact">5. Associated Source Artifact</h2>

<p>
The source artifact associated with this case is:
</p>

<pre><code>Conformance/valid/structural/01_front_panel_canvas_widgets_and_ui_libraries/case.frog
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
  <li>a valid top-level canonical source object,</li>
  <li>a present <code>front_panel</code> object,</li>
  <li>an object-valued <code>canvas</code>,</li>
  <li>an array-valued <code>widgets</code>,</li>
  <li>an array-valued <code>ui_libraries</code>,</li>
  <li>a widget instance with the structurally required fields <code>id</code>, <code>role</code>, and <code>widget</code>.</li>
</ul>

<hr/>

<h2 id="expected-outcome">7. Expected Outcome</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established
Expected preservation: front_panel remains source-owned UI composition
</code></pre>

<hr/>

<h2 id="why-this-case-must-pass">8. Why This Case Must Pass</h2>

<p>
This case must pass because it stays inside published source-shape law without claiming any extra semantic guarantees.
</p>

<pre><code>front_panel object
+
canvas object
+
widgets array
+
ui_libraries array
+
structurally valid widget instance
    -&gt;
structural acceptance required
</code></pre>

<hr/>

<h2 id="what-this-case-is-not-testing">9. What This Case Is Not Testing</h2>

<ul>
  <li>widget class legality,</li>
  <li>role/class compatibility,</li>
  <li>value-type legality,</li>
  <li>diagram-side widget interaction,</li>
  <li>runtime realization.</li>
</ul>

<hr/>

<h2 id="summary">10. Summary</h2>

<p>
This is a narrow structural acceptance case for the conservative published <code>front_panel</code> shape.
</p>
