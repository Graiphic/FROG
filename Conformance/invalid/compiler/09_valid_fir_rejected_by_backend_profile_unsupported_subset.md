<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">09 - Valid FIR Rejected by Backend Profile Unsupported Subset</h1>

<p align="center">
  <strong>Negative source/FIR/lowering case where valid FIR is outside a selected backend profile subset</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/09_valid_fir_rejected_by_backend_profile_unsupported_subset</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: implementation / profile subset</code></pre>

<h2>Purpose</h2>

<p>
This case verifies that a selected backend profile may reject valid FROG FIR as an unsupported subset without pretending that the source, semantic meaning, or canonical FIR is invalid.
</p>

<pre><code>valid FROG
   +
valid FIR
   +
unsupported selected backend subset
   =
explicit unsupported_subset or profile rejection</code></pre>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../Language/Semantic validation before FIR.md"><code>Language/Semantic validation before FIR.md</code></a> owns the distinction between invalid FROG and unsupported implementation subset.</li>
  <li><a href="../../../IR/Execution IR.md"><code>IR/Execution IR.md</code></a> owns canonical FIR validity.</li>
  <li><a href="../../../Profiles/Native CPU LLVM.md"><code>Profiles/Native CPU LLVM.md</code></a> owns the selected profile subset and explicit rejection posture.</li>
  <li><a href="../../../Conformance/Source FIR Lowering conformance matrix.md"><code>Conformance/Source FIR Lowering conformance matrix.md</code></a> owns the <code>unsupported_subset</code> outcome vocabulary.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The source program is structurally valid, semantically accepted, and derivable to canonical FIR. The selected backend profile or implementation subset does not yet support one valid FIR family, such as a valid host-capability call, a valid UI service boundary, a valid provider-backed call, or another published construct outside the selected profile's accepted subset.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: rejected for the selected profile or reported unsupported_subset
Expected backend-contract result: not applicable
Expected provider resolution: not applicable
Expected backend-family consumption: not applicable</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: selected backend profile subset gate</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the program and FIR are valid,
but the selected backend profile or implementation subset does not support
the required FIR family without semantic loss or hidden runtime assumptions</code></pre>

<h2>Must Not Happen</h2>

<ul>
  <li>the source must not be reported as structurally invalid,</li>
  <li>the semantic meaning must not be reported as invalid FROG,</li>
  <li>valid FIR must not be rewritten into a guessed supported shape,</li>
  <li>profile rejection must not be used to narrow the language definition.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>semantic acceptance remains distinct from backend-profile support,</li>
  <li>FIR validity remains distinct from selected-profile lowerability,</li>
  <li><code>unsupported_subset</code> remains distinct from <code>reject_meaning</code>,</li>
  <li>implementation incompleteness remains distinct from public language law.</li>
</ul>

<h2>Summary</h2>

<p>
A conforming implementation must reject or report unsupported subset at the selected profile gate. The rejection is downstream and must not erase the upstream fact that the source, semantic meaning, and canonical FIR are valid.
</p>
