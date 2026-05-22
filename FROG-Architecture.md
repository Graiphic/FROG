<h1>FROG Architecture</h1>

<p>
Long-form architectural detail for the public FROG specification, including program representation, source-to-FIR posture, runtime/compiler complementarity, the original ASCII execution architecture, observability, targets, security, interoperability, and language/tooling separation.
</p>

<p>
This document preserves long-form material that was previously maintained in
the public root README. The root README now acts as a concise orientation page
and links here for detail.
</p>

<hr/>

<h2 id="core-concept-diagram-front-panel-and-public-interface">Core concept: Diagram, Front Panel, Public Interface</h2>

<p>
A FROG program combines multiple related but distinct source-level concepts.
The repository deliberately separates them so that execution meaning, public API, UI-facing authoring, and observability posture remain coherent over time.
</p>

<h3>Diagram — the authoritative executable graph</h3>

<p>
The diagram defines the executable logic of the program.
It is the authoritative source-level execution graph.
</p>

<p>
It contains:
</p>

<ul>
  <li>primitive nodes,</li>
  <li>structure nodes,</li>
  <li>sub-FROG invocations,</li>
  <li>interface boundary nodes,</li>
  <li>widget-related graph nodes,</li>
  <li>probe and watch attachment points when declared by the source or IDE model,</li>
  <li>directed graph edges,</li>
  <li>source-level annotations and documentation.</li>
</ul>

<h3>Public interface — the reusable program boundary</h3>

<p>
The public interface defines the typed reusable boundary of a FROG.
It is not owned by the front panel.
It is defined independently and participates in the diagram through <code>interface_input</code> and <code>interface_output</code>.
</p>

<h3>Front Panel — the interaction layer</h3>

<p>
The front panel defines the graphical interaction layer of the program.
It contains widget instances, layout information, composition, styling, and optional UI-library references.
</p>

<p>
A FROG MAY exist without a front panel.
When absent, the program remains a valid executable graphical artifact centered on its diagram and public interface.
</p>

<h3>Widget interaction model</h3>

<p>
FROG distinguishes two widget interaction paths:
</p>

<ul>
  <li><strong>natural value path</strong> — widget primary value participation through <code>widget_value</code>,</li>
  <li><strong>object-style path</strong> — explicit widget access through <code>widget_reference</code> together with <code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, and <code>frog.ui.method_invoke</code>.</li>
</ul>

<h3>Observability model</h3>

<p>
FROG also treats execution observability as a first-class architectural concern rather than as an afterthought.
The long-term IDE-facing posture should support at least:
</p>

<ul>
  <li><strong>probes</strong> — local inspection objects attached to wires, ports, nodes, structure boundaries, or other graph-meaningful surfaces in order to expose live or sampled values,</li>
  <li><strong>watches</strong> — persistent observation surfaces that follow selected program objects, values, references, or execution-local states across editing, execution, and debugging workflows,</li>
  <li><strong>projection back to source-meaningful objects</strong> rather than debugger-only opaque runtime internals.</li>
</ul>

<p>
Probes and watches do not redefine the program’s executable meaning.
They belong to the observability, debugging, and IDE-facing architecture.
They matter because graphical dataflow systems are especially strong when live execution can be inspected directly on the graph and on its public interaction surfaces.
</p>

<hr/>

<h2 id="runtime-and-compiler-complementarity">Runtime and compiler complementarity</h2>

<p>
FROG explicitly rejects the idea that runtime and compiler must be treated as competing ideologies.
They are complementary downstream strategies that can both begin from the same canonical source and the same open FIR.
</p>

<p>
A compiler path prepares deployable artifacts specialized for a target family.
A runtime path provides live execution services such as state management, observability, communication, hosting, front-panel interaction, monitoring, or controlled operational adaptation.
</p>

<p>
Different targets require different balances:
</p>

<ul>
  <li>some deployments are compiler-heavy,</li>
  <li>some are runtime-heavy,</li>
  <li>some are hybrid,</li>
  <li>and some need bridge paths toward existing operational hardware runtimes already present in the field.</li>
</ul>

<p>
FROG is designed so that the language does not have to choose one of those paths as its permanent identity.
The language stays upstream.
The deployment strategy stays modular.
</p>

<p>
That modularity also applies to the runtime boundary.
A full runtime host is valuable for IDE execution, debugging, probes, watches, live front panels, diagnostics, dynamic loading, and rich observability.
It is not, however, required to be the permanent shape of every deployed FROG artifact.
</p>

<p>
A compiler-oriented deployment may instead emit a specialized launcher or self-contained executable that carries only the accepted program scope, its selected capability modules, its declared external dependencies, its required host services, and the UI/assets that the deployment actually uses.
In that posture, the deployed artifact does not need to install or embed an entire general-purpose runtime when a smaller declared dependency closure is sufficient.
</p>

<p>
This also applies to the primitive surface. FROG distinguishes foundational
core operators, lightweight standard library families, and optional external
capability libraries. Standard libraries such as <code>frog.image.*</code> may be
provided by a base FROG distribution, but they remain explicit nodes in the
program, explicit references in FIR, and explicit items in the lowered
dependency closure.
</p>

<p>
Such dependency closure may be realized through implementation-selected
providers, including dynamic libraries, static libraries, compiled objects,
generated backend artifacts, or equivalent package mechanisms. The public
specification defines the primitive contract and dependency boundary; it does
not require one mandatory runtime-loader architecture.
</p>

<pre><code>IDE / debug posture
  .frog
    -&gt; FIR
    -&gt; lowering / backend contract
    -&gt; full runtime host
    -&gt; probes, watches, live UI, diagnostics

Deployment posture
  .frog
    -&gt; FIR
    -&gt; lowering / backend contract
    -&gt; compiled artifact + selected capability closure
    -&gt; generated launcher or self-contained executable
</code></pre>

<p>
External functions remain explicit dependencies rather than hidden runtime features.
For example, an image-decoding function may be supplied by a base standard
library module, declared native library, static library, or other
backend-consumable artifact.
The runtime or launcher resolves and orchestrates the declared call boundary; it does not become the semantic owner of image decoding.
</p>

<hr/>

<h2 id="open-fir-bridges-and-deployment-modularity">Open FIR bridges and deployment modularity</h2>

<p>
The openness of FIR is one of the most strategically important properties of FROG.
An open FIR is not only an inspectable intermediate representation.
It is the public architectural bridge surface that makes downstream combinability possible.
</p>

<p>
Because FIR remains open:
</p>

<ul>
  <li>bridges can be built from FIR toward compiler families,</li>
  <li>bridges can be built from FIR toward runtime families,</li>
  <li>bridges can be built toward existing operational hardware stacks,</li>
  <li>deployment preparation can compute a selected dependency and capability closure,</li>
  <li>and the same upstream program can support multiple deployment combinations without redefining the language itself.</li>
</ul>

<p>
This is especially important for heterogeneous hardware ecosystems.
Some vendors expose their value primarily through runtime stacks.
Others expose it through compiler flows.
Others use a hybrid posture.
FROG’s goal is to remain upstream from those differences rather than being captured by one of them.
</p>

<pre><code>canonical .frog source
        |
        v
validated meaning
        |
        v
open FIR
   +----+-----------------------------+-----------------------------+
   |                                  |                             |
   v                                  v                             v
runtime bridge family         compiler bridge family       hybrid bridge family
   |                                  |                             |
   v                                  v                             v
Python / Rust / C++            LLVM / native paths          target-specific mixes
reference runtimes             vendor compiler chains       runtime + compiled flows
   |                                  |                             |
   +--------------------------+-------+-----------------------------+
                              |
                              v
                heterogeneous deployment targets
</code></pre>

<p>
That modularity does not mean all downstream bridges already exist.
It means the architecture is deliberately shaped so they can exist without changing the language truth.
</p>

<p>
This distinction is strategically important:
FROG should support complete runtime-hosted execution where that is the right engineering answer, and it should also support deployment-specialized artifacts where only the program-specific runtime services, libraries, widgets, manifests, and assets are carried forward.
The open specification defines the source, FIR, contracts, profiles, and public boundaries; individual implementations decide how to realize full hosts, generated launchers, static links, dynamic links, or other deployment packages without redefining the language.
</p>

<hr/>

<h2 id="specification-architecture">Specification architecture</h2>

<p>
The repository is intentionally split into distinct architectural layers:
</p>

<ul>
  <li><strong>Expression</strong> — canonical source representation, source sections, source serialization rules, source-schema posture, and structural validity,</li>
  <li><strong>Language</strong> — normative execution semantics for validated program meaning,</li>
  <li><strong>IR</strong> — canonical open execution-facing representations derived from validated program meaning,</li>
  <li><strong>Libraries</strong> — intrinsic standardized primitive vocabularies and primitive-local behavior,</li>
  <li><strong>Profiles</strong> — optional standardized capability families and profile-owned capability contracts,</li>
  <li><strong>IDE</strong> — authoring architecture, editor-facing models, execution observability, debugging semantics, inspection workflows, snippets, and Express authoring.</li>
</ul>

<p>
This separation is deliberate.
It prevents the language from being reduced to one editor, one runtime, one compiler, or one vendor implementation.
</p>

<pre>
Expression/   -> canonical source form and structural validity
Language/     -> validated program meaning
IR/           -> canonical open execution-facing representation
Libraries/    -> intrinsic standardized primitive vocabularies
Profiles/     -> optional standardized capability families
IDE/          -> authoring, observability, debugging, inspection
</pre>

<pre>
what is saved      -> Expression/
what is true       -> Language/
what is derived    -> IR/
what exists        -> Libraries/ and Profiles/
what is edited     -> IDE/
</pre>

<p>
Beyond those six core families, the published repository also contains support and governance areas that should not be confused with semantic owners:
</p>

<pre>
what is exemplified   -> Examples/
what is expected      -> Conformance/
what is prototyped    -> Implementations/Reference/
what version means    -> Versioning/
</pre>

<hr/>

<h2 id="program-representation">Program representation</h2>

<p>
FROG programs should be understood across <strong>five</strong> distinct representation levels.
</p>

<h3>1. FROG Expression</h3>

<p>
The <strong>FROG Expression</strong> is the serialized source representation stored in a <code>.frog</code> file.
It is the canonical source form of a FROG program.
</p>

<h3>2. Structural validity</h3>

<p>
A loadable JSON source file is not automatically a structurally valid canonical FROG source file.
Structural validity is an explicit stage owned by <code>Expression/</code>.
</p>

<h3>3. FROG Program Model</h3>

<p>
The <strong>FROG Program Model</strong> is the canonical editable in-memory representation used by IDEs during authoring.
</p>

<h3>4. Validated program meaning</h3>

<p>
A source-derived FROG program must first be validated against the relevant language, primitive-library, and profile rules.
That validated state is where normative execution meaning becomes a trustworthy basis for later derivation.
</p>

<h3>5. Canonical open execution-facing representation</h3>

<p>
A validated FROG is not executed directly from raw source text.
A conforming toolchain validates the source-derived program representation and then derives a canonical open execution-facing representation suitable for execution preparation, analysis, normalization, optimization, lowering, or compilation.
</p>

<pre>
.frog source
    |
    v
loadability
    |
    v
structural validity
    |
    v
Program Model / validated source-derived program
    |
    v
validated program meaning
    |
    v
canonical execution-facing representation (FIR posture)
    |
    v
lowering / backend-facing handoff
</pre>

<hr/>

<h2 id="execution-architecture">Execution architecture</h2>

<p>
A conforming FROG ecosystem should separate <strong>authoring</strong>, <strong>canonical source</strong>, <strong>structural validity</strong>, <strong>validated program meaning</strong>, <strong>canonical open execution-facing representation</strong>, and <strong>target-specific execution realization</strong>.
</p>

<p>
The architectural posture below deliberately combines three requirements:
</p>

<ul>
  <li>a clear source-to-execution derivation corridor,</li>
  <li>a clear downstream split between runtime-family and compiler-family consumers,</li>
  <li>and a clear observability/debugging branch that preserves probes and watches as first-class IDE-facing concepts without turning them into program semantics.</li>
</ul>

<pre>
                                   FROG IDE
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
                                  +----------------------------------+----------------------------------+
                                  |                                                                     |
                                  v                                                                     v
                         Runtime-family consumers                                             Compiler-family consumers
                                  |                                                                     |
                                  v                                                                     v
                        Target execution instance                                          Target artifact / executable
                                  |                                                                     |
                          +-------+------------------------------------------+--------------------------+
                          |                                                  |
                          v                                                  v
        Source-aligned execution observability                  Runtime activity on the active target
        (mapped back to meaningful FROG objects)
                          |
                          v
       Debugging / inspection / pause / resume / break / step
                          |
                +---------+----------+
                |                    |
                v                    v
             Probes                Watches
</pre>

<p>
A serious downstream compiler path MAY eventually target compiler families such as LLVM.
However, those downstream families remain consumers of lowered FROG forms rather than the definition of FROG itself.
</p>

<p>
Likewise, probes and watches belong to the execution-observability and IDE-facing posture layered on top of execution projection.
They do not redefine the validated executable meaning of the program.
</p>

<hr/>

<h2 id="execution-observability-debugging-and-inspection">Execution observability, debugging, and inspection</h2>

<p>
Interactive inspection and debugging are not performed directly on raw serialized source.
They are performed on a live execution derived from validated program content and projected back onto source-meaningful objects.
</p>

<p>
In FROG, debugging and inspection are dataflow-first rather than line-oriented.
They operate on observable graph activity, structures, sub-FROG scopes, value flow, local memory, UI-related execution objects, probes, watch surfaces, and public-interface participation rather than on a fictional sequential instruction list.
</p>

<p>
A useful long-term observability posture includes:
</p>

<ul>
  <li><strong>probes</strong> for localized value inspection on wires, ports, nodes, structures, and other graph-facing surfaces,</li>
  <li><strong>watches</strong> for persistent observation of selected values, references, state cells, widget objects, or public-interface objects,</li>
  <li><strong>source projection</strong> so that runtime observations remain attributable to source-meaningful objects rather than to opaque backend-private internals,</li>
  <li><strong>host-independent semantics</strong> so that observability remains an architectural capability of the ecosystem rather than a private trick of one runtime.</li>
</ul>

<p>
This matters especially for graphical dataflow programming because live inspection is not an optional luxury.
It is part of the practical readability and engineering power of the model.
</p>

<p>
In this architecture:
</p>

<ul>
  <li><strong>probes</strong> are best understood as local, execution-projected inspection points placed near graph-meaningful surfaces,</li>
  <li><strong>watches</strong> are best understood as persistent observation surfaces that remain useful across longer debugging and analysis workflows,</li>
  <li><strong>break / pause / resume / step</strong> belong to debugging control posture,</li>
  <li><strong>runtime telemetry alone</strong> is not sufficient unless it can be mapped back to meaningful FROG objects.</li>
</ul>

<hr/>

<h2 id="execution-targets">Execution targets</h2>

<p>
FROG programs are designed to remain source-level stable across multiple hardware classes.
The language is not tied to one processor family, one operating system, one runtime architecture, or one vendor.
</p>

<p>
Representative target classes include:
</p>

<ul>
  <li><strong>General-purpose CPUs</strong> — workstation, server, and industrial PC execution,</li>
  <li><strong>Real-time targets</strong> — deterministic measurement and control systems,</li>
  <li><strong>Embedded systems</strong> — ARM and edge-oriented devices,</li>
  <li><strong>GPUs</strong> — accelerated compute targets,</li>
  <li><strong>FPGAs</strong> — programmable-logic targets,</li>
  <li><strong>Microcontrollers</strong> — constrained embedded execution,</li>
  <li><strong>Industrial edge controllers</strong> — integrated vendor-specific control and acquisition platforms.</li>
</ul>

<hr/>

<h2 id="open-industrial-hardware-standard">Open industrial hardware standard</h2>

<p>
FROG aims to be more than a language that merely supports multiple targets.
Its long-term goal is to provide an <strong>open industrial graphical programming standard</strong> that hardware and software ecosystems can build on without requiring a proprietary language boundary.
</p>

<p>
That ambition includes a future where hardware vendors can expose runtime bridges, compiler bridges, backend contracts, and operational integration layers against the same open upstream language basis rather than forcing users into isolated graphical silos.
</p>

<hr/>

<h2 id="security-and-optimization-by-design">Security and optimization by design</h2>

<p>
FROG integrates validation, inspectability, governance, and optimization into its architecture.
</p>

<p>
Optimization occurs primarily in execution preparation, FIR normalization, lowering, compilation, and backend stages.
Those downstream stages may vary across implementations while remaining downstream from the same open language corridor.
</p>

<p>
Security claims must remain disciplined.
FROG does not claim that graphical form automatically guarantees safety or security.
Its narrower and stronger claim is that open source, explicit graph structure, open FIR, readable governance, and explicit downstream handoff reduce structural opacity and improve inspectability.
</p>

<p>
This means FROG is security-relevant through architecture:
it favors auditability, traceability, reviewable transformation, explicit validation, bounded acceptance surfaces, and controlled runtime/compiler boundaries.
Those properties do not replace security engineering, but they make security engineering less opaque.
</p>

<p>
The same point applies to responsibility.
In an AI-assisted development context, the critical question is not only whether code can be produced.
The critical question is whether the produced logic can be inspected, explained, accepted, and evolved by accountable humans and organizations.
FROG is designed to make that responsibility chain more explicit.
</p>

<hr/>

<h2 id="interoperability">Interoperability</h2>

<p>
FROG is designed for interoperability at several levels:
</p>

<ul>
  <li><strong>source interoperability</strong>,</li>
  <li><strong>editing interoperability</strong>,</li>
  <li><strong>structural interoperability</strong>,</li>
  <li><strong>semantic interoperability</strong>,</li>
  <li><strong>IR interoperability</strong>,</li>
  <li><strong>execution interoperability</strong>,</li>
  <li><strong>governance interoperability</strong>,</li>
  <li><strong>ecosystem interoperability</strong>.</li>
</ul>

<p>
Representative integration targets may include:
</p>

<ul>
  <li>C / C++,</li>
  <li>Rust,</li>
  <li>Python,</li>
  <li>.NET,</li>
  <li>other ABI-compatible environments.</li>
</ul>

<p>
In the long-term architectural model, interoperability also means that one open FIR may be bridged toward multiple runtime or compiler families without forcing one operational stack to become the language definition.
</p>

<hr/>

<h2 id="separation-of-language-and-tooling">Language separation</h2>

<p>
FROG explicitly separates:
</p>

<ul>
  <li>the language specification,</li>
  <li>the canonical source representation,</li>
  <li>source-schema posture and structural validity,</li>
  <li>the editable program model,</li>
  <li>validated program meaning,</li>
  <li>the canonical open execution-facing representation,</li>
  <li>intrinsic standardized primitive vocabularies,</li>
  <li>optional standardized capability profiles,</li>
  <li>compiler implementations,</li>
  <li>backend implementations,</li>
  <li>runtime implementations,</li>
  <li>development environments,</li>
  <li>hardware adaptation layers,</li>
  <li>deployment and orchestration layers.</li>
</ul>

<p>
At the modeling level, FROG also separates:
</p>

<ul>
  <li>language from IDE,</li>
  <li>source from structural validation,</li>
  <li>structural validity from semantic truth,</li>
  <li>semantic truth from derived execution-facing representation,</li>
  <li>intrinsic libraries from optional profiles,</li>
  <li>runtime families from one another,</li>
  <li>runtime families from LLVM-oriented native compilation,</li>
  <li>public interface from front panel,</li>
  <li>natural widget value flow from object-style widget interaction,</li>
  <li>program execution meaning from probes and watches,</li>
  <li>specification corpus governance from <code>.frog spec_version</code>,</li>
  <li><code>.frog spec_version</code> from <code>metadata.program_version</code>.</li>
</ul>

<hr/>
