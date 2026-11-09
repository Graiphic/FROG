<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IDE Watch</h1>

<p align="center">
  <strong>Persistent watch entries and centralized watch views for execution inspection in a FROG IDE</strong><br/>
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
  <li><a href="#watch-model">7. Watch Model</a></li>
  <li><a href="#watches-vs-probes">8. Watches vs Probes</a></li>
  <li><a href="#watch-list-model">9. Watch List Model</a></li>
  <li><a href="#watch-target-model">10. Watch Target Model</a></li>
  <li><a href="#watch-kinds">11. Watch Kinds</a></li>
  <li><a href="#observation-semantics">12. Observation Semantics</a></li>
  <li><a href="#live-paused-and-retained-behavior">13. Live, Paused, and Retained Behavior</a></li>
  <li><a href="#edge-watches">14. Edge Watches</a></li>
  <li><a href="#port-watches">15. Port Watches</a></li>
  <li><a href="#local-memory-watches">16. Local-Memory Watches</a></li>
  <li><a href="#ui-related-watches">17. UI-Related Watches</a></li>
  <li><a href="#watch-lifecycle-and-management">18. Watch Lifecycle and Management</a></li>
  <li><a href="#watch-presentation-model">19. Watch Presentation Model</a></li>
  <li><a href="#validation-and-safety-rules">20. Validation and Safety Rules</a></li>
  <li><a href="#illustrative-examples">21. Illustrative Examples</a></li>
  <li><a href="#directory-navigation">22. Directory Navigation</a></li>
  <li><a href="#out-of-scope">23. Out of Scope</a></li>
  <li><a href="#summary">24. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the <strong>watch model</strong> of a FROG IDE.
A watch is a <strong>persistent IDE-managed inspection entry</strong> associated with one source-visible target so that selected observations remain visible in a centralized watch view across execution, navigation, and debugging activity.
</p>

<p>
A watch is not part of the FROG source language.
It belongs to the <strong>IDE inspection layer</strong>.
A watch consumes source-aligned execution observability and presents that information in a <strong>centralized, persistent, user-manageable form</strong>.
</p>

<p>
Typical watch workflows include:
</p>

<ul>
  <li>keeping selected observations visible across stepping, breakpoints, and pause / resume cycles,</li>
  <li>monitoring multiple graph locations from one centralized inspection view,</li>
  <li>preserving visibility even when the user navigates away from the watched source target,</li>
  <li>retaining recent observations after pause or after execution has ended when supported.</li>
</ul>

<pre><code>execution observability
        |
        +----------------------+----------------------+
        |                                             |
        v                                             v
      probe                                         watch
  local attachment                           centralized persistence
  target-centric                              list-centric
  diagram-centric                          monitoring-centric
</code></pre>

<p>
A watch may be used together with debugging, but it is not itself a debugger control object.
</p>

<hr/>

<h2 id="document-role">2. Document Role</h2>

<p>
This document is the watch-focused document of the <code>IDE/</code> family.
Its role is to define persistent centralized inspection behavior built on top of the shared observability layer.
</p>

<p>
It should be read together with:
</p>

<ul>
  <li><code>IDE/Readme.md</code> for the overall IDE architecture and document map,</li>
  <li><code>IDE/Observability.md</code> for the source-aligned observable projection consumed by watches,</li>
  <li><code>IDE/Debugging.md</code> for pause, resume, break, and step behavior used together with watches,</li>
  <li><code>IDE/Probes.md</code> for local source-attached inspection behavior distinct from watches.</li>
</ul>

<p>
This document is intentionally modular.
It defines watch behavior only.
It does not absorb execution semantics, debugger control, full probe behavior, or runtime-private observability mechanisms.
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
│   -&gt; source-aligned execution observability consumed by watches
├── Probes.md
│   -&gt; local source-attached inspection behavior distinct from watches
├── Watch.md
│   -&gt; this document
└── Debugging.md
    -&gt; interactive debugging behavior compatible with watches
</code></pre>

<p>
Upstream repository areas that matter for watches are:
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
This navigation matters because watches depend on a clean chain:
source-visible targets, validated execution meaning, execution-facing attribution, source-aligned observability, and finally persistent IDE-side inspection.
</p>

<hr/>

<h2 id="architectural-position-and-dependencies">4. Architectural Position and Dependencies</h2>

<h3>4.1 Repository position</h3>

<p>
This document belongs in <code>IDE/</code> because it defines how a FROG IDE presents persistent centralized inspection of source-aligned execution observations.
It does <strong>not</strong> belong to <code>Language/</code> because it does not own execution meaning.
It does <strong>not</strong> belong to <code>IR/</code> because it does not own execution-facing derivation.
</p>

<h3>4.2 Dependencies</h3>

<p>
This document depends on the following specifications:
</p>

<ul>
  <li><code>IDE/Readme.md</code> — architectural role of observability, debugging, probes, and watches in the IDE layer,</li>
  <li><code>IDE/Observability.md</code> — source-aligned observable projection consumed by watches,</li>
  <li><code>IDE/Debugging.md</code> — pause, resume, stepping, and breakpoint behavior used together with watches,</li>
  <li><code>IDE/Probes.md</code> — local source-attached inspection behavior distinct from watches,</li>
  <li><code>Expression/Diagram.md</code> — canonical source-visible edges, ports, and related identity,</li>
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
    -&gt; remains ownership-distinct from watches
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
  <li><code>IDE/Debugging.md</code> remains authoritative for debugger control behavior,</li>
  <li><code>IDE/Probes.md</code> remains authoritative for local source-attached inspection behavior,</li>
  <li><code>IDE/Watch.md</code> remains authoritative only for persistent centralized watch behavior and meaning.</li>
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

IDE/Watch.md
    owns centralized persistent inspection behavior
</code></pre>

<hr/>

<h2 id="design-principles">6. Design Principles</h2>

<h3>6.1 Source-visible targets only</h3>

<p>
A watch MUST track a source-visible target.
A watch MUST NOT require the user to reason about runtime-private buffers, queues, or scheduler artifacts.
</p>

<h3>6.2 Persistent centralized inspection</h3>

<p>
A watch is intended to remain visible and manageable independently from the immediate local viewport around the target.
That persistent centralized role is what distinguishes it from a probe.
</p>

<h3>6.3 Non-intrusive observation</h3>

<p>
Creating, updating, enabling, disabling, or removing a watch MUST NOT alter execution semantics.
Watches belong to the IDE inspection layer only.
</p>

<h3>6.4 Committed observations only</h3>

<p>
A watch MUST NOT present speculative, transient, or half-committed source-level observations as stable values.
Watched values and state snapshots MUST correspond to committed source-aligned observations.
</p>

<h3>6.5 Pause-consistent consumption</h3>

<p>
When execution is paused, a watch MUST remain consistent with the language-valid pause-consistent snapshot exposed to the IDE.
A watch MUST NOT contradict the paused view.
</p>

<h3>6.6 Debug-compatible but not debug-owned</h3>

<p>
Watches MAY be used together with pause, stepping, and breakpoints, but they do not define debugger control semantics.
They consume observable execution state; they do not own the control model.
</p>

<h3>6.7 Observability support and capability support remain distinct</h3>

<p>
The repository uses <code>Profiles/</code> for optional standardized executable capability families.
This document may also refer to stronger or weaker watch support through observability support.
These notions MUST NOT be conflated.
</p>

<ul>
  <li>a capability family defines what executable capability surfaces exist,</li>
  <li>an observability support level defines how much source-aligned execution detail is exposed to watch entries.</li>
</ul>

<h3>6.8 Minimal but extensible</h3>

<p>
FROG defines a minimal watch model.
Stronger observability support MAY add richer filtering, grouping, formatting, history retention, custom renderers, or richer contextual retention, provided that the base semantic meaning remains preserved.
</p>

<hr/>

<h2 id="watch-model">7. Watch Model</h2>

<h3>7.1 Definition</h3>

<p>
A watch is an IDE-managed persistent inspection entry associated with exactly one primary target.
It displays the most relevant compatible source-aligned observation for that target in a centralized watch view.
</p>

<h3>7.2 Conceptual fields</h3>

<p>
Conceptually, a watch contains:
</p>

<ul>
  <li>a watch identifier,</li>
  <li>a primary target,</li>
  <li>a watch kind,</li>
  <li>a current watch-entry state,</li>
  <li>a last-known compatible observation when available,</li>
  <li>relevant execution context for that observation when needed,</li>
  <li>optional retained observation metadata when supported.</li>
</ul>

<h3>7.3 Outside the canonical source</h3>

<p>
A watch does not exist inside the canonical FROG Expression.
It is an IDE-side inspection construct layered over live or retained source-aligned observability.
Creating or removing a watch MUST NOT modify the source program.
</p>

<hr/>

<h2 id="watches-vs-probes">8. Watches vs Probes</h2>

<p>
Watches and probes consume the same underlying observability space, but they serve different workflows.
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
It is typically used near the source object being inspected and is naturally diagram-centric.
</p>

<h3>8.2 Watch</h3>

<p>
A watch is primarily persistent and centralized.
It belongs to a watch list or equivalent IDE-managed collection intended for broader monitoring.
</p>

<h3>8.3 Relationship</h3>

<p>
An IDE MAY allow:
</p>

<ul>
  <li>creating a watch from a source target,</li>
  <li>creating a watch from an existing probe target,</li>
  <li>creating a probe from an existing watch target.</li>
</ul>

<p>
However, a watch MUST NOT be treated as merely a differently rendered probe.
Its defining property is persistent centralized inspection across navigation and debugging activity.
</p>

<hr/>

<h2 id="watch-list-model">9. Watch List Model</h2>

<h3>9.1 Canonical logical model</h3>

<p>
The canonical logical model of watches is a <strong>watch list</strong>.
A watch list is an IDE-managed collection of persistent watch entries associated with one editing, inspection, or debugging context.
</p>

<h3>9.2 Possible presentations</h3>

<p>
A watch list MAY be presented as:
</p>

<ul>
  <li>a dedicated watch window,</li>
  <li>a dockable watch panel,</li>
  <li>a side inspector pane,</li>
  <li>another equivalent centralized IDE presentation.</li>
</ul>

<p>
The exact UI layout is not standardized here.
The standardized semantic model is that watch entries belong to an ordered and user-manageable collection.
</p>

<h3>9.3 Minimum watch-list capabilities</h3>

<p>
A watch-capable IDE SHOULD support at least:
</p>

<ul>
  <li>adding a watch,</li>
  <li>removing a watch,</li>
  <li>listing active watches,</li>
  <li>mapping a watch entry back to its source target,</li>
  <li>showing the most recent compatible observation when available.</li>
</ul>

<hr/>

<h2 id="watch-target-model">10. Watch Target Model</h2>

<h3>10.1 General rule</h3>

<p>
A watch MUST attach to exactly one primary source-visible target.
A watch target MAY be:
</p>

<ul>
  <li>a source edge,</li>
  <li>a source node input port,</li>
  <li>a source node output port,</li>
  <li>a local-memory slot owned by a local-memory node instance,</li>
  <li>another source-visible target supported by the active observability support.</li>
</ul>

<h3>10.2 Execution context</h3>

<p>
When repeated observations of the same target would otherwise be ambiguous, a watch MUST preserve sufficient execution context to interpret the latest relevant observation.
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

<h3>10.3 Stable target, changing observation</h3>

<p>
A watch entry is normally attached to a stable source target, not to one permanently fixed dynamic occurrence.
Repeated compatible observations of that target MAY therefore update one watch entry over time, while the IDE MAY also display contextual metadata about the most recent observation.
</p>

<hr/>

<h2 id="watch-kinds">11. Watch Kinds</h2>

<h3>11.1 Minimum baseline</h3>

<p>
The <strong>minimum watch baseline</strong> requires support for:
</p>

<ul>
  <li><code>edge_watch</code></li>
</ul>

<p>
This is the only required watch kind for a minimal conforming watch implementation.
</p>

<h3>11.2 Optional standardized watch kinds</h3>

<p>
Additional standardized watch kinds MAY include:
</p>

<ul>
  <li><code>port_watch</code>,</li>
  <li><code>local_memory_watch</code>.</li>
</ul>

<p>
These kinds are standardized so that compatible implementations can expose the same meanings, but they are not part of the minimum baseline.
</p>

<h3>11.3 Stronger-support additions</h3>

<p>
A stronger observability support MAY additionally support:
</p>

<ul>
  <li><code>ui_sequence_watch</code>,</li>
  <li><code>structure_watch</code>,</li>
  <li><code>custom_watch</code>.</li>
</ul>

<h3>11.4 Priority of ordinary dataflow inspection</h3>

<p>
Among all watch forms, <code>edge_watch</code> is the canonical and most natural watch kind, because ordinary FROG dataflow is most naturally monitored at the edge level.
</p>

<hr/>

<h2 id="observation-semantics">12. Observation Semantics</h2>

<h3>12.1 General rule</h3>

<p>
A watch does not invent values.
It displays values or state only when compatible source-aligned observations become available through the IDE-facing observability model.
</p>

<h3>12.2 Last-known committed observation</h3>

<p>
The primary semantic model of a watch is the <strong>last-known committed observation</strong> associated with the watched target in the relevant execution context.
</p>

<p>
This may be:
</p>

<ul>
  <li>a value observed on an edge,</li>
  <li>a value observed at a node port,</li>
  <li>a state snapshot observed for local memory,</li>
  <li>another source-aligned committed observation exposed by a stronger observability support.</li>
</ul>

<h3>12.3 Multiple observations over time</h3>

<p>
If a watched target receives multiple compatible observations over time, the base model requires that the watch be able to show at least the most recent one.
A stronger support level MAY additionally preserve bounded history or richer time-oriented rendering.
</p>

<h3>12.4 Full values and previews</h3>

<p>
A watch MAY display:
</p>

<ul>
  <li>a full materialized value,</li>
  <li>a summarized preview,</li>
  <li>a type-aware rendering,</li>
  <li>an availability marker with limited value detail.</li>
</ul>

<p>
In every case, the semantic meaning remains the same:
the display corresponds to the last-known compatible committed observation for the watched target.
</p>

<h3>12.5 No fabricated values</h3>

<p>
If no compatible observation exists yet, the watch entry MUST remain explicitly unavailable rather than implying that a real value exists.
</p>

<h3>12.6 No fabricated history</h3>

<p>
A watch MUST NOT imply that a full history exists unless such history is actually preserved by the active observability support.
</p>

<hr/>

<h2 id="live-paused-and-retained-behavior">13. Live, Paused, and Retained Behavior</h2>

<pre><code>running execution
      |
      ├── compatible committed observation arrives
      |        └── watch may update live
      |
      ├── debugger pauses at safe boundary
      |        └── watch must match paused snapshot
      |
      └── execution ends
               └── watch may become retained if supported
</code></pre>

<h3>13.1 Live behavior</h3>

<p>
During running execution, a watch MAY update whenever a compatible committed observation becomes available for its target.
The refresh strategy and update frequency are implementation-defined.
</p>

<h3>13.2 Paused behavior</h3>

<p>
When execution is paused under debugging control, a watch MUST reflect the paused execution snapshot exposed to the IDE.
A watch MUST NOT display a value or state snapshot that contradicts the paused source-level view.
</p>

<h3>13.3 Retained behavior</h3>

<p>
A stronger observability support MAY allow a watch to retain its last-known observation after pause, after normal completion, after fault termination, or after abort.
If retained behavior is supported, the IDE MUST clearly distinguish between:
</p>

<ul>
  <li>live-updating watch entries,</li>
  <li>paused-snapshot watch entries,</li>
  <li>retained watch entries.</li>
</ul>

<h3>13.4 Instance association</h3>

<p>
When retained behavior is supported, a retained observation SHOULD remain attributable to the execution instance from which it was derived.
An IDE MUST NOT silently conflate retained observations from unrelated instances.
</p>

<hr/>

<h2 id="edge-watches">14. Edge Watches</h2>

<h3>14.1 Role</h3>

<p>
An <code>edge_watch</code> is attached to one source edge.
It is the canonical watch kind for ordinary dataflow monitoring in FROG.
</p>

<h3>14.2 Update trigger</h3>

<p>
An edge watch is updated when the watched edge receives a compatible observable <code>edge_value_available</code> observation in the relevant execution context, or when an equivalent paused committed observation confirms the current edge value.
</p>

<h3>14.3 Meaning</h3>

<p>
The meaning of an edge watch is:
</p>

<pre><code>show the last-known committed value
that became available on this source edge
in the relevant execution context
</code></pre>

<h3>14.4 Fan-out</h3>

<p>
If one source output fans out to multiple edges, each edge MAY have its own watch entry.
This remains correct because source-level watch semantics are edge-based even if runtime implementation details are optimized differently.
</p>

<h3>14.5 Repeated dynamic contexts</h3>

<p>
If the same watched edge is activated repeatedly across iterations or nested scopes, the watch entry MAY continue updating with the latest compatible observation while displaying contextual metadata about that observation.
</p>

<hr/>

<h2 id="port-watches">15. Port Watches</h2>

<h3>15.1 Role</h3>

<p>
A <code>port_watch</code> is attached to a specific source node port rather than to a full edge.
This watch form is useful when the IDE wants to center monitoring on node boundaries.
</p>

<h3>15.2 Input and output ports</h3>

<p>
A port watch MAY target:
</p>

<ul>
  <li>an input port,</li>
  <li>an output port.</li>
</ul>

<h3>15.3 Preferred use of edge watches</h3>

<p>
When the source graph already provides an explicit edge identity for ordinary valueflow, an implementation SHOULD prefer edge watches as the canonical monitoring form.
Port watches are an additional inspection form, not the preferred default for ordinary wire-level understanding.
</p>

<h3>15.4 Meaning</h3>

<p>
A port watch attached to an output port SHOULD be semantically equivalent to monitoring the emitted committed value at that source boundary.
A port watch attached to an input port SHOULD reflect the committed value received at that input in the relevant execution context.
</p>

<hr/>

<h2 id="local-memory-watches">16. Local-Memory Watches</h2>

<h3>16.1 Purpose</h3>

<p>
A <code>local_memory_watch</code> is attached to the local-memory slot owned by a local-memory node instance.
Its purpose is to monitor source-aligned state rather than only ordinary edge values.
</p>

<h3>16.2 Canonical case: <code>frog.core.delay</code></h3>

<p>
For <code>frog.core.delay</code>, a local-memory watch SHOULD make it possible to inspect at least, when the active observability support exposes the corresponding information:
</p>

<ul>
  <li>the most recent observed <code>state_read</code>,</li>
  <li>the most recent observed <code>state_updated</code>,</li>
  <li>the current committed stored value visible in a paused snapshot.</li>
</ul>

<h3>16.3 Local scope</h3>

<p>
A local-memory watch MUST respect node-instance-local scope.
It MUST NOT imply shared state across unrelated node instances or unrelated live execution instances.
</p>

<h3>16.4 Meaning</h3>

<p>
A local-memory watch observes source-aligned local-memory activity associated with the node instance.
It does not redefine the primitive-local behavior of <code>frog.core.delay</code> or the language-level semantics of local memory.
</p>

<pre><code>delay node instance
   ├── observed state_read
   ├── observed state_updated
   └── paused current stored value
        (only when exposed by the active observability support)
</code></pre>

<hr/>

<h2 id="ui-related-watches">17. UI-Related Watches</h2>

<h3>17.1 Ordinary widget-related valueflow</h3>

<p>
Ordinary widget-related valueflow SHOULD be watched through the same canonical mechanisms as other dataflow:
</p>

<ul>
  <li>edge watches on edges connected to <code>widget_value</code>,</li>
  <li>port watches on relevant widget-related ports when supported.</li>
</ul>

<h3>17.2 Object-style widget interaction</h3>

<p>
Object-style widget interaction through <code>widget_reference</code>, <code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, and <code>frog.ui.method_invoke</code> MAY be monitored through:
</p>

<ul>
  <li>ordinary watched valueflow when relevant,</li>
  <li>pause-consistent debug inspection,</li>
  <li>stronger-support UI-specific watch renderers.</li>
</ul>

<p>
For ordinary widget value participation in dataflow, tools SHOULD prefer the natural <code>widget_value</code> representation rather than object-style property access.
</p>

<h3>17.3 UI sequencing</h3>

<p>
The <code>ui_in</code> / <code>ui_out</code> sequencing edges are not ordinary data-value edges.
A stronger observability support MAY allow watching such sequencing activity, but it MUST be interpreted as <strong>effect-order observation</strong> rather than ordinary value observation.
</p>

<p>
If supported, such watches MUST remain aligned with both:
</p>

<ul>
  <li>the source-facing widget interaction model,</li>
  <li>the relevant <code>frog.ui.*</code> primitive definitions.</li>
</ul>

<hr/>

<h2 id="watch-lifecycle-and-management">18. Watch Lifecycle and Management</h2>

<h3>18.1 Creation</h3>

<p>
A watch is created when the IDE attaches a new watch entry to a valid target.
Creation MUST fail if the requested target is not valid under the active observability support.
</p>

<h3>18.2 Watch-entry states</h3>

<p>
A watch MAY conceptually be in one of the following states:
</p>

<ul>
  <li><code>created</code> — the watch exists but has not yet received a compatible observation,</li>
  <li><code>live</code> — the watch is receiving live updates,</li>
  <li><code>paused</code> — the watch reflects a paused consistent snapshot,</li>
  <li><code>retained</code> — the watch shows a retained post-execution observation when supported,</li>
  <li><code>unavailable</code> — the target exists but no compatible observation is currently available,</li>
  <li><code>closed</code> — the watch has been removed.</li>
</ul>

<p>
These are watch-entry states.
They MUST NOT be confused with the execution state of the underlying instance.
</p>

<h3>18.3 Management operations</h3>

<p>
A watch-capable IDE SHOULD support at least:
</p>

<ul>
  <li>add watch,</li>
  <li>remove watch,</li>
  <li>list active watches,</li>
  <li>show source target from watch entry,</li>
  <li>show watch entry from source target.</li>
</ul>

<h3>18.4 Optional management behavior</h3>

<p>
A stronger observability support MAY additionally support:
</p>

<ul>
  <li>sorting watches,</li>
  <li>grouping watches by scope or target kind,</li>
  <li>pinning selected watches,</li>
  <li>clearing retained observation state,</li>
  <li>creating a probe from a watch target or a watch from a probe target.</li>
</ul>

<hr/>

<h2 id="watch-presentation-model">19. Watch Presentation Model</h2>

<h3>19.1 General rule</h3>

<p>
This document does not standardize one mandatory watch-window layout.
It standardizes the semantic meaning of watch entries.
</p>

<h3>19.2 Minimal displayed information</h3>

<p>
A watch presentation SHOULD be able to show at least:
</p>

<ul>
  <li>watch target identity,</li>
  <li>watch kind,</li>
  <li>the most recent compatible observation when available,</li>
  <li>watch-entry state such as live, paused, retained, or unavailable,</li>
  <li>basic execution-context information when relevant.</li>
</ul>

<h3>19.3 Possible presentations</h3>

<p>
An IDE MAY present this information through:
</p>

<ul>
  <li>a dedicated watch window,</li>
  <li>a docked watch pane,</li>
  <li>a structured inspection sidebar,</li>
  <li>another equivalent centralized view.</li>
</ul>

<h3>19.4 Structured-value expansion</h3>

<p>
A watch presentation MAY allow expanding structured values into child fields or child elements when the active observability support and available type information permit it.
Such expansion behavior is optional.
</p>

<hr/>

<h2 id="validation-and-safety-rules">20. Validation and Safety Rules</h2>

<ul>
  <li>A watch MUST target a valid source-visible object supported by the active observability support.</li>
  <li>A watch MUST NOT alter execution semantics.</li>
  <li>A watch shown during pause MUST remain consistent with the paused execution snapshot exposed to the IDE.</li>
  <li>A retained watch value MUST be clearly distinguishable from a live-updating value and from a paused-snapshot value.</li>
  <li>A watch MUST NOT misrepresent sequencing observation as an ordinary data value.</li>
  <li>A local-memory watch MUST respect node-instance-local scope.</li>
  <li>A watch MUST NOT imply arbitrary user-defined watch expressions unless a later specification defines them.</li>
  <li>A watch MUST NOT be presented as a probe unless the IDE explicitly re-exposes the same target through the probe model.</li>
  <li>A watch MUST NOT silently conflate observations from unrelated execution instances when retained behavior is supported.</li>
</ul>

<hr/>

<h2 id="illustrative-examples">21. Illustrative Examples</h2>

<p>
The examples below are illustrative only.
They show possible watch-facing representations.
They are <strong>not</strong> a required transport format.
</p>

<h3>21.1 Edge watch</h3>

<pre><code>{
  "watch_id": "w1",
  "kind": "edge_watch",
  "target": {
    "kind": "edge",
    "id": "e_sum"
  },
  "state": "live",
  "last_observation": {
    "preview": "17.5"
  }
}</code></pre>

<h3>21.2 Port watch</h3>

<pre><code>{
  "watch_id": "w2",
  "kind": "port_watch",
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

<h3>21.3 Local-memory watch</h3>

<pre><code>{
  "watch_id": "w3",
  "kind": "local_memory_watch",
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

<h3>21.4 Retained watch</h3>

<pre><code>{
  "watch_id": "w4",
  "kind": "edge_watch",
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

<h2 id="directory-navigation">22. Directory Navigation</h2>

<p>
This document is easier to use when read together with the neighboring IDE documents:
</p>

<pre><code>IDE/
├── Readme.md
│   -&gt; overall IDE ownership, components, and document map
├── Observability.md
│   -&gt; source-aligned execution visibility consumed by watch entries
├── Probes.md
│   -&gt; local source-attached inspection distinct from watches
├── Watch.md
│   -&gt; this document; persistent centralized inspection
└── Debugging.md
    -&gt; interactive debugging behavior compatible with watches
</code></pre>

<p>
For readers following repository ownership boundaries, the main cross-layer path is:
</p>

<pre><code>Expression/
    -&gt; what source-visible watch targets exist

Language/
    -&gt; what execution means and when observation is coherent

IR/
    -&gt; what execution-facing attribution preserves

IDE/Observability.md
    -&gt; what source-aligned observations are exposed

IDE/Watch.md
    -&gt; how those observations are consumed persistently
</code></pre>

<hr/>

<h2 id="out-of-scope">23. Out of Scope</h2>

<ul>
  <li>arbitrary textual watch expressions,</li>
  <li>computed watches over user-defined graph expressions,</li>
  <li>mandatory time history for every watch entry,</li>
  <li>reverse-time watch browsing,</li>
  <li>mandatory remote watch streaming,</li>
  <li>standardized distributed aggregation of watch lists,</li>
  <li>watch-triggered execution modification,</li>
  <li>a mandatory dedicated watch-window UI toolkit or layout.</li>
</ul>

<hr/>

<h2 id="summary">24. Summary</h2>

<p>
FROG watches are <strong>persistent centralized inspection entries</strong> used to monitor selected source-visible targets across execution and debugging activity.
They are distinct from probes, even though both consume the same source-aligned observability.
</p>

<p>
This specification establishes that:
</p>

<ul>
  <li>watches belong to the IDE inspection layer,</li>
  <li>watches track source-visible targets such as edges, ports, and local-memory state,</li>
  <li><code>edge_watch</code> is the minimum and primary watch form,</li>
  <li>watched values represent compatible committed observations,</li>
  <li>paused and retained watch views must be clearly distinguishable,</li>
  <li>watches provide centralized persistent monitoring rather than local inline inspection.</li>
</ul>
