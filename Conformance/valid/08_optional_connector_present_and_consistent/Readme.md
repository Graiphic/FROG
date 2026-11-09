<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Valid Conformance Case 08</h1>

<p align="center">
  <strong>Optional connector presence is valid when it consistently projects the declared public interface</strong><br/>
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
This case defines a structurally valid canonical <code>.frog</code> source file in which the optional <code>connector</code> section is present and consistently projects the already-declared public interface.
</p>

<p>
The case exists to publish the positive structural rule for connector presence:
a connector is valid when it remains a graphical node-side projection of the public logical boundary already owned by <code>interface</code>.
</p>

<hr/>

<h2 id="case-purpose">2. Case Purpose</h2>

<p>
This case verifies that a conforming implementation accepts a source file where:
</p>

<ul>
  <li><code>interface</code> declares the public ports,</li>
  <li><code>connector</code> is present,</li>
  <li><code>connector</code> maps only those already-declared public ports,</li>
  <li>no connector-side ownership collapse occurs.</li>
</ul>

<p>
It makes the following truth explicit:
</p>

<pre><code>connector present
+
connector consistent with interface
   -&gt;
canonical source valid
</code></pre>

<hr/>

<h2 id="case-classification">3. Case Classification</h2>

<ul>
  <li><strong>Case family:</strong> valid</li>
  <li><strong>Primary owner:</strong> <code>Expression/</code></li>
  <li><strong>Primary boundary:</strong> canonical source structural validity</li>
  <li><strong>Case kind:</strong> canonical source acceptance with present consistent <code>connector</code></li>
  <li><strong>Expected loadability:</strong> loadable</li>
  <li><strong>Expected structural validity:</strong> valid</li>
  <li><strong>Expected meaning:</strong> established or eligible to be established for the represented slice</li>
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
  <li><code>interface</code> defines the public typed inputs and outputs of the FROG,</li>
  <li><code>connector</code> is optional,</li>
  <li>when present, <code>connector</code> graphically projects the public interface when the FROG is reused as a node,</li>
  <li><code>connector</code> never defines new logical ports.</li>
</ul>

<p>
Therefore, a connector is structurally valid when it remains subordinate to and consistent with the public logical boundary declared in <code>interface</code>.
</p>

<pre><code>interface
   -&gt; owns public logical ports

connector
   -&gt; graphically projects those ports

connector consistent with interface
   -&gt; canonical source valid
</code></pre>

<hr/>

<h2 id="input-shape">5. Input Shape</h2>

<p>
The associated valid source artifact for this case SHOULD be a JSON-loadable <code>.frog</code> file with:
</p>

<ul>
  <li>a valid top-level object root,</li>
  <li>all required sections present,</li>
  <li>a present <code>connector</code> section,</li>
  <li>an <code>interface</code> declaring one or more public ports,</li>
  <li>a <code>connector</code> that maps only those already-declared port identifiers,</li>
  <li>a valid minimal <code>diagram</code>.</li>
</ul>

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
      }
    ],
    "outputs": [
      {
        "id": "out_sum",
        "name": "sum",
        "type": "float64"
      }
    ]
  },
  "connector": {
    "ports": [
      {
        "id": "in_a",
        "side": "left",
        "label": "a"
      },
      {
        "id": "out_sum",
        "side": "right",
        "label": "sum"
      }
    ]
  },
  "diagram": {
    "nodes": [],
    "edges": []
  }
}</code></pre>

<p>
In this shape, the connector does not invent logical ports.
It only projects the interface that already exists.
</p>

<hr/>

<h2 id="expected-outcome">6. Expected Outcome</h2>

<p>
The expected outcome is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established or eligible to be established for the represented slice
Expected preservation:
  connector remains a projection of interface
  rather than a second logical contract surface
</code></pre>

<p>
A conforming implementation MUST accept the case as canonical source.
It MUST preserve the distinction between:
</p>

<ul>
  <li><code>interface</code> as public logical contract,</li>
  <li><code>connector</code> as graphical projection.</li>
</ul>

<hr/>

<h2 id="why-this-case-must-pass">7. Why this Case Must Pass</h2>

<p>
The published source model does not forbid connector presence.
It defines how connector presence remains valid.
</p>

<p>
This case must therefore pass because:
</p>

<ul>
  <li>the required canonical sections are present,</li>
  <li><code>connector</code> is allowed to be present,</li>
  <li>the connector remains consistent with the already-declared interface,</li>
  <li>the connector does not attempt to define new logical ports or blur section ownership.</li>
</ul>

<pre><code>interface present
+
connector present
+
connector references only declared interface ports
   -&gt;
acceptance required
</code></pre>

<hr/>

<h2 id="what-this-case-is-not-testing">8. What this Case Is Not Testing</h2>

<p>
This case is intentionally narrow.
It is not testing:
</p>

<ul>
  <li>the full richness of connector layout conventions,</li>
  <li>diagram execution semantics,</li>
  <li>widget participation,</li>
  <li>front-panel composition,</li>
  <li>runtime behavior,</li>
  <li>backend-family behavior.</li>
</ul>

<p>
It only tests one boundary:
</p>

<pre><code>connector present
   -&gt;
must remain consistent with interface ownership
</code></pre>

<hr/>

<h2 id="preservation-and-boundary-notes">9. Preservation and Boundary Notes</h2>

<p>
This case helps preserve the architectural distinction between:
</p>

<ul>
  <li>public logical contract,</li>
  <li>graphical reusable-node projection,</li>
  <li>valid projection,</li>
  <li>ownership collapse.</li>
</ul>

<p>
It therefore protects the rule:
</p>

<pre><code>connector present
   -/-> second interface definition

graphical mapping
   -/-> logical declaration
</code></pre>

<p>
This matters because conformance must publish not only that invalid connector uses are rejected, but also that correct connector use is accepted.
</p>

<hr/>

<h2 id="implementation-requirements">10. Implementation Requirements</h2>

<p>
A conforming implementation handling this case SHOULD:
</p>

<ul>
  <li>load the source as JSON,</li>
  <li>validate required section presence,</li>
  <li>validate that the connector refers only to already-declared interface ports,</li>
  <li>accept the source as structurally valid canonical source,</li>
  <li>allow later semantic and derivation stages to proceed according to the published architecture.</li>
</ul>

<p>
A conforming implementation MUST NOT:
</p>

<ul>
  <li>reject the file merely because a connector is present,</li>
  <li>reinterpret the connector as a second interface-definition surface,</li>
  <li>collapse projection ownership into logical-boundary ownership.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This case establishes a focused positive truth:
</p>

<pre><code>connector present and consistent with interface
   -&gt;
canonical source still valid
   -&gt;
acceptance required
</code></pre>

<p>
It anchors the positive side of the connector boundary in the FROG canonical source model.
</p>
