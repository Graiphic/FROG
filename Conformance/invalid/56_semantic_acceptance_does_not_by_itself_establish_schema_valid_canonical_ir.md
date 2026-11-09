<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Conformance Case — Invalid: Semantic Acceptance Does Not by Itself Establish Schema-Valid Canonical Execution IR</h1>

<p align="center">
  <strong>Invalid conformance case for the false claim that established program meaning alone is sufficient to claim schema-valid canonical Execution IR in FROG v0.1</strong><br/>
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
  <li><a href="#why-semantic-acceptance-is-insufficient-here">7. Why Semantic Acceptance Is Insufficient Here</a></li>
  <li><a href="#illustrative-invalid-reading">8. Illustrative Invalid Reading</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr />

<h2 id="case-overview">1. Case Overview</h2>

<p>
This invalid case covers any program, tool behavior, validation behavior, derivation behavior, construction behavior, or conformance claim that treats semantic acceptance as though it were already sufficient proof of schema-valid canonical Execution IR.
</p>

<p>
Typical anti-patterns include:
</p>

<ul>
  <li>declaring a program fully IR-conforming immediately after semantic validation,</li>
  <li>skipping canonical IR derivation checks because meaning was established,</li>
  <li>skipping canonical JSON construction checks because meaning was established,</li>
  <li>skipping IR schema validation because the program is semantically accepted,</li>
  <li>treating “valid meaning exists” as equivalent to “canonical IR has been correctly emitted and validated”.</li>
</ul>

<hr />

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected loadability:</strong> loadable</p>

<p><strong>Expected structural validity:</strong> valid</p>

<p><strong>Expected meaning:</strong> established</p>

<p><strong>Expected IR result:</strong> not yet established by that fact alone</p>

<p><strong>Expected IR schema result:</strong> not established by that fact alone</p>

<p><strong>Expected IR architectural result:</strong> not established by that fact alone</p>

<p>
<strong>Expected rejection:</strong>
semantic acceptance has been incorrectly used as a substitute for later IR derivation, construction, and schema validation stages.
</p>

<hr />

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
The anti-pattern under test is:
</p>

<pre><code>semantic acceptance
      therefore
schema-valid canonical IR already established
</code></pre>

<p>
A minimal conceptual example is:
</p>

<pre><code>tool behavior:
  validate source
  establish program meaning
  stop there
  still claim:
    canonical IR is already valid
    canonical JSON is already schema-valid
</code></pre>

<p>
That claim is invalid unless the later IR stages have actually been satisfied.
</p>

<hr />

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>semantic acceptance versus IR derivation,</li>
  <li>IR derivation versus IR construction,</li>
  <li>IR construction versus schema validation,</li>
  <li>meaning existence versus canonical JSON IR validity.</li>
</ul>

<hr />

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because FROG v0.1 defines a staged corridor, not a collapsed one.
</p>

<p>
It is invalid because:
</p>

<ul>
  <li>semantic acceptance belongs to the Language-owned boundary,</li>
  <li>derivation belongs to the IR derivation boundary,</li>
  <li>construction belongs to the IR construction boundary,</li>
  <li>schema-valid canonical JSON belongs to the IR schema boundary,</li>
  <li>none of those later stages is automatically established merely because meaning exists.</li>
</ul>

<p>
The required rule is:
</p>

<pre><code>semantic acceptance
      !=
schema-valid canonical IR by itself
</code></pre>

<hr />

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator, conformance reader, or architecture-aware toolchain should reject this claim when semantic acceptance is being used as a substitute for actual IR derivation, construction, or schema validation.
</p>

<p>
It should report that at least one later stage remains unproven or unsatisfied:
</p>

<ul>
  <li>canonical IR may not yet have been derived,</li>
  <li>canonical IR may not yet have been properly constructed,</li>
  <li>canonical JSON may not yet have been emitted,</li>
  <li>the emitted IR may not yet have been checked against the published schema family.</li>
</ul>

<hr />

<h2 id="why-semantic-acceptance-is-insufficient-here">7. Why Semantic Acceptance Is Insufficient Here</h2>

<p>
Semantic acceptance is necessary.
It is not sufficient for this later claim.
</p>

<p>
That is because semantic acceptance establishes:
</p>

<ul>
  <li>that validated meaning exists,</li>
  <li>that the program has crossed the semantic boundary successfully.</li>
</ul>

<p>
But semantic acceptance does not by itself prove:
</p>

<ul>
  <li>that canonical IR derivation preserved all required distinctions,</li>
  <li>that canonical IR construction produced the required document shape,</li>
  <li>that canonical JSON IR was emitted correctly,</li>
  <li>that the emitted JSON satisfies the published IR schema family.</li>
</ul>

<p>
This case therefore protects the staged reading:
</p>

<pre><code>accepted meaning
      is upstream of
schema-valid canonical IR

not identical to it
</code></pre>

<hr />

<h2 id="illustrative-invalid-reading">8. Illustrative Invalid Reading</h2>

<p>
A simplified invalid reading is:
</p>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected meaning: established

Claimed conclusion:
  canonical IR is already valid
  canonical JSON IR is already schema-valid
</code></pre>

<p>
That conclusion is invalid when no explicit later-stage justification has been established.
</p>

<p>
The invalidity is therefore not:
</p>

<pre><code>the meaning is false
</code></pre>

<p>
The invalidity is:
</p>

<pre><code>the later IR-stage claim is being asserted
without satisfying the later IR stages
</code></pre>

<hr />

<h2 id="summary">9. Summary</h2>

<p>
This case must be rejected because FROG v0.1 does not allow semantic acceptance alone to stand in for canonical IR derivation, construction, and schema validation.
</p>

<p>
A conforming implementation must keep the stages distinct and must not claim schema-valid canonical Execution IR unless that later claim has actually been established.
</p>

<p>
The essential rule is:
</p>

<pre><code>semantic acceptance
    does not by itself establish
schema-valid canonical Execution IR
</code></pre>
