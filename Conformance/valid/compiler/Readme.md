<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Conformance — Valid Compiler Corridor Cases</h1>

<p align="center">
  <strong>Positive compiler-corridor conformance cases for published FROG execution paths</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-subdirectory-exists">2. Why This Subdirectory Exists</a></li>
  <li><a href="#position-in-the-conformance-corridor">3. Position in the Conformance Corridor</a></li>
  <li><a href="#what-valid-compiler-corridor-means">4. What Valid Compiler-Corridor Means</a></li>
  <li><a href="#published-case-order">5. Published Case Order</a></li>
  <li><a href="#what-these-cases-test">6. What These Cases Test</a></li>
  <li><a href="#what-these-cases-do-not-claim">7. What These Cases Do Not Claim</a></li>
  <li><a href="#relation-with-native_cpu_llvm">8. Relation with <code>native_cpu_llvm</code></a></li>
  <li><a href="#expected-case-structure">9. Expected Case Structure</a></li>
  <li><a href="#expected-outcome-classes">10. Expected Outcome Classes</a></li>
  <li><a href="#recommended-case-growth-order">11. Recommended Case Growth Order</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This subdirectory defines positive conformance cases for the compiler corridor of published FROG.
</p>

<p>
These cases are not generic execution examples.
They are public positive anchors for the staged path:
</p>

<pre><code>.frog
   -&gt;
structural validity
   -&gt;
semantic acceptance
   -&gt;
canonical Execution IR derivation
   -&gt;
canonical JSON IR validity where applicable
   -&gt;
lowering eligibility
   -&gt;
backend-contract eligibility
   -&gt;
declared backend-family consumability where the case declares such a scope</code></pre>

<p>
This directory exists to make the compiler corridor publicly testable rather than merely descriptive.
</p>

<hr/>

<h2 id="why-this-subdirectory-exists">2. Why This Subdirectory Exists</h2>

<p>
The main conformance surface of FROG already distinguishes:
</p>

<ul>
  <li>loadability,</li>
  <li>structural validity,</li>
  <li>semantic acceptance,</li>
  <li>canonical Execution IR derivation,</li>
  <li>canonical JSON IR validation where applicable,</li>
  <li>later lowering and backend-facing handoff where applicable,</li>
  <li>declared backend-family consumption where applicable.</li>
</ul>

<p>
A serious compilation corridor requires that these downstream-sensitive stages become publicly testable as well.
</p>

<p>
This subdirectory therefore exists to expose positive cases where the repository can say:
</p>

<pre><code>this published program slice
is not only language-valid

it is also
IR-derivable,
lowering-eligible,
backend-contract-eligible,
and suitable for a declared downstream compiler-family corridor</code></pre>

<hr/>

<h2 id="position-in-the-conformance-corridor">3. Position in the Conformance Corridor</h2>

<p>
These cases sit downstream from ordinary semantic acceptance and upstream from any private backend realization.
</p>

<p>
They therefore belong to the following public corridor:
</p>

<pre><code>source acceptance
   -&gt;
semantic acceptance
   -&gt;
IR preservation
   -&gt;
canonical JSON IR validity where applicable
   -&gt;
lowering eligibility
   -&gt;
backend-contract eligibility
   -&gt;
declared backend-family consumability</code></pre>

<p>
They do not replace the rest of the conformance corpus.
They extend it along the compiler-oriented corridor.
</p>

<hr/>

<h2 id="what-valid-compiler-corridor-means">4. What Valid Compiler-Corridor Means</h2>

<p>
A valid compiler-corridor case is a published case that must satisfy all declared upstream and downstream conditions for its stated scope.
</p>

<p>
In general, that means:
</p>

<ul>
  <li>the source is loadable where applicable,</li>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the program is semantically accepted,</li>
  <li>the program derives to canonical FROG Execution IR without loss of required distinctions,</li>
  <li>the resulting canonical JSON IR is valid where a canonical JSON payload is part of the case,</li>
  <li>the case is eligible for lowering,</li>
  <li>the case is eligible for backend-contract emission,</li>
  <li>the case is consumable by the declared backend-family corridor when such a corridor is part of the case declaration.</li>
</ul>

<p>
These cases are therefore stronger than ordinary language-valid cases.
They explicitly test corridor closure beyond semantic acceptance.
</p>

<hr/>

<h2 id="published-case-order">5. Published Case Order</h2>

<p>
The canonical file order for this directory SHOULD be:
</p>

<pre><code>01_pure_arithmetic_is_consumable.md
02_structured_control_is_consumable.md
03_explicit_state_is_consumable.md
04_provider_backed_standard_library_call_declares_provider_requirement.md
05_widget_value_and_reference_are_preserved_across_lowering.md
06_system_library_call_preserves_capability_and_local_status.md
07_scalar_math_call_preserves_value_library_identity.md</code></pre>

<p>
This order reflects a deliberate corridor-closing strategy:
</p>

<pre><code>pure computation
   -&gt;
structured control
   -&gt;
explicit state</code></pre>

<p>
The order is intentionally conservative.
It closes one serious native compiled core before widening the corridor to more effect-heavy or runtime-heavy surfaces.
</p>

<p>
The fourth, fifth, sixth, and seventh cases extend the corridor from the first
pure/control/state core into explicit provider-backed dependency closure,
explicit UI/widget category preservation, bounded <code>frog.system</code>
host-capability reads, and published scalar <code>frog.math</code> value-call
preservation. They remain bounded case records; they do not claim that all
provider, UI, host-capability, math-domain, or runtime-service-heavy programs
are accepted by every backend family.
</p>

<hr/>

<h2 id="what-these-cases-test">6. What These Cases Test</h2>

<p>
A valid compiler-corridor case may test one or more of the following:
</p>

<ul>
  <li>that a semantically accepted program slice remains lowering-eligible,</li>
  <li>that explicit state remains lowerable as explicit state,</li>
  <li>that structured control remains lowerable without semantic collapse,</li>
  <li>that the canonical open IR preserves enough recoverability for compiler-corridor use,</li>
  <li>that a backend contract can be emitted without hidden assumptions,</li>
  <li>that a declared backend-family consumer may consume the case without semantic reinvention.</li>
</ul>

<p>
These cases therefore test positive corridor closure, not merely source validity.
</p>

<hr/>

<h2 id="what-these-cases-do-not-claim">7. What These Cases Do Not Claim</h2>

<p>
These cases do not claim:
</p>

<ul>
  <li>that every valid FROG program is automatically compiler-corridor-valid,</li>
  <li>that every semantically accepted program is automatically lowerable for every backend family,</li>
  <li>that every compiler-family corridor is identical,</li>
  <li>that one backend family defines FROG,</li>
  <li>that passing a valid compiler-corridor case replaces the rest of conformance.</li>
</ul>

<p>
A valid compiler-corridor case is always bounded by its declared scope.
</p>

<hr/>

<h2 id="relation-with-native_cpu_llvm">8. Relation with <code>native_cpu_llvm</code></h2>

<p>
For v0.1, the first intended downstream compilation corridor is the optional profile:
</p>

<pre><code>native_cpu_llvm</code></pre>

<p>
Accordingly, the first three compiler-corridor cases in this directory target
the conservative accepted subset of that profile.
</p>

<p>
The published positive native-core anchors currently correspond to:
</p>

<ul>
  <li><code>01_pure_arithmetic_is_consumable</code></li>
  <li><code>02_structured_control_is_consumable</code></li>
  <li><code>03_explicit_state_is_consumable</code></li>
</ul>

<p>
Together they express the first bounded claim that:
</p>

<ul>
  <li>pure typed computation,</li>
  <li>bounded structured control,</li>
  <li>explicit local memory</li>
</ul>

<p>
can travel through the declared Native CPU LLVM corridor when all staged obligations are satisfied.
</p>

<p>
Additional positive source/FIR/lowering anchors now cover provider-backed
dependency closure and UI/widget category preservation under their own declared
provider-capable or host-service-capable scopes:
</p>

<ul>
  <li><code>04_provider_backed_standard_library_call_declares_provider_requirement</code></li>
  <li><code>05_widget_value_and_reference_are_preserved_across_lowering</code></li>
  <li><code>06_system_library_call_preserves_capability_and_local_status</code></li>
  <li><code>07_scalar_math_call_preserves_value_library_identity</code></li>
</ul>

<p>
UI-heavy or runtime-service-heavy cases remain explicitly bounded by their
declared capabilities. They do not expand the default first Native CPU LLVM
core subset by implication.
</p>

<hr/>

<h2 id="expected-case-structure">9. Expected Case Structure</h2>

<p>
Each valid compiler-corridor case SHOULD state, as applicable:
</p>

<ul>
  <li>the source scope,</li>
  <li>the semantic scope,</li>
  <li>the canonical IR expectation,</li>
  <li>the canonical JSON IR expectation where relevant,</li>
  <li>the lowering expectation,</li>
  <li>the backend-contract expectation,</li>
  <li>the declared backend-family or profile scope,</li>
  <li>the expected preserved distinctions.</li>
</ul>

<p>
A useful pattern is:
</p>

<pre><code>Expected loadability:
Expected structural validity:
Expected semantic acceptance:
Expected IR derivation:
Expected IR schema result:
Expected lowering result:
Expected backend-contract result:
Declared profile scope:
Declared backend-family scope:
Required preserved distinctions:</code></pre>

<hr/>

<h2 id="expected-outcome-classes">10. Expected Outcome Classes</h2>

<p>
Positive compiler-corridor cases will typically use outcome declarations such as:
</p>

<ul>
  <li><code>Expected structural validity: valid</code></li>
  <li><code>Expected semantic acceptance: accepted</code></li>
  <li><code>Expected IR derivation: derivable</code></li>
  <li><code>Expected IR schema result: schema-valid</code></li>
  <li><code>Expected lowering result: lowerable</code></li>
  <li><code>Expected backend-contract result: emittable</code></li>
  <li><code>Expected backend-family consumption: consumable</code></li>
</ul>

<p>
Not every future case must go all the way to a declared backend-family consumer.
However, once a case declares that scope, the later corridor stages become part of the public expectation.
</p>

<hr/>

<h2 id="recommended-case-growth-order">11. Recommended Case Growth Order</h2>

<p>
Growth in this directory SHOULD remain disciplined.
</p>

<p>
The recommended order is:
</p>

<pre><code>pure computation
   -&gt;
structured control
   -&gt;
explicit state
   -&gt;
restricted interop
   -&gt;
provider-backed dependency closure
   -&gt;
UI/widget category preservation
   -&gt;
bounded system host-capability reads
   -&gt;
published scalar math value-call preservation
   -&gt;
more complex effect surfaces</code></pre>

<p>
This order keeps the first compilation corridor tight and auditable.
</p>

<p>
The first three published cases are therefore not arbitrary examples.
They are the minimum coherent positive nucleus of the corridor.
</p>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
This subdirectory defines positive conformance cases for the published FROG compiler corridor.
</p>

<p>
The canonical v0.1 positive order is:
</p>

<pre><code>01_pure_arithmetic_is_consumable
02_structured_control_is_consumable
03_explicit_state_is_consumable
04_provider_backed_standard_library_call_declares_provider_requirement
05_widget_value_and_reference_are_preserved_across_lowering
06_system_library_call_preserves_capability_and_local_status
07_scalar_math_call_preserves_value_library_identity</code></pre>

<p>
These cases extend the public truth surface beyond ordinary language validity by making the following stages testable where declared:
</p>

<pre><code>semantic acceptance
   -&gt;
canonical Execution IR derivation
   -&gt;
canonical JSON IR validity
   -&gt;
lowering eligibility
   -&gt;
backend-contract eligibility
   -&gt;
declared backend-family consumability</code></pre>

<p>
For v0.1, the first intended target is the conservative subset of the optional <code>native_cpu_llvm</code> profile.
</p>

<p>
This keeps the compiler corridor explicit, bounded, and publicly testable on its positive path.
</p>
