<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IR Backend Contract</h1>

<p align="center">
  <strong>Normative consumption contract for lowered execution forms downstream from the canonical Execution IR Document in FROG v0.1</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#boundary-contract">2. Boundary Contract</a></li>
  <li><a href="#position-in-the-pipeline">3. Position in the Pipeline</a></li>
  <li><a href="#scope">4. Scope</a></li>
  <li><a href="#non-goals">5. Non-Goals</a></li>
  <li><a href="#core-definition">6. Core Definition</a></li>
  <li><a href="#contract-parties">7. Contract Parties</a></li>
  <li><a href="#backend-family-target-profile-deployment-mode-and-runtime-boundary">8. Backend Family, Target Profile, Deployment Mode, and Runtime Boundary</a></li>
  <li><a href="#contract-identity-and-backend-family">9. Contract Identity and Backend Family</a></li>
  <li><a href="#contract-boundary">10. Contract Boundary</a></li>
  <li><a href="#minimum-preconditions">11. Minimum Preconditions</a></li>
  <li><a href="#required-contract-content">12. Required Contract Content</a></li>
  <li><a href="#producer-obligations">13. Producer Obligations</a></li>
  <li><a href="#consumer-obligations">14. Consumer Obligations</a></li>
  <li><a href="#preservation-invariants">15. Preservation Invariants</a></li>
  <li><a href="#control-state-and-boundaries">16. Control, State, and Boundaries</a></li>
  <li><a href="#ui-participation">17. UI Participation</a></li>
  <li><a href="#diagnostics-observability-and-fault-attribution">18. Diagnostics, Observability, and Fault Attribution</a></li>
  <li><a href="#relation-with-libraries-profiles-conformance-and-compiler-families">19. Relation with Libraries, Profiles, Conformance, and Compiler Families</a></li>
  <li><a href="#minimal-conceptual-shape">20. Minimal Conceptual Shape</a></li>
  <li><a href="#contract-lifecycle">21. Contract Lifecycle</a></li>
  <li><a href="#out-of-scope-for-v01">22. Out of Scope for v0.1</a></li>
  <li><a href="#summary">23. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the backend contract for FROG v0.1.
</p>

<p>
The backend contract is the first standardized consumer-facing handoff after lowering.
It defines what a later compiler stage, backend stage, execution-preparation stage, deployment-preparation stage, or runtime intake stage may rely on when consuming a lowered FROG execution form.
</p>

<p>
It exists to keep the following layers distinct:
</p>

<ul>
  <li>the canonical Execution IR Document, which remains open, inspectable, and source-attributable,</li>
  <li>the specialization space of lowering,</li>
  <li>the standardized consumer-facing handoff,</li>
  <li>the private realization that follows.</li>
</ul>

<pre><code>validated program meaning
        |
        v
canonical Execution IR Document
        |
        v
lowering
        |
        v
backend contract
        |
        v
private realization</code></pre>

<p>
The backend contract is therefore:
</p>

<ul>
  <li>not the canonical Execution IR Document itself,</li>
  <li>not the whole lowering space,</li>
  <li>not one private runtime representation,</li>
  <li>not one vendor ABI,</li>
  <li>not one mandatory executable artifact format.</li>
</ul>

<hr/>

<h2 id="boundary-contract">2. Boundary Contract</h2>

<p>
The backend contract is the standardized declaration by which a lowered execution form says:
</p>

<ul>
  <li>what executable content is being offered for consumption,</li>
  <li>which backend-facing assumptions are already fixed,</li>
  <li>which capabilities are required,</li>
  <li>which execution-relevant distinctions remain explicit or encoded,</li>
  <li>which source-aligned attribution obligations still apply,</li>
  <li>which later-stage freedoms remain available to the consumer,</li>
  <li>which situations must be rejected rather than silently reinterpreted.</li>
</ul>

<p>
It is therefore a consumption contract, not merely a lowered graph dump.
In practical terms, the contract says:
</p>

<pre><code>"You may consume this lowered form
under these assumptions,
with these preserved meanings,
and with these remaining obligations."</code></pre>

<p>
A backend contract is not necessarily identical to the lowered artifact itself.
A backend contract may be:
</p>

<ul>
  <li>embedded in a lowered artifact,</li>
  <li>stored alongside it,</li>
  <li>represented through a dedicated contract payload,</li>
  <li>represented through another equivalent explicit mechanism.</li>
</ul>

<p>
What matters is not one transport syntax.
What matters is the presence of a standardized, explicit consumer-facing handoff downstream from the canonical open IR boundary.
</p>

<hr/>

<h2 id="position-in-the-pipeline">3. Position in the Pipeline</h2>

<p>
The intended architecture is:
</p>

<pre><code>canonical source (.frog)
        |
        v
validated program meaning
        |
        v
canonical Execution IR Document
  - open
  - inspectable
  - source-attributable
  - schema-checkable in canonical JSON form
  - not backend-private
        |
        v
Lowering
  - specialization begins
        |
        v
Backend Contract
  - standardized consumable handoff
        |
        v
compiler / backend / deployment / runtime realization</code></pre>

<p>
The key interpretation rule is:
</p>

<ul>
  <li>Execution IR remains the canonical open execution-facing representation.</li>
  <li>Lowering is where specialization begins.</li>
  <li>Backend Contract is where consumer assumptions become explicit.</li>
  <li>Private realization remains downstream and is not standardized here.</li>
</ul>

<p>
The backend contract therefore sits:
</p>

<ul>
  <li>after lowering has begun,</li>
  <li>after the canonical open IR boundary has already been established,</li>
  <li>before private realization takes over.</li>
</ul>

<hr/>

<h2 id="scope">4. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>what a backend contract is in FROG,</li>
  <li>what information a lowered producer MUST provide to later stages,</li>
  <li>what a backend consumer MAY assume,</li>
  <li>what a backend consumer MUST preserve or reject,</li>
  <li>what remains attributable across the handoff,</li>
  <li>which architectural categories MUST remain explicit at the contract boundary.</li>
</ul>

<p>
This document owns the standardized consumable handoff boundary.
</p>

<p>
It does not own:
</p>

<ul>
  <li>the full lowering transformation space,</li>
  <li>the full private runtime realization,</li>
  <li>the full observability protocol,</li>
  <li>the full debugger protocol,</li>
  <li>the full deployment artifact model.</li>
</ul>

<hr/>

<h2 id="non-goals">5. Non-Goals</h2>

<p>
This document does not define:
</p>

<ul>
  <li>one mandatory serialized artifact format,</li>
  <li>one mandatory ABI,</li>
  <li>one mandatory scheduler representation,</li>
  <li>one mandatory bytecode format,</li>
  <li>one mandatory JIT interface,</li>
  <li>one mandatory deployment package format,</li>
  <li>one mandatory debugging protocol,</li>
  <li>one mandatory runtime-private state object model,</li>
  <li>one mandatory runtime-module layout,</li>
  <li>one mandatory LLVM-only contract shape.</li>
</ul>

<p>
It also does not redefine:
</p>

<ul>
  <li>canonical source,</li>
  <li>validated program meaning,</li>
  <li>the canonical open Execution IR Document,</li>
  <li>the full lowering transformation space.</li>
</ul>

<p>
This document standardizes the handoff boundary, not the entire world downstream from it.
</p>

<hr/>

<h2 id="core-definition">6. Core Definition</h2>

<p>
A backend contract is the explicit standardized statement that a lowered FROG execution form is now ready for downstream consumption under declared assumptions.
</p>

<p>
A backend contract therefore defines:
</p>

<ul>
  <li>the lowered executable scope being handed off,</li>
  <li>the backend family or families the handoff targets,</li>
  <li>the target-facing assumptions already fixed,</li>
  <li>the categories that remain semantically significant at the handoff,</li>
  <li>the producer commitments that the consumer may rely on,</li>
  <li>the consumer obligations that still apply,</li>
  <li>the rejection conditions if assumptions are violated.</li>
</ul>

<p>
Compactly:
</p>

<pre><code>backend contract
   =
standardized handoff
for
consuming lowered executable meaning
without guessing what lowering already fixed</code></pre>

<hr/>

<h2 id="contract-parties">7. Contract Parties</h2>

<p>
The contract has two architectural parties:
</p>

<ul>
  <li><strong>Producer</strong> — the stage that emits the lowered execution form and the associated backend contract,</li>
  <li><strong>Consumer</strong> — the later stage that consumes the contract in order to produce machine code, bytecode, deployment artifacts, runtime intake structures, or another downstream realization artifact.</li>
</ul>

<p>
These parties are architectural roles.
They are not tied to one vendor, one executable, or one repository layout.
</p>

<p>
A producer MAY be:
</p>

<ul>
  <li>a compiler front-stage,</li>
  <li>a lowering pipeline,</li>
  <li>a profile-specific specialization stage,</li>
  <li>a deployment-preparation stage.</li>
</ul>

<p>
A consumer MAY be:
</p>

<ul>
  <li>a native backend,</li>
  <li>an LLVM-oriented compiler stage,</li>
  <li>a VM-oriented consumer,</li>
  <li>a runtime loader,</li>
  <li>a deployment packager,</li>
  <li>a hybrid execution-preparation stage.</li>
</ul>

<p>
The producer/consumer distinction matters because the contract exists precisely to prevent hidden assumptions between them.
</p>

<hr/>

<h2 id="backend-family-target-profile-deployment-mode-and-runtime-boundary">8. Backend Family, Target Profile, Deployment Mode, and Runtime Boundary</h2>

<p>
These four concepts MUST remain distinct:
</p>

<ul>
  <li><strong>Backend family</strong> — the family of downstream consumer architecture, such as native-code-oriented, VM-oriented, interpreter-oriented, accelerator-oriented, or other backend families.</li>
  <li><strong>Target profile</strong> — the FROG profile-level capability surface and target-facing expectations recognized by the specification stack.</li>
  <li><strong>Deployment mode</strong> — how the produced artifact is intended to be packaged, delivered, started, embedded, or hosted.</li>
  <li><strong>Runtime boundary</strong> — the point where private realization begins and standardized handoff ends.</li>
</ul>

<p>
These are related but not interchangeable.
</p>

<pre><code>backend family
   != target profile

backend family
   != deployment mode

deployment mode
   != runtime-private realization

runtime boundary
   != canonical IR boundary</code></pre>

<p>
A backend contract MAY mention all four.
It MUST NOT collapse them into one ambiguous target label.
</p>

<p>
Deployment mode is especially important for the runtime boundary.
A contract may support a full-runtime-hosted deployment, a generated launcher, a self-contained executable, a dynamically linked package, a statically linked package, or another explicit deployment posture.
Those postures are deployment choices, not language semantics.
</p>

<p>
When a deployment does not require a complete general-purpose runtime, the contract SHOULD make the selected capability closure explicit:
which compiled program scope is present, which runtime or host services are required, which external libraries or symbols are required, which UI/widget host capabilities are required, and which assets must travel with the artifact.
This allows a downstream consumer to produce a program-specific launcher or executable without smuggling hidden runtime assumptions into the language.
</p>

<hr/>

<h2 id="contract-identity-and-backend-family">9. Contract Identity and Backend Family</h2>

<p>
A backend contract SHOULD carry explicit identity sufficient to answer:
</p>

<ul>
  <li>which lowered executable scope it describes,</li>
  <li>which producer emitted it,</li>
  <li>which backend family it targets,</li>
  <li>which versioned contract semantics apply,</li>
  <li>which canonical or lowered artifact it is associated with.</li>
</ul>

<p>
This does not require one mandatory identifier syntax.
It does require that a consumer not be forced to guess which contract semantics apply.
</p>

<p>
A backend-family declaration MUST be explicit enough to prevent ambiguity such as:
</p>

<ul>
  <li>native LLVM-oriented consumer versus interpreter consumer,</li>
  <li>CPU-native consumer versus accelerator-oriented consumer,</li>
  <li>backend family versus deployment mode confusion.</li>
</ul>

<hr/>

<h2 id="contract-boundary">10. Contract Boundary</h2>

<p>
The backend contract is the first place where lowered producer assumptions become explicit for downstream consumption.
</p>

<p>
It is therefore the boundary where the producer says:
</p>

<ul>
  <li>what is already fixed,</li>
  <li>what remains flexible,</li>
  <li>what category distinctions remain semantically relevant,</li>
  <li>what the consumer must preserve,</li>
  <li>what the consumer must reject if it cannot honor the contract.</li>
</ul>

<p>
The backend contract MUST NOT be read as:
</p>

<ul>
  <li>the definition of FROG semantics,</li>
  <li>a replacement for the canonical open IR,</li>
  <li>a license to reinterpret invalid upstream material,</li>
  <li>a hidden runtime-private implementation dump.</li>
</ul>

<p>
The contract boundary is therefore:
</p>

<pre><code>explicit
consumer-facing
specialization boundary</code></pre>

<p>
and not:
</p>

<pre><code>the point where truth becomes private by default</code></pre>

<hr/>

<h2 id="minimum-preconditions">11. Minimum Preconditions</h2>

<p>
A conforming backend contract in base v0.1 requires, at minimum:
</p>

<ul>
  <li>a semantically accepted program,</li>
  <li>a canonical Execution IR Document that was valid for the accepted subset,</li>
  <li>a lowered form that remained semantically faithful,</li>
  <li>preserved attribution and recoverability up to the contract boundary where still required,</li>
  <li>explicit declaration of backend-facing assumptions relevant to the consumer.</li>
</ul>

<p>
A backend contract MUST NOT be emitted from:
</p>

<ul>
  <li>raw source alone,</li>
  <li>merely loadable source,</li>
  <li>merely structurally valid source,</li>
  <li>an IR that already lost required category distinctions before lowering finished,</li>
  <li>a silently repaired invalid construct.</li>
</ul>

<pre><code>no validated meaning
   -&gt; no conforming backend contract

no canonical open IR
   -&gt; no conforming backend contract

no semantically faithful lowering
   -&gt; no conforming backend contract</code></pre>

<hr/>

<h2 id="required-contract-content">12. Required Contract Content</h2>

<p>
A backend contract MUST expose enough information for a consumer to decide whether it can consume the lowered form without guessing.
</p>

<p>
In base v0.1, that required content SHOULD include, as applicable:
</p>

<ul>
  <li>contract identity,</li>
  <li>producer identity or equivalent producer declaration,</li>
  <li>contract version or contract semantics version,</li>
  <li>backend family declaration,</li>
  <li>lowered executable scope,</li>
  <li>backend artifact kind declarations where artifacts are produced,</li>
  <li>ABI profile declarations where a native or foreign-call boundary is fixed,</li>
  <li>capability requirements,</li>
  <li>provider requirements and provider slots where a call is provider-backed,</li>
  <li>selected runtime or host-service requirements where execution depends on them,</li>
  <li>external dependency declarations such as native libraries, shared libraries, static libraries, symbols, or equivalent callable artifacts,</li>
  <li>package-relative artifact and dependency references where packaging is part of the handoff,</li>
  <li>type or representation commitments already fixed,</li>
  <li>storage or memory commitments already fixed,</li>
  <li>effect / call / service boundary commitments already fixed,</li>
  <li>control and state semantics that remain relevant to the consumer,</li>
  <li>required preserved category distinctions,</li>
  <li>mapping or attribution anchors still required for diagnostics or recoverability,</li>
  <li>rejection conditions.</li>
</ul>

<p>
This does not force one serialized syntax.
It does force one explicit consumer-facing content surface.
</p>

<h3>Manifest-Compatible Contract Surface</h3>

<p>
A backend contract may be carried directly by a lowered artifact, stored beside
that artifact, or expressed as a manifest-compatible handoff. FROG does not
require one manifest syntax here, but the contract surface must be explicit
enough for a runtime host, generated launcher, packager, or backend consumer
to validate compatibility without guessing.
</p>

<p>
Where applicable, a manifest-compatible handoff should make the following
facts explicit:
</p>

<ul>
  <li>the source, FIR, lowering, and backend artifact references needed for attribution and compatibility checks,</li>
  <li>target, platform, architecture, backend family, and ABI profile assumptions,</li>
  <li>produced native artifacts, bytecode artifacts, interpreted artifacts, or other backend products,</li>
  <li>entrypoint descriptors and lifecycle hooks that a consumer may call,</li>
  <li>ownership, memory, buffer, and release-policy commitments already fixed at the backend boundary,</li>
  <li>provider slots and abstract provider requirements for provider-backed calls,</li>
  <li>host capability requirements such as filesystem, clock, process, UI host, or provider access,</li>
  <li>dependency-closure metadata for packaging, linking, loading, or rejection diagnostics.</li>
</ul>

<p>
The backend contract must not become a private runtime recipe. It declares
what a consumer may rely on and what it must reject; it does not standardize
Graiphic-private loader internals, scheduler internals, optimizer choices, or
runtime object tables.
</p>

<hr/>

<h2 id="producer-obligations">13. Producer Obligations</h2>

<p>
A producer emitting a backend contract MUST:
</p>

<ul>
  <li>emit the contract only after semantically faithful lowering,</li>
  <li>state backend-facing assumptions explicitly,</li>
  <li>state the consumer-relevant preserved distinctions explicitly or equivalently,</li>
  <li>state required capabilities explicitly,</li>
  <li>state rejection conditions explicitly,</li>
  <li>preserve enough mapping continuity for diagnostics and downstream fault attribution where still required,</li>
  <li>avoid presenting backend-private convenience as though it were canonical language truth.</li>
</ul>

<p>
A producer MUST NOT:
</p>

<ul>
  <li>hide unresolved semantic ambiguity behind a contract,</li>
  <li>silently repair invalid upstream content,</li>
  <li>erase required distinctions and hope the consumer can reconstruct them privately,</li>
  <li>emit a contract that leaves consumer obligations guess-based.</li>
</ul>

<hr/>

<h2 id="consumer-obligations">14. Consumer Obligations</h2>

<p>
A consumer receiving a backend contract MAY assume only what the contract makes explicit or what the referenced specification layers already define.
</p>

<p>
A consumer MUST:
</p>

<ul>
  <li>respect the declared backend family and capability assumptions,</li>
  <li>respect preserved category distinctions that remain semantically relevant at the contract boundary,</li>
  <li>reject the contract if it cannot honor a required assumption,</li>
  <li>avoid silently reinterpreting ambiguous or unsupported contract content,</li>
  <li>avoid back-projecting its own private realization choices as though they were required by FROG itself.</li>
</ul>

<p>
A consumer MUST NOT:
</p>

<ul>
  <li>treat unspecified freedoms as guaranteed freedoms,</li>
  <li>treat omitted semantics as permission to invent new semantics,</li>
  <li>erase required attribution or diagnostics anchors if the contract still requires them,</li>
  <li>reinterpret public-interface, state, structure, UI, or effect categories in ways that contradict the contract.</li>
</ul>

<h3>Runtime or Launcher Consumption Without LLVM Identity</h3>

<p>
A runtime host, generated launcher, packager, or executable wrapper consumes
the backend contract and any referenced artifacts. It should not be required
to parse LLVM IR or link against LLVM merely because one backend family used
LLVM-oriented tooling earlier in the pipeline.
</p>

<p>
In a native deployment posture, the consumer validates the declared artifact
kinds, ABI profile, entrypoints, provider requirements, host capabilities, and
dependency closure. LLVM may have been part of producing those artifacts, but
LLVM is not the FROG runtime identity and not the public definition of FIR.
</p>

<hr/>

<h2 id="preservation-invariants">15. Preservation Invariants</h2>

<p>
The backend contract is downstream from lowering, but it is not a truth-erasure boundary by default.
</p>

<p>
At the contract boundary, the following invariants MUST still hold where applicable:
</p>

<ul>
  <li>semantic faithfulness,</li>
  <li>explicit state remains explicit state,</li>
  <li>required control and boundary semantics remain preserved,</li>
  <li>required UI-related category distinctions remain preserved,</li>
  <li>required attribution and recoverability remain preserved to the extent still declared at the contract boundary,</li>
  <li>consumer assumptions are explicit, not inferred from private conventions.</li>
</ul>

<p>
Compactly:
</p>

<pre><code>backend contract
   may narrow consumer freedoms

backend contract
   must not erase required truth silently</code></pre>

<hr/>

<h2 id="control-state-and-boundaries">16. Control, State, and Boundaries</h2>

<p>
At the backend contract boundary, the producer MUST preserve enough information so that the consumer does not have to guess critical execution categories.
</p>

<p>
This includes, where relevant:
</p>

<ul>
  <li>structured control obligations,</li>
  <li>state participation obligations,</li>
  <li>initialization obligations,</li>
  <li>effect boundaries,</li>
  <li>public boundary participation,</li>
  <li>structure-boundary participation,</li>
  <li>explicit sequencing obligations that remain relevant downstream.</li>
</ul>

<p>
The contract MUST NOT collapse:
</p>

<ul>
  <li>ordinary connectivity and explicit state participation,</li>
  <li>ordinary connectivity and explicit sequencing,</li>
  <li>public boundary participation and structure-boundary participation,</li>
  <li>explicit state semantics and scheduler-private persistence tricks.</li>
</ul>

<p>
A consumer must never be forced to infer these categories only from opaque lowered shapes if the contract still depends on them.
</p>

<p>
Likewise, a consumer must not be forced to infer dependency closure from implementation convention.
If a lowered program needs an external callable surface, a native ABI entry point, a file or image capability, a widget host service, or another support module, that requirement belongs in the contract or in an explicitly referenced profile/execution contract.
The downstream realization may then decide whether to satisfy it through a full runtime host, a shared library, a static link, a generated launcher module, or another declared mechanism.
</p>

<hr/>

<h2 id="ui-participation">17. UI Participation</h2>

<p>
UI-related participation remains especially sensitive at the contract boundary.
</p>

<p>
Where UI-related categories remain relevant to the consumer, the contract MUST preserve the distinction between:
</p>

<ul>
  <li>public interface participation,</li>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li>standardized UI-object primitive operations,</li>
  <li>explicit UI sequencing.</li>
</ul>

<p>
A contract MAY present these through:
</p>

<ul>
  <li>explicit category fields,</li>
  <li>service-boundary declarations,</li>
  <li>call-surface declarations,</li>
  <li>other equivalent explicit means.</li>
</ul>

<p>
It MUST NOT collapse them into one vague “UI interaction” bucket if later consumption still depends on the distinction.
</p>

<hr/>

<h2 id="diagnostics-observability-and-fault-attribution">18. Diagnostics, Observability, and Fault Attribution</h2>

<p>
The backend contract SHOULD preserve enough mapping continuity so that later failures remain attributable.
</p>

<p>
This does not require a full debugger protocol here.
It does require that downstream consumers are not forced to diagnose failures on completely anonymous artifacts when the contract can still preserve meaningful anchors.
</p>

<p>
Accordingly, where relevant, the contract SHOULD support:
</p>

<ul>
  <li>diagnostic mapping to lowered units or partitions,</li>
  <li>fault attribution to preserved state, control, UI, or call categories,</li>
  <li>traceability toward upstream canonical IR or validated contributors where still required,</li>
  <li>distinction between contract rejection and downstream realization failure.</li>
</ul>

<p>
This discipline is especially important for an industrial compiler corridor, because backend failures must not become uninterpretable merely because the contract boundary was underspecified.
</p>

<hr/>

<h2 id="relation-with-libraries-profiles-conformance-and-compiler-families">19. Relation with Libraries, Profiles, Conformance, and Compiler Families</h2>

<p>
The backend contract is downstream from Libraries, Profiles, and the canonical IR stack, but it must still remain compatible with them.
</p>

<p>
Accordingly:
</p>

<ul>
  <li><code>Libraries/</code> still own intrinsic primitive identity and primitive-local meaning,</li>
  <li><code>Profiles/</code> still own optional capability surfaces and target-facing capability families,</li>
  <li><code>Conformance/</code> still owns the public accept / reject / preserve truth surface,</li>
  <li>compiler families such as LLVM-oriented native backends remain downstream consumers.</li>
</ul>

<p>
The backend contract therefore does not replace these layers.
It carries their downstream-consumable consequences after lowering.
</p>

<p>
This matters especially for compiler families:
</p>

<pre><code>FROG
   -&gt;
validated meaning
   -&gt;
canonical Execution IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
LLVM-oriented or other backend-family consumer</code></pre>

<p>
The contract may support an LLVM-oriented route.
LLVM still does not become the definition of FROG.
</p>

<hr/>

<h2 id="minimal-conceptual-shape">20. Minimal Conceptual Shape</h2>

<p>
A minimal conceptual backend contract may be thought of as carrying categories such as:
</p>

<pre><code>backend contract
├── contract identity
├── version / semantics version
├── producer declaration
├── backend family declaration
├── executable scope declaration
├── capability requirements
├── lowered representation commitments
├── control / state / effect commitments
├── preserved category distinctions
├── mapping / attribution anchors where required
└── rejection conditions</code></pre>

<p>
This is illustrative only.
It does not impose one serialized payload syntax.
It makes explicit the minimal conceptual surface a consumer should not have to guess.
</p>

<hr/>

<h2 id="contract-lifecycle">21. Contract Lifecycle</h2>

<p>
A backend contract typically passes through the following lifecycle:
</p>

<pre><code>canonical IR exists
   -&gt;
lowering fixes backend-relevant assumptions
   -&gt;
producer emits backend contract
   -&gt;
consumer validates compatibility
   -&gt;
consumer accepts or rejects
   -&gt;
accepted contract drives downstream realization</code></pre>

<p>
This lifecycle matters because rejection is a first-class outcome.
A contract is not useful if unsupported assumptions are silently ignored.
</p>

<p>
A conforming consumer SHOULD reject explicitly when:
</p>

<ul>
  <li>required capabilities are unsupported,</li>
  <li>required dependency or host-service declarations cannot be resolved,</li>
  <li>backend family is mismatched,</li>
  <li>preserved category requirements cannot be honored,</li>
  <li>contract version semantics are unsupported,</li>
  <li>required control, state, or effect commitments are incompatible with the consumer.</li>
</ul>

<hr/>

<h2 id="out-of-scope-for-v01">22. Out of Scope for v0.1</h2>

<p>
The following are out of scope for this document in base v0.1:
</p>

<ul>
  <li>one mandatory binary contract format,</li>
  <li>one mandatory ABI surface,</li>
  <li>one mandatory LLVM contract shape,</li>
  <li>one mandatory runtime-loader protocol,</li>
  <li>one mandatory deployment-package specification,</li>
  <li>one mandatory debug-info transport format,</li>
  <li>one mandatory scheduler artifact model,</li>
  <li>one mandatory cross-process or cross-device transport scheme.</li>
</ul>

<p>
These may be specified later through additional documents, profiles, implementation workspaces, or backend-family-specific material.
They do not weaken the current handoff-boundary obligations.
</p>

<hr/>

<h2 id="summary">23. Summary</h2>

<p>
This document defines the backend contract for FROG v0.1 as the first standardized consumer-facing handoff after lowering.
</p>

<p>
Its core rules are:
</p>

<ul>
  <li>the backend contract is downstream from canonical open IR and lowering,</li>
  <li>it makes consumer assumptions explicit,</li>
  <li>it defines what the consumer may rely on,</li>
  <li>it preserves required semantic distinctions and obligations at the handoff boundary,</li>
  <li>it does not replace language truth, canonical IR, or private runtime realization,</li>
  <li>it enables serious downstream compiler or runtime families without collapsing FROG into one of them.</li>
</ul>

<p>
A serious industrial compilation corridor therefore has the following discipline:
</p>

<pre><code>.frog
   -&gt;
validated meaning
   -&gt;
canonical FROG Execution IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
LLVM-oriented or other downstream consumer
   -&gt;
private realization</code></pre>

<p>
The backend contract is the explicit handoff that makes this downstream corridor robust.
It allows specialization to become consumable without letting private compiler or runtime assumptions replace the open language-owned truth surface upstream.
</p>
