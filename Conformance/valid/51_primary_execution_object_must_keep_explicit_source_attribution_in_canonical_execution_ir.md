<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 51</h1>

<p align="center">
  <strong>Primary execution object must keep explicit source attribution in canonical Execution IR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr />

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-exercised">4. Boundary Being Exercised</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-valid">6. What Makes the Case Valid</a></li>
  <li><a href="#expected-meaning">7. Expected Meaning</a></li>
  <li><a href="#expected-ir-reading">8. Expected IR Reading</a></li>
  <li><a href="#expected-preservation">9. Expected Preservation</a></li>
  <li><a href="#what-must-not-happen">10. What Must Not Happen</a></li>
  <li><a href="#rationale">11. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">12. Minimum Conformance Reading</a></li>
</ul>

<hr />

<h2 id="case-name">1. Case Name</h2>

<p>
Case:
<code>51_primary_execution_object_must_keep_explicit_source_attribution_in_canonical_execution_ir</code>
</p>

<hr />

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected loadability:</strong> loadable</p>

<p><strong>Expected structural validity:</strong> valid</p>

<p><strong>Expected meaning:</strong> established</p>

<p><strong>Expected IR result:</strong> derivable</p>

<p><strong>Expected IR schema result:</strong> schema-valid</p>

<p><strong>Expected IR architectural result:</strong> valid</p>

<p>
<strong>Expected preservation:</strong>
a primary execution-facing IR object remains explicitly anchored to validated source-visible contributor(s) through canonical attribution carriers rather than through undocumented implementation inference.
</p>

<hr />

<h2 id="why">3. Why</h2>

<p>
This case exists to make one architectural rule explicit:
</p>

<pre><code>primary execution object
            requires
recoverable source attribution
</code></pre>

<p>
Base FROG v0.1 allows normalization, explicitification, and support-object expansion,
but it does not allow a primary execution-facing IR object to become source-orphaned at the canonical open-IR boundary.
</p>

<p>
This case protects the distinction between:
</p>

<ul>
  <li>a valid primary execution object with explicit recoverable source anchoring, and</li>
  <li>a generated primary object whose origin exists only in private implementation state.</li>
</ul>

<hr />

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
This case exercises the preservation boundary across:
</p>

<pre><code>validated source-visible contributor
            |
            v
validated program meaning
            |
            v
canonical Execution IR Document
            |
            v
primary execution object with explicit source anchor
</code></pre>

<p>
The distinction under test is between:
</p>

<ul>
  <li>primary execution identity with explicit attribution, and</li>
  <li>primary execution identity that depends only on hidden compiler memory.</li>
</ul>

<hr />

<h2 id="scenario">5. Scenario</h2>

<p>
A valid FROG program contains an execution-relevant validated contributor such as:
</p>

<ul>
  <li>a primitive node,</li>
  <li>a public interface participation node,</li>
  <li>a widget participation node,</li>
  <li>a structure node,</li>
  <li>an explicit local-memory primitive.</li>
</ul>

<p>
That contributor derives to one primary execution-facing IR object.
The case is valid if the canonical Execution IR keeps the source-side origin explicitly recoverable, for example through:
</p>

<ul>
  <li>object-local attribution,</li>
  <li><code>unit.source_map[]</code> records,</li>
  <li>or an equivalent explicit carrier compatible with the published IR identity, construction, and schema posture.</li>
</ul>

<p>
For the canonical JSON posture published for base v0.1, the preferred explicit carrier is <code>source_map[]</code>.
</p>

<hr />

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
This case is valid because the execution-facing object under test is:
</p>

<ul>
  <li>execution-relevant,</li>
  <li>semantically accepted,</li>
  <li>primary in the open IR,</li>
  <li>explicitly attributable to validated source-visible contributor(s),</li>
  <li>compatible with the published canonical JSON IR carrier posture,</li>
  <li>architecturally faithful.</li>
</ul>

<p>
This case therefore asserts:
</p>

<pre><code>primary execution identity
        must remain
source-recoverable
</code></pre>

<hr />

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>the source-visible contributor is execution-relevant,</li>
  <li>its primary IR-side execution role is valid,</li>
  <li>its source-side origin remains explicitly recoverable at the canonical IR boundary,</li>
  <li>the implementation does not need private hidden tables to explain where the primary object came from.</li>
</ul>

<p>
The semantic reading is therefore:
</p>

<pre><code>validated execution contributor
        survives as
primary execution object
with explicit source anchoring
</code></pre>

<hr />

<h2 id="expected-ir-reading">8. Expected IR Reading</h2>

<p>
A conforming canonical IR reading of this case should permit an interpretation equivalent to:
</p>

<pre><code>{
  "unit": {
    "objects": [
      {
        "id": "obj.primitive.add.main",
        "family": "primitive",
        "role": "primary",
        "primitive_id": "frog.math.add"
      }
    ],
    "source_map": [
      {
        "ir_id": "obj.primitive.add.main",
        "sources": [
          {
            "kind": "source_node",
            "path": "diagram.nodes.add_1",
            "id": "diagram.nodes.add_1"
          }
        ]
      }
    ],
    "correspondence": []
  }
}
</code></pre>

<p>
The exact identifiers are illustrative.
The conformance point is that the primary object has an explicit recoverable source anchor in canonical Execution IR.
</p>

<hr />

<h2 id="expected-preservation">9. Expected Preservation</h2>

<p>
A conforming implementation must preserve all of the following:
</p>

<ul>
  <li>the distinction between source-visible contributor identity and IR object identity,</li>
  <li>the explicit attribution relation between them,</li>
  <li>the ability to explain which source-side contributor produced the primary IR object,</li>
  <li>compatibility with published canonical IR carriers such as <code>source_map[]</code>,</li>
  <li>the IR architectural distinction between public open attribution and private implementation knowledge.</li>
</ul>

<p>
At minimum, later tooling must be able to recover:
</p>

<ul>
  <li>which IR object is under discussion,</li>
  <li>which source-visible contributor or contributors it came from,</li>
  <li>whether the attribution is direct or multi-contributor.</li>
</ul>

<hr />

<h2 id="what-must-not-happen">10. What Must Not Happen</h2>

<p>
A conforming implementation must not do any of the following:
</p>

<ul>
  <li>emit a primary execution object with no recoverable source anchor,</li>
  <li>rely only on undocumented object naming convention to reconstruct attribution,</li>
  <li>rely only on hidden internal compiler state to reconstruct attribution,</li>
  <li>claim that a primary object may remain unattributed because its source origin is “obvious”,</li>
  <li>erase contributor identity during normalization while still claiming conforming canonical IR.</li>
</ul>

<p>
The forbidden collapse is:
</p>

<pre><code>primary IR object
        -/-> source anchor recoverable only in private implementation memory
</code></pre>

<hr />

<h2 id="rationale">11. Rationale</h2>

<p>
This rule matters because FROG IR is meant to remain inspectable, attributable, portable, and implementation-independent.
</p>

<p>
If primary execution objects are allowed to lose explicit source anchoring, then:
</p>

<ul>
  <li>independent implementations become harder to compare,</li>
  <li>conformance becomes weaker,</li>
  <li>fault attribution becomes less trustworthy,</li>
  <li>later lowering and diagnostics lose a stable public anchor.</li>
</ul>

<p>
FROG therefore needs the stronger rule:
</p>

<pre><code>primary execution object
    must keep
explicit source attribution
</code></pre>

<hr />

<h2 id="minimum-conformance-reading">12. Minimum Conformance Reading</h2>

<p>
A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>one validated contributor becomes a primary execution-facing IR object,</li>
  <li>that primary object remains explicitly source-attributed,</li>
  <li>the attribution remains visible at the canonical IR boundary,</li>
  <li>the resulting IR is schema-valid and architecturally valid.</li>
</ul>

<p>
The public truth asserted by this case is:
</p>

<pre><code>primary execution object
            must keep
explicit source attribution
in canonical Execution IR
</code></pre>
