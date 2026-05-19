<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Semantic Validation Before FIR</h1>

<p align="center">
  <strong>Public contract for accepting validated program meaning before deriving Execution IR / FIR</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document defines the semantic validation gate that sits between
structurally valid canonical <code>.frog</code> source and canonical
Execution IR / FIR derivation.
</p>

<pre><code>canonical .frog source
   -&gt; loadability
   -&gt; structural validity
   -&gt; semantic validation before FIR
   -&gt; validated program meaning
   -&gt; FIR derivation</code></pre>

<p>
The gate exists to prevent FIR producers, lowerers, runtimes, or examples from
inventing missing meaning. FIR must be derived from accepted meaning, not from
guesses over raw source.
</p>

<hr/>

<h2>Non-Goals</h2>

<p>
This document does not define:
</p>

<ul>
  <li>the JSON source schema,</li>
  <li>the material FIR object model,</li>
  <li>the lowering contract,</li>
  <li>runtime scheduling, rendering, or host behavior,</li>
  <li>one private implementation architecture.</li>
</ul>

<p>
It defines what must be true before a conforming implementation may say that a
program has validated meaning eligible for FIR derivation.
</p>

<hr/>

<h2>Validation Inputs</h2>

<p>
Semantic validation starts from structurally valid canonical source.
</p>

<p>
The validation input may include:
</p>

<ul>
  <li>public interface declarations,</li>
  <li>diagram nodes, ports, edges, constants, calls, structures, and state carriers,</li>
  <li>type expressions and value constraints,</li>
  <li>front-panel widget instances where they participate in execution,</li>
  <li>widget class references and package references required to validate widget participation,</li>
  <li>library and primitive identities used by executable nodes,</li>
  <li>profile requirements explicitly used by the program,</li>
  <li>source attribution needed for diagnostics.</li>
</ul>

<p>
The validation input must not include runtime-private reconstruction,
host-rendered widget geometry, editor selection state, or compiler-private
lowering conveniences.
</p>

<hr/>

<h2>Validation Output</h2>

<p>
Semantic validation has exactly two public outcomes:
</p>

<table>
  <thead>
    <tr>
      <th>Outcome</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>accepted</code></td>
      <td>The program has validated program meaning and may enter FIR derivation.</td>
    </tr>
    <tr>
      <td><code>rejected</code></td>
      <td>The program has no validated program meaning for this specification surface and must not be derived into FIR.</td>
    </tr>
  </tbody>
</table>

<p>
A third implementation-local result may exist:
</p>

<table>
  <thead>
    <tr>
      <th>Result</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>unsupported</code></td>
      <td>The source may be valid FROG, but the implementation subset cannot validate or derive it yet. This must be explicit and must not be confused with invalid FROG.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Required Semantic Checks</h2>

<p>
For the accepted subset, semantic validation must establish all applicable
checks below before FIR derivation.
</p>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Required checks</th>
      <th>Owner</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Program boundary</td>
      <td>One accepted program meaning, compatible public interface, no conflicting boundary declarations.</td>
      <td><code>Language/</code>, <code>Expression/Interface.md</code></td>
    </tr>
    <tr>
      <td>Types</td>
      <td>All typed values, ports, constants, widget values, and call boundaries are well typed under published type rules.</td>
      <td><code>Expression/Type.md</code>, <code>Language/</code></td>
    </tr>
    <tr>
      <td>Nodes</td>
      <td>Each executable node has a known family, legal ports, legal arity, legal direction, and legal value constraints.</td>
      <td><code>Language/</code>, <code>Libraries/</code>, <code>Profiles/</code></td>
    </tr>
    <tr>
      <td>Edges</td>
      <td>Each edge connects legal endpoints, preserves direction, respects structure boundaries, and has compatible value flow.</td>
      <td><code>Language/</code></td>
    </tr>
    <tr>
      <td>Interface participation</td>
      <td>Interface inputs and outputs are connected, defaulted, or intentionally unconnected under published rules.</td>
      <td><code>Language/</code>, <code>Expression/Interface.md</code></td>
    </tr>
    <tr>
      <td>Widget value participation</td>
      <td>Widget class, role, value type, direction, binding, and diagram participation are compatible.</td>
      <td><code>Expression/Widget.md</code>, <code>Language/</code></td>
    </tr>
    <tr>
      <td>Widget reference participation</td>
      <td>Object/reference access remains distinct from natural widget value flow and uses legal class-side members.</td>
      <td><code>Expression/Widget interaction.md</code>, <code>Language/</code></td>
    </tr>
    <tr>
      <td>Widget event participation</td>
      <td>Event identity, payload type, source widget, and execution participation are legal where events are supported.</td>
      <td><code>Language/</code>, widget class contract</td>
    </tr>
    <tr>
      <td>Structures</td>
      <td>Case, loop, region, boundary, selector, iteration, and tunnel semantics are explicit and valid.</td>
      <td><code>Language/Control structures.md</code></td>
    </tr>
    <tr>
      <td>State and cycles</td>
      <td>Feedback, delay, local memory, and recurrence are explicit and legal. No hidden runtime memory may legalize an invalid cycle.</td>
      <td><code>Language/State and cycles.md</code></td>
    </tr>
    <tr>
      <td>Constants</td>
      <td>Constant values are well typed, representable, and compatible with their consumers.</td>
      <td><code>Language/</code>, <code>Expression/Type.md</code></td>
    </tr>
    <tr>
      <td>Primitive calls</td>
      <td>Intrinsic primitive identity, ports, value constraints, and status behavior are legal.</td>
      <td><code>Libraries/</code></td>
    </tr>
    <tr>
      <td>Library calls</td>
      <td>Namespaced identity, provider requirements, capability requirements, effect model, status/error contract, and dependency requirements are explicit enough for derivation.</td>
      <td><code>Libraries/</code>, <code>Profiles/</code>, FIR library-call model</td>
    </tr>
    <tr>
      <td>Error/status flow</td>
      <td>Error or status behavior is represented according to published rules. It must not be hidden inside a runtime convention.</td>
      <td><code>Language/</code>, <code>Libraries/</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Explicit Rejection Rules</h2>

<p>
Semantic validation must reject, or explicitly report unsupported, instead of
repairing meaning silently.
</p>

<p>
The following are invalid for semantic acceptance:
</p>

<ul>
  <li>unknown executable node family with no published extension or profile rule,</li>
  <li>known node with incompatible port count, direction, or type use,</li>
  <li>edge endpoint that does not exist or is not legal for dataflow,</li>
  <li>type mismatch with no published conversion rule,</li>
  <li>structure boundary crossing without an explicit legal terminal or tunnel,</li>
  <li>cycle that requires hidden memory or scheduler repair,</li>
  <li>widget binding that references an unknown widget, member, value type, or incompatible role,</li>
  <li>attempt to use visual layout or SVG geometry as executable dependency,</li>
  <li>library call without a valid published identity or required capability model,</li>
  <li>profile feature used without the required profile declaration where profiles require explicit opt-in.</li>
</ul>

<p>
If a conforming implementation does not support a valid feature yet, it must
report unsupported scope explicitly. It must not misclassify the source as
invalid FROG merely because its own subset is smaller.
</p>

<hr/>

<h2>Validated Meaning Content</h2>

<p>
Accepted validated meaning must make the following information determinate for
the accepted program:
</p>

<ul>
  <li>public interface boundary and type commitments,</li>
  <li>accepted executable graph participation,</li>
  <li>accepted structure and region semantics,</li>
  <li>accepted explicit state and initialization semantics,</li>
  <li>accepted widget value, event, or reference participation,</li>
  <li>accepted primitive and library call identities,</li>
  <li>accepted dependency and value-flow structure,</li>
  <li>diagnostic attribution to accepted source contributors.</li>
</ul>

<p>
FIR derivation may normalize or materialize support objects later. It must not
still be deciding these semantic facts.
</p>

<hr/>

<h2>Error Reporting Posture</h2>

<p>
Semantic rejection should carry enough information for conformance and tooling:
</p>

<ul>
  <li>stage: <code>semantic_validation</code>,</li>
  <li>owner: language, library, profile, widget contract, or source-to-FIR boundary,</li>
  <li>diagnostic code or stable diagnostic family,</li>
  <li>source attribution where available,</li>
  <li>human-readable explanation,</li>
  <li>whether the result is invalid FROG or unsupported implementation subset.</li>
</ul>

<p>
This keeps invalidity distinct from implementation incompleteness.
</p>

<hr/>

<h2>Relation With T014 Library-Call Closure</h2>

<p>
The library-call rows in this document intentionally name the validation
requirements without closing every representation detail. The FIR library-call
model owns the exact public representation for provider identity, capability
requirements, effect model, status contract, dependency closure, and lowering
handoff facts.
</p>

<p>
Until that model is closed, implementations must keep <code>frog.xxx</code>
library-call derivation bounded to supported and explicitly recognized
families.
</p>

<hr/>

<h2>Summary</h2>

<p>
Semantic validation before FIR is the gate that turns structurally valid source
into accepted program meaning. It establishes types, nodes, bindings,
structures, state, library calls, widget participation, and errors explicitly
before FIR derivation. It rejects invalid meaning, reports unsupported subsets
honestly, and prevents downstream layers from becoming semantic guesswork.
</p>
