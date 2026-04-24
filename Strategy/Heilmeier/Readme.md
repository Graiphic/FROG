<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">🐸 FROG — Heilmeier Framing</h1>

<p align="center">
  Strategic framing for the technological purpose, expected impact, ecosystem significance, and long-term program logic of FROG<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<p align="center">
  FROG is a post-AI accountability architecture for executable graphical programming:<br/>
  it aims to make generated or human-authored system logic inspectable, attributable, controllable, evolvable, and bridgeable across runtime, compiler, and hardware families.
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#why-this-page-exists">1. Why this Page Exists</a></li>
  <li><a href="#strategic-thesis">2. Strategic Thesis</a></li>
  <li><a href="#positioning-gap">3. Positioning Gap</a></li>
  <li><a href="#heilmeier-h1">4. Heilmeier H1 — What are you trying to do?</a></li>
  <li><a href="#heilmeier-h2">5. Heilmeier H2 — How is it done today, and what are the limits?</a></li>
  <li><a href="#heilmeier-h3">6. Heilmeier H3 — What is new, and why do we think it will work?</a></li>
  <li><a href="#heilmeier-h4">7. Heilmeier H4 — Who cares if we succeed?</a></li>
  <li><a href="#heilmeier-h5">8. Heilmeier H5 — What difference will success make?</a></li>
  <li><a href="#heilmeier-h6">9. Heilmeier H6 — What are the risks and what still needs to be proved?</a></li>
  <li><a href="#heilmeier-h7">10. Heilmeier H7 — How much will it cost?</a></li>
  <li><a href="#heilmeier-h8">11. Heilmeier H8 — How long will it take and what are the midterm and final exams?</a></li>
  <li><a href="#relation-with-strategy-roadmap-and-versioning">12. Relation with Strategy, Roadmap, and Versioning</a></li>
  <li><a href="#what-is-already-demonstrated-in-the-repository">13. What is Already Demonstrated in the Repository</a></li>
  <li><a href="#future-end-to-end-poc-direction">14. Future End-to-End POC Direction</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="why-this-page-exists">1. Why this Page Exists</h2>

<p>
This page explains why FROG exists as a technological program, what category gap it targets, why that gap matters now, what is already demonstrated in the repository, and what still needs to be proved before the long-term multi-runtime and multi-compiler vision is realized.
</p>

<p>
This page is intentionally strategic.
It is not the normative language definition.
It is not the reference implementation documentation.
It is not the roadmap.
It is not the current corpus-version status surface.
It is a program-framing document that explains the problem, the opportunity, the expected impact, the risk posture, and the long-term execution logic behind FROG.
</p>

<p>
It also clarifies an important strategic evolution in Graiphic’s trajectory.
GO HW made visible how powerful graphical orchestration can be when software logic, AI, and hardware concerns are treated as one architectural system.
FROG represents the next step in that trajectory:
going deeper than one cockpit, one runtime, one IDE, or one integration stack, and opening the language foundation itself.
</p>

<p>
That shift is difficult, ambitious, and deliberate.
It moves the center of gravity from “how to build and operate graphical systems well” toward “how to make the language, the execution-facing representation, and the downstream bridge boundaries themselves open, inspectable, modular, and governable.”
</p>

<p>
In the generative AI era, that shift becomes even more important.
The ability to produce software is becoming increasingly abundant.
The scarce resource becomes accountable understanding:
the ability to know what was produced, what it means, who validated it, how it changed, what it will execute, and who remains responsible for it.
</p>

<hr/>

<h2 id="strategic-thesis">2. Strategic Thesis</h2>

<p>
FROG aims to make <strong>graphical system-grade programming open, inspectable, hardware-agnostic, compiler-agnostic, auditable, accountable, evolvable, and deployable</strong> across heterogeneous execution targets.
</p>

<p>
The core strategic claim is not merely that FROG is graphical.
The core claim is that FROG attacks the structural lock-in of graphical industrial programming by opening the language layer itself:
</p>

<ul>
  <li>the saved program format,</li>
  <li>the canonical source model,</li>
  <li>the validated semantic layering,</li>
  <li>the execution-facing FIR,</li>
  <li>the lowering path toward backend families,</li>
  <li>the contract boundary between open program meaning and downstream execution,</li>
  <li>and the bridge surface through which runtime families and compiler families can be attached to one common language stack.</li>
</ul>

<p>
In practical terms, FROG seeks to combine:
</p>

<ul>
  <li>the accessibility historically associated with graphical dataflow programming,</li>
  <li>the execution seriousness required for industrial and system-grade deployment,</li>
  <li>the openness and inspectability needed for modern interoperability and portability,</li>
  <li>the auditability and structured artifacts needed for AI-assisted generation and transformation workflows,</li>
  <li>the accountability needed to assign responsibility, validate behavior, and control evolution in industrial contexts,</li>
  <li>the sovereignty-preserving properties needed for long-term industrial independence from opaque vendor-controlled formats,</li>
  <li>and the modularity needed to combine runtimes, compilers, hardware bridges, and deployment strategies without redefining the language each time.</li>
</ul>

<p>
The strategic thesis is therefore not merely “open graphical programming”.
It is that the next generation of graphical industrial programming infrastructure should be able to establish itself as the <strong>open-source standard foundation for graphical dataflow language infrastructure</strong>, while remaining:
</p>

<ul>
  <li>machine-friendly enough for AI-era tooling,</li>
  <li>human-reviewable enough for security and engineering trust,</li>
  <li>accountable enough for industrial validation, responsibility assignment, and controlled maintenance,</li>
  <li>open enough for multi-vendor implementation,</li>
  <li>structured enough to preserve meaning across validation, derivation, and backend handoff,</li>
  <li>modular enough to support multiple runtime and compiler combinations from the same source program,</li>
  <li>and governable enough that current repository posture remains explicit rather than hidden in one proprietary lifecycle.</li>
</ul>

<p>
FROG is therefore not trying to make graphical programming merely more pleasant.
It is trying to make executable graphical programming more open, more serious, more composable, more inspectable, more accountable, and more future-proof.
</p>

<hr/>

<h2 id="positioning-gap">3. Positioning Gap</h2>

<p>
FROG targets a missing zone in the current software landscape.
</p>

<p>
At one end of the spectrum, syntax-first languages can deliver depth of control, determinism, portability, and deployment seriousness, but they often impose substantial implementation burden on domain engineers, system integrators, and hardware-oriented teams.
At the other end of the spectrum, accessible graphical or low-code environments may reduce friction but often remain constrained by proprietary formats, opaque execution layers, limited backend openness, or hardware-vendor lock-in.
</p>

<p>
The gap is now even wider in the generative AI era.
Modern toolchains increasingly generate, rewrite, transform, explain, validate, and assist program structure.
That increases the value of representations that are both machine-friendly and human-reviewable.
It also increases the value of representations that make responsibility assignable and change control practical.
Many current environments satisfy one side of that requirement better than the other.
</p>

<p>
FROG targets the missing zone where the following properties are combined rather than traded against one another:
</p>

<ul>
  <li>visual dataflow programming rather than syntax-heavy implementation work,</li>
  <li>system-grade execution depth rather than shallow convenience scripting only,</li>
  <li>hardware agnosticism rather than ecosystem lock-in,</li>
  <li>compiler agnosticism at the architectural boundary rather than one downstream toolchain becoming hidden language truth,</li>
  <li>runtime agnosticism at the architectural boundary rather than one runtime becoming hidden language truth,</li>
  <li>open and inspectable program artifacts rather than opaque saved formats,</li>
  <li>accountable software evolution rather than fast production without explainable responsibility,</li>
  <li>a path from rapid prototyping to deployment rather than a permanent split between prototype tooling and production tooling,</li>
  <li>structured machine-compatible source artifacts together with direct human structural review,</li>
  <li>and an open execution-facing IR that can be bridged toward existing operational runtimes or compiler families rather than forcing one execution doctrine for all targets.</li>
</ul>

<p>
This is why FROG matters.
The opportunity is not to imitate historical graphical tools.
The opportunity is to open a missing systems capability category:
<strong>executable graphical programming with an open language layer, an inspectable bridge surface to heterogeneous execution ecosystems, and a responsibility-oriented review path for the post-AI software era</strong>.
</p>

<hr/>

<h2 id="heilmeier-h1">4. Heilmeier H1 — What are you trying to do?</h2>

<p>
FROG is trying to create an open, hardware-agnostic graphical dataflow language and associated architecture that can support a serious path from canonical saved source to execution-facing FIR, backend-specific lowering, backend contract generation, runtime-family bridging, compiler-family bridging, and eventually real deployable execution paths across heterogeneous hardware targets.
</p>

<p>
In simple terms, the project is trying to make the following possible:
</p>

<ul>
  <li>save graphical programs in an open and inspectable format,</li>
  <li>validate those programs against published semantic rules,</li>
  <li>derive a standardized execution-facing FROG IR,</li>
  <li>lower that IR toward multiple backend families,</li>
  <li>bridge that IR toward runtime families when execution should remain runtime-driven,</li>
  <li>bridge that IR toward compiler families when deployment should become artifact-driven,</li>
  <li>combine runtime services and compiled artifacts when a hybrid deployment is the right answer,</li>
  <li>track what was generated, validated, transformed, accepted, and handed downstream,</li>
  <li>run or compile those programs on heterogeneous targets without making one private vendor stack the definition of the language.</li>
</ul>

<p>
The long-term goal is not just a new editor.
It is an open language foundation that can eventually support:
</p>

<ul>
  <li>industrial automation,</li>
  <li>robotics,</li>
  <li>embedded and edge systems,</li>
  <li>AI-assisted development and transformation workflows,</li>
  <li>CPU, ARM, microcontroller, FPGA, GPU, and other backend-oriented execution paths,</li>
  <li>and deployment patterns where execution, observability, and front-panel interaction may be distributed across different machines or targets.</li>
</ul>

<p>
More specifically, FROG is trying to make AI-era program generation more governable.
It aims to provide a language stack where:
</p>

<ul>
  <li>the saved source remains structured and machine-friendly,</li>
  <li>the program structure remains graphically reviewable by humans,</li>
  <li>the execution-facing FIR remains open to inspection,</li>
  <li>downstream compilation remains separate from the normative language definition,</li>
  <li>downstream runtime realization remains separate from the normative language definition,</li>
  <li>responsibility can be attached to inspectable artifacts rather than only to opaque generated output,</li>
  <li>and the repository can state its current posture explicitly without forcing that truth into strategic prose.</li>
</ul>

<h3>Runtime and compiler complementarity</h3>

<p>
A key part of the FROG vision is that runtime and compiler are not opposing ideologies.
They are complementary downstream strategies that can both begin from the same open upstream truth.
</p>

<p>
A compiler path prepares a deployable artifact specialized for a target family.
A runtime path provides the live execution engine, state handling, observability services, communication surfaces, integration posture, and front-panel interaction needed to make that execution operational.
Different targets and industrial contexts require different mixes of those two roles.
</p>

<p>
FROG therefore aims to support a world where one canonical <code>.frog</code> source and one open execution-facing FIR can later be combined with:
</p>

<ul>
  <li>compiler-heavy deployments,</li>
  <li>runtime-heavy deployments,</li>
  <li>hybrid deployments where compiled execution and runtime services coexist,</li>
  <li>and bridge paths toward existing hardware-specific operational runtimes where that is the most credible industrial route.</li>
</ul>

<p>
That is one of the most important strategic consequences of an open FIR:
it does not merely preserve inspectability.
It preserves combinability.
</p>

<h3>Post-AI accountability and control</h3>

<p>
Generative AI changes the economics of software creation.
It can produce code, tests, documentation, and system logic at a speed that traditional human review processes were not designed to absorb.
This does not remove the need for responsibility.
It makes responsibility harder.
</p>

<p>
In industrial contexts, software cannot only be produced.
It must be understood, inspected, validated, attributed, maintained, controlled, and evolved.
A generated artifact that cannot be explained, traced, or governed is not enough for serious system-grade deployment.
</p>

<p>
FROG addresses this accountability gap by giving generated or human-authored logic a structured path through graphical source, semantic validation, open FIR, lowering, backend contracts, runtime-family consumption, compiler-family consumption, and observable execution.
</p>

<p>
The goal is not to reject generative AI.
The goal is to make AI-assisted system creation governable.
FROG provides an architectural interface through which humans can inspect, understand, control, assign responsibility for, and evolve software that may increasingly be produced with AI assistance.
</p>

<hr/>

<h2 id="heilmeier-h2">5. Heilmeier H2 — How is it done today, and what are the limits?</h2>

<p>
Today, the landscape is fragmented.
</p>

<ul>
  <li>Syntax-first languages provide strong execution depth but impose significant implementation friction for many domain and hardware-oriented teams.</li>
  <li>Graphical systems can provide accessibility but often remain proprietary, opaque, or tied to a specific vendor ecosystem.</li>
  <li>The saved program representation is often not designed as an open, durable, inspectable interoperability layer.</li>
  <li>The internal execution representation is often not designed to remain open, recoverable, or backend-neutral.</li>
  <li>The runtime and compiler story is often hidden inside one integrated platform rather than exposed as a modular architecture.</li>
  <li>The bridge from source to heterogeneous hardware is often controlled by one stack rather than expressed through open language-owned boundaries.</li>
</ul>

<p>
The practical limits of the current situation are structural rather than cosmetic:
</p>

<ul>
  <li>vendor lock-in,</li>
  <li>difficulty of porting graphical system logic across hardware ecosystems,</li>
  <li>difficulty of integrating modern AI tooling with opaque graphical formats,</li>
  <li>difficulty of building an open multi-vendor ecosystem around system-grade graphical execution,</li>
  <li>difficulty of cleanly separating source truth, execution-facing IR, backend lowering, runtime realization, and compiler specialization,</li>
  <li>difficulty of constructing combinable runtime + compiler deployment strategies from one open intermediate representation,</li>
  <li>difficulty of keeping version posture explicit rather than burying it in one vendor release logic.</li>
</ul>

<p>
There is also a reviewability problem.
Textual languages can certainly be audited, but structural understanding is often indirect:
reviewers reconstruct system architecture from syntax, coding conventions, build context, and analysis tooling.
In closed graphical systems, the problem is different but equally serious:
the structure may be visual, yet the saved format, the intermediate representation, and the downstream execution path remain opaque or vendor-bound.
</p>

<p>
There is also a modularity problem.
Many systems force a single execution philosophy.
They implicitly say:
one language means one runtime,
or one language means one compiler,
or one hardware vendor means one mandatory operational stack.
That limits deployment freedom and makes it hard to reuse serious existing runtime or compiler infrastructure already present in industry.
</p>

<h3>The post-AI inspection bottleneck</h3>

<p>
A new limitation is emerging as generative AI becomes common in software work:
production is accelerating faster than review capacity.
AI-assisted tools can generate large quantities of plausible-looking code, tests, documentation, and integration logic.
But understanding, verification, licensing confidence, safety analysis, operational validation, and long-term maintenance still require accountable human and organizational control.
</p>

<p>
The result is an inspection bottleneck.
The scarce resource is no longer only the ability to produce software.
The scarce resource becomes the ability to inspect, understand, govern, and trust what has been produced.
</p>

<p>
For industrial systems, this is not optional.
A system must have a responsible owner, a reviewable behavior, a controlled change path, an auditable execution chain, and a maintainable evolution path.
Generating more code faster is not enough if responsibility becomes harder to assign and behavior becomes harder to explain.
</p>

<p>
The issue is therefore not merely “better usability”.
The issue is that graphical industrial programming has historically lacked a sufficiently open and standardized architectural foundation,
and AI-era software generation is making the cost of opaque or weakly reviewable representations even higher.
</p>

<p>
FROG departs from that pattern.
It preserves the spirit of serious graphical system programming,
but it does not accept the historical collapse of language, file format, IDE, runtime, compiler, and hardware ecosystem into one inseparable product stack.
It also does not accept that AI-generated system logic should remain a mass of opaque text without a structured review and accountability path.
</p>

<hr/>

<h2 id="heilmeier-h3">6. Heilmeier H3 — What is new, and why do we think it will work?</h2>

<p>
What is new is not just the idea of drawing dataflow graphs.
What is new is the combination of the following architectural commitments inside one coherent open system:
</p>

<ul>
  <li>an open canonical <code>.frog</code> source representation,</li>
  <li>a canonical source model that is structured JSON rather than a private opaque file format,</li>
  <li>an explicit separation between saved source, validated semantics, derived FIR, lowering, backend contract, runtime realization, and compiler realization,</li>
  <li>a standardized execution-facing FROG IR that remains inspectable and recoverable,</li>
  <li>a backend-family-oriented lowering path rather than one hidden private execution pipeline,</li>
  <li>a future path toward known backend/compiler targets without making those targets the definition of the language,</li>
  <li>a runtime-bridge posture that allows execution services to remain modular rather than becoming hidden language truth,</li>
  <li>a responsibility-oriented representation stack for software increasingly authored, modified, or accelerated by AI-assisted tools,</li>
  <li>a centralized repository-visible versioning surface rather than scattered implicit version claims.</li>
</ul>

<p>
The project can work because the repository has already begun to demonstrate the architecture in small executable slices rather than relying only on abstract claims.
The current slices already show that the published boundaries are not merely conceptual:
</p>

<ul>
  <li>public input/output dataflow,</li>
  <li>natural widget-value participation,</li>
  <li>object-style UI interaction,</li>
  <li>explicit local memory and valid feedback through delay,</li>
  <li>and a bounded vertical slice where canonical source, UI packaging, FIR publication, lowering output, backend contract emission, shared acceptance, and runtime-family consumption are all visible.</li>
</ul>

<p>
The architectural argument is especially strong in the AI era:
FROG does not offer only a machine-friendly source representation.
It also offers a human-reviewable primary program structure and an open execution-facing FIR.
That combination is precisely what reduces the auditability gap between generated artifacts and deployed behavior.
</p>

<p>
That does not yet prove the full industrial compiler and deployment story.
But it does provide technical evidence that the architecture is capable of sustaining a real execution path without collapsing all layers into one implementation shortcut.
</p>

<h3>The open FIR as the bridge surface</h3>

<p>
One of the most strategically important ideas in FROG is that the open FIR is not merely an inspectable intermediate file.
It is the public architectural bridge surface that makes downstream modularity possible.
</p>

<p>
Because FIR remains open:
</p>

<ul>
  <li>bridges can be built from FIR toward compiler families,</li>
  <li>bridges can be built from FIR toward runtime families,</li>
  <li>bridges can be built toward existing operational stacks already used by hardware vendors,</li>
  <li>and the same upstream graphical program can support multiple deployment combinations without redefining the language itself.</li>
</ul>

<p>
This matters a great deal in practice.
Some hardware ecosystems expose their value primarily through runtime stacks.
Others expose their value primarily through compiler flows.
Others still rely on a hybrid model.
An open FIR allows FROG to remain upstream from those differences instead of being captured by one of them.
</p>

<p>
That means FROG can remain open even when the downstream operational bridge is proprietary.
A hardware vendor may expose a FROG bridge toward its own operational runtime or compiler pipeline without forcing that private operational layer to become the language definition.
This is strategically important for real industrial adoption, because it reduces the cost of integration with existing hardware ecosystems rather than demanding that every serious target be rebuilt from zero.
</p>

<h3>Reference runtime families as proof, not monopoly</h3>

<p>
Another important part of why we think this can work is that the repository does not treat runtime as a purely abstract future concept.
It already publishes a bounded reference-runtime family corridor and reference consumers in multiple implementation languages.
</p>

<p>
That matters strategically because it demonstrates three things at once:
</p>

<ul>
  <li>the language can be consumed by more than one implementation culture,</li>
  <li>runtime realization is not owned by one mandatory implementation language,</li>
  <li>and the repository can eventually publish reusable runtime recipes rather than one monolithic runtime ideology.</li>
</ul>

<p>
The long-term importance of this direction is larger than one example.
It opens the door to runtime families specialized for:
</p>

<ul>
  <li>pure operation,</li>
  <li>operation plus observability,</li>
  <li>operation plus IDE-facing development services,</li>
  <li>operation plus communication variables and protocol bridges,</li>
  <li>and distributed execution patterns where the operational target and the front panel are not co-located.</li>
</ul>

<p>
That is not yet the same as saying the whole space is closed.
It is the strategic claim that FROG is being shaped to make that space structurally possible.
</p>

<h3>FROG as a responsibility architecture</h3>

<p>
FROG can also work because it turns software creation into a chain of inspectable artifacts rather than a jump from intent to opaque implementation.
That matters when code is generated or transformed by AI-assisted tools.
</p>

<pre><code>human or AI-assisted intent
        |
        v
structured .frog source
        |
        v
graphical review
        |
        v
semantic validation
        |
        v
open FIR
        |
        v
lowering / backend contract
        |
        +-----------------------------+-----------------------------+
        |                             |
        v                             v
runtime family                  compiler family
        |                             |
        v                             v
observable execution            deployable artifact
</code></pre>

<p>
This chain does not remove the need for expert engineering.
It makes expert engineering more governable by giving reviewers and organizations explicit surfaces where meaning, transformation, responsibility, and downstream handoff can be inspected.
</p>

<hr/>

<h2 id="heilmeier-h4">7. Heilmeier H4 — Who cares if we succeed?</h2>

<p>
If FROG succeeds, multiple groups should care.
</p>

<ul>
  <li><strong>Industrial users and integrators</strong> gain an open graphical programming foundation that is not structurally locked to one hardware vendor or one execution stack.</li>
  <li><strong>Hardware vendors</strong> gain a path to expose serious industrial capabilities through open upstream language artifacts while still attaching their own downstream runtime or compiler bridges where appropriate.</li>
  <li><strong>System builders</strong> gain a cleaner way to target CPUs, embedded systems, edge devices, FPGA-oriented flows, GPU-oriented flows, and future hardware families through one architectural language stack.</li>
  <li><strong>AI and tooling ecosystems</strong> gain structured source and open IR artifacts rather than opaque graphical artifacts.</li>
  <li><strong>Engineering managers and accountable system owners</strong> gain a more structured way to understand what was generated, validated, changed, accepted, and deployed.</li>
  <li><strong>Security and assurance stakeholders</strong> gain a more direct way to inspect generated or transformed logic across source and execution-facing layers.</li>
  <li><strong>Runtime and compiler implementers</strong> gain a public bridge surface from which serious execution paths can be constructed without redefining the language.</li>
  <li><strong>The broader engineering community</strong> gains an alternative model for graphical programming that is open, portable, modular, and structurally modern.</li>
  <li><strong>States, sovereign industrial actors, and strategic technology programs</strong> gain a path away from dependence on opaque vendor-controlled graphical programming infrastructure.</li>
</ul>

<p>
The broader strategic consequence is large:
FROG could help transform graphical system programming from a closed vendor capability into an open ecosystem capability.
</p>

<p>
It could also support a healthier market structure.
Multiple independent IDEs, runtimes, compiler bridges, analysis tools, and hardware integrations could coexist around one open language basis.
That openness remains compatible with steward-controlled official branding and with a future Graiphic flagship IDE carrying official FROG identity.
Openness of the language does not require branding collapse.
</p>

<p>
In the generative AI era, another group should care deeply:
organizations that must remain responsible for systems they did not write line by line.
FROG can provide a structured interface through which fast generated work can be inspected, validated, accepted, and evolved under human and organizational responsibility.
</p>

<hr/>

<h2 id="heilmeier-h5">8. Heilmeier H5 — What difference will success make?</h2>

<p>
If successful, FROG would change more than one tool.
It would change the structure of the category.
</p>

<ul>
  <li>Graphical programs would no longer need to live inside opaque saved formats.</li>
  <li>The saved source could remain open, structured, machine-friendly, and durable.</li>
  <li>Execution-facing representations could become inspectable, standardizable, and portable.</li>
  <li>Graphical industrial programming could become hardware-agnostic by design rather than vendor-limited by default.</li>
  <li>Graphical industrial programming could become architecturally compiler-agnostic rather than hidden behind one mandatory downstream toolchain.</li>
  <li>Graphical industrial programming could become architecturally runtime-agnostic rather than hidden behind one mandatory operational stack.</li>
  <li>Multiple hardware manufacturers could participate in one open language ecosystem instead of forcing users into mutually isolated stacks.</li>
  <li>AI-assisted tooling could operate on open source and open IR artifacts rather than reverse-engineering closed graphical representations.</li>
  <li>Critical generated logic could become easier to review visually and structurally rather than only through indirect reconstruction.</li>
  <li>Runtime-family and compiler-family combinations could become a deployment choice rather than a language limitation.</li>
  <li>Operational execution could live on one target while front-panel interaction and observability could, where appropriate, live elsewhere through explicit communication layers rather than implicit vendor magic.</li>
  <li>Repository status and transition posture could remain publicly readable and governable rather than hidden in one product lifecycle.</li>
</ul>

<p>
The long-term difference would therefore be structural:
FROG could shift graphical system-grade programming from vendor-controlled lock-in toward open, multi-vendor, inspectable infrastructure.
</p>

<p>
That difference is not only technical.
It is also strategic.
In industrial and security-sensitive domains, the ability to inspect, attribute, govern, port, compile, and bridge logic matters for trust, resilience, and technological sovereignty.
</p>

<p>
If FROG succeeds fully, it could make possible something that has historically remained aspirational:
a serious graphical language stack that can travel from authoring to operation across many kinds of hardware and many kinds of downstream execution infrastructure without surrendering the language to one proprietary execution authority.
</p>

<h3>Impact on post-AI engineering responsibility</h3>

<p>
If FROG succeeds, it would also change how organizations manage AI-assisted software creation.
The question would no longer be only “can an AI produce code quickly?”
The question would become:
</p>

<ul>
  <li>what structure was produced,</li>
  <li>what semantic meaning was accepted,</li>
  <li>what execution-facing representation was derived,</li>
  <li>what backend contract was handed downstream,</li>
  <li>what runtime or compiler consumed it,</li>
  <li>what behavior was observed,</li>
  <li>who reviewed and accepted the change,</li>
  <li>and how the system can evolve without losing responsibility.</li>
</ul>

<p>
That is the accountability difference.
FROG can make fast software creation more compatible with responsible industrial control.
</p>

<hr/>

<h2 id="heilmeier-h6">9. Heilmeier H6 — What are the risks and what still needs to be proved?</h2>

<p>
Several important things still need to be proved.
</p>

<ul>
  <li>The standardized FROG execution IR must be stabilized more fully.</li>
  <li>The lowering model toward backend families must be specified more deeply.</li>
  <li>A future backend/compiler-oriented path must be demonstrated more concretely.</li>
  <li>Real deployable targets must be shown beyond the current bounded reference path.</li>
  <li>Backend diversity must be demonstrated without collapsing the architecture into one backend-specific truth.</li>
  <li>The relationship between open IR, backend contracts, runtime-family bridges, compiler-family bridges, and deployment artifacts must be shown end to end.</li>
  <li>Reference runtime recipes must grow beyond one bounded corridor strongly enough to prove that runtime plurality is architectural rather than rhetorical.</li>
  <li>The relationship between operational targets, observability services, communication variables, and remote or distributed front-panel patterns must be shown more concretely.</li>
  <li>The claimed auditability advantage must remain disciplined and credible rather than overstated.</li>
  <li>The claimed post-AI accountability advantage must be demonstrated through real review and acceptance workflows, not only stated as strategy.</li>
  <li>The distinction between strategy, roadmap sequencing, and current repository-status truth must remain explicit.</li>
</ul>

<p>
The key risk is not that the idea is meaningless.
The key risk is that the project could stop too early at the “interesting prototype” stage without closing the future standardized FIR, runtime-bridge, and compiler-bridge story strongly enough.
</p>

<p>
Another risk is strategic overclaim.
FROG should not claim that graphical representation automatically guarantees security or that textual languages cannot be reviewed.
Its real claim is narrower and stronger:
it can reduce structural opacity by combining machine-friendly source, direct graphical reviewability, open execution-facing representation, modular downstream bridge boundaries, and explicit repository-visible governance.
</p>

<p>
A further risk is architectural drift.
If FIR becomes too tied to one reference runtime, or too tied to one compiler corridor, the strategic value of the open bridge surface would weaken.
That must be actively prevented.
</p>

<p>
There is also a post-AI accountability risk.
FROG should not claim that AI-generated logic becomes trustworthy merely because it is represented graphically.
The credible claim is that FROG can give humans and organizations better surfaces for inspection, validation, attribution, and controlled evolution.
That advantage must be proved through examples, acceptance artifacts, validation flows, and real downstream consumers.
</p>

<hr/>

<h2 id="heilmeier-h7">10. Heilmeier H7 — How much will it cost?</h2>

<p>
At the current repository stage, the project should be understood as an architecture-building and proof-building effort rather than as a finalized program budget.
</p>

<p>
The cost profile should eventually separate into at least six layers:
</p>

<ul>
  <li>normative specification work,</li>
  <li>reference implementation and executable slice closure,</li>
  <li>future backend/compiler/runtime proof of concept work,</li>
  <li>bridge-family work toward heterogeneous hardware ecosystems,</li>
  <li>post-AI review, accountability, and acceptance workflow proof work,</li>
  <li>future IDE and ecosystem-enablement work.</li>
</ul>

<p>
A later roadmap phase should convert this strategic framing into explicit work packages, milestones, cost ranges, and resource assumptions.
The current repository-status posture and current detailed repository-surface status remain governed centrally in <code>Versioning/</code>, not in this strategic framing.
</p>

<p>
It is also likely that the cost of success will not be concentrated in one implementation only.
An open language foundation creates value precisely because multiple implementers, hardware partners, and downstream runtime or compiler actors may contribute to different parts of the ecosystem over time.
</p>

<hr/>

<h2 id="heilmeier-h8">11. Heilmeier H8 — How long will it take and what are the midterm and final exams?</h2>

<p>
The project should be judged by successive proof points rather than by one monolithic promise.
</p>

<p>
Natural milestone categories are:
</p>

<ul>
  <li><strong>Early exam</strong> — prove executable architectural slices from canonical source through runtime consumption.</li>
  <li><strong>Midterm exam</strong> — stabilize the execution-facing FROG IR and the backend-lowering story more fully.</li>
  <li><strong>Advanced exam</strong> — demonstrate credible FIR bridge paths toward both runtime families and compiler families.</li>
  <li><strong>Deeper advanced exam</strong> — demonstrate an end-to-end backend/compiler proof of concept, ideally including a known backend family such as LLVM for CPU-oriented execution.</li>
  <li><strong>Post-AI accountability exam</strong> — demonstrate that AI-assisted or human-authored graph logic can be generated, reviewed, validated, attributed, accepted, and evolved through explicit repository-visible artifacts.</li>
  <li><strong>Distributed systems exam</strong> — demonstrate a credible path where operational execution can live on a target while observability and front-panel interaction can remain external through explicit communication layers.</li>
  <li><strong>Long-term exam</strong> — demonstrate a credible path toward a serious IDE, a broader primitive base, stronger structures, richer memory semantics, and multi-target deployment.</li>
</ul>

<p>
The important point is that each stage should be auditable, explicit, and grounded in repository-visible artifacts rather than in vague aspiration.
</p>

<p>
In the AI-era framing, a particularly important exam is whether FROG can demonstrate the full review loop:
</p>

<ul>
  <li>structured source generation,</li>
  <li>human graphical review,</li>
  <li>semantic validation,</li>
  <li>inspectable FIR derivation,</li>
  <li>backend handoff without loss of architectural traceability,</li>
  <li>runtime or compiler consumption without hidden ownership transfer,</li>
  <li>explicit acceptance artifacts,</li>
  <li>responsibility assignment around generated or transformed logic,</li>
  <li>and, eventually, the ability to select between runtime-heavy, compiler-heavy, or hybrid deployment strategies without changing the upstream language truth.</li>
</ul>

<p>
The ordering of those proofs belongs to the roadmap layer.
The current published repository posture belongs to the versioning layer.
This document explains why those proof points matter strategically.
</p>

<hr/>

<h2 id="relation-with-strategy-roadmap-and-versioning">12. Relation with Strategy, Roadmap, and Versioning</h2>

<p>
The repository contains three distinct repository-wide framing and governance surfaces that answer three different questions:
</p>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Primary question</th>
      <th>What it must not replace</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Strategy/</code></td>
      <td>Why does FROG matter?</td>
      <td>Normative technical ownership, closure sequencing, or current repository-status truth</td>
    </tr>
    <tr>
      <td><code>Roadmap/</code></td>
      <td>In what order should FROG be closed?</td>
      <td>Normative technical ownership, strategic rationale, or current repository-status truth</td>
    </tr>
    <tr>
      <td><code>Versioning/</code></td>
      <td>What is the current published repository posture, what doctrine governs version evolution, and what is the current detailed per-surface status?</td>
      <td>Normative technical ownership, strategic rationale, or milestone sequencing</td>
    </tr>
  </tbody>
</table>

<p>
This means:
</p>

<ul>
  <li>Strategy explains the <strong>why</strong>,</li>
  <li>Roadmap explains the <strong>next</strong>,</li>
  <li>Versioning explains the <strong>current published repository posture</strong>.</li>
</ul>

<p>
This Heilmeier framing belongs to the strategic layer.
It may refer to the current baseline of the project, but it must not become the authoritative place where the repository declares its current detailed state.
When that information is needed, the authoritative repository-visible entry points are:
</p>

<ul>
  <li><code>Versioning/Readme.md</code>,</li>
  <li><code>Versioning/Matrix.md</code>.</li>
</ul>

<p>
This boundary matters especially in a project like FROG because strategy can explain why runtime/compiler modularity, FIR openness, and post-AI accountability matter, but it must not silently redefine the normative ownership of FIR, lowering, profiles, runtime responsibilities, or implementation behavior.
</p>

<hr/>

<h2 id="what-is-already-demonstrated-in-the-repository">13. What is Already Demonstrated in the Repository</h2>

<p>
The repository already demonstrates a minimal executable reference path through a sequence of small vertical slices and a bounded vertical corridor that now extends into a UI-bearing runtime-family example.
</p>

<ul>
  <li><strong>01_pure_addition</strong> — end-to-end public-input/public-output arithmetic path.</li>
  <li><strong>02_ui_value_roundtrip</strong> — natural widget-value path without object-style UI collapse.</li>
  <li><strong>03_ui_property_write</strong> — first explicit object-style UI interaction path.</li>
  <li><strong>04_stateful_feedback_delay</strong> — first explicit-memory and valid-feedback path.</li>
  <li><strong>05_bounded_ui_accumulator</strong> — frozen bounded reference slice joining canonical source, front-panel object publication, widget packaging, FIR publication, lowering publication, backend contract emission, shared runtime-family acceptance, runtime-family consumption, and first LLVM proof behavior.</li>
</ul>

<p>
These slices matter because they show that the architecture already supports real execution exercises without requiring the final full industrial compiler pipeline to exist yet.
</p>

<p>
They should be understood as proof of architectural viability, not as proof that all long-term execution targets are already closed.
</p>

<p>
The repository also already publishes a bounded reference runtime-family workspace with multiple implementation-language consumers.
That is strategically meaningful because it demonstrates that FROG is not tied to one mandatory runtime implementation language and that the runtime-family idea is already being exercised publicly.
</p>

<p>
This remains a bounded proof corridor.
It does not yet mean that all future runtime recipes are complete, nor that all compiler bridge families are already demonstrated.
But it does mean that the architecture has moved beyond theory.
</p>

<p>
These published slices also matter strategically because they keep the project grounded in inspectable repository-visible artifacts.
The strategic argument for openness, auditability, modular downstream seriousness, future FIR-bridge value, and post-AI accountability is stronger when each stage is already exercised in public.
The authoritative current repository-wide status of those published surfaces remains centralized in <code>Versioning/</code>.
</p>

<hr/>

<h2 id="future-end-to-end-poc-direction">14. Future End-to-End POC Direction</h2>

<p>
A future end-to-end proof of concept should demonstrate the full chain more explicitly and more convincingly.
</p>

<p>
The ideal future demonstration path is no longer only a straight line.
Strategically, it should prove that one canonical source and one open FIR can feed multiple legitimate downstream combinations.
</p>

<pre><code>.frog canonical source
    |
    v
validation
    |
    v
standardized FROG execution IR / FIR
    |
    +--------------------------------------------------+
    |                                                  |
    v                                                  v
runtime bridge family                            compiler bridge family
    |                                                  |
    |                                                  |
    |      pure operation                              |      target specialization
    |      operation + observability                   |      native artifact generation
    |      operation + communication                   |      deployment-oriented lowering
    |      operation + distributed UI                  |      backend-specific compilation
    |                                                  |
    v                                                  v
reference runtimes /                               LLVM / native CPU paths /
existing operational stacks                        vendor compiler families /
hardware runtime families                          future backend families
    |                                                  |
    +--------------------------+-----------------------+
                               |
                               v
                 heterogeneous deployment targets
         CPU / ARM / edge / MCU / FPGA / GPU / vendor-specific systems
</code></pre>

<p>
For CPU-oriented execution, an LLVM-oriented demonstration is especially meaningful.
It shows that FROG is not merely a saved graphical representation or a toy runtime, but a language architecture capable of targeting serious compilation paths while preserving its own open upstream boundaries.
</p>

<p>
For hardware-oriented targets, the same logic should later extend toward backend families appropriate for embedded systems, microcontrollers, FPGA-oriented flows, GPUs, and hardware ecosystems that already expose their own operational runtime layers.
</p>

<p>
A particularly important proof point would be to demonstrate that FIR can bridge not only toward open reference runtimes and open compiler corridors, but also toward existing operational hardware stacks where that is the pragmatic industrial integration route.
That would prove that FROG can become an upstream standard even when the downstream operational layer is vendor-specific.
</p>

<p>
Another meaningful proof point would be a distributed execution demonstration in which:
</p>

<ul>
  <li>a valid canonical <code>.frog</code> source artifact is produced,</li>
  <li>its structure is reviewed graphically by a human,</li>
  <li>its validated meaning is derived into inspectable FROG IR,</li>
  <li>that IR is consumed by an operational target runtime or target-specific compiler path,</li>
  <li>the operational logic runs on a remote target,</li>
  <li>observability and front-panel interaction remain available through explicit communication surfaces,</li>
  <li>the end-to-end chain remains attributable and auditable.</li>
</ul>

<h3>Future post-AI accountability demonstration</h3>

<p>
A future proof should also demonstrate the accountability loop explicitly.
A realistic demonstration could show an AI-assisted tool producing or modifying a FROG graph, then forcing that output through human graphical review, validation, FIR derivation, lowering, backend contract emission, shared acceptance, and runtime or compiler consumption.
</p>

<p>
The goal of such a proof would not be to show that AI-generated systems are automatically correct.
The goal would be to show that AI-assisted creation can be controlled through structured artifacts, explicit validation, human review surfaces, and traceable downstream handoff.
</p>

<p>
That is the strategic answer to the post-AI inspection problem:
not rejecting generation, but making generated system logic inspectable, attributable, controlled, and evolvable.
</p>

<p>
The strategic importance of those proofs belongs here.
The ordering of the work belongs to <code>Roadmap/</code>.
The current repository-status truth and detailed surface status belong to <code>Versioning/</code>.
</p>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
FROG is trying to open a missing category:
accessible graphical programming with a serious path to system-grade execution, hardware agnosticism, compiler agnosticism at the architectural boundary, runtime agnosticism at the architectural boundary, inspectable intermediate representation, post-AI accountability, and future deployable backend/runtime integration.
</p>

<p>
The repository already demonstrates meaningful parts of this architecture through executable reference slices and a frozen bounded runtime-family corridor, but it does not yet claim that the final industrial runtime/compiler story is complete.
</p>

<p>
The strategic case for FROG is therefore clear:
</p>

<ul>
  <li>open the saved source format,</li>
  <li>keep that source structured and machine-friendly,</li>
  <li>make program structure graphically reviewable by humans,</li>
  <li>open and standardize the execution-facing dataflow representation,</li>
  <li>treat the open FIR as the public bridge surface for downstream modularity,</li>
  <li>separate language truth from runtime-private realization,</li>
  <li>separate language truth from compiler-private realization,</li>
  <li>prepare future paths toward runtime-family and compiler-family combinations from one source program,</li>
  <li>enable an open, multi-vendor, industrial graphical programming ecosystem,</li>
  <li>reduce the auditability gap introduced by increasingly AI-assisted software generation,</li>
  <li>support assignable responsibility through structured, inspectable, validated, and traceable artifacts,</li>
  <li>support industrial security and technological sovereignty through open, inspectable, and governable program artifacts.</li>
</ul>

<p>
That is the long-term significance of the project.
This document explains <strong>why</strong> that significance matters.
It does not replace the roadmap’s sequencing role, and it does not replace the versioning layer’s role as the centralized source for current published repository-status truth.
</p>
