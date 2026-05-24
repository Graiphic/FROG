<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">12 - Math Library Unknown or Malformed Call Is Rejected</h1>

<p align="center">
  <strong>Negative source/FIR/lowering case for invalid or premature <code>frog.math</code> and candidate math calls</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/12_math_library_unknown_or_malformed_call_is_rejected</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: semantic validation</code></pre>

<h2>Purpose</h2>

<p>
This case verifies that invalid math-library source usage is rejected or
reported at the correct stage rather than repaired by runtime convention,
backend intrinsics, or private provider guesses.
</p>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../Libraries/Math.md"><code>Libraries/Math.md</code></a> owns the published scalar <code>frog.math.*</code> primitive ids, required ports, scalar-only v0.1 scope, and candidate/deferred namespace boundaries.</li>
  <li><a href="../../../Libraries/Core.md"><code>Libraries/Core.md</code></a> owns core arithmetic and comparison primitives that must not be reclassified as malformed <code>frog.math</code> calls.</li>
  <li><a href="../../../IR/Library call model.md"><code>IR/Library call model.md</code></a> owns the rule that unknown or unsupported call identity remains explicit and must not be guessed.</li>
  <li><a href="../../../Conformance/Source FIR Lowering conformance matrix.md"><code>Conformance/Source FIR Lowering conformance matrix.md</code></a> owns the staged rejection vocabulary.</li>
</ul>

<h2>Invalid Scenarios</h2>

<ul>
  <li>the source references an unknown scalar primitive such as <code>frog.math.magic_norm</code>,</li>
  <li>a required input port is missing, such as <code>exponent</code> for <code>frog.math.pow</code> or <code>max_value</code> for <code>frog.math.clamp</code>,</li>
  <li>a required input or output port has the wrong direction, role, or type,</li>
  <li>the source attempts array, matrix, tensor, cluster-wide, or broadcast lifting for a v0.1 scalar-only primitive,</li>
  <li>the source treats the candidate <code>frog.numeric</code> namespace as though it already published exact primitive ids,</li>
  <li>the source uses deferred namespaces such as <code>frog.math.random</code>, <code>frog.math.interpolate</code>, <code>frog.math.linalg</code>, <code>frog.math.fit</code>, <code>frog.math.optimize</code>, or <code>frog.math.calculus</code> as implemented v0.1 APIs,</li>
  <li>a provider-heavy numerical call omits the provider, status, manifest, or capability requirements that would be required after future promotion.</li>
</ul>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable when the file shape itself is well formed
Expected structural validity: valid when only the primitive contract is wrong
Expected semantic acceptance: rejected
Expected IR derivation: not derivable for semantic primitive-contract failures
Expected IR schema result: not applicable
Expected lowering result: not applicable
Expected backend-contract result: not applicable
Expected provider resolution: not applicable
Expected backend-family consumption: not applicable</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: semantic validation</code></pre>

<p>
If a future implementation subset recognizes a promoted math domain as valid
FROG but does not support it for a selected backend, the rejection may instead
be reported later as <code>unsupported_subset</code>. That later posture requires
the relevant namespace to be publicly promoted first.
</p>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the source references an unknown or invalid math primitive,
omits a required public port,
uses an incompatible port type or direction,
violates the published scalar-only v0.1 boundary,
or treats a candidate/deferred math namespace as an implemented API</code></pre>

<h2>Must Not Happen</h2>

<ul>
  <li>the implementation must not invent a private math primitive to satisfy the call,</li>
  <li>the implementation must not silently substitute a different public primitive,</li>
  <li>the implementation must not repair missing ports from runtime defaults,</li>
  <li>the implementation must not reinterpret arrays, matrices, tensors, or clusters through hidden broadcast semantics,</li>
  <li>the implementation must not accept hidden global random-number state as a valid public <code>frog.math.random</code> contract,</li>
  <li>the implementation must not treat BLAS, LAPACK, solver, statistics, or fitting provider choices as FROG language law.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>semantic rejection remains distinct from runtime execution failure,</li>
  <li>published scalar <code>frog.math.*</code> identity remains distinct from candidate or deferred math namespaces,</li>
  <li><code>frog.math.*</code> remains distinct from <code>frog.core.*</code> intrinsic operators,</li>
  <li>invalid primitive usage remains distinct from a valid call rejected later by a selected backend subset.</li>
</ul>

<h2>Summary</h2>

<p>
Invalid or premature math-library source usage must stop at the correct
conformance stage. Runtime support cannot retroactively make an unknown
primitive, malformed port set, nonscalar lift, hidden random state, or
provider-heavy candidate call valid FROG.
</p>
