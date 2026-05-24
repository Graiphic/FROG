<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Conformance — Invalid Compiler Corridor Cases</h1>

<p align="center">
  <strong>Negative compiler-corridor conformance cases for published FROG execution paths</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-subdirectory-exists">2. Why This Subdirectory Exists</a></li>
  <li><a href="#position-in-the-conformance-corridor">3. Position in the Conformance Corridor</a></li>
  <li><a href="#what-invalid-compiler-corridor-means">4. What Invalid Compiler-Corridor Means</a></li>
  <li><a href="#four-primary-failure-classes">5. Four Primary Failure Classes</a></li>
  <li><a href="#published-case-order">6. Published Case Order</a></li>
  <li><a href="#what-these-cases-test">7. What These Cases Test</a></li>
  <li><a href="#what-these-cases-do-not-claim">8. What These Cases Do Not Claim</a></li>
  <li><a href="#relation-with-native_cpu_llvm">9. Relation with <code>native_cpu_llvm</code></a></li>
  <li><a href="#expected-case-structure">10. Expected Case Structure</a></li>
  <li><a href="#expected-outcome-classes">11. Expected Outcome Classes</a></li>
  <li><a href="#recommended-case-growth-order">12. Recommended Case Growth Order</a></li>
  <li><a href="#relation-with-valid-compiler-corridor-cases">13. Relation with Valid Compiler-Corridor Cases</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This subdirectory defines negative conformance cases for the compiler corridor of published FROG.
</p>

<p>
These cases are not generic invalid-language cases.
They are public rejection anchors for situations where a program or derived artifact may still be valid at one upstream stage, but must be rejected at a later compiler-corridor stage.
</p>

<p>
The relevant corridor is:
</p>

<pre><code>.frog
   -&gt;
structural validity
   -&gt;
semantic acceptance
   -&gt;
canonical Execution IR derivation
   -&gt;
canonical JSON IR validation where applicable
   -&gt;
lowering eligibility
   -&gt;
backend-contract eligibility
   -&gt;
declared backend-family consumability</code></pre>

<p>
This directory exists so that failures in the compiler corridor become explicit public truth rather than implicit implementation drift.
</p>

<hr/>

<h2 id="why-this-subdirectory-exists">2. Why This Subdirectory Exists</h2>

<p>
Once the repository recognizes a serious compiler-oriented corridor, it is no longer enough to publish only:
</p>

<ul>
  <li>ordinary source invalidity,</li>
  <li>ordinary semantic invalidity,</li>
  <li>ordinary IR-preservation invalidity.</li>
</ul>

<p>
It also becomes necessary to publish failures that occur later in the corridor.
</p>

<p>
This subdirectory therefore exists to expose cases where the repository can say:
</p>

<pre><code>this program slice
is not acceptable
for the declared compiler corridor stage

even if
some upstream stage was still accepted</code></pre>

<hr/>

<h2 id="position-in-the-conformance-corridor">3. Position in the Conformance Corridor</h2>

<p>
These cases sit downstream from ordinary source and semantic validity and make later rejection boundaries public.
</p>

<p>
They belong to the following staged reading:
</p>

<pre><code>language-valid
   -&gt;
IR-derivable
   -&gt;
IR-schema-valid where applicable
   -&gt;
lowerable
   -&gt;
backend-contract-emittable
   -&gt;
backend-family-consumable</code></pre>

<p>
A compiler-corridor invalid case makes explicit where this staged progression must stop.
</p>

<hr/>

<h2 id="what-invalid-compiler-corridor-means">4. What Invalid Compiler-Corridor Means</h2>

<p>
An invalid compiler-corridor case is a published case that must be rejected at a declared downstream stage of the compiler corridor.
</p>

<p>
This means that the repository explicitly distinguishes:
</p>

<ul>
  <li>language invalidity,</li>
  <li>IR invalidity,</li>
  <li>lowering invalidity,</li>
  <li>backend-contract invalidity,</li>
  <li>backend-family consumer invalidity.</li>
</ul>

<p>
These are not interchangeable.
</p>

<hr/>

<h2 id="four-primary-failure-classes">5. Four Primary Failure Classes</h2>

<h3>5.1 Language-valid but profile-rejected</h3>
<p>
The program is structurally valid and semantically accepted by FROG, but outside the declared compilation profile surface.
</p>

<h3>5.2 IR-derivable but not lowerable</h3>
<p>
The program is language-valid and derivable to canonical FROG Execution IR, but not lowerable under the declared corridor without semantic loss or illicit invention.
</p>

<h3>5.3 Lowerable but not backend-contract-emittable</h3>
<p>
The program or lowered form is language-valid, IR-derivable, and lowerable, but still cannot be emitted as a consumer-safe backend contract without ambiguity or hidden assumptions.
</p>

<h3>5.4 Contract-emittable but consumer-rejected</h3>
<p>
The program and lowered form are language-valid, IR-derivable, lowerable, and backend-contract-emittable, but rejected by the declared backend-family consumer because that consumer cannot honor the declared assumptions.
</p>

<h3>5.5 Provider-backed call missing provider requirement</h3>
<p>
The program and FIR call may be valid, but the lowering or backend contract
omits the abstract provider, capability, or dependency requirement required by
the public call model.
</p>

<h3>5.6 ABI, profile, artifact, or manifest mismatch</h3>
<p>
The downstream handoff may exist, but the declared ABI profile, target profile,
artifact reference, checksum, provider slot, or dependency closure cannot be
validated by the selected consumer.
</p>

<h3>5.7 Backend-family identity collapse</h3>
<p>
A downstream contract or profile assertion is invalid if it treats one backend
family, such as an LLVM-oriented route, as the public identity of FROG, FIR, or
runtime conformance.
</p>

<h3>5.8 Valid FIR outside selected backend subset</h3>
<p>
The source and FIR may be valid FROG, but the selected backend profile or
implementation subset may still reject explicitly as <code>unsupported_subset</code>
without redefining the language.
</p>

<h3>5.9 Invalid <code>frog.system</code> call shape</h3>
<p>
The source may be loadable, but an unknown <code>frog.system</code> primitive,
missing required port, wrong port type, empty environment name, or malformed
capability id must be rejected before FIR derivation.
</p>

<h3>5.10 Unsatisfied system host capability</h3>
<p>
The source and FIR may be valid, but the selected host, profile, provider, or
deployment posture may still reject or report unsupported, denied, blocked, or
unavailable <code>frog.system</code> capability reads explicitly.
</p>

<h3>5.11 Invalid or premature math-library call</h3>
<p>
The source may be loadable, but an unknown <code>frog.math</code> primitive,
missing required scalar math port, nonscalar lift, or premature use of
candidate/deferred math namespaces such as <code>frog.numeric</code>,
<code>frog.math.random</code>, <code>frog.math.interpolate</code>, or
<code>frog.math.linalg</code> must be rejected before FIR derivation unless a
future public contract promotes that namespace explicitly.
</p>

<hr/>

<h2 id="published-case-order">6. Published Case Order</h2>

<p>
The canonical file order for this directory SHOULD be:
</p>

<pre><code>01_language_valid_but_profile_rejected.md
02_ir_derivable_but_not_lowerable.md
03_lowerable_but_not_backend_contract_emittable.md
04_contract_emittable_but_consumer_rejected.md
05_provider_backed_call_missing_provider_requirement.md
06_backend_contract_abi_profile_mismatch.md
07_manifest_missing_artifact_or_failed_checksum.md
08_llvm_must_not_be_treated_as_frog_runtime_identity.md
09_valid_fir_rejected_by_backend_profile_unsupported_subset.md
10_system_library_unknown_or_malformed_call_is_rejected.md
11_system_library_capability_or_subset_must_report_explicitly.md
12_math_library_unknown_or_malformed_call_is_rejected.md</code></pre>

<p>
This order mirrors the downstream corridor in the cleanest way:
</p>

<pre><code>profile gate
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
backend-family consumer</code></pre>

<hr/>

<h2 id="what-these-cases-test">7. What These Cases Test</h2>

<p>
These cases test the negative edges of the compiler corridor.
</p>

<ul>
  <li>that a valid FROG program is correctly rejected by a bounded profile claim,</li>
  <li>that explicit state or structure is not silently compiled through an unfaithful lowering path,</li>
  <li>that a backend contract is rejected rather than emitted with hidden assumptions,</li>
  <li>that a backend-family consumer rejects unsupported contract content rather than silently reinterpreting it.</li>
  <li>that provider-backed calls do not lose provider/capability requirements before contract emission,</li>
  <li>that manifest-compatible handoffs reject missing artifacts and failed checksums,</li>
  <li>that LLVM-oriented routes remain downstream backend-family routes rather than FROG identity.</li>
  <li>that invalid <code>frog.system</code> call identity, ports, or inputs are rejected before FIR derivation,</li>
  <li>that unsatisfied <code>frog.system</code> host capabilities are rejected or reported explicitly rather than guessed.</li>
  <li>that invalid or premature math-library calls are rejected before private runtime helpers, backend intrinsics, hidden random state, or provider guesses can redefine the public surface.</li>
</ul>

<hr/>

<h2 id="what-these-cases-do-not-claim">8. What These Cases Do Not Claim</h2>

<p>
These cases do not claim:
</p>

<ul>
  <li>that upstream validity disappears when a downstream stage rejects,</li>
  <li>that every rejection is a language error,</li>
  <li>that every backend-family consumer must reject for the same reason,</li>
  <li>that LLVM-oriented rejection becomes language truth,</li>
  <li>that profile rejection means semantic invalidity.</li>
</ul>

<hr/>

<h2 id="relation-with-native_cpu_llvm">9. Relation with <code>native_cpu_llvm</code></h2>

<p>
For v0.1, the first intended downstream compilation corridor is the optional profile:
</p>

<pre><code>native_cpu_llvm</code></pre>

<p>
Accordingly, the first invalid compiler-corridor cases SHOULD preferentially target rejection boundaries relevant to that profile.
</p>

<hr/>

<h2 id="expected-case-structure">10. Expected Case Structure</h2>

<p>
Each invalid compiler-corridor case SHOULD state, as applicable:
</p>

<ul>
  <li>the source scope,</li>
  <li>the semantic scope,</li>
  <li>the canonical IR expectation,</li>
  <li>the canonical JSON IR expectation where relevant,</li>
  <li>the lowering expectation,</li>
  <li>the backend-contract expectation,</li>
  <li>the declared profile scope,</li>
  <li>the declared backend-family scope,</li>
  <li>the exact rejection stage,</li>
  <li>the exact rejection reason.</li>
</ul>

<hr/>

<h2 id="expected-outcome-classes">11. Expected Outcome Classes</h2>

<p>
Negative compiler-corridor cases will typically use outcome declarations such as:
</p>

<ul>
  <li><code>Expected structural validity: valid</code></li>
  <li><code>Expected semantic acceptance: accepted</code></li>
  <li><code>Expected IR derivation: derivable</code></li>
  <li><code>Expected lowering result: rejected</code> or <code>lowerable</code></li>
  <li><code>Expected backend-contract result: not emittable</code> or <code>emittable</code></li>
  <li><code>Expected backend-family consumption: rejected</code> or <code>not applicable</code></li>
</ul>

<p>
The key discipline is that the rejection stage must be explicit.
</p>

<hr/>

<h2 id="recommended-case-growth-order">12. Recommended Case Growth Order</h2>

<p>
Growth in this directory SHOULD remain disciplined and mirrored against the positive compiler corridor.
</p>

<p>
The recommended order is:
</p>

<pre><code>profile rejection
   -&gt;
lowering rejection
   -&gt;
backend-contract rejection
   -&gt;
backend-family consumer rejection
   -&gt;
provider/manifest resolution rejection
   -&gt;
backend-family identity-collapse rejection
   -&gt;
unsupported-subset reporting
   -&gt;
bounded host-capability rejection and status reporting</code></pre>

<p>
Published scalar math rejection sits with semantic primitive-contract
rejection. Future provider-heavy math-domain rejection should be staged later
only after exact public primitive contracts, status surfaces, and provider
requirements are promoted.
</p>

<hr/>

<h2 id="relation-with-valid-compiler-corridor-cases">13. Relation with Valid Compiler-Corridor Cases</h2>

<p>
This directory must be read together with:
</p>

<ul>
  <li><code>Conformance/valid/compiler/</code></li>
</ul>

<p>
The positive side says:
</p>

<pre><code>this corridor stage is accepted</code></pre>

<p>
The negative side says:
</p>

<pre><code>this corridor stage must reject here
and for this reason</code></pre>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
This subdirectory defines negative conformance cases for the published FROG compiler corridor.
</p>

<p>
The canonical v0.1 rejection order is:
</p>

<pre><code>01_language_valid_but_profile_rejected
02_ir_derivable_but_not_lowerable
03_lowerable_but_not_backend_contract_emittable
04_contract_emittable_but_consumer_rejected
05_provider_backed_call_missing_provider_requirement
06_backend_contract_abi_profile_mismatch
07_manifest_missing_artifact_or_failed_checksum
08_llvm_must_not_be_treated_as_frog_runtime_identity
09_valid_fir_rejected_by_backend_profile_unsupported_subset
10_system_library_unknown_or_malformed_call_is_rejected
11_system_library_capability_or_subset_must_report_explicitly
12_math_library_unknown_or_malformed_call_is_rejected</code></pre>

<p>
This keeps the compiler corridor explicit not only on the success path, but also on the rejection path.
</p>
