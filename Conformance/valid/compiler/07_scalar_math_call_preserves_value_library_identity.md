<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">07 - Scalar Math Call Preserves Value Library Identity</h1>

<p align="center">
  <strong>Positive source/FIR/lowering case for a published scalar <code>frog.math</code> value call</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>valid/compiler/07_scalar_math_call_preserves_value_library_identity</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: FIR derivation and preservation</code></pre>

<p>
The case also depends on semantic validation, lowering, backend-contract
emission, and backend-family consumption where a selected consumer claims
support for the published scalar math subset.
</p>

<h2>Purpose</h2>

<p>
This case verifies that a published flat scalar <code>frog.math.*</code> call
remains an explicit pure standard-library value call across the public
execution corridor.
</p>

<pre><code>.frog source
   -> semantic validation
   -> FIR standard-library value call
   -> lowering without provider guessing
   -> backend contract or manifest-compatible handoff
   -> consumer execution under the declared numeric profile</code></pre>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../Libraries/Math.md"><code>Libraries/Math.md</code></a> owns the published <code>frog.math.*</code> scalar primitive ids, ports, scalar-only v0.1 scope, and profile-defined numeric edge behavior.</li>
  <li><a href="../../../Libraries/Core.md"><code>Libraries/Core.md</code></a> owns intrinsic core arithmetic and comparison primitives that must remain distinct from <code>frog.math.*</code>.</li>
  <li><a href="../../../IR/Library call model.md"><code>IR/Library call model.md</code></a> owns FIR call identity, call class, effect metadata, and provider/capability requirement posture.</li>
  <li><a href="../../../IR/Lowering.md"><code>IR/Lowering.md</code></a> owns target-oriented specialization without redefining public call identity.</li>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns consumer-facing assumptions, profiles, diagnostics, and rejection conditions.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The program uses a published scalar <code>frog.math</code> primitive such as
<code>frog.math.sqrt</code>, <code>frog.math.pow</code>,
<code>frog.math.sin</code>, <code>frog.math.atan2</code>,
<code>frog.math.round</code>, or <code>frog.math.clamp</code>.
</p>

<p>
The source names only the public primitive and its public ports. It does not
name a private helper, native math symbol, provider binary, backend intrinsic,
runtime hook, or host-specific implementation detail.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: lowerable when the selected profile claims the scalar math subset
Expected backend-contract result: emittable
Expected provider resolution: not required for the published scalar value-only subset unless the selected backend declares its own implementation dependency explicitly
Expected backend-family consumption: consumable by a consumer that claims the published scalar math subset and the active numeric profile</code></pre>

<h2>Expected FIR Features</h2>

<ul>
  <li>the FIR object preserves the exact public <code>frog.math.*</code> call identity,</li>
  <li>the call class remains <code>standard_library_value</code> or an equivalent explicit value-library class,</li>
  <li>the effect remains <code>pure</code>,</li>
  <li>all required input and output ports remain explicit and typed according to <code>Libraries/Math.md</code> and the active type rules,</li>
  <li>the scalar-only v0.1 boundary remains visible,</li>
  <li>profile-defined numeric edge policy remains a declared profile fact rather than hidden runtime behavior.</li>
</ul>

<h2>Expected Lowering and Contract Features</h2>

<ul>
  <li>lowering may specialize the call for a backend family, but it must preserve source attribution and the public call identity,</li>
  <li>the backend contract or manifest-compatible handoff must not claim that a private helper name, native symbol, or backend intrinsic is the FROG primitive identity,</li>
  <li>ordinary scalar math does not require a provider slot by default,</li>
  <li>if a selected backend declares an implementation dependency for its own scalar math realization, that dependency remains a backend-consumer fact rather than language law.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li><code>frog.math.*</code> scalar calls remain distinct from <code>frog.core.*</code> intrinsic operators,</li>
  <li>published flat scalar names remain distinct from future <code>frog.math.elementary</code> organization names,</li>
  <li>source-level call identity remains distinct from private runtime helper identity,</li>
  <li>profile-defined numeric edge behavior remains distinct from primitive identity.</li>
</ul>

<h2>Case Classification</h2>

<pre><code>Case classification:
- positive
- source/FIR/lowering
- frog.math
- scalar value library
- pure standard-library call
- implementation-independent</code></pre>

<h2>Summary</h2>

<p>
A conforming implementation that claims support for the published scalar
<code>frog.math</code> subset must accept, derive, lower, contract-emit, and
consume this case under its declared numeric profile without turning one
private runtime helper, backend intrinsic, or native math symbol into FROG
language law.
</p>
