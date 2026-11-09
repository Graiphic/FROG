<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 57</h1>

<p align="center">
  <strong>Region ownership must remain recoverable in canonical Execution IR</strong><br/>
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
<code>57_region_ownership_must_remain_recoverable_in_canonical_execution_ir</code>
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
a region that exists in the canonical Execution IR remains explicitly and recoverably owned by its structure object rather than floating as an unattached graph fragment.
</p>

<hr />

<h2 id="why">3. Why</h2>

<p>
This case exists to make one structural rule explicit:
</p>

<pre><code>explicit region
        requires
recoverable structure ownership
</code></pre>

<p>
In base FROG v0.1, structured control remains structured in the open IR.
That requires region ownership to remain explicit and recoverable rather than being left to guesswork or hidden internal nesting conventions.
</p>

<hr />

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
This case exercises the preservation boundary across:
</p>

<pre><code>validated structure
        |
        v
canonical Execution IR Document
        |
        v
structured execution object
        +
owned region record(s)
        +
recoverable ownership relation
</code></pre>

<p>
The distinction under test is between:
</p>

<ul>
  <li>a structured execution object with explicit owned region identity, and</li>
  <li>a payload where region-local graph content exists but the owning structure relation is no longer recoverable.</li>
</ul>

<hr />

<h2 id="scenario">5. Scenario</h2>

<p>
A valid FROG program contains a structured control construct such as:
</p>

<ul>
  <li><code>case</code>,</li>
  <li><code>for_loop</code>,</li>
  <li><code>while_loop</code>.</li>
</ul>

<p>
That structure derives to:
</p>

<ul>
  <li>one primary structure object,</li>
  <li>one or more explicit region records or an equivalent explicit region-bearing form,</li>
  <li>a recoverable ownership relation from each region to the owning structure.</li>
</ul>

<p>
The case is valid if the canonical Execution IR preserves that ownership relation explicitly, for example through:
</p>

<ul>
  <li>a region record carrying <code>owner_object_id</code>,</li>
  <li>a structure object carrying explicit <code>region_ids</code>,</li>
  <li>or both, where both remain mutually coherent.</li>
</ul>

<hr />

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
This case is valid because:
</p>

<ul>
  <li>the source is valid,</li>
  <li>semantic meaning is established,</li>
  <li>structured control remains explicit in the open IR,</li>
  <li>region identity remains present and recoverable,</li>
  <li>the owning structure relation remains explicit and recoverable,</li>
  <li>the resulting canonical IR remains schema-valid and architecturally valid.</li>
</ul>

<p>
This case therefore asserts:
</p>

<pre><code>region identity
        is not enough

region ownership
        must also remain recoverable
</code></pre>

<hr />

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>the structure family remains explicit,</li>
  <li>the structure owns one or more regions,</li>
  <li>region-local content belongs to a recoverable structured scope,</li>
  <li>the region is not merely an anonymous grouping artifact.</li>
</ul>

<p>
The semantic reading is therefore:
</p>

<pre><code>validated structured control
        survives as
structure object
        +
owned region identity
        +
recoverable ownership relation
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
        "id": "obj.loop.main",
        "family": "structure",
        "role": "primary",
        "structure_family": "while_loop",
        "region_ids": ["region.loop.body"]
      }
    ],
    "regions": [
      {
        "id": "region.loop.body",
        "owner_object_id": "obj.loop.main",
        "member_object_ids": ["obj.loop.body.add"],
        "member_connection_ids": ["conn.loop.body.1"]
      }
    ]
  }
}
</code></pre>

<p>
The exact identifiers are illustrative.
The conformance point is that the region does not exist independently of a recoverable owning structure relation.
</p>

<hr />

<h2 id="expected-preservation">9. Expected Preservation</h2>

<p>
A conforming implementation must preserve all of the following:
</p>

<ul>
  <li>structure family identity,</li>
  <li>region identity,</li>
  <li>region-to-owner relation,</li>
  <li>region-local membership relation where published,</li>
  <li>the distinction between structured ownership and mere graph grouping.</li>
</ul>

<p>
At minimum, later tooling must be able to recover:
</p>

<ul>
  <li>which structure owns the region,</li>
  <li>which region is under discussion,</li>
  <li>which region-local content belongs to that structure-owned region.</li>
</ul>

<hr />

<h2 id="what-must-not-happen">10. What Must Not Happen</h2>

<p>
A conforming implementation must not do any of the following:
</p>

<ul>
  <li>emit region records with no recoverable owning structure,</li>
  <li>treat region ownership as inferable only from private nesting state,</li>
  <li>flatten structure ownership away while still claiming recoverable explicit regions,</li>
  <li>treat region-local content as detached from structure identity.</li>
</ul>

<p>
The forbidden collapse is:
</p>

<pre><code>region record exists
        -/-> therefore ownership may be left implicit
</code></pre>

<hr />

<h2 id="rationale">11. Rationale</h2>

<p>
This rule matters because FROG v0.1 keeps structured control explicit at the canonical IR boundary.
If region ownership becomes unrecoverable, then:
</p>

<ul>
  <li>structure family meaning weakens,</li>
  <li>boundary-crossing interpretation weakens,</li>
  <li>region-local diagnostics weaken,</li>
  <li>later lowering loses a stable structural anchor.</li>
</ul>

<p>
FROG therefore needs the stronger rule:
</p>

<pre><code>explicit region
    must remain
explicitly owned
</code></pre>

<hr />

<h2 id="minimum-conformance-reading">12. Minimum Conformance Reading</h2>

<p>
A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>a structured object derives into canonical IR,</li>
  <li>its region or regions remain explicit,</li>
  <li>their owning structure remains explicitly recoverable,</li>
  <li>the resulting IR is schema-valid and architecturally valid.</li>
</ul>

<p>
The public truth asserted by this case is:
</p>

<pre><code>region ownership
        must remain recoverable
in canonical Execution IR
</code></pre>
