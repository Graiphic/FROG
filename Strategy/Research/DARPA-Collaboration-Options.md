# DARPA-oriented collaboration options

**Exploratory note · 11 November 2026 · Separate from the general project strategy**

This note collects possible extensions for verification-oriented research discussions, including a potential fit with DARPA interests. It describes candidate work rather than an agreed programme, a funded partnership or an implementation commitment. The general purpose of FROG and Studio remains in [Visual programming and human control](../Visual-Programming-and-Human-Control.md).

## Why investigate a collaboration

A specialized partner may need guarantees about a declared property under resource, timing or fault assumptions. Such needs can justify an application-specific demonstrator or a verification adapter. They do not automatically redefine the language, the everyday Studio workflow or the common roadmap.

First identify a concrete problem, the intended users, the guarantee required and a credible comparison. Decide whether a shared improvement has wider value or should remain a dedicated extension. Cost, maintenance, semantic fidelity and opportunity cost belong in that decision.

## One candidate: sequential occupancy of a bounded queue

A deliberately limited experiment could model the occupancy of a queue with capacity eight, initialized at zero. It represents a sequential admission controller, not FIFO contents or a concurrent queue implementation. Fix the machine-integer type and arithmetic semantics before implementation.

| Command | Acceptance for a valid state `0 <= n <= 8` | Next occupancy |
| --- | --- | --- |
| Enqueue | `n < 8` | `n + 1` if accepted, otherwise `n` |
| Dequeue | `n > 0` | `n - 1` if accepted, otherwise `n` |

The proposed contract covers initialization, preservation of bounds, exact admission, exact updates, unchanged state on rejection and arithmetic validity. Rejecting every command preserves the bound but fails the service requirement. The 18 admissible state/command pairs form a finite comparison set. Mutating `n < 8` to `n <= 8` exposes enqueue at eight producing nine; nine enqueues from zero provide a candidate execution replay.

These are proposed checks, not FROG proof results. This model excludes message contents, concurrency, deadlines, energy budgets, physical faults and target hardware behavior. Those topics would require separate contracts and experiments.

## Possible verification tools and trust boundaries

A restricted Rust/Verus export is one candidate to assess, alongside other suitable methods. The [Verus guide](https://verus-lang.github.io/verus/guide/overview.html) describes verification of Rust against specifications and excludes verification of the verifier and Rust/LLVM compilers from its current objectives.

A successful check of exported Rust would not automatically establish fidelity of the export, the original graph or Studio's separate LLVM binary. Preserve machine arithmetic, old/next state, evaluation conditions and effects. An eager dataflow Select must not silently become a lazy conditional when that changes behavior.

Identify the program, properties, assumptions, exporter and verifier. Keep failed, unsupported, inconclusive, tool-error and stale results distinct. Empty obligations or hidden assumptions cannot establish the intended contract. A failed proof attempt alone is not a checked counterexample.

## Collaboration-specific evaluation and decision

If the purpose is to test human benefit, compare workflows given equivalent requirements, assumptions and verifier information. Examine correct diagnosis, valid repair, effort and mistaken confidence. A finite technical demonstration and a human-workflow evaluation answer different questions.

[PROVERS](https://www.darpa.mil/research/programs/pipelined-reasoning-of-verifiers-enabling-robust-systems) is relevant public research context. It establishes no Graiphic participation or endorsement. No cost, delivery calendar or selected verifier integration is committed here.

Proceed only after a bounded feasibility decision with an owner, resources, supported subset and exit criteria. Keep dedicated research deliverables separate from product priorities. Propose any reusable improvement to the core through the existing architecture, governance and validation process.
