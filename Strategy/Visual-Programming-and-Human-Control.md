# Visual programming and human control

**Human oversight of code generation · philosophy reviewed 11 November 2026 · capability references updated 13 November 2026 · Non-normative**

Graiphic's strategic focus is to increase the human capacity to follow, understand and validate software as machine generation expands. The strategic premise is that generated code becomes routine in software production, while some engineering processes continue to require human review and acceptance. Producing a candidate program and understanding what one is accepting have different costs.

FROG provides a structured program model, serialized in `.frog`. Graiphic Studio projects that model into navigable graphical dataflow so people can investigate behavior and intervene. FIR and downstream IR connect the program to supported compilers and execution targets. The objective is continuity between generated logic, human understanding and the version that actually runs.

This direction builds on general-purpose graphical programming. The same environment also serves programs written directly by people, software inherited from colleagues and long-term maintenance. FROG remains the open language foundation; Studio remains Graiphic's proprietary integrated product.

## A visual representation people can work with

Text is one way to express a program. Graphical dataflow adds a spatial abstraction: connections, color, shape, position, text, arrangement and labels can expose relationships together. Hierarchy lets an engineer start with an overview and enter the detail relevant to a question. These cues should help people find dependencies, state, effects and places to intervene.

The goal is to make relevant information accessible within human attention and working capacity. A page filled with an unreadable graph would not achieve it. Consistent visual conventions, useful grouping, readable labels and progressive access to detail are part of the design. Layout guides comprehension; actual connections, structures and effects determine program meaning.

Research on graphical programming describes the role of layout and perceptual cues while emphasizing learned reading skills and task context. The intended benefit therefore depends on the quality of the representation and its use, rather than graphics alone. See [Petre's study of graphical readership](https://www.cs.toronto.edu/~chechik/courses18/csc2125/paper12.pdf).

## Serialization, graphical projection and machine execution

The target architecture accepts direct generation of a FROG program and, progressively, translations from existing languages under explicit support contracts. The model carries meaning; `.frog` serializes it. Drawing its graphical view is a projection of that model, distinct from executing the program.

```text
Generated or existing source -- declared semantic translation --+
                                                              |
Direct FROG authoring or generation ---------------------------+
                                                              v
                                                FROG program (.frog)
                                                   /             \
                             Studio's navigable diagram     common Frontend
                                  human review/change      validation and FIR
                                                   \        target admission
                                                    \      lowering/backend IR
                                                     \     compiler and target
                                                      same program revision
```

Human inspection should retain high-level structure and meaningful names. Compilation needs precise types, operations and effects. These are related views of the same program, with different responsibilities. A low-level machine IR alone may no longer contain the hierarchy useful to the reader.

Bringing C, C++, Rust, Java, Python and other languages into this workflow is a long-term interoperability direction. Each bridge must declare its subset, dependencies and behavior. Mutable state, memory, exceptions, external calls and concurrency cannot be discarded to make a simpler picture. General imports are prospective; this philosophy does not announce a universal converter.

## Connect understanding to execution and correction

For supported programs, Studio links the Front Panel, Block Diagram and Source with Run, Highlight and Build. Values and activity can be examined in context. The engineer should be able to return to relevant logic, make a change and check the resulting behavior.

Where a process requires human acceptance, the reviewed program and the executed or packaged revision must remain connected. An old view or result cannot stand for a changed candidate. Source identities and mappings help establish associations; their existence alone does not establish a faithful translation.

## Follow generation as it changes

Following machine production involves more than viewing one finished program. Engineers need to identify what a new generation changed, which behavior it may affect and which revision they accepted. Stable object identities, a stable mental map, meaningful change views and impact navigation are important design and evaluation requirements.

These requirements describe the intended workflow, not a claim that every importer, change view or acceptance mechanism is already delivered. Evaluate whether people can understand a consequential change, find an introduced error and complete a valid correction with the available tools.

## Demonstrate value through complete examples

The calculator comparison illustrates the spatial abstraction. Its C implementation contains about 1,000 lines (1,039 C/H lines in the supplied example), while a graphical page can present the program architecture with access to its cases. Events, a queue and explicit state provide concrete relationships for an engineer to follow.

The claim is that an architectural overview can make relevant structure accessible on one page. Detail remains available in the underlying structures and cases. The C version includes UI, parser and queue code, while the overview exposes selected graphical detail. This comparison is not evidence of an automatic C importer or a measured review-speed advantage.

A useful walkthrough moves from the overview to a relevant case, follows values with Highlight and checks a change. Compare understanding, diagnosis and valid correction on equivalent tasks. The AI-generation/inspection chart expresses the intended combination of machine production and human access; its positions remain qualitative targets.

## An open language and an integrated product

FROG's public source, type, semantic and FIR contracts provide a foundation for compatible tooling and independent implementations. Studio develops the human interface to that foundation: authoring, abstraction, navigation, diagnostics, execution observation and supported deployment.

The open model can serve both tools producing programs and people reviewing them. Official project leadership, contribution rules, licenses and public/private boundaries remain those of the existing [governance](../GOVERNANCE.md#official-project-rules), [contribution process](../CONTRIBUTING.md#contribution-rules) and [component philosophy](../docs/graiphic-philosophy-and-validation.md).

## Keep program meaning distinct from deployment choices

A graphical notation does not require slow interpreted execution. The goal is to preserve the useful meaning and optimization information in FIR/IR, then use qualified lowering, established compilers and the required runtime services for the selected target.

C, C++, Rust, Java and Python are abstractions, but they also define execution semantics and runtime requirements. Algorithm, data structures, memory behavior, runtime costs, IR quality, optimization and hardware all affect efficiency. A shared compiler backend alone does not guarantee equal performance.

[LLVM's frontend guidance](https://www.llvm.org/docs/Frontend/PerformanceTips.html) explains how IR construction and language-specific information influence optimization. [MLIR's rationale](https://mlir.llvm.org/docs/Rationale/Rationale/) describes progressive lowering across abstraction levels. These support the architectural approach; they do not qualify a particular FROG bridge or add a new compiler dependency.

The existing foundation includes the public common Frontend and selected Studio Run, Highlight and LLVM/native paths on Windows x64. The [12 November source checkpoint](../docs/frontend-checkpoint-2026-11-12.json) identifies the Frontend consumed by Studio 0.0.8.094. It records a source publication. The [9 November checkpoint](../docs/frontend-checkpoint-2026-11-09.json) retains the standalone-build evidence for its own source kit. The [evidence guide](Inspection-and-Assurance.md#trace-one-program-through-the-stages) follows a public example through the stages. Broader imports and targets require their own qualification.

## Evaluate ideas by their value to the project

The central problem is human comprehension and acceptance of software produced at machine scale. Useful ideas from customer or research discussions should be assessed against that goal, their broader value, architecture fit and maintenance cost.

Partner-specific formal-verification adapters, bounded demonstrators and specialized embedded guarantees remain separate extensions or collaborations. They do not determine the core philosophy or impose a next milestone. The [general Heilmeier framing](Heilmeier/Readme.md) develops the project objective; the [evidence guide](Inspection-and-Assurance.md) explains the supporting checks. [DARPA-oriented collaboration options](Research/DARPA-Collaboration-Options.md) remain an independent exploration.
