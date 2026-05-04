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
  FROG can also carry optional source provenance through <code>ide.provenance</code>, allowing provenance-aware tools to distinguish unknown, human-direct, AI-generated, AI-assisted, imported, reviewed, stale, invalid, and untrusted source-object states without changing executable meaning.
</p>

<p align="center">
  Specification work initiated: <strong>8 March 2026</strong>
</p>

<p align="center">
  <a href="#what-is-frog">What is FROG?</a> •
  <a href="#frog-whitepaper">FROG whitepaper</a> •
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

<h2 id="frog-whitepaper">FROG Whitepaper</h2>

<p>
A dedicated strategic and technical whitepaper is available for readers who want a broader explanation of FROG’s purpose, technological ambition, relationship with SOTA GO and GO HW, AI-era relevance, runtime/compiler modularity, open FIR posture, and long-term ecosystem strategy.
</p>

<p>
This whitepaper explains why FROG should be understood not merely as a graphical tool, but as an open graphical dataflow language foundation designed for inspectable, hardware-agnostic, compiler-agnostic, AI-ready industrial software.
</p>

<ul>
  <li>
    <a href="https://github.com/Graiphic/GO-Whitepapers/tree/main/FROG%20-%20Opening%20the%20Next%20Era%20of%20Graphical">
      Open the FROG whitepaper repository page
    </a>
  </li>
  <li>
    <a href="https://graiphic.github.io/GO-Whitepapers/#/FROG%20-%20Opening%20the%20Next%20Era%20of%20Graphical/Readme">
      Open the FROG whitepaper on GitHub Pages
    </a>
  </li>
</ul>

<p>
The whitepaper is non-normative.
The authoritative specification remains this FROG repository.
The whitepaper provides strategic framing, industrial positioning, and a higher-level explanation of why FROG matters in the continuity of Graiphic’s GO program.
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
      <td>Generated or human-authored logic can be carried through explicit source, validation, FIR, lowering, backend contracts, acceptance, runtime/compiler consumption, and optional <code>ide.provenance</code> evidence.</td>
      <td>Industrial users can reason about responsibility, review, control, provenance, and evolution instead of receiving a large opaque block of generated code.</td>
    </tr>
    <tr>
      <td><strong>Security-oriented</strong></td>
      <td>FROG does not claim that graphical form automatically guarantees safety; it reduces opacity through explicit structure and inspectable artifacts.</td>
      <td>Security and assurance conversations can be grounded in auditability, traceability, and controlled downstream handoff.</td>
    </tr>
  </tbody>
</table>

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
At the current published state, the repository contains the six core architectural specification families:
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
  <li><strong><code>Examples/</code></strong> — illustrative named source slices, executable example dossiers, and bounded compiler-corridor mirrors,</li>
  <li><strong><code>Conformance/</code></strong> — public accept / reject / preserve expectations for the published repository state,</li>
  <li><strong><code>Implementations/Reference/</code></strong> — a non-normative reference implementation workspace used to exercise disciplined execution paths,</li>
  <li><strong><code>Versioning/</code></strong> — centralized specification-governance and current-status reporting for the published specification corpus,</li>
  <li><strong><code>Strategy/</code></strong> — a non-normative strategic framing layer distinct from normative ownership,</li>
  <li><strong><code>Roadmap/</code></strong> — a non-normative closure-sequencing layer distinct from both strategy and specification.</li>
</ul>

<p>
The published numbered example surface now exposes a repository-visible executable progression under <code>Examples/01_*</code> through <code>Examples/05_*</code>.
Each numbered example has a canonical <code>.frog</code> source, a published FIR artifact, a published lowering artifact, a backend-contract path, runtime acceptance material, and an LLVM-oriented native proof surface.
</p>

<p>
The progression is intentionally staged:
</p>

<pre><code>01_pure_addition
  -> pure public-interface arithmetic

02_ui_value_roundtrip
  -> natural widget_value participation

03_ui_property_write
  -> widget_reference and frog.ui.property_write

04_stateful_feedback_delay
  -> explicit delay-backed feedback state

05_bounded_ui_accumulator
  -> combined UI + property write + bounded loop + explicit state corridor
</code></pre>

<p>
<code>Examples/05_bounded_ui_accumulator/</code> remains the primary applicative vertical-slice anchor because it combines source, front-panel package, FIR, lowering, backend contract, runtime-family acceptance, Python/Rust/C/C++ reference-consumer posture, and an LLVM-oriented native proof path.
Examples <code>01</code> through <code>04</code> are smaller executable proof slices used to keep individual concerns inspectable before they are combined in Example <code>05</code>.
</p>

<p>
The correct current statement is therefore:
the repository materially exposes an executable Examples <code>01</code> through <code>05</code> corridor, while Example <code>05</code> remains the richest applicative UI/state/runtime/native reference corridor.
This does not claim full generalized symmetry across all future examples, all runtime families, or rendered-native front-panel closure.
</p>

<hr/>

<h2 id="campaign-priority">Campaign priority</h2>

<p>
The current campaign priority is explicit:
<strong>keep the published executable corridor green, then generalize cautiously.</strong>
</p>

<p>
A serious example is no longer considered finished merely because it is source-readable or architecturally plausible.
A serious example should progressively converge toward:
</p>

<ul>
  <li>one canonical <code>.frog</code> source,</li>
  <li>one explicit front-panel posture when applicable,</li>
  <li>one explicit FIR reading,</li>
  <li>one explicit lowering posture,</li>
  <li>one backend contract,</li>
  <li>one shared runtime-acceptance posture,</li>
  <li>and, where applicable, one LLVM-oriented native proof path.</li>
</ul>

<p>
The current published numbered examples provide this progression in bounded form:
</p>

<pre><code>.frog
  -> FIR
  -> lowering
  -> backend contract
  -> runtime acceptance
  -> LLVM proof
</code></pre>

<p>
Example <code>05_bounded_ui_accumulator</code> remains the primary applicative corridor for runtime-family and UI-facing work.
The earlier examples now serve as smaller executable anchors for pure computation, widget value flow, object-style UI effects, and explicit state.
</p>

<p>
This campaign does <strong>not</strong> make one runtime the definition of FROG.
It makes the opposite point:
the language remains stable while downstream consumers remain modular and independently checkable.
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
This model enables automatic parallelism where valid, clear dependency visibility, deterministic execution models where required, and efficient mapping to heterogeneous hardware.
</p>

<hr/>

<h2 id="from-prototyping-to-critical-systems">From prototyping to critical systems</h2>

<p>
FROG is designed to support both rapid experimentation and demanding deployment.
</p>

<p>
The same programming model is intended to scale across domains such as scientific computing, measurement and control, industrial automation, embedded systems, real-time control, microcontroller-oriented execution, accelerated and edge computing, and high-performance systems.
</p>

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
A FROG may exist without a front panel.
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
FROG treats execution observability as a first-class architectural concern.
Probes and watches belong to observability, debugging, and IDE-facing architecture.
They do not redefine executable meaning.
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
</code></pre>

<p>
That modularity does not mean all downstream bridges already exist.
It means the architecture is deliberately shaped so they can exist without changing the language truth.
</p>

<hr/>

<h2 id="repository-structure">Repository structure</h2>

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
├── Libraries/                        Intrinsic standardized primitive and widget-library specifications
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

<hr/>

<h2 id="repository-runtime-and-native-execution-direction">Runtime and native execution direction</h2>

<p>
The repository direction is intentionally explicit:
published examples should become consumable through runtime-family and compiler-family paths without making either path the definition of FROG.
</p>

<p>
The current executable reference reading is:
</p>

<pre><code>canonical .frog source
      |
      v
FIR
      |
      v
lowering
      |
      +----------------------------+----------------------------+
      |                            |
      v                            v
backend contract              LLVM-oriented module
      |                            |
      v                            v
runtime acceptance            native proof
</code></pre>

<p>
For Examples <code>01</code> through <code>04</code>, the runtime acceptance and LLVM proofs are intentionally narrow reference proofs.
For Example <code>05</code>, the repository carries the richer applicative path involving a front-panel package, widget values, widget references, UI property writes, explicit state, bounded iteration, runtime-family acceptance, and LLVM native proof material.
</p>

<p>
The reference implementation workspace remains stage-separated:
Deriver, Lowerer, ContractEmitter, Runtime, and LLVM are downstream consumers of the published source/FIR/lowering corridor rather than semantic owners of the language.
</p>

<hr/>

<h2 id="internal-documentation-map">Internal documentation map</h2>

<pre><code>FROG/
├── Readme.md
│   -> repository landing page and global architectural entry point
├── Examples/
│   └── Readme.md
│       -> numbered executable example progression and closure state
├── Expression/
│   -> canonical source representation
├── Language/
│   -> normative execution semantics
├── IR/
│   -> execution-facing FIR, derivation rules, lowering, backend contract
├── Libraries/
│   -> intrinsic primitives, widget class law, and Default realizations
├── IDE/
│   -> authoring, observability, debugging, inspection
├── Implementations/Reference/
│   -> non-normative executable workspace
│      ├── ArtifactChecks/
│      ├── Deriver/
│      ├── Lowerer/
│      ├── ContractEmitter/
│      ├── Runtime/
│      ├── LLVM/
│      └── Pipeline/
└── Versioning/
    -> centralized status and governance matrix
</code></pre>

<hr/>

<h2 id="recommended-reading-path">Recommended reading path</h2>

<p>
Readers who are new to the repository should normally approach it in the following order:
</p>

<pre><code>Readme.md
   |
   v
Expression/Readme.md
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
</code></pre>

<p>
Readers who want to understand the current executable reference path should then continue with:
</p>

<pre><code>Examples/Readme.md
   |
   v
Examples/01_pure_addition/Readme.md
Examples/02_ui_value_roundtrip/Readme.md
Examples/03_ui_property_write/Readme.md
Examples/04_stateful_feedback_delay/Readme.md
Examples/05_bounded_ui_accumulator/Readme.md
   |
   v
Implementations/Reference/checks.md
   |
   v
Implementations/Reference/Pipeline/check_examples01_05_full.py
</code></pre>

<hr/>

<h2 id="specification-architecture">Specification architecture</h2>

<p>
The repository is intentionally split into distinct architectural layers:
</p>

<ul>
  <li><strong>Expression</strong> — canonical source representation, source sections, source serialization rules, source-schema posture, and structural validity,</li>
  <li><strong>Language</strong> — normative execution semantics for validated program meaning,</li>
  <li><strong>IR</strong> — canonical open execution-facing representations derived from validated program meaning,</li>
  <li><strong>Libraries</strong> — intrinsic standardized primitive vocabularies and widget class-law surfaces,</li>
  <li><strong>Profiles</strong> — optional standardized capability families and profile-owned capability contracts,</li>
  <li><strong>IDE</strong> — authoring architecture, editor-facing models, execution observability, debugging semantics, inspection workflows, snippets, and Express authoring.</li>
</ul>

<pre><code>what is saved      -> Expression/
what is true       -> Language/
what is derived    -> IR/
what exists        -> Libraries/ and Profiles/
what is edited     -> IDE/
what is exemplified -> Examples/
what is prototyped -> Implementations/Reference/
what version means -> Versioning/
</code></pre>

<hr/>

<h2 id="program-representation">Program representation</h2>

<p>
FROG programs should be understood across distinct representation levels:
</p>

<ol>
  <li><strong>FROG Expression</strong> — the serialized source representation stored in a <code>.frog</code> file.</li>
  <li><strong>Structural validity</strong> — explicit source-shape validity owned by <code>Expression/</code>.</li>
  <li><strong>FROG Program Model</strong> — editable source-derived in-memory representation for IDEs.</li>
  <li><strong>Validated program meaning</strong> — normative semantic acceptance.</li>
  <li><strong>Canonical open execution-facing representation</strong> — FIR posture.</li>
  <li><strong>Lowering / backend-facing handoff</strong> — downstream specialized representation.</li>
</ol>

<pre><code>.frog source
    |
    v
loadability
    |
    v
structural validity
    |
    v
validated program meaning
    |
    v
canonical execution-facing FIR
    |
    v
lowering / backend-facing handoff
    |
    +--> runtime-family consumption
    \--> compiler-family consumption
</code></pre>

<hr/>

<h2 id="execution-architecture">Execution architecture</h2>

<p>
A conforming FROG ecosystem should separate authoring, canonical source, structural validity, validated program meaning, canonical open execution-facing representation, and target-specific execution realization.
</p>

<pre><code>FROG IDE / tooling
       |
       v
FROG Program Model
       |
       v
FROG Expression (.frog)
       |
       v
validation against Expression / Language / Libraries / Profiles
       |
       v
FROG Execution IR
       |
       v
lowering / backend-facing handoff
       |
       +--------------------------+
       |                          |
       v                          v
runtime-family consumers     compiler-family consumers
       |                          |
       v                          v
observable execution         native / target artifact
</code></pre>

<p>
A serious downstream compiler path may target compiler families such as LLVM.
However, those downstream families remain consumers of lowered FROG forms rather than the definition of FROG itself.
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

<ul>
  <li><strong>Probes</strong> expose localized value inspection on graph-meaningful surfaces.</li>
  <li><strong>Watches</strong> provide persistent observation of selected values, references, states, widgets, or public-interface objects.</li>
  <li><strong>Source projection</strong> maps runtime observations back to meaningful FROG objects.</li>
</ul>

<hr/>

<h2 id="execution-targets">Execution targets</h2>

<p>
FROG programs are designed to remain source-level stable across multiple hardware classes.
The language is not tied to one processor family, one operating system, one runtime architecture, or one vendor.
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
FROG aims to provide an <strong>open industrial graphical programming standard</strong> that hardware and software ecosystems can build on without requiring a proprietary language boundary.
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
Its narrower and stronger claim is that open source, explicit graph structure, open FIR, and readable governance reduce structural opacity and improve inspectability.
</p>

<hr/>

<h2 id="interoperability">Interoperability</h2>

<p>
FROG is designed for interoperability at several levels:
source interoperability, editing interoperability, structural interoperability, semantic interoperability, IR interoperability, execution interoperability, governance interoperability, and ecosystem interoperability.
</p>

<p>
Representative integration targets may include C / C++, Rust, Python, .NET, other ABI-compatible environments, runtime stacks, compiler stacks, and hardware adaptation layers.
</p>

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
  <li>standardized widget class-law surfaces,</li>
  <li>optional standardized capability profiles,</li>
  <li>compiler implementations,</li>
  <li>backend implementations,</li>
  <li>runtime implementations,</li>
  <li>development environments,</li>
  <li>hardware adaptation layers,</li>
  <li>deployment and orchestration layers.</li>
</ul>

<p>
At the modeling level, FROG also separates public interface from front panel, natural widget value flow from object-style widget interaction, program execution meaning from probes and watches, runtime families from compiler families, and specification corpus governance from source-level metadata.
</p>

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
</p>

<hr/>

<h2 id="project-status">Project status</h2>

<p>
FROG is currently under active design, cleanup, stabilization, and executable-corridor closure.
The repository already contains substantial material across canonical source representation, source-schema posture, language semantics, execution-facing IR architecture, intrinsic standardized primitive libraries, optional profile architecture, IDE architecture, governance surfaces, strategic framing, roadmap posture, examples, conformance material, and a non-normative reference implementation workspace.
</p>

<p>
At the current published state, the repository has reached a stronger closure milestone:
Examples <code>01</code> through <code>05</code> materially expose a repository-visible executable corridor across source, FIR, lowering, backend contracts, runtime acceptance, and LLVM-oriented proof material.
</p>

<p>
The Example <code>05_bounded_ui_accumulator</code> slice remains the primary applicative vertical-slice anchor because it combines front-panel package participation, widget values, widget references, UI property writes, bounded iteration, explicit state, public output, runtime-family acceptance, and LLVM-native proof posture.
Examples <code>01</code> through <code>04</code> provide smaller executable anchors for isolated concerns.
</p>

<p>
At the same time, the repository has not yet reached:
</p>

<ul>
  <li>full generalized multi-runtime symmetry across all serious examples,</li>
  <li>a generic contract executor that removes all example-specific runtime acceptance logic,</li>
  <li>a generic LLVM backend driven by lowered-unit kind rather than bounded example patterns,</li>
  <li>full native rendered front-panel closure,</li>
  <li>or final depth across all observability, debugging, and IDE-facing surfaces.</li>
</ul>

<p>
The current direction is therefore:
</p>

<ul>
  <li><strong>keep the Examples 01–05 executable corridor green,</strong></li>
  <li><strong>convert the reference runtime and LLVM proofs from example-specific code toward generic pattern-driven engines,</strong></li>
  <li><strong>then resume qualitative deepening of complex widget families and front-panel runtime behavior.</strong></li>
</ul>

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
