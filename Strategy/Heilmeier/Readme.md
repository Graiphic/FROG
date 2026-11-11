# FROG and Graiphic Studio — Heilmeier framing

**Reviewed 11 November 2026 · Strategy, not a normative specification or a release claim**

FROG is the open graphical dataflow language foundation. Graiphic Studio is Graiphic's proprietary environment for authoring, inspection, execution and correction. The objective is to preserve informed human control as more software is produced with AI assistance.

This page follows the eight questions in [DARPA's Heilmeier Catechism](https://www.darpa.mil/about/heilmeier-catechism). Impact belongs with question 4; risks, cost, duration and exams are separate questions 5–8. The previous framing split impact into an extra question and combined duration with exams. Older section anchors are retained below for incoming links.

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
## The strategic problem

Generating a candidate implementation is becoming easier. Understanding its consequential behavior, checking requirements and carrying a valid repair into an accepted system remain engineering work. The graphical language can make dependencies, state and effects easier to investigate. Whether it improves human performance must be measured.

Inspection, test execution and formal proof complement one another. A readable diagram is not a correctness proof; a proof of a weak requirement is not assurance that the intended service works. The detailed [inspection and assurance direction](../Inspection-and-Assurance.md) describes these boundaries, the proposed experiment and its current maturity.

<a id="heilmeier-h1"></a>
<a id="question-1"></a>
## 1. What are we trying to do?

Help engineers understand what software will do, identify why a change is wrong, and make a repair that reaches the version actually run. This should remain possible when they did not write the first implementation themselves.

The shared graphical program should connect the requirement under review, relevant logic, available evidence and a place to intervene. Human responsibility means understanding both the behavior and the limits of the evidence used to accept it.

<a id="heilmeier-h2"></a>
<a id="question-2"></a>
## 2. How is it done today, and what are the limits?

Engineers already use source review, coding agents, graphical programming, debuggers, tests, model-based engineering and formal verification. These approaches have substantial capabilities. Textual code can be analyzed and proved; graphical tools can support AI through APIs and other integrations. A binary format does not make AI integration impossible.

The question is how much effort an engineer spends connecting intent, code or graph, observations, proof obligations, a correction and the resulting build. Graphical abstraction can reduce that effort, but can also hide effects or encourage false confidence. Existing proof-engineering programmes and model-to-code verification tools are relevant baselines, not empty territory.

<a id="heilmeier-h3"></a>
<a id="question-3"></a>
## 3. What is new, and why might it work?

The proposed contribution is an integrated workflow around an open, executable dataflow representation: human or agent authoring, hierarchical inspection, declared contracts, relevant runtime observations, correction and supported compilation. FROG separates language meaning and execution-facing representations from particular editors, compilers and runtime implementations. Studio develops the corresponding user experience.

JSON, dataflow graphs, code generation and formal verification already exist. Differentiation depends on faithful mappings and a demonstrable improvement in a complete engineering task, not the presence of those ingredients alone.

There is an existing technical foundation: public source and type contracts, a common C++ Frontend library/CLI, finite executable dossiers and selected normal Run/Highlight/LLVM paths. This makes a restricted experiment plausible. It does not establish a general source importer, a FROG-to-Verus bridge or correctness of every compiled programme. See the [9 November Frontend checkpoint](../../docs/frontend-checkpoint-2026-11-09.json) and [current status](../../FROG-Project-Status.md).

<a id="heilmeier-h4"></a>
<a id="heilmeier-h5"></a>
<a id="question-4"></a>
## 4. Who cares, and what difference would success make?

Instrumentation, acquisition, test, automation and software-maintenance teams provide a concrete initial field. The language architecture is general purpose. Engineers could spend less effort reconstructing dependencies, diagnosing an incorrect assumption and checking that a repair reached the accepted executable.

For embedded or critical systems, resource and fault constraints motivate explicit properties and target assumptions. Host examples do not establish real-time behavior, fault tolerance or target qualification. Those claims require separate evidence.

The intended industrial result is a shorter path to a system that can be understood, accepted and maintained. Success must include correctness, scope understanding, integration cost and maintenance effort. The proprietary Studio experience and prospective integrations provide the commercial opportunity; the FROG language remains open under its existing licenses.

<a id="heilmeier-h6"></a>
<a id="question-5"></a>
## 5. What are the risks?

- A translation may change integer semantics, state, aliasing, evaluation order or effects. For example, an eager dataflow selection is not generally equivalent to a lazy conditional.
- A diagram, observation or simulated scenario may hide the condition that matters on the target.
- A verifier may prove an insufficient requirement, rely on an unmodeled external component, check no relevant obligations, or return an inconclusive result.
- A proof of exported code may be mistaken for proof of the original program, the exporter or a separate product binary.
- A stale result may be attached to a changed program, or a convincing display may increase confidence without increasing understanding.
- Tool integration, training and maintenance may cost more than the workflow saves.

Use explicit supported subsets, trusted-component boundaries, source identities, negative cases, fixed requirements and comparisons against known behavior. Keep failed, unsupported, inconclusive and stale results visible. Do not suppress difficult behavior or weaken a requirement to produce a successful demonstration.

<a id="heilmeier-h7"></a>
<a id="question-6"></a>
## 6. How much will it cost?

A costed programme is not established by this strategy. No funding award or commercial price is inferred from this framing.

Estimate a bounded feasibility stage first: semantic review, the minimal model and properties, exporter or verification-condition work, tool integration, diagnostics, execution comparison and evaluation design. Name the engineering and verification responsibilities, infrastructure and contingency assumptions. A subsequent industrial pilot needs its own scope and cost estimate. Universal language or hardware coverage is not one deliverable.

<a id="heilmeier-h8"></a>
<a id="question-7"></a>
## 7. How long will it take?

No delivery calendar is committed. Duration depends on the chosen semantic subset, existing implementation support, verifier environment, staffing and access to evaluation participants or equipment.

First establish feasibility and a bounded contract. Then decide whether to build a restricted demonstrator, evaluate it and expand to an industrial pilot. A calendar should follow that decision and its resource estimate. The sequence does not authorize a new product campaign or displace existing source stabilization, coverage and performance priorities.

<a id="question-8"></a>
## 8. What are the intermediate and final exams?

| Gate | Required evidence |
| --- | --- |
| Feasibility | A precisely defined transition model, machine types, assumptions, exclusions and a working verifier reference. A manually verified model validates tool use, not a FROG integration. |
| Restricted implementation | Stable property/program associations; deterministic export for a declared subset; explicit refusal outside it; agreement with an independent reference for the finite cases. |
| Verification and repair | An attributed success for the correct artifact; meaningful rejection of the deliberate defect; checked witnesses where available; no success after a relevant edit, weakened requirement or empty obligation set. |
| Human-workflow pilot | Equal requirements, assumptions and verifier information in textual and graphical workflows; measured diagnosis accuracy, valid correction, effort and mistaken confidence; predeclared acceptance criteria. |
| Industrial scope | Reproducible results for the selected application, supported target and operating conditions, including integration and maintenance cost. Expand claims only when new evidence supports the new scope. |

A candidate demonstrator is a sequential occupancy controller for a queue of capacity eight, initialized at zero. Check bounds, exact admission, exact state updates, rejection behavior and arithmetic. All 18 admissible state/command pairs provide a finite comparison set. A mutation `n < 8` to `n <= 8` should expose the transition from eight to nine. Rejecting every command must also fail the functional contract, despite preserving the bound.

These are proposed exams, not reported results. The scope excludes FIFO contents, concurrency, timing and physical faults. Verification of exported Rust would not automatically establish fidelity of the exporter or correctness of Studio's separate LLVM-generated binary. Runtime Highlight may illustrate a checked scenario; it is not the proof engine.

<a id="what-is-already-demonstrated-in-the-repository"></a>
## Existing foundation and remaining work

The numbered reference dossiers and the public common Frontend provide existing bounded evidence. LLVM is already part of selected execution paths; it is not merely a future first experiment. This does not imply complete FIR/operator coverage, general multi-runtime equivalence or support for all deployment targets.

FROG serialization/profile work and Studio development continue. Broad imports from existing languages and additional compiler targets remain future work. No FROG/Studio-to-Verus integration or comparative human-performance result is established here. Do not replace these distinctions with a single maturity label.

<a id="future-end-to-end-poc-direction"></a>
## Next experiment, subject to feasibility

Connect a small supported executable graph to typed requirements and an existing verifier, then examine whether a human can diagnose and repair a violated property more accurately or efficiently. Reuse the existing representations and ownership boundaries. Evaluate Verus as a candidate rather than declaring a new backend already selected or implemented.

The relevant public research context includes [PROVERS](https://www.darpa.mil/research/programs/pipelined-reasoning-of-verifiers-enabling-robust-systems) and the [Verus overview](https://verus-lang.github.io/verus/guide/overview.html). These references establish the external tools and research questions, not a Graiphic partnership, endorsement or funded programme.

<a id="relation-with-strategy-roadmap-and-versioning"></a>
## Ownership and status

Strategy explains the objective and hypotheses. [Roadmap](../../Roadmap/Readme.md) owns sequencing; [Versioning](../../Versioning/Readme.md) and attributed implementation dossiers own their published status. Normative source, semantics, FIR and target contracts remain with their existing specification owners.

<a id="summary"></a>
## Summary

The ambition is continuity of informed human control from generated logic to an accepted executable. Readable dataflow, testing and scoped formal verification can support that goal together. Faithful representations, actual engineering benefit and honest evidence boundaries must establish the value.
