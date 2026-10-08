# IEC 61499 — industrial bridge evaluation

**Status: informative evaluation note, 8 October 2026.** No IEC 61499 backend,
exporter, importer, runtime integration, adopted FROG profile or certification is
established by this document. The candidate paths and checks below are proposals;
none has been implemented or executed for this documentation update.

This note makes the IEC 61499 direction explicit alongside the
[industrial bridge architecture](../../FROG-Architecture.md#industrial-execution-bridges)
and the [engineering traceability proposal](engineering-traceability.md).
It adds no source field, primitive or mandatory industrial dependency.

## What IEC 61499 contributes

[IEC 61499-1:2012](https://webstore.iec.ch/en/publication/5506) describes an
architecture for function blocks in distributed industrial measurement and
control systems. Here, IEC 61499 names that architecture and standard family,
not a particular network transport protocol.

The [Eclipse 4diac introduction](https://eclipse.dev/4diac/doc/intro/iec61499.html)
explains distinct event and data connections, event-associated data updates and
the Execution Control Chart of a basic function block. These concepts matter
when assessing activation, state and causality; graphical resemblance to a FROG
Diagram does not establish semantic equivalence.

[UniversalAutomation.org's technology description](https://universalautomation.org/uao-technology/)
describes a shared runtime based on IEC 61499. This is an informative example of
an ecosystem to evaluate, not a FROG partnership, endorsed implementation or
demonstration of interoperability. Runtime access and integration rights would
need a separate assessment. No vendor runtime is required by FROG.

## Three different integration claims

| Candidate | Proposed scope | What it would not establish by itself |
| --- | --- | --- |
| Data exchange | Exchange explicitly typed inputs and outputs through a separately supported transport or service contract. | Execution of a FROG program by an IEC 61499 runtime. |
| Encapsulated computation | Invoke a bounded FROG calculation behind a target-supported function-block interface, with explicit activation, data and completion/error behavior. | Translation of arbitrary FROG structures or distributed execution. |
| Program translation | Lower a declared FROG subset into an identified IEC 61499 toolchain/runtime's accepted realization. | Universal source conversion, binary portability or identical scheduling across runtimes. |

These are evaluation options, not interchangeable capabilities. The first useful
experiment could be a small calculation with an explicit interface; the choice
between encapsulation and translation remains open until a target is selected.
The bridge could use runtime, compilation or a hybrid realization.

## Pipeline and ownership

```text
Canonical .frog source and explicit public interface
          | structural and semantic validation
          v
Validated meaning -> canonical FIR
          | lowering and backend contract
          v
Candidate industrial bridge (declared subset)
          | target-specific realization and activation boundary
          v
Identified IEC 61499 toolchain/runtime and configuration
```

This is a proposed downstream integration path. [Language](../../Language/Readme.md)
keeps ownership of FROG execution meaning;
[FIR](../../IR/Readme.md), [lowering](../../IR/Lowering.md) and the
[backend contract](../../IR/Backend%20contract.md) retain their published
boundaries. An event connection, state machine or deployment configuration in a
target tool is not a second source of implicit behavior for an unchanged FROG
program. Any behavior required by the FROG program needs an explicit applicable
source or capability contract. Requirements links do not supply it.

The public interface remains independent of Front Panel widgets. A calculation
without a Front Panel should be the initial study case; UI interaction, hardware
I/O, foreign calls and references are supported only if the selected subset and
host contracts actually cover them. A private editable document or a successful
Highlight simulation is not evidence of a valid public FIR or industrial backend.

## Questions to resolve for a bounded bridge

The following is a review checklist, not new normative FROG requirements:

| Concern | Required decision before claiming the corresponding support |
| --- | --- |
| Activation and data | Define when inputs are sampled, how one activation completes, which event/data associations are used and when outputs become available. Do not equate an event with a Boolean data wire. |
| Causality and effects | Preserve FROG dependencies and externally visible effects; specify ordering, reentrancy, concurrent activation and queue/overflow behavior for the chosen runtime. |
| Types | Map exact widths, signedness, floating-point behavior, overflow, conversions, strings, arrays and aggregates; reject unsupported representations. |
| Structures and state | Declare supported branches/loops, initialization, retained state, reset, lifetime, termination and zero-iteration behavior. No silent replacement with a different execution pattern. |
| Interface and calls | Specify port identities, parameters/directions, invocation, completion, errors, cancellation and resource ownership for the selected encapsulation or translation boundary. |
| Time and resources | Identify clocks, deadlines if claimed, blocking, memory limits, target workload and instrumentation effects. No hard real-time or safety guarantee follows from source portability. |
| Deployment and distribution | Identify device/resource assignment and dependencies. Distribution, communications, latency and failure handling are separate capabilities, not an automatic consequence of a local bridge. |
| Mapping and evidence | Retain source/FIR attribution and identify generated artifacts, target instance, versions, configuration and observation limits. Never fabricate observations for optimized-away objects. |
| Unsupported content | Diagnose unsupported capabilities before launch; preserve the source rather than silently dropping nodes, coercing types or weakening behavior. |

The published [Interop profile](../../Profiles/Interop.md) remains unchanged.
Data exchange or remote access needs its own explicit supported contract; this
note does not add a network primitive to that profile or standardize a vendor ABI.

## Proposed first validation slice

**Not executed.** A future experiment would select one runtime/toolchain version
and one integration path, then record at least the following cases:

1. A headless scalar calculation with explicit inputs and output: compare values
   and types against a FROG reference for the declared numeric subset.
2. Multiple consumers of one result: verify one result per activation and its
   completion association, without additional evaluations caused by observation.
3. Repeated activation of a stateful accepted calculation: verify initialization,
   state evolution, reset and instance isolation.
4. Event-associated input changes and completion/error handling: check the chosen
   runtime's actual sampling and ordering, including the declared concurrency limit.
5. Unsupported structures, types, host/UI requirements or unpreservable effects:
   verify explicit rejection with source attribution and no attempted launch.
6. Rebuild or change the target configuration: retain historical results while
   refusing automatic applicability to the new artifact or instance.

Any timing or distributed claim would require additional tests of that exact
configuration. A local arithmetic result cannot establish them. Records could
use the [associated-document proposal](engineering-traceability.md), once a
format is adopted, to connect pinned source/build/target/test revisions with real
observations. A requirement link or a passed test alone does not authorize release.

## Current result and deferred work

This update supplies an explicit evaluation direction and primary-source reading
links. Selection of a target, legal/access review, semantic mapping, a bridge
contract, implementation, executable fixtures and target evidence remain open.
Formal adoption follows [FROG governance](../../GOVERNANCE.md). No normative
schema, standard primitive, implementation or existing file compatibility changes.
