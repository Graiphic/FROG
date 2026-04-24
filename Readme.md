<div align="center" class="go-pages-link" data-render-target="github">
  <a href="https://graiphic.github.io/FROG/">
    <img src="./assets/open-github-pages-banner.svg" alt="Open the GitHub Pages version" width="640" />
  </a>
</div>

<p align="center">
  <img src="FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">🐸 FROG — Free Open Graphical Language</h1>

<p align="center">
  <strong>Free Open Graphical Dataflow Programming Language</strong><br/>
  FROG is an open, hardware-agnostic graphical dataflow programming language designed to describe computation as explicit executable graphs while remaining accessible, explicit, inspectable, portable, auditable, modular, and scalable across heterogeneous execution targets.
</p>

<p align="center">
  FROG aims to establish itself as the open-source standard foundation for graphical dataflow language infrastructure.
</p>

<p align="center">
  <strong>FROG attacks the structural lock-in of graphical industrial programming by opening the language layer itself:</strong><br/>
  canonical source, validated meaning, execution-facing FIR, lowering, backend contracts, runtime bridges, and compiler bridges.
</p>

<p align="center">
  In the generative AI era, software production becomes abundant, but accountable understanding becomes scarce.<br/>
  FROG is designed to make generated or human-authored logic structured, graphical, inspectable, controllable, and governable.
</p>

<p align="center">
  Specification work initiated: <strong>8 March 2026</strong>
</p>

<p align="center">
  <a href="#what-is-frog">What is FROG?</a> •
  <a href="#what-makes-frog-different">What makes FROG different?</a> •
  <a href="#from-go-hw-to-frog">From GO HW to FROG</a> •
  <a href="#what-this-repository-defines">What this repository defines</a> •
  <a href="#what-frog-is-not">What FROG is not</a> •
  <a href="#published-repository-state">Published repository state</a> •
  <a href="#campaign-priority">Campaign priority</a> •
  <a href="#positioning">Positioning</a> •
  <a href="#breaking-the-syntax-first-bottleneck">Breaking the syntax-first bottleneck</a> •
  <a href="#why-frog-exists-now">Why FROG exists now</a> •
  <a href="#frog-in-the-ai-era">FROG in the AI era</a> •
  <a href="#post-ai-accountability-inspection-and-control">Post-AI accountability</a> •
  <a href="#dataflow-programming">Dataflow programming</a> •
  <a href="#from-prototyping-to-critical-systems">From prototyping to critical systems</a> •
  <a href="#core-concept-diagram-front-panel-and-public-interface">Core concept</a> •
  <a href="#runtime-and-compiler-complementarity">Runtime and compiler complementarity</a> •
  <a href="#open-fir-bridges-and-deployment-modularity">Open FIR bridges and deployment modularity</a> •
  <a href="#repository-structure">Repository structure</a> •
  <a href="#repository-runtime-and-native-execution-direction">Runtime and native execution direction</a> •
  <a href="#internal-documentation-map">Internal documentation map</a> •
  <a href="#recommended-reading-path">Recommended reading path</a> •
  <a href="#specification-architecture">Specification architecture</a> •
  <a href="#program-representation">Program representation</a> •
  <a href="#execution-architecture">Execution architecture</a> •
  <a href="#execution-observability-debugging-and-inspection">Execution observability, debugging, and inspection</a> •
  <a href="#execution-targets">Execution targets</a> •
  <a href="#open-industrial-hardware-standard">Open industrial hardware standard</a> •
  <a href="#security-and-optimization-by-design">Security &amp; optimization</a> •
  <a href="#interoperability">Interoperability</a> •
  <a href="#separation-of-language-and-tooling">Language separation</a> •
  <a href="#governance-official-branding-and-ecosystem">Governance, official branding, and ecosystem</a> •
  <a href="#project-status">Project status</a> •
  <a href="#license">License</a>
</p>

<hr/>

<h2 id="what-is-frog">What is FROG?</h2>

<p>
FROG is an open, hardware-agnostic <strong>graphical dataflow programming language</strong>.
It represents computation as explicit executable graphs rather than as syntax-first sequences of textual instructions.
</p>

<p>
Instead of describing a program primarily through ordered text, FROG describes a program through:
</p>

<ul>
  <li>typed nodes,</li>
  <li>typed ports,</li>
  <li>directed graph connections,</li>
  <li>structured control regions,</li>
  <li>explicit public interface boundaries,</li>
  <li>optional front-panel widgets and interaction layers,</li>
  <li>explicit observability surfaces such as probes, watches, and inspection-aware source objects.</li>
</ul>

<p>
Execution emerges from data availability, dependency structure, explicit control constructs, intrinsic standardized primitive behavior, optional profile-owned capability behavior, and explicit local-memory semantics rather than from manually authored instruction order.
</p>

<p>
FROG is designed to remain independent from any specific IDE, compiler, runtime, operating system, or hardware vendor.
That separation provides a durable basis for multiple independent implementations, long-term industrial interoperability, and auditable portability across toolchains.
</p>

<p>
FROG is intended to scale from accessible graphical authoring to demanding execution contexts such as industrial automation, embedded systems, heterogeneous compute targets, and future conforming execution ecosystems.
</p>

<hr/>

<h2 id="what-makes-frog-different">What makes FROG different?</h2>

<p>
FROG is not differentiated by one isolated feature.
Graphical programming, multi-target deployment, model-based execution, runtime systems, compilers, and open specifications already exist in different forms across the software landscape.
</p>

<p>
FROG’s differentiation is the <strong>combination</strong> of those concerns into one open graphical language architecture where source, meaning, execution-facing representation, lowering, runtime consumption, compiler consumption, and hardware adaptation remain explicitly separated.
</p>

<p>
The strategic claim is therefore precise:
FROG aims to open the language layer of graphical industrial programming itself.
It does this by making the canonical source open, the validated meaning explicit, the FIR inspectable, and the downstream runtime/compiler bridge boundaries modular.
</p>

<table>
  <thead>
    <tr>
      <th>Property</th>
      <th>What it means in FROG</th>
      <th>Why it matters</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>Inspectable</strong></td>
      <td>Source, graph structure, FIR, lowering, backend contracts, acceptance artifacts, and runtime behavior are intended to remain traceable.</td>
      <td>Generated or transformed systems can be reviewed as structured artifacts rather than trusted as opaque output.</td>
    </tr>
    <tr>
      <td><strong>Hardware-agnostic</strong></td>
      <td>The language is not owned by one CPU, OS, runtime, compiler, board family, or hardware vendor.</td>
      <td>The same upstream program model can be bridged toward heterogeneous execution targets.</td>
    </tr>
    <tr>
      <td><strong>FIR-open</strong></td>
      <td>FIR is treated as a public execution-facing bridge surface, not as a backend-private internal object.</td>
      <td>Runtime families, compiler families, and hardware bridges can attach downstream without redefining the language.</td>
    </tr>
    <tr>
      <td><strong>Hyper-modular</strong></td>
      <td>IDE, source, semantics, FIR, lowering, backend contracts, runtime families, compiler families, and profiles remain distinct.</td>
      <td>The ecosystem can grow without collapsing into one monolithic product stack.</td>
    </tr>
    <tr>
      <td><strong>AI-compatible</strong></td>
      <td>The canonical source is structured, the program graph is reviewable, and execution-facing artifacts are inspectable.</td>
      <td>AI-assisted generation and transformation can be paired with human review and validation instead of opaque automation.</td>
    </tr>
    <tr>
      <td><strong>Accountability-oriented</strong></td>
      <td>Generated or human-authored logic can be carried through explicit source, validation, FIR, lowering, backend contracts, acceptance, and runtime/compiler consumption.</td>
      <td>Industrial users can reason about responsibility, review, control, and evolution instead of receiving a large opaque block of generated code.</td>
    </tr>
    <tr>
      <td><strong>Security-oriented</strong></td>
      <td>FROG does not claim that graphical form automatically guarantees safety; it reduces opacity through explicit structure and inspectable artifacts.</td>
      <td>Security and assurance conversations can be grounded in auditability, traceability, and controlled downstream handoff.</td>
    </tr>
  </tbody>
</table>

<p>
A useful analogy is the historical role of C as a portable systems-language layer above many hardware targets.
FROG does not claim to be C, and it does not yet claim universal target coverage.
The analogy is architectural:
FROG aims to provide an open upstream graphical language layer whose FIR can be bridged toward many downstream runtime, compiler, and hardware families.
</p>

<pre><code>open graphical source
        |
        v
validated language meaning
        |
        v
open execution-facing FIR
        |
        +--------------------------+--------------------------+
        |                          |                          |
        v                          v                          v
runtime-family bridges      compiler-family bridges     hardware/vendor bridges
        |                          |                          |
        v                          v                          v
live execution services     native / optimized paths    operational target stacks
</code></pre>

<p>
This is the core disruption hypothesis of FROG:
industrial graphical programming should no longer require the language, the editor, the runtime, the compiler, and the hardware ecosystem to be owned by one inseparable stack.
</p>

<hr/>

<h2 id="from-go-hw-to-frog">From GO HW to FROG</h2>

<p>
FROG is not an isolated idea.
It is a deeper architectural step in Graiphic’s trajectory.
</p>

<p>
GO HW made visible how powerful graphical orchestration can become when AI, logic, and hardware are treated as one executable system.
That work demonstrated the value of a graphical cockpit able to design, deploy, and monitor complex graph-based execution paths.
It also revealed a structural limit:
as long as the language boundary, the saved format, and the execution-facing representation remain too tightly coupled to one stack, long-term openness, modularity, inspectability, and sovereignty remain constrained.
</p>

<p>
FROG is the answer to that limit.
It takes the ambition one layer deeper.
Instead of stopping at graphical orchestration as a tooling achievement, FROG opens the language foundation itself:
</p>

<ul>
  <li>open canonical source,</li>
  <li>open validated semantic layering,</li>
  <li>open execution-facing FIR,</li>
  <li>explicit lowering and backend handoff boundaries,</li>
  <li>runtime-family modularity,</li>
  <li>compiler-family modularity,</li>
  <li>and an ecosystem where language truth is not owned by one product stack.</li>
</ul>

<p>
This is a more difficult path.
It is also the more ambitious one.
It aims at full architectural mastery from source to execution, while preserving openness and long-term industrial sovereignty.
</p>

<hr/>

<h2 id="what-this-repository-defines">What this repository defines</h2>

<p>
This repository defines the <strong>published FROG specification</strong>.
It is the repository where the language and its surrounding specification layers are written, clarified, stabilized, and progressively closed.
</p>

<p>
Its role is to provide a durable open foundation for future:
</p>

<ul>
  <li>IDEs,</li>
  <li>validators,</li>
  <li>runtimes,</li>
  <li>compilers,</li>
  <li>execution backends,</li>
  <li>profile-supporting toolchains,</li>
  <li>ecosystem services and integrations.</li>
</ul>

<p>
The repository also contains repository-level support material that helps make the specification inspectable in practice:
named examples,
conformance material,
a non-normative reference implementation workspace,
a strategic framing layer,
a non-normative roadmap layer,
and a centralized specification-versioning surface.
Those areas support the published specification, but they do not replace its ownership boundaries.
</p>

<p>
This repository does <strong>not</strong> define one mandatory product implementation.
It does not equate the language with one IDE, one runtime, one compiler, one vendor stack, or one deployment model.
</p>

<hr/>

<h2 id="what-frog-is-not">What FROG is not</h2>

<ul>
  <li><strong>FROG is not an IDE.</strong></li>
  <li><strong>FROG is not a single runtime.</strong></li>
  <li><strong>FROG is not a single compiler.</strong></li>
  <li><strong>FROG is not a vendor product.</strong></li>
  <li><strong>FROG is not a security guarantee just because a program is graphical.</strong></li>
  <li><strong>FROG is an open language specification with distinct source, semantic, FIR, library, profile, IDE-facing, conformance, and version-governance layers.</strong></li>
</ul>

<p>
That distinction matters.
Multiple independent implementations should be possible around the standard.
At the same time, the official <strong>FROG</strong> product identity and official branding can remain steward-controlled.
Graiphic may build the flagship official IDE carrying the FROG name, while the language itself remains open and implementable by others.
</p>

<hr/>

<h2 id="published-repository-state">Published repository state</h2>

<p>
At the current published state, the repository already contains the six core architectural specification families:
<code>Expression/</code>,
<code>Language/</code>,
<code>IR/</code>,
<code>Libraries/</code>,
<code>Profiles/</code>,
and <code>IDE/</code>.
These remain the primary ownership layers of the published language specification.
</p>

<p>
The repository also contains repository-level support areas and repository-level framing / governance layers:
</p>

<ul>
  <li><strong><code>Examples/</code></strong> — illustrative named source slices, applicative vertical-slice anchors, and bounded compiler-corridor mirrors,</li>
  <li><strong><code>Conformance/</code></strong> — public accept / reject / preserve expectations for the published repository state,</li>
  <li><strong><code>Implementations/Reference/</code></strong> — a non-normative reference implementation workspace used to exercise disciplined execution paths,</li>
  <li><strong><code>Versioning/</code></strong> — centralized specification-governance and current-status reporting for the published specification corpus,</li>
  <li><strong><code>Strategy/</code></strong> — a non-normative strategic framing layer distinct from normative ownership,</li>
  <li><strong><code>Roadmap/</code></strong> — a non-normative closure-sequencing layer distinct from both strategy and specification.</li>
</ul>

<p>
The published example surface already contains both:
</p>

<ul>
  <li>a numbered example-slice progression under <code>Examples/01_*</code> through <code>Examples/05_*</code>,</li>
  <li>and a narrower conservative compiler-corridor mirror under <code>Examples/compiler/</code>.</li>
</ul>

<p>
The first repository-visible applicative vertical-slice anchor is:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/</code></pre>

<p>
That slice is currently the primary named source-to-runtime-to-native anchor because it visibly combines:
</p>

<ul>
  <li>front-panel participation,</li>
  <li>widget-value participation,</li>
  <li>minimal widget-reference participation,</li>
  <li>bounded structured control,</li>
  <li>explicit local state,</li>
  <li>public output publication,</li>
  <li>a published FIR artifact,</li>
  <li>a published backend contract artifact,</li>
  <li>published downstream reference runtime consumers,</li>
  <li>shared runtime-family acceptance material,</li>
  <li>and a first LLVM-oriented native proof corridor.</li>
</ul>

<p>
The current published runtime and native surface already includes:
</p>

<ul>
  <li>a repository-visible runtime directory under <code>Implementations/Reference/Runtime/</code>,</li>
  <li>a shared runtime-family acceptance surface under <code>Implementations/Reference/Runtime/acceptance/</code>,</li>
  <li>a Python runtime-family consumer under <code>Implementations/Reference/Runtime/python/</code>,</li>
  <li>a Rust runtime-family consumer under <code>Implementations/Reference/Runtime/rust/</code>,</li>
  <li>a C/C++ runtime-family consumer under <code>Implementations/Reference/Runtime/cpp/</code>,</li>
  <li>and a first LLVM-oriented downstream dossier under <code>Implementations/Reference/LLVM/</code>.</li>
</ul>

<p>
At the same time, the repository does <strong>not</strong> claim full generalized symmetry across all future examples, all runtime families, or full rendered-native front-panel closure.
The correct current statement is narrower:
the repository now materially exposes a <strong>repository-visible bounded symmetry</strong> for the canonical Example 05 corridor across FIR, lowering, backend contract emission, shared acceptance material, Python, Rust, C/C++, and a first LLVM-oriented native proof path.
</p>

<p>
That distinction matters.
The published direction already supports language/runtime/compiler separation, and the repository-visible executable closure is now materially stronger than before, while still remaining example-scoped and intentionally non-generalized.
</p>

<hr/>

<h2 id="campaign-priority">Campaign priority</h2>

<p>
The current campaign priority is explicit:
<strong>close serious published examples all the way to real execution and make multi-runtime modularity repository-visible.</strong>
</p>

<p>
A serious example is no longer considered finished merely because it is source-readable or architecturally plausible.
A serious example should progressively converge toward:
</p>

<ul>
  <li>one canonical <code>.frog</code> source,</li>
  <li>one explicit front-panel posture,</li>
  <li>one explicit peripheral UI object realization file when applicable,</li>
  <li>one explicit FIR reading,</li>
  <li>one explicit lowering posture,</li>
  <li>one backend contract,</li>
  <li>one shared runtime-family acceptance posture,</li>
  <li>one Python mini runtime path,</li>
  <li>one Rust mini runtime path,</li>
  <li>one C/C++ mini runtime path,</li>
  <li>and, where applicable, one LLVM-oriented native executable path.</li>
</ul>

<p>
The canonical repository anchor for that campaign is currently:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/</code></pre>

<p>
That slice should be read as the first repository-visible proof that the same named example can be carried through:
</p>

<ul>
  <li>source,</li>
  <li>front-panel package,</li>
  <li>FIR,</li>
  <li>lowering,</li>
  <li>backend contract emission,</li>
  <li>runtime-family acceptance,</li>
  <li>runtime-family consumption,</li>
  <li>and a first native compiler-family corridor.</li>
</ul>

<p>
This campaign does <strong>not</strong> make one runtime the definition of FROG.
It makes the opposite point:
the language remains stable while downstream consumers remain modular.
</p>

<hr/>

<h2 id="positioning">Positioning</h2>

<p>
FROG is designed to combine the accessibility of graphical programming with the execution depth required for deterministic, industrial, embedded, high-performance, and safety-relevant systems.
</p>

<p>
Its ambition is to reduce the historical trade-off between:
</p>

<ul>
  <li>ease of expression,</li>
  <li>clarity of system design,</li>
  <li>deterministic execution,</li>
  <li>deployment scalability,</li>
  <li>hardware integration depth,</li>
  <li>human auditability of program structure.</li>
</ul>

<p align="center">
  <img src="frog-orville-chart.png" alt="FROG positioning chart" width="640" />
</p>

<p align="center">
  <em>
    FROG aims to combine graphical accessibility, explicit dataflow, auditability, modular downstream execution, and system-grade deployment in one open language model.
  </em>
</p>

<hr/>

<h2 id="breaking-the-syntax-first-bottleneck">Breaking the syntax-first bottleneck</h2>

<p>
A major barrier in many traditional programming environments is that useful system design often begins only after a long period of syntax learning, pattern memorization, and language-specific implementation habits.
</p>

<p>
This creates an inversion:
instead of starting from the system that should exist,
developers often start from the syntax they already know how to write.
</p>

<p>
That inversion limits experimentation and slows architectural thinking.
It encourages people to ask:
</p>

<p><strong>“What can I build with the implementation techniques I already master?”</strong></p>

<p>
rather than:
</p>

<p><strong>“What system should I build, and how should its behavior be expressed?”</strong></p>

<p>
FROG is designed to reduce that bottleneck by moving more of the developer’s effort toward:
</p>

<ul>
  <li>data movement,</li>
  <li>system structure,</li>
  <li>interfaces,</li>
  <li>control regions,</li>
  <li>state visibility,</li>
  <li>execution semantics.</li>
</ul>

<p>
The goal is not to eliminate engineering complexity.
The goal is to shift complexity toward the system itself rather than toward syntax-first representation.
</p>

<hr/>

<h2 id="why-frog-exists-now">Why FROG exists now</h2>

<p>
Graphical dataflow programming has already demonstrated major advantages in many engineering domains:
</p>

<ul>
  <li>natural parallelism,</li>
  <li>clear orchestration of behavior,</li>
  <li>strong correspondence between software structure and system behavior,</li>
  <li>high productivity for engineers, scientists, and domain experts,</li>
  <li>strong suitability for instrumentation, control, and observable systems.</li>
</ul>

<p>
However, many historical graphical environments have been tightly coupled to proprietary ecosystems where language, tooling, runtime, and hardware support are inseparable.
</p>

<p>
That model limits portability, slows independent ecosystem growth, prevents multiple actors from implementing the same language cleanly, and often leaves the saved program format and execution-facing layers too opaque for durable multi-vendor reuse.
</p>

<p>
FROG exists to define an <strong>open language specification</strong> for graphical dataflow programming that remains separate from:
</p>

<ul>
  <li>any single IDE,</li>
  <li>any single runtime,</li>
  <li>any single compiler,</li>
  <li>any single hardware vendor.</li>
</ul>

<p>
It also exists because the AI era changes the stakes.
Generated software, AI-assisted transformation, multi-target deployment, industrial security, and technological sovereignty all increase the value of open source formats, open execution-facing IRs, and modular downstream bridge boundaries.
</p>

<p>
Generative AI changes the economics of software creation.
It can produce code, tests, documentation, and system logic far faster than traditional human review processes were designed to absorb.
That acceleration does not remove the need for responsibility.
It makes responsibility, understanding, inspection, integration, and controlled evolution more important.
</p>

<p>
In industrial environments, software cannot merely be produced.
It must be understood, validated, attributed, maintained, audited, and controlled.
FROG exists in that context:
not as a rejection of AI-assisted software generation, but as a language architecture intended to make fast-generated or human-authored logic structurally inspectable and governable before it reaches real execution.
</p>

<p>
This repository therefore defines the language standard and the surrounding specification layers needed to support future conforming implementations.
The objective is to make it possible for different actors to build compatible FROG tooling while targeting one shared open language definition.
</p>

<hr/>

<h2 id="frog-in-the-ai-era">FROG in the AI era</h2>

<p>
FROG is not only relevant as an open graphical language.
It is also relevant as an <strong>AI-era auditability architecture</strong>.
</p>

<p>
A modern programming ecosystem increasingly needs representations that are:
</p>

<ul>
  <li>easy for tools to generate and transform,</li>
  <li>easy for humans to inspect and review,</li>
  <li>explicit enough to preserve structure across validation and derivation stages,</li>
  <li>open enough to avoid sovereignty loss through opaque vendor-controlled representations.</li>
</ul>

<p>
FROG addresses that need through three complementary properties:
</p>

<h3>Canonical JSON source</h3>

<p>
The canonical <code>.frog</code> source format is structured, machine-friendly, human-readable JSON.
That makes it naturally compatible with tooling pipelines, validation workflows, version control, deterministic serialization, and AI-assisted generation or transformation.
</p>

<h3>Graphically reviewable program structure</h3>

<p>
FROG keeps the executable structure explicit at the language level.
The program is not primarily hidden behind text parsing, coding idioms, or reconstruction tooling.
A reviewer can inspect nodes, ports, graph connections, structures, state boundaries, interface boundaries, widget interaction paths, probes, watch surfaces, and other source-meaningful execution objects directly as program objects.
</p>

<h3>Inspectable execution-facing FIR</h3>

<p>
FROG does not stop at an open source file.
The execution-facing FIR layer also remains open, inspectable, attributable, and recoverable.
This reduces the gap between:
</p>

<ul>
  <li>what was authored or generated,</li>
  <li>what was validated as program meaning,</li>
  <li>what was derived for execution-facing preparation,</li>
  <li>what is later lowered toward backend consumption.</li>
</ul>

<p>
That open FIR is also strategically important because it preserves the possibility of attaching multiple downstream runtime families and compiler families without making any one of them the hidden truth of the language.
</p>

<p>
This is why FROG is AI-compatible by architecture rather than by slogan.
AI-assisted generation can target structured source.
Human reviewers can inspect graph-level meaning.
Toolchains can validate semantics and derive FIR.
Downstream consumers can receive explicit contracts rather than opaque intent.
</p>

<p>
The strategic point is not that AI should be rejected.
The strategic point is that AI-assisted system creation needs better control surfaces.
When generation becomes cheap and fast, understanding becomes the scarce resource.
FROG is shaped for that scarcity: it provides structured source, graphical meaning, semantic validation, open FIR, explicit downstream handoff, and observable runtime/compiler consumption.
</p>

<hr/>

<h2 id="post-ai-accountability-inspection-and-control">Post-AI accountability: inspection and control</h2>

<p>
Generative AI is changing the bottleneck of software engineering.
The bottleneck is no longer only the ability to produce code.
Increasingly, the bottleneck is the ability to understand, inspect, validate, attribute, integrate, maintain, and evolve what has been produced.
</p>

<p>
That shift matters especially in industry.
When software drives machines, instruments, robots, production lines, embedded systems, energy systems, medical systems, or safety-relevant workflows, a generated artifact is not enough.
A serious organization must be able to answer:
</p>

<ul>
  <li>what logic is being executed,</li>
  <li>which source artifact owns that logic,</li>
  <li>which semantic rules validated it,</li>
  <li>which execution-facing representation was derived,</li>
  <li>which lowering and backend contract were consumed,</li>
  <li>which runtime or compiler family executed it,</li>
  <li>which behavior was expected,</li>
  <li>which behavior was observed,</li>
  <li>and who is responsible for accepting that chain.</li>
</ul>

<p>
FROG is designed to support that responsibility chain.
It does not claim that graphical representation automatically makes software correct.
It claims that structured graphical source, explicit dataflow, validated meaning, open FIR, lowering, backend contracts, acceptance artifacts, and runtime/compiler separation make the system easier to inspect and govern than opaque generated code alone.
</p>

<pre><code>AI-generated or human-authored intent
        |
        v
structured .frog source
        |
        v
graphical review and source-level inspection
        |
        v
semantic validation
        |
        v
open execution-facing FIR
        |
        v
lowering and backend contract
        |
        v
runtime-family or compiler-family consumption
        |
        v
observable execution and accountable evolution
</code></pre>

<p>
In that sense, FROG is not anti-AI.
FROG is a control architecture for a world where AI can generate more software than humans can comfortably inspect through linear text alone.
It gives humans and organizations a structured interface for understanding, reviewing, controlling, and evolving fast-produced software.
</p>

<p>
This is also why FROG matters for future engineering roles.
As code generation becomes more automated, the most valuable engineers will increasingly be those who can specify, inspect, validate, integrate, and govern complex generated systems.
FROG aims to provide a language and representation stack suited to that shift.
</p>

<hr/>

<h2 id="dataflow-programming">Dataflow programming</h2>

<p>
FROG follows a true <strong>dataflow execution model</strong>.
</p>

<p>
In instruction-sequenced programming, execution is primarily described as ordered steps.
In dataflow programming, operations become executable when their required input data is available.
</p>

<pre>
Traditional execution

A → B → C → D


Dataflow execution

   A
  / \
 B   C
  \ /
   D
</pre>

<p>
Execution order therefore emerges from dependencies rather than from manually authored textual ordering.
This model enables:
</p>

<ul>
  <li>automatic parallelism where valid,</li>
  <li>clear dependency visibility,</li>
  <li>deterministic execution models where required,</li>
  <li>efficient mapping to heterogeneous hardware.</li>
</ul>

<hr/>

<h2 id="from-prototyping-to-critical-systems">From prototyping to critical systems</h2>

<p>
FROG is designed to support both rapid experimentation and demanding deployment.
</p>

<p>
The same programming model is intended to scale across domains such as:
</p>

<ul>
  <li>scientific computing,</li>
  <li>measurement and control,</li>
  <li>industrial automation,</li>
  <li>embedded systems,</li>
  <li>real-time control,</li>
  <li>microcontroller-oriented execution,</li>
  <li>accelerated and edge computing,</li>
  <li>high-performance systems.</li>
</ul>

<p>
Usability, execution depth, and auditability are treated as complementary goals rather than mutually exclusive ones.
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

<hr/>

<h2 id="repository-structure">Repository structure</h2>

<p>
This repository is organized by <strong>architectural responsibility</strong> plus repository-level support areas.
The six core specification families remain the architectural baseline of FROG.
The support areas exist to make that baseline more inspectable, testable, executable, and governable without moving normative ownership away from the specification layers.
</p>

<pre><code>FROG/
│
├── Conformance/                      Public accept / reject / preserve expectations
├── Examples/                         Illustrative named source slices and executable example dossiers
├── Expression/                       Canonical source specification for .frog programs
├── IDE/                              IDE architecture, authoring, observability, debugging, and inspection
├── IR/                               Canonical open execution-facing representation and downstream handoff boundaries
├── Implementations/
│   └── Reference/                    Non-normative reference implementation workspace and executable prototypes
├── Language/                         Normative execution semantics for validated program meaning
├── Libraries/                        Intrinsic standardized primitive-library specifications
├── Profiles/                         Optional standardized capability-family specifications
├── Roadmap/                          Non-normative closure sequencing and milestone tracking
├── Strategy/                         Non-normative strategic framing layer
├── Versioning/                       Centralized specification-version governance and current-status matrix
│
├── assets/                           Shared repository assets used by README and GitHub Pages
├── CLA.md                            Contributor license agreement requirements
├── CONTRIBUTING.md                   Contribution process and contribution rules
├── GOVERNANCE.md                     Governance, stewardship, and ecosystem model
├── FROG logo.svg                     Official logo asset
├── LICENSE                           Repository license
├── Readme.md                         Repository landing page and architectural overview
└── frog-orville-chart.png            Positioning illustration used by the repository
</code></pre>

<p>
The six core specification families are:
</p>

<ul>
  <li><strong><code>Expression/</code></strong></li>
  <li><strong><code>Language/</code></strong></li>
  <li><strong><code>IR/</code></strong></li>
  <li><strong><code>Libraries/</code></strong></li>
  <li><strong><code>Profiles/</code></strong></li>
  <li><strong><code>IDE/</code></strong></li>
</ul>

<p>
The current repository-level support and governance areas are:
</p>

<ul>
  <li><strong><code>Examples/</code></strong> — illustrative named source cases and executable closure dossiers,</li>
  <li><strong><code>Conformance/</code></strong> — expected outcomes for validation, preservation, and rejection,</li>
  <li><strong><code>Implementations/Reference/</code></strong> — non-normative prototype workspace used to exercise the current reference path,</li>
  <li><strong><code>Versioning/</code></strong> — centralized current corpus-governance and per-surface current-status reporting.</li>
</ul>

<hr/>

<h2 id="repository-runtime-and-native-execution-direction">Runtime and native execution direction</h2>

<p>
The repository direction is now intentionally explicit:
the same canonical example corridor should be consumable through <strong>multiple runtime implementations</strong> and, where applicable, through a <strong>native compiler-oriented path</strong>.
</p>

<p>
The preferred long-term reading for a serious executable example is:
</p>

<pre><code>canonical .frog source
      |
      v
validated meaning
      |
      v
FIR
      |
      v
lowering
      |
      v
backend contract and/or compiler-facing lowered artifact
      |
      +-------------------------------+-------------------------------+-------------------------------+
      |                               |                               |
      v                               v                               v
Python mini runtime            Rust mini runtime               C/C++ mini runtime
      |
      +------------------------------- shared acceptance surface -------------------------------+
      |
      \------------------------------- optional LLVM-oriented native path ----------------------->
</code></pre>

<p>
For the canonical Example 05 slice, the repository now materially exposes this direction in bounded form:
</p>

<ul>
  <li>a Python runtime-family consumer,</li>
  <li>a Rust runtime-family consumer,</li>
  <li>a C/C++ runtime-family consumer,</li>
  <li>a shared runtime-family acceptance posture,</li>
  <li>and a first LLVM-oriented native proof path.</li>
</ul>

<p>
This does not imply that all of these paths are already closed for all published examples.
It defines the explicit repository direction and, for Example 05, a first repository-visible bounded closure.
</p>

<p>
The reference implementation workspace is therefore expected to remain <strong>stage-separated</strong> and <strong>family-separated</strong>:
Python, Rust, and C/C++ runtime realizations should be understandable as parallel consumers of the same corridor,
while LLVM-oriented native compilation remains a downstream consumer path rather than the definition of FROG itself.
</p>

<hr/>

<h2 id="internal-documentation-map">Internal documentation map</h2>

<p>
The repository contains multiple normative and architectural documents.
The map below summarizes the intended role of the major Markdown documents in the current baseline of the repository.
</p>

<pre><code>FROG/
├── Readme.md
│   -> repository landing page and global architectural entry point
├── CONTRIBUTING.md
│   -> contribution workflow, expectations, and cross-document coherence rules
├── CLA.md
│   -> contributor license agreement entry point and legal contribution notice
├── GOVERNANCE.md
│   -> repository governance, stewardship model, conformance direction,
│      certification direction, and branding boundary
│
├── Examples/
│   └── Readme.md
│       -> architectural role of named slices, executable example dossiers,
│          closure status expectations, and relation with reference consumers
│
├── Conformance/
│   └── Readme.md
│       -> public conformance posture, staged expected outcomes,
│          preservation obligations, and rejection expectations
│
├── Implementations/
│   └── Reference/
│       └── Readme.md
│           -> non-normative reference workspace, executable-slice purpose,
│              stage-separated architecture, runtime-family posture,
│              and native-path direction
│
├── Expression/
│   ├── Readme.md
│   │   -> architectural entry point for canonical source representation
│   ├── Schema.md
│   │   -> source-schema posture and machine-checkable structural validation boundary
│   ├── Diagram.md
│   │   -> authoritative executable graph as canonical source representation
│   ├── Front panel.md
│   │   -> optional front-panel composition and interaction surface
│   ├── Widget.md
│   │   -> widget instance model, identity, value behavior, properties, methods, and events
│   └── Widget interaction.md
│       -> diagram-side widget interaction paths and execution-facing access model
│
├── Language/
│   ├── Readme.md
│   │   -> architectural entry point for normative execution semantics
│   ├── Control structures.md
│   │   -> normative execution meaning of case, for_loop, and while_loop
│   └── State and cycles.md
│       -> normative meaning of explicit local memory and valid feedback cycles
│
├── IR/
│   ├── Readme.md
│   │   -> architectural entry point for the FIR / IR layer and ownership boundary
│   ├── Execution IR.md
│   │   -> canonical open execution-facing representation
│   ├── Derivation rules.md
│   │   -> correspondence rules from validated meaning to execution-facing representation
│   ├── Lowering.md
│   │   -> lowering boundary toward target-oriented executable forms
│   └── Backend contract.md
│       -> backend-facing contract for downstream consumers
│
├── IDE/
│   ├── Readme.md
│   │   -> IDE architecture entry point
│   ├── Observability.md
│   │   -> probes, watches, execution projection, and inspection-facing posture
│   ├── Debugging.md
│   │   -> debugging control, stop semantics at the IDE layer, and runtime-to-source projection consumption
│   ├── Probes.md
│   │   -> local live inspection tools
│   └── Watch.md
│       -> persistent centralized inspection tools
│
├── Implementations/Reference/Runtime/
│   ├── Readme.md
│   │   -> runtime-family entry point and shared consumer posture
│   ├── acceptance/Readme.md
│   │   -> shared acceptance material for runtime-family consumers
│   ├── python/Readme.md
│   │   -> Python mini-runtime posture and example pipe direction
│   ├── rust/Readme.md
│   │   -> Rust mini-runtime posture and example pipe direction
│   └── cpp/Readme.md
│       -> C/C++ mini-runtime posture and example pipe direction
│
└── Implementations/Reference/LLVM/
    └── Readme.md
        -> LLVM-oriented native path posture and ownership boundary
</code></pre>

<hr/>

<h2 id="recommended-reading-path">Recommended reading path</h2>

<p>
Readers who are new to the repository should normally approach it in the following order:
</p>

<pre>
Readme.md
   |
   v
Expression/Readme.md
   |
   v
Expression/Schema.md
   |
   v
Language/Readme.md
   |
   v
IR/Readme.md
   |
   v
Libraries/Readme.md
   |
   v
Profiles/Readme.md
   |
   v
IDE/Readme.md
</pre>

<p>
Readers who want to understand the currently published repository-level executable/reference path SHOULD then continue with:
</p>

<pre>
Examples/Readme.md
   |
   v
Examples/05_bounded_ui_accumulator/Readme.md
   |
   v
Conformance/Readme.md
   |
   v
Implementations/Reference/Readme.md
   |
   v
Implementations/Reference/Runtime/Readme.md
   |
   +-- Implementations/Reference/Runtime/acceptance/Readme.md
   |
   +-- Implementations/Reference/Runtime/python/Readme.md
   +-- Implementations/Reference/Runtime/rust/Readme.md
   +-- Implementations/Reference/Runtime/cpp/Readme.md
   |
   \-- Implementations/Reference/LLVM/Readme.md
</pre>

<p>
That second path answers a staged set of questions:
</p>

<ul>
  <li><strong><code>Examples/</code></strong> — which illustrative named slices are being used,</li>
  <li><strong><code>Examples/05_bounded_ui_accumulator/</code></strong> — which bounded applicative corridor is currently the primary anchor,</li>
  <li><strong><code>Conformance/</code></strong> — what those slices are expected to validate, preserve, or reject,</li>
  <li><strong><code>Implementations/Reference/</code></strong> — how a non-normative prototype pipeline currently tries to process them,</li>
  <li><strong><code>Runtime/</code></strong> — how the shared runtime family is organized,</li>
  <li><strong><code>Runtime/acceptance/</code></strong> — which observables keep runtime-family consumers aligned,</li>
  <li><strong><code>python/</code>, <code>rust/</code>, <code>cpp/</code></strong> — how the example corridor is consumed per runtime language,</li>
  <li><strong><code>LLVM/</code></strong> — how the native compiler-oriented path stays downstream from FROG rather than defining it.</li>
</ul>

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

<h2 id="governance-official-branding-and-ecosystem">Governance, official branding, and ecosystem</h2>

<p>
FROG is governed as an <strong>open specification</strong>.
The repository is intended to remain readable, implementable, and usable by independent parties while preserving long-term architectural coherence.
</p>

<p>
The current governance model is steward-led.
Graiphic is the initial steward of the FROG specification repository and is responsible for maintaining architectural coherence, reviewing proposed changes, and publishing authoritative repository revisions.
</p>

<p>
Version governance, transition rules, and current repository status belong to the dedicated repository governance surfaces under <code>Versioning/</code>.
Individual architectural documents should remain modular and should not become standalone version-governance documents.
</p>

<p>
The ecosystem direction is intentionally open:
multiple independent IDEs, runtimes, compiler bridges, validators, and tooling layers may eventually coexist around the same open standard.
At the same time, official branding and the official flagship product identity may remain controlled.
Graiphic intends to build the flagship proprietary IDE on top of the FROG standard, and that official IDE may be the one carrying the formal FROG product name.
</p>

<hr/>

<h2 id="project-status">Project status</h2>

<p>
FROG is currently under active design, cleanup, stabilization, and executable-corridor closure.
The repository already contains substantial material across canonical source representation, source-schema posture, language semantics, execution-facing IR architecture, intrinsic standardized primitive libraries, optional profile architecture, IDE architecture, governance surfaces, strategic framing, roadmap posture, examples, conformance material, and a non-normative reference implementation workspace.
</p>

<p>
At the current published state, the repository has now reached a stronger closure milestone than before:
it materially exposes a repository-visible bounded Example 05 corridor across source, front-panel package, FIR, lowering, backend contract emission, shared runtime-family acceptance, Python runtime, Rust runtime, C/C++ runtime, and a first LLVM-oriented native path.
</p>

<p>
At the same time, the repository has not yet reached:
</p>

<ul>
  <li>full generalized multi-runtime symmetry across all serious examples,</li>
  <li>full native rendered front-panel closure,</li>
  <li>or final depth across all observability, debugging, and IDE-facing surfaces.</li>
</ul>

<p>
The current direction is therefore twofold:
</p>

<ul>
  <li><strong>stabilize the open specification layers</strong>,</li>
  <li><strong>close repository-visible serious examples from source to runtime and, where declared, to native execution</strong>.</li>
</ul>

<p>
The long-term ambition is to establish a durable open graphical programming ecosystem that can scale from experimentation to deeply integrated industrial deployment while remaining inspectable across the source, semantic, execution-facing, observability, and governance layers.
</p>

<p>
In the generative AI era, that ambition also carries a responsibility dimension:
FROG aims to help keep fast-produced software understandable, attributable, controllable, and evolvable by humans and organizations that remain responsible for real execution.
</p>

<hr/>

<h2 id="license">License</h2>

<p>
This project is licensed under the <strong>Apache License 2.0</strong>.
See <code>LICENSE</code> for details.
</p>

<p>
External contributions are governed through the repository contribution process and Contributor License Agreement requirements.
See <code>CONTRIBUTING.md</code> and <code>CLA.md</code>.
</p>

<p>
Repository stewardship, governance direction, and ecosystem positioning are described in <code>GOVERNANCE.md</code>.
</p>

<p align="center">
  <a href="https://cla-assistant.io/Graiphic/FROG">
    <img src="./assets/cla-assistant-badge.svg" alt="CLA Assistant" width="640" />
  </a>
</p>

<hr/>

<p align="center">
  <strong>FROG — Free Open Graphical Language</strong><br/>
  Open graphical dataflow programming, specified as a language rather than owned as a product.
</p>
