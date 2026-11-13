# FROG and Graiphic Studio — Heilmeier framing

**Human oversight of code generation · general framing reviewed 11 November 2026 · capability references updated 13 November 2026 · Non-normative**

The project aims to increase human capacity to follow, understand and validate machine-generated software. FROG provides a structured program model serialized in `.frog`; Studio projects it into graphical dataflow for human comprehension and intervention. FIR and downstream IR connect the program to supported compilation and execution.

This page applies the eight questions of [the Heilmeier Catechism](https://www.darpa.mil/about/heilmeier-catechism) to the general project. The [philosophy](../Visual-Programming-and-Human-Control.md) explains the architecture and limits. Partner-specific research remains separate. Existing section anchors are retained.

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

The strategic premise is that machine generation becomes routine in software production. Human attention and review effort do not automatically grow with that production. In processes requiring human acceptance, an engineer must still understand consequential behavior, judge a change and identify what is being accepted.

Graphical abstraction offers an additional way to present relationships: color, shape, position, text, arrangement, labels, connections and hierarchy. FROG and Studio seek to use that dimension to make machine-produced software accessible to human investigation, while retaining a path to efficient and deployable execution.

The same foundation also supports direct human programming and maintenance. It remains a general-purpose language and product, with a focused strategic motivation rather than a restriction on who authors a program.

<a id="heilmeier-h1"></a>
<a id="question-1"></a>
## 1. What are we trying to do?

Help people keep up with generated software well enough to understand relevant behavior and make informed acceptance decisions. An engineer should be able to inspect the program, follow what a proposed change does, find a problem and check a correction in the version that actually runs.

The goal is to increase useful human oversight through a graphical abstraction, with access to necessary detail. It does not assume that a person can read every generated operation or that an attractive overview alone validates a program.

<a id="heilmeier-h2"></a>
<a id="question-2"></a>
## 2. How is it done today, and what are the limits?

Engineers use source review, coding assistants, debuggers, tests, static analysis, graphical programming and other inspection tools. Existing textual and graphical environments already provide important capabilities and meaningful comparison baselines.

The difficulty is connecting generated changes to relevant behavior and to the version accepted for execution. Long textual implementations can require readers to reconstruct relationships across files and functions. Graphical tools can also overwhelm readers when hierarchy or layout is weak. AI integration can use supported programmatic interfaces; a binary format alone does not establish an impossibility.

The relevant comparison is the complete review and correction task with equivalent information, application scope and tool versions. The intended benefit of FROG must be demonstrated in that context.

<a id="heilmeier-h3"></a>
<a id="question-3"></a>
## 3. What is new, and why might it work?

The proposed contribution connects machine production, human graphical comprehension and machine execution through a common structured program representation. A FROG model can be serialized in `.frog`, projected into navigable graphical dataflow and processed through FIR/IR for supported compilers and targets.

Two entry routes are intended: direct FROG generation and declared translations from existing languages. Universal conversion is not an existing capability. A bridge must preserve supported types, state, memory, effects and execution rules; its graph must remain faithful to what the engineer will execute.

The graphical abstraction uses connections and perceptual cues to expose relationships, with hierarchy from overview to detail. The calculator illustrates the idea: about 1,000 C/H lines correspond to an application whose architecture can be shown on one graphical page with navigable cases. The source and overview expose different detail, so this is an illustration of abstraction, not a measured productivity result or evidence of an automatic importer.

The technical foundation includes public language and FIR contracts, the common C++ Frontend library and CLI, and selected Studio Run, Highlight and LLVM/native paths on Windows x64. The [12 November source checkpoint](../../docs/frontend-checkpoint-2026-11-12.json) identifies the Frontend consumed by Studio 0.0.8.094. The [9 November checkpoint](../../docs/frontend-checkpoint-2026-11-09.json) records separate standalone checks for its source kit. The contribution to establish is how effectively these techniques support human oversight.

A graphical abstraction can target efficient native execution. Efficiency depends on algorithms, semantics, data representation, runtime requirements, IR quality, optimization and hardware. Using an established backend is a sound direction, not a guarantee of equal performance for every language or program.

<a id="heilmeier-h4"></a>
<a id="heilmeier-h5"></a>
<a id="question-4"></a>
## 4. Who cares, and what difference would success make?

Teams that require human review or acceptance of software changes have a direct need. Industrial engineering, instrumentation, acquisition, test and automation provide concrete initial applications. Maintainers and teams inheriting unfamiliar software face related comprehension problems.

Success would let people locate consequential behavior, understand generated changes and complete valid corrections with an effort compatible with the workflow. It would connect acceptance to an identified executable revision. The same product also supports people writing programs directly.

FROG remains an open language foundation supporting compatible tools. Studio's proprietary value is the integrated human experience: graphical comprehension, authoring, navigation, execution observation and supported deployment. Adoption requires useful application coverage, usability and performance as well as an explanatory diagram.

<a id="heilmeier-h6"></a>
<a id="question-5"></a>
## 5. What are the risks?

- A conversion can alter semantics or hide memory, state, exceptions, concurrency or external behavior.
- A generated graph can be too dense, or its layout can change so much between revisions that the engineer loses the mental map.
- Abstraction can conceal relevant details and create confidence unsupported by the available evidence.
- A review, observation or acceptance can refer to an older revision than the code actually executed.
- Poor lowering, lost semantic information or runtime overhead can undermine performance despite a mature compiler backend.
- Broad import and target ambitions, missing function coverage or dedicated partner requests can exceed available engineering and maintenance resources.

Use explicit supported subsets, appropriate hierarchy, attributable diagnostics, meaningful change navigation and identified revisions. Compare behavior and performance under declared conditions. Advanced application-specific guarantees need their own methods and remain separate from the general product objective.

<a id="heilmeier-h7"></a>
<a id="question-6"></a>
## 6. How much will it cost?

No costed overall programme is established here. Estimate bounded stages: language/profile stabilization, import and semantic mapping, graphical abstraction and layout, navigation of changes, execution/source associations, FIR/lowering, function coverage, performance, documentation and maintenance.

Reuse the existing Frontend, product components and qualified compiler infrastructure. Include the effort to preserve semantics, support users and maintain each bridge. A partner-specific verification demonstrator needs its own scope and budget rather than becoming the cost model for the project as a whole.

<a id="heilmeier-h8"></a>
<a id="question-7"></a>
## 7. How long will it take?

No new delivery calendar is committed by this page. Continue stabilizing source/profile documentation and the supported execution path. Broader language-import work, missing-function coverage and performance progress through the maintained roadmap; additional compiler or hardware bridges have their own readiness decisions.

Stage the human-oversight workflow by declared application and source subsets rather than promise all languages and targets at once. Design requirements such as stable change views are not announced as delivered features. Existing owners and the [roadmap](../../Roadmap/Readme.md) retain sequencing authority.

<a id="question-8"></a>
## 8. What are the intermediate and final exams?

| Area | Evidence to seek |
| --- | --- |
| Faithful representation | An identified supported input maps to a FROG model without silently losing relevant behavior; unsupported constructs are reported. |
| Human comprehension | A reviewer can move from an overview to necessary detail and correctly explain the behavior relevant to a task. |
| Following generation | A reviewer can identify a meaningful generated change, its affected behavior and the revision under consideration. |
| Diagnosis and correction | An introduced fault can be found and a valid correction checked without damaging unrelated behavior. |
| Acceptance and execution | The reviewed, tested and built program revisions are attributable; changes invalidate relevant old conclusions. |
| Machine efficiency and deployment | FIR/lowering and the selected backend produce correct results with recorded runtime, memory and packaging conditions for the declared target. |
| Practical adoption | Representative applications remain usable at the declared scale, with an understood integration and maintenance burden. |

The calculator is an initial demonstration because its inputs and outputs are familiar while it exercises events, a queue and explicit state. Show the overview, enter a case, follow values with Highlight and check a change. Its roughly 1,000-line C comparison motivates the abstraction; source-line counts and screen area alone do not establish human efficiency.

For comparative claims, use equivalent requirements and information. Measure correct understanding, valid changes, effort and mistaken confidence. For performance comparisons, control algorithm, semantics, inputs, target and configuration; a shared compiler name alone is insufficient.

The [public UI Value Roundtrip dossier](../../Examples/02_ui_value_roundtrip/Readme.md) provides a small technical baseline with Source, FIR, lowering, acceptance outputs and an optional native proof. A human evaluation still needs defined tasks, equivalent information, observed errors and recorded effort. Establish importer, palette and target coverage for each scope claimed.

<a id="what-is-already-demonstrated-in-the-repository"></a>
## Existing foundation and remaining work

The common Frontend, public contracts and finite dossiers are existing foundations. Selected Studio paths already connect normal execution, Highlight and native LLVM compilation. [Project status](../../FROG-Project-Status.md), checkpoints and individual dossiers retain their own dates and scope.

General imports, complete change-review workflows and broader target support require further work and attributable evidence. The strategy keeps the distinction between intended architecture, editor capability and an admitted executable program.

<a id="future-end-to-end-poc-direction"></a>
## Separate collaboration options

Formal-verification adapters and specialized resource or fault guarantees can justify dedicated collaborations. Their value, architecture fit, cost and maintenance are assessed separately. [DARPA-oriented verification options](../Research/DARPA-Collaboration-Options.md) remain an independent exploration; they do not define the general philosophy or impose a required next milestone.

<a id="relation-with-strategy-roadmap-and-versioning"></a>
## Ownership and status

Strategy explains purpose and hypotheses. [Roadmap](../../Roadmap/Readme.md) owns sequencing; [Versioning](../../Versioning/Readme.md) and attributed dossiers own implementation status. Existing specifications, governance, licenses and public/private boundaries remain in force.

<a id="summary"></a>
## Summary

The central objective is human oversight of software produced at machine scale. FROG offers a structured common model, Studio uses graphical abstraction for comprehension and intervention, and FIR/IR support qualified compilation and execution. Faithful mappings, practical human benefit and machine efficiency each need evidence within their declared scope.
