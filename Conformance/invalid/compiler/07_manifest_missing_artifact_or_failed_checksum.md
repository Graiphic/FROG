<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">07 - Manifest Missing Artifact or Failed Checksum</h1>

<p align="center">
  <strong>Negative source/FIR/lowering case for manifest-compatible handoff rejection before runtime consumption</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/07_manifest_missing_artifact_or_failed_checksum</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: manifest / provider resolution</code></pre>

<h2>Purpose</h2>

<p>
This case verifies that a manifest-compatible backend handoff must be rejected when a required artifact is missing, package-relative artifact reference is invalid, or declared checksum does not match the artifact content.
</p>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns manifest-compatible handoff content, artifact references, dependency closure, and rejection conditions.</li>
  <li><a href="../../../IR/Lowering.md"><code>IR/Lowering.md</code></a> owns dependency-closure planning before the handoff.</li>
  <li><a href="../../../Conformance/Source FIR Lowering conformance matrix.md"><code>Conformance/Source FIR Lowering conformance matrix.md</code></a> owns the staged conformance vocabulary used by this case.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The upstream program has already passed source validation, semantic validation, FIR derivation, lowering, and backend-contract emission for the selected corridor.
</p>

<p>
The invalidity is in the manifest-compatible handoff: a declared artifact is absent, an artifact reference cannot be resolved inside the package or deployment closure, or the artifact checksum fails validation.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: lowerable
Expected backend-contract result: emittable
Expected provider resolution: rejected
Expected backend-family consumption: not reached</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: manifest / provider resolution</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the handoff declares an artifact or dependency closure,
but the referenced artifact is missing,
the package-relative reference is unresolved,
or the declared checksum does not match the artifact bytes</code></pre>

<h2>Must Not Happen</h2>

<ul>
  <li>the launcher or runtime must not search outside the declared package closure to guess the missing artifact,</li>
  <li>the checksum mismatch must not be ignored as an optimization detail,</li>
  <li>provider resolution failure must not be reported as semantic invalidity,</li>
  <li>the consumer must not execute an undeclared substitute artifact.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>backend contract emission remains distinct from manifest/provider resolution,</li>
  <li>manifest resolution remains distinct from runtime-private loading strategy,</li>
  <li>artifact identity remains distinct from source or FIR identity.</li>
</ul>

<h2>Summary</h2>

<p>
A conforming consumer must reject this case at manifest/provider resolution. A contract or manifest-compatible handoff is useful only when declared artifacts and dependency closure can be validated without guessing.
</p>
