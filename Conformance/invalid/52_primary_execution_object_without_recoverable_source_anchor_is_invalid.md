<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Conformance Case — Invalid: Primary Execution Object Without Recoverable Source Anchor Is Invalid</h1>

<p align="center">
  <strong>Invalid conformance case for a canonical Execution IR primary object that lacks explicit recoverable source attribution in FROG v0.1</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr />

<h2>Contents</h2>
<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#expected-status">2. Expected Status</a></li>
  <li><a href="#intended-anti-pattern">3. Intended Anti-Pattern</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-invalid">5. Why this Case Is Invalid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#why-ir-reading-is-forbidden">7. Why IR Reading Is Forbidden</a></li>
  <li><a href="#why-hidden-attribution-is-wrong">8. Why Hidden Attribution Is Wrong</a></li>
  <li><a href="#illustrative-invalid-ir-shape">9. Illustrative Invalid IR Shape</a></li>
  <li><a href="#summary">10. Summary</a></li>
</ul>

<hr />

<h2 id="case-overview">1. Case Overview</h2>

<p>
This invalid case covers a program, tool behavior, derivation behavior, construction behavior, or IR emission behavior that introduces or preserves a primary execution-facing IR object without keeping its source-side origin explicitly recoverable at the canonical open-IR boundary.
</p>

<p>
Typical anti-patterns include:
</p>

<ul>
  <li>emitting a primary object with no object-local attribution and no corresponding <code>source_map[]</code> anchoring,</li>
  <li>claiming that attribution can be reconstructed from object names alone,</li>
  <li>claiming that attribution exists only in private compiler state and therefore need not appear in canonical IR,</li>
  <li>dropping source attribution during normalization while still claiming semantic faithfulness and conforming IR emission.</li>
</ul>

<p>
Conceptually, the forbidden collapse is:
</p>

<pre><code>primary execution object
        ==
source anchor known only privately
</code></pre>

<p>
Base FROG v0.1 does not allow that equivalence at the canonical IR boundary.
</p>

<hr />

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected loadability:</strong> loadable</p>

<p><strong>Expected structural validity:</strong> valid</p>

<p><strong>Expected meaning:</strong> established or otherwise assumed by the invalid claim</p>

<p><strong>Expected IR result:</strong> not derivable as conforming canonical IR</p>

<p><strong>Expected IR schema result:</strong> not sufficient even if schema-valid shape is claimed</p>

<p><strong>Expected IR architectural result:</strong> invalid</p>

<p>
<strong>Expected rejection:</strong>
a primary execution object has been left without explicit recoverable source anchoring.
</p>

<hr />

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
The invalid intent is any source-to-IR behavior where a primary execution-facing object remains present but its recoverable source anchoring disappears from the canonical open IR.
</p>

<p>
A minimal conceptual anti-pattern is:
</p>

<pre><code>source:
  diagram.nodes.add_1 exists and is valid

tool interpretation:
  derive obj.primitive.add.main
  emit no explicit source anchor
  claim:
    "the source node can be inferred later"
</code></pre>

<p>
Another invalid reading is:
</p>

<pre><code>validated delay primitive
      |
      +-- explicit state-bearing object emitted
      |
      +-- source-side origin omitted from open IR
      |
      +-- tool claims attribution remains available internally
</code></pre>

<p>
That is not a valid architectural reading of base FROG v0.1.
</p>

<hr />

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>primary execution identity versus source-orphaned IR identity,</li>
  <li>explicit open-IR attribution versus hidden implementation attribution,</li>
  <li>recoverable source anchoring versus inferred naming coincidence,</li>
  <li>canonical IR carrier discipline versus private compiler convenience.</li>
</ul>

<hr />

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because a primary execution-facing object in canonical Execution IR must remain attributable to validated source-visible contributor(s).
</p>

<p>
More precisely, it is invalid because:
</p>

<ul>
  <li>primary execution objects are part of the public open-IR truth surface,</li>
  <li>their source-side origin must remain recoverable,</li>
  <li>private hidden attribution is not a substitute for canonical explicit attribution,</li>
  <li>the published IR posture includes explicit attribution carriers such as <code>source_map[]</code> when that boundary is in scope,</li>
  <li>schema-visible shape would still be insufficient if the IR remains architecturally source-orphaned.</li>
</ul>

<p>
The required distinction is:
</p>

<pre><code>recoverable source attribution
              !=
private hidden knowledge
</code></pre>

<hr />

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator, IR checker, or conformance reader should reject the case when the emitted or claimed IR result depends on that loss of explicit source anchoring.
</p>

<p>
It should report that at least one of the following boundaries has been violated:
</p>

<ul>
  <li>required source attribution for a primary execution object has been lost,</li>
  <li>canonical IR no longer preserves a recoverable source-side anchor,</li>
  <li>published attribution carrier expectations have been violated where the canonical IR JSON posture is in scope,</li>
  <li>the resulting IR is architecturally invalid even if some structural shape is still present.</li>
</ul>

<p>
If the omission is justified through implementation convenience, optimization, or “obvious” naming,
that should still be rejected.
None of those replaces explicit recoverability at the canonical open-IR boundary.
</p>

<hr />

<h2 id="why-ir-reading-is-forbidden">7. Why IR Reading Is Forbidden</h2>

<p>
A conforming canonical IR reading is forbidden for this invalid case because the claimed IR result fails to preserve the explicit source-attribution obligations of the published IR layer.
</p>

<p>
There is no conforming basis for claiming all of the following at once:
</p>

<ul>
  <li>the emitted object is primary,</li>
  <li>the object is part of canonical open IR,</li>
  <li>the object has no explicit recoverable source anchor in canonical IR,</li>
  <li>the result is still conforming.</li>
</ul>

<p>
That final step is exactly what base v0.1 rejects.
</p>

<hr />

<h2 id="why-hidden-attribution-is-wrong">8. Why Hidden Attribution Is Wrong</h2>

<p>
A conforming toolchain must not silently “repair” or rationalize this case by:
</p>

<ul>
  <li>keeping the attribution only in private runtime or compiler memory,</li>
  <li>assuming later tools can reconstruct the source anchor from identifier spelling,</li>
  <li>pretending that primary objects do not need published open-IR attribution carriers,</li>
  <li>treating hidden internal provenance as architecturally equivalent to explicit canonical attribution.</li>
</ul>

<p>
Such behavior would weaken inspection, comparability, diagnostics, and conformance trust at the exact point where FROG IR is intended to remain open and inspectable.
</p>

<p>
The forbidden repair pattern is:
</p>

<pre><code>primary object exists
        -/-> therefore source attribution may remain private only
</code></pre>

<hr />

<h2 id="illustrative-invalid-ir-shape">9. Illustrative Invalid IR Shape</h2>

<p>
A simplified invalid canonical IR reading of this anti-pattern would look like:
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
    "source_map": [],
    "correspondence": []
  }
}
</code></pre>

<p>
This shape is invalid for the case under test because the primary execution object is present but has no recoverable explicit source anchor at the canonical IR boundary.
</p>

<p>
The point is not that every IR array must always be populated identically.
The point is that a primary object must not become source-orphaned while still being presented as conforming canonical Execution IR.
</p>

<hr />

<h2 id="summary">10. Summary</h2>

<p>
This case must be rejected because FROG v0.1 does not allow a primary execution-facing IR object to remain without explicit recoverable source anchoring in canonical open IR.
</p>

<p>
A conforming implementation must reject source-to-IR behavior that leaves primary objects attributable only through private hidden state or undocumented inference.
</p>

<p>
The essential rule is:
</p>

<pre><code>primary execution object
    is not valid
without recoverable source anchor
in canonical Execution IR
</code></pre>
