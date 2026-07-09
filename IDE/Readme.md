<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IDE Architecture</h1>

<p align="center">
  <strong>Architecture and responsibilities of a FROG development environment</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#document-role">2. Document Role</a></li>
  <li><a href="#repository-navigation">3. Repository Navigation</a></li>
  <li><a href="#scope">4. Scope of this Document</a></li>
  <li><a href="#relation-with-other-specifications">5. Relation with Other Specifications</a></li>
  <li><a href="#responsibility-boundaries">6. Responsibility Boundaries</a></li>
  <li><a href="#high-level-architecture">7. High-Level Architecture</a></li>
  <li><a href="#current-documents">8. Current Documents</a></li>
  <li><a href="#main-components">9. Main Components</a></li>
  <li><a href="#ide-shell">10. IDE Shell</a></li>
  <li><a href="#diagram-editor">11. Diagram Editor</a></li>
  <li><a href="#front-panel-editor">12. Front Panel Editor</a></li>
  <li><a href="#frog-program-model">13. FROG Program Model</a></li>
  <li><a href="#relation-with-frog-expression">14. Relation with FROG Expression</a></li>
  <li><a href="#execution-integration-boundary">15. Execution Integration Boundary</a></li>
  <li><a href="#palette">16. Palette</a></li>
  <li><a href="#express-authoring">17. Express Authoring</a></li>
  <li><a href="#execution-observability">18. Execution Observability</a></li>
  <li><a href="#debugging">19. Debugging</a></li>
  <li><a href="#probes">20. Probes</a></li>
  <li><a href="#watch">21. Watch</a></li>
  <li><a href="#snippets">22. Snippets</a></li>
  <li><a href="#design-principles">23. Design Principles</a></li>
  <li><a href="#repository-direction">24. Repository Direction</a></li>
  <li><a href="#summary">25. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The FROG IDE is the graphical development environment used to design, edit, validate, inspect, and debug programs written in the FROG graphical dataflow language.
A conforming IDE MAY additionally integrate execution-facing services such as validation pipelines, execution preparation, compiler services, runtime services, or equivalent subsystems.
</p>

<p>
A FROG IDE does not treat the saved <code>.frog</code> file as the live editable artifact.
Instead, it loads and maintains an internal <strong>FROG Program Model</strong>, which is the canonical editable in-memory representation used during authoring.
The canonical saved source file remains the <strong>FROG Expression</strong>, defined in <code>Expression/</code>.
</p>

<p>
Interactive inspection and debugging are not performed directly on raw serialized source.
They are performed on a live execution derived from validated canonical program content, through a source-aligned observability layer that allows the IDE to project runtime activity back onto diagram-visible, front-panel-visible, and other source-meaningful objects.
</p>

<p>
A conforming IDE MAY expose authoring-facing views, shortcuts, aliases, assistant-driven insertion flows, guided entries, and other conveniences that are more ergonomic than the raw canonical source representation.
However, those conveniences do not become new language constructs by themselves.
The IDE remains responsible for preserving canonical source identity and for keeping semantic ownership in the specification layers that define it normatively.
</p>

<p>
This includes <strong>Express authoring</strong>:
an IDE MAY provide guided, configurable, task-oriented insertion experiences for common operations, but those experiences MUST normalize to canonical FROG content already owned by <code>Expression/</code>, <code>Language/</code>, <code>Libraries/</code>, and <code>Profiles/</code> where relevant.
</p>

<p>
This document defines how a conforming FROG IDE is organized as an authoring, discoverability, observability, debugging, inspection, and transport environment without making the language dependent on one particular editor implementation.
</p>

<p>
<code>IDE/</code> defines IDE-facing architecture for conforming tools.
Proprietary IDE implementations may exist and do not redefine the FROG language.
Editor efforts currently tracked around the language are listed in
<a href="./Editors.md">Editors and authoring tools</a>.
</p>

<pre><code>Repository architecture around IDE/

Expression/   -&gt; canonical source form
Language/     -&gt; normative execution semantics
Libraries/    -&gt; intrinsic primitive vocabularies
Profiles/     -&gt; optional standardized capability families
IR/           -&gt; canonical execution-facing representation and downstream boundaries
IDE/          -&gt; authoring, discoverability, observability, debugging, inspection

IDE/ own tooling behavior.
IDE/ do not own source meaning or execution meaning.
</code></pre>

<pre><code>Mental model in one glance

User edits in the IDE
        |
        v
IDE maintains Program Model
        |
        v
Program Model serializes to canonical .frog source
        |
        v
validated canonical content is handed to execution-facing systems
        |
        v
runtime activity comes back through source-aligned observability
        |
        +-&gt; debugging
        +-&gt; probes
        \-&gt; watches
</code></pre>

<hr/>

<h2 id="document-role">2. Document Role</h2>

<p>
This document is the architectural entry point of the <code>IDE/</code> repository family.
It explains:
</p>

<ul>
  <li>what the IDE layer owns,</li>
  <li>what it does not own,</li>
  <li>how the documents inside <code>IDE/</code> are organized,</li>
  <li>how IDE-facing concerns relate to <code>Expression/</code>, <code>Language/</code>, <code>IR/</code>, <code>Libraries/</code>, and <code>Profiles/</code>.</li>
</ul>

<p>
It should be read first when navigating the IDE-facing documentation.
The other documents in <code>IDE/</code> refine individual subdomains such as palette behavior, observability, debugging, probes, watches, and snippets.
</p>

<hr/>

<h2 id="repository-navigation">3. Repository Navigation</h2>

<p>
The current IDE-facing repository family is organized as follows:
</p>

<pre><code>IDE/
├── Readme.md
│   -&gt; this document, the architectural entry point
├── Palette.md
│   -&gt; palette model for the active insertable space
├── Express.md
│   -&gt; guided authoring and normalization to canonical content
├── Observability.md
│   -&gt; source-aligned execution observability contract for IDE tooling
├── Debugging.md
│   -&gt; interactive debugging control behavior
├── Probes.md
│   -&gt; local inspection probes for values and selected execution state
├── Watch.md
│   -&gt; persistent centralized watch-based inspection model
└── Snippet.md
    -&gt; image-backed snippet capture, transport, paste, and reuse workflows
</code></pre>

<p>
The main upstream repository families that the IDE layer depends on are:
</p>

<pre><code>Expression/
    -&gt; canonical source representation

Language/
    -&gt; normative execution semantics

Libraries/
    -&gt; intrinsic standardized primitive vocabularies

Profiles/
    -&gt; optional standardized capability families

IR/
    -&gt; canonical execution-facing representation and downstream handoff boundaries
</code></pre>

<p>
This navigation matters because a FROG IDE is not a self-contained owner of language truth.
It is a tooling layer built on top of source, meaning, execution-facing derivation, and observable execution projection.
</p>

<hr/>

<h2 id="scope">4. Scope of this Document</h2>

<p>
This document specifies the architectural responsibilities of a FROG IDE.
</p>

<p>
It defines:
</p>

<ul>
  <li>the role of the IDE shell,</li>
  <li>the role of diagram and front-panel editors,</li>
  <li>the role of the IDE-managed editable Program Model,</li>
  <li>the IDE-facing boundary between authoring and execution-facing integration,</li>
  <li>the role of palette-driven authoring,</li>
  <li>the architectural role of Express authoring,</li>
  <li>the role of execution observability,</li>
  <li>the role of interactive debugging,</li>
  <li>the role of probes,</li>
  <li>the role of watch-based inspection,</li>
  <li>the role of snippet-based authoring transport.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>the canonical <code>.frog</code> source format in full,</li>
  <li>the normative execution semantics of the language,</li>
  <li>the complete intrinsic primitive catalogs,</li>
  <li>the complete optional profile catalogs,</li>
  <li>the full compiler architecture,</li>
  <li>the full runtime architecture,</li>
  <li>the full execution-oriented IR architecture.</li>
</ul>

<pre><code>What this document owns
- IDE-facing architecture
- editing model
- discoverability model
- observability integration
- debugging / probes / watch / snippets
- Express behavior as an IDE concern

What this document does not own
- canonical source structure
- language semantics
- primitive semantics
- profile capability semantics
- compiler / runtime internals
- IR ownership
</code></pre>

<hr/>

<h2 id="relation-with-other-specifications">5. Relation with Other Specifications</h2>

<p>
The FROG repository separates several normative concerns.
</p>

<ul>
  <li><code>Expression/</code> defines the canonical source representation of a FROG program.</li>
  <li><code>Language/</code> defines cross-cutting normative execution semantics.</li>
  <li><code>Libraries/</code> defines intrinsic standardized primitive catalogs and primitive-local semantics.</li>
  <li><code>Profiles/</code> defines optional standardized capability families and profile-owned capability contracts.</li>
  <li><code>IR/</code> defines canonical execution-facing representation, derivation, construction, lowering, and backend contracts.</li>
  <li><code>IDE/</code> defines authoring, discoverability, observability, debugging, inspection, snippet behavior, and IDE-facing insertion behavior.</li>
</ul>

<p>
Accordingly:
</p>

<ul>
  <li>this document is IDE-facing,</li>
  <li>it may reference source, language, intrinsic-library, profile-owned, IR-side, and implementation-supported extension concepts,</li>
  <li>but it does not replace the specifications that own those concepts normatively.</li>
</ul>

<pre><code>Ownership summary

Canonical source identity          -&gt; Expression/
Execution semantics               -&gt; Language/
Intrinsic primitive-local meaning -&gt; Libraries/
Optional capability meaning       -&gt; Profiles/
Execution-facing representation   -&gt; IR/
Tooling behavior and UX           -&gt; IDE/
</code></pre>

<pre><code>One construct, multiple layers

Example: widget interaction

Expression/  -&gt; widget objects and source-facing interaction representation
Libraries/   -&gt; frog.ui.* primitive contracts
Language/    -&gt; cross-cutting execution semantics
IR/          -&gt; canonical execution-facing representation after validation
IDE/         -&gt; how the user inserts, edits, observes, and debugs it
</code></pre>

<hr/>

<h2 id="responsibility-boundaries">6. Responsibility Boundaries</h2>

<p>
A conforming FROG IDE MUST preserve the architectural separation between:
</p>

<ul>
  <li>authoring,</li>
  <li>canonical source serialization,</li>
  <li>cross-cutting execution semantics,</li>
  <li>intrinsic primitive definitions,</li>
  <li>optional profile-owned capability definitions,</li>
  <li>execution-oriented processing,</li>
  <li>runtime execution,</li>
  <li>canonical execution-facing representation,</li>
  <li>source-aligned observability,</li>
  <li>interactive debugging and inspection.</li>
</ul>

<p>
In particular:
</p>

<ul>
  <li>the IDE edits programs through an IDE-managed Program Model,</li>
  <li>the canonical saved artifact remains the <code>.frog</code> source representation,</li>
  <li>language meaning is not defined by the editor UI,</li>
  <li>primitive meaning is not defined by palette placement,</li>
  <li>profile support is not defined by authoring convenience alone,</li>
  <li>debugging and inspection do not redefine execution semantics,</li>
  <li>the IDE MUST NOT treat its own Program Model as though it were the normative Execution IR.</li>
</ul>

<pre><code>Boundary rule

Nice IDE view
     -&gt;
canonical Program Model identity
     -&gt;
canonical source identity
     -&gt;
validated execution-facing content

Never:

nice IDE view
     -&gt;
new hidden language construct
</code></pre>

<hr/>

<h2 id="high-level-architecture">7. High-Level Architecture</h2>

<p>
The IDE architecture combines:
</p>

<ul>
  <li>authoring surfaces,</li>
  <li>an editable in-memory Program Model,</li>
  <li>serialization and reload services,</li>
  <li>execution-facing integration boundaries,</li>
  <li>source-aligned observability intake,</li>
  <li>debugging and inspection tools.</li>
</ul>

<pre><code>                                   FROG IDE
                 +--------------------------------------------------+
                 | Diagram + Front Panel UI + Probes + Watches      |
                 +--------------------------+-----------------------+
                                            |
                                            v
                                 FROG Program Model
                           (editable in-memory source model)
                                            |
                     +----------------------+----------------------+
                     |                                             |
                     | save / load                                 | execute / validate
                     v                                             v
           OPEN SOURCE LAYER                              Validation against
           FROG Expression                                +----------------------+
           (.frog, canonical source)                      | Expression/          |
                                                          | Language/            |
                                                          | Libraries/           |
                                                          | Profiles/            |
                                                          +----------+-----------+
                                                                     |
                                                                     v
                                                   OPEN EXECUTION LAYER
                                                   FROG Execution IR
                        (canonical execution-facing document, derived, inspectable,
                           source-attributed, execution-facing, not backend-private)
                                                                     |
                                                                     v
                                                     Identity / Mapping preservation
                                                                     |
                                                                     v
                                                    Lowering / backend-facing handoff
                                                                     |
                                                                     v
                                          Compiler(s) / Backend(s) / Runtime(s)
                                                                     |
                                                                     v
                                                       Target execution instance
                                                                     |
                                                                     v
                                           Source-aligned execution observability
                                          (mapped back to meaningful FROG objects)
                                               /                |                 \
                                              /                 |                  \
                                             v                  v                   v
                                       Debugging            Probes               Watches
                                  pause / resume /      local source-       persistent centralized
                                   break / step         attached view          inspection view
</code></pre>

<p>
This architecture is conceptual.
Different IDE implementations MAY distribute these responsibilities differently across modules or processes, but the responsibility boundaries SHOULD remain stable.
</p>

<pre><code>Authoring / execution split

Editors + Palette + Express
            |
            v
      Program Model
            |
            v
     canonical source
            |
            v
validation / preparation
            |
            v
execution-facing systems
            |
            v
observability back to the IDE
            |
            +-&gt; debugging
            +-&gt; probes
            \-&gt; watches
</code></pre>

<hr/>

<h2 id="current-documents">8. Current Documents</h2>

<p>
The IDE layer is currently organized as follows:
</p>

<pre><code>IDE/
├── Readme.md
│   -&gt; architectural entry point for the FROG IDE and Program Model
├── Palette.md
│   -&gt; palette model for the active insertable space, including primitives,
│      structures, node insertions, annotations, guided entries,
│      conditional profile-defined entries, and third-party entries
├── Express.md
│   -&gt; guided Express authoring model and normalization to canonical FROG content
├── Authoring provenance.md
│   -&gt; IDE-side creation, verification, display, review, and preservation workflows for source-carried provenance
├── Observability.md
│   -&gt; source-aligned live execution observability contract for IDE tooling
├── Debugging.md
│   -&gt; interactive debugging behavior built on source-aligned observability
├── Probes.md
│   -&gt; live local inspection probes for values and selected execution state
├── Watch.md
│   -&gt; persistent centralized watch-based inspection model
└── Snippet.md
    -&gt; image-backed snippet capture, transport, paste, and reuse workflows
</code></pre>

<p>
Together, these documents define the current IDE-facing baseline for authoring, discoverability, source-carried provenance display and review, source-aligned execution visibility, interactive debugging, inspection, and portable authoring-fragment transport.
</p>

<hr/>

<h2 id="main-components">9. Main Components</h2>

<p>
A FROG IDE typically includes the following architectural components:
</p>

<ul>
  <li>an IDE shell,</li>
  <li>a diagram editor,</li>
  <li>a front-panel editor,</li>
  <li>a FROG Program Model,</li>
  <li>source serialization and reload services,</li>
  <li>validation and execution-preparation integration,</li>
  <li>palette services,</li>
  <li>Express authoring services,</li>
  <li>authoring provenance services,</li>
  <li>execution observability integration,</li>
  <li>debugging services,</li>
  <li>probe services,</li>
  <li>watch services,</li>
  <li>snippet services.</li>
</ul>

<pre><code>Main component grouping

Authoring components
- IDE shell
- diagram editor
- front panel editor
- palette
- Express services
- authoring provenance services
- snippets

Model components
- Program Model
- serialization / reload
- consistency management

Execution-facing integration
- validation gateway
- execution preparation bridge
- observability intake
- debugging / probe / watch consumers
</code></pre>

<hr/>

<h2 id="ide-shell">10. IDE Shell</h2>

<p>
The IDE shell is the host application responsible for coordinating the development environment.
It manages the surrounding application experience rather than the language itself.
</p>

<p>
Typical responsibilities include:
</p>

<ul>
  <li>window and layout management,</li>
  <li>menus, toolbars, and command routing,</li>
  <li>project and file navigation,</li>
  <li>document lifecycle management,</li>
  <li>plugin and extension loading,</li>
  <li>integration between editors, palette tools, Express configuration tools, validation services, execution tooling, debugging tools, probe tools, watch tools, and snippet tools.</li>
</ul>

<pre><code>IDE shell owns orchestration, not language meaning.

Good examples:
- command routing
- dockable panes
- project browser
- view coordination
- execution-tool pane coordination

Not owned by the shell:
- canonical source semantics
- primitive semantics
- profile semantics
- IR semantics
</code></pre>

<hr/>

<h2 id="diagram-editor">11. Diagram Editor</h2>

<p>
The diagram editor is the graphical programming environment where users construct executable FROG graphs.
It is an authoring surface for the FROG Program Model.
</p>

<p>
The diagram editor manipulates concepts such as:
</p>

<ul>
  <li>primitive nodes,</li>
  <li>structure nodes,</li>
  <li>sub-FROG invocation nodes,</li>
  <li>interface boundary nodes,</li>
  <li><code>widget_value</code> nodes,</li>
  <li><code>widget_reference</code> nodes,</li>
  <li>wires and typed ports,</li>
  <li>annotations and layout information,</li>
  <li>editor interaction state.</li>
</ul>

<p>
Key capabilities typically include:
</p>

<ul>
  <li>smooth zoom and pan,</li>
  <li>real-time node and wire manipulation,</li>
  <li>incremental validation feedback,</li>
  <li>large-graph navigation,</li>
  <li>execution observability overlays,</li>
  <li>debugging overlays and source-level pause localization,</li>
  <li>probe placement and probe visualization,</li>
  <li>watch creation and watch/source navigation workflows,</li>
  <li>snippet capture, paste, image export, and drag-and-drop insertion workflows,</li>
  <li>Express insertion, reconfiguration, and detach/materialize workflows where supported.</li>
</ul>

<hr/>

<h2 id="front-panel-editor">12. Front Panel Editor</h2>

<p>
The front-panel editor is used to design the graphical interaction layer associated with a FROG program.
It edits the widget tree declared in the <code>front_panel</code> section of the FROG Expression.
</p>

<p>
The front-panel editor is responsible for:
</p>

<ul>
  <li>widget declaration,</li>
  <li>widget composition and nesting,</li>
  <li>layout and positioning,</li>
  <li>styling and visual presentation,</li>
  <li>design-time widget configuration.</li>
</ul>

<p>
A front-panel selection pane MAY expose source-owned object management
properties such as stacking order, grouping, lock state, and editor visibility.
When such a pane hides a front-panel object, the hidden state MUST be stored on
the source object, for example as an object-level <code>hidden</code> flag in
the <code>front_panel</code>/<code>frontPanel</code> object list. This is an
authoring visibility affordance for the front-panel editor; it must not become
a hidden runtime mechanism and must not redefine diagram execution semantics.
</p>

<p>
The front panel is not:
</p>

<ul>
  <li>the public logical interface of the FROG,</li>
  <li>the executable graph of the FROG,</li>
  <li>the place where ordinary value-flow bindings are defined.</li>
</ul>

<pre><code>Three related but different things

Public interface  -&gt; reusable program boundary
Diagram           -&gt; authoritative executable graph
Front panel       -&gt; graphical interaction surface

Do not collapse them into one concept.
</code></pre>

<hr/>

<h2 id="frog-program-model">13. FROG Program Model</h2>

<p>
The FROG Program Model is the canonical editable in-memory representation maintained by the IDE.
It is the authoritative form of the program during authoring.
</p>

<p>
It unifies the information required for editing and IDE-side consistency management, including:
</p>

<ul>
  <li>metadata,</li>
  <li>public interface definitions,</li>
  <li>connector information,</li>
  <li>diagram content,</li>
  <li>structure regions and nested scopes,</li>
  <li>front-panel widget trees,</li>
  <li>icon and editor-related data,</li>
  <li>cross-section consistency information,</li>
  <li>authoring-side view state where such state is needed to preserve a stable editing experience without redefining canonical meaning.</li>
</ul>

<p>
When source provenance is enabled, the Program Model SHOULD keep enough stable source-object identity to create, verify, display, and update <code>ide.provenance</code> attestations without allowing those attestations to redefine canonical meaning.
</p>

<pre><code>Authoring ownership

IDE edits Program Model
        |
        v
Program Model preserves canonical source identity
        |
        v
Source serialization produces .frog Expression
</code></pre>

<pre><code>What the Program Model is not

It is not:
- the raw .frog file
- the execution IR
- the runtime instance
- the normative language semantics

It is:
- the IDE's canonical editable model
</code></pre>

<hr/>

<h2 id="relation-with-frog-expression">14. Relation with FROG Expression</h2>

<p>
The canonical saved source artifact of a FROG program is the FROG Expression stored in a <code>.frog</code> file.
That source format is defined in <code>Expression/</code>.
</p>

<p>
The IDE is expected to:
</p>

<ul>
  <li>load canonical source into the Program Model,</li>
  <li>edit the Program Model during authoring,</li>
  <li>serialize the Program Model back into canonical source,</li>
  <li>preserve source-level meaning and identity across those operations.</li>
</ul>

<pre><code>Round-trip invariant

load source
   -&gt; build Program Model
   -&gt; edit through IDE views
   -&gt; serialize canonical source
   -&gt; reload
   -&gt; recover same canonical construct identity
</code></pre>

<hr/>

<h2 id="execution-integration-boundary">15. Execution Integration Boundary</h2>

<p>
A FROG IDE MAY integrate validation, execution preparation, compiler services, runtime services, or equivalent execution-facing subsystems.
</p>

<p>
However, this document does not define those subsystems in full.
It defines only the IDE-facing architectural boundary around them.
</p>

<p>
At minimum, an execution-capable IDE typically needs:
</p>

<ul>
  <li>validation before live execution or build preparation,</li>
  <li>a transition from editable authoring state to execution-prepared state,</li>
  <li>a way to attach source identity to execution-observable objects,</li>
  <li>a way to consume live execution observations for debugging and inspection.</li>
</ul>

<pre><code>Execution boundary rule

Program Model / source meaning
            |
            v
validated canonical content
            |
            v
execution-facing systems

Not required:
- editor aliases
- palette labels
- Express UI state
- presentation-only hints
</code></pre>

<hr/>

<h2 id="palette">16. Palette</h2>

<p>
The palette is the primary IDE mechanism for discovering and inserting elements from the <strong>active insertable space</strong>.
That space may include intrinsic primitives, profile-defined primitives, structures, node insertions, annotations, guided entries, and implementation-supported third-party entries.
</p>

<p>
The palette belongs to the IDE layer because it is an authoring and discovery surface rather than a primitive catalog.
</p>

<pre><code>Palette responsibility

Palette answers:
- how users find things
- how users browse things
- how users insert things

Palette does not answer:
- what a primitive means
- what a profile means
- what execution semantics are
- what IR semantics are
</code></pre>

<hr/>

<h2 id="express-authoring">17. Express Authoring</h2>

<p>
Express authoring is the IDE-layer capability that allows a user to insert or edit common programming constructs through a guided configuration experience rather than through raw low-level assembly alone.
It exists to improve discoverability, speed of use, and usability for common tasks without creating a separate language.
</p>

<p>
An Express entry is therefore:
</p>

<ul>
  <li>assistant-driven,</li>
  <li>instance-local,</li>
  <li>task-oriented,</li>
  <li>normalizable to canonical FROG content.</li>
</ul>

<pre><code>Express lifecycle

choose entry
    -&gt;
configure
    -&gt;
resolve to canonical target
    -&gt;
serialize canonical content
    -&gt;
reopen / reconfigure if desired
    -&gt;
detach safely if desired
</code></pre>

<hr/>

<h2 id="execution-observability">18. Execution Observability</h2>

<p>
Execution observability is the architectural layer that exposes a source-aligned live view of a running FROG instance to the IDE.
It allows runtime activity to be projected back onto diagram and front-panel-related source objects without redefining the execution semantics of the language.
</p>

<p>
Conceptually, execution observability sits between live execution and IDE inspection/debugging features.
It is responsible for making runtime activity visible in terms such as:
</p>

<ul>
  <li>node activation,</li>
  <li>edge-level value availability,</li>
  <li>structure entry and region selection,</li>
  <li>loop iteration progression,</li>
  <li>local-memory state activity,</li>
  <li>pause-consistent execution snapshots.</li>
</ul>

<pre><code>Live execution
      |
      v
observable source-aligned events
      |
      +-&gt; debugging
      +-&gt; probes
      +-&gt; watches
      \-&gt; visual execution overlays
</code></pre>

<p>
The detailed source-level contract for this layer belongs in <code>IDE/Observability.md</code>.
</p>

<hr/>

<h2 id="debugging">19. Debugging</h2>

<p>
Debugging is the interactive control layer built on top of live execution observability.
It allows an IDE to pause, resume, inspect, and guide execution in a source-level way.
</p>

<p>
Canonical debugging capabilities include:
</p>

<ul>
  <li>execution highlighting,</li>
  <li>manual pause and resume,</li>
  <li>breakpoints,</li>
  <li>single-step controls,</li>
  <li>fault-directed source localization.</li>
</ul>

<pre><code>Not this:
line 41 -&gt; line 42 -&gt; line 43

But this:
observable graph activity
      -&gt;
pause / break / step over source-meaningful events
</code></pre>

<p>
The detailed source-level behavior of these controls belongs in <code>IDE/Debugging.md</code>.
</p>

<hr/>

<h2 id="probes">20. Probes</h2>

<p>
Probes are source-aligned live inspection tools built on top of execution observability and used together with debugging.
They allow an IDE to observe values or source-level execution state associated with graph-meaningful objects.
</p>

<p>
Probes belong to the inspection layer.
They do not alter program semantics, and they are not part of the canonical <code>.frog</code> source representation.
They remain compatible with debugging, but they are not owned by debugger control.
</p>

<pre><code>Probe mental model

edge / port / state object
          |
          v
temporary local live inspection
</code></pre>

<p>
The detailed source-level behavior of probes belongs in <code>IDE/Probes.md</code>.
</p>

<hr/>

<h2 id="watch">21. Watch</h2>

<p>
Watch is the persistent inspection layer of the IDE.
Where probes are attached directly to visible source locations, watch entries are persistent inspection objects gathered in a centralized view.
</p>

<p>
A watch-capable IDE typically supports:
</p>

<ul>
  <li>persistent observation across pauses and resumes,</li>
  <li>multiple simultaneous watched objects,</li>
  <li>navigation from watch entry to source object,</li>
  <li>inspection of values, state, and selected source-aligned execution objects,</li>
  <li>combination with debugging workflows.</li>
</ul>

<pre><code>Probe vs Watch

Probe -&gt; local, attached, immediate
Watch -&gt; centralized, persistent, managed list
</code></pre>

<p>
The detailed watch model belongs in <code>IDE/Watch.md</code>.
</p>

<hr/>

<h2 id="snippets">22. Snippets</h2>

<p>
Snippets are portable IDE artifacts used to capture, transport, preview, and reinsert reusable authoring fragments.
They support workflows such as:
</p>

<ul>
  <li>copy and paste of reusable diagram fragments,</li>
  <li>drag-and-drop insertion into a target editor,</li>
  <li>sharing small reusable authoring patterns,</li>
  <li>transporting a graph fragment with its visual layout and related metadata,</li>
  <li>front-panel or composite authoring reuse,</li>
  <li>visual sharing of reusable fragments outside the IDE.</li>
</ul>

<p>
In FROG, a standalone snippet artifact is intended to be <strong>image-backed</strong>.
It is visible as a normal image outside the IDE and decodable as structured payload inside a conforming IDE.
</p>

<pre><code>Snippet model

visible image
      +
embedded structured payload
      =
portable authoring fragment
</code></pre>

<p>
The detailed snippet transport and insertion model belongs in <code>IDE/Snippet.md</code>.
</p>

<hr/>

<h2 id="design-principles">23. Design Principles</h2>

<ul>
  <li>Clear separation of source, authoring model, and execution-facing systems</li>
  <li>Clear separation of Program Model and canonical Execution IR</li>
  <li>Language independence from IDE implementation</li>
  <li>Open serialized program representation</li>
  <li>Instant editing feedback</li>
  <li>Incremental validation</li>
  <li>Hardware-agnostic execution</li>
  <li>Cross-platform tooling</li>
  <li>Scalable graphical editing for large programs</li>
  <li>Explicit separation between interface, front panel, and diagram</li>
  <li>Source-aligned execution observability</li>
  <li>Dataflow-native debugging semantics</li>
  <li>Non-intrusive live inspection through probes</li>
  <li>Persistent centralized inspection through watch views</li>
  <li>Portable authoring-fragment transport through snippets</li>
  <li>Image-backed reusable snippet workflows for drag-and-drop authoring reuse</li>
  <li>Authoring convenience without semantic drift</li>
  <li>Preservation of canonical identity across aliases, views, guided flows, and contextual insertion workflows</li>
  <li>Express authoring as an IDE convenience layer rather than a separate language</li>
  <li>Deterministic normalization from guided authoring to canonical source content</li>
  <li>Optional IDE recoverability metadata without executable authority</li>
  <li>Optional authoring provenance metadata without executable authority</li>
  <li>Visible distinction between unknown, AI-generated, human-direct, human-reviewed, stale, invalid, and untrusted provenance states where provenance-aware tooling is enabled</li>
  <li>Clear distinction between intrinsic capability discovery, optional profile discovery, and third-party discovery</li>
</ul>

<hr/>

<h2 id="repository-direction">24. Repository Direction</h2>

<p>
At the current repository stage, <code>IDE/</code> is the home for IDE-facing specifications such as:
</p>

<ul>
  <li>overall IDE architecture,</li>
  <li>palette behavior,</li>
  <li>Express authoring behavior,</li>
  <li>execution observability,</li>
  <li>debugging,</li>
  <li>probes,</li>
  <li>watches,</li>
  <li>snippets.</li>
</ul>

<p>
The long-term repository architecture SHOULD preserve a stable separation between:
</p>

<ul>
  <li><code>Expression/</code> for canonical source representation,</li>
  <li><code>Language/</code> for normative execution semantics,</li>
  <li><code>Libraries/</code> for intrinsic standardized primitive catalogs,</li>
  <li><code>Profiles/</code> for optional standardized capability families,</li>
  <li><code>IR/</code> for canonical execution-facing representation and downstream handoff boundaries,</li>
  <li><code>IDE/</code> for authoring, discoverability, guided authoring behavior, observability, debugging, and inspection.</li>
</ul>

<pre><code>Long-term direction

Keep IDE/ focused on:
- authoring
- discoverability
- observability
- debugging
- inspection
- snippet transport
- guided UX layers

Do not drift IDE/ into:
- source ownership
- language semantics
- primitive semantics
- profile semantics
- IR ownership
- runtime specification
</code></pre>

<hr/>

<h2 id="summary">25. Summary</h2>

<p>
A FROG IDE is not just a diagram editor.
It is a layered authoring and inspection environment centered on:
</p>

<ul>
  <li>a canonical editable Program Model,</li>
  <li>a canonical source representation defined outside the IDE layer,</li>
  <li>palette-driven and Express-capable authoring workflows,</li>
  <li>a source-aligned execution observability layer,</li>
  <li>dataflow-native debugging and inspection tools,</li>
  <li>portable snippet-based authoring reuse workflows.</li>
</ul>

<p>
In that architecture:
</p>

<ul>
  <li>editing operates on the Program Model,</li>
  <li>storage operates through the canonical FROG Expression,</li>
  <li>execution-facing systems remain separate from IDE ownership,</li>
  <li>runtime activity becomes visible through execution observability,</li>
  <li>debugging, probes, and watches consume that observable view as parallel IDE-side tools,</li>
  <li>snippets provide portable, image-backed authoring-fragment reuse workflows,</li>
  <li>Express authoring provides guided insertion and reconfiguration without creating a separate language layer,</li>
  <li>the Program Model remains distinct from canonical Execution IR and from runtime-private realization.</li>
</ul>

<p>
This architecture allows FROG to remain an open graphical language while supporting serious IDE implementations without confusing source ownership, language ownership, intrinsic primitive ownership, optional profile ownership, execution-facing systems, and IDE-facing tooling.
</p>
