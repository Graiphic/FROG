<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Expression Specification</h1>

<p align="center">
  <strong>Canonical source specification for <code>.frog</code> programs and related source-owned widget artifacts</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-expression-exists">2. Why Expression Exists</a></li>
  <li><a href="#scope-of-this-directory">3. Scope of this Directory</a></li>
  <li><a href="#architectural-position">4. Architectural Position</a></li>
  <li><a href="#specification-documents">5. Specification Documents</a></li>
  <li><a href="#what-a-frog-file-describes">6. What a <code>.frog</code> File Describes</a></li>
  <li><a href="#representation-levels">7. Representation Levels</a></li>
  <li><a href="#file-tree-and-artifact-families">8. File Tree and Artifact Families</a></li>
  <li><a href="#file-types">9. File Types</a></li>
  <li><a href="#top-level-json-structure">10. Top-Level JSON Structure</a></li>
  <li><a href="#section-presence-rules">11. Section Presence Rules</a></li>
  <li><a href="#sections-overview">12. Sections Overview</a></li>
  <li><a href="#cross-cutting-subsystems">13. Cross-Cutting Subsystems</a></li>
  <li><a href="#frog-and-wfrog-boundary">14. <code>.frog</code> and <code>.wfrog</code> Boundary</a></li>
  <li><a href="#interface-connector-diagram-and-front-panel">15. Interface, Connector, Diagram, and Front Panel</a></li>
  <li><a href="#source-well-formedness-and-structural-validity">16. Source Well-Formedness and Structural Validity</a></li>
  <li><a href="#execution-relevance-validation-and-derivation-boundary">17. Execution Relevance, Validation, and Derivation Boundary</a></li>
  <li><a href="#canonical-formatting-and-ordering">18. Canonical Formatting and Ordering</a></li>
  <li><a href="#normative-terminology">19. Normative Terminology</a></li>
  <li><a href="#status">20. Status</a></li>
  <li><a href="#license">21. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory defines the canonical source specification of FROG — Free Open Graphical Language.
</p>

<p>
The fundamental executable unit of the language is a FROG. A FROG is a complete graphical dataflow program represented by a structured JSON source file with the <code>.frog</code> extension.
</p>

<p>
That canonical source file is called the FROG Expression. It is the authoritative, editable, transparent, and version-control-friendly description of a FROG program at the source level.
</p>

<p>
The purpose of this directory is to define:
</p>

<ul>
  <li>the canonical top-level structure of a <code>.frog</code> file,</li>
  <li>the required and optional source sections of that file,</li>
  <li>the source-level representation of those sections,</li>
  <li>the source-visible cross-cutting subsystems that appear in canonical source, including the type system, the widget instance model, the widget class contract model, the widget interaction model, the widget-package boundary, and the source-facing representation of structures and local-memory constructs,</li>
  <li>the structural validity boundary that a canonical <code>.frog</code> source file MUST satisfy before later semantic validation and execution-facing derivation,</li>
  <li>the source-schema posture under which parts of canonical source shape may be made machine-checkable without collapsing source structure into semantic validation or implementation-private behavior.</li>
</ul>

<p>
This directory is about the authoritative source form of a FROG. It is not the complete runtime architecture, not the complete IDE implementation model, not the complete semantic interpretation of validated programs, and not the open execution-facing IR layer.
</p>

<p>
This directory also defines the source-owned boundary between the canonical <code>.frog</code> program file and related widget-oriented source artifacts such as <code>.wfrog</code> packages. The canonical program source remains the <code>.frog</code> file. Widget-oriented packages do not replace that role.
</p>

<p>
Cross-cutting execution semantics are related to this source specification but are not owned here when they belong to validated program meaning. Likewise, optional capability families may appear in executable diagrams, but their normative ownership remains outside <code>Expression/</code> when they belong to profile specifications rather than to the source format itself.
</p>

<p>
Source-version interpretation is centralized in <code>Versioning/Readme.md</code>. In particular, the <code>spec_version</code> field follows the repository-wide cumulative version model: later source-format versions should normally extend earlier valid forms rather than silently replace them, unless an explicit breaking boundary is declared.
</p>

<pre><code>Repository architecture around Expression

Expression/   -&gt; canonical source form
Language/     -&gt; validated program meaning
IR/           -&gt; open execution-facing representation
Libraries/    -&gt; intrinsic primitive vocabularies
Profiles/     -&gt; optional standardized capability families
IDE/          -&gt; authoring, observability, debugging, inspection

Expression/ owns source structure and source-visible object shape.
Expression/ owns source well-formedness and structural validity.
Expression/ owns source-schema posture for canonical source.
Expression/ owns the source-side boundary between .frog and related .wfrog packages.
Expression/ does not own validated program meaning.
Expression/ does not own open execution-facing derivation.
</code></pre>

<hr/>

<h2 id="why-expression-exists">2. Why Expression Exists</h2>

<p>
A graphical language still needs a durable, explicit, tool-independent source form. That is the role of the FROG Expression.
</p>

<p>
The Expression layer exists so that a FROG program can be:
</p>

<ul>
  <li>saved as canonical source,</li>
  <li>read and diffed by humans,</li>
  <li>versioned in source control,</li>
  <li>loaded by different tools,</li>
  <li>checked for source well-formedness and structural validity independently of one IDE implementation,</li>
  <li>preserved without hidden binary-only authoring state,</li>
  <li>validated before later semantic interpretation and downstream derivation.</li>
</ul>

<p>
Accordingly, this directory defines how a FROG is serialized, how canonical source becomes structurally valid, and what belongs to the source-owned validation corridor before later semantic interpretation. It does not by itself define every detail of how a FROG is edited, semantically validated by a given tool, derived into IR, lowered, compiled, executed, or deployed.
</p>

<pre><code>Expression/ answers:

- What is in a .frog file?
- Which sections exist?
- Which sections are required?
- How are source objects serialized?
- Which source-level cross-cutting models apply?
- What makes source structurally valid as canonical source?
- What belongs to source-schema posture and machine-checkable source shape?
- How does canonical .frog source relate to source-side .wfrog widget packages?

Expression/ does not answer by itself:

- validated program meaning
- open execution-facing IR structure
- full runtime architecture
- full compiler architecture
- full IDE authoring architecture
</code></pre>

<p>
This layer exists precisely to keep canonical source explicit without forcing later semantic, runtime, or realization decisions to leak backward into the source format.
</p>

<hr/>

<h2 id="scope-of-this-directory">3. Scope of this Directory</h2>

<p>
This directory defines the canonical source format of FROG programs.
</p>

<p>
It includes:
</p>

<ul>
  <li>top-level source sections,</li>
  <li>cross-cutting source subsystems used by multiple sections,</li>
  <li>source-facing structure and state representation,</li>
  <li>the source-owned boundary for widget-oriented package references,</li>
  <li>the source-facing boundary between widget instance shape, widget class contract shape, widget behavior doctrine, and widget realization doctrine,</li>
  <li>source-schema posture for machine-checkable canonical source shape.</li>
</ul>

<p>
It does not define:
</p>

<ul>
  <li>the complete runtime architecture,</li>
  <li>the complete in-memory editable Program Model used by IDE implementations,</li>
  <li>the complete open execution-facing IR used by toolchains,</li>
  <li>the complete cross-cutting execution semantics of the language,</li>
  <li>the complete intrinsic primitive vocabulary of FROG,</li>
  <li>the complete optional profile surface of FROG,</li>
  <li>the full repository-wide policy for specification-version transition.</li>
</ul>

<p>
This directory also does not collapse widget class law, widget behavior law, widget realization, or host rendering into the canonical <code>.frog</code> file. Those concerns may be referenced from source, but they remain distinct architectural layers.
</p>

<p>
Likewise, this directory defines the format and source-owned boundary of <code>.wfrog</code> packages, but the standard primitive widget library published for ecosystem reuse belongs in <code>Libraries/</code>, not in <code>Expression/</code>.
</p>

<hr/>

<h2 id="architectural-position">4. Architectural Position</h2>

<p>
The Expression layer is the source-owned entry point of the representation pipeline.
</p>

<pre><code>canonical .frog source
  -&gt; structural validity
  -&gt; validated program meaning
  -&gt; canonical open execution-facing IR
  -&gt; lowering
  -&gt; backend contract
  -&gt; downstream backend-family consumption
</code></pre>

<p>
This directory governs the first two items only insofar as they concern source shape and structural validity.
</p>

<p>
The following distinctions MUST remain explicit:
</p>

<ul>
  <li>loadable source is not identical to structurally valid canonical source,</li>
  <li>structurally valid canonical source is not identical to semantically accepted program meaning,</li>
  <li>canonical source is not identical to execution-facing IR,</li>
  <li>execution-facing IR is not identical to lowered form,</li>
  <li>lowered form is not identical to backend contract,</li>
  <li>backend contract is not identical to one private runtime realization.</li>
</ul>

<p>
The following source-package distinction MUST also remain explicit:
</p>

<ul>
  <li>the canonical program source artifact is the <code>.frog</code> file,</li>
  <li>a <code>.wfrog</code> package is a widget-oriented source artifact family,</li>
  <li>a <code>.wfrog</code> package may be referenced by canonical source,</li>
  <li>a <code>.wfrog</code> package does not replace the canonical program-owned role of <code>.frog</code>.</li>
</ul>

<p>
The following versioning distinction MUST also remain explicit:
</p>

<ul>
  <li>the source file's <code>spec_version</code> identifies the source-format compatibility target of that artifact,</li>
  <li>a <code>.wfrog</code> package may carry its own <code>wfrog_version</code> for widget-package format compatibility,</li>
  <li>the full repository-wide version-transition policy remains centralized in <code>Versioning/Readme.md</code>.</li>
</ul>

<p>
This positioning is essential for widget closure: runtimes and hosts may interpret canonical source, but they must not silently become the definition of canonical source.
</p>

<hr/>

<h2 id="specification-documents">5. Specification Documents</h2>

<p>
The FROG Expression is defined through the following documents in this directory:
</p>

<pre><code>Expression/
├── Readme.md
│   -&gt; architectural entry point for the canonical source specification
├── Schema.md
│   -&gt; source-schema posture and machine-checkable structural validation boundary
├── schema/
│   └── frog.schema.json
│       -&gt; conservative machine-checkable top-level canonical source schema
├── Metadata.md
│   -&gt; descriptive program metadata and non-executable identification fields
├── Type.md
│   -&gt; canonical type-expression model used across the source format
├── Interface.md
│   -&gt; public typed inputs and outputs of a FROG
├── Connector.md
│   -&gt; graphical perimeter mapping of interface ports when the FROG is reused as a node
├── Diagram.md
│   -&gt; authoritative executable graph as canonical source representation
├── Front panel.md
│   -&gt; optional front-panel composition and user-facing interaction surface
├── Widget.md
│   -&gt; widget instance model, widget identity, roles, class reference, value participation, and source-side package references
├── Widget class contract.md
│   -&gt; class-level widget contract for values, properties, methods, events, parts, access legality, and object-surface exposure
├── Widget interaction.md
│   -&gt; diagram-side widget interaction paths and execution-facing widget access model
├── Widget package (.wfrog).md
│   -&gt; normative package format for .wfrog widget classes, composite publication, realization resources, and bounded behavior publication
├── Widget realization.md
│   -&gt; host-facing realization boundary, visual resources, part-to-visual mapping, and SVG integration
├── Widget behavior.md
│   -&gt; normative behavior boundary for class-owned reaction, declarative rules, bounded expressions, and host-private support
├── Control structures.md
│   -&gt; source-facing representation of canonical control structures
├── State and cycles.md
│   -&gt; source-facing representation of explicit local memory and feedback-cycle formation rules
├── Icon.md
│   -&gt; reusable-node icon representation
├── IDE preferences.md
│   -&gt; optional IDE-facing source metadata and recoverability-oriented authoring preferences
├── Source provenance.md
│   -&gt; optional source-carried authoring provenance, signed attestations, issuer trust, and human review markers
└── Cache.md
    -&gt; optional non-authoritative cache content for tooling convenience
</code></pre>

<p>
The canonical numeric vocabulary is specified separately in
<a href="./Numeric%20representations.md"><code>Numeric representations.md</code></a>.
It defines compact source tokens, canonical names, defaults, parameterized
forms, migration rules, array propagation, and non-semantic visual type-family
guidance shared by widgets, terminals, bindings, and interface ports.
</p>

<p>
Public interface authoring that uses a visual slot pattern is specified in
<a href="Interface%20map.md"><code>Interface map.md</code></a>. That document
defines source-owned pattern selection, transforms, widget-value bindings,
deterministic migration, and Array-binding transfer without moving executable
dataflow authority out of the Diagram.
</p>

<p>
The documents in this directory play four different roles:
</p>

<ul>
  <li>canonical source scope and ownership matrices,</li>
  <li>top-level source-section specifications,</li>
  <li>cross-cutting source subsystems,</li>
  <li>widget-oriented source package boundary documents,</li>
  <li>source-schema posture and machine-checkable structural support artifacts.</li>
</ul>

<h3>5.1 Top-level source-section specifications</h3>

<p>
Some documents define dedicated top-level source sections such as:
</p>

<ul>
  <li><code>metadata</code>,</li>
  <li><code>interface</code>,</li>
  <li><code>connector</code>,</li>
  <li><code>diagram</code>,</li>
  <li><code>front_panel</code>,</li>
  <li><code>icon</code>,</li>
  <li><code>ide</code>,</li>
  <li><code>cache</code>.</li>
</ul>

<p>
<code>Canonical source scope.md</code> provides the compact cross-section view
of this ownership model. It should be read before drilling into individual
section documents when the question is whether a source fact is required,
optional, descriptive metadata, diagram-owned, front-panel-owned, binding
source, or excluded from executable source meaning.
</p>

<h3>5.2 Cross-cutting source subsystems</h3>

<p>
Other documents define cross-cutting source subsystems used throughout the source format. In particular:
</p>

<ul>
  <li><code>Type.md</code> defines canonical value-type expressions and source-level type rules used throughout the source format.</li>
  <li><code>Widget.md</code> defines the widget instance model used inside <code>front_panel</code>.</li>
  <li><code>Widget class contract.md</code> defines the class-level widget contract behind standardized widget-object surfaces, including part ownership, member legality, mutability, and access constraints.</li>
  <li><code>Widget interaction.md</code> defines how executable diagrams may interact with widgets through standardized diagram-level interaction mechanisms.</li>
  <li><code>Control structures.md</code> and <code>State and cycles.md</code> define source-facing representation rules that participate in later semantic interpretation but must appear explicitly in canonical source.</li>
</ul>
<p>
Source provenance is also a cross-cutting source-carried subsystem. <code>Source provenance.md</code> defines the optional <code>ide.provenance</code> object used to record authoring, generation, import, modification, and human-review attestations for source-visible objects without changing executable meaning.
</p>


<h3>5.3 Widget-oriented source package boundary documents</h3>

<p>
The widget-oriented package documents define how canonical source relates to source-side widget packages.
</p>

<ul>
  <li><code>Widget package (.wfrog).md</code> defines the <code>.wfrog</code> artifact family, its package structure, its package kinds, and the publishable format for widget classes, composite widgets, realization resources, and bounded behavior surfaces.</li>
  <li><code>Widget realization.md</code> defines the host-facing realization boundary, including part-to-visual mapping and SVG integration.</li>
  <li><code>Widget behavior.md</code> defines the behavior doctrine used by widget classes and related package content, including intrinsic behavior, declarative rules, bounded expressions, and host-private support boundaries.</li>
</ul>

<p>
These documents do not turn widget packages into top-level canonical program files. They define how source may reference those packages while preserving ownership boundaries.
</p>

<h3>5.4 Source-schema posture</h3>

<p>
<code>Schema.md</code> and the files under <code>schema/</code> define the conservative machine-checkable posture of canonical source shape.
</p>

<p>
They do not replace semantic validation. They provide machine-checkable support for source-level structure where that structure can be checked without collapsing later interpretation layers.
</p>

<hr/>

<h2 id="what-a-frog-file-describes">6. What a <code>.frog</code> File Describes</h2>

<p>
A canonical <code>.frog</code> file describes one complete FROG program as source.
</p>

<p>
At minimum, it describes:
</p>

<ul>
  <li>its identity and descriptive metadata,</li>
  <li>its public typed boundary,</li>
  <li>its authoritative executable diagram.</li>
</ul>

<p>
It may additionally describe:
</p>

<ul>
  <li>its reusable-node connector,</li>
  <li>its front-panel composition,</li>
  <li>its icon,</li>
  <li>its IDE-facing source metadata,</li>
  <li>its non-authoritative cache data.</li>
</ul>

<p>
When a front panel is present, the <code>.frog</code> file may also reference widget-oriented packages needed to interpret widget class law, widget bounded behavior, or host-facing realization. Even in those cases, the <code>.frog</code> file remains canonical program source, not a container for all widget package contents.
</p>

<p>
The <code>.frog</code> file is canonical source, not compiled output, not IR, and not one implementation's private runtime representation.
</p>

<hr/>

<h2 id="representation-levels">7. Representation Levels</h2>

<p>
The repository architecture distinguishes several layers:
</p>

<pre><code>canonical .frog source
  -&gt; structural validity
  -&gt; validated language meaning
  -&gt; open execution-facing IR
  -&gt; lowering
  -&gt; backend contract
  -&gt; downstream backend-family consumption
</code></pre>

<p>
This directory governs the first two items only insofar as they concern source shape and structural validity.
</p>

<p>
The following distinctions MUST remain explicit:
</p>

<ul>
  <li>loadable source is not identical to structurally valid canonical source,</li>
  <li>structurally valid canonical source is not identical to semantically accepted program meaning,</li>
  <li>canonical source is not identical to execution-facing IR,</li>
  <li>execution-facing IR is not identical to lowered form,</li>
  <li>lowered form is not identical to backend contract,</li>
  <li>backend contract is not identical to one private runtime realization.</li>
</ul>

<p>
The following source-artifact distinction MUST also remain explicit:
</p>

<ul>
  <li>canonical program source is not identical to widget-package source,</li>
  <li>widget-package source is not identical to host-private rendering logic,</li>
  <li>visual assets are not identical to widget class law,</li>
  <li>runtime-private convenience is not identical to language-owned object law.</li>
</ul>

<p>
These distinctions are especially important in the widget corridor because source shape, class law, behavior law, realization resources, runtime interpretation, and host realization must remain inspectably separate.
</p>

<hr/>

<h2 id="file-tree-and-artifact-families">8. File Tree and Artifact Families</h2>

<p>
A canonical <code>.frog</code> source file is a single JSON document whose internal sections refer to source-level artifact families.
</p>

<p>
At the top level, canonical source remains intentionally compact.
</p>

<ul>
  <li>type information is embedded locally through canonical type expressions,</li>
  <li>widget instances are embedded inside <code>front_panel</code> when a front panel is present,</li>
  <li>widget interactions are expressed inside <code>diagram</code>,</li>
  <li>structure and state constructs are embedded where they naturally belong in the source graph,</li>
  <li>widget-oriented package references may appear where relevant without becoming separate top-level program sections.</li>
</ul>

<pre><code>Top-level simplicity rule

No dedicated top-level:
- types
- widgets
- widget_interactions
- control_structures
- state_model
- widget_packages

These concerns are embedded where they naturally belong.
</code></pre>

<p>
The broader source artifact family therefore includes:
</p>

<ul>
  <li>the canonical program file <code>.frog</code>,</li>
  <li>optional referenced <code>.wfrog</code> packages for widget classes, composite widget definitions, realization resources, behavior publication, or bundle content,</li>
  <li>optional visual assets such as SVG where referenced by widget realization content.</li>
</ul>

<p>
The future standardized primitive widget families that make up the reusable front-panel baseline are expected to be published through <code>Libraries/</code> using these artifact families, rather than being redefined inside this architectural directory.
</p>

<hr/>

<h2 id="file-types">9. File Types</h2>

<p>
The canonical editable source artifact of a FROG program is a file with the <code>.frog</code> extension.
</p>

<p>
That file MUST contain canonical JSON source conforming to the structural rules defined by this directory.
</p>

<p>
The widget-oriented package family uses the <code>.wfrog</code> extension. A <code>.wfrog</code> package is a structured JSON widget-oriented artifact that may be referenced by canonical source.
</p>

<p>
A <code>.wfrog</code> package is not a second canonical program file. It is a related source artifact family with its own typed package kinds and version line.
</p>

<p>
Repository support artifacts such as schema files, examples, conformance material, or implementation workspaces are not themselves canonical <code>.frog</code> source unless explicitly stated otherwise.
</p>

<hr/>

<h2 id="top-level-json-structure">10. Top-Level JSON Structure</h2>

<p>
At the top level, a canonical <code>.frog</code> source file is a JSON object.
</p>

<p>
The top-level object contains required and optional sections whose detailed structure is owned by the companion documents in this directory.
</p>

<p>
Conceptually:
</p>

<pre><code>{
  "spec_version": "...",
  "metadata": { ... },
  "interface": { ... },
  "diagram": { ... },

  "front_panel": { ... },
  "connector": { ... },
  "icon": { ... },
  "ide": { ... },
  "cache": { ... }
}
</code></pre>

<p>
This top-level simplicity is deliberate. Many cross-cutting subsystems participate in canonical source without becoming separate top-level sections.
</p>

<p>
Widget-oriented package references, when present, appear inside the relevant source-owned structures such as widget instances in the front panel. They do not become standalone top-level program sections merely because they are important.
</p>

<hr/>

<h2 id="section-presence-rules">11. Section Presence Rules</h2>

<p>
A canonical <code>.frog</code> source file MUST contain:
</p>

<ul>
  <li><code>spec_version</code>,</li>
  <li><code>metadata</code>,</li>
  <li><code>interface</code>,</li>
  <li><code>diagram</code>.</li>
</ul>

<p>
A canonical <code>.frog</code> source file MAY additionally contain:
</p>

<ul>
  <li><code>front_panel</code>,</li>
  <li><code>connector</code>,</li>
  <li><code>icon</code>,</li>
  <li><code>ide</code>,</li>
  <li><code>cache</code>.</li>
</ul>

<p>
The <code>front_panel</code> section is optional. It SHOULD be present when the FROG intentionally includes a user-facing interaction layer. When absent, the FROG is interpreted as a program with no serialized front-panel composition.
</p>

<p>
The <code>connector</code> section is optional. It SHOULD be present when the FROG is intended to be reused as a graphical node inside another diagram.
</p>

<p>
Optional sections MUST NOT modify the authoritative execution semantics established by validated program meaning.
</p>

<pre><code>Presence summary

MUST:
- spec_version
- metadata
- interface
- diagram

MAY:
- connector
- front_panel
- icon
- ide
- cache
</code></pre>

<p>
The presence of <code>spec_version</code> is the source-level anchor for cumulative source evolution. Later source-format versions should normally extend earlier valid forms rather than silently invalidate them, as governed centrally in <code>Versioning/Readme.md</code>.
</p>

<p>
Widget-oriented package references are governed by the relevant source structures that use them. Their presence does not create new mandatory top-level sections in canonical <code>.frog</code> source.
</p>

<hr/>

<h2 id="sections-overview">12. Sections Overview</h2>

<h3>12.1 Metadata</h3>

<p>
Required section defining identity, documentation, authorship, and related descriptive information.
</p>

<p>
Detailed specification: <code>Metadata.md</code>
</p>

<h3>12.2 Interface</h3>

<p>
Required section defining the public typed inputs and outputs of the FROG. All declared types MUST use canonical FROG type expressions.
</p>

<p>
Detailed specifications: <code>Interface.md</code>, <code>Type.md</code>
</p>

<h3>12.3 Connector</h3>

<p>
Optional section defining the graphical mapping of interface ports when the FROG is reused as a node. The connector never defines new logical ports.
</p>

<p>
Detailed specification: <code>Connector.md</code>
</p>

<h3>12.4 Diagram</h3>

<p>
Required section defining the executable dataflow graph of the FROG. The diagram contains nodes, edges, dependencies, regions, boundaries, and annotations as source-level constructs. It is the authoritative source-level execution structure of the program.
</p>

<p>
Detailed specifications: <code>Diagram.md</code>, <code>Type.md</code>, <code>Widget interaction.md</code>, the source-facing companion documents <code>Control structures.md</code> and <code>State and cycles.md</code>, the normative semantic documents in <code>Language/</code>, the relevant intrinsic primitive-library specifications in <code>Libraries/</code>, and any relevant optional standardized capability specifications in <code>Profiles/</code>.
</p>

<h3>12.5 Front Panel</h3>

<p>
Optional section defining the user interaction layer of the FROG through widget instances, layout, and related UI composition data.
</p>

<p>
When absent, the canonical source defines no serialized front-panel composition for that FROG.
</p>

<p>
The front panel defines widget containment and panel-side composition. It does not by itself define the full widget object contract, the full executable widget interaction model, or the authoritative executable graph.
</p>

<p>
Detailed specifications: <code>Front panel.md</code>, <code>Widget.md</code>, <code>Widget class contract.md</code>, <code>Widget package (.wfrog).md</code>, <code>Widget realization.md</code>, <code>Widget behavior.md</code>
</p>

<h3>12.6 Icon</h3>

<p>
Optional section containing the icon used by tools when the FROG is represented as a reusable node.
</p>

<p>
Detailed specification: <code>Icon.md</code>
</p>

<h3>12.7 IDE Preferences</h3>

<p>
Optional section containing IDE-facing preferences and recoverability metadata serialized with the FROG itself. This section belongs to the FROG Expression because a FROG behaves as a durable editable program unit and may embed source-level IDE preferences together with non-authoritative authoring recoverability aids.
</p>

<p>
Runtimes and other execution-facing systems MUST ignore this section for execution semantics.
</p>

<p>
Detailed specification: <code>IDE preferences.md</code>
</p>

<h3>12.8 Cache</h3>

<p>
Optional section containing derived, non-authoritative tooling data used to accelerate workflows.
</p>

<p>
Detailed specification: <code>Cache.md</code>
</p>

<pre><code>Section role summary

metadata    -&gt; descriptive identity
interface   -&gt; public typed boundary
connector   -&gt; graphical node-side projection of interface
diagram     -&gt; authoritative executable graph
front_panel -&gt; optional user interaction surface
icon        -&gt; reusable-node icon
ide         -&gt; non-authoritative IDE-facing metadata
cache       -&gt; non-authoritative tooling cache
</code></pre>

<hr/>

<h2 id="cross-cutting-subsystems">13. Cross-Cutting Subsystems</h2>

<h3>13.1 Type System</h3>

<p>
FROG defines a normative built-in type system described in <code>Type.md</code>.
</p>

<p>
This subsystem applies wherever values are declared, connected, constrained, validated, or coerced, including:
</p>

<ul>
  <li>interface ports,</li>
  <li>diagram ports and edge validation,</li>
  <li>structure boundaries,</li>
  <li>typed configuration values such as <code>default</code> or <code>initial</code>,</li>
  <li>widget primary values,</li>
  <li>typed widget members.</li>
</ul>

<p>
The type system is cross-cutting. It is not represented as a dedicated required top-level section.
</p>

<h3>13.2 Widget Instance Model</h3>

<p>
FROG defines a normative widget instance model described in <code>Widget.md</code>.
</p>

<p>
A widget is not merely a drawing. At source level, it is a structured UI object instance with identity, class reference, role, optional typed value, persisted instance-side properties where allowed, and optional source-side references to widget-oriented packages.
</p>

<p>
The widget instance model is cross-cutting. It governs widget instances contained inside <code>front_panel</code> when that section is present.
</p>

<h3>13.3 Widget Class Contract Model</h3>

<p>
FROG defines a normative class-level widget contract model described in <code>Widget class contract.md</code>.
</p>

<p>
This model governs the stable object-surface contract behind widget classes, including:
</p>

<ul>
  <li>class identity,</li>
  <li>member categories,</li>
  <li>part ownership,</li>
  <li>property readability and writability,</li>
  <li>method invocability,</li>
  <li>event exposure,</li>
  <li>mutability boundaries,</li>
  <li>addressing legality.</li>
</ul>

<p>
This class-level contract remains distinct from:
</p>

<ul>
  <li>front-panel composition,</li>
  <li>diagram-side executable widget interaction,</li>
  <li>one private IDE implementation strategy,</li>
  <li>one private runtime realization.</li>
</ul>

<h3>13.4 Widget Interaction Model</h3>

<p>
FROG defines a normative widget interaction model described in <code>Widget interaction.md</code>.
</p>

<p>
This model governs diagram-level interaction with widgets through standardized mechanisms such as:
</p>

<ul>
  <li>natural value participation through <code>widget_value</code>,</li>
  <li>object-style widget access through <code>widget_reference</code>,</li>
  <li>property read,</li>
  <li>property write,</li>
  <li>method invocation,</li>
  <li>event observation where supported by the interaction corridor.</li>
</ul>

<p>
These interactions are represented inside the executable diagram rather than as a dedicated top-level source section.
</p>

<p>
The current source-level widget interaction model already uses standardized primitive identifiers in the <code>frog.ui.*</code> namespace. Future intrinsic UI primitive specifications and future profile-based capability expansions MUST remain consistent with that source-level interaction model and with the class-level member contracts declared for widgets.
</p>

<h3>13.5 Widget Package Boundary</h3>

<p>
FROG defines a source-owned widget package boundary described in <code>Widget package (.wfrog).md</code>, <code>Widget realization.md</code>, and <code>Widget behavior.md</code>.
</p>

<p>
This boundary governs how canonical source may refer to widget-oriented packages without collapsing architectural layers.
</p>

<p>
In particular:
</p>

<ul>
  <li><code>.frog</code> remains the canonical program source file,</li>
  <li><code>.wfrog</code> defines widget-oriented package artifacts,</li>
  <li>widget class law is not the same thing as widget realization,</li>
  <li>widget realization is not the same thing as host-private rendering logic,</li>
  <li>widget behavior law is not the same thing as unrestricted arbitrary host code,</li>
  <li>SVG is a visual asset layer, not hidden executable truth.</li>
</ul>

<p>
The standardized primitive widget families intended for ecosystem-wide reuse are expected to be published in <code>Libraries/</code> on top of this boundary, not inside this boundary document set itself.
</p>

<h3>13.6 Control-Structure and Local-Memory Source Dependencies</h3>

<p>
FROG includes source-facing representation rules for structures, explicit local memory, and feedback-cycle formation inside this directory:
</p>

<ul>
  <li><code>Control structures.md</code> defines the source-facing representation of structure families and their serialized form,</li>
  <li><code>State and cycles.md</code> defines the source-facing representation of explicit local-memory elements and feedback-cycle formation constraints.</li>
</ul>

<p>
The structural serialization of those constructs belongs here because they must appear explicitly in canonical source. Their validated execution meaning belongs to <code>Language/</code>.
</p>

<pre><code>Cross-cutting ownership

Serialized shape of structures/cycles   -&gt; Expression/
Execution meaning of structures/cycles  -&gt; Language/
Intrinsic primitive meaning             -&gt; Libraries/
Optional capability meaning             -&gt; Profiles/
Widget instance-side source shape       -&gt; Expression/
Widget class-side contract shape        -&gt; Expression/
Diagram-side widget interaction shape   -&gt; Expression/
Source-side widget package boundary     -&gt; Expression/
Validated meaning of widget interaction -&gt; Language/ and related primitive/profile ownership
</code></pre>

<h3>13.7 Source Provenance Boundary</h3>

<p>
FROG defines an optional source provenance boundary in <code>Source provenance.md</code>. Its standardized home is <code>ide.provenance</code>, where tools may carry attestations for nodes, edges, structures, widgets, interface objects, annotations, changesets, or whole-document review states.
</p>

<p>
This boundary is deliberately non-executable. Missing provenance means unknown or unattested, not automatically AI-generated. Valid provenance may support inspection, review, and accountability, but it MUST NOT define canonical source meaning, validated program meaning, FIR derivation, lowering, backend handoff, or runtime execution semantics.
</p>

<pre><code>Source provenance boundary

ide.provenance
  -&gt; authoring / generation / review evidence
  -/-> executable meaning

diagram / interface / front_panel
  -&gt; source objects that may be attested
  -&gt; remain authoritative for their own source roles
</code></pre>

<hr/>

<h2 id="frog-and-wfrog-boundary">14. <code>.frog</code> and <code>.wfrog</code> Boundary</h2>

<p>
The canonical program source artifact of FROG is the <code>.frog</code> file.
</p>

<p>
The widget-oriented package artifact family uses the <code>.wfrog</code> extension.
</p>

<p>
This boundary is intentional and normative.
</p>

<ul>
  <li><code>.frog</code> owns canonical program source.</li>
  <li><code>.frog</code> owns program sections such as <code>metadata</code>, <code>interface</code>, <code>diagram</code>, and optional <code>front_panel</code>.</li>
  <li><code>.frog</code> owns widget instances used by the program when a front panel is present.</li>
  <li><code>.wfrog</code> may own widget class package content, composite widget package content, widget realization package content, widget behavior publication content, or bundle content.</li>
  <li><code>.wfrog</code> does not replace the canonical program-owned role of <code>.frog</code>.</li>
</ul>

<p>
The following distinctions MUST remain explicit:
</p>

<pre><code>canonical program source      != widget-oriented package
widget instance declaration   != widget class definition
widget class law              != widget realization
widget behavior law           != unrestricted host code
widget semantics              != SVG visual asset
runtime interpretation        != language ownership
one runtime implementation    != FROG widget definition
</code></pre>

<p>
A canonical <code>.frog</code> file MAY reference <code>.wfrog</code> packages through source-owned structures such as widget instances. Those package references must remain explicit and reviewable.
</p>

<hr/>

<h2 id="interface-connector-diagram-and-front-panel">15. Interface, Connector, Diagram, and Front Panel</h2>

<h3>15.1 Interface</h3>

<p>
The interface defines the public typed boundary of a FROG.
</p>

<p>
It answers:
</p>

<ul>
  <li>which public inputs exist,</li>
  <li>which public outputs exist,</li>
  <li>what their types are,</li>
  <li>what their source-level metadata is.</li>
</ul>

<h3>15.2 Connector</h3>

<p>
The connector defines how those public interface ports are projected graphically when the FROG is reused as a node inside another diagram.
</p>

<p>
The connector does not create new interface meaning. It is a graphical perimeter mapping of already-declared interface ports.
</p>

<h3>15.3 Diagram</h3>

<p>
The diagram is the authoritative executable graph of the FROG.
</p>

<p>
It defines:
</p>

<ul>
  <li>nodes,</li>
  <li>edges,</li>
  <li>dependencies,</li>
  <li>regions,</li>
  <li>explicit state participation where applicable,</li>
  <li>diagram-side widget interaction when present.</li>
</ul>

<p>
The diagram remains the executable authority. Front-panel composition and connector appearance do not replace it.
</p>

<h3>15.4 Front Panel</h3>

<p>
When present, the front panel defines widgets, layout, and visual interaction for the FROG itself. It does not define the public API and it does not replace the executable graph.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>a front-panel widget does not create a public interface port by itself,</li>
  <li>a control widget is not automatically a public input,</li>
  <li>an indicator widget is not automatically a public output.</li>
</ul>

<p>
Any relationship between public interface behavior and front-panel behavior MUST be represented through the diagram.
</p>

<pre><code>Do not collapse these into one concept

interface   != front_panel
connector   != interface
diagram     != front_panel
diagram     is the executable authority
</code></pre>

<p>
The following distinction MUST also remain explicit:
</p>

<pre><code>front_panel             -&gt; widget containment and panel composition
widget instance model   -&gt; instance-side object shape
widget class contract   -&gt; class-side member legality and exposure
widget interaction      -&gt; diagram-side executable object access
widget packages         -&gt; related source-side package artifacts
</code></pre>

<hr/>

<h2 id="source-well-formedness-and-structural-validity">16. Source Well-Formedness and Structural Validity</h2>

<p>
A canonical <code>.frog</code> file MUST first be loadable as source.
</p>

<p>
That is not yet sufficient for structural validity.
</p>

<p>
Structural validity concerns whether the source:
</p>

<ul>
  <li>contains the required top-level sections,</li>
  <li>uses the correct source-owned section shapes,</li>
  <li>uses canonical field structures where required,</li>
  <li>respects source-level identity and containment rules,</li>
  <li>respects source-level addressing constraints for cross-cutting subsystems such as widget interaction,</li>
  <li>uses source-owned package references in structurally valid locations when such references are present.</li>
</ul>

<p>
Structural validity does not by itself establish the full validated meaning of the program.
</p>

<p>
Structural validity also does not by itself define cross-version policy. The repository-wide cumulative version model and related transition doctrine remain centralized in <code>Versioning/Readme.md</code>.
</p>

<p>
Likewise, structural validity of source-owned package references does not by itself establish the full semantic validity of widget law, realization law, widget behavior law, or runtime support claims.
</p>

<hr/>

<h2 id="execution-relevance-validation-and-derivation-boundary">17. Execution Relevance, Validation, and Derivation Boundary</h2>

<p>
Expression/ governs canonical source before semantic validation and before open execution-facing derivation.
</p>

<p>
This means:
</p>

<ul>
  <li>source shape is owned here,</li>
  <li>source-level cross-cutting subsystems needed for serialization are owned here,</li>
  <li>validated meaning is owned later,</li>
  <li>IR derivation is owned later,</li>
  <li>lowering and backend handoff are owned later.</li>
</ul>

<p>
Some source constructs are clearly execution-relevant, including:
</p>

<ul>
  <li>diagram connectivity,</li>
  <li>structure boundaries,</li>
  <li>explicit local memory,</li>
  <li>widget interaction expressed in the diagram,</li>
  <li>source-visible widget-package references that later guide widget-class, widget behavior, and realization interpretation.</li>
</ul>

<p>
Even so, Expression/ owns them only as source representation and structural validity. Their validated meaning belongs elsewhere.
</p>

<hr/>

<h2 id="canonical-formatting-and-ordering">18. Canonical Formatting and Ordering</h2>

<p>
Canonical source SHOULD remain stable, explicit, and diff-friendly.
</p>

<p>
Tools SHOULD preserve:
</p>

<ul>
  <li>stable section ordering,</li>
  <li>stable object key ordering where the relevant specification defines one,</li>
  <li>stable identity fields,</li>
  <li>stable explicitness over hidden implicit reconstruction where practical.</li>
</ul>

<p>
Non-authoritative IDE or cache sections MUST NOT be allowed to obscure the authoritative source meaning of required executable sections.
</p>

<p>
Likewise, source-owned references to widget packages SHOULD remain explicit and readable rather than being hidden inside one implementation's private resolution metadata.
</p>

<hr/>

<h2 id="normative-terminology">19. Normative Terminology</h2>

<p>
The key words <strong>MUST</strong>, <strong>MUST NOT</strong>, <strong>SHOULD</strong>, <strong>SHOULD NOT</strong>, and <strong>MAY</strong> in this document are to be interpreted as normative requirement language.
</p>

<p>
When this document describes ownership boundaries, those boundaries are normative architectural boundaries unless explicitly stated otherwise.
</p>

<hr/>

<h2 id="status">20. Status</h2>

<p>
This directory defines the canonical source layer of the published FROG specification.
</p>

<p>
Some parts of the source model are intentionally conservative so that:
</p>

<ul>
  <li>canonical source remains stable,</li>
  <li>structural validity remains machine-checkable where appropriate,</li>
  <li>later semantic and execution-facing layers can evolve without collapsing back into private tool behavior,</li>
  <li>later source-format versions can remain cumulative extensions of earlier valid forms rather than silent replacements.</li>
</ul>

<p>
The active closure direction for the widget architecture in this layer is to keep the canonical program source compact while making the source-owned boundary to widget-oriented <code>.wfrog</code> packages explicit, inspectable, extensible, and multi-runtime compatible.
</p>

<p>
The immediate downstream closure direction is to use that boundary to support a minimal standardized widget primitive baseline in <code>Libraries/</code>, from which richer front-panel ecosystems and developer-defined composite widgets can be built.
</p>

<hr/>

<h2 id="license">21. License</h2>

<p>
See the repository-level license information for the licensing terms governing this specification.
</p>
