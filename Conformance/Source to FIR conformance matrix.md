<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Source To FIR Conformance Matrix</h1>

<p align="center">
  <strong>Public accept / reject / preserve matrix for canonical <code>.frog</code> source through FIR derivation</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document defines the public conformance shape for the corridor:
</p>

<pre><code>canonical .frog source
   -&gt; structural validity
   -&gt; semantic validation
   -&gt; validated program meaning
   -&gt; canonical Execution IR / FIR</code></pre>

<p>
It does not replace the case corpus under <code>Conformance/valid/</code> and
<code>Conformance/invalid/</code>. It defines how future cases should be
classified and what each family must prove.
</p>

<hr/>

<h2>Outcome Vocabulary</h2>

<table>
  <thead>
    <tr>
      <th>Outcome</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>accept_source</code></td>
      <td>Source is loadable and structurally valid.</td>
    </tr>
    <tr>
      <td><code>reject_source</code></td>
      <td>Source is malformed or structurally invalid.</td>
    </tr>
    <tr>
      <td><code>accept_meaning</code></td>
      <td>Semantic validation succeeds and validated program meaning exists.</td>
    </tr>
    <tr>
      <td><code>reject_meaning</code></td>
      <td>Source is structurally valid but semantically invalid.</td>
    </tr>
    <tr>
      <td><code>derive_fir</code></td>
      <td>Validated meaning is eligible for canonical FIR derivation.</td>
    </tr>
    <tr>
      <td><code>reject_fir_preservation</code></td>
      <td>FIR was produced but lost or misrepresented required distinctions.</td>
    </tr>
    <tr>
      <td><code>unsupported_subset</code></td>
      <td>The source may be valid FROG, but the implementation subset does not support it yet.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Stage Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Stage</th>
      <th>Conformance must check</th>
      <th>Must not confuse with</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Loadability</td>
      <td>File can be parsed as the declared source serialization.</td>
      <td>Structural validity or semantic validity.</td>
    </tr>
    <tr>
      <td>Structural validity</td>
      <td>Required sections, section shape, stable ids, source-owned reference shape.</td>
      <td>Semantic acceptance.</td>
    </tr>
    <tr>
      <td>Semantic validation</td>
      <td>Types, nodes, edges, structures, widget participation, state, calls, status/error rules.</td>
      <td>FIR construction.</td>
    </tr>
    <tr>
      <td>FIR derivation</td>
      <td>Required execution-facing meaning is represented with attribution and preserved distinctions.</td>
      <td>Lowering or runtime execution.</td>
    </tr>
    <tr>
      <td>FIR structural validation</td>
      <td>Canonical JSON FIR satisfies the published IR schema surface where applicable.</td>
      <td>Semantic correctness by itself.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Required Case Families</h2>

<p>
The public corpus should contain accepted and rejected cases for each row once
the relevant specification surface is closed.
</p>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Positive expectation</th>
      <th>Negative expectation</th>
      <th>Preservation expectation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Required source sections</td>
      <td>Required <code>spec_version</code>, <code>metadata</code>, <code>interface</code>, and <code>diagram</code> accepted.</td>
      <td>Missing required section rejected at source structure stage.</td>
      <td>Source attribution remains recoverable.</td>
    </tr>
    <tr>
      <td>Interface boundary</td>
      <td>Typed inputs and outputs accepted and represented in validated meaning.</td>
      <td>Duplicate, unknown, or incompatible public ports rejected.</td>
      <td>FIR preserves public boundary identity and direction.</td>
    </tr>
    <tr>
      <td>Diagram primitive</td>
      <td>Known primitive with valid ports and types accepted.</td>
      <td>Unknown primitive, wrong arity, or incompatible types rejected.</td>
      <td>FIR preserves primitive identity, typed ports, and dependencies.</td>
    </tr>
    <tr>
      <td>Constants</td>
      <td>Typed constant accepted when representable and connected legally.</td>
      <td>Invalid literal or incompatible consumer rejected.</td>
      <td>FIR preserves typed value and attribution.</td>
    </tr>
    <tr>
      <td>Edges</td>
      <td>Directed legal endpoint connections accepted.</td>
      <td>Missing endpoint, wrong direction, or invalid boundary crossing rejected.</td>
      <td>FIR preserves directed value-flow dependency.</td>
    </tr>
    <tr>
      <td>Structures</td>
      <td>Explicit case, loop, and region constructs accepted under published rules.</td>
      <td>Implicit visual grouping or illegal terminal usage rejected.</td>
      <td>FIR preserves region and boundary distinctions.</td>
    </tr>
    <tr>
      <td>State and cycles</td>
      <td>Explicit delay or state carrier legalizes supported recurrence.</td>
      <td>Cycle requiring hidden runtime memory rejected.</td>
      <td>FIR preserves explicit state identity and initialization.</td>
    </tr>
    <tr>
      <td>Widget value</td>
      <td>Valid control or indicator value binding accepted.</td>
      <td>Unknown widget, incompatible role, or wrong value type rejected.</td>
      <td>FIR preserves widget identity, value type, direction, and binding source.</td>
    </tr>
    <tr>
      <td>Widget reference</td>
      <td>Valid object/reference access accepted where class contract permits it.</td>
      <td>Illegal member, part, method, or access mode rejected.</td>
      <td>FIR preserves reference access as distinct from widget natural value flow.</td>
    </tr>
    <tr>
      <td>Widget event</td>
      <td>Valid event payload participation accepted where event model is supported.</td>
      <td>Unknown event, wrong payload type, or missing source widget rejected.</td>
      <td>FIR preserves event identity, payload type, and widget attribution.</td>
    </tr>
    <tr>
      <td>Front-panel visuals</td>
      <td>Layout, label, style, asset references accepted as source where structurally valid.</td>
      <td>Visual data used as hidden execution dependency rejected at semantic stage.</td>
      <td>FIR excludes visual geometry from primary execution meaning while preserving useful downstream references where specified.</td>
    </tr>
    <tr>
      <td><code>.wfrog</code> and Default assets</td>
      <td>Valid package references accepted for widget validation and realization context.</td>
      <td>Missing required package or unsupported class surface reported explicitly.</td>
      <td>FIR does not absorb SVG or package internals as execution semantics.</td>
    </tr>
    <tr>
      <td>Library calls</td>
      <td>Supported <code>frog.xxx</code> call accepted with explicit identity, ports, status, and dependency requirements.</td>
      <td>Unknown library call, invalid provider, or incompatible effect/status contract rejected or reported unsupported.</td>
      <td>FIR preserves namespaced call identity and dependency facts according to the FIR library-call model.</td>
    </tr>
    <tr>
      <td>Unsupported valid source</td>
      <td>Implementation reports unsupported subset when source is valid but not implemented.</td>
      <td>Implementation must not call valid FROG invalid merely because the implementation is bounded.</td>
      <td>No guessed FIR may be emitted for unsupported meaning.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Minimum Case Record Shape</h2>

<p>
A source-to-FIR conformance case should state:
</p>

<ul>
  <li>case id and title,</li>
  <li>owner stage: source, semantic validation, FIR derivation, or FIR schema,</li>
  <li>expected loadability,</li>
  <li>expected structural validity,</li>
  <li>expected semantic result,</li>
  <li>expected FIR eligibility,</li>
  <li>required preservation facts,</li>
  <li>expected rejection reason or unsupported reason where applicable,</li>
  <li>source artifacts and expected derived artifacts where the case is positive.</li>
</ul>

<p>
The case should be small enough that a failed implementation result has one
clear owner.
</p>

<hr/>

<h2>Promotion Rule</h2>

<p>
A feature family should not be described as systematically covered until the
following are present:
</p>

<ul>
  <li>source-scope rule,</li>
  <li>semantic validation rule,</li>
  <li>positive conformance case,</li>
  <li>negative conformance case,</li>
  <li>FIR preservation expectation,</li>
  <li>bounded implementation result or explicit unsupported-subset posture.</li>
</ul>

<p>
Examples may demonstrate a feature earlier, but examples alone do not close
the conformance surface.
</p>

<hr/>

<h2>Open Items</h2>

<p>
The following families need additional cases before broad systematic
source-to-FIR coverage can be claimed:
</p>

<ul>
  <li>full section-level source schemas for top-level <code>.frog</code> sections,</li>
  <li>complete semantic rejection cases for type mismatches and invalid conversions,</li>
  <li>structure and loop tunnel conformance,</li>
  <li>explicit state and invalid cycle conformance,</li>
  <li>widget value/reference/event conformance beyond the initial examples,</li>
  <li>library-call conformance after the FIR library-call model is closed,</li>
  <li>error/status corridor conformance,</li>
  <li>unsupported-subset reporting conformance.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
Source-to-FIR conformance makes the public corridor testable. It separates
source acceptance, semantic acceptance, FIR eligibility, FIR preservation, and
unsupported implementation subsets. It protects the core FROG rule that FIR is
derived from validated meaning, not from runtime-private guesses or visual
layout.
</p>
