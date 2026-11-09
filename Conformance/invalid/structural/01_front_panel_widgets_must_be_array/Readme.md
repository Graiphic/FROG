<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Invalid Structural Case — Front Panel Widgets Must Be an Array</h1>

<p align="center">
  <strong>Loadable canonical source rejected because <code>front_panel.widgets</code> is not an array</strong><br/>
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
  <li><a href="#why-this-case-must-fail">8. Why This Case Must Fail</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines a loadable <code>.frog</code> source file that must be rejected at structural validation because <code>front_panel.widgets</code> is an object instead of an array.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case exists to make one conservative schema-owned front-panel rule publicly testable:
</p>

<pre><code>front_panel.widgets
    MUST be an array when present
</code></pre>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li>Case family: invalid / structural</li>
  <li>Primary owner: <code>Expression/</code></li>
  <li>Expected loadability: loadable</li>
  <li>Expected structural validity: invalid</li>
  <li>Expected meaning: not established</li>
</ul>

<hr/>

<h2 id="normative-basis">4. Normative Basis</h2>

<p>
This case is grounded in the published source-shape rule that <code>widgets</code> is an array when present inside <code>front_panel</code>.
</p>

<hr/>

<h2 id="associated-source-artifact">5. Associated Source Artifact</h2>

<p>
The source artifact associated with this case is:
</p>

<pre><code>Conformance/invalid/structural/01_front_panel_widgets_must_be_array/case.frog
</code></pre>

<p>
That artifact is the authoritative concrete input for this case. This README explains why it must be rejected.
</p>

<hr/>

<h2 id="input-shape">6. Input Shape</h2>

<p>
The associated source artifact contains a present <code>front_panel</code> object, but its <code>widgets</code> field is incorrectly serialized as an object.
</p>

<hr/>

<h2 id="expected-outcome">7. Expected Outcome</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: invalid
Expected meaning: not established
Expected rejection reason: front_panel.widgets is not an array
</code></pre>

<hr/>

<h2 id="why-this-case-must-fail">8. Why This Case Must Fail</h2>

<p>
This case must fail before semantic validation because the source shape violates an explicit front-panel structural rule.
</p>

<hr/>

<h2 id="summary">9. Summary</h2>

<p>
This is a narrow structural rejection case for incorrect <code>front_panel.widgets</code> shape.
</p>
