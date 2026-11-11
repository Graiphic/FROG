# FROG and Graiphic Studio — Heilmeier framing

**General project framing · reviewed 11 November 2026 · Non-normative**

FROG provides an open graphical dataflow language foundation. Graiphic Studio is Graiphic's proprietary environment for building programs, understanding their behavior and maintaining them through execution and change. This page describes the overall project, using the eight questions of [the Heilmeier Catechism](https://www.darpa.mil/about/heilmeier-catechism).

The [project philosophy](../Visual-Programming-and-Human-Control.md) explains the role of visual representation, execution and human control. Specialized research proposals remain separate from this general framing. Older section anchors are retained for incoming links.

- [Objective](#question-1)
- [Current practice](#question-2)
- [New approach and basis for confidence](#question-3)
- [Beneficiaries and impact](#question-4)
- [Risks](#question-5)
- [Cost](#question-6)
- [Duration](#question-7)
- [Intermediate and final exams](#question-8)

<a id="why-this-page-exists"></a>
<a id="strategic-thesis"></a>
<a id="positioning-gap"></a>
## The project and the problem

People need to build software, understand unfamiliar behavior and make changes that work. The program may be their own, inherited from a colleague or proposed by an AI tool. As software grows, connecting an overview to relevant detail and actual execution becomes important engineering work.

Graphical programming offers a spatial way to express dependencies and organize behavior. The opportunity is to make that representation useful throughout development and maintenance, supported by an open language foundation and an integrated environment. Existing graphical environments, textual tools and AI assistants provide substantial capabilities and meaningful comparison baselines.

<a id="heilmeier-h1"></a>
<a id="question-1"></a>
## 1. What are we trying to do?

Give engineers a practical way to build programs whose organization and behavior they can understand and change. They should be able to connect an interface to its logic, follow what happens during execution, locate an unexpected result and check a correction in the version that runs.

FROG supplies the open language and public representation contracts. Studio develops the corresponding programming experience, from visual authoring through supported execution and deployment. Human control matters throughout that workflow, whether the first implementation is drawn by a person or proposed by a tool.

<a id="heilmeier-h2"></a>
<a id="question-2"></a>
## 2. How is it done today, and what are the limits?

Engineers use textual languages, graphical programming, model-based tools, debuggers, tests and increasingly AI assistance. Many environments already integrate parts of the workflow. Text code can be analyzed and visualized; graphical tools can expose programmatic interfaces. Neither medium automatically establishes correctness or makes useful automation impossible.

Practical difficulty arises when people must reconstruct relationships, move between representations, find the consequential state or effect, and determine whether a change reached the actual executable. Graphical abstraction can help, but poorly organized diagrams can also hide detail. The relevant comparison is the complete engineering task with a defined tool version and application.

<a id="heilmeier-h3"></a>
<a id="question-3"></a>
## 3. What is new, and why might it work?

The proposed contribution combines an open graphical dataflow language with an integrated environment for authoring, navigation, execution observation and correction. Connections expose dependencies; structures and hierarchy organize behavior; values and activity can be examined in the context of the program. The engineer can work on the representation used to express the application.

Public source, semantic and FIR contracts separate the language from a particular editor, compiler or target. They provide a foundation for independent tooling and implementations. Compatibility and supported execution profiles must remain explicit as integrations develop.

Graphs, JSON and code generation are established techniques. Differentiation depends on the quality of the integrated experience, faithful mappings and the practical benefit in programming and maintenance. The calculator walkthrough illustrates the intended continuity from an overview to a case, execution and a correction. It is a concrete demonstration format, not a measured productivity study.

There is an existing foundation: the public common C++ Frontend library/CLI, finite executable dossiers and selected Studio normal Run, Highlight and LLVM/native paths on Windows/x86-64. The [9 November checkpoint](../../docs/frontend-checkpoint-2026-11-09.json) identifies its own evidence. Broader function coverage, general source imports and additional targets require further work.

<a id="heilmeier-h4"></a>
<a id="heilmeier-h5"></a>
<a id="question-4"></a>
## 4. Who cares, and what difference would success make?

Engineers building instrumentation, acquisition, test and automation applications provide a concrete initial field. Maintainers, reviewers and teams transferring work between people also need to understand existing behavior and make reliable changes. The language architecture is general purpose.

Success would make relevant relationships easier to find, reduce the effort of moving from diagnosis to a working change and support continued maintenance as applications evolve. AI-assisted development is an important use case alongside everyday human programming. The benefit must be evaluated on actual tasks, including errors and integration effort.

The open FROG foundation supports an ecosystem of compatible tools. Studio's proprietary value lies in a useful, reliable and extensible engineering experience. Industrial adoption depends on application coverage, performance, usability and maintained integrations as well as the clarity of the representation.

<a id="heilmeier-h6"></a>
<a id="question-5"></a>
## 5. What are the risks?

- Large graphs or unsuitable abstractions can overwhelm the engineer or hide relevant state and effects.
- Source, editor views, execution and target mappings can diverge unless their contracts and compatibility are maintained.
- Missing function coverage, performance limits or weak deployment support can prevent practical use despite an attractive overview.
- A visually convincing presentation can encourage conclusions that the available tests or observations do not establish.
- Integration and maintenance costs can outweigh the benefit, while partner-specific requests can distract from common product needs.

Use explicit contracts, readable hierarchy, attributable diagnostics, suitable regression checks and complete application examples. Assess usefulness and maintenance cost before promoting an extension into the common product. Specialized guarantees require their own evidence; they are not a condition imposed on every Studio workflow.

<a id="heilmeier-h7"></a>
<a id="question-6"></a>
## 6. How much will it cost?

A costed overall programme is not established by this page. Estimates should cover the real product work: language/profile stabilization, function and widget coverage, editor usability, execution and Build, diagnostics, performance, documentation, integrations and maintenance.

Estimate each bounded development stage against its existing owners and reusable components. Include qualification and adoption effort. A dedicated research demonstrator or customer-specific bridge needs a separate scope, owner and cost estimate; its budget should not silently become the cost model for FROG and Studio as a whole.

<a id="heilmeier-h8"></a>
<a id="question-7"></a>
## 7. How long will it take?

No new delivery calendar is committed here. Continue stabilizing source/profile documentation and the existing execution path. After stabilization, language-import work and missing-function/performance work can progress within the maintained roadmap. Additional compiler or hardware bridges need their own readiness decisions.

Duration depends on the chosen application coverage, staffing, integration complexity and qualification scope. The [roadmap](../../Roadmap/Readme.md) owns sequencing. A prospective partner discussion does not automatically reorder it or establish a date for a new capability.

<a id="question-8"></a>
## 8. What are the intermediate and final exams?

| Area | Evidence to seek |
| --- | --- |
| Language and compatible tools | Identified source/profile contracts, reproducible validation and faithful preservation of supported types, structures and behavior. |
| Complete application workflow | A representative application authored or modified in Studio, executed with known inputs and checked outputs, then saved/reopened and built within its supported profile. |
| Understanding and diagnosis | An engineer can move from an overview to the relevant detail, explain behavior and find a fault using the available observations. |
| A valid change | The intended correction reaches the tested executable while unrelated behavior and supported data remain intact. |
| Usability and performance | Representative editing, observation and execution tasks stay usable at the declared application scale; costs and limits are recorded. |
| Integration and adoption | Supported tools or targets work under explicit contracts, with reproducible setup and an understood maintenance burden. |

The calculator is a useful initial walkthrough because its interface and expected results are familiar while it exercises events, a queue and explicit state. Extend examples according to actual user needs. For comparative benefit claims, give workflows equivalent tasks and assess correct understanding, valid changes and effort. Source-line counts or the size of an overview alone do not establish superiority.

Acceptance criteria should match the stage being evaluated. Language validity, target support, execution results and human effectiveness answer different questions. A successful bounded example does not qualify every palette function or deployment target.

<a id="what-is-already-demonstrated-in-the-repository"></a>
## Existing foundation and remaining work

The common Frontend, published contracts and finite reference dossiers are existing foundations. Selected Studio paths already connect normal execution, Highlight and native LLVM compilation. [Project status](../../FROG-Project-Status.md), versioned checkpoints and individual dossiers retain their own dates and scope.

Source/profile stabilization, function coverage and performance continue. General imports from other languages and broader deployment support remain development directions. The general project is evaluated on its useful programming workflow and supported applications.

<a id="future-end-to-end-poc-direction"></a>
## Separate collaboration options

Specific partners may justify a dedicated experiment, verification adapter or constrained-target integration. Assess its wider value, architecture fit, resources and maintenance before deciding whether any part belongs in the core. [DARPA-oriented verification options](../Research/DARPA-Collaboration-Options.md), including a possible bounded-model demonstrator, are kept separately. They do not define the general project or its required next milestone.

<a id="relation-with-strategy-roadmap-and-versioning"></a>
## Ownership and status

Strategy explains purpose and hypotheses. [Roadmap](../../Roadmap/Readme.md) owns sequencing; [Versioning](../../Versioning/Readme.md) and attributed dossiers own their published implementation status. Existing specification, governance, licenses and public/private boundaries remain in force.

<a id="summary"></a>
## Summary

FROG and Studio pursue useful graphical programming: build software, understand its behavior and maintain control as it executes and evolves. Visual structure, an open foundation and an integrated working environment support that purpose. Concrete applications and appropriate evidence must establish the benefit.
