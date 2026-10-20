<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IDE Probes</h1>

<p align="center">
  <strong>Source-attached probes for execution inspection in a FROG IDE</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#document-role">2. Document Role</a></li>
  <li><a href="#repository-navigation">3. Repository Navigation</a></li>
  <li><a href="#architectural-position-and-dependencies">4. Architectural Position and Dependencies</a></li>
  <li><a href="#ownership-boundary">5. Ownership Boundary</a></li>
  <li><a href="#design-principles">6. Design Principles</a></li>
  <li><a href="#probe-model">7. Probe Model</a></li>
  <li><a href="#probes-vs-watches">8. Probes vs Watches</a></li>
  <li><a href="#probe-target-model">9. Probe Target Model</a></li>
  <li><a href="#probe-kinds">10. Probe Kinds</a></li>
  <li><a href="#probe-lifecycle-and-states">11. Probe Lifecycle and States</a></li>
  <li><a href="#observation-semantics">12. Observation Semantics</a></li>
  <li><a href="#live-paused-and-retained-views">13. Live, Paused, and Retained Views</a></li>
  <li><a href="#edge-probes">14. Edge Probes</a></li>
  <li><a href="#port-probes">15. Port Probes</a></li>
  <li><a href="#local-memory-probes">16. Local-Memory Probes</a></li>
  <li><a href="#ui-related-probes">17. UI-Related Probes</a></li>
  <li><a href="#presentation-and-management">18. Presentation and Management</a></li>
  <li><a href="#validation-and-safety-rules">19. Validation and Safety Rules</a></li>
  <li><a href="#illustrative-examples">20. Illustrative Examples</a></li>
  <li><a href="#directory-navigation">21. Directory Navigation</a></li>
  <li><a href="#out-of-scope">22. Out of Scope</a></li>
  <li><a href="#summary">23. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the <strong>probe model</strong> of a FROG IDE.
A probe is an <strong>IDE-managed, source-attached inspection object</strong> associated with one source-visible execution target so that a user can inspect source-aligned execution observations.
</p>

<p>
A probe does <strong>not</strong> redefine execution semantics.
It consumes the source-aligned observability already exposed to the IDE and presents that information in a local inspection form suitable for diagram-centric workflows.
</p>

<p>
Probes are primarily designed to make ordinary dataflow understandable at the level of:
</p>

<ul>
  <li>edges,</li>
  <li>node ports when needed,</li>
  <li>local-memory state when supported,</li>
  <li>certain UI-related execution objects under compatible observability support.</li>
</ul>

<pre><code>execution-facing systems
        |
        v
execution observability
        |
        +-----------+-----------+
        |                       |
        v                       v
   local probe             centralized watch
 source-attached         persistent list entry
 diagram-centric         monitoring-centric
</code></pre>

<p>
A probe therefore belongs to the <strong>IDE inspection layer</strong>.
It may be used together with debugging, but it is not itself a debugger control object.
</p>

<hr/>

<h2 id="document-role">2. Document Role</h2>

<p>
This document is the probe-focused document of the <code>IDE/</code> family.
Its role is to define local, source-attached inspection behavior built on top of the shared observability layer.
</p>

<p>
It should be read together with:
</p>

<ul>
  <li><code>IDE/Readme.md</code> for the overall IDE architecture and document map,</li>
  <li><code>IDE/Observability.md</code> for the source-aligned observable projection consumed by probes,</li>
  <li><code>IDE/Debugging.md</code> for pause, resume, break, and step behavior used together with probes,</li>
  <li><code>IDE/Watch.md</code> for centralized persistent inspection behavior distinct from probes.</li>
</ul>

<p>
This document is intentionally modular.
It defines probe behavior only.
It does not absorb execution semantics, debugger control, full watch behavior, or runtime-private observability mechanisms.
</p>

<hr/>

<h2 id="repository-navigation">3. Repository Navigation</h2>

<p>
The most relevant repository files and directories for this document are:
</p>

<pre><code>IDE/
├── Readme.md
│   -&gt; overall IDE architecture and document map
├── Observability.md
│   -&gt; source-aligned execution observability consumed by probes
├── Probes.md
│   -&gt; this document
├── Watch.md
│   -&gt; centralized persistent inspection distinct from probes
└── Debugging.md
    -&gt; interactive debugging behavior compatible with probes
</code></pre>

<p>
Upstream repository areas that matter for probes are:
</p>

<pre><code>Expression/
├── Diagram.md
└── Widget interaction.md

Language/
├── Control structures.md
└── State and cycles.md

IR/
├── Execution IR.md
└── Derivation rules.md

Libraries/ and Profiles/
└── ... executable capability surfaces observed through source-aligned projection
</code></pre>

<p>
This navigation matters because probes depend on a clean chain:
source-visible targets, validated execution meaning, execution-facing attribution, source-aligned observability, and finally local IDE-side inspection.
</p>

<hr/>

<h2 id="architectural-position-and-dependencies">4. Architectural Position and Dependencies</h2>

<h3>4.1 Repository position</h3>

<p>
This document belongs in <code>IDE/</code> because it defines how an IDE inspects execution through source-attached probe objects.
It does <strong>not</strong> belong to <code>Language/</code> because it does not own execution meaning.
It does <strong>not</strong> belong to <code>IR/</code> because it does not own execution-facing derivation.
</p>

<h3>4.2 Dependencies</h3>

<p>
This document depends on the following specifications:
</p>

<ul>
  <li><code>IDE/Readme.md</code> — architectural role of observability, debugging, probes, and watches in the IDE layer,</li>
  <li><code>IDE/Observability.md</code> — source-aligned observable projection consumed by probes,</li>
  <li><code>IDE/Debugging.md</code> — pause, resume, stepping, and breakpoint behavior used together with probes,</li>
  <li><code>IDE/Watch.md</code> — centralized persistent inspection behavior distinct from probes,</li>
  <li><code>Expression/Diagram.md</code> — canonical node, port, and edge representation,</li>
  <li><code>Expression/Widget interaction.md</code> — source-visible widget interaction paths,</li>
  <li><code>Language/Control structures.md</code> — normative structure execution meaning,</li>
  <li><code>Language/State and cycles.md</code> — normative local-memory and cycle behavior,</li>
  <li><code>IR/Execution IR.md</code> — execution-facing identity and attribution posture,</li>
  <li><code>IR/Derivation rules.md</code> — source-to-execution-facing correspondence rules.</li>
</ul>

<h3>4.3 Dependency diagram</h3>

<pre><code>Expression/ + Language/ + Libraries/ + Profiles/
                      |
                      v
                     IR/
                      |
                      v
          IDE/Observability.md
               /           \
              v             v
      IDE/Probes.md    IDE/Watch.md

IDE/Debugging.md
    -&gt; consumes the same observable world
    -&gt; remains ownership-distinct from probes
</code></pre>

<hr/>

<h2 id="ownership-boundary">5. Ownership Boundary</h2>

<p>
If a conflict appears, the following ownership rules apply:
</p>

<ul>
  <li><code>Expression/</code> remains authoritative for source-visible targets and canonical source identity,</li>
  <li><code>Language/</code> remains authoritative for execution meaning, committed state, and safe observation boundaries,</li>
  <li><code>Libraries/</code> and <code>Profiles/</code> remain authoritative for primitive-local and capability-local behavior,</li>
  <li><code>IR/</code> remains authoritative for execution-facing attribution posture,</li>
  <li><code>IDE/Observability.md</code> remains authoritative for the observable projection exposed to tools,</li>
  <li><code>IDE/Debugging.md</code> remains authoritative for debugger controls,</li>
  <li><code>IDE/Watch.md</code> remains authoritative for centralized persistent watch behavior,</li>
  <li><code>IDE/Probes.md</code> remains authoritative only for source-attached probe behavior and meaning.</li>
</ul>

<pre><code>Expression/
    owns what is source-visible and serializable

Language/
    owns what execution means and when it is safe to observe

Libraries/ and Profiles/
    own primitive-local and capability-local executable meaning

IR/
    owns execution-facing attribution and derivation posture

IDE/Observability.md
    owns the observable projection exposed to tools

IDE/Probes.md
    owns local source-attached inspection behavior
</code></pre>

<hr/>

<h2 id="design-principles">6. Design Principles</h2>

<h3>6.1 Source-attached inspection</h3>

<p>
A probe MUST attach to a source-visible execution target.
A probe MUST NOT require the user to understand runtime-private buffers, queues, or scheduler artifacts.
</p>

<h3>6.2 Non-intrusive observation</h3>

<p>
A standard probe MUST NOT change program semantics.
Adding, removing, opening, or closing a probe MUST NOT alter the meaning of the FROG graph.
</p>

<h3>6.3 Committed observations only</h3>

<p>
Probe-visible values or states MUST correspond to committed source-level observations.
A probe MUST NOT present speculative, half-updated, or otherwise incoherent source meaning as stable.
</p>

<h3>6.4 Dataflow-first model</h3>

<p>
Because FROG is dataflow-based, the canonical first-class probe target is the <strong>source edge</strong>.
This preserves the natural inspection model of observing values as they become available in the graph.
</p>

<h3>6.5 Local rather than centralized inspection</h3>

<p>
A probe is defined by <strong>local source attachment</strong>.
It is not the same thing as a centralized persistent watch entry.
A probe MAY be rendered inline, near its target, or in a detachable local view, but its semantic identity remains target-local.
</p>

<h3>6.6 Debug-compatible but not debug-owned</h3>

<p>
Probes MAY be used during free-running execution, during a paused debug session, or in a retained post-execution view when supported.
They are compatible with debugging, but they are not owned by the debugger command model.
</p>

<h3>6.7 Observability support and capability support remain distinct</h3>

<p>
The repository uses <code>Profiles/</code> for optional standardized executable capability families.
This document may also refer to stronger or weaker probe support through observability support.
These notions MUST NOT be conflated.
</p>

<ul>
  <li>a capability family defines what executable capability surfaces exist,</li>
  <li>an observability support level defines how much source-aligned execution detail is exposed to probes.</li>
</ul>

<h3>6.8 Minimal but extensible</h3>

<p>
FROG defines a minimal probe core.
Stronger observability support MAY expose richer rendering, richer history, type-specific visualization, or custom probe forms, provided that the base semantic meaning remains preserved.
</p>

<hr/>

<h2 id="probe-model">7. Probe Model</h2>

<h3>7.1 Definition</h3>

<p>
A probe is an IDE-managed inspection object associated with exactly one primary target.
It receives compatible source-aligned observations and presents a user-facing inspection view of them.
</p>

<h3>7.2 Conceptual fields</h3>

<p>
Conceptually, a probe contains:
</p>

<ul>
  <li>a probe identifier,</li>
  <li>a primary target,</li>
  <li>a probe kind,</li>
  <li>a current probe-entry state,</li>
  <li>a last-known compatible observation when available,</li>
  <li>the relevant execution context for that observation when needed,</li>
  <li>an IDE-chosen display strategy.</li>
</ul>

<h3>7.3 Outside the canonical source</h3>

<p>
A probe is not part of the canonical FROG Expression.
It is an IDE-side inspection construct layered over execution observability or retained post-execution inspection.
Creating or removing a probe MUST NOT modify the source program.
</p>

<hr/>

<h2 id="probes-vs-watches">8. Probes vs Watches</h2>

<p>
Probes and watches consume the same underlying observability space, but they serve different workflows.
</p>

<pre><code>same underlying observations
        |
        +----------------------+----------------------+
        |                                             |
        v                                             v
      probe                                         watch
  local attachment                           centralized persistence
  target-centric                              list-centric
  diagram-centric                          monitoring-centric
</code></pre>

<h3>8.1 Probe</h3>

<p>
A probe is primarily local and target-centric.
It is typically used near the source-visible object being inspected and is naturally diagram-centric.
</p>

<h3>8.2 Watch</h3>

<p>
A watch is primarily persistent and centralized.
It belongs to a watch list or equivalent IDE-managed collection intended for cross-navigation and longer-lived monitoring.
</p>

<h3>8.3 Relationship</h3>

<p>
An IDE MAY allow:
</p>

<ul>
  <li>creating a probe from a source target,</li>
  <li>creating a watch from a source target,</li>
  <li>creating a watch from an existing probe target,</li>
  <li>creating a probe from an existing watch target.</li>
</ul>

<p>
However, a probe MUST NOT be treated as merely a differently rendered watch.
Its defining property is source attachment.
</p>

<hr/>

<h2 id="probe-target-model">9. Probe Target Model</h2>

<h3>9.1 General rule</h3>

<p>
A probe MUST attach to exactly one primary source-visible target.
A primary probe target MAY be:
</p>

<ul>
  <li>a source edge,</li>
  <li>a source node input port,</li>
  <li>a source node output port,</li>
  <li>a local-memory slot owned by a local-memory node instance,</li>
  <li>another source-visible target supported by the active observability support.</li>
</ul>

<h3>9.2 Execution context</h3>

<p>
When repeated observations of the same target would otherwise be ambiguous, a probe MUST preserve sufficient execution context to disambiguate them.
Relevant context MAY include:
</p>

<ul>
  <li>execution instance identity,</li>
  <li>sub-FROG stack,</li>
  <li>structure nesting,</li>
  <li>selected region identity,</li>
  <li>loop iteration identity,</li>
  <li>activation identity.</li>
</ul>

<h3>9.3 Target-local meaning</h3>

<p>
A probe target defines <strong>where</strong> inspection is attached.
The active observability support defines <strong>how much</strong> compatible observation can actually be exposed for that target.
</p>

<pre><code>probe entry
  ├── primary target
  │     └── edge / port / local_memory / supported source-visible target
  ├── latest compatible observation
  │     └── committed value or committed state snapshot
  └── execution context
        ├── instance
        ├── subfrog stack
        ├── structure stack
        ├── selected region
        ├── iteration
        └── activation id
</code></pre>

<hr/>

<h2 id="probe-kinds">10. Probe Kinds</h2>

<h3>10.1 Minimum baseline</h3>

<p>
The <strong>minimum probe baseline</strong> requires support for:
</p>

<ul>
  <li><code>edge_probe</code></li>
</ul>

<p>
This is the only required probe kind for a minimal conforming probe implementation.
</p>

<h3>10.2 Optional standardized probe kinds</h3>

<p>
Additional standardized probe kinds MAY include:
</p>

<ul>
  <li><code>port_probe</code>,</li>
  <li><code>local_memory_probe</code>.</li>
</ul>

<p>
These kinds are standardized so that compatible implementations can expose the same meanings, but they are not part of the minimum baseline.
</p>

<h3>10.3 Stronger-support additions</h3>

<p>
A stronger observability support MAY additionally support:
</p>

<ul>
  <li><code>ui_sequence_probe</code>,</li>
  <li><code>structure_probe</code>,</li>
  <li><code>custom_probe</code>.</li>
</ul>

<h3>10.4 Priority of ordinary dataflow inspection</h3>

<p>
Among all probe forms, <code>edge_probe</code> is the canonical and most important probe kind.
It is the primary inspection form for ordinary dataflow values.
</p>

<hr/>

<h2 id="probe-lifecycle-and-states">11. Probe Lifecycle and States</h2>

<h3>11.1 Creation</h3>

<p>
A probe is created when the IDE attaches a new probe object to a valid target.
Creation MUST fail if the requested target is not a valid probe target under the active observability support.
</p>

<h3>11.2 Probe-entry states</h3>

<p>
A probe MAY conceptually be in one of the following states:
</p>

<ul>
  <li><code>created</code> — the probe exists but has not yet received a compatible observation,</li>
  <li><code>live</code> — the probe is receiving live execution updates,</li>
  <li><code>paused</code> — the probe reflects a paused consistent snapshot,</li>
  <li><code>retained</code> — the probe shows a retained post-execution observation when supported,</li>
  <li><code>unavailable</code> — the target exists but no compatible observation is currently available,</li>
  <li><code>closed</code> — the probe has been removed.</li>
</ul>

<p>
These are probe-entry states.
They MUST NOT be confused with the execution state of the underlying live instance.
</p>

<h3>11.3 Removal</h3>

<p>
Removing a probe affects only the inspection layer.
It MUST NOT alter the graph, execution semantics, source identity, or debugger control state.
</p>

<hr/>

<h2 id="observation-semantics">12. Observation Semantics</h2>

<h3>12.1 General rule</h3>

<p>
A probe does not invent values.
It displays values or source-aligned state only when those observations become available through the observability model exposed to the IDE.
</p>

<h3>12.2 Last-known committed observation</h3>

<p>
The primary semantic model of a probe is the <strong>last-known committed observation</strong>.
This means the most recent committed source-level value or state snapshot observed for the probe target in the relevant execution context.
</p>

<h3>12.3 Multiple observations over time</h3>

<p>
If a probe target receives multiple compatible observations over time, the base model requires that the probe be able to show at least the most recent one.
A stronger support level MAY additionally retain a bounded history or a richer time-oriented representation.
</p>

<h3>12.4 Value previews and full values</h3>

<p>
A runtime is not required to expose a fully materialized value for every observation.
A probe MAY therefore display:
</p>

<ul>
  <li>a full value snapshot,</li>
  <li>a summarized preview,</li>
  <li>a type-aware rendering,</li>
  <li>an opaque placeholder indicating availability without a full displayable value.</li>
</ul>

<p>
Whatever rendering strategy is used, the semantic meaning MUST remain that the probe refers to a compatible committed observation for its target.
</p>

<h3>12.5 No fabricated values</h3>

<p>
If no compatible committed observation exists yet, the probe MUST remain unavailable rather than implying that a value exists.
</p>

<h3>12.6 No fabricated history</h3>

<p>
A probe MUST NOT imply that a full historical sequence exists unless such history is actually preserved by the active observability support.
</p>

<hr/>

<h2 id="live-paused-and-retained-views">13. Live, Paused, and Retained Views</h2>

<pre><code>running execution
      |
      ├── compatible committed observation arrives
      |        └── probe may update live
      |
      ├── debugger pauses at safe boundary
      |        └── probe must match paused snapshot
      |
      └── execution ends
               └── probe may become retained if supported
</code></pre>

<h3>13.1 Live view</h3>

<p>
During a running execution, a probe MAY update as new compatible committed observations become available.
The exact refresh cadence is implementation-defined.
</p>

<h3>13.2 Paused view</h3>

<p>
When a debug session is paused, probe-visible information MUST match the paused execution snapshot exposed to the IDE.
A probe MUST NOT show values or state that contradict the paused source-level view.
</p>

<h3>13.3 Retained view</h3>

<p>
A stronger observability support MAY support retained probe values after execution has completed, faulted, aborted, or otherwise ended.
If retained values are supported, the IDE MUST clearly distinguish them from:
</p>

<ul>
  <li>live-updating values,</li>
  <li>paused-snapshot values.</li>
</ul>

<h3>13.4 Instance association</h3>

<p>
When retained behavior is supported, a retained observation SHOULD remain attributable to the execution instance from which it was derived.
An IDE MUST NOT silently conflate retained observations from unrelated instances.
</p>

<hr/>

<h2 id="edge-probes">14. Edge Probes</h2>

<h3>14.1 Role</h3>

<p>
An <code>edge_probe</code> is attached to one source edge.
It is the canonical probe kind for ordinary dataflow inspection in FROG.
</p>

<h3>14.2 Update trigger</h3>

<p>
An edge probe is updated when the target edge receives a compatible observable <code>edge_value_available</code> observation in the relevant execution context, or when an equivalent paused committed observation confirms the current edge value.
</p>

<h3>14.3 Meaning</h3>

<p>
The meaning of an edge probe is:
</p>

<pre><code>show the last-known committed value
that became available on this source edge
in the relevant execution context
</code></pre>

<h3>14.4 Fan-out</h3>

<p>
If one source output fans out to multiple edges, each edge MAY have its own probe.
This is correct because source-level inspection is naturally edge-based even if the runtime internally optimizes fan-out.
</p>

<h3>14.5 Repeated dynamic contexts</h3>

<p>
If the same edge is activated across different iterations or nested scopes, the probe SHOULD preserve the relevant context.
A stronger support level MAY show iteration metadata or a visible context stack together with the displayed value.
</p>

<hr/>

<h2 id="port-probes">15. Port Probes</h2>

<h3>15.1 Purpose</h3>

<p>
A <code>port_probe</code> is attached to a specific source node port rather than to a full edge.
This is useful when the IDE wants to present inspection at the node boundary level.
</p>

<h3>15.2 Input and output ports</h3>

<p>
A port probe MAY target:
</p>

<ul>
  <li>an input port,</li>
  <li>an output port.</li>
</ul>

<h3>15.3 Preferred use of edge probes</h3>

<p>
When the source graph already provides an explicit edge identity for ordinary valueflow, an implementation SHOULD prefer edge probes for canonical dataflow inspection.
Port probes are an additional inspection form, not the preferred default for ordinary wire-level understanding.
</p>

<h3>15.4 Meaning</h3>

<p>
A port probe attached to an output port SHOULD be semantically equivalent to probing the emitted committed value at that source boundary.
A port probe attached to an input port SHOULD reflect the committed value received at that input in the relevant execution context.
</p>

<hr/>

<h2 id="local-memory-probes">16. Local-Memory Probes</h2>

<h3>16.1 Purpose</h3>

<p>
A <code>local_memory_probe</code> is attached to the local-memory slot owned by a local-memory node instance.
Its purpose is to inspect source-aligned memory behavior rather than only ordinary edge values.
</p>

<h3>16.2 Canonical case: <code>frog.core.delay</code></h3>

<p>
For <code>frog.core.delay</code>, a local-memory probe SHOULD make it possible to inspect at least, when the active observability support exposes the corresponding information:
</p>

<ul>
  <li>the most recent observed <code>state_read</code>,</li>
  <li>the most recent observed <code>state_updated</code>,</li>
  <li>the current stored state visible at pause time.</li>
</ul>

<h3>16.3 Meaning</h3>

<p>
A local-memory probe is not merely another edge probe.
It is attached to the stateful source-aligned object owned by the node instance.
It therefore observes local-memory activity without redefining primitive-local behavior or language-level cycle semantics.
</p>

<h3>16.4 Deterministic paused interpretation</h3>

<p>
If a local-memory probe displays a current stored value during a paused snapshot, that value MUST be the committed stored value for the paused execution instance.
</p>

<pre><code>delay node instance
   ├── observed state_read
   ├── observed state_updated
   └── paused current stored value
        (only when exposed by the active observability support)
</code></pre>

<hr/>

<h2 id="ui-related-probes">17. UI-Related Probes</h2>

<h3>17.1 Ordinary widget-related valueflow</h3>

<p>
Ordinary widget-related valueflow SHOULD be inspected through the same canonical mechanisms as other dataflow:
</p>

<ul>
  <li>edge probes on edges connected to <code>widget_value</code>,</li>
  <li>port probes on relevant widget-related ports when supported.</li>
</ul>

<h3>17.2 Object-style widget interaction</h3>

<p>
Object-style widget interaction through <code>widget_reference</code>, <code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, and <code>frog.ui.method_invoke</code> MAY be inspected through:
</p>

<ul>
  <li>ordinary valueflow targets when relevant,</li>
  <li>pause-consistent debug inspection,</li>
  <li>stronger-support UI-specific probe renderers.</li>
</ul>

<p>
For ordinary widget value participation in dataflow, tools SHOULD prefer the natural <code>widget_value</code> representation rather than object-style property access.
</p>

<h3>17.3 UI sequencing edges</h3>

<p>
The <code>ui_in</code> / <code>ui_out</code> sequencing edges are not ordinary data-value edges.
A stronger observability support MAY support probes on these edges, but such probes MUST be interpreted as <strong>effect-order inspection</strong> rather than ordinary value inspection.
</p>

<p>
If supported, such probes MUST remain aligned with both:
</p>

<ul>
  <li>the source-facing widget interaction model,</li>
  <li>the relevant <code>frog.ui.*</code> primitive definitions.</li>
</ul>

<hr/>

<h2 id="presentation-and-management">18. Presentation and Management</h2>

<h3>18.1 Presentation model</h3>

<p>
This document does not standardize one required probe window layout.
It standardizes the source-level meaning of probe displays.
</p>

<h3>18.2 Minimal displayed information</h3>

<p>
A probe display SHOULD be able to show at least:
</p>

<ul>
  <li>the probe target identity,</li>
  <li>the most recent compatible observation when available,</li>
  <li>basic state information such as live, paused, retained, or unavailable,</li>
  <li>basic execution-context information when relevant.</li>
</ul>

<h3>18.3 Possible presentations</h3>

<p>
An IDE MAY present probes through:
</p>

<ul>
  <li>inline overlays near the target,</li>
  <li>detachable probe windows,</li>
  <li>a probe inspector pane,</li>
  <li>another equivalent local-inspection presentation.</li>
</ul>

<h3>18.4 Probe management</h3>

<p>
A probe-capable IDE SHOULD support at least:
</p>

<ul>
  <li>create probe,</li>
  <li>remove probe,</li>
  <li>list active probes,</li>
  <li>locate a probe from its source target,</li>
  <li>locate the source target from its probe entry.</li>
</ul>

<p>
A stronger observability support MAY additionally support:
</p>

<ul>
  <li>pinning a probe window,</li>
  <li>grouping probes by FROG or by execution instance,</li>
  <li>sorting by creation order,</li>
  <li>showing last-update timestamps,</li>
  <li>bulk removal.</li>
</ul>

<h3>18.5 Probe naming and numbering</h3>

<p>
An IDE MAY assign probe numbers or names for management purposes.
Such naming is an IDE concern only and is not part of the canonical source model.
</p>

<hr/>

<h2 id="validation-and-safety-rules">19. Validation and Safety Rules</h2>

<ul>
  <li>A probe MUST target a valid source-visible object supported by the active observability support.</li>
  <li>A probe MUST NOT alter execution semantics.</li>
  <li>A probe shown during pause MUST reflect the paused execution snapshot exposed to the IDE.</li>
  <li>A retained probe value MUST be clearly distinguishable from a live-updating value and from a paused-snapshot value.</li>
  <li>A probe MUST NOT expose a contradictory combination of target identity, execution context, and observation.</li>
  <li>A local-memory probe MUST respect node-instance-local memory scope.</li>
  <li>A UI-sequencing probe, if supported, MUST NOT be misrepresented as an ordinary data-value probe.</li>
  <li>A probe MUST NOT be presented as a watch entry unless the IDE explicitly re-exposes that target through the watch model.</li>
  <li>A probe MUST NOT silently conflate observations from unrelated execution instances when retained behavior is supported.</li>
</ul>

<hr/>

<h2 id="illustrative-examples">20. Illustrative Examples</h2>

<p>
The examples below are illustrative only.
They show possible probe-facing representations.
They are <strong>not</strong> a required transport format.
</p>

<h3>20.1 Edge probe</h3>

<pre><code>{
  "probe_id": "p1",
  "kind": "edge_probe",
  "target": {
    "kind": "edge",
    "id": "e_sum"
  },
  "state": "live",
  "last_observation": {
    "preview": "17.5"
  }
}</code></pre>

<h3>20.2 Port probe</h3>

<pre><code>{
  "probe_id": "p2",
  "kind": "port_probe",
  "target": {
    "kind": "port",
    "node_id": "add_1",
    "port": "result",
    "direction": "output"
  },
  "state": "paused",
  "last_observation": {
    "preview": "17.5"
  }
}</code></pre>

<h3>20.3 Local-memory probe for <code>frog.core.delay</code></h3>

<pre><code>{
  "probe_id": "p3",
  "kind": "local_memory_probe",
  "target": {
    "kind": "local_memory",
    "node_id": "delay_1"
  },
  "state": "paused",
  "last_observation": {
    "preview": "5.0"
  },
  "details": {
    "last_event": "state_updated"
  }
}</code></pre>

<h3>20.4 Retained edge probe</h3>

<pre><code>{
  "probe_id": "p4",
  "kind": "edge_probe",
  "target": {
    "kind": "edge",
    "id": "e_output"
  },
  "state": "retained",
  "instance_id": "run_42",
  "last_observation": {
    "preview": "42"
  }
}</code></pre>

<hr/>

<h2 id="directory-navigation">21. Directory Navigation</h2>

<p>
This document is easier to use when read together with the neighboring IDE documents:
</p>

<pre><code>IDE/
├── Readme.md
│   -&gt; overall IDE ownership, components, and document map
├── Observability.md
│   -&gt; source-aligned execution visibility consumed by probes
├── Probes.md
│   -&gt; this document; local source-attached inspection
├── Watch.md
│   -&gt; centralized persistent inspection distinct from probes
└── Debugging.md
    -&gt; interactive debugging behavior compatible with probes
</code></pre>

<p>
For readers following repository ownership boundaries, the main cross-layer path is:
</p>

<pre><code>Expression/
    -&gt; what source-visible probe targets exist

Language/
    -&gt; what execution means and when observation is coherent

IR/
    -&gt; what execution-facing attribution preserves

IDE/Observability.md
    -&gt; what source-aligned observations are exposed

IDE/Probes.md
    -&gt; how those observations are consumed locally
</code></pre>

<hr/>

<h2 id="out-of-scope">22. Out of Scope</h2>

<ul>
  <li>conditional probes that trigger debugger break behavior,</li>
  <li>watch expressions over arbitrary graph expressions,</li>
  <li>mandatory history buffers for every probe,</li>
  <li>reverse-time probe inspection,</li>
  <li>standardized distributed probe aggregation,</li>
  <li>remote probe streaming protocols,</li>
  <li>probe-driven execution modification,</li>
  <li>mandatory custom-probe packaging and discovery rules.</li>
</ul>

<hr/>

<h2 id="summary">23. Summary</h2>

<p>
FROG probes are <strong>source-attached inspection objects</strong> for live, paused, and optionally retained execution views.
The canonical probe model is centered on ordinary dataflow inspection through source edges, while also allowing additional probe forms for ports and local memory when supported.
</p>

<p>
This specification establishes that:
</p>

<ul>
  <li>probes belong to the IDE inspection layer rather than the source language itself,</li>
  <li>probes consume execution observability but do not own debugger controls,</li>
  <li><code>edge_probe</code> is the minimum and primary probe form,</li>
  <li>probe-visible values represent compatible committed observations,</li>
  <li>paused probe views must remain causally consistent,</li>
  <li>retained values are optional and must be clearly identified as such,</li>
  <li>probes remain distinct from centralized watch entries,</li>
  <li>stronger support may add richer or more specialized probe forms without changing the base semantics.</li>
</ul>

## Studio reconciliation — 14 October 2026

The [current Studio record](../docs/studio-reconciliation-2026-10-14.md) separates existing native subscriptions from complete product coverage. Real typed observations retain session/generation and lifetime. No observation is manufactured for an unexecuted point, and a presentation refresh must neither run the producer nor decide a condition from a lossy latest-value stream.
