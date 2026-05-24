<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Math Library Conformance Plan</h1>

<p align="center">
  <strong>Public staged conformance plan for <code>frog.math</code>, the <code>frog.numeric</code> candidate, and future math domain namespaces</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document records the public conformance promotion posture for the math
library surface. It keeps the published scalar <code>frog.math.*</code> v0.1
catalog testable while preventing candidate or deferred math namespaces from
becoming accidental language law.
</p>

<p>
The plan is intentionally staged:
</p>

<pre><code>published scalar frog.math
   -> concrete accept/reject conformance cases

candidate frog.numeric and future frog.math.* domains
   -> planned conformance families until exact primitive contracts are promoted</code></pre>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../Libraries/Math.md"><code>Libraries/Math.md</code></a> owns the published scalar <code>frog.math.*</code> v0.1 primitive catalog and the staged candidate notes.</li>
  <li><a href="../Libraries/Core.md"><code>Libraries/Core.md</code></a> owns intrinsic core arithmetic and comparison primitives that must remain distinct from <code>frog.math.*</code>.</li>
  <li><a href="../Libraries/Signal.md"><code>Libraries/Signal.md</code></a>, <a href="../Libraries/Waveform.md"><code>Libraries/Waveform.md</code></a>, and <a href="../Libraries/Table.md"><code>Libraries/Table.md</code></a> own signal, waveform, and table semantics that generic math must not absorb.</li>
  <li><a href="../IR/Library call model.md"><code>IR/Library call model.md</code></a> owns FIR call identity, effect/status metadata, and abstract provider or capability requirements.</li>
  <li><a href="../IR/Lowering.md"><code>IR/Lowering.md</code></a> and <a href="../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> own downstream specialization, provider requirements, manifests, diagnostics, and consumer-facing rejection conditions.</li>
</ul>

<h2>Current Concrete Cases</h2>

<p>
The current concrete case files for T048 are:
</p>

<ul>
  <li><a href="./valid/compiler/07_scalar_math_call_preserves_value_library_identity.md"><code>valid/compiler/07_scalar_math_call_preserves_value_library_identity</code></a> - positive scalar <code>frog.math.*</code> value-call preservation across source, FIR, lowering, and backend contract boundaries.</li>
  <li><a href="./invalid/compiler/12_math_library_unknown_or_malformed_call_is_rejected.md"><code>invalid/compiler/12_math_library_unknown_or_malformed_call_is_rejected</code></a> - negative rejection of unknown, malformed, nonscalar, or premature candidate math calls.</li>
</ul>

<p>
These cases cover the published flat scalar surface only. They do not publish
<code>frog.numeric</code>, <code>frog.math.random</code>,
<code>frog.math.interpolate</code>, <code>frog.math.linalg</code>, or any other
future domain namespace as implemented v0.1 APIs.
</p>

<h2>Planned Case Families</h2>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Current posture</th>
      <th>Required future conformance evidence before broad coverage is claimed</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Scalar math recognition</td>
      <td>Concrete T048 cases added.</td>
      <td>Accept published flat <code>frog.math.*</code> names, reject unknown or malformed names, preserve pure value-call identity in FIR and lowering.</td>
    </tr>
    <tr>
      <td><code>frog.numeric</code> classification and conversions</td>
      <td>Candidate only.</td>
      <td>Promote exact primitive ids, port types, representation assumptions, conversion legality, numeric edge policy, positive classification cases, and negative invalid-conversion cases.</td>
    </tr>
    <tr>
      <td>Deterministic random-state posture</td>
      <td>Candidate only under <code>frog.math.random</code>.</td>
      <td>Promote explicit PRNG state value shape or explicit entropy/capability inputs, accept state-threaded generation, and reject hidden global RNG state or runtime-private seed tables.</td>
    </tr>
    <tr>
      <td>Interpolation preconditions</td>
      <td>Candidate only under <code>frog.math.interpolate</code>.</td>
      <td>Promote exact primitive ids, dimension rules, ordering rules, duplicate-sample behavior, out-of-range policy, status behavior, and negative precondition cases.</td>
    </tr>
    <tr>
      <td>Provider-backed linear algebra</td>
      <td>Deferred provider-aware surface.</td>
      <td>Promote matrix/vector value shapes, provider/status rules, manifest/provider requirements, positive provider-boundary cases, and negative missing-provider or hidden-BLAS/LAPACK-identity cases.</td>
    </tr>
  </tbody>
</table>

<h2>Boundary Rules</h2>

<ul>
  <li>Scalar <code>frog.math.*</code> calls remain standard-library value calls, not intrinsic <code>frog.core.*</code> operators.</li>
  <li>Candidate namespaces must not be accepted as published v0.1 primitive catalogs until exact public contracts are added.</li>
  <li>Signal, waveform, and table semantics remain owned by their existing libraries.</li>
  <li>Provider-heavy numerical domains must declare provider, status, manifest, and capability requirements explicitly.</li>
  <li>Conformance cases must not name Graiphic private runtime providers, binaries, loader mechanisms, services, local paths, or implementation internals.</li>
</ul>

<h2>Summary</h2>

<p>
The T048 conformance promotion makes published scalar <code>frog.math</code>
calls checkable now and keeps the broader mathematical roadmap visible without
turning future numeric, random, interpolation, linear-algebra, fitting,
optimization, statistics, or calculus work into hidden runtime law.
</p>
