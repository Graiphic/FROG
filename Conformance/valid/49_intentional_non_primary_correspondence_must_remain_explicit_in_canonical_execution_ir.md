<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 49</h1>

<p align="center">
  <strong>Intentional non-primary correspondence must remain explicit in canonical Execution IR</strong><br/>
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
<code>49_intentional_non_primary_correspondence_must_remain_explicit_in_canonical_execution_ir</code>
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
a source-visible declaration that does not become a primary execution object remains explicitly recoverable through canonical correspondence carriers rather than through silence or private implementation convention.
</p>

<hr />

<h2 id="why">3. Why</h2>

<p>
This case exists to make one architectural rule explicit:
</p>

<pre><code>intentional non-primary outcome
            requires
explicit recoverable correspondence

not
silent omission
</code></pre>

<p>
Base FROG v0.1 allows some source-visible contributors to remain outside the set of primary execution-facing IR objects.
That is valid only when the non-primary outcome is still explicit and recoverable where the specification requires it.
</p>

<p>
This case therefore protects the distinction between:
</p>

<ul>
  <li>a deliberate non-primary outcome that remains architecturally explicit, and</li>
  <li>an implementation that simply drops source ownership and leaves later tooling unable to explain what happened.</li>
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
explicit correspondence without forced primary execution identity
</code></pre>

<p>
The distinction under test is between:
</p>

<ul>
  <li>non-primary but recoverable source-visible participation, and</li>
  <li>identity loss disguised as omission.</li>
</ul>

<hr />

<h2 id="scenario">5. Scenario</h2>

<p>
A valid FROG program contains a source-visible declaration whose identity matters to recoverability but which does not need to become a second independent primary execution object in the canonical open IR.
</p>

<p>
Typical examples include:
</p>

<ul>
  <li>an interface declaration entry that corresponds to explicit boundary participation in the diagram,</li>
  <li>a widget declaration whose identity is referenced by a valid <code>widget_value</code> or <code>widget_reference</code> participation object,</li>
  <li>another declaration-side contributor that remains relevant to recoverability without becoming its own primary execution object.</li>
</ul>

<p>
The case is valid if the canonical Execution IR preserves that relation explicitly, for example through:
</p>

<ul>
  <li><code>unit.correspondence[]</code> records,</li>
  <li>compatible <code>correspondence_refs</code> from participating objects,</li>
  <li>explicit recoverable declaration linkage compatible with the published IR identity, derivation, construction, and schema posture.</li>
</ul>

<hr />

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
This case is valid because FROG v0.1 does <strong>not</strong> require every source-visible contributor to become a primary execution object.
</p>

<p>
It is valid because:
</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>the declaration-side contributor is still relevant to recoverability,</li>
  <li>the canonical IR keeps that relevance explicit through correspondence rather than through silent absence,</li>
  <li>the resulting IR remains compatible with the published canonical JSON IR posture,</li>
  <li>the resulting IR remains architecturally faithful.</li>
</ul>

<p>
This case therefore asserts:
</p>

<pre><code>not primary
    does not mean
not recoverable
</code></pre>

<hr />

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>the source-visible declaration is not automatically promoted into a second primary execution object,</li>
  <li>the source-visible declaration is still part of the recoverable architectural story,</li>
  <li>the boundary between declaration identity and execution participation identity remains explicit,</li>
  <li>the non-primary outcome is intentional and recorded rather than accidental.</li>
</ul>

<p>
The semantic reading is therefore:
</p>

<pre><code>declaration relevance
        survives as
recoverable correspondence

not as
forced extra execution object
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
        "id": "obj.boundary.in.main",
        "family": "boundary",
        "role": "primary",
        "correspondence_refs": ["corr.interface.in.gain"]
      }
    ],
    "source_map": [
      {
        "ir_id": "obj.boundary.in.main",
        "sources": [
          {
            "kind": "source_node",
            "path": "diagram.nodes.interface_input_gain",
            "id": "diagram.nodes.interface_input_gain"
          }
        ]
      }
    ],
    "correspondence": [
      {
        "id": "corr.interface.in.gain",
        "kind": "declaration_reference",
        "source_ref": {
          "kind": "source_interface_entry",
          "path": "interface.inputs.gain",
          "id": "interface.inputs.gain"
        },
        "ir_refs": ["obj.boundary.in.main"]
      }
    ]
  }
}
</code></pre>

<p>
The exact identifiers are illustrative.
The conformance point is that the declaration-side contributor remains explicitly recoverable through canonical IR correspondence carriers.
</p>

<hr />

<h2 id="expected-preservation">9. Expected Preservation</h2>

<p>
A conforming implementation must preserve all of the following:
</p>

<ul>
  <li>the distinction between declaration identity and execution-object identity,</li>
  <li>the distinction between primary execution participation and non-primary correspondence,</li>
  <li>the explicit recoverability of the declaration-side contributor,</li>
  <li>compatibility with published canonical IR carriers such as <code>source_map[]</code> and <code>correspondence[]</code>,</li>
  <li>the IR architectural distinction between declaration reference and accidental omission.</li>
</ul>

<p>
At minimum, later tooling must be able to recover:
</p>

<ul>
  <li>which source-visible declaration was involved,</li>
  <li>which IR-side object or objects it relates to,</li>
  <li>that the relationship is declaration-side correspondence rather than a second independent execution object.</li>
</ul>

<hr />

<h2 id="what-must-not-happen">10. What Must Not Happen</h2>

<p>
A conforming implementation must not do any of the following:
</p>

<ul>
  <li>force every declaration-side contributor into an additional primary execution object merely because it exists in source,</li>
  <li>drop the declaration-side contributor completely while still claiming recoverability,</li>
  <li>treat silence as sufficient proof of intentional non-primary outcome,</li>
  <li>replace explicit correspondence with undocumented positional convention,</li>
  <li>confuse declaration reference with primary execution identity.</li>
</ul>

<p>
The forbidden collapse is:
</p>

<pre><code>source-visible declaration
        -/-> silent omission

intentional non-primary outcome
        -/-> undocumented disappearance
</code></pre>

<hr />

<h2 id="rationale">11. Rationale</h2>

<p>
This distinction matters because the FROG IR boundary is meant to stay inspectable, attributable, and implementation-independent.
</p>

<p>
If declaration-side contributors may disappear silently, then:
</p>

<ul>
  <li>IR inspection becomes weaker,</li>
  <li>source-to-IR explanations become less reliable,</li>
  <li>independent implementations become harder to compare,</li>
  <li>conformance loses the ability to distinguish intentional omission from accidental identity loss.</li>
</ul>

<p>
FROG therefore needs the stronger rule:
</p>

<pre><code>non-primary is allowed
but only when recoverability remains explicit
</code></pre>

<hr />

<h2 id="minimum-conformance-reading">12. Minimum Conformance Reading</h2>

<p>
A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>a source-visible contributor does not become a second primary execution object,</li>
  <li>that non-primary outcome remains explicitly recoverable in canonical Execution IR,</li>
  <li>the resulting IR is schema-valid and architecturally valid,</li>
  <li>the distinction between non-primary correspondence and identity loss remains explicit.</li>
</ul>

<p>
The public truth asserted by this case is:
</p>

<pre><code>intentional non-primary correspondence
            must remain explicit in
canonical Execution IR
</code></pre>
