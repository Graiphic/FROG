<h1>FROG Strategy and Positioning</h1>

<p>
Strategic framing for FROG: what the language is, why Graiphic opened this layer, how it relates to GO HW, and why inspectable graphical dataflow matters in the AI era.
</p>

<p>
This document preserves long-form material that was previously maintained in
the public root README. The root README now acts as a concise orientation page
and links here for detail.
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
The next implementation priority after keeping the checks green is to remove unnecessary example-specific execution logic.
The target direction is:
</p>

<pre><code>backend contract JSON
  -> generic reference contract executor
  -> observed runtime snapshot

lowered_unit.kind
  -> generic LLVM emitter dispatch
  -> native proof module
</code></pre>

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
