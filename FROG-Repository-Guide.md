<h1>FROG Repository Guide</h1>

<p>
Public repository orientation: what this repository defines, what it deliberately does not define, how the public specification boundary works, and where readers should go next.
</p>

<p>
This document preserves long-form material that was previously maintained in
the public root README. The root README now acts as a concise orientation page
and links here for detail.
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
Examples <code>16</code> through <code>24</code> remain public post-boundary widget-facing material for Picture, Label, Decoration, Subpanel, Tab, Ring, Listbox, Table, and Tree while their runtime implementation line continues privately in <code>Graiphic/FROG-Runtime</code>.
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
