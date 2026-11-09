<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 61</h1>

<p align="center">
  <strong>Structure-terminal roles must remain recoverable in canonical Execution IR</strong><br/>
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
<code>61_structure_terminal_roles_must_remain_recoverable_in_canonical_execution_ir</code>
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
structure-intrinsic terminal roles remain explicitly recoverable in canonical Execution IR rather than being degraded into ordinary undifferentiated ports.
</p>

<hr />

<h2 id="why">3. Why</h2>

<p>
This case exists to make one structural rule explicit:
</p>

<pre><code>structure terminal exists
        requires
recoverable structure-terminal role
</code></pre>

<p>
In base FROG v0.1, structured control is not only region ownership plus boundary crossings.
Some structures also carry terminal roles that are intrinsic to the structure itself.
Those roles must remain recoverable at the canonical IR boundary.
</p>

<p>
Typical examples include:
</p>

<ul>
  <li>a <code>case</code> selector terminal,</li>
  <li>a <code>for_loop</code> iteration-related terminal,</li>
  <li>a <code>while_loop</code> continuation-condition terminal.</li>
</ul>

<hr />

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
This case exercises the preservation boundary across:
</p>

<pre><code>validated structure terminal role
        |
        v
canonical Execution IR Document
        |
        v
structure object
        +
recoverable structure-terminal role
</code></pre>

<p>
The distinction under test is between:
</p>

<ul>
  <li>a terminal whose role is intrinsic to the meaning of the structure, and</li>
  <li>an ordinary generic port that happens to sit near the structure.</li>
</ul>

<hr />

<h2 id="scenario">5. Scenario</h2>

<p>
A valid FROG program contains a structure whose semantics depend on one or more intrinsic terminal roles.
Examples include:
</p>

<ul>
  <li>a boolean or string selector for a <code>case</code>,</li>
  <li>a continuation-condition terminal for a <code>while_loop</code>,</li>
  <li>an iteration-related terminal for a <code>for_loop</code>.</li>
</ul>

<p>
The case is valid if the canonical Execution IR preserves enough information to recover:
</p>

<ul>
  <li>that the terminal is structure-intrinsic,</li>
  <li>which structure owns it,</li>
  <li>which standardized role it carries,</li>
  <li>that its meaning is not equivalent to an ordinary boundary-crossing value.</li>
</ul>

<p>
This recoverability may be carried through:
</p>

<ul>
  <li>explicit structure-terminal support objects,</li>
  <li>equivalent structured terminal-port representation,</li>
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
  <li>the structure-terminal role remains explicitly recoverable,</li>
  <li>the owning structure relation remains recoverable,</li>
  <li>the resulting IR is schema-valid and architecturally valid.</li>
</ul>

<p>
This case therefore asserts:
</p>

<pre><code>structure-terminal role
        is not just
generic port metadata
</code></pre>

<hr />

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>the structure family remains explicit,</li>
  <li>the terminal role remains tied to the semantics of that structure family,</li>
  <li>the terminal is distinguishable from ordinary boundary crossing,</li>
  <li>later tooling can still identify which intrinsic structure role is under discussion.</li>
</ul>

<p>
The semantic reading is therefore:
</p>

<pre><code>validated structure-intrinsic terminal
        survives as
recoverable structure-terminal role
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
        "id": "obj.support.case.selector",
        "family": "support",
        "role": "support",
        "support_kind": "structure_terminal"
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
        "id": "conn.case.selector.1",
        "source": {
          "object_id": "obj.selector.source",
          "port_id": "out"
        },
        "destination": {
          "object_id": "obj.support.case.selector",
          "port_id": "in"
        }
      }
    ]
  }
}
</code></pre>

<p>
The exact identifiers are illustrative.
The conformance point is that the structure-terminal role remains explicitly recoverable rather than being reduced to an ordinary port with no structure-intrinsic meaning.
</p>

<hr />

<h2 id="expected-preservation">9. Expected Preservation</h2>

<p>
A conforming implementation must preserve all of the following:
</p>

<ul>
  <li>structure family identity,</li>
  <li>structure-terminal role recoverability,</li>
  <li>attachment of the terminal role to the owning structure,</li>
  <li>distinction between structure-terminal role and boundary-terminal role,</li>
  <li>distinction between structure-terminal role and ordinary port role.</li>
</ul>

<p>
At minimum, later tooling must be able to recover:
</p>

<ul>
  <li>which structure is involved,</li>
  <li>which terminal is under discussion,</li>
  <li>which intrinsic role that terminal carries.</li>
</ul>

<hr />

<h2 id="what-must-not-happen">10. What Must Not Happen</h2>

<p>
A conforming implementation must not do any of the following:
</p>

<ul>
  <li>collapse structure-terminal roles into ordinary generic ports,</li>
  <li>treat selector, loop-condition, or iteration-related roles as unrecoverable after derivation,</li>
  <li>leave the role recoverable only through private implementation knowledge,</li>
  <li>confuse structure-terminal roles with structure-boundary crossing roles.</li>
</ul>

<p>
The forbidden collapse is:
</p>

<pre><code>intrinsic structure terminal exists
        -/-> therefore role may remain generic or implicit
</code></pre>

<hr />

<h2 id="rationale">11. Rationale</h2>

<p>
This rule matters because some terminals are part of the standardized semantics of the structure itself.
If those roles become unrecoverable, then:
</p>

<ul>
  <li>structure-family meaning weakens,</li>
  <li>diagnostics around structured control weaken,</li>
  <li>later lowering loses one of its explicit structured anchors,</li>
  <li>independent implementations become harder to compare faithfully.</li>
</ul>

<p>
FROG therefore needs the stronger rule:
</p>

<pre><code>structure-terminal role
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
  <li>its intrinsic terminal role remains recoverable,</li>
  <li>that role remains distinguishable from ordinary ports and boundary terminals,</li>
  <li>the resulting IR is schema-valid and architecturally valid.</li>
</ul>

<p>
The public truth asserted by this case is:
</p>

<pre><code>structure-terminal roles
        must remain recoverable
in canonical Execution IR
</code></pre>
