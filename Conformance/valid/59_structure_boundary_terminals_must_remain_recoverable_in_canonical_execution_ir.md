<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 59</h1>

<p align="center">
  <strong>Structure-boundary terminals must remain recoverable in canonical Execution IR</strong><br/>
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
<code>59_structure_boundary_terminals_must_remain_recoverable_in_canonical_execution_ir</code>
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
structure-boundary crossing points remain explicitly recoverable in canonical Execution IR rather than being reduced to ordinary unconstrained connectivity.
</p>

<hr />

<h2 id="why">3. Why</h2>

<p>
This case exists to make one structural rule explicit:
</p>

<pre><code>explicit structure boundary crossing
            requires
recoverable boundary-terminal role
</code></pre>

<p>
In base FROG v0.1, structured control remains structured in the open IR.
That means communication across a structure wall must remain explicit and recoverable rather than becoming indistinguishable from ordinary flat graph connectivity.
</p>

<hr />

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
This case exercises the preservation boundary across:
</p>

<pre><code>validated structured control
        |
        v
canonical Execution IR Document
        |
        v
structure object
        +
owned region relation
        +
recoverable structure-boundary terminal role
</code></pre>

<p>
The distinction under test is between:
</p>

<ul>
  <li>an explicit structure-boundary crossing, and</li>
  <li>a flat connection pattern that leaves the crossing role implicit or ambiguous.</li>
</ul>

<hr />

<h2 id="scenario">5. Scenario</h2>

<p>
A valid FROG program contains a structure whose execution meaning requires values to cross the structure wall.
Typical examples include:
</p>

<ul>
  <li>data entering a <code>case</code> structure,</li>
  <li>data entering or leaving a <code>for_loop</code>,</li>
  <li>data entering or leaving a <code>while_loop</code>.</li>
</ul>

<p>
The case is valid if the canonical Execution IR preserves enough information to recover:
</p>

<ul>
  <li>that a crossing exists,</li>
  <li>which structure owns that crossing,</li>
  <li>whether the crossing is boundary-related rather than ordinary same-scope connectivity,</li>
  <li>which side of the structure wall the related ports or terminals belong to.</li>
</ul>

<p>
This recoverability may be carried through:
</p>

<ul>
  <li>explicit structure-boundary terminal support objects,</li>
  <li>equivalent structured boundary-port representation,</li>
  <li>or another explicit representation compatible with the published IR derivation, construction, identity, and schema posture.</li>
</ul>

<hr />

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
This case is valid because:
</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>the structure remains explicit in canonical IR,</li>
  <li>the boundary crossing remains explicit and recoverable,</li>
  <li>the related structure ownership remains recoverable,</li>
  <li>the resulting IR is schema-valid and architecturally valid.</li>
</ul>

<p>
This case therefore asserts:
</p>

<pre><code>cross-structure communication
        is not just
ordinary flat connectivity
</code></pre>

<hr />

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>the structure wall remains semantically relevant,</li>
  <li>the crossing point remains recoverable,</li>
  <li>boundary-related flow remains distinguishable from same-scope flow,</li>
  <li>later tools can still identify the structure-facing role of the crossing.</li>
</ul>

<p>
The semantic reading is therefore:
</p>

<pre><code>validated structure crossing
        survives as
recoverable boundary-terminal relation
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
        "id": "obj.case.main",
        "family": "structure",
        "role": "primary",
        "structure_family": "case",
        "region_ids": ["region.case.true", "region.case.false"]
      },
      {
        "id": "obj.support.case.input.x",
        "family": "support",
        "role": "support",
        "support_kind": "structure_boundary_terminal"
      }
    ],
    "regions": [
      {
        "id": "region.case.true",
        "owner_object_id": "obj.case.main"
      },
      {
        "id": "region.case.false",
        "owner_object_id": "obj.case.main"
      }
    ],
    "connections": [
      {
        "id": "conn.case.in.1",
        "source": {
          "object_id": "obj.upstream",
          "port_id": "out"
        },
        "destination": {
          "object_id": "obj.support.case.input.x",
          "port_id": "in"
        }
      }
    ]
  }
}
</code></pre>

<p>
The exact identifiers are illustrative.
The conformance point is that the structure-boundary crossing role remains explicitly recoverable rather than being flattened into undifferentiated connectivity.
</p>

<hr />

<h2 id="expected-preservation">9. Expected Preservation</h2>

<p>
A conforming implementation must preserve all of the following:
</p>

<ul>
  <li>structure identity,</li>
  <li>structure-owned region identity where applicable,</li>
  <li>boundary-crossing recoverability,</li>
  <li>distinction between boundary-terminal role and ordinary port role,</li>
  <li>attachment of the crossing relation to the owning structure context.</li>
</ul>

<p>
At minimum, later tooling must be able to recover:
</p>

<ul>
  <li>which structure is involved,</li>
  <li>which crossing is under discussion,</li>
  <li>that the crossing is boundary-related rather than ordinary flat connectivity.</li>
</ul>

<hr />

<h2 id="what-must-not-happen">10. What Must Not Happen</h2>

<p>
A conforming implementation must not do any of the following:
</p>

<ul>
  <li>collapse all structure-boundary crossings into ordinary same-scope connections,</li>
  <li>leave boundary-crossing role recoverable only through private nesting state,</li>
  <li>flatten the structure wall away while still claiming recoverable structured control in canonical IR,</li>
  <li>erase which structure owns the crossing relation.</li>
</ul>

<p>
The forbidden collapse is:
</p>

<pre><code>boundary crossing exists
        -/-> therefore boundary-terminal role may remain implicit
</code></pre>

<hr />

<h2 id="rationale">11. Rationale</h2>

<p>
This rule matters because structure-boundary recoverability is one of the core ways FROG keeps structured control explicit and inspectable at the open IR boundary.
</p>

<p>
If structure-boundary terminals become unrecoverable, then:
</p>

<ul>
  <li>structure meaning weakens,</li>
  <li>region and structure ownership become less useful,</li>
  <li>later lowering loses a clear structured anchor,</li>
  <li>debugging and diagnostics across structure walls become less reliable.</li>
</ul>

<p>
FROG therefore needs the stronger rule:
</p>

<pre><code>explicit structure wall crossing
    must remain
explicitly recoverable
</code></pre>

<hr />

<h2 id="minimum-conformance-reading">12. Minimum Conformance Reading</h2>

<p>
A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>a structure derives into canonical IR,</li>
  <li>its boundary crossing points remain recoverable,</li>
  <li>the related boundary-terminal role remains distinguishable from ordinary connectivity,</li>
  <li>the resulting IR is schema-valid and architecturally valid.</li>
</ul>

<p>
The public truth asserted by this case is:
</p>

<pre><code>structure-boundary terminals
        must remain recoverable
in canonical Execution IR
</code></pre>
