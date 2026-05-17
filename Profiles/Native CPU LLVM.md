<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Profile — Native CPU LLVM</h1>

<p align="center">
  <strong>Optional standardized native CPU compilation profile for FROG v0.1</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-profile-exists">2. Why This Profile Exists</a></li>
  <li><a href="#architectural-position">3. Architectural Position</a></li>
  <li><a href="#scope">4. Scope</a></li>
  <li><a href="#non-goals">5. Non-Goals</a></li>
  <li><a href="#profile-identity">6. Profile Identity</a></li>
  <li><a href="#core-purpose">7. Core Purpose</a></li>
  <li><a href="#compilation-corridor">8. Compilation Corridor</a></li>
  <li><a href="#profile-entry-condition">9. Profile Entry Condition</a></li>
  <li><a href="#accepted-v01-subset-posture">10. Accepted v0.1 Subset Posture</a></li>
  <li><a href="#accepted-program-shape">11. Accepted Program Shape</a></li>
  <li><a href="#control-structure-posture">12. Control-Structure Posture</a></li>
  <li><a href="#explicit-state-posture">13. Explicit State Posture</a></li>
  <li><a href="#type-and-representation-posture">14. Type and Representation Posture</a></li>
  <li><a href="#effect-posture">15. Effect Posture</a></li>
  <li><a href="#ui-posture">16. UI Posture</a></li>
  <li><a href="#interop-and-external-call-posture">17. Interop and External Call Posture</a></li>
  <li><a href="#lowering-expectations">18. Lowering Expectations</a></li>
  <li><a href="#backend-contract-expectations">19. Backend Contract Expectations</a></li>
  <li><a href="#related-execution-contract">20. Related Execution Contract</a></li>
  <li><a href="#consumer-backend-family-posture">21. Consumer Backend-Family Posture</a></li>
  <li><a href="#rejection-conditions">22. Rejection Conditions</a></li>
  <li><a href="#conformance-reading">23. Conformance Reading</a></li>
  <li><a href="#relation-with-other-specification-layers">24. Relation with Other Specification Layers</a></li>
  <li><a href="#future-evolution">25. Future Evolution</a></li>
  <li><a href="#summary">26. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines <code>native_cpu_llvm</code> as an optional standardized FROG profile in base v0.1.
</p>

<p>
Its purpose is to define a first serious compilation-oriented profile for programs intended to pass through:
</p>

<pre><code>validated meaning
   -&gt;
canonical FROG Execution IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
LLVM-oriented native CPU backend family
</code></pre>

<p>
This profile is intentionally conservative.
</p>

<p>
It does not claim that every valid FROG program belongs automatically to the same first native compiled corridor.
Instead, it defines one bounded profile surface under which a producer and a downstream consumer may make a serious, public, disciplined claim that a semantically accepted FROG program can be compiled toward a native CPU realization through an LLVM-oriented backend family without collapsing FROG into LLVM itself.
</p>

<hr/>

<h2 id="why-this-profile-exists">2. Why This Profile Exists</h2>

<p>
The core repository architecture already distinguishes:
</p>

<ul>
  <li>canonical source,</li>
  <li>validated language meaning,</li>
  <li>canonical Execution IR,</li>
  <li>lowering,</li>
  <li>backend contract,</li>
  <li>downstream backend families.</li>
</ul>

<p>
That separation makes it possible to define a first industrial compilation corridor without collapsing FROG into one specific compiler technology.
</p>

<p>
This profile exists to make one important downstream route explicit:
</p>

<ul>
  <li>a route where a conforming implementation can take an accepted subset of FROG,</li>
  <li>derive canonical open IR,</li>
  <li>lower it faithfully,</li>
  <li>emit a consumer-facing backend contract,</li>
  <li>and hand that contract to an LLVM-oriented native backend family targeting CPUs.</li>
</ul>

<p>
This profile therefore turns a general compiler direction into an explicit optional capability class.
</p>

<hr/>

<h2 id="architectural-position">3. Architectural Position</h2>

<p>
This profile must be read inside the repository-wide ownership boundaries:
</p>

<pre><code>Expression/
   - canonical source form

Language/
   - validated program meaning

Libraries/
   - intrinsic primitive vocabularies

IR/
   - canonical Execution IR
   - derivation
   - identity and recoverability
   - schema
   - lowering
   - backend contract

Profiles/
   - optional standardized capability families

Implementations/
   - non-normative executable workspaces
</code></pre>

<p>
Accordingly, this document:
</p>

<ul>
  <li>does not redefine canonical source,</li>
  <li>does not redefine semantic truth,</li>
  <li>does not redefine the canonical Execution IR,</li>
  <li>does not redefine lowering or backend contract ownership,</li>
  <li>does define an optional standardized capability family for one compilation corridor.</li>
</ul>

<p>
Compactly:
</p>

<pre><code>Language
   defines
meaning

IR
   defines
open execution-facing representation

Lowering
   specializes
for downstream compilation

Backend contract
   hands off
lowered executable meaning

This profile
   constrains
one bounded native CPU LLVM-oriented compilation corridor
</code></pre>

<hr/>

<h2 id="scope">4. Scope</h2>

<p>
This profile defines:
</p>

<ul>
  <li>a first compilation-ready subset posture for FROG v0.1,</li>
  <li>the capability assumptions required for CPU-native LLVM-oriented downstream consumption,</li>
  <li>the execution-facing and lowering-facing expectations that a conforming implementation must satisfy before claiming this profile,</li>
  <li>the rejection conditions for programs or lowered forms that exceed the profile,</li>
  <li>the boundary between this compilation profile and stronger execution-contract closure.</li>
</ul>

<p>
This profile is therefore not only about “can LLVM exist downstream?” but about whether a disciplined published corridor exists from accepted FROG meaning to an LLVM-oriented native CPU backend family.
</p>

<hr/>

<h2 id="non-goals">5. Non-Goals</h2>

<p>
This profile is not:
</p>

<ul>
  <li>a replacement for the core language,</li>
  <li>a hidden backend specification disguised as a profile,</li>
  <li>a way to redefine the canonical Execution IR as LLVM IR,</li>
  <li>a promise that every valid FROG program is automatically Native CPU LLVM compatible,</li>
  <li>a promise that UI-heavy or runtime-heavy programs belong to the same first compilation corridor as pure native cores,</li>
  <li>a definition of one mandatory private runtime implementation,</li>
  <li>a definition of one universal execution contract for all future consumers.</li>
</ul>

<p>
This profile also does not authorize silent semantic invention at consumer level.
</p>

<hr/>

<h2 id="profile-identity">6. Profile Identity</h2>

<p>
The identifier of this profile is:
</p>

<pre><code>native_cpu_llvm</code></pre>

<p>
This identifier names an optional standardized capability claim.
</p>

<p>
It does not mean:
</p>

<ul>
  <li>LLVM defines FROG,</li>
  <li>native CPU execution is the only valid realization route,</li>
  <li>all deployment modes are equivalent,</li>
  <li>all runtime assumptions are already standardized.</li>
</ul>

<p>
It means only that one optional profile exists for a bounded native CPU compilation corridor oriented toward LLVM-family downstream consumers.
</p>

<hr/>

<h2 id="core-purpose">7. Core Purpose</h2>

<p>
The core purpose of this profile is to enable a first industrially credible statement of the form:
</p>

<pre><code>For the accepted Native CPU LLVM v0.1 subset,
a semantically accepted FROG program
can be derived to canonical FROG Execution IR,
lowered faithfully,
emitted as a backend contract,
and consumed by an LLVM-oriented native CPU backend family.
</code></pre>

<p>
This profile exists so that this statement becomes:
</p>

<ul>
  <li>public,</li>
  <li>bounded,</li>
  <li>conformance-compatible,</li>
  <li>architecturally disciplined.</li>
</ul>

<hr/>

<h2 id="compilation-corridor">8. Compilation Corridor</h2>

<p>
The intended corridor for this profile is:
</p>

<pre><code>.frog
   -&gt;
structural validation
   -&gt;
semantic validation
   -&gt;
canonical FROG Execution IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
LLVM-oriented native CPU consumer
   -&gt;
native executable realization
</code></pre>

<p>
This profile applies only if the program remains within the accepted subset and the implementation preserves the full corridor discipline.
</p>

<p>
In particular:
</p>

<ul>
  <li>the canonical Execution IR remains open and language-owned,</li>
  <li>lowering remains downstream specialization,</li>
  <li>backend contract remains the standardized consumer-facing handoff,</li>
  <li>LLVM remains a downstream consumer family.</li>
</ul>

<p>
This corridor is intentionally compilation-oriented first.
</p>

<p>
It says:
</p>

<pre><code>accepted meaning
   can reach
backend-family consumability
</code></pre>

<p>
It does not yet, by itself, say:
</p>

<pre><code>every backend-consumable case
is automatically execution-start closed
</code></pre>

<hr/>

<h2 id="profile-entry-condition">9. Profile Entry Condition</h2>

<p>
A program may only claim this profile if all of the following are true:
</p>

<ul>
  <li>the program is structurally valid canonical FROG source,</li>
  <li>the program is semantically accepted by the core language and any referenced optional capability surfaces,</li>
  <li>the program remains within the accepted v0.1 subset of this profile,</li>
  <li>the implementation can derive canonical FROG Execution IR without losing required distinctions,</li>
  <li>the implementation can lower that IR faithfully toward a native CPU LLVM-oriented handoff,</li>
  <li>the implementation can emit a backend contract consistent with that handoff.</li>
</ul>

<p>
If any of these conditions fails, the profile claim must be rejected.
</p>

<hr/>

<h2 id="accepted-v01-subset-posture">10. Accepted v0.1 Subset Posture</h2>

<p>
The accepted v0.1 subset of this profile is intentionally conservative.
</p>

<p>
The first published goal is not to absorb the entire language ecosystem at once, but to close a serious native compiled corridor for a bounded and intelligible subset.
</p>

<p>
This means the first accepted core naturally favors:
</p>

<ul>
  <li>deterministic pure or mostly pure computation,</li>
  <li>explicit and structurally clear control flow,</li>
  <li>explicit state where state is admitted,</li>
  <li>clear lowering routes,</li>
  <li>clear backend-contract emission.</li>
</ul>

<p>
The following remain outside the default first accepted core of this profile unless explicitly supported by a conforming implementation statement:
</p>

<ul>
  <li>UI-heavy execution surfaces,</li>
  <li>runtime-service-dependent interactive widget manipulation as a primary compiled-core feature,</li>
  <li>distributed or multi-device semantics,</li>
  <li>accelerator-specific placement semantics,</li>
  <li>hard real-time commitments,</li>
  <li>profile surfaces that require a different backend family or stronger runtime contracts.</li>
</ul>

<hr/>

<h2 id="accepted-program-shape">11. Accepted Program Shape</h2>

<p>
A program shape belongs naturally to this profile when it can be understood as:
</p>

<ul>
  <li>a semantically accepted graph,</li>
  <li>with explicit structure where required,</li>
  <li>with explicit state where required,</li>
  <li>with effects kept bounded and explicit,</li>
  <li>with a coherent route to lowered native CPU realization.</li>
</ul>

<p>
The preferred first-profile posture is therefore:
</p>

<pre><code>clear accepted graph
   -&gt;
clear canonical Execution IR
   -&gt;
clear lowering
   -&gt;
clear backend handoff
   -&gt;
clear native CPU consumer route
</code></pre>

<p>
Programs that depend on broad hidden services, broad implicit runtime mediation, or ambiguous execution-side assumptions do not belong naturally to the first accepted subset.
</p>

<hr/>

<h2 id="control-structure-posture">12. Control-Structure Posture</h2>

<p>
Structured control may belong to this profile when it remains:
</p>

<ul>
  <li>explicit in source meaning,</li>
  <li>preserved in derivation,</li>
  <li>coherently lowerable,</li>
  <li>compatible with a native CPU backend-family handoff.</li>
</ul>

<p>
This includes, in principle, bounded structured forms such as:
</p>

<ul>
  <li>case-style branching,</li>
  <li>loop structures with explicit boundaries,</li>
  <li>state-aware repeated execution where state remains explicit.</li>
</ul>

<p>
The profile does not require that every conceivable future control feature already belong to the accepted v0.1 subset.
</p>

<p>
It does require that admitted control structures be:
</p>

<pre><code>semantically accepted
   +
IR-preservable
   +
lowerable
   +
contract-emittable
</code></pre>

<hr/>

<h2 id="explicit-state-posture">13. Explicit State Posture</h2>

<p>
Explicit state may belong to this profile when:
</p>

<ul>
  <li>state is explicit in language meaning,</li>
  <li>state participation remains explicit in derivation,</li>
  <li>lowering preserves required state boundaries,</li>
  <li>the backend contract can describe the relevant commitments clearly enough for the consumer.</li>
</ul>

<p>
This profile does not authorize stateful behavior to re-enter through hidden persistence or accidental runtime convention.
</p>

<p>
If state exists, it must remain:
</p>

<pre><code>explicit upstream
   -&gt;
explicit in IR
   -&gt;
explicit in lowering commitments
   -&gt;
explicit enough at backend handoff
</code></pre>

<hr/>

<h2 id="type-and-representation-posture">14. Type and Representation Posture</h2>

<p>
This profile assumes that admitted programs use types and representations that a native CPU LLVM-oriented consumer can honor without inventing language meaning.
</p>

<p>
That does not require the language to collapse into one consumer-private representation model.
</p>

<p>
It does require that:
</p>

<ul>
  <li>required type distinctions remain preserved through derivation and lowering,</li>
  <li>the lowered form be explicit enough for the consumer to honor representation-sensitive obligations,</li>
  <li>profile claims be rejected when representation requirements exceed what the consumer can honor consistently.</li>
</ul>

<p>
This profile therefore supports serious compilation-facing discipline without making one private data layout the hidden definition of the language.
</p>

<hr/>

<h2 id="effect-posture">15. Effect Posture</h2>

<p>
The first version of this profile is conservative around effects.
</p>

<p>
In particular:
</p>

<ul>
  <li>pure or mostly pure computation belongs naturally to this first corridor,</li>
  <li>effect surfaces may be admitted only when the producer can model them explicitly in lowering and backend contract form,</li>
  <li>effect ordering that remains architecturally significant must not be erased silently by the consumer.</li>
</ul>

<p>
This profile therefore allows effectful compilation only under explicit discipline.
It is not a blanket authorization for arbitrary host dependency surfaces.
</p>

<hr/>

<h2 id="ui-posture">16. UI Posture</h2>

<p>
UI-heavy interactive surfaces are not part of the default first compiled-core promise of this profile.
</p>

<p>
More precisely:
</p>

<ul>
  <li>runtime-mediated UI services may be supported later, but they are not assumed by default in the first closed profile corridor,</li>
  <li>widget-object manipulation as a primary compiled-core feature is outside the default accepted subset,</li>
  <li>interactive event-loop ownership is outside the default accepted subset,</li>
  <li>full front-panel runtime closure is not implied by this profile alone.</li>
</ul>

<p>
This keeps the first industrial profile focused on closing a real native compiled core instead of prematurely absorbing the full UI runtime story.
</p>

<hr/>

<h2 id="interop-and-external-call-posture">17. Interop and External Call Posture</h2>

<p>
Restricted interop MAY belong to this profile if and only if:
</p>

<ul>
  <li>the callable boundary is explicit,</li>
  <li>the type boundary is explicit,</li>
  <li>the lowering path is coherent,</li>
  <li>the backend contract makes the assumptions explicit,</li>
  <li>the consumer can honor the call-surface obligations.</li>
</ul>

<p>
Interop support under this profile is therefore:
</p>

<ul>
  <li>possible,</li>
  <li>restricted,</li>
  <li>contract-bound,</li>
  <li>not a license for arbitrary foreign-runtime dependency surfaces.</li>
</ul>

<p>
When an accepted program uses an external callable capability, the dependency must remain explicit enough for lowering, backend contract emission, and deployment preparation.
For example, a native image-decoding capability may be represented as a declared external call surface or library dependency; it must not appear as an implicit feature of the runtime simply because one implementation can provide it.
</p>

<hr/>

<h2 id="lowering-expectations">18. Lowering Expectations</h2>

<p>
A producer claiming this profile MUST lower accepted semantic meaning in a way that remains faithful to the preserved distinctions of the program.
</p>

<p>
In particular, lowering under this profile MUST NOT:
</p>

<ul>
  <li>erase explicit control commitments that still matter downstream,</li>
  <li>erase explicit state commitments that still matter downstream,</li>
  <li>silently reinterpret unsupported features as though they belonged to the accepted subset,</li>
  <li>pretend one backend-family convenience is itself the language definition.</li>
</ul>

<p>
Lowering under this profile SHOULD move toward a form that is:
</p>

<ul>
  <li>consumer-usable,</li>
  <li>backend-oriented,</li>
  <li>explicit about required commitments,</li>
  <li>compatible with native CPU realization.</li>
</ul>

<hr/>

<h2 id="backend-contract-expectations">19. Backend Contract Expectations</h2>

<p>
A profile-supporting producer MUST emit a backend contract explicit enough for a native CPU LLVM-oriented consumer to decide whether it can consume the lowered form without guessing.
</p>

<p>
That contract SHOULD make explicit, as applicable:
</p>

<ul>
  <li>backend family posture,</li>
  <li>accepted lowered executable scope,</li>
  <li>representation commitments,</li>
  <li>state and control commitments,</li>
  <li>effect or interop commitments,</li>
  <li>required capabilities,</li>
  <li>rejection conditions.</li>
</ul>

<p>
The backend contract remains downstream from canonical open IR and lowering.
This profile depends on that distinction and does not replace it.
</p>

<p>
Compactly:
</p>

<pre><code>canonical IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
consumer decision
</code></pre>

<p>
The backend contract answers:
</p>

<pre><code>Can this downstream consumer family
accept and honor the lowered executable commitments?
</code></pre>

<hr/>

<h2 id="related-execution-contract">20. Related Execution Contract</h2>

<p>
This profile is compilation-oriented first.
</p>

<p>
Accordingly, it should be read together with any companion execution-contract document that closes the additional question:
</p>

<pre><code>When may a backend-consumable lowered FROG program
actually be considered ready to begin execution
under a bounded native CPU LLVM corridor?
</code></pre>

<p>
The distinction is essential:
</p>

<pre><code>this profile
   defines
a bounded compilation corridor

backend contract
   defines
standardized consumer-facing handoff

execution contract
   defines
additional profile-level closure
for beginning execution under explicit assumptions
</code></pre>

<p>
Therefore:
</p>

<ul>
  <li>a program may be language-valid yet profile-rejected,</li>
  <li>a program may be profile-valid yet not execution-ready,</li>
  <li>a lowered form may be backend-consumable yet still require additional execution-side assumptions to be stated explicitly,</li>
  <li>execution-start closure must not be silently inferred from compilability alone.</li>
</ul>

<p>
A companion execution contract, when published, belongs to the same profile family but serves a different role:
</p>

<ul>
  <li>this document says how a bounded accepted FROG subset reaches a native CPU LLVM-oriented backend family,</li>
  <li>the execution contract says under what explicit conditions that bounded route may actually begin execution.</li>
</ul>

<p>
This separation preserves the repository-wide architecture:
</p>

<pre><code>Language
   owns meaning

IR
   owns open execution-facing representation, lowering, and backend handoff

Profiles
   may define bounded capability corridors and execution-side companion constraints

Implementations
   still own private runtime realization
</code></pre>

<p>
This is the intended reading discipline:
</p>

<pre><code>compilation corridor
   is necessary
but not always sufficient
for execution-start closure
</code></pre>

<hr/>

<h2 id="consumer-backend-family-posture">21. Consumer Backend-Family Posture</h2>

<p>
A consumer under this profile is any backend-family consumer that is:
</p>

<ul>
  <li>native CPU oriented,</li>
  <li>LLVM oriented in downstream compilation posture,</li>
  <li>able to honor the emitted backend contract.</li>
</ul>

<p>
Such a consumer MAY:
</p>

<ul>
  <li>compile to machine code,</li>
  <li>optimize through LLVM-oriented passes,</li>
  <li>produce native object or executable artifacts,</li>
  <li>participate in JIT or AOT flows,</li>
  <li>delegate some support needs to a runtime surface declared by the contract,</li>
  <li>produce a generated launcher or self-contained executable when the accepted program scope and its selected dependencies are explicit enough.</li>
</ul>

<p>
The generated-launcher or self-contained posture is not a promise that all FROG programs can avoid every runtime service.
It means that, for an accepted compiled subset, the consumer may package only the program-specific code, selected support modules, declared external libraries, required host services, and required assets instead of depending on a complete installed runtime host.
</p>

<p>
Such a consumer MUST NOT:
</p>

<ul>
  <li>reinterpret unsupported profile features silently,</li>
  <li>pretend LLVM defines the language,</li>
  <li>erase contract-required distinctions without honoring them.</li>
</ul>

<hr/>

<h2 id="rejection-conditions">22. Rejection Conditions</h2>

<p>
A profile claim MUST be rejected if any of the following applies:
</p>

<ul>
  <li>the program exceeds the accepted v0.1 subset,</li>
  <li>the implementation cannot derive canonical FROG Execution IR faithfully,</li>
  <li>the implementation cannot lower the accepted semantics coherently,</li>
  <li>the implementation cannot emit a backend contract explicit enough for the consumer,</li>
  <li>the consumer cannot honor required control, state, representation, or capability commitments,</li>
  <li>the route would require silent semantic invention or hidden runtime assumptions beyond the declared profile surface.</li>
</ul>

<p>
This is one of the key benefits of the profile:
</p>

<pre><code>explicit profile rejection
   is better than
implicit backend drift
</code></pre>

<p>
Likewise, explicit execution-contract rejection is better than pretending compilability automatically means safe executable start.
</p>

<hr/>

<h2 id="conformance-reading">23. Conformance Reading</h2>

<p>
This profile should be read together with the repository conformance corridor.
</p>

<p>
In particular, claiming <code>native_cpu_llvm</code> support does not weaken the public truth surface around:
</p>

<ul>
  <li>accept / reject correctness,</li>
  <li>IR derivation correctness,</li>
  <li>identity and recoverability,</li>
  <li>schema validity,</li>
  <li>lowering discipline,</li>
  <li>backend-contract discipline.</li>
</ul>

<p>
Future conformance growth for this profile SHOULD therefore introduce cases that distinguish:
</p>

<ul>
  <li>language-valid but profile-rejected,</li>
  <li>IR-derivable but not lowerable under this profile,</li>
  <li>lowerable but not contract-emittable under this profile,</li>
  <li>contract-emittable but consumer-rejected under this profile,</li>
  <li>consumer-acceptable but execution-contract-rejected where stronger execution closure is required.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specification-layers">24. Relation with Other Specification Layers</h2>

<p>
This profile remains subordinate to the rest of the published architecture:
</p>

<ul>
  <li><code>Expression/</code> still owns source form,</li>
  <li><code>Language/</code> still owns validated meaning,</li>
  <li><code>Libraries/</code> still own intrinsic primitive law,</li>
  <li><code>IR/</code> still owns canonical IR, derivation, schema, lowering, and backend contract,</li>
  <li><code>Profiles/</code> only owns the optional standardized capability claim described here and any explicit profile-level companion closures such as execution contracts.</li>
</ul>

<p>
This is critical:
</p>

<pre><code>Native CPU LLVM
   is a profile claim

it is not
the language core

it is not
the canonical IR

it is not
the hidden definition of execution for all FROG programs
</code></pre>

<hr/>

<h2 id="future-evolution">25. Future Evolution</h2>

<p>
This profile is intentionally a first closure step.
It MAY evolve later through:
</p>

<ul>
  <li>wider accepted structured-control coverage,</li>
  <li>wider explicit-state coverage,</li>
  <li>better effect-surface support,</li>
  <li>restricted UI-service support,</li>
  <li>better interop coverage,</li>
  <li>richer native deployment modes,</li>
  <li>additional backend-family refinements,</li>
  <li>stronger execution-contract closure for bounded executable subsets.</li>
</ul>

<p>
However, future growth SHOULD remain disciplined:
</p>

<pre><code>close one coherent corridor
before broadening the optional surface
</code></pre>

<hr/>

<h2 id="summary">26. Summary</h2>

<p>
This document defines <code>native_cpu_llvm</code> as an optional standardized FROG profile for a first industrial native CPU compilation corridor oriented toward LLVM-family downstream consumers.
</p>

<p>
Its core rules are:
</p>

<ul>
  <li>the profile is optional,</li>
  <li>the profile is downstream-compilation-oriented,</li>
  <li>the profile does not redefine the language core or the canonical IR,</li>
  <li>the profile accepts only a conservative v0.1 subset,</li>
  <li>the profile requires faithful derivation, lowering, and backend-contract emission,</li>
  <li>the profile does not silently equate compilability with execution-start closure,</li>
  <li>the profile requires explicit rejection when the corridor cannot be honored.</li>
</ul>

<p>
This profile therefore enables a serious public statement:
</p>

<pre><code>FROG can support a real native compiled corridor
without collapsing the language into LLVM,
because LLVM remains downstream from
validated meaning,
canonical open IR,
lowering,
and backend contract.
</code></pre>

<p>
And where execution-start closure matters, that closure must be stated explicitly through a companion execution contract rather than being guessed from compilability alone.
</p>

<p>
That is the purpose of this profile in base v0.1.
</p>
