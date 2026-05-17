<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IR Lowering</h1>

<p align="center">
  <strong>Normative lowering boundary and specialization rules downstream from the canonical Execution IR Document in FROG v0.1</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#reading-legend">2. Reading Legend</a></li>
  <li><a href="#why-this-document-exists">3. Why This Document Exists</a></li>
  <li><a href="#position-in-the-pipeline">4. Position in the Pipeline</a></li>
  <li><a href="#scope">5. Scope</a></li>
  <li><a href="#non-goals">6. Non-Goals</a></li>
  <li><a href="#what-lowering-means-in-frog">7. What Lowering Means in FROG</a></li>
  <li><a href="#lowering-boundary">8. Lowering Boundary</a></li>
  <li><a href="#minimum-preconditions">9. Minimum Preconditions</a></li>
  <li><a href="#lowering-stages">10. Lowering Stages</a></li>
  <li><a href="#core-invariants">11. Core Invariants</a></li>
  <li><a href="#required-preservation-obligations">12. Required Preservation Obligations</a></li>
  <li><a href="#allowed-lowering-transformations">13. Allowed Lowering Transformations</a></li>
  <li><a href="#forbidden-lowering-transformations">14. Forbidden Lowering Transformations</a></li>
  <li><a href="#control-structures-and-regions">15. Control Structures and Regions</a></li>
  <li><a href="#state-memory-and-feedback">16. State, Memory, and Feedback</a></li>
  <li><a href="#interfaces-ui-boundaries-and-object-interaction">17. Interfaces, UI Boundaries, and Object Interaction</a></li>
  <li><a href="#data-representation-storage-and-layout">18. Data Representation, Storage, and Layout</a></li>
  <li><a href="#scheduling-placement-and-partitioning">19. Scheduling, Placement, and Partitioning</a></li>
  <li><a href="#attribution-and-mapping-across-lowering">20. Attribution and Mapping Across Lowering</a></li>
  <li><a href="#relation-with-backend-families-and-llvm-oriented-paths">21. Relation with Backend Families and LLVM-Oriented Paths</a></li>
  <li><a href="#relation-with-observation-debugging-and-diagnostics">22. Relation with Observation, Debugging, and Diagnostics</a></li>
  <li><a href="#conceptual-lowered-products">23. Conceptual Lowered Products</a></li>
  <li><a href="#minimal-conceptual-shape">24. Minimal Conceptual Shape</a></li>
  <li><a href="#examples">25. Examples</a></li>
  <li><a href="#out-of-scope-for-v01">26. Out of Scope for v0.1</a></li>
  <li><a href="#summary">27. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the normative architectural boundary called lowering in FROG v0.1.
</p>

<p>
Lowering begins only after a canonical Execution IR Document has been derived and constructed from validated FROG program meaning.
Lowering exists to transform that canonical open IR into forms that are more target-oriented, backend-oriented, deployment-oriented, or realization-oriented without redefining:
</p>

<ul>
  <li>canonical source,</li>
  <li>language semantics,</li>
  <li>the canonical open execution-facing role of the Execution IR,</li>
  <li>the architectural distinction between open representation and private realization.</li>
</ul>

<p>
In compact form:
</p>

<pre><code>validated meaning
      |
      v
canonical Execution IR Document
      |
      v
lowering
      |
      v
specialized lowered form(s)
      |
      v
backend-facing contract
      |
      v
private realization</code></pre>

<p>
This document does not standardize one universal compiler pipeline.
It defines what kind of transformation space belongs to lowering, what lowering MUST preserve, what lowering MAY specialize, and what lowering MUST NOT silently erase.
</p>

<hr/>

<h2 id="reading-legend">2. Reading Legend</h2>

<ul>
  <li>Open specification-facing representation or layer</li>
  <li>Semantic truth, attribution, or recoverability obligation</li>
  <li>Boundary, mapping, schema-visible identity carrier, or standardized handoff</li>
  <li>Lowering / specialization / target adaptation zone</li>
  <li>Implementation-private or runtime-private realization zone</li>
</ul>

<hr/>

<h2 id="why-this-document-exists">3. Why This Document Exists</h2>

<p>
Without a dedicated lowering boundary, repositories tend to accumulate architectural confusion such as:
</p>

<ul>
  <li>treating the canonical open IR as though it were already a target-specific graph,</li>
  <li>smuggling private scheduler decisions into the normative open IR,</li>
  <li>flattening execution structure too early and losing source-faithful attribution,</li>
  <li>confusing backend contracts with lowering transformations,</li>
  <li>mixing portable execution structure with deployment-specific or ABI-specific details,</li>
  <li>confusing downstream compiler families with the definition of FROG itself.</li>
</ul>

<p>
FROG deliberately keeps these layers separate.
</p>

<p>
This document therefore exists to answer one precise question:
</p>

<pre><code>What transformations are allowed when moving from the canonical open Execution IR
toward target-specific execution preparation, while keeping the architecture sound?</code></pre>

<p>
The answer must be strong enough to support a serious industrial compilation corridor while still preserving the openness of the FROG IR.
</p>

<hr/>

<h2 id="position-in-the-pipeline">4. Position in the Pipeline</h2>

<p>
The relevant surrounding pipeline is:
</p>

<pre><code>.frog canonical source
        |
        v
Expression/
        |
        v
validated program meaning
        |
        v
canonical Execution IR Document
        |
        v
lowering
        |
        v
specialized lowered forms
        |
        v
backend contract
        |
        v
private realization</code></pre>

<p>
Key rule:
</p>

<ul>
  <li>Execution IR = canonical, open, inspectable, source-attributable execution-facing representation</li>
  <li>Lowering = specialization boundary downstream from the canonical open IR</li>
  <li>Backend contract = later explicit consumer-facing handoff</li>
  <li>Runtime = private realization</li>
</ul>

<p>
Lowering therefore begins after the canonical Execution IR Document exists and remains before backend contract emission or runtime-private execution machinery.
</p>

<hr/>

<h2 id="scope">5. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the lowering boundary,</li>
  <li>the transformation categories that belong to lowering,</li>
  <li>the invariants that lowering MUST preserve,</li>
  <li>the classes of specialization that lowering MAY perform,</li>
  <li>the classes of transformation that lowering MUST NOT perform,</li>
  <li>the relation between canonical open IR and later backend-oriented forms.</li>
</ul>

<p>
This document does not fully define:
</p>

<ul>
  <li>the canonical Execution IR object model in full,</li>
  <li>the backend contract in full,</li>
  <li>one mandatory target-specific lowered representation,</li>
  <li>one mandatory runtime representation,</li>
  <li>one mandatory compiler architecture,</li>
  <li>one mandatory ABI,</li>
  <li>one mandatory scheduler model.</li>
</ul>

<hr/>

<h2 id="non-goals">6. Non-Goals</h2>

<p>
This document does not define:
</p>

<ul>
  <li>one compiler architecture,</li>
  <li>one lowered IR format,</li>
  <li>one scheduler model,</li>
  <li>one ABI,</li>
  <li>one runtime representation,</li>
  <li>one LLVM-only lowering route,</li>
  <li>one mandatory backend-family taxonomy.</li>
</ul>

<p>
It also does not grant permission to treat backend-private choices as though they were language-level truth.
</p>

<p>
The point of lowering is not to make the open IR disappear.
The point of lowering is to create a disciplined specialization corridor downstream from the open IR.
</p>

<hr/>

<h2 id="what-lowering-means-in-frog">7. What Lowering Means in FROG</h2>

<p>
In FROG, lowering means the transformation of the canonical Execution IR Document into forms that are closer to execution realization while still remaining semantically faithful to the validated program.
</p>

<p>
Typical lowering goals include:
</p>

<ul>
  <li>making execution costs or storage requirements more explicit,</li>
  <li>making scheduling constraints or partition boundaries more explicit,</li>
  <li>making ABI-relevant or backend-relevant call boundaries explicit,</li>
  <li>preparing control structure representation for later backend consumption,</li>
  <li>preparing state, storage, or effect representation for later backend consumption,</li>
  <li>preparing a backend-facing contract that can be consumed by a compiler family, runtime family, interpreter family, or hybrid realization path.</li>
</ul>

<p>
Lowering is therefore not simple pretty-printing and not semantic invention.
It is target-oriented specialization under preservation constraints.
</p>

<pre><code>canonical open IR
   -&gt; preserve meaning
   -&gt; preserve required attribution / recoverability
   -&gt; increase realization-oriented explicitness
   -&gt; remain upstream from backend contract and private realization</code></pre>

<hr/>

<h2 id="lowering-boundary">8. Lowering Boundary</h2>

<p>
The lowering boundary begins only after all of the following exist:
</p>

<ul>
  <li>validated program meaning,</li>
  <li>canonical Execution IR derivation,</li>
  <li>canonical Execution IR construction,</li>
  <li>a canonical open IR artifact that is attributable and recoverable.</li>
</ul>

<p>
The lowering boundary ends before or at the point where a later backend-facing contract is emitted.
</p>

<p>
Accordingly:
</p>

<pre><code>validated meaning
   -&gt; canonical IR
   -&gt; lowering
   -&gt; backend contract
   -&gt; private realization</code></pre>

<p>
Lowering MUST NOT be used as a retroactive substitute for:
</p>

<ul>
  <li>semantic validation,</li>
  <li>derivation law,</li>
  <li>identity and mapping law,</li>
  <li>canonical IR construction law.</li>
</ul>

<p>
A backend consumer may rely on lowered forms.
It MUST NOT become the place where missing upstream correctness is invented after the fact.
</p>

<hr/>

<h2 id="minimum-preconditions">9. Minimum Preconditions</h2>

<p>
A conforming lowering pipeline in base v0.1 requires, at minimum:
</p>

<ul>
  <li>a semantically accepted program,</li>
  <li>a canonical Execution IR Document that satisfies the IR-layer obligations,</li>
  <li>preserved identity and attribution sufficient for recoverability at the canonical open-IR boundary,</li>
  <li>a structurally coherent and construction-compatible open IR representation,</li>
  <li>no unresolved dependence on editor-only state for execution meaning.</li>
</ul>

<p>
Lowering MUST NOT begin from:
</p>

<ul>
  <li>raw source alone,</li>
  <li>merely loadable source,</li>
  <li>merely structurally valid source,</li>
  <li>an IR that has already lost required attribution or category distinctions,</li>
  <li>opaque runtime-private artifacts presented as though they were canonical IR.</li>
</ul>

<pre><code>no validated meaning
   -&gt; no conforming lowering

no canonical open IR
   -&gt; no conforming lowering

lost attribution before lowering
   -&gt; non-conforming upstream corridor</code></pre>

<hr/>

<h2 id="lowering-stages">10. Lowering Stages</h2>

<p>
This document does not impose one mandatory number of lowering stages.
However, it distinguishes a conceptually useful staged corridor:
</p>

<pre><code>Stage A
canonical open IR
   -&gt;
specialization-ready lowered form

Stage B
specialization-ready lowered form
   -&gt;
backend-facing lowered form

Stage C
backend-facing lowered form
   -&gt;
backend contract

Stage D
backend contract
   -&gt;
backend-family consumption / private realization</code></pre>

<p>
An implementation MAY merge stages.
An implementation MAY materialize intermediate lowered forms explicitly or implicitly.
The architectural distinction between:
</p>

<ul>
  <li>canonical open IR,</li>
  <li>lowering,</li>
  <li>backend contract,</li>
  <li>private realization</li>
</ul>

<p>
MUST remain intelligible and MUST NOT collapse into one opaque step.
</p>

<p>
In practice, the most important rule is:
</p>

<pre><code>canonical IR stays canonical

lowering specializes

backend contract hands off

runtime realizes</code></pre>

<hr/>

<h2 id="core-invariants">11. Core Invariants</h2>

<p>
All conforming lowering routes in base v0.1 MUST preserve the following invariants:
</p>

<ul>
  <li>validated program meaning remains unchanged,</li>
  <li>the distinction between canonical open IR and lowered form remains explicit,</li>
  <li>required attribution remains recoverable until the specification explicitly allows a later boundary to relax it,</li>
  <li>required category distinctions are not silently collapsed,</li>
  <li>structured control is not flattened in a way that destroys required semantics before permitted downstream boundaries,</li>
  <li>explicit state remains explicit state,</li>
  <li>public interface participation is not rewritten as UI participation,</li>
  <li><code>widget_value</code>, <code>widget_reference</code>, and standardized UI-object operations do not collapse into one undifferentiated category,</li>
  <li>ordinary connectivity is not silently rewritten as unrelated boundary participation,</li>
  <li>private scheduler or runtime policy is not retroactively treated as open IR law.</li>
</ul>

<p>
Compactly:
</p>

<pre><code>lowering MAY specialize
lowering MUST NOT redefine
lowering MAY re-express
lowering MUST NOT erase required truth early</code></pre>

<hr/>

<h2 id="required-preservation-obligations">12. Required Preservation Obligations</h2>

<p>
Lowering is permitted to become more realization-oriented.
It is not permitted to become semantically lossy where the specification still requires recoverability.
</p>

<p>
Accordingly, lowering MUST preserve, at minimum:
</p>

<ul>
  <li>semantic faithfulness,</li>
  <li>attribution compatibility,</li>
  <li>identity continuity sufficient for diagnostics and mapping where still required,</li>
  <li>explicit state participation,</li>
  <li>structured-region ownership where still relevant at the lowered stage,</li>
  <li>public-boundary obligations,</li>
  <li>distinction between pure computation, effects, UI object interaction, and state participation where relevant.</li>
</ul>

<p>
Preservation does not always mean byte-for-byte structural similarity.
It means that lowering must not remove distinctions that are still architecturally required.
</p>

<pre><code>preservation
   != shape freezing

preservation
   = meaning + required distinctions + recoverable obligations
      survive the lowering boundary</code></pre>

<hr/>

<h2 id="allowed-lowering-transformations">13. Allowed Lowering Transformations</h2>

<p>
The following transformation classes are allowed when they preserve the required invariants:
</p>

<ul>
  <li>making storage classes explicit,</li>
  <li>making buffer ownership explicit,</li>
  <li>making call boundaries explicit,</li>
  <li>introducing backend-oriented support objects or support records,</li>
  <li>introducing explicit execution ordering constraints where already justified by validated meaning,</li>
  <li>preparing control forms for CFG-like or block-like downstream representations,</li>
  <li>preparing pure dataflow subgraphs for SSA-friendly downstream representations,</li>
  <li>preparing explicit memory cells, frames, or state carriers for downstream consumption,</li>
  <li>introducing placement or partition annotations for later backend-family consumption,</li>
  <li>materializing effect boundaries more explicitly,</li>
  <li>materializing selected dependency and capability boundaries for later deployment preparation,</li>
  <li>specializing data representation and layout choices,</li>
  <li>specializing calling conventions at the backend-facing side when still downstream from canonical open IR.</li>
</ul>

<p>
Allowed lowering is therefore a broad specialization space, but it remains bounded by upstream truth.
</p>

<hr/>

<h2 id="forbidden-lowering-transformations">14. Forbidden Lowering Transformations</h2>

<p>
The following are forbidden:
</p>

<ul>
  <li>changing validated program meaning,</li>
  <li>silently repairing semantically invalid constructs,</li>
  <li>inserting hidden state and then presenting it as though it had open-IR origin,</li>
  <li>erasing required attribution or correspondence too early,</li>
  <li>rewriting explicit local memory as accidental persistence,</li>
  <li>rewriting public interface participation as widget participation,</li>
  <li>collapsing <code>widget_reference</code> identity into standardized UI-object operation identity,</li>
  <li>treating backend-family convenience as language truth,</li>
  <li>treating deployment-specific partitioning as though it were canonical IR semantics,</li>
  <li>treating runtime-private scheduler decisions as though they were mandated by the open IR,</li>
  <li>using lowering to erase distinctions still required for conformance, observability, diagnostics, or backend-handoff correctness.</li>
</ul>

<pre><code>forbidden
   =
semantic drift
or
premature truth erasure
or
backend-private truth substitution</code></pre>

<hr/>

<h2 id="control-structures-and-regions">15. Control Structures and Regions</h2>

<p>
Structured control MUST remain semantically structured across lowering until a later lowered form can represent the same semantics without ambiguity.
</p>

<p>
Allowed lowering may:
</p>

<ul>
  <li>make region entry and exit more explicit,</li>
  <li>make selector, iteration, and condition roles more explicit,</li>
  <li>prepare block or CFG-oriented downstream forms,</li>
  <li>introduce explicit region frames or continuation support structure.</li>
</ul>

<p>
Forbidden lowering may not:
</p>

<ul>
  <li>erase structure ownership if region semantics are still needed,</li>
  <li>erase boundary roles and then rely on private scheduler behavior to reconstruct them,</li>
  <li>treat visual grouping as though it were sufficient structure information,</li>
  <li>replace structured semantics with opaque backend-private control glue at the canonical lowering boundary.</li>
</ul>

<pre><code>validated structured control
   -&gt; canonical IR structured control
   -&gt; lowered structured form or equivalent
   -&gt; backend consumption

never
   -&gt; semantic structure loss masked as optimization</code></pre>

<hr/>

<h2 id="state-memory-and-feedback">16. State, Memory, and Feedback</h2>

<p>
Explicit state is one of the most important lowering-sensitive areas.
</p>

<p>
Lowering MAY:
</p>

<ul>
  <li>make memory cells explicit,</li>
  <li>choose storage classes,</li>
  <li>introduce frame or state-slot representations,</li>
  <li>make initialization materialization more explicit,</li>
  <li>specialize buffer reuse or memory placement.</li>
</ul>

<p>
Lowering MUST NOT:
</p>

<ul>
  <li>turn explicit state into implicit accidental persistence,</li>
  <li>drop initialization carriers that still matter semantically,</li>
  <li>rewrite state participation into ordinary connectivity,</li>
  <li>silently change snapshot, commit, or visibility semantics already required upstream.</li>
</ul>

<p>
In compact form:
</p>

<pre><code>explicit memory
   may become
lowered memory representation

explicit memory
   must not become
hidden accidental persistence</code></pre>

<hr/>

<h2 id="interfaces-ui-boundaries-and-object-interaction">17. Interfaces, UI Boundaries, and Object Interaction</h2>

<p>
Lowering must remain especially disciplined around public boundary and UI-related families.
</p>

<p>
It MUST preserve the distinction between:
</p>

<ul>
  <li>public interface participation,</li>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li>standardized UI-object primitive operation,</li>
  <li>explicit UI sequencing.</li>
</ul>

<p>
Lowering MAY:
</p>

<ul>
  <li>make UI object interaction calls more explicit,</li>
  <li>introduce runtime-service call surfaces,</li>
  <li>separate compilable pure cores from runtime-mediated UI effect surfaces,</li>
  <li>prepare backend-facing call contracts for UI-related services.</li>
</ul>

<p>
Lowering MUST NOT:
</p>

<ul>
  <li>collapse UI categories into one undifferentiated “UI thing”,</li>
  <li>rewrite public boundary participation as UI interaction,</li>
  <li>erase explicit UI sequencing where it remains semantically relevant,</li>
  <li>pretend a runtime-service call boundary is the same thing as canonical open-IR identity.</li>
</ul>

<hr/>

<h2 id="data-representation-storage-and-layout">18. Data Representation, Storage, and Layout</h2>

<p>
One of lowering’s legitimate jobs is to choose more explicit data-representation strategies.
</p>

<p>
Lowering MAY specialize:
</p>

<ul>
  <li>scalar representation,</li>
  <li>aggregate layout,</li>
  <li>buffer materialization,</li>
  <li>ownership style,</li>
  <li>copy versus borrow style decisions,</li>
  <li>stack-like versus heap-like realization tendencies,</li>
  <li>alignment-sensitive layout constraints,</li>
  <li>representation choices needed for backend-family compatibility.</li>
</ul>

<p>
These choices remain downstream from canonical open IR.
They MUST NOT be back-projected as though they were the canonical meaning of FROG itself.
</p>

<p>
The open IR says what execution-facing structure exists.
Lowering says how that structure is prepared for a backend family.
</p>

<hr/>

<h2 id="scheduling-placement-and-partitioning">19. Scheduling, Placement, and Partitioning</h2>

<p>
Lowering MAY introduce more explicit scheduling, placement, and partition information.
This is one of the main reasons lowering exists.
</p>

<p>
Examples include:
</p>

<ul>
  <li>partitioning pure subgraphs and effectful subgraphs,</li>
  <li>separating compilable program cores from runtime-hosted, UI-hosted, or external-call support surfaces,</li>
  <li>marking host/device or core/accelerator boundaries,</li>
  <li>preparing CPU-oriented or runtime-oriented scheduling surfaces,</li>
  <li>introducing region-level execution partitions,</li>
  <li>annotating data movement or synchronization boundaries.</li>
</ul>

<p>
However, lowering MUST NOT:
</p>

<ul>
  <li>treat one scheduler choice as though it were the language semantics,</li>
  <li>replace explicit state or explicit sequencing semantics with unspecified scheduler magic,</li>
  <li>collapse backend-family placement choices into target-profile law,</li>
  <li>destroy category distinctions merely because one backend prefers a flatter representation.</li>
</ul>

<pre><code>placement / partitioning
   belongs to lowering

placement / partitioning
   does not redefine FROG semantics</code></pre>

<hr/>

<h2 id="attribution-and-mapping-across-lowering">20. Attribution and Mapping Across Lowering</h2>

<p>
Lowering remains downstream from the canonical open-IR identity boundary.
That means attribution does not stop mattering the moment lowering begins.
</p>

<p>
A conforming lowering path MUST preserve enough mapping continuity so that:
</p>

<ul>
  <li>diagnostics remain attributable,</li>
  <li>debugging and observation can still relate lowered artifacts to upstream IR where required,</li>
  <li>backend-facing consumers are not forced to guess where lowered structure came from,</li>
  <li>conformance reasoning can still distinguish specialization from semantic invention.</li>
</ul>

<p>
The exact carrier may vary by implementation.
The obligation does not.
</p>

<p>
At some later downstream boundary, a private runtime may introduce completely different handles or activation identifiers.
That downstream step MUST remain explicitly downstream from the open-IR mapping obligations.
</p>

<hr/>

<h2 id="relation-with-backend-families-and-llvm-oriented-paths">21. Relation with Backend Families and LLVM-Oriented Paths</h2>

<p>
FROG lowering is intentionally backend-family-aware but backend-family-independent at the architectural level.
</p>

<p>
This means:
</p>

<ul>
  <li>lowering MAY prepare forms that are suitable for LLVM-oriented native code generation,</li>
  <li>lowering MAY prepare forms suitable for interpreters, virtual machines, specialized runtimes, or mixed realization paths,</li>
  <li>lowering MUST NOT collapse the FROG IR into LLVM IR,</li>
  <li>lowering MUST NOT let LLVM become the definition of FROG.</li>
</ul>

<p>
A correct LLVM-oriented route is therefore:
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
LLVM-oriented backend consumption</code></pre>

<p>
An incorrect reading would be:
</p>

<pre><code>FROG IR = LLVM IR</code></pre>

<p>
FROG remains upstream, open, inspectable, and language-owned.
LLVM remains downstream, consumer-oriented, and compiler-family-specific.
</p>

<hr/>

<h2 id="relation-with-observation-debugging-and-diagnostics">22. Relation with Observation, Debugging, and Diagnostics</h2>

<p>
Lowering must remain compatible with later observation and diagnostics needs.
</p>

<p>
This does not require that every lowered artifact remain as human-readable as the canonical open IR.
It does require that lowering not destroy all mapping continuity too early.
</p>

<p>
In particular, lowering SHOULD preserve enough continuity so that:
</p>

<ul>
  <li>compiler diagnostics can still be related to upstream structure,</li>
  <li>backend failures can still be attributed to meaningful upstream artifacts,</li>
  <li>future debug-information emission remains possible,</li>
  <li>conformance analysis can still detect whether a failure came from semantic invalidity, derivation failure, lowering failure, or backend-consumer failure.</li>
</ul>

<hr/>

<h2 id="conceptual-lowered-products">23. Conceptual Lowered Products</h2>

<p>
This document does not define one mandatory lowered product.
It does define a conceptually legitimate product space.
</p>

<p>
Examples of legitimate lowered products include:
</p>

<ul>
  <li>a control-normalized lowered graph,</li>
  <li>a storage-explicit lowered graph,</li>
  <li>a partitioned lowered graph,</li>
  <li>a backend-facing lowered module,</li>
  <li>a contract-oriented lowered call surface,</li>
  <li>a pure-core / effect-surface split representation,</li>
  <li>a deployment-preparation descriptor for a selected launcher, host-service, or dependency closure.</li>
</ul>

<p>
Multiple lowered products MAY coexist in one implementation.
The important rule is that they remain clearly downstream from canonical open IR and clearly upstream from private realization where applicable.
</p>

<hr/>

<h2 id="minimal-conceptual-shape">24. Minimal Conceptual Shape</h2>

<p>
A minimal conceptual lowered form may be thought of as carrying categories such as:
</p>

<pre><code>lowered module
├── lowered units / partitions
├── lowered control carriers
├── lowered state / storage carriers
├── lowered call boundaries
├── lowered effect boundaries
├── lowered data-layout commitments
├── mapping anchors to upstream canonical IR
└── backend-preparation metadata</code></pre>

<p>
This is illustrative only.
The purpose is to make explicit that a lowered form is richer in realization-oriented detail than canonical open IR while still remaining distinct from backend contract and private runtime machinery.
</p>

<hr/>

<h2 id="examples">25. Examples</h2>

<h3>25.1 Valid lowering example — pure arithmetic subgraph</h3>

<pre><code>canonical IR pure arithmetic region
   -&gt;
SSA-friendly lowered subgraph
   -&gt;
LLVM-oriented backend contract preparation

Valid because:
- semantic meaning is preserved
- purity assumptions remain faithful
- lowering remains downstream from canonical IR</code></pre>

<h3>25.2 Valid lowering example — explicit state</h3>

<pre><code>canonical IR explicit local memory
   -&gt;
lowered state cell + initialization carrier + read/write handling
   -&gt;
backend-facing storage preparation

Valid because:
- explicit state remains explicit
- initialization remains explicit
- no hidden persistence is invented</code></pre>

<h3>25.3 Invalid lowering example — category collapse</h3>

<pre><code>canonical IR widget_reference
   +
canonical IR standardized property_write operation
   -&gt;
one opaque lowered "ui object"

Invalid because:
- widget reference participation and UI-object operation were collapsed
- required category distinctions were lost too early</code></pre>

<h3>25.4 Invalid lowering example — semantic invention</h3>

<pre><code>canonical IR with no explicit local memory
   -&gt;
lowered hidden buffer providing persistence
   -&gt;
backend contract

Invalid because:
- lowering invented hidden state
- semantic meaning changed</code></pre>

<h3>25.5 Invalid lowering example — backend truth substitution</h3>

<pre><code>canonical IR structure
   -&gt;
backend-preferred flat block set
   -&gt;
all region ownership and attribution lost
   -&gt;
consumer expected to reconstruct meaning from private scheduler assumptions

Invalid because:
- lowering erased required recoverability
- backend convenience replaced upstream truth</code></pre>

<hr/>

<h2 id="out-of-scope-for-v01">26. Out of Scope for v0.1</h2>

<p>
The following are out of scope for this document in base v0.1:
</p>

<ul>
  <li>one mandatory lowered IR syntax,</li>
  <li>one mandatory LLVM lowering sequence,</li>
  <li>one mandatory ABI surface,</li>
  <li>one mandatory memory model for every backend family,</li>
  <li>one mandatory scheduler architecture,</li>
  <li>one mandatory placement taxonomy,</li>
  <li>backend-family-specific optimization pass catalogs,</li>
  <li>runtime-private realization details,</li>
  <li>full debug-information emission standards.</li>
</ul>

<p>
These may be specified later by additional profiles, backend-family documents, implementation documents, or future IR-layer refinements.
They do not weaken the current lowering-boundary obligations.
</p>

<hr/>

<h2 id="summary">27. Summary</h2>

<p>
This document defines the normative lowering boundary downstream from the canonical Execution IR Document in FROG v0.1.
</p>

<p>
Its core rules are:
</p>

<ul>
  <li>lowering begins only after canonical open IR exists,</li>
  <li>lowering specializes but does not redefine,</li>
  <li>lowering may make realization-oriented structure explicit,</li>
  <li>lowering must preserve semantic faithfulness and required recoverability,</li>
  <li>lowering remains distinct from backend contract and from private realization,</li>
  <li>LLVM-oriented routes are valid downstream consumers, but LLVM does not define FROG.</li>
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
backend-facing contract
   -&gt;
LLVM-oriented or other downstream backend family</code></pre>

<p>
FROG remains the open language and open IR stack upstream.
Lowering is the specialization boundary that makes serious compilation possible without collapsing the language into one private compiler architecture.
</p>
