<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Conformance Case — Invalid: Schema-Valid IR Shape Does Not Justify IR Architectural Collapse</h1>

<p align="center">
  <strong>Invalid conformance case for a canonical IR payload that may satisfy structural schema expectations while violating required open-IR architectural distinctions in FROG v0.1</strong><br/>
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
  <li><a href="#why-schema-validity-is-insufficient-here">7. Why Schema Validity Is Insufficient Here</a></li>
  <li><a href="#illustrative-invalid-reading">8. Illustrative Invalid Reading</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr />

<h2 id="case-overview">1. Case Overview</h2>

<p>
This invalid case covers a program, derivation, construction, or IR emission behavior in which the resulting canonical JSON payload may appear structurally acceptable, but a required IR architectural distinction has been erased, blurred, or replaced by undocumented interpretation.
</p>

<p>
Typical anti-patterns include:
</p>

<ul>
  <li>preserving the expected JSON categories while collapsing object-family meaning,</li>
  <li>keeping <code>source_map[]</code> and <code>correspondence[]</code> arrays present but using them in a way that no longer preserves recoverable distinctions,</li>
  <li>flattening structured control beyond recoverability while still emitting schema-compatible containers,</li>
  <li>collapsing <code>widget_value</code> and <code>widget_reference</code> into one generic family while still emitting valid-looking records,</li>
  <li>claiming that “the JSON passes” is sufficient proof of canonical IR correctness.</li>
</ul>

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
schema-compatible shape has been used to justify an IR architectural collapse.
</p>

<hr />

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
The anti-pattern under test is:
</p>

<pre><code>schema-valid shape
      therefore
architecturally valid IR
</code></pre>

<p>
A minimal conceptual example is:
</p>

<pre><code>tool behavior:
  emit objects[]
  emit connections[]
  emit regions[]
  emit source_map[]
  emit correspondence[]
  satisfy published JSON shape

but:
  collapse required architectural distinctions
  rely on private interpretation to recover meaning
</code></pre>

<p>
That is exactly the kind of false equivalence this case rejects.
</p>

<hr />

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>schema-validity versus IR architectural validity,</li>
  <li>carrier presence versus meaningful recoverability,</li>
  <li>structural JSON acceptance versus source-faithful IR preservation,</li>
  <li>open IR architecture versus private implementation interpretation.</li>
</ul>

<hr />

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because base FROG v0.1 does not define IR correctness as “schema-valid JSON only”.
</p>

<p>
It is invalid because:
</p>

<ul>
  <li>schema validity checks structural form,</li>
  <li>IR validity also requires architectural faithfulness,</li>
  <li>architectural distinctions remain normative even when the JSON carrier is accepted,</li>
  <li>carrier presence does not justify loss of recoverability or family distinction.</li>
</ul>

<p>
The required rule is:
</p>

<pre><code>schema-valid
      !=
IR-valid by itself
</code></pre>

<hr />

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator, conformance reader, or architecture-aware IR checker should reject the case if the emitted IR depends on that collapse.
</p>

<p>
It should report that at least one of the following has failed:
</p>

<ul>
  <li>required family distinction preservation,</li>
  <li>required identity or attribution recoverability,</li>
  <li>required non-primary correspondence clarity,</li>
  <li>required structural-control recoverability,</li>
  <li>required open-IR architectural faithfulness despite schema-compatible shape.</li>
</ul>

<hr />

<h2 id="why-schema-validity-is-insufficient-here">7. Why Schema Validity Is Insufficient Here</h2>

<p>
Schema validation is necessary.
It is not sufficient.
</p>

<p>
That is because JSON schema can validate:
</p>

<ul>
  <li>presence of categories,</li>
  <li>record shapes,</li>
  <li>selected enums and required fields,</li>
  <li>selected structural constraints.</li>
</ul>

<p>
But schema validation does not by itself prove:
</p>

<ul>
  <li>correct semantic correspondence,</li>
  <li>correct architectural preservation,</li>
  <li>correct distinction between intentional non-primary and accidental omission,</li>
  <li>correct preservation of structured-control identity,</li>
  <li>correct interpretation of all published IR families.</li>
</ul>

<p>
This case exists so that conformance keeps that layered reading explicit.
</p>

<hr />

<h2 id="illustrative-invalid-reading">8. Illustrative Invalid Reading</h2>

<p>
A simplified invalid reading of the anti-pattern is:
</p>

<pre><code>{
  "ir_version": "0.1",
  "kind": "execution_ir",
  "document_id": "doc.main",
  "unit": {
    "id": "unit.main",
    "objects": [ ... ],
    "connections": [ ... ],
    "regions": [ ... ],
    "source_map": [ ... ],
    "correspondence": [ ... ]
  }
}
</code></pre>

<p>
This shape may be structurally acceptable.
It is still invalid for the case under test if, for example:
</p>

<ul>
  <li>the object families no longer preserve required meaning,</li>
  <li>the attribution records no longer anchor the correct contributors,</li>
  <li>the correspondence records blur declaration reference with identity loss,</li>
  <li>the regions remain present but no longer preserve real structure ownership.</li>
</ul>

<p>
The invalidity is therefore not “the JSON is malformed”.
The invalidity is:
</p>

<pre><code>the JSON shape is not enough
because the IR architecture has been collapsed
</code></pre>

<hr />

<h2 id="summary">9. Summary</h2>

<p>
This case must be rejected because FROG v0.1 does not allow schema-compatible JSON shape to replace IR architectural correctness.
</p>

<p>
A conforming implementation must reject or fail a case where a payload may be schema-valid yet still violates required open-IR distinctions.
</p>

<p>
The essential rule is:
</p>

<pre><code>schema-valid IR shape
    does not justify
IR architectural collapse
</code></pre>
