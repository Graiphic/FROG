<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Execution Control and Observation Boundaries Specification</h1>

<p align="center">
  Normative control-safe and observation-safe boundaries for validated FROG executions<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#scope-for-v01">4. Scope for v0.1</a></li>
  <li><a href="#core-boundary-model">5. Core Boundary Model</a>
    <ul>
      <li><a href="#general-principle">5.1 General principle</a></li>
      <li><a href="#boundary-families">5.2 Boundary families</a></li>
      <li><a href="#illustrative-boundary-diagram">5.3 Illustrative boundary diagram</a></li>
    </ul>
  </li>
  <li><a href="#safe-observation-points">6. Safe Observation Points</a>
    <ul>
      <li><a href="#definition">6.1 Definition</a></li>
      <li><a href="#required-guarantees">6.2 Required guarantees</a></li>
      <li><a href="#causal-prefix-rule">6.3 Causal-prefix rule</a></li>
    </ul>
  </li>
  <li><a href="#pause-consistent-snapshots">7. Pause-Consistent Snapshots</a>
    <ul>
      <li><a href="#paused-state-entry">7.1 Entry into paused state</a></li>
      <li><a href="#snapshot-content">7.2 Snapshot content</a></li>
      <li><a href="#no-impossible-intermediate-state">7.3 No impossible intermediate state</a></li>
    </ul>
  </li>
  <li><a href="#safe-debug-stops">8. Safe Debug Stops</a>
    <ul>
      <li><a href="#general-definition">8.1 General definition</a></li>
      <li><a href="#relation-to-safe-observation-points">8.2 Relation to safe observation points</a></li>
      <li><a href="#source-aligned-stop-location">8.3 Source-aligned stop location</a></li>
    </ul>
  </li>
  <li><a href="#execution-state-boundaries">9. Execution State Boundaries</a>
    <ul>
      <li><a href="#instance-states">9.1 Instance states</a></li>
      <li><a href="#normal-completion-boundary">9.2 Normal completion boundary</a></li>
      <li><a href="#fault-boundary">9.3 Fault boundary</a></li>
      <li><a href="#abort-boundary">9.4 Abort boundary</a></li>
    </ul>
  </li>
  <li><a href="#control-requests-and-boundary-realization">10. Control Requests and Boundary Realization</a>
    <ul>
      <li><a href="#pause-requests">10.1 Pause requests</a></li>
      <li><a href="#debug-stop-requests">10.2 Debug-stop requests</a></li>
      <li><a href="#fault-directed-pause">10.3 Fault-directed pause</a></li>
      <li><a href="#resume-after-boundary">10.4 Resume after boundary</a></li>
    </ul>
  </li>
  <li><a href="#interaction-with-local-memory-structures-and-sub-frog">11. Interaction with Local Memory, Structures, and Sub-FROG</a>
    <ul>
      <li><a href="#local-memory-consistency">11.1 Local-memory consistency</a></li>
      <li><a href="#structure-region-consistency">11.2 Structure and region consistency</a></li>
      <li><a href="#sub-frog-context-consistency">11.3 Sub-FROG context consistency</a></li>
    </ul>
  </li>
  <li><a href="#relation-with-ide-observability-debugging-and-inspection">12. Relation with IDE, Observability, Debugging, and Inspection</a></li>
  <li><a href="#validation-rules">13. Validation Rules</a></li>
  <li><a href="#out-of-scope-for-v01">14. Out of Scope for v0.1</a></li>
  <li><a href="#summary">15. Summary</a></li>
  <li><a href="#license">16. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the language-level boundaries at which a validated FROG execution may be
observed, paused, stopped for debugging, completed, faulted, or aborted without losing source-level
semantic consistency.
</p>

<p>
It complements <code>Language/Execution model.md</code>.
That companion document defines the fundamental execution-model objects such as:
</p>

<ul>
  <li>validated executable graph,</li>
  <li>live execution instance,</li>
  <li>source identity,</li>
  <li>activation,</li>
  <li>execution context,</li>
  <li>semantic execution milestones,</li>
  <li>committed source-level state.</li>
</ul>

<p>
This document builds on those definitions and specifies the control-safe and observation-safe
boundaries at which source-level meaning may be exposed coherently.
</p>

<p>
This document does not define:
</p>

<ul>
  <li>IDE command syntax,</li>
  <li>event transport schemas,</li>
  <li>breakpoint UI behavior,</li>
  <li>probe window layout,</li>
  <li>watch table presentation,</li>
  <li>runtime-internal scheduler design.</li>
</ul>

<p>
Those concerns belong elsewhere.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<p>
This document has the following goals:
</p>

<ul>
  <li>define the language-level boundaries at which source-aligned execution state may be observed coherently,</li>
  <li>define when a live execution instance may enter <code>paused</code> without exposing partially committed source-level meaning,</li>
  <li>define what makes a debug stop safe at the language level,</li>
  <li>define minimal boundary semantics for normal completion, fault, and abort,</li>
  <li>provide a normative basis on top of which IDE observability, debugging, probes, and watches can be specified cleanly.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document depends on:
</p>

<ul>
  <li><code>Language/Readme.md</code> for ownership of execution semantics,</li>
  <li><code>Language/Execution model.md</code> for live execution instances, source identity, execution context, and committed source-level state,</li>
  <li><code>Language/Control structures.md</code> for the execution meaning of <code>case</code>, <code>for_loop</code>, and <code>while_loop</code>,</li>
  <li><code>Language/State and cycles.md</code> for the execution meaning of explicit local memory and valid cycles,</li>
  <li><code>IDE/Execution observability.md</code> for the IDE-facing projection of language-level boundaries,</li>
  <li><code>IDE/Debugging.md</code> for interactive debugging controls layered over the language-level boundaries defined here,</li>
  <li><code>IDE/Probes.md</code> and <code>IDE/Watch.md</code> for inspection constructs that consume committed source-level observations.</li>
</ul>

<p>
Responsibility separation MUST remain explicit:
</p>

<ul>
  <li><code>Language/Execution model.md</code> defines what execution activity means,</li>
  <li>this document defines where execution activity may be observed or controlled safely,</li>
  <li><code>IDE/Execution observability.md</code> defines how those boundaries are projected to IDE tooling,</li>
  <li><code>IDE/Debugging.md</code> defines how a debugger requests, names, and presents those stops,</li>
  <li><code>IDE/Probes.md</code> and <code>IDE/Watch.md</code> define how committed observations are displayed and retained.</li>
</ul>

<hr/>

<h2 id="scope-for-v01">4. Scope for v0.1</h2>

<p>
For FROG v0.1, this document standardizes:
</p>

<ul>
  <li>safe observation points,</li>
  <li>pause-consistent snapshots,</li>
  <li>safe debug stops,</li>
  <li>minimal instance states relevant to control boundaries,</li>
  <li>minimal boundary semantics for completion, fault, and abort,</li>
  <li>the requirement that source-visible control and observation remain causally consistent.</li>
</ul>

<p>
For v0.1, this document does not standardize:
</p>

<ul>
  <li>a complete debugger command language,</li>
  <li>a full fault taxonomy,</li>
  <li>a distributed pause protocol,</li>
  <li>a multi-runtime synchronization model,</li>
  <li>runtime capability or security profiles,</li>
  <li>a full asynchronous orchestration model.</li>
</ul>

<hr/>

<h2 id="core-boundary-model">5. Core Boundary Model</h2>

<h3 id="general-principle">5.1 General principle</h3>

<p>
A live execution instance MAY pass through many runtime-internal transient states that are useful
for implementation but are not yet valid for source-level interpretation.
</p>

<p>
Language-level observation and control MUST therefore occur only at boundaries where the exposed
source-level meaning is coherent.
These are boundary points, not arbitrary internal microstates.
</p>

<p>
A conforming implementation MAY use any internal scheduler, buffering strategy, or staging strategy,
provided that the source-level boundaries exposed to other layers remain equivalent in meaning.
</p>

<h3 id="boundary-families">5.2 Boundary families</h3>

<p>
For v0.1, the core boundary families are:
</p>

<ul>
  <li><strong>safe observation boundary</strong> — the execution may be observed coherently,</li>
  <li><strong>safe pause boundary</strong> — the execution may enter <code>paused</code> coherently,</li>
  <li><strong>safe debug-stop boundary</strong> — the execution may stop for debugging coherently,</li>
  <li><strong>terminal boundary</strong> — the execution reaches <code>completed</code>, <code>faulted</code>, or <code>aborted</code> coherently.</li>
</ul>

<p>
For v0.1, every safe pause boundary MUST also be a safe observation boundary.
Every safe debug-stop boundary MUST also be a safe pause boundary.
</p>

<h3 id="illustrative-boundary-diagram">5.3 Illustrative boundary diagram</h3>

<pre>
runtime-internal activity
        │
        │   (not necessarily source-safe to expose)
        ▼
 ┌───────────────────────────────┐
 │ safe observation boundary     │
 └───────────────────────────────┘
        │
        ├── may continue running
        │
        ├── may enter paused state
        │
        ▼
 ┌───────────────────────────────┐
 │ safe pause boundary           │
 └───────────────────────────────┘
        │
        ├── ordinary pause
        ├── debugger stop
        ├── fault-directed pause
        │
        ▼
 ┌───────────────────────────────┐
 │ safe debug-stop boundary      │
 └───────────────────────────────┘
        │
        ├── resume
        ├── abort
        └── diagnose / inspect
</pre>

<hr/>

<h2 id="safe-observation-points">6. Safe Observation Points</h2>

<h3 id="definition">6.1 Definition</h3>

<p>
A safe observation point is a moment at which a live execution instance may be observed without
exposing partially committed source-level state.
</p>

<p>
At a safe observation point, the exposed source-level meaning MUST be interpretable as a coherent
causal prefix of the live execution instance.
</p>

<h3 id="required-guarantees">6.2 Required guarantees</h3>

<p>
At a safe observation point:
</p>

<ul>
  <li>every source-visible observation MUST belong to one live execution instance,</li>
  <li>every source-visible observation MUST remain attributable to stable source identity,</li>
  <li>every source-visible observation that requires dynamic disambiguation MUST remain attributable to an execution context,</li>
  <li>no source-visible local-memory state MAY be half-updated,</li>
  <li>no selected structure region MAY be presented in a contradictory way,</li>
  <li>no source-visible value MAY be presented as committed if it is still only a runtime-internal transient artifact.</li>
</ul>

<h3 id="causal-prefix-rule">6.3 Causal-prefix rule</h3>

<p>
A safe observation point MUST correspond to a causally coherent prefix of source-level execution
meaning.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>an already exposed source-level observation MUST NOT be contradicted by later reinterpretation of the same paused or inspected state,</li>
  <li>a source-visible value MUST NOT appear as available if the source-aligned execution meaning required for that availability has not yet become committed,</li>
  <li>a structure region MUST NOT appear both selected and not selected in the same coherent observed state,</li>
  <li>a local-memory update MUST NOT appear partially applied.</li>
</ul>

<hr/>

<h2 id="pause-consistent-snapshots">7. Pause-Consistent Snapshots</h2>

<h3 id="paused-state-entry">7.1 Entry into paused state</h3>

<p>
If a live execution instance enters the <code>paused</code> state, it MUST do so only at a safe pause boundary.
For v0.1, every safe pause boundary MUST satisfy all requirements of a safe observation point.
</p>

<h3 id="snapshot-content">7.2 Snapshot content</h3>

<p>
A pause-consistent snapshot is the source-aligned state visible when a live execution instance is
paused at a safe pause boundary.
</p>

<p>
When a pause-consistent snapshot is exposed, it MAY include, where supported by the active profile:
</p>

<ul>
  <li>the current instance state,</li>
  <li>the most relevant current or last-completed source-visible activity,</li>
  <li>currently committed local-memory state,</li>
  <li>already committed source-visible values,</li>
  <li>current structure-selection state,</li>
  <li>the execution context needed to interpret the snapshot correctly.</li>
</ul>

<p>
The exact transport shape and the exact presentation are not defined here.
The semantic consistency requirements are defined here.
</p>

<h3 id="no-impossible-intermediate-state">7.3 No impossible intermediate state</h3>

<p>
A pause-consistent snapshot MUST NOT present an impossible intermediate source state.
In particular:
</p>

<ul>
  <li>a node MUST NOT appear both completed and not yet source-committed,</li>
  <li>a source-visible value MUST NOT appear committed if its producing source-level milestone has not been committed,</li>
  <li>a local-memory value MUST NOT appear between old state and new state,</li>
  <li>a sub-FROG activation MUST NOT lose the caller/callee relationship required by execution context.</li>
</ul>

<hr/>

<h2 id="safe-debug-stops">8. Safe Debug Stops</h2>

<h3 id="general-definition">8.1 General definition</h3>

<p>
A safe debug stop is a safe pause boundary at which the live execution instance may stop under
debugging control while preserving source-level semantic consistency.
</p>

<p>
A safe debug stop is not merely a scheduler interruption.
It is a source-valid control boundary.
</p>

<h3 id="relation-to-safe-observation-points">8.2 Relation to safe observation points</h3>

<p>
Every safe debug stop MUST satisfy all requirements of:
</p>

<ul>
  <li>a safe observation point, and</li>
  <li>a safe pause boundary.</li>
</ul>

<p>
Therefore, any source-visible state exposed at a safe debug stop MUST be interpretable as a
pause-consistent snapshot.
</p>

<h3 id="source-aligned-stop-location">8.3 Source-aligned stop location</h3>

<p>
A safe debug stop MUST remain attributable to a source-aligned target and execution context.
Depending on the triggering cause and the active profile, the relevant source-aligned target MAY be:
</p>

<ul>
  <li>a node activation,</li>
  <li>an edge availability boundary,</li>
  <li>a structure activation or region boundary,</li>
  <li>a sub-FROG call boundary,</li>
  <li>a fault-attributed source location.</li>
</ul>

<p>
This document defines the language-level requirement that the stop location remain source-aligned.
It does not define debugger UI behavior, breakpoint widgets, or command naming.
</p>

<hr/>

<h2 id="execution-state-boundaries">9. Execution State Boundaries</h2>

<h3 id="instance-states">9.1 Instance states</h3>

<p>
For the purpose of control and observation boundaries, a live execution instance MUST be
interpretable in one of the following high-level states:
</p>

<ul>
  <li><code>running</code> — the instance is actively progressing,</li>
  <li><code>paused</code> — the instance is suspended at a safe pause boundary,</li>
  <li><code>completed</code> — the instance terminated normally at a terminal boundary,</li>
  <li><code>faulted</code> — the instance terminated because of an execution fault at a terminal boundary,</li>
  <li><code>aborted</code> — the instance was externally terminated at a terminal boundary.</li>
</ul>

<p>
A stricter profile MAY define additional pre-start or teardown-adjacent states.
Those additional states MUST NOT invalidate the v0.1 meanings above.
</p>

<h3 id="normal-completion-boundary">9.2 Normal completion boundary</h3>

<p>
A live execution instance reaches the <code>completed</code> state only when its validated executable
graph has reached normal source-level completion.
</p>

<p>
At the normal completion boundary:
</p>

<ul>
  <li>the instance MUST no longer be <code>running</code>,</li>
  <li>the final source-visible committed state MUST be coherent,</li>
  <li>the execution context of the terminating scope MUST remain attributable where relevant.</li>
</ul>

<h3 id="fault-boundary">9.3 Fault boundary</h3>

<p>
A fault boundary is a terminal boundary caused by an execution fault.
For v0.1, this document does not standardize a full fault taxonomy.
It standardizes only the boundary meaning.
</p>

<p>
At a fault boundary:
</p>

<ul>
  <li>the live execution instance MUST reach a source-consistent termination outcome,</li>
  <li>the runtime MUST NOT expose a contradictory partially committed source state as the terminal observable state,</li>
  <li>where a source-aligned fault location can be identified, it SHOULD remain attributable to source identity and execution context.</li>
</ul>

<p>
A debugging-capable profile MAY convert a would-be immediate fault termination into a safe debug stop
before final termination, provided that the stop still satisfies the boundary requirements of this document.
</p>

<h3 id="abort-boundary">9.4 Abort boundary</h3>

<p>
An abort boundary is a terminal boundary caused by an external stop request rather than by normal
source-level completion.
</p>

<p>
At an abort boundary:
</p>

<ul>
  <li>the live execution instance MUST terminate in a source-consistent way,</li>
  <li>the final exposed source-visible state MUST correspond to a coherent causal prefix rather than to a contradictory partial state,</li>
  <li>the abort MUST NOT be represented as normal completion.</li>
</ul>

<hr/>

<h2 id="control-requests-and-boundary-realization">10. Control Requests and Boundary Realization</h2>

<h3 id="pause-requests">10.1 Pause requests</h3>

<p>
A pause request does not imply arbitrary immediate interruption.
For v0.1, a live execution instance MAY defer realization of a pause request until the next safe
pause boundary compatible with the active execution profile.
</p>

<p>
This rule exists to preserve source-level semantic consistency.
</p>

<h3 id="debug-stop-requests">10.2 Debug-stop requests</h3>

<p>
A debug-stop request, whether triggered by a breakpoint-like mechanism, a step operation, or another
debugging control, MUST ultimately resolve only at a safe debug stop.
</p>

<p>
The triggering mechanism is not standardized here.
The boundary requirement is standardized here.
</p>

<h3 id="fault-directed-pause">10.3 Fault-directed pause</h3>

<p>
When debugging is active, a runtime MAY pause at a safe debug stop associated with a fault-relevant
source location instead of terminating immediately.
</p>

<p>
If such a pause occurs:
</p>

<ul>
  <li>the pause MUST still satisfy the requirements of a safe debug stop,</li>
  <li>the instance MUST remain source-consistent,</li>
  <li>the runtime MUST NOT fabricate a cleaner state than the actually committed source-level prefix.</li>
</ul>

<h3 id="resume-after-boundary">10.4 Resume after boundary</h3>

<p>
If a live execution instance resumes from <code>paused</code>, it resumes from a source-consistent paused
boundary rather than from an arbitrary runtime-internal transient state.
</p>

<p>
This does not constrain how the runtime re-enters internal scheduling.
It constrains only the source-level meaning of the paused/resumed transition.
</p>

<hr/>

<h2 id="interaction-with-local-memory-structures-and-sub-frog">11. Interaction with Local Memory, Structures, and Sub-FROG</h2>

<h3 id="local-memory-consistency">11.1 Local-memory consistency</h3>

<p>
At any safe observation boundary, safe pause boundary, safe debug-stop boundary, or terminal boundary,
source-visible local-memory state MUST be coherent with the local-memory semantics defined elsewhere.
</p>

<p>
In particular:
</p>

<ul>
  <li>a local-memory slot MUST NOT be exposed as half-updated,</li>
  <li>the observed state MUST correspond either to the previously committed local-memory state or to the newly committed local-memory state,</li>
  <li>a valid feedback cycle using explicit local memory MUST remain interpretable according to the relevant local-memory primitive semantics.</li>
</ul>

<h3 id="structure-region-consistency">11.2 Structure and region consistency</h3>

<p>
At any source-visible boundary:
</p>

<ul>
  <li>a <code>case</code> structure MUST NOT expose contradictory region selection,</li>
  <li>a loop boundary MUST preserve the iteration context needed to interpret the observed state,</li>
  <li>a structure-owned region MUST remain attributable to its owning structure and to the relevant execution context.</li>
</ul>

<h3 id="sub-frog-context-consistency">11.3 Sub-FROG context consistency</h3>

<p>
When nested sub-FROG activity is exposed by the active profile, the observed or paused state MUST
preserve the caller/callee relationship through execution context.
</p>

<p>
A boundary MUST NOT collapse nested activity into an ambiguous state that loses:
</p>

<ul>
  <li>which call site is active,</li>
  <li>which nested scope is current,</li>
  <li>which committed observations belong to caller-level or callee-level activity.</li>
</ul>

<hr/>

<h2 id="relation-with-ide-observability-debugging-and-inspection">12. Relation with IDE, Observability, Debugging, and Inspection</h2>

<p>
This document defines the language-level boundary semantics that other layers rely on.
</p>

<ul>
  <li><code>IDE/Execution observability.md</code> SHOULD project safe observation boundaries and pause-consistent state in an IDE-visible form.</li>
  <li><code>IDE/Debugging.md</code> SHOULD define user-facing controls such as breakpoints, stepping, and resume behavior in terms of the safe debug stops defined here.</li>
  <li><code>IDE/Probes.md</code> SHOULD interpret probe values only from committed source-level observations valid at or across these boundaries.</li>
  <li><code>IDE/Watch.md</code> SHOULD interpret watch values only from committed source-level observations valid at or across these boundaries.</li>
</ul>

<p>
Accordingly, <code>IDE/</code> MUST NOT redefine the normative language meaning of:
</p>

<ul>
  <li>safe observation point,</li>
  <li>pause-consistent snapshot,</li>
  <li>safe debug stop,</li>
  <li>terminal boundary semantics for completion, fault, or abort.</li>
</ul>

<hr/>

<h2 id="validation-rules">13. Validation Rules</h2>

<p>
Conforming implementations MUST satisfy the following rules:
</p>

<ul>
  <li>a source-visible pause MUST occur only at a safe pause boundary,</li>
  <li>a source-visible debug stop MUST occur only at a safe debug-stop boundary,</li>
  <li>a source-visible observation MUST NOT expose partially committed source-level meaning,</li>
  <li>a source-visible terminal state MUST correspond to a coherent source-level boundary,</li>
  <li>source-visible local-memory state MUST NOT be half-updated at any safe or terminal boundary,</li>
  <li>source-visible structure and region state MUST NOT be contradictory at any safe or terminal boundary,</li>
  <li>source-visible caller/callee relationships MUST remain attributable through execution context when nested activity is exposed.</li>
</ul>

<p>
Conforming implementations SHOULD additionally support enough source-aligned attribution to allow:
</p>

<ul>
  <li>fault-relevant source attribution where possible,</li>
  <li>coherent probe updates,</li>
  <li>coherent watch updates,</li>
  <li>coherent pause snapshots for debugging-capable profiles.</li>
</ul>

<hr/>

<h2 id="out-of-scope-for-v01">14. Out of Scope for v0.1</h2>

<ul>
  <li>the exact debugger command set,</li>
  <li>the exact event transport format,</li>
  <li>the exact watch/probe rendering model,</li>
  <li>a full fault taxonomy,</li>
  <li>a mandatory distributed pause mechanism,</li>
  <li>a mandatory cross-runtime synchronization protocol,</li>
  <li>security and capability policies for host or system execution,</li>
  <li>full asynchronous task orchestration semantics,</li>
  <li>runtime profile negotiation.</li>
</ul>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
This document defines the language-level boundaries at which source-aligned execution may be safely
observed or controlled.
</p>

<ul>
  <li>a safe observation point exposes only coherent committed source-level meaning,</li>
  <li>a pause-consistent snapshot is a coherent source-aligned paused view,</li>
  <li>a safe debug stop is a source-valid paused boundary usable by debugging tools,</li>
  <li>normal completion, fault, and abort are terminal boundaries with distinct meanings,</li>
  <li>local memory, structure selection, and nested sub-FROG execution MUST remain coherent at every source-visible boundary,</li>
  <li><code>IDE/</code> consumes these boundaries but does not own their normative meaning.</li>
</ul>

<p>
This gives FROG a durable execution-boundary layer in <code>Language/</code>, allowing observability,
debugging, probes, watches, and future runtime profiles to build on one shared semantic foundation.
</p>

<hr/>

<h2 id="license">16. License</h2>

<p>
This specification is part of the FROG repository and is licensed under the repository license.
</p>
