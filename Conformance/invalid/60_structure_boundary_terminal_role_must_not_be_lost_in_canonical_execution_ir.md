<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Conformance Case — Invalid: Structure-Boundary Terminal Role Must Not Be Lost in Canonical Execution IR</h1>

<p align="center">
  <strong>Invalid conformance case for a canonical Execution IR reading that preserves connectivity but loses recoverable structure-boundary terminal role in FROG v0.1</strong><br/>
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
  <li><a href="#why-loss-of-boundary-role-is-forbidden">7. Why Loss of Boundary Role Is Forbidden</a></li>
  <li><a href="#illustrative-invalid-ir-shape">8. Illustrative Invalid IR Shape</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr />

<h2 id="case-overview">1. Case Overview</h2>

<p>
This invalid case covers any derivation, construction, emission, or conformance claim in which connectivity across a structure wall remains present, but the recoverable role of the structure-boundary crossing has been lost.
</p>

<p>
Typical anti-patterns include:
</p>

<ul>
  <li>rewriting structure-wall crossings as ordinary flat graph connections,</li>
  <li>preserving only the dataflow endpoints while erasing the fact that a structure boundary was crossed,</li>
  <li>claiming boundary-crossing role can be inferred later from nested implementation state,</li>
  <li>keeping a structure object and regions but dropping the recoverable crossing role between outside and inside scope.</li>
</ul>

<hr />

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected loadability:</strong> loadable</p>

<p><strong>Expected structural validity:</strong> valid or apparently valid at a superficial carrier level</p>

<p><strong>Expected meaning:</strong> established or otherwise assumed by the invalid claim</p>

<p><strong>Expected IR result:</strong> not derivable as conforming canonical IR</p>

<p><strong>Expected IR schema result:</strong> not sufficient even if structural shape appears acceptable</p>

<p><strong>Expected IR architectural result:</strong> invalid</p>

<p>
<strong>Expected rejection:</strong>
structure-boundary terminal role has been lost or made unrecoverable.
</p>

<hr />

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
The anti-pattern under test is:
</p>

<pre><code>cross-structure connectivity exists
      therefore
boundary-terminal role need not remain explicit
</code></pre>

<p>
A minimal conceptual example is:
</p>

<pre><code>tool behavior:
  derive a structure
  keep its region records
  keep some connections
  drop explicit structure-boundary crossing role
  claim:
    "the crossing can be inferred from the graph"
</code></pre>

<p>
Another invalid reading is:
</p>

<pre><code>outside object ----&gt; inside object

tool still claims:
  no recoverable boundary-terminal role is needed
</code></pre>

<p>
That is not a valid open-IR reading in base FROG v0.1.
</p>

<hr />

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>ordinary connectivity versus structure-boundary crossing,</li>
  <li>explicit structure wall semantics versus flat graph interpretation,</li>
  <li>recoverable terminal role versus hidden private nesting convention,</li>
  <li>carrier presence versus true structured-boundary recoverability.</li>
</ul>

<hr />

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because base FROG v0.1 keeps structured control explicit in canonical Execution IR.
If a structure wall is semantically relevant, then the crossing role across that wall must remain recoverable.
</p>

<p>
It is invalid because:
</p>

<ul>
  <li>flat connectivity is not equivalent to structure-boundary recoverability,</li>
  <li>structured control requires more than mere endpoint existence,</li>
  <li>private nesting inference is not a substitute for explicit open-IR structure-boundary meaning,</li>
  <li>schema-like carrier presence would still be insufficient if the structure-boundary role is gone.</li>
</ul>

<p>
The required rule is:
</p>

<pre><code>connectivity across a structure wall
      !=
recoverable boundary-terminal role by itself
</code></pre>

<hr />

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator, IR checker, or conformance reader should reject the case when structure-boundary role is missing, ambiguous, or recoverable only through undocumented private interpretation.
</p>

<p>
It should report that at least one of the following boundaries has failed:
</p>

<ul>
  <li>required structure-boundary role recoverability,</li>
  <li>required distinction between ordinary connectivity and structured crossing,</li>
  <li>required coherence between structure ownership and boundary-crossing role,</li>
  <li>required architectural validity of structured control in canonical IR.</li>
</ul>

<hr />

<h2 id="why-loss-of-boundary-role-is-forbidden">7. Why Loss of Boundary Role Is Forbidden</h2>

<p>
A conforming toolchain must not rationalize this case by:
</p>

<ul>
  <li>keeping only flat graph connectivity,</li>
  <li>assuming later tools can rediscover the crossing role,</li>
  <li>pretending that structure-boundary semantics are “obvious enough” from local layout,</li>
  <li>using structure records as decoration while boundary-crossing meaning has actually been flattened away.</li>
</ul>

<p>
Such behavior would preserve the appearance of structured IR while dropping one of the central recoverability obligations of structured control.
</p>

<p>
The forbidden repair pattern is:
</p>

<pre><code>crossing remains
        -/-> therefore boundary-terminal role may disappear
</code></pre>

<hr />

<h2 id="illustrative-invalid-ir-shape">8. Illustrative Invalid IR Shape</h2>

<p>
A simplified invalid canonical IR reading of this anti-pattern would look like:
</p>

<pre><code>{
  "unit": {
    "objects": [
      {
        "id": "obj.case.main",
        "family": "structure",
        "role": "primary",
        "structure_family": "case"
      },
      {
        "id": "obj.outside",
        "family": "primitive",
        "role": "primary"
      },
      {
        "id": "obj.inside",
        "family": "primitive",
        "role": "primary"
      }
    ],
    "connections": [
      {
        "id": "conn.1",
        "source": {
          "object_id": "obj.outside",
          "port_id": "out"
        },
        "destination": {
          "object_id": "obj.inside",
          "port_id": "in"
        }
      }
    ],
    "regions": [
      {
        "id": "region.case.true",
        "owner_object_id": "obj.case.main"
      }
    ]
  }
}
</code></pre>

<p>
This reading is invalid for the case under test if the connection crosses a structure wall but the IR no longer preserves that structured-boundary role explicitly enough to recover it.
</p>

<p>
The point is not only that connections exist.
The point is that crossing a structure wall is not semantically equivalent to ordinary same-scope connectivity.
</p>

<hr />

<h2 id="summary">9. Summary</h2>

<p>
This case must be rejected because FROG v0.1 does not allow structure-boundary terminal role to disappear while keeping only the appearance of connectivity.
</p>

<p>
A conforming implementation must reject canonical IR that preserves graph edges but loses the recoverable structured-boundary meaning of those edges.
</p>

<p>
The essential rule is:
</p>

<pre><code>structure-boundary terminal role
    must not be lost
in canonical Execution IR
</code></pre>
