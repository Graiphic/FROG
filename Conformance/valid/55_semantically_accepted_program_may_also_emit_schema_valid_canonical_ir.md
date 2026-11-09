<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 55</h1>

<p align="center">
  <strong>A semantically accepted program may also emit schema-valid canonical Execution IR</strong><br/>
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
  <li><a href="#expected-reading">7. Expected Reading</a></li>
  <li><a href="#expected-preservation">8. Expected Preservation</a></li>
  <li><a href="#what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#rationale">10. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr />

<h2 id="case-name">1. Case Name</h2>

<p>
Case:
<code>55_semantically_accepted_program_may_also_emit_schema_valid_canonical_ir</code>
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
the accepted program meaning is correctly carried across derivation and construction into a canonical Execution IR Document whose emitted JSON is schema-valid and architecturally faithful.
</p>

<hr />

<h2 id="why">3. Why</h2>

<p>
This case exists to show the positive full corridor:
</p>

<pre><code>semantic acceptance
      -&gt;
derivable canonical IR
      -&gt;
schema-valid canonical JSON IR
</code></pre>

<p>
The point is not that semantic acceptance automatically proves IR schema-validity by itself.
The point is that the full corridor can be satisfied, and this is what a conforming successful pipeline looks like.
</p>

<hr />

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
This case exercises the staged corridor:
</p>

<pre><code>loadable source
      -&gt;
structurally valid source
      -&gt;
validated meaning
      -&gt;
canonical Execution IR derivation
      -&gt;
canonical JSON IR emission
      -&gt;
IR schema validation
</code></pre>

<p>
The distinction under test is:
</p>

<ul>
  <li>semantic acceptance as one stage, and</li>
  <li>successful canonical IR emission and schema validation as later stages.</li>
</ul>

<hr />

<h2 id="scenario">5. Scenario</h2>

<p>
A valid FROG program is accepted semantically and then processed through the published IR corridor.
</p>

<p>
The resulting canonical Execution IR:
</p>

<ul>
  <li>is derivable from validated meaning,</li>
  <li>is constructed as one canonical Execution IR Document containing one execution unit,</li>
  <li>preserves the required attribution, correspondence, family distinctions, and boundary distinctions,</li>
  <li>and is emitted as canonical JSON that matches the published IR schema family.</li>
</ul>

<hr />

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
This case is valid because all of the following are true:
</p>

<ul>
  <li>the source is acceptable,</li>
  <li>semantic meaning is established,</li>
  <li>derivation succeeds without architectural collapse,</li>
  <li>construction produces the canonical Execution IR Document,</li>
  <li>the emitted canonical JSON satisfies the published IR schema,</li>
  <li>the resulting IR remains architecturally valid.</li>
</ul>

<p>
This case therefore asserts the positive combined reading:
</p>

<pre><code>semantic acceptance
    may lead to
schema-valid canonical IR

when derivation and construction are also correct
</code></pre>

<hr />

<h2 id="expected-reading">7. Expected Reading</h2>

<p>
A conforming reading of this case should allow all of the following to be true together:
</p>

<ul>
  <li>validated meaning exists,</li>
  <li>canonical Execution IR is derivable from that meaning,</li>
  <li>canonical IR is properly constructed,</li>
  <li>canonical JSON IR is schema-valid,</li>
  <li>IR architectural validity is preserved.</li>
</ul>

<p>
A compact reading is:
</p>

<pre><code>accepted meaning
      +
correct derivation
      +
correct construction
      +
schema-valid canonical IR
      +
architectural faithfulness
</code></pre>

<hr />

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>
A conforming implementation must preserve:
</p>

<ul>
  <li>the staged difference between semantic acceptance and IR schema validation,</li>
  <li>the required source-to-IR correspondence,</li>
  <li>the required architectural distinctions of the canonical IR boundary,</li>
  <li>the explicit carrier discipline required by the published schema posture.</li>
</ul>

<hr />

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>
A conforming implementation must not read this case as permission to:
</p>

<ul>
  <li>skip derivation or construction reasoning because semantic acceptance already succeeded,</li>
  <li>skip IR schema validation because the meaning is known to be valid,</li>
  <li>collapse schema-validity into semantic validity,</li>
  <li>collapse semantic validity into schema-validity.</li>
</ul>

<hr />

<h2 id="rationale">10. Rationale</h2>

<p>
This case matters because FROG v0.1 is explicitly staged.
Conformance therefore needs at least one positive case showing that those stages can align successfully without being conflated.
</p>

<p>
The corridor under test is:
</p>

<pre><code>accepted meaning
does not replace later IR stages

but
accepted meaning
can flow through later IR stages correctly
</code></pre>

<hr />

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>IR derivation succeeds,</li>
  <li>canonical JSON IR emission succeeds,</li>
  <li>the emitted IR is schema-valid and architecturally valid.</li>
</ul>

<p>
The public truth asserted by this case is:
</p>

<pre><code>a semantically accepted program
may also emit
schema-valid canonical Execution IR

when the later IR stages are also correct
</code></pre>
