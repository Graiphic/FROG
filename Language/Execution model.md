<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Execution Model Specification</h1>

<p align="center">
  Normative execution-model concepts for validated FROG programs<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#scope-for-v01">4. Scope for v0.1</a></li>
  <li><a href="#validated-executable-graph-and-live-execution-instance">5. Validated Executable Graph and Live Execution Instance</a>
    <ul>
      <li><a href="#validated-executable-graph">5.1 Validated executable graph</a></li>
      <li><a href="#live-execution-instance">5.2 Live execution instance</a></li>
      <li><a href="#instance-independence">5.3 Instance independence</a></li>
    </ul>
  </li>
  <li><a href="#source-identity">6. Source Identity</a></li>
  <li><a href="#activation-and-execution-context">7. Activation and Execution Context</a>
    <ul>
      <li><a href="#activation">7.1 Activation</a></li>
      <li><a href="#execution-context">7.2 Execution context</a></li>
      <li><a href="#illustrative-context-shape">7.3 Illustrative context shape</a></li>
    </ul>
  </li>
  <li><a href="#semantic-execution-milestones">8. Semantic Execution Milestones</a>
    <ul>
      <li><a href="#general-rule">8.1 General rule</a></li>
      <li><a href="#node-milestones">8.2 Node milestones</a></li>
      <li><a href="#edge-value-availability">8.3 Edge value availability</a></li>
      <li><a href="#structure-and-region-milestones">8.4 Structure and region milestones</a></li>
      <li><a href="#local-memory-milestones">8.5 Local-memory milestones</a></li>
      <li><a href="#sub-frog-milestones">8.6 Sub-FROG milestones</a></li>
    </ul>
  </li>
  <li><a href="#committed-source-level-state">9. Committed Source-Level State</a>
    <ul>
      <li><a href="#committed-state-definition">9.1 Definition</a></li>
      <li><a href="#committed-values-and-snapshots">9.2 Committed values and snapshots</a></li>
      <li><a href="#what-this-document-does-not-define">9.3 What this document does not define</a></li>
    </ul>
  </li>
  <li><a href="#runtime-freedom-and-observable-equivalence">10. Runtime Freedom and Observable Equivalence</a></li>
  <li><a href="#validation-rules">11. Validation Rules</a></li>
  <li><a href="#out-of-scope-for-v01">12. Out of Scope for v0.1</a></li>
  <li><a href="#summary">13. Summary</a></li>
  <li><a href="#license">14. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the core execution-model concepts of the FROG language.
It specifies the minimal language-level objects and relations needed to describe what a validated FROG means when it executes, independently of any particular IDE, runtime implementation, compiler pipeline, transport protocol, or execution target.
</p>

<p>
This document is not the canonical source specification.
It does not define how a FROG is serialized in a <code>.frog</code> file.
It also does not define IDE-facing event delivery, debugging commands, probe windows, watch views, or runtime-internal scheduler design.
</p>

<p>
Instead, this document defines the stable execution-model vocabulary that other specifications rely on.
That vocabulary includes:
</p>

<ul>
  <li>the validated executable graph,</li>
  <li>the live execution instance,</li>
  <li>source identity,</li>
  <li>activation,</li>
  <li>execution context,</li>
  <li>semantic execution milestones,</li>
  <li>committed source-level state,</li>
  <li>runtime freedom constrained by observable semantic equivalence.</li>
</ul>

<p>
These concepts form the language-level baseline on top of which observability, debugging, probes, watch behavior, runtime profiles, and future lowering layers MAY be defined without moving semantic ownership out of <code>Language/</code>.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<p>
This document has the following goals:
</p>

<ul>
  <li>define the minimal execution-model concepts that apply across validated FROG programs,</li>
  <li>give source-aligned execution meaning a normative home in <code>Language/</code>,</li>
  <li>separate language-level execution meaning from IDE-facing observation and control behavior,</li>
  <li>remain compatible with multiple runtimes, schedulers, transports, and execution targets,</li>
  <li>provide a durable semantic basis for later specifications such as execution observability, debugging boundaries, and execution profiles.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document depends on the following specifications:
</p>

<ul>
  <li><code>Readme.md</code> for the repository-wide architecture and layering,</li>
  <li><code>Language/Readme.md</code> for the ownership boundary of <code>Language/</code>,</li>
  <li><code>Language/Control structures.md</code> for the normative execution semantics of <code>case</code>, <code>for_loop</code>, and <code>while_loop</code>,</li>
  <li><code>Language/State and cycles.md</code> for the normative execution semantics of explicit local memory and valid feedback cycles,</li>
  <li><code>Expression/Diagram.md</code> for the canonical source representation of the validated executable graph,</li>
  <li><code>Expression/Control structures.md</code> for the canonical representation of structure families, regions, and structure-owned boundaries,</li>
  <li><code>Expression/State and cycles.md</code> for the canonical representation of local-memory constructs and cycle-facing source constraints,</li>
  <li><code>IDE/Execution observability.md</code> for the IDE-facing projection of execution activity,</li>
  <li><code>IDE/Debugging.md</code> for user-facing control semantics layered over language-defined execution boundaries,</li>
  <li><code>IDE/Probes.md</code> and <code>IDE/Watch.md</code> for IDE-managed inspection constructs that consume committed source-level observations.</li>
</ul>

<p>
This document MUST NOT contradict those specifications.
If a conflict appears:
</p>

<ul>
  <li><code>Expression/</code> remains authoritative for canonical source representation and source-visible identity,</li>
  <li><code>Language/</code> remains authoritative for normative execution meaning,</li>
  <li><code>Libraries/</code> remain authoritative for primitive identity, primitive-local behavior, ports, and required metadata,</li>
  <li><code>IDE/</code> remains authoritative only for IDE-facing observation, control, presentation, and tooling behavior.</li>
</ul>

<p>
This document intentionally defines execution-model concepts without defining the full pause model, safe observation boundaries, or safe debug-stop boundaries.
Those topics belong to the companion document:
<code>Language/Execution control and observation boundaries.md</code>.
</p>

<hr/>

<h2 id="scope-for-v01">4. Scope for v0.1</h2>

<p>
For FROG v0.1, this document standardizes the minimal language-level execution-model core needed by the rest of the repository.
At minimum, this includes:
</p>

<ul>
  <li>what a validated executable graph is at execution time,</li>
  <li>what a live execution instance is,</li>
  <li>how source-level execution identity is preserved,</li>
  <li>what an activation is,</li>
  <li>what an execution context is,</li>
  <li>which execution milestones exist at the semantic level,</li>
  <li>what committed source-level state means,</li>
  <li>how runtime freedom is constrained by observable semantic equivalence.</li>
</ul>

<p>
For v0.1, this document does not standardize:
</p>

<ul>
  <li>a runtime-internal scheduler architecture,</li>
  <li>a transport protocol or event-stream protocol,</li>
  <li>an IDE command model,</li>
  <li>pause, resume, or stepping commands,</li>
  <li>safe observation points or safe debug stops,</li>
  <li>a full runtime profile model,</li>
  <li>a lowering or execution-IR contract,</li>
  <li>distributed execution semantics,</li>
  <li>asynchronous execution semantics beyond the baseline model defined here.</li>
</ul>

<hr/>

<h2 id="validated-executable-graph-and-live-execution-instance">5. Validated Executable Graph and Live Execution Instance</h2>

<h3 id="validated-executable-graph">5.1 Validated executable graph</h3>

<p>
A validated executable graph is the language-level executable meaning of a FROG after:
</p>

<ul>
  <li>canonical source parsing,</li>
  <li>structural validation,</li>
  <li>type validation,</li>
  <li>primitive and structure-family validation,</li>
  <li>cycle-validity validation,</li>
  <li>all other v0.1 validation rules required by the relevant specifications.</li>
</ul>

<p>
A validated executable graph MUST be source-aligned.
Its executable objects MUST remain attributable to source-visible objects defined in the canonical FROG Expression.
</p>

<p>
A validated executable graph is not an IDE view, not a runtime trace, and not an execution log.
It is the validated language-level executable object whose meaning is then realized by one or more live executions.
</p>

<h3 id="live-execution-instance">5.2 Live execution instance</h3>

<p>
A live execution instance is one running realization of one validated executable graph.
A live execution instance owns its own dynamic execution history.
</p>

<p>
At minimum, a live execution instance conceptually includes:
</p>

<ul>
  <li>one validated executable graph as its semantic basis,</li>
  <li>its own dynamic activation history,</li>
  <li>its own dynamic execution contexts,</li>
  <li>its own local-memory state where local memory exists,</li>
  <li>its own nested sub-FROG invocation history where sub-FROG calls occur.</li>
</ul>

<p>
A live execution instance is the semantic object over which later specifications may define:
</p>

<ul>
  <li>observation,</li>
  <li>debug control,</li>
  <li>diagnostics,</li>
  <li>runtime integration,</li>
  <li>execution profiles.</li>
</ul>

<h3 id="instance-independence">5.3 Instance independence</h3>

<p>
Distinct live execution instances of the same validated executable graph MUST remain semantically independent unless a stricter future profile explicitly standardizes a shared external execution resource.
</p>

<p>
In particular, for base v0.1:
</p>

<ul>
  <li>local memory MUST NOT be implicitly shared across independent live execution instances,</li>
  <li>dynamic activation identity MUST be instance-relative,</li>
  <li>execution context MUST be interpreted relative to one specific live execution instance,</li>
  <li>committed source-level observations MUST be attributable to one specific live execution instance.</li>
</ul>

<hr/>

<h2 id="source-identity">6. Source Identity</h2>

<p>
Observable and inspectable execution meaning MUST remain attributable to stable source-level identity.
A runtime MAY use internal identifiers, compiled forms, optimized graphs, or backend-specific scheduling objects, but those implementation details MUST remain projectable back to source-level objects.
</p>

<p>
For v0.1, source-level execution identity MAY correspond to objects such as:
</p>

<ul>
  <li>a node,</li>
  <li>an edge,</li>
  <li>a structure instance,</li>
  <li>a structure-owned region,</li>
  <li>a sub-FROG call site,</li>
  <li>a local-memory primitive instance,</li>
  <li>a source-level port or terminal when a stricter profile requires that granularity.</li>
</ul>

<p>
Two distinct source-level objects MUST NOT become semantically indistinguishable merely because a runtime reuses internal scheduling or storage mechanisms.
</p>

<p>
Conversely, multiple internal runtime objects MAY correspond to one source-level object, provided that the externally meaningful execution identity remains source-aligned and semantically consistent.
</p>

<hr/>

<h2 id="activation-and-execution-context">7. Activation and Execution Context</h2>

<h3 id="activation">7.1 Activation</h3>

<p>
An activation is one dynamic occurrence of execution activity associated with a source-aligned executable object inside one live execution instance.
</p>

<p>
For v0.1, the language-level notion of activation is intentionally generic.
It applies, as relevant, to:
</p>

<ul>
  <li>a node activation,</li>
  <li>a structure activation,</li>
  <li>a region activation,</li>
  <li>a loop iteration activation,</li>
  <li>a sub-FROG invocation activation.</li>
</ul>

<p>
A source-level object MAY activate:
</p>

<ul>
  <li>zero times,</li>
  <li>exactly once,</li>
  <li>multiple times sequentially,</li>
  <li>multiple times under distinct nested dynamic scopes,</li>
  <li>multiple times across repeated structure or sub-FROG activations.</li>
</ul>

<p>
Therefore, source identity alone is not sufficient to describe dynamic execution meaning.
Dynamic activity MUST also be attributable to an execution context.
</p>

<h3 id="execution-context">7.2 Execution context</h3>

<p>
An execution context is the dynamic path under which one activation or one committed source-level observation occurs inside one live execution instance.
</p>

<p>
For v0.1, an execution context MAY include:
</p>

<ul>
  <li>the live execution instance identity,</li>
  <li>sub-FROG invocation nesting,</li>
  <li>structure nesting,</li>
  <li>the selected region identifier when a structure owns alternative regions,</li>
  <li>the current loop iteration index when applicable,</li>
  <li>the current activation identifier for the relevant executable object.</li>
</ul>

<p>
An execution context MUST be rich enough to distinguish semantically different dynamic occurrences of the same source-level object.
</p>

<p>
Accordingly, the following situations MUST remain distinguishable when they are semantically distinct:
</p>

<ul>
  <li>two different iterations of the same loop body,</li>
  <li>two different invocations of the same sub-FROG call site,</li>
  <li>activity inside a selected <code>case</code> region versus activity outside that region,</li>
  <li>caller-level activity versus callee-level activity,</li>
  <li>multiple dynamic activations of the same source node within one live execution instance.</li>
</ul>

<p>
A context MAY be represented differently by different runtimes.
Its internal encoding is not standardized here.
Its semantic role is standardized.
</p>

<h3 id="illustrative-context-shape">7.3 Illustrative context shape</h3>

<p>
The following diagram illustrates the semantic idea of execution context.
It is illustrative only.
It does not define a mandatory serialized representation.
</p>

<pre>
instance I42
└── subfrog call site: compute_gain
    └── while_loop body
        └── iteration = 3
            └── case region = "locked"
                └── node activation: n17 / activation 2
</pre>

<p>
In this example, the node identity alone is insufficient.
The full dynamic meaning depends on the instance, call path, structure path, selected region, iteration, and local activation occurrence.
</p>

<hr/>

<h2 id="semantic-execution-milestones">8. Semantic Execution Milestones</h2>

<h3 id="general-rule">8.1 General rule</h3>

<p>
This document standardizes semantic execution milestones, not an IDE event protocol.
A runtime MAY expose those milestones through events, callbacks, traces, logs, polling APIs, or equivalent mechanisms, but the language-level meaning MUST remain stable.
</p>

<p>
A stricter specification MAY define canonical observable event names or payload categories.
That later projection MUST preserve the semantic milestones defined here.
</p>

<h3 id="node-milestones">8.2 Node milestones</h3>

<p>
For a source-aligned executable node, the following semantic milestones MAY exist where relevant to the execution model or to stricter derived specifications:
</p>

<ul>
  <li>the node becomes executable or ready,</li>
  <li>the node activation begins,</li>
  <li>the node activation completes,</li>
  <li>the node activation terminates because of an execution fault.</li>
</ul>

<p>
This document does not require every runtime to expose each milestone directly.
It defines that these milestones are semantically meaningful distinctions when higher-level specifications rely on them.
</p>

<h3 id="edge-value-availability">8.3 Edge value availability</h3>

<p>
When a source-level edge receives a value that becomes semantically available to downstream execution, that availability constitutes a semantic milestone.
</p>

<p>
This milestone is language-level because it belongs to source-aligned execution meaning, even if an implementation chooses different buffer strategies, queue strategies, batching strategies, or transport strategies internally.
</p>

<h3 id="structure-and-region-milestones">8.4 Structure and region milestones</h3>

<p>
For standardized control structures, semantic milestones MAY include:
</p>

<ul>
  <li>structure activation begins,</li>
  <li>a region becomes selected,</li>
  <li>a selected region begins execution,</li>
  <li>a loop iteration begins,</li>
  <li>a loop iteration ends,</li>
  <li>the structure activation completes.</li>
</ul>

<p>
These milestones do not replace the more specific semantics defined in
<code>Language/Control structures.md</code>.
They provide the shared execution-model vocabulary needed when such activity must later be observed, debugged, or diagnosed in a source-aligned way.
</p>

<h3 id="local-memory-milestones">8.5 Local-memory milestones</h3>

<p>
For explicit local memory, semantic milestones MAY include:
</p>

<ul>
  <li>the stored state is read as part of one activation,</li>
  <li>the stored state is updated according to the primitive-local state rule,</li>
  <li>the resulting committed local-memory state becomes the new state for later relevant activations.</li>
</ul>

<p>
These milestones do not replace the normative state semantics defined in
<code>Language/State and cycles.md</code>.
They provide the execution-model vocabulary needed when local memory is later projected to observation, probes, or watches.
</p>

<h3 id="sub-frog-milestones">8.6 Sub-FROG milestones</h3>

<p>
For a <code>subfrog</code> call site, semantic milestones MAY include:
</p>

<ul>
  <li>the call-site activation begins,</li>
  <li>the callee instance or nested execution scope is entered,</li>
  <li>the callee completes,</li>
  <li>the caller-side call-site activation completes or faults accordingly.</li>
</ul>

<p>
A runtime MAY expose only caller-level visibility in a minimal profile.
A stricter profile MAY expose nested callee-level activity.
In either case, the language-level model requires that nested dynamic meaning remain attributable through execution context.
</p>

<hr/>

<h2 id="committed-source-level-state">9. Committed Source-Level State</h2>

<h3 id="committed-state-definition">9.1 Definition</h3>

<p>
Committed source-level state is the source-aligned execution state that is semantically valid to treat as an actual state of a live execution instance at the language level.
</p>

<p>
Committed source-level state exists to distinguish:
</p>

<ul>
  <li>semantic state that is already valid to interpret at the source level,</li>
  <li>internal runtime-transient state that MAY exist implementation-wise but is not yet semantically committed for source-level interpretation.</li>
</ul>

<p>
A runtime MAY use internal transient buffers, partial scheduling queues, temporary values, speculative internal states, or backend-specific staging mechanisms.
Those internal details do not automatically become committed source-level state.
</p>

<h3 id="committed-values-and-snapshots">9.2 Committed values and snapshots</h3>

<p>
For v0.1, the following are examples of what MAY become committed source-level state when semantically valid:
</p>

<ul>
  <li>a value that is source-level meaningful on an edge,</li>
  <li>a value that is source-level meaningful at a node port,</li>
  <li>a selected structure region,</li>
  <li>a completed loop iteration milestone,</li>
  <li>a local-memory state snapshot after a valid state update,</li>
  <li>another source-aligned state snapshot defined by a stricter profile.</li>
</ul>

<p>
When a later specification refers to a last-known committed observation, last-known committed value, or committed state snapshot, that wording MUST be interpreted relative to the notion defined here.
</p>

<p>
Committed source-level state is always interpreted relative to:
</p>

<ul>
  <li>one live execution instance,</li>
  <li>one source-level target,</li>
  <li>one relevant execution context.</li>
</ul>

<h3 id="what-this-document-does-not-define">9.3 What this document does not define</h3>

<p>
This document defines the notion of committed source-level state, but it does not yet define:
</p>

<ul>
  <li>the full safe-observation boundary model,</li>
  <li>the full pause-consistent snapshot model,</li>
  <li>the full safe-debug-stop model,</li>
  <li>the exact control boundaries at which a runtime must pause or may expose a coherent snapshot.</li>
</ul>

<p>
Those boundary rules belong to the companion document
<code>Language/Execution control and observation boundaries.md</code>.
</p>

<hr/>

<h2 id="runtime-freedom-and-observable-equivalence">10. Runtime Freedom and Observable Equivalence</h2>

<p>
Different runtimes MAY use different:
</p>

<ul>
  <li>threads,</li>
  <li>queues,</li>
  <li>buffers,</li>
  <li>memory layouts,</li>
  <li>scheduling strategies,</li>
  <li>backend partitioning strategies,</li>
  <li>compiled or lowered internal forms.</li>
</ul>

<p>
However, this freedom is constrained.
The source-aligned execution meaning of a validated FROG MUST remain equivalent across conforming implementations.
</p>

<p>
In particular, conforming runtimes MUST NOT disagree about:
</p>

<ul>
  <li>which source-level object an execution activity belongs to,</li>
  <li>whether distinct execution contexts are semantically distinct,</li>
  <li>whether a sub-FROG activation belongs to the caller instance through the relevant execution context,</li>
  <li>which structure region is selected for one structure activation,</li>
  <li>whether a local-memory update changes the committed state for later relevant activations,</li>
  <li>what counts as source-level committed meaning once the relevant semantic milestone has occurred.</li>
</ul>

<p>
This separation allows backend freedom without semantic fragmentation of the language.
</p>

<hr/>

<h2 id="validation-rules">11. Validation Rules</h2>

<p>
Implementations MUST enforce the following language-level rules:
</p>

<ul>
  <li>every live execution instance MUST correspond to one validated executable graph,</li>
  <li>every source-aligned execution observation or inspection result MUST remain attributable to stable source identity,</li>
  <li>dynamic execution meaning MUST remain attributable to an execution context whenever source identity alone is insufficient,</li>
  <li>distinct live execution instances MUST remain semantically independent unless a stricter future profile explicitly defines otherwise,</li>
  <li>committed source-level state MUST NOT be confused with runtime-internal transient implementation state,</li>
  <li>runtime freedom MUST preserve source-aligned semantic equivalence.</li>
</ul>

<p>
Implementations SHOULD additionally make it possible to retain enough identity and context information to support:
</p>

<ul>
  <li>source-aligned observability,</li>
  <li>source-aligned debugging,</li>
  <li>source-aligned probes and watches,</li>
  <li>diagnostic attribution at the level of nodes, structures, regions, and sub-FROG call sites.</li>
</ul>

<hr/>

<h2 id="out-of-scope-for-v01">12. Out of Scope for v0.1</h2>

<ul>
  <li>the exact safe-observation model,</li>
  <li>the exact safe-debug-stop model,</li>
  <li>the full pause/resume state machine,</li>
  <li>the full fault/abort control-boundary model,</li>
  <li>a mandatory runtime event transport,</li>
  <li>a mandatory event payload schema,</li>
  <li>a mandatory timestamp model,</li>
  <li>a mandatory value-serialization model,</li>
  <li>distributed execution semantics,</li>
  <li>asynchronous task orchestration semantics,</li>
  <li>execution-IR contracts,</li>
  <li>compiler-lowering contracts,</li>
  <li>runtime capability and security profiles.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
This document defines the execution-model vocabulary that belongs normatively to <code>Language/</code>.
</p>

<ul>
  <li>a validated FROG executes as a validated executable graph,</li>
  <li>a live execution instance is one running realization of that graph,</li>
  <li>execution meaning MUST remain attributable to stable source identity,</li>
  <li>dynamic activity MUST also remain attributable to execution context whenever source identity alone is insufficient,</li>
  <li>semantic execution milestones exist independently of any one observability protocol,</li>
  <li>committed source-level state distinguishes semantically valid source-level meaning from runtime-internal transient state,</li>
  <li>runtime freedom is allowed only when source-aligned semantic equivalence is preserved.</li>
</ul>

<p>
This gives FROG a stable language-level execution-model foundation on top of which observability, debugging, probes, watches, runtime profiles, and future lowering layers can be specified without collapsing architectural ownership.
</p>

<hr/>

<h2 id="license">14. License</h2>

<p>
This specification is part of the FROG repository and is licensed under the repository license.
</p>
