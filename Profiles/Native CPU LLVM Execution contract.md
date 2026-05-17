<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Profile — Native CPU LLVM Execution Contract</h1>

<p align="center">
  <strong>Optional execution-start closure for the Native CPU LLVM profile in FROG v0.1</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#architectural-position">3. Architectural Position</a></li>
  <li><a href="#scope">4. Scope</a></li>
  <li><a href="#non-goals">5. Non-Goals</a></li>
  <li><a href="#core-definition">6. Core Definition</a></li>
  <li><a href="#relation-with-the-native-cpu-llvm-profile">7. Relation with the Native CPU LLVM Profile</a></li>
  <li><a href="#relation-with-irbackend-contractmd">8. Relation with IR/Backend contract.md</a></li>
  <li><a href="#execution-corridor">9. Execution Corridor</a></li>
  <li><a href="#contract-entry-condition">10. Contract Entry Condition</a></li>
  <li><a href="#execution-unit">11. Execution Unit</a></li>
  <li><a href="#entry-surface">12. Entry Surface</a></li>
  <li><a href="#execution-lifecycle">13. Execution Lifecycle</a></li>
  <li><a href="#execution-modes">14. Execution Modes</a></li>
  <li><a href="#state-instantiation-and-initialization">15. State Instantiation and Initialization</a></li>
  <li><a href="#scheduling-and-order-visibility">16. Scheduling and Order Visibility</a></li>
  <li><a href="#effects-io-and-host-services">17. Effects, IO, and Host Services</a></li>
  <li><a href="#data-representation-and-call-boundary-posture">18. Data Representation and Call-Boundary Posture</a></li>
  <li><a href="#ui-posture">19. UI Posture</a></li>
  <li><a href="#fault-rejection-and-termination-posture">20. Fault, Rejection, and Termination Posture</a></li>
  <li><a href="#producer-obligations">21. Producer Obligations</a></li>
  <li><a href="#consumer-obligations">22. Consumer Obligations</a></li>
  <li><a href="#conformance-reading">23. Conformance Reading</a></li>
  <li><a href="#relation-with-other-specification-layers">24. Relation with Other Specification Layers</a></li>
  <li><a href="#future-evolution">25. Future Evolution</a></li>
  <li><a href="#summary">26. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines an optional execution contract for the <code>native_cpu_llvm</code> profile in FROG v0.1.
</p>

<p>
Its purpose is to close one specific gap that remains after:
</p>

<pre><code>.frog source
   -&gt;
structural acceptance
   -&gt;
semantic acceptance
   -&gt;
canonical FROG Execution IR
   -&gt;
lowering
   -&gt;
backend contract
</code></pre>

<p>
namely:
</p>

<pre><code>Under what explicit assumptions
may an accepted Native CPU LLVM program
actually begin execution?
</code></pre>

<p>
This document does not redefine the language, the canonical Execution IR, lowering, or the generic backend contract.
It defines an additional profile-level closure for bounded execution start under the Native CPU LLVM profile.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
A compilation corridor and a backend handoff are not yet identical to an execution-start guarantee.
</p>

<p>
Even when a program is:
</p>

<ul>
  <li>language-valid,</li>
  <li>profile-valid,</li>
  <li>IR-derivable,</li>
  <li>lowerable,</li>
  <li>backend-contract-emittable,</li>
  <li>and consumer-acceptable,</li>
</ul>

<p>
the following questions still remain:
</p>

<ul>
  <li>what exactly is instantiated,</li>
  <li>what exactly counts as the entry boundary,</li>
  <li>how explicit state is initialized,</li>
  <li>whether execution is one-shot or retained-instance based,</li>
  <li>what host services are assumed,</li>
  <li>what faults require rejection before start,</li>
  <li>what conditions count as termination or quiescence.</li>
</ul>

<p>
This document exists to answer those questions conservatively for the first Native CPU LLVM execution-ready corridor.
</p>

<hr/>

<h2 id="architectural-position">3. Architectural Position</h2>

<p>
This document must be read under the repository-wide layering:
</p>

<pre><code>Expression/
   - canonical source form

Language/
   - validated program meaning

Libraries/
   - intrinsic primitive law

IR/
   - canonical execution-facing representation
   - derivation
   - schema posture
   - lowering
   - backend contract

Profiles/
   - optional standardized capability families
   - bounded profile-side capability closure

Implementations/
   - non-normative executable realization workspaces
</code></pre>

<p>
Accordingly:
</p>

<ul>
  <li>the language still owns meaning,</li>
  <li>IR still owns canonical execution-facing representation, lowering, and backend handoff,</li>
  <li>this document only adds bounded execution-start closure for one optional profile corridor,</li>
  <li>private runtime realization remains downstream.</li>
</ul>

<hr/>

<h2 id="scope">4. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>what execution-start readiness means for the bounded Native CPU LLVM corridor,</li>
  <li>what execution unit is considered startable,</li>
  <li>what lifecycle assumptions must be explicit,</li>
  <li>what initialization and state assumptions must be explicit,</li>
  <li>what host-service assumptions must be explicit,</li>
  <li>what must cause rejection rather than silent fallback.</li>
</ul>

<p>
This document does not define a universal runtime for all FROG programs.
</p>

<hr/>

<h2 id="non-goals">5. Non-Goals</h2>

<p>
This document is not:
</p>

<ul>
  <li>a replacement for <code>IR/Backend contract.md</code>,</li>
  <li>a universal FROG runtime specification,</li>
  <li>a mandatory ABI for every implementation,</li>
  <li>a complete event-loop or UI runtime specification,</li>
  <li>a license to move private runtime choices into language law,</li>
  <li>a promise that every valid FROG program belongs to this execution-ready subset.</li>
</ul>

<hr/>

<h2 id="core-definition">6. Core Definition</h2>

<p>
For the Native CPU LLVM profile, an <strong>execution contract</strong> is the explicit profile-level statement that:
</p>

<ul>
  <li>a bounded accepted program scope exists,</li>
  <li>its lowered form and backend contract are available,</li>
  <li>its execution-start assumptions are explicit enough,</li>
  <li>and a native CPU LLVM-oriented consumer may begin execution without inventing hidden semantic rules.</li>
</ul>

<p>
Compactly:
</p>

<pre><code>backend contract
   says
what a downstream consumer may rely on

execution contract
   says
what else must be explicit
before actual execution start may be claimed
</code></pre>

<hr/>

<h2 id="relation-with-the-native-cpu-llvm-profile">7. Relation with the Native CPU LLVM Profile</h2>

<p>
The Native CPU LLVM profile defines the bounded compilation corridor.
</p>

<p>
This document defines the bounded execution-start closure that may accompany that corridor.
</p>

<p>
The distinction is:
</p>

<pre><code>Native CPU LLVM profile
   =
bounded route to native CPU LLVM-oriented backend-family consumability

Native CPU LLVM execution contract
   =
bounded route from consumability
to explicit execution-start readiness
</code></pre>

<p>
A program may therefore be:
</p>

<ul>
  <li>profile-valid but not yet execution-contract-valid,</li>
  <li>consumer-acceptable but not yet execution-start-ready,</li>
  <li>compilable in principle but still missing explicit start assumptions.</li>
</ul>

<hr/>

<h2 id="relation-with-irbackend-contractmd">8. Relation with IR/Backend contract.md</h2>

<p>
This document must be read as a companion to <code>IR/Backend contract.md</code>, not as a replacement for it.
</p>

<p>
The intended architectural split is:
</p>

<pre><code>IR/Backend contract.md
   =
generic standardized downstream handoff

this document
   =
profile-level execution-start closure
for one bounded Native CPU LLVM corridor
</code></pre>

<p>
The generic backend contract remains owned by <code>IR/</code>.
This document does not move that ownership into <code>Profiles/</code>.
</p>

<hr/>

<h2 id="execution-corridor">9. Execution Corridor</h2>

<p>
The intended bounded corridor is:
</p>

<pre><code>.frog
   -&gt;
structural acceptance
   -&gt;
semantic acceptance
   -&gt;
canonical FROG Execution IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
execution contract satisfaction
   -&gt;
native CPU LLVM execution start
</code></pre>

<p>
This document applies only at the last transition:
</p>

<pre><code>backend-contract-emitted
   -&gt;
execution-start-ready
</code></pre>

<hr/>

<h2 id="contract-entry-condition">10. Contract Entry Condition</h2>

<p>
A program scope may only claim conformance with this execution contract if all of the following are true:
</p>

<ul>
  <li>the source is canonical where required,</li>
  <li>the program is semantically accepted,</li>
  <li>the program remains within the accepted Native CPU LLVM profile subset,</li>
  <li>canonical Execution IR has been derived faithfully,</li>
  <li>lowering has preserved the required commitments,</li>
  <li>a backend contract has been emitted,</li>
  <li>the execution unit is explicit,</li>
  <li>the execution mode is explicit,</li>
  <li>required host-service assumptions are explicit,</li>
  <li>no hidden runtime assumption is being presented as though it were already standardized.</li>
</ul>

<p>
Failure of any one of these conditions requires rejection of the execution-contract claim.
</p>

<hr/>

<h2 id="execution-unit">11. Execution Unit</h2>

<p>
Execution begins from an explicitly identified <strong>execution unit</strong>.
</p>

<p>
An execution unit is the bounded lowered program scope that a conforming consumer is expected to instantiate and start.
</p>

<p>
An execution unit MUST make explicit:
</p>

<ul>
  <li>its identity,</li>
  <li>its input boundary,</li>
  <li>its output boundary,</li>
  <li>its explicit state boundary where applicable,</li>
  <li>its initialization posture,</li>
  <li>its selected execution mode.</li>
</ul>

<p>
An implementation MUST NOT claim start readiness for an ambiguous or partially implicit execution unit.
</p>

<hr/>

<h2 id="entry-surface">12. Entry Surface</h2>

<p>
The entry surface is the explicit activation boundary through which the execution unit begins running.
</p>

<p>
The entry surface MAY be realized downstream as:
</p>

<ul>
  <li>a function-like native boundary,</li>
  <li>a process-main preparation boundary,</li>
  <li>a step-call boundary,</li>
  <li>a retained-instance activation boundary,</li>
  <li>another explicit native intake shape consistent with the backend contract.</li>
</ul>

<p>
This document does not require one universal concrete form.
</p>

<p>
It does require that the following be explicit:
</p>

<pre><code>what must exist before activation
what values are supplied at activation
what state already exists at activation
what outputs become externally visible
what condition counts as completion or quiescence
</code></pre>

<hr/>

<h2 id="execution-lifecycle">13. Execution Lifecycle</h2>

<p>
The conservative lifecycle admitted by this document is:
</p>

<pre><code>prepare
   -&gt;
instantiate
   -&gt;
initialize
   -&gt;
activate
   -&gt;
run or step
   -&gt;
produce visible outputs
   -&gt;
terminate, quiesce, or await next permitted activation
   -&gt;
dispose
</code></pre>

<p>
A conforming implementation does not need to expose all these phases as public APIs, but it MUST preserve the architectural distinction between them whenever that distinction affects correctness.
</p>

<p>
In particular, an implementation MUST NOT silently collapse:
</p>

<ul>
  <li>instantiation into initialization,</li>
  <li>initialization into activation,</li>
  <li>quiescence into termination,</li>
  <li>retained-instance continuation into a fresh-instance restart,</li>
</ul>

<p>
when doing so would change required semantics or state behavior.
</p>

<hr/>

<h2 id="execution-modes">14. Execution Modes</h2>

<p>
This document recognizes the following conservative execution modes for the first Native CPU LLVM execution-ready corridor:
</p>

<ul>
  <li><strong>one-shot mode</strong> — instantiate, run to completion or quiescence, then terminate,</li>
  <li><strong>step mode</strong> — instantiate once, then advance through repeated explicit activations,</li>
  <li><strong>retained-instance host-driven mode</strong> — the host controls repeated activations of a previously materialized execution unit.</li>
</ul>

<p>
The selected execution mode MUST be explicit.
</p>

<p>
A conforming implementation MUST NOT silently assume:
</p>

<pre><code>compiled
   implies
always one-shot process-main execution
</code></pre>

<p>
because some accepted scopes may instead rely on repeated explicit activation.
</p>

<hr/>

<h2 id="state-instantiation-and-initialization">15. State Instantiation and Initialization</h2>

<p>
Explicit state remains explicit under this execution contract.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>state MUST be materialized as explicit state, not reintroduced as accidental hidden persistence,</li>
  <li>required initial values MUST be supplied or derived exactly as permitted upstream,</li>
  <li>the first activation MUST observe the correct initialized state,</li>
  <li>state continuity across activations MUST exist only when the execution mode explicitly allows it.</li>
</ul>

<p>
A conforming execution contract MUST therefore make one of the following postures explicit:
</p>

<ul>
  <li><strong>fresh-instance posture</strong> — each activation gets a newly instantiated state scope,</li>
  <li><strong>retained-instance posture</strong> — state persists across permitted activations,</li>
  <li><strong>externally managed-instance posture</strong> — state lifetime is controlled by an explicit host-side owner.</li>
</ul>

<hr/>

<h2 id="scheduling-and-order-visibility">16. Scheduling and Order Visibility</h2>

<p>
A downstream consumer MAY choose its own scheduler strategy, optimizer sequence, or native realization details.
</p>

<p>
However, it MUST preserve all order-visible obligations that remain significant after lowering and backend handoff.
</p>

<p>
In particular:
</p>

<ul>
  <li>data dependencies remain binding,</li>
  <li>explicit control boundaries remain binding where preserved,</li>
  <li>explicit state visibility remains binding,</li>
  <li>effect ordering remains binding where preserved,</li>
  <li>externally visible outputs MUST NOT appear in an order that contradicts the accepted lowered meaning.</li>
</ul>

<p>
Compactly:
</p>

<pre><code>scheduler freedom
   exists only inside
the freedom still left open
by
meaning
   -&gt;
IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
execution contract
</code></pre>

<hr/>

<h2 id="effects-io-and-host-services">17. Effects, IO, and Host Services</h2>

<p>
The first Native CPU LLVM execution-ready corridor is conservative around effects.
</p>

<p>
Pure or mostly pure executable scopes belong most naturally to this contract.
</p>

<p>
Effectful behavior MAY belong to this contract only if:
</p>

<ul>
  <li>the effect boundary is explicit,</li>
  <li>the required host-service surface is explicit,</li>
  <li>the lowered form preserves the relevant commitments,</li>
  <li>the backend contract exposes the relevant assumptions clearly enough,</li>
  <li>execution may begin without hidden service discovery or hidden semantic invention.</li>
</ul>

<p>
Host services MAY include, for example:
</p>

<ul>
  <li>basic process services,</li>
  <li>memory allocation services,</li>
  <li>declared time or clock access,</li>
  <li>declared external callable boundaries,</li>
  <li>declared runtime-service bridges introduced during lowering.</li>
</ul>

<p>
A program scope MUST be rejected under this contract if execution start depends on an undeclared host-service assumption.
</p>

<p>
Those host services may be satisfied by different deployment postures.
A full runtime host is one possible answer.
A generated launcher, statically linked support module, dynamically linked library, or self-contained executable may also be a valid answer when the required services, external dependencies, and callable boundaries are explicit.
The execution contract requires explicitness; it does not require every accepted deployment to carry an entire general-purpose runtime.
</p>

<hr/>

<h2 id="data-representation-and-call-boundary-posture">18. Data Representation and Call-Boundary Posture</h2>

<p>
This document does not impose one universal ABI.
</p>

<p>
It does require that execution-start assumptions around data representation and activation boundaries be explicit enough for the consumer to begin execution without hidden guesses.
</p>

<p>
Accordingly, where a callable boundary matters, the producer MUST make explicit:
</p>

<ul>
  <li>what values are passed at activation,</li>
  <li>what values are returned or exposed at completion or quiescence,</li>
  <li>what stateful instance handles exist, if any,</li>
  <li>what ownership or lifetime assumptions are required,</li>
  <li>what representation-sensitive commitments remain significant at the boundary.</li>
</ul>

<p>
The exact private representation remains downstream.
</p>

<p>
What is forbidden is:
</p>

<pre><code>opaque hidden boundary assumption
presented as though
the profile had already standardized it
</code></pre>

<hr/>

<h2 id="ui-posture">19. UI Posture</h2>

<p>
UI-heavy interactive execution is outside the default first execution-ready promise of this document.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>pure compiled cores are in scope,</li>
  <li>bounded explicit non-UI effects may be in scope,</li>
  <li>runtime-mediated UI services are out of scope by default,</li>
  <li>interactive event-loop ownership is out of scope by default,</li>
  <li>widget-object manipulation as a primary execution dependency is out of scope by default.</li>
</ul>

<p>
This document therefore does not close the full UI runtime story.
It leaves room for that future work without pretending it is already solved by the first Native CPU LLVM execution-ready corridor.
</p>

<hr/>

<h2 id="fault-rejection-and-termination-posture">20. Fault, Rejection, and Termination Posture</h2>

<p>
A conforming implementation MUST distinguish at least the following conditions:
</p>

<ul>
  <li><strong>profile rejection</strong> — the accepted FROG program is valid but outside the accepted Native CPU LLVM subset,</li>
  <li><strong>execution-contract rejection</strong> — the compilation corridor exists but the execution-start assumptions are incomplete, unsupported, or inconsistent,</li>
  <li><strong>startup fault</strong> — execution was allowed to begin but failed during preparation, instantiation, or initialization,</li>
  <li><strong>runtime fault</strong> — execution began and later failed under explicit runtime or host conditions,</li>
  <li><strong>normal termination</strong> — execution completed under the selected mode,</li>
  <li><strong>quiescent continuation posture</strong> — execution reached a stable wait condition under step or retained-instance semantics.</li>
</ul>

<p>
These states MUST NOT be silently collapsed.
</p>

<p>
In particular:
</p>

<pre><code>language-valid
   +
profile-valid
   +
backend-consumable
does not automatically imply
execution-start success
</code></pre>

<hr/>

<h2 id="producer-obligations">21. Producer Obligations</h2>

<p>
A producer claiming this execution contract MUST:
</p>

<ul>
  <li>emit a backend contract consistent with the Native CPU LLVM profile,</li>
  <li>identify the execution unit explicitly,</li>
  <li>identify the execution mode explicitly,</li>
  <li>identify the state-instantiation posture explicitly where state exists,</li>
  <li>identify required host-service assumptions explicitly,</li>
  <li>reject unsupported execution-start conditions rather than silently inventing semantics,</li>
  <li>avoid implying that one private ABI or runtime model is already standardized if it is not.</li>
</ul>

<hr/>

<h2 id="consumer-obligations">22. Consumer Obligations</h2>

<p>
A consumer claiming conformance with this execution contract MUST:
</p>

<ul>
  <li>honor the backend contract assumptions it accepts,</li>
  <li>honor the explicit execution mode it accepts,</li>
  <li>materialize initialization consistently with explicit state requirements,</li>
  <li>avoid rewriting preserved semantic distinctions at execution intake,</li>
  <li>reject unsupported assumptions explicitly rather than silently degrading meaning,</li>
  <li>keep private runtime realization downstream from standardized handoff surfaces.</li>
</ul>

<p>
A consumer MAY still choose:
</p>

<ul>
  <li>its optimizer sequence,</li>
  <li>its scheduler implementation,</li>
  <li>its ABI strategy,</li>
  <li>its AOT or JIT realization strategy,</li>
  <li>its deployment packaging,</li>
  <li>its choice between a full runtime host, a generated launcher, static linkage, dynamic linkage, or another explicit support-module arrangement,</li>
</ul>

<p>
as long as those choices do not violate preserved upstream commitments.
</p>

<hr/>

<h2 id="conformance-reading">23. Conformance Reading</h2>

<p>
This document is intended to support future executable conformance growth.
</p>

<p>
A future executable conformance family may read cases through a corridor such as:
</p>

<pre><code>valid/executable/...
   source accepted
   -&gt;
   IR preserved
   -&gt;
   lowering accepted
   -&gt;
   backend contract accepted
   -&gt;
   execution contract satisfied
   -&gt;
   observable execution outcome matches expectation
</code></pre>

<p>
Negative executable families may distinguish:
</p>

<ul>
  <li>language-valid but profile-rejected,</li>
  <li>profile-valid but execution-contract-rejected,</li>
  <li>startup-fault cases,</li>
  <li>runtime-fault cases where a published truth surface later justifies them.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specification-layers">24. Relation with Other Specification Layers</h2>

<p>
This document remains subordinate to the rest of the published architecture:
</p>

<ul>
  <li><code>Expression/</code> still owns source form,</li>
  <li><code>Language/</code> still owns validated meaning,</li>
  <li><code>Libraries/</code> still own intrinsic primitive law,</li>
  <li><code>IR/</code> still owns canonical execution-facing representation, derivation, schema, lowering, and generic backend handoff,</li>
  <li><code>Profiles/</code> only owns the optional execution-start closure stated here for one bounded profile corridor.</li>
</ul>

<p>
This is critical:
</p>

<pre><code>Native CPU LLVM execution contract
   is a profile-side execution-start closure

it is not
the language core

it is not
the canonical IR

it is not
the generic backend contract

it is not
the hidden definition of runtime realization for all FROG programs
</code></pre>

<hr/>

<h2 id="future-evolution">25. Future Evolution</h2>

<p>
Later revisions may extend this execution contract with more detailed treatment of:
</p>

<ul>
  <li>declared ABI families,</li>
  <li>deployment-mode taxonomies,</li>
  <li>real-time-oriented execution contracts,</li>
  <li>interop-heavy execution corridors,</li>
  <li>runtime-mediated UI services,</li>
  <li>multi-thread or partition-aware execution contracts,</li>
  <li>embedded or reduced-service execution profiles.</li>
</ul>

<p>
Any such extension MUST preserve the repository-wide rule:
</p>

<pre><code>open language and IR truth upstream
private realization downstream
</code></pre>

<p>
Future growth SHOULD also remain disciplined:
</p>

<pre><code>close one execution-ready corridor clearly
before opening many partially specified runtime surfaces
</code></pre>

<hr/>

<h2 id="summary">26. Summary</h2>

<p>
This document closes one specific missing question in the Native CPU LLVM corridor:
</p>

<pre><code>When may a bounded accepted FROG program,
already lowered and backend-contract-emitted,
actually be considered ready to begin execution?
</code></pre>

<p>
Its answer is conservative:
</p>

<ul>
  <li>only for a bounded accepted subset,</li>
  <li>only when execution-unit assumptions are explicit,</li>
  <li>only when lifecycle and initialization assumptions are explicit,</li>
  <li>only when host-service assumptions are explicit,</li>
  <li>only when producer and consumer both honor the preserved corridor commitments,</li>
  <li>and without pretending that the whole future runtime ecosystem is already standardized.</li>
</ul>

<p>
This document therefore closes:
</p>

<pre><code>backend-consumable under profile
   -&gt;
execution-start ready under explicit assumptions
</code></pre>

<p>
without collapsing:
</p>

<pre><code>generic backend handoff
   into
profile-side execution-start closure

or

profile-side execution-start closure
   into
private runtime realization
</code></pre>

<p>
That is the intended role of the Native CPU LLVM execution contract in FROG v0.1.
</p>
