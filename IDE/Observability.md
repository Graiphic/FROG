<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IDE Observability</h1>

<p align="center">
  <strong>Source-aligned execution observability for FROG IDEs</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#document-role">2. Document Role</a></li>
  <li><a href="#repository-position-and-dependencies">3. Repository Position and Dependencies</a></li>
  <li><a href="#ownership-boundary">4. Ownership Boundary</a></li>
  <li><a href="#why-observability-is-a-separate-layer">5. Why Observability Is a Separate Layer</a></li>
  <li><a href="#high-level-observability-model">6. High-Level Observability Model</a></li>
  <li><a href="#source-alignment">7. Source Alignment</a></li>
  <li><a href="#observable-objects">8. Observable Objects</a></li>
  <li><a href="#observable-events-and-snapshots">9. Observable Events and Snapshots</a></li>
  <li><a href="#execution-context-and-identity">10. Execution Context and Identity</a></li>
  <li><a href="#pause-consistent-observation">11. Pause-Consistent Observation</a></li>
  <li><a href="#diagram-front-panel-and-public-interface-observability">12. Diagram, Front Panel, and Public Interface Observability</a></li>
  <li><a href="#probes-and-watches">13. Probes and Watches</a></li>
  <li><a href="#debugging-consumption-of-observability">14. Debugging Consumption of Observability</a></li>
  <li><a href="#fault-observability">15. Fault Observability</a></li>
  <li><a href="#minimum-observability-baseline">16. Minimum Observability Baseline</a></li>
  <li><a href="#illustrative-observability-objects">17. Illustrative Observability Objects</a></li>
  <li><a href="#directory-navigation">18. Directory Navigation</a></li>
  <li><a href="#out-of-scope">19. Out of Scope</a></li>
  <li><a href="#summary">20. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the <strong>execution observability layer</strong> of a FROG IDE.
It specifies how live execution activity may be exposed to IDE tooling in a way that remains <strong>source-aligned</strong>, <strong>context-aware</strong>, and <strong>architecturally separate</strong> from both execution semantics and runtime-private implementation details.
</p>

<p>
In FROG, observability is the architectural bridge between:
</p>

<ul>
  <li>live execution on a target runtime or backend,</li>
  <li>IDE-facing inspection tools,</li>
  <li>debugging control,</li>
  <li>probes,</li>
  <li>watches,</li>
  <li>execution overlays and highlighting.</li>
</ul>

<p>
Observability therefore does not define what execution means.
Execution meaning remains owned by the validated program meaning and by the relevant language, library, and profile specifications.
Observability defines how execution is <strong>projected back</strong> to meaningful FROG objects so that tools can inspect, highlight, pause, debug, and explain a live run without relying on opaque backend-private internals.
</p>

<pre><code>Language/
    -&gt; owns execution meaning

IR/
    -&gt; owns canonical execution-facing representation and attribution posture

IDE/Observability.md
    -&gt; owns the source-aligned observable projection consumed by tools

IDE/Debugging.md
    -&gt; owns interactive debug control built on top of observability
</code></pre>

<hr/>

<h2 id="document-role">2. Document Role</h2>

<p>
This document is the observability-focused document of the <code>IDE/</code> family.
Its role is to define the source-aligned visible world that IDE tools consume when they need to inspect or control a live execution.
</p>

<p>
It should be read together with:
</p>

<ul>
  <li><code>IDE/Readme.md</code> for the overall IDE architecture and ownership boundary,</li>
  <li><code>IDE/Debugging.md</code> for interactive debugging control,</li>
  <li><code>IDE/Probes.md</code> for local inspection tools,</li>
  <li><code>IDE/Watch.md</code> for persistent centralized inspection,</li>
  <li><code>IR/Execution IR.md</code> and <code>IR/Derivation rules.md</code> for execution-facing attribution and mapping posture.</li>
</ul>

<p>
This document is intentionally modular.
It defines observability only.
It does not absorb execution semantics, debugger control, probe behavior in full, watch behavior in full, or implementation-private runtime design.
</p>

<hr/>

<h2 id="repository-position-and-dependencies">3. Repository Position and Dependencies</h2>

<h3>3.1 Repository position</h3>

<p>
This document belongs in <code>IDE/</code> because it defines how live execution becomes visible to IDE tools.
It does <strong>not</strong> belong to <code>Language/</code> because it does not own execution meaning.
It does <strong>not</strong> belong to <code>Implementations/</code> because it is not a private runtime mechanism.
It is an IDE-facing architectural contract.
</p>

<h3>3.2 Dependencies</h3>

<p>
This document depends on the following repository surfaces:
</p>

<ul>
  <li><code>IDE/Readme.md</code> — architectural ownership of authoring, observability, debugging, probes, and watches,</li>
  <li><code>IDE/Debugging.md</code> — interactive debugging controls that consume observability,</li>
  <li><code>IDE/Probes.md</code> — local inspection tools built on the observable layer,</li>
  <li><code>IDE/Watch.md</code> — persistent centralized inspection built on the observable layer,</li>
  <li><code>Expression/Readme.md</code> — canonical source layer,</li>
  <li><code>Expression/Diagram.md</code> — source-visible executable graph objects,</li>
  <li><code>Expression/Front panel.md</code> — source-visible front-panel layer,</li>
  <li><code>Expression/Widget.md</code> — source-visible widget instance model,</li>
  <li><code>Expression/Widget interaction.md</code> — source-visible widget interaction paths,</li>
  <li><code>Expression/Source provenance.md</code> — optional source-carried authoring and review evidence,</li>
  <li><code>IDE/Authoring provenance.md</code> — IDE-side provenance verification, display, and review workflow,</li>
  <li><code>Language/Readme.md</code> — normative execution semantics,</li>
  <li><code>Language/Control structures.md</code> — normative structure execution meaning,</li>
  <li><code>Language/State and cycles.md</code> — normative local-memory meaning,</li>
  <li><code>IR/Readme.md</code> — execution-facing representation boundary,</li>
  <li><code>IR/Execution IR.md</code> — execution-facing representation and attribution posture,</li>
  <li><code>IR/Derivation rules.md</code> — source-to-execution-facing correspondence rules.</li>
</ul>

<h3>3.3 Dependency diagram</h3>

<pre><code>Expression/ + Language/ + Libraries/ + Profiles/
                      |
                      v
                     IR/
                      |
                      v
          IDE/Observability.md
             /         |         \
            v          v          v
   IDE/Debugging.md  IDE/Probes.md  IDE/Watch.md
</code></pre>

<hr/>

<h2 id="ownership-boundary">4. Ownership Boundary</h2>

<p>
If a conflict appears, the following ownership rules apply:
</p>

<ul>
  <li><code>Expression/</code> remains authoritative for canonical source-visible object identity,</li>
  <li><code>Language/</code> remains authoritative for execution semantics and safe-state meaning,</li>
  <li><code>Libraries/</code> and <code>Profiles/</code> remain authoritative for primitive-local and capability-local behavior,</li>
  <li><code>IR/</code> remains authoritative for canonical execution-facing derivation posture,</li>
  <li><code>IDE/Observability.md</code> remains authoritative only for how execution becomes observable to tools in source-aligned terms.</li>
</ul>

<pre><code>Expression/
    owns what source objects exist

Language/
    owns what execution means

IR/
    owns how validated meaning is represented for execution-facing consumption

IDE/Observability.md
    owns how live execution is projected back to meaningful source objects
</code></pre>

<p>
Observability must therefore never be mistaken for execution semantics.
If an implementation can observe something privately but cannot attribute it back to a meaningful FROG object, that private signal alone is not sufficient to count as source-aligned FROG observability.
</p>

<hr/>

<h2 id="why-observability-is-a-separate-layer">5. Why Observability Is a Separate Layer</h2>

<p>
A graphical dataflow environment needs more than raw telemetry and more than a debugger stop command.
It needs a stable architectural layer able to answer questions such as:
</p>

<ul>
  <li>which source node is active,</li>
  <li>which edge currently carries a value,</li>
  <li>which structure region is active,</li>
  <li>which loop iteration produced this observation,</li>
  <li>which widget-related object this value belongs to,</li>
  <li>which public-interface object this runtime event corresponds to.</li>
</ul>

<p>
That is why observability is separated from both:
</p>

<ul>
  <li>runtime-private mechanics,</li>
  <li>interactive debugger control.</li>
</ul>

<p>
The runtime may implement many private scheduling and execution details.
The debugger may expose pause, resume, step, and break behavior.
Observability is the shared source-aligned projection layer that makes both inspection and debugging intelligible.
</p>

<p>
A provenance-aware IDE MAY combine observability overlays with provenance overlays so that a reviewer can see both live execution state and authoring/review state. These overlays MUST remain distinguishable: execution observability describes what is happening at runtime, while provenance describes source-carried authoring evidence and review status.
</p>

<hr/>

<h2 id="high-level-observability-model">6. High-Level Observability Model</h2>

<p>
The high-level model is:
</p>

<pre><code>validated program meaning
        |
        v
execution-facing representation
        |
        v
target execution instance
        |
        v
source-aligned observability projection
        |
        +-- execution highlighting
        +-- debugging
        +-- probes
        \-- watches
</code></pre>

<p>
A more explicit architectural reading is:
</p>

<pre><code>target execution instance
        |
        v
source-aligned observability projection
   /              |               |                 \
  v               v               v                  v
debugging   local probes   watch entries   execution overlays
</code></pre>

<p>
This projection is expected to preserve:
</p>

<ul>
  <li>source object identity,</li>
  <li>dynamic execution context,</li>
  <li>causal ordering compatible with source meaning,</li>
  <li>pause-consistent inspection when paused.</li>
</ul>

<hr/>

<h2 id="source-alignment">7. Source Alignment</h2>

<p>
Observability in FROG MUST be <strong>source-aligned</strong>.
That means the IDE-facing observable world is expressed in terms of objects that remain meaningful from the point of view of a FROG developer.
</p>

<p>
Examples of source-aligned observable objects include:
</p>

<ul>
  <li>diagram nodes,</li>
  <li>diagram edges,</li>
  <li>structure nodes and selected regions,</li>
  <li>sub-FROG call sites,</li>
  <li>loop iterations,</li>
  <li>local-memory objects,</li>
  <li>front-panel widgets where relevant,</li>
  <li>widget-related diagram execution objects such as <code>widget_value</code> and <code>widget_reference</code>.</li>
</ul>

<p>
An implementation MAY use private runtime identifiers internally, but a conforming observability layer MUST be able to map the user-facing observation back to meaningful source identity.
</p>

<hr/>

<h2 id="observable-objects">8. Observable Objects</h2>

<h3>8.1 General rule</h3>

<p>
An observable object is a source-aligned execution object for which the observability layer can expose activity, values, state, or contextualized execution presence.
</p>

<h3>8.2 Minimum object families</h3>

<p>
The minimum standardized observable object families are:
</p>

<ul>
  <li><code>node</code>,</li>
  <li><code>edge</code>,</li>
  <li><code>structure</code>,</li>
  <li><code>region</code>,</li>
  <li><code>subfrog_call</code>,</li>
  <li><code>local_state</code>.</li>
</ul>

<h3>8.3 UI-related observable families</h3>

<p>
When the active example or runtime path includes front-panel or widget interaction participation, the observability layer SHOULD also be able to expose source-aligned visibility for:
</p>

<ul>
  <li><code>widget_value</code>,</li>
  <li><code>widget_reference</code>,</li>
  <li><code>frog.ui.property_read</code>,</li>
  <li><code>frog.ui.property_write</code>,</li>
  <li><code>frog.ui.method_invoke</code>.</li>
</ul>

<h3>8.4 Front-panel reflection is secondary</h3>

<p>
A front panel MAY reflect observable state.
However, the front panel is not the authoritative execution graph.
Accordingly, source-aligned observability remains primarily diagram-aligned even when front-panel reflections are also available.
</p>

<hr/>

<h2 id="observable-events-and-snapshots">9. Observable Events and Snapshots</h2>

<h3>9.1 Observable events</h3>

<p>
The observability layer MAY expose event-like notifications such as:
</p>

<ul>
  <li>node activation start,</li>
  <li>node activation completion,</li>
  <li>edge value availability,</li>
  <li>structure activation start,</li>
  <li>region selection,</li>
  <li>loop iteration progression,</li>
  <li>fault association with a source-aligned object.</li>
</ul>

<p>
These events are tool-facing observations.
They do not define semantics by themselves.
</p>

<h3>9.2 Observable snapshots</h3>

<p>
The observability layer MAY also expose snapshots.
A snapshot is a coherent IDE-facing view of currently relevant committed execution state.
</p>

<p>
A useful snapshot may include:
</p>

<ul>
  <li>selected active object,</li>
  <li>visible committed edge values,</li>
  <li>visible committed local state,</li>
  <li>current structure stack,</li>
  <li>current iteration context,</li>
  <li>current widget-related state when relevant.</li>
</ul>

<h3>9.3 Event and snapshot coexistence</h3>

<p>
A conforming implementation MAY provide:
</p>

<ul>
  <li>event streams only,</li>
  <li>snapshots only,</li>
  <li>or both.</li>
</ul>

<p>
The important requirement is not the transport shape.
The important requirement is source alignment and consistency.
</p>

<hr/>

<h2 id="execution-context-and-identity">10. Execution Context and Identity</h2>

<h3>10.1 Why context matters</h3>

<p>
The same source object may be activated multiple times in one run.
Therefore, observability must preserve not only static identity, but also relevant execution context.
</p>

<h3>10.2 Minimum context fields</h3>

<p>
The observability layer SHOULD preserve, when relevant:
</p>

<ul>
  <li>instance identity,</li>
  <li>sub-FROG nesting context,</li>
  <li>structure stack,</li>
  <li>selected region identity,</li>
  <li>loop iteration identity,</li>
  <li>selected object identity.</li>
</ul>

<h3>10.3 Identity / mapping preservation</h3>

<p>
The observability layer should remain compatible with the repository-wide rule of identity and mapping preservation:
a reader should be able to follow meaningful attribution from source-level object identity toward execution-facing and live-observable activity.
</p>

<pre><code>source object
    -&gt; validated meaning
    -&gt; execution-facing representation
    -&gt; target execution activity
    -&gt; observable source-aligned projection
</code></pre>

<hr/>

<h2 id="pause-consistent-observation">11. Pause-Consistent Observation</h2>

<p>
Where <code>ide.provenance</code> exists, source-aligned identity also lets the IDE associate an observable object with its provenance state. That association is useful for inspection and review, but it is not a runtime semantic fact.
</p>



<p>
When a live execution is paused for debugging or fault inspection, the observability layer MUST be able to support a <strong>pause-consistent</strong> IDE view.
</p>

<p>
That means the IDE-facing observable state must not contradict the committed source-level meaning of the paused execution prefix.
</p>

<p>
At minimum, a pause-consistent observation SHOULD allow the IDE to rely on the following:
</p>

<ul>
  <li>displayed committed node completions are stable,</li>
  <li>displayed committed edge values are stable,</li>
  <li>displayed committed local-memory state is stable,</li>
  <li>displayed structure / region context is coherent,</li>
  <li>displayed fault association, if any, is not contradicted by a partially presented source state.</li>
</ul>

<p>
This document consumes language-level safe-state guarantees where those are defined.
It does not define execution semantics by itself.
</p>

<hr/>

<h2 id="diagram-front-panel-and-public-interface-observability">12. Diagram, Front Panel, and Public Interface Observability</h2>

<h3>12.1 Diagram-first posture</h3>

<p>
Diagram observability is primary because the diagram is the authoritative executable graph.
</p>

<h3>12.2 Front-panel observability</h3>

<p>
Front-panel reflections MAY be exposed when they are source-meaningful.
Examples include:
</p>

<ul>
  <li>current widget value,</li>
  <li>indicator publication result,</li>
  <li>widget property changes relevant to explicit UI interaction nodes.</li>
</ul>

<h3>12.3 Public-interface observability</h3>

<p>
Public-interface participation MAY also be observable where an IDE or runtime can meaningfully attribute activity to:
</p>

<ul>
  <li>interface inputs,</li>
  <li>interface outputs,</li>
  <li>public publication events.</li>
</ul>

<p>
However, diagram objects remain the primary source-aligned execution surfaces for debugger stops and fine-grained dataflow inspection.
</p>

<hr/>

<h2 id="probes-and-watches">13. Probes and Watches</h2>

<h3>13.1 Observability is upstream of probes and watches</h3>

<p>
Probes and watches consume the observability layer.
They are not alternative execution models.
They are inspection tools built on top of source-aligned projection.
</p>

<h3>13.2 Probes</h3>

<p>
A probe is a local inspection object attached to a graph-meaningful surface.
Typical targets include:
</p>

<ul>
  <li>edges,</li>
  <li>ports,</li>
  <li>selected local-state objects,</li>
  <li>other source-aligned observable objects when supported.</li>
</ul>

<p>
Probes are usually immediate and localized.
They depend on observability to obtain source-aligned values and context.
</p>

<h3>13.3 Watches</h3>

<p>
A watch is a persistent observation surface.
It tracks selected source-aligned objects or values across longer execution and debugging workflows.
</p>

<p>
Watches depend on observability to:
</p>

<ul>
  <li>resolve source identity,</li>
  <li>receive updated observed values or states,</li>
  <li>preserve relevant execution context when necessary.</li>
</ul>

<h3>13.4 Observability does not own their UI</h3>

<p>
This document defines the shared source-aligned substrate they consume.
It does not standardize their full user-interface behavior.
</p>

<hr/>

<h2 id="debugging-consumption-of-observability">14. Debugging Consumption of Observability</h2>

<p>
Interactive debugging is defined on top of observability rather than beside it.
</p>

<p>
The debugger uses observability to:
</p>

<ul>
  <li>highlight active source objects,</li>
  <li>localize pause points,</li>
  <li>show coherent paused state,</li>
  <li>identify relevant loop / structure / sub-FROG context,</li>
  <li>support step completion in source-aligned terms.</li>
</ul>

<p>
Accordingly:
</p>

<pre><code>observability
    -&gt; makes execution visible

debugging
    -&gt; controls execution using that visible source-aligned world
</code></pre>

<p>
The same architectural rule must remain visible at repository level:
debugging, probes, and watches are parallel consumers of observability, not subfeatures hidden inside one another.
</p>

<hr/>

<h2 id="fault-observability">15. Fault Observability</h2>

<p>
When a runtime or backend can associate a fault with a meaningful source-aligned object, the observability layer SHOULD preserve that association for IDE consumers.
</p>

<p>
Useful fault observability may include:
</p>

<ul>
  <li>fault kind,</li>
  <li>primary associated source object,</li>
  <li>relevant context stack,</li>
  <li>whether the fault was observed at a pause-consistent state or only as terminal failure.</li>
</ul>

<p>
Fault observability must still remain source-aligned.
A backend-private crash address alone is not sufficient as FROG-level fault observability.
</p>

<hr/>

<h2 id="minimum-observability-baseline">16. Minimum Observability Baseline</h2>

<p>
A minimum observability-capable IDE/runtime pairing SHOULD be able to expose at least:
</p>

<ul>
  <li>node-level activity visibility,</li>
  <li>edge-level value availability visibility,</li>
  <li>structure and selected-region context visibility,</li>
  <li>loop iteration context when relevant,</li>
  <li>pause-consistent observable state when paused,</li>
  <li>source identity for selected observable objects.</li>
</ul>

<p>
Stronger support levels MAY add:
</p>

<ul>
  <li>widget-related source observability,</li>
  <li>more detailed local-state observability,</li>
  <li>readiness-aware overlays,</li>
  <li>richer fault localization,</li>
  <li>deeper value inspection.</li>
</ul>

<hr/>

<h2 id="illustrative-observability-objects">17. Illustrative Observability Objects</h2>

<p>
The examples below are illustrative only.
They show possible observability-facing objects.
They are <strong>not</strong> a required transport format.
</p>

<h3>17.1 Edge value event</h3>

<pre><code>{
  "kind": "edge_value_available",
  "instance_id": "run_42",
  "target": {
    "kind": "edge",
    "id": "e_sum"
  },
  "context": {
    "subfrog_stack": [],
    "structure_stack": [
      {
        "structure_id": "loop_1",
        "region_id": "body",
        "iteration": 3
      }
    ]
  }
}</code></pre>

<h3>17.2 Selected structure context snapshot</h3>

<pre><code>{
  "kind": "pause_snapshot",
  "instance_id": "run_42",
  "selected_object": {
    "kind": "node",
    "id": "add_1"
  },
  "context": {
    "subfrog_stack": [],
    "structure_stack": [
      {
        "structure_id": "loop_1",
        "region_id": "body",
        "iteration": 3
      }
    ]
  },
  "committed_state": {
    "visible_edges": ["e_sum"],
    "visible_local_state": ["delay_1"]
  }
}</code></pre>

<h3>17.3 Fault-associated observable object</h3>

<pre><code>{
  "kind": "fault_observation",
  "instance_id": "run_42",
  "fault_kind": "ui_write_failure",
  "primary_object": {
    "kind": "node",
    "id": "write_gain"
  }
}</code></pre>

<hr/>

<h2 id="directory-navigation">18. Directory Navigation</h2>

<p>
This document is easier to use when read together with the neighboring IDE documents:
</p>

<pre><code>IDE/
├── Readme.md
│   -&gt; overall IDE ownership, components, and document map
├── Observability.md
│   -&gt; this document; source-aligned execution visibility
├── Debugging.md
│   -&gt; interactive debugger control built on this observable layer
├── Probes.md
│   -&gt; local inspection tools consuming the observable layer
└── Watch.md
    -&gt; persistent inspection tools consuming the observable layer
</code></pre>

<p>
For readers following repository ownership boundaries, the main cross-layer path is:
</p>

<pre><code>Expression/
    -&gt; what source objects exist

Language/
    -&gt; what execution means

IR/
    -&gt; what execution-facing derivation preserves

IDE/Observability.md
    -&gt; how execution becomes visible back in source terms
</code></pre>

<hr/>

<h2 id="out-of-scope">19. Out of Scope</h2>

<p>
The following topics remain intentionally out of scope for this document:
</p>

<ul>
  <li>full runtime telemetry protocols,</li>
  <li>distributed tracing and remote fleet observability,</li>
  <li>reverse execution,</li>
  <li>deterministic replay,</li>
  <li>performance profiling,</li>
  <li>deep serialization formats for every possible value family,</li>
  <li>full watch-expression semantics,</li>
  <li>UI rendering of probes and watches,</li>
  <li>implementation-private scheduler state presentation.</li>
</ul>

<hr/>

<h2 id="summary">20. Summary</h2>

<p>
The FROG observability layer is the source-aligned projection of live execution back onto meaningful FROG objects.
It is the architectural substrate used by execution overlays, probes, watches, and interactive debugging.
</p>

<p>
This specification standardizes:
</p>

<ul>
  <li>observable source-aligned object families,</li>
  <li>observable events and snapshots,</li>
  <li>identity and mapping preservation,</li>
  <li>pause-consistent observation,</li>
  <li>the observability relationship with debugging, probes, and watches.</li>
</ul>

<p>
This preserves the core architectural distinction between:
</p>

<ul>
  <li>execution meaning,</li>
  <li>execution-facing derivation,</li>
  <li>source-aligned observability,</li>
  <li>interactive debugger control,</li>
  <li>inspection tools built on top of that visible world.</li>
</ul>
