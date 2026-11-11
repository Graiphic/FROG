# Inspection, verification and human control

**Strategy update · 11 November 2026 · Non-normative research direction**

Graiphic's objective is to help engineers understand consequential software behavior, assess the evidence behind it and carry a valid correction into the version that runs. This matters whether a person draws the program, an agent generates it, or a future supported bridge imports it from another language.

FROG supplies the open graphical dataflow language and its public representation contracts. Graiphic Studio is Graiphic's proprietary engineering environment built around that foundation. Their shared direction is **continuity of informed human control from generated logic to executable results**. Openness supports independent implementations; the product's value depends on faithful mappings, useful interaction, maintained integrations and demonstrated engineering results.

A compact diagram can help someone find where to investigate. It cannot establish correctness merely by looking clear. Inspection, automated testing and formal verification address complementary questions.

## Three questions, with different evidence

| Question | Evidence that can address it | Limit |
| --- | --- | --- |
| What does this program do, and where should I intervene? | Graph structure, source, types, effects, hierarchy and source-aligned observations | Readability does not establish that a requirement holds. |
| What happened under these conditions? | An identified execution, controlled inputs, expected outputs and recorded observations | Successful runs establish results for those cases, not all possible executions. |
| Does a stated property hold throughout a defined model? | An explicit specification, assumptions and a sound verification method for the supported semantics | The conclusion applies to the model or artifact actually verified, under its assumptions and trusted toolchain. |

Semantic validation, target admission, compilation, execution and application-level proof remain distinct. A program may be well typed and still implement the wrong algorithm. A compiled program may still fail in execution. A mathematical proof may establish a property that is too weak to describe the intended service.

Source mappings and hashes identify the objects and versions involved. They do not prove that a translator preserves meaning, authenticate an author by themselves, or turn a verifier log into an independently checkable proof certificate.

## Current foundation and proposed capabilities

FROG's syntax, profiles, serialization and documentation continue to evolve. Existing contracts retain their versioned owners; this strategy note neither replaces them nor adds a language feature.

| Surface | Position as of this review |
| --- | --- |
| Common Frontend | Public C++ library and CLI, with an attributed [9 November source checkpoint](../docs/frontend-checkpoint-2026-11-09.json). Its finite checks are not whole-product qualification. |
| Studio execution | Normal Run, Highlight and native LLVM compilation exist for selected functions and profiles. The demonstrated native product direction is Windows on x86-64; a visible palette is not an execution-support matrix. |
| Canonical source and Studio documents | Public Source and Studio authoring profiles have explicit [compatibility boundaries](../docs/studio-source-compatibility.md). A saved editor document is not automatically admitted by every consumer. |
| Existing-language imports | General imports from C, C++, Rust, JavaScript or Java remain future work; source and build context must be preserved within each declared subset. |
| Other deployment targets | Arm edge systems, microcontrollers, FPGA and other compiler or runtime bridges are prospective capabilities requiring their own contracts and evidence. |
| Formal verification | No FROG/Studio-to-Verus integration, whole-language correctness proof or verified LLVM-generated product binary is established by this update. A restricted verification experiment is proposed below. |
| Human benefit | Faster accurate diagnosis and valid repair are research hypotheses. A smaller picture or fewer visible lines is not a measured productivity result. |

The public [project status](../FROG-Project-Status.md), [versioning records](../Versioning/Readme.md), implementation checkpoints and individual dossiers supply their own attributed evidence. This review adds no new runtime test result or release certification.

## A bounded verification experiment

A useful first candidate is a **sequential occupancy controller for a queue of capacity eight**. It represents occupancy, not the queue's message contents or a concurrent queue implementation. One command is processed per transition. A machine-integer type and its arithmetic rules must be fixed before exporting a proof model.

For an admissible state `0 <= n <= 8`, the intended behavior is:

| Command | Acceptance condition | Next occupancy |
| --- | --- | --- |
| Enqueue | `n < 8` | `n + 1` if accepted, otherwise `n` |
| Dequeue | `n > 0` | `n - 1` if accepted, otherwise `n` |

The proposed obligations cover initialization at zero, preservation of the bound, exact acceptance, exact state changes, unchanged state after rejection and arithmetic validity. Checking the bound alone is insufficient: rejecting every command preserves the bound but fails the service requirement.

There are 18 admissible state/command pairs. Exhaustive comparison on that finite transition domain would be useful implementation evidence. Proving initialization and invariant preservation would support an inductive claim over finite command sequences in this model. These are different forms of evidence; neither is reported here as already executed for FROG.

A deliberate mutation from `n < 8` to `n <= 8` has a concrete violating transition: enqueue at eight produces nine. Nine enqueues from zero illustrate a candidate replay. A failed proof attempt alone is not a validated counterexample, and a verifier is not assumed to provide an automatically replayable Studio trace.

This initial scope excludes FIFO contents, concurrency, real-time deadlines, energy budgets, physical faults, device behavior and general compiler correctness. A numerical or Boolean occupancy model must not be described as a verified embedded messaging system.

## Translation and result boundaries

Reuse the existing typed program representations and source identities. The feasibility review must first establish which operations, state transitions and effects they can express faithfully. A companion property document is an option to evaluate, not a new standardized `.frog` field or an approved public verification profile.

A restricted Rust/Verus exporter is one candidate; direct verification-condition generation is another. Tool choice requires an explicit semantic and maintenance argument. Existing formal-methods work already combines models, contracts and verification; drawing a graph or generating annotated code is not sufficient evidence of a new scientific contribution.

A faithful mapping must preserve machine-integer behavior, old and next state, evaluation conditions and relevant effects. An eager dataflow Select cannot silently become a lazy conditional if doing so changes evaluation, faults or effects. An unsupported operation must block a complete claim, rather than disappear behind an assumption or an ignored function body.

If Rust is exported for Verus, a successful check concerns the exported code and specifications. It does not automatically prove the source-to-Rust mapping, Studio's execution engine or the separate LLVM-generated binary. The [Verus overview](https://verus-lang.github.io/verus/guide/overview.html) explicitly excludes verification of the verifier and Rust/LLVM compilers from its current objectives.

Results need to identify the program, properties, assumptions, exporter, verifier and configuration. Distinguish established, not established, unsupported, tool error, inconclusive/timeout and stale results. A relevant edit invalidates old evidence. Zero checked obligations, hidden assumptions or a weakened requirement cannot produce a complete-success claim.

## Evaluate the engineer's actual task

The comparison should give textual and graphical workflows the same requirements, contracts, assumptions, verifier results and task information. Measure correct diagnosis, valid correction, elapsed effort, incorrect conclusions and confidence in those conclusions. Include the cost of integration and re-verification. Set acceptance criteria before collecting results; reduced time only counts as an improvement when correctness and scope understanding are preserved.

The calculator comparison illustrates architecture and navigation. Its C implementation also includes UI, parser and queue work, while an overview exposes only selected graphical detail. The comparison of source-line count with screen area does not establish faster review or better defect detection.

[PROVERS](https://www.darpa.mil/research/programs/pipelined-reasoning-of-verifiers-enabling-robust-systems) provides relevant public context on accessible formal methods and proof maintenance. It is not evidence of Graiphic participation, endorsement or funding. No external discussion creates an implementation commitment or validates the proposed human benefit.

## Development order and decisions

Stabilize the source/profile documentation and the existing execution path. Continue missing-function coverage, correctness and performance work within their existing owners. Define supported import contracts before claiming language interoperability; additional deployment bridges need independent qualification.

A verification experiment should enter this sequence only after a bounded feasibility decision, with a fixed semantic subset, resources and exit criteria. This strategy update authorizes no new backend, product implementation, automatic proof execution or roadmap campaign. No programme budget, funded partnership or delivery calendar is established here.

Read the [eight-question Heilmeier framing](Heilmeier/Readme.md), [leadership and validation philosophy](../docs/graiphic-philosophy-and-validation.md) and [format-first architecture](FormatFirstNotIdeFirst.md) for the complementary strategic and governance boundaries.
