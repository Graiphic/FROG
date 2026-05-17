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
  <a href="#public-specification-and-implementation-boundary">Public boundary</a> •
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

<h2 id="public-specification-and-implementation-boundary">Public Specification and Implementation Boundary</h2>

<p>
This repository defines the public FROG specification, including the canonical source model, FIR, public contracts, conformance material, examples, widget-facing material, and bounded non-normative reference implementation material.
</p>

<p>
The public reference runtime exists to keep the published specification executable, inspectable, and testable.
It is intentionally bounded, non-production, and conformance-oriented.
The current public reference runtime closure is Examples <code>01</code> through <code>15</code>.
</p>

<p>
Graiphic may develop proprietary production implementations of FROG, including FROG Studio, production runtimes, deployment systems, enterprise integrations, certification tooling, and commercial support services.
Those implementations are products built on top of the public specification.
They do not redefine the FROG language specification and they do not make the public reference runtime mandatory.
Runtime development for examples beyond Example <code>15</code> continues in Graiphic's proprietary <code>Graiphic/FROG-Runtime</code> repository unless a later public reference surface is explicitly promoted.
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
The published numbered example surface now exposes a repository-visible progression under <code>Examples/01_*</code> through <code>Examples/15_*</code>.
Examples <code>01</code> through <code>15</code> define the current public reference runtime closure.
Examples beyond this boundary may remain repository-visible as specification-facing, widget-facing, conformance-facing, or design-progression examples, while Graiphic production runtime work for those later examples continues in the proprietary <code>Graiphic/FROG-Runtime</code> repository unless explicitly promoted later.
</p>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Short purpose</th>
      <th>Front-panel / widget surface</th>
      <th>Current validation posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>01_pure_addition</code></td>
      <td>Small pure arithmetic proof using public inputs and a public output.</td>
      <td>None; pure dataflow slice.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract &rarr; runtime &rarr; LLVM module.</td>
    </tr>
    <tr>
      <td><code>02_ui_value_roundtrip</code></td>
      <td>First natural widget-value roundtrip from a UI value to a result.</td>
      <td>Inline front-panel value participation.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract &rarr; runtime &rarr; LLVM module.</td>
    </tr>
    <tr>
      <td><code>03_ui_property_write</code></td>
      <td>Object-style widget reference plus <code>frog.ui.property_write</code>.</td>
      <td>Inline front-panel property-write participation.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract &rarr; runtime &rarr; LLVM module.</td>
    </tr>
    <tr>
      <td><code>04_stateful_feedback_delay</code></td>
      <td>Explicit state and valid feedback through <code>frog.core.delay</code>.</td>
      <td>None; stateful dataflow slice.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract &rarr; runtime &rarr; LLVM module.</td>
    </tr>
    <tr>
      <td><code>05_bounded_ui_accumulator</code></td>
      <td>Primary bounded LabVIEW-like accumulator corridor with state, UI binding, and native proof.</td>
      <td>Numeric control and indicator through the Default Numeric realization.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract &rarr; LLVM kernel manifest &rarr; C++/Python/Rust runtime surfaces.</td>
    </tr>
    <tr>
      <td><code>06_boolean_value_roundtrip</code></td>
      <td>Boolean control-to-indicator roundtrip.</td>
      <td>Boolean control and indicator through the Default Boolean realization.</td>
      <td>Widget runtime slice with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>07_string_value_roundtrip</code></td>
      <td>Hello-world String control-to-indicator roundtrip.</td>
      <td>String control and indicator through the Default String realization.</td>
      <td>Widget runtime slice with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>08_enum_value_roundtrip</code></td>
      <td>Enum mode selection roundtrip.</td>
      <td>Enum control and indicator through the Default Enum realization.</td>
      <td>Widget runtime slice with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>09_path_value_roundtrip</code></td>
      <td>Path control-to-indicator roundtrip, including browse-button UI behavior.</td>
      <td>Path control and indicator through the Default Path realization.</td>
      <td>Widget runtime slice with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>10_button_press_to_boolean</code></td>
      <td>Button <code>switch_until_released</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>11_button_switch_when_pressed</code></td>
      <td>Button <code>switch_when_pressed</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>12_button_switch_when_released</code></td>
      <td>Button <code>switch_when_released</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>13_button_latch_when_pressed</code></td>
      <td>Button <code>latch_when_pressed</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>14_button_latch_when_released</code></td>
      <td>Button <code>latch_when_released</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>15_button_latch_until_released</code></td>
      <td>Button <code>latch_until_released</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
  </tbody>
</table>

<p>
Widget examples are framed by the repository-visible
<a href="./Examples/widget_public_boundary.md">Widget Example Public Boundary</a>.
That public boundary keeps the <code>.frog</code> / <code>.wfrog</code> /
Default SVG / runtime-consumer split inspectable without publishing Graiphic's
internal widget acceptance recipe, visual QA procedure, or private runtime
roadmap.
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

06_boolean_value_roundtrip
  -> Boolean widget value roundtrip

07_string_value_roundtrip
  -> String widget value roundtrip

08_enum_value_roundtrip
  -> Enum widget value roundtrip

09_path_value_roundtrip
  -> Path widget value roundtrip

10_button_press_to_boolean
  -> Button switch_until_released state to Boolean indicator

11_button_switch_when_pressed
  -> Button switch_when_pressed state to Boolean indicator

12_button_switch_when_released
  -> Button switch_when_released state to Boolean indicator

13_button_latch_when_pressed
  -> Button latch_when_pressed state to Boolean indicator

14_button_latch_when_released
  -> Button latch_when_released state to Boolean indicator

15_button_latch_until_released
  -> Button latch_until_released state to Boolean indicator
</code></pre>

<p>
<code>Examples/05_bounded_ui_accumulator/</code> remains the primary applicative vertical-slice anchor because it combines source, front-panel package, FIR, lowering, backend contract, runtime-family acceptance, Python/Rust/C/C++ reference-consumer posture, and an LLVM-oriented native proof path.
Examples <code>01</code> through <code>04</code> are smaller executable proof slices used to keep individual concerns inspectable before they are combined in Example <code>05</code>.
Examples <code>06</code> through <code>15</code> are current widget-focused public reference runtime slices used to keep Boolean, String, Enum, Path, and Button front-panel behavior honest across C++, Python, and Rust.
Runtime development for examples beyond Example <code>15</code> continues in Graiphic's proprietary runtime repository unless explicitly promoted later as public reference material.
</p>

<p>
Example <code>16_picture_logo_jpeg</code> is the first post-boundary Picture
progression example. It remains public as source, library, widget, and
realization material, and models <code>Path -&gt; frog.image.decode_file_rgba8
-&gt; Picture</code> without adding public runtime implementation beyond the
Example <code>15</code> closure.
</p>

<p>
The correct current statement is therefore:
the repository materially exposes an executable Examples <code>01</code> through <code>15</code> progression, while Example <code>05</code> remains the richest applicative UI/state/runtime/native reference corridor and Examples <code>06</code> through <code>15</code> define the current bounded public widget-runtime wave.
This does not claim full generalized symmetry across all future examples, all runtime families, or rendered-native front-panel closure.
</p>

<p>
The current reference checks are intentionally repository-visible:
</p>

<pre><code>python -m pip install pytest
python Implementations/Reference/check_reference_workspace.py --include-pytest

cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp --output-on-failure

cargo test --manifest-path Implementations/Reference/Runtime/rust/Cargo.toml
</code></pre>

<p>
Optional native LLVM checks require <code>clang</code>:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-llvm-build
python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge
</code></pre>

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
For example, an image-decoding function may be supplied by a declared native library, static library, or other backend-consumable artifact.
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
Examples <code>06</code> through <code>15</code> currently exercise the public widget-front-panel runtime discipline for Boolean, String, Enum, Path, and Button across the C++, Python, and Rust reference runtimes.
Runtime development for examples beyond Example <code>15</code> belongs to Graiphic's proprietary runtime work unless explicitly promoted later as public reference material.
</p>

<p>
The current repository-visible execution checks are:
</p>

<ul>
  <li><code>ArtifactChecks/</code> — verifies that the executable corridor artifacts are present and coherent at a high level,</li>
  <li><code>Deriver/</code> — checks <code>.frog -> FIR</code> for the published source-pattern families, including the current widget examples where supported,</li>
  <li><code>Lowerer/</code> — checks <code>FIR -> lowering</code> for the published FIR-unit families, including the current widget examples where supported,</li>
  <li><code>ContractEmitter/</code> — checks <code>lowering -> backend contract</code>,</li>
  <li><code>Runtime/</code> — checks <code>contract -> runtime acceptance snapshot</code>,</li>
  <li><code>LLVM/</code> — checks <code>lowering -> LLVM module</code> and, where requested, native build proof.</li>
</ul>

<p>
The reference implementation workspace remains stage-separated:
Deriver, Lowerer, ContractEmitter, Runtime, and LLVM are downstream consumers of the published source/FIR/lowering corridor rather than semantic owners of the language.
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
│       -> numbered executable example progression and closure state
│
├── Conformance/
│   └── Readme.md
│       -> public conformance posture, staged expected outcomes,
│          preservation obligations, and rejection expectations
│
├── Implementations/
│   └── Reference/
│       ├── Readme.md
│       │   -> non-normative reference workspace overview
│       ├── checks.md
│       │   -> repository-visible check commands
│       ├── ArtifactChecks/
│       │   -> preflight checks for published reference artifacts
│       ├── Deriver/
│       │   -> supported .frog -> FIR reference derivation
│       ├── Lowerer/
│       │   -> supported FIR -> lowering reference lowering
│       ├── ContractEmitter/
│       │   -> lowering -> backend contract materialization
│       ├── Runtime/
│       │   -> backend contract -> runtime acceptance checks
│       ├── LLVM/
│       │   -> lowering -> LLVM-oriented native proof modules
│       └── Pipeline/
│           -> coordinated reference check pipeline
│
├── Expression/
│   -> canonical source representation
├── Language/
│   -> normative execution semantics
├── IR/
│   -> FIR, derivation rules, lowering, backend contract posture
├── Libraries/
│   -> intrinsic primitives, widget class law, and Default realizations
├── Profiles/
│   -> optional standardized capability-family specifications
├── IDE/
│   -> authoring, observability, debugging, and inspection
└── Versioning/
    -> centralized status and governance matrix
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
Readers who want to understand the currently published executable reference path should then continue with:
</p>

<pre>
Examples/Readme.md
   |
   +-- Examples/01_pure_addition/Readme.md
   +-- Examples/02_ui_value_roundtrip/Readme.md
   +-- Examples/03_ui_property_write/Readme.md
   +-- Examples/04_stateful_feedback_delay/Readme.md
   \-- Examples/05_bounded_ui_accumulator/Readme.md
        |
        v
Implementations/Reference/Readme.md
   |
   v
Implementations/Reference/checks.md
   |
   v
Implementations/Reference/check_reference_workspace.py
</pre>

<p>
That second path answers a staged set of questions:
</p>

<ul>
  <li><strong><code>Examples/</code></strong> — which executable slices are being used,</li>
  <li><strong><code>Examples/01_*</code> through <code>Examples/04_*</code></strong> — which isolated concerns are covered before the full corridor,</li>
  <li><strong><code>Examples/05_bounded_ui_accumulator/</code></strong> — which combined applicative corridor is currently the primary anchor,</li>
  <li><strong><code>Examples/06_*</code> through <code>Examples/15_*</code></strong> — which widget-front-panel runtime slices are currently covered,</li>
  <li><strong><code>Implementations/Reference/</code></strong> — how the non-normative reference pipeline processes them,</li>
  <li><strong><code>Runtime/</code></strong> — how backend contracts are checked through acceptance snapshots,</li>
  <li><strong><code>LLVM/</code></strong> — how lowered units are checked through native proof modules.</li>
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
At the current published state, the repository has reached a stronger closure milestone:
Examples <code>01</code> through <code>15</code> materially expose repository-visible source, FIR, lowering, backend-contract, widget, and LLVM-oriented proof material.
Examples <code>06</code> through <code>15</code> extend the current public reference workspace with validated widget-front-panel runtime slices for Boolean, String, Enum, Path, and Button.
</p>

<p>
The Example <code>05_bounded_ui_accumulator</code> slice remains the primary applicative vertical-slice anchor because it combines front-panel package participation, widget values, widget references, UI property writes, bounded iteration, explicit state, public output, runtime-family acceptance, and LLVM-native proof posture.
Examples <code>01</code> through <code>04</code> provide smaller executable anchors for isolated concerns.
Examples beyond Example <code>15</code> may remain visible as specification-facing, widget-facing, conformance-facing, or design-progression material.
They do not, by presence alone, expand the public runtime implementation maintained in this repository.
</p>

<p>
The first such post-boundary example is <code>16_picture_logo_jpeg</code>, which
uses a Path control, the standard <code>frog.image.decode_file_rgba8</code>
primitive contract, and a Picture indicator. Runtime support for this example
continues in <code>Graiphic/FROG-Runtime</code> unless deliberately promoted later.
</p>

<p>
At the same time, the repository has not yet reached:
</p>

<ul>
  <li>full generalized multi-runtime symmetry across all future serious examples,</li>
  <li>a generic contract executor that removes all example-specific runtime acceptance logic,</li>
  <li>a generic LLVM backend driven by lowered-unit kind rather than bounded example patterns,</li>
  <li>full native non-browser rendered front-panel closure,</li>
  <li>or final depth across all observability, debugging, and IDE-facing surfaces.</li>
</ul>

<p>
The current direction is therefore:
</p>

<ul>
  <li><strong>keep the Examples 01-15 executable and widget-runtime surfaces green as the bounded public reference runtime snapshot,</strong></li>
  <li><strong>keep the public reference runtime bounded, non-production, and conformance-oriented,</strong></li>
  <li><strong>continue runtime development for later examples in Graiphic's proprietary <code>Graiphic/FROG-Runtime</code> repository unless explicitly promoted,</strong></li>
  <li><strong>and allow Graiphic production runtime and IDE development to continue privately without redefining the public specification.</strong></li>
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
    <img src="./assets/cla-assistant-badge.svg" alt="CLA Assistant" />
  </a>
</p>

<hr/>

<p align="center">
  <strong>FROG — Free Open Graphical Language</strong><br/>
  Open graphical dataflow programming, specified as a language rather than owned as a product.
</p>

<hr/>
