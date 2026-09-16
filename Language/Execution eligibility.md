# Execution eligibility and diagnostics

Revision **1.0 — 2026-09-16**. Maintainer: FROG.
Status: **normative admission requirements**, with implementation evidence kept separately.

This is the stable reference for deciding whether a FROG program may be
validated, built and started. It consolidates the admission gate in
[Semantic validation before FIR](Semantic%20validation%20before%20FIR.md),
[Control structures](Control%20structures.md) and
[State and cycles](State%20and%20cycles.md). It does not create new source fields,
change the polarity of a loop, or silently publish an editor-specific profile.

**MUST**, **MUST NOT**, **SHOULD** and **MAY** describe requirements, not a claim
that every current editor, compiler or runtime implements them.
A palette icon is not evidence of executable support.

## 1. Independent gates — EXEC-001

| Gate | Required evidence |
| --- | --- |
| Loadable | Source can be parsed; its envelope and version are recognized. |
| Editable / saveable | The authoring work can be preserved, including incomplete wires. |
| Structurally valid | Identities, references, scopes and source shapes are coherent. |
| Semantically accepted | All applicable rules of the selected language/profile are established. |
| Buildable for a target | Required lowering, providers and backend capabilities are available. |
| Current artifact | The built artifact matches the requested program and configuration. |
| Startable | Runtime and target launch prerequisites are satisfied. |

These gates MUST NOT be collapsed into “no errors in the list”.
Unsupported validation is not semantic acceptance. A missing runtime is not a
type error. Saving an incomplete document MUST NOT assert that it is executable.

Public semantic outcomes remain accepted/rejected, with an explicit unsupported
implementation outcome where appropriate. Internal unknown/pending/stale states
MUST NOT be interpreted as accepted.

## 2. Revision and execution requests — EXEC-002

Validation and construction MUST identify the same semantic revision, entry
point, active dependency closure, contracts, language/profile, build configuration
and target capabilities. Editing a type, binding, branch, default policy, dependency
or compilation symbol invalidates affected results. Undo/redo and reload must
re-establish the corresponding result. Pure presentation changes do not change
program meaning; moving a node across a scope boundary does.

A Run request MAY build an artifact first. It need not require a pre-existing
artifact, but MUST NOT silently start an older artifact instead of the requested
revision. A late asynchronous validation result MUST NOT authorize newer source.
Actual running instances and the current editable source are distinct states.

## 3. Source, ports and types

- **EXEC-003 — identities and scope.** Validate uniqueness in the scope required
  by the source format and resolve qualified endpoints. Layout, proximity,
  crossed lines, labels and the visible region do not establish connectivity.
  Unknown families or modes require a published contract and supported validator,
  otherwise report invalid or unsupported as appropriate.
- **EXEC-004 — wires.** Endpoints must exist, directions must be legal, a consumer
  must have the permitted number of producers, boundaries must be crossed through
  explicit legal bindings, and types must be compatible. Active incomplete wire
  fragments block semantic acceptance. Fan-out is legal when every use is valid.
- **EXEC-005 — required inputs and active operands.** Apply required, optional,
  recommended, conditional and grouped requirements from the actual node contract.
  A default replaces an absent connection only when that contract permits it.
  Merely enlarging an extensible node does not create mandatory operands.
  For the base variadic Add, a/b remain required and optional unwired slots do not
  participate. A malformed wire on an optional slot is not an absent operand.
  Other primitives follow their own published arity, not Add's appearance.
- **EXEC-006 — types and values.** Resolve types, representations, domains and
  statically known shapes without substituting an invented f64 or hidden cast.
  Only contract-authorized coercions are legal. Unknown dynamic properties
  require a defined runtime check; they are not automatically static errors.
- **EXEC-007 — participation and outputs.** Validate every node in every active
  ordinary region, including isolated nodes with unused outputs. Dead-code
  elimination must not legalize invalid source. A defined but unconsumed ordinary
  output is not an error by itself. Declared structure outputs and promised
  public outputs still require their complete production contract.

## 4. Structures and their outputs

**EXEC-008 — recursive coverage.** For every declared structure output and every
region/normal completion path participating in its contract:

1. Reject malformed, incompatible or multiple producers.
2. Accept one valid producer.
3. Otherwise accept a replacement only if explicitly authorized, typed and
   applicable to that output/path.
4. Otherwise reject the undefined output.

Validate nested producers recursively. An outside wire, an unused outside output,
a hidden branch, or a constant selector does not waive this obligation.
Ordinary runtime alternatives remain subject to validation before optimization.
A structure with no data tunnels can be valid when its control and body contracts
are complete. Error/cancellation paths follow their declared protocol and MUST
NOT invent successful output values.

**EXEC-009 — four different defaults.** A default branch selects a region;
an output default replaces an absent producer; a zero-iteration value defines
a loop's empty execution; previous state is explicit memory. These are distinct.
Defaults must have resolved types, valid domain/shape, deterministic meaning and
persistent source representation or explicit generated producers. A replacement
MUST NOT conceal a broken wire, incompatible type, multiple sources or fragment.
False, zero and the empty string are values, not missing fields. A resource handle
MUST NOT be fabricated from zero. A one-time command generating constants is not
a persistent default policy for future branches.

| Family | Admission obligations |
| --- | --- |
| **EXEC-010 — Case** | Resolve the selector; require deterministic, complete selection and output coverage independently. Base bool requires true/false; base string uses unique exact matches and its required default. Integer, enum, ranges or patterns require their published profile and domain rules. A visual label does not replace enum identity/value. |
| **EXEC-011 — For** | Base count and index are i64; count is non-negative; the body and count contract must be complete. Validate the body even for constant zero. A last_value output requires a compatible zero_iteration_value whenever zero iterations remain possible. That value does not replace an unwired body output. |
| **EXEC-012 — While** | Base While is post-test and continues while condition is true. The body must produce its Boolean condition and required outputs on each normal iteration. “Stop if true” requires explicit semantic normalization or a published mode. Possible non-termination is not by itself invalid source. |
| **EXEC-013 — compile-time exclusion** | Only explicit compile-time selection can exclude regions/dependencies. Hidden ordinary branches are not excluded. Resolve the actual published selection policy and context, including symbol errors, ordering and fallback. Do not invent first-match or exclusivity when the selected profile does not define it. Retained inactive source must remain preservable. |
| **EXEC-014 — extended families and modes** | Event, Sequence, Timed, In Place, type-specialized or parallel constructs and loop auto-indexing/conditional collection/reduction require their applicable published contracts and implementation support. Serialization or an editor implementation alone does not establish execution support. |

For EXEC-014 the minimum family obligations are:

- Event: event/source registration, payload identity, region-local data, filtering
  and timeout policy; all normally terminating cases produce their shared outputs.
  An optional timeout is not a missing mandatory selector.
- Sequence: legal frame order, explicit inter-frame values, no read-before-produce,
  and complete final outputs; empty frames may be valid.
- Timed: clock, period, offsets, units, deadline policy and target guarantees;
  a missed runtime deadline differs from a statically invalid schedule.
- In Place: matching boundaries, types/ranks/fields, reinsertion and resource
  lifetime/ownership; unchanged-value forwarding must belong to its contract.
- Parallel/type specialization: explicit merge/arbitration or candidate selection,
  supported capabilities, and no unexplained ambiguity.
- Extended loop output modes: exact element type/rank, empty collection/reduction
  result, termination point and output participation of the final iteration.
  Base last_value does not implicitly authorize these modes.

## 5. State, calls and runtime limits

- **EXEC-015 — memory and cycles.** Validate initialization, type, state scope,
  lifetime/reset and next-state production. Only a contract-recognized temporal
  dependency breaks an instantaneous cycle. The instantaneous dependency graph
  must be acyclic after removing those temporal dependencies. A component
  containing Delay may still contain an illegal combinational sub-cycle.
  Arbitrary stateful I/O or a drawn loop frame is not temporal memory.
- **EXEC-016 — interfaces and dependencies.** Resolve active static calls,
  signatures, required inputs, outputs and dependency versions transitively.
  An unrelated unreferenced broken program need not block the requested entry.
  A project-wide build may intentionally validate more. Dynamic loading must use
  its own declared runtime contract, not pretend to be a resolved static call.
- **EXEC-017 — widget participation.** Keep public interface, widget value,
  reference/property access, event binding and graphical realization distinct.
  Missing semantic references, incompatible roles/types and illegal writes are
  diagnosed. An unwired display indicator does not automatically produce a
  promised public output. A replaceable visual resource is not automatically a
  language failure.
- **EXEC-018 — target and artifact.** Verify actual providers, capabilities,
  lowering and launch preconditions. Missing support blocks the appropriate
  build/run stage without inventing a broken source wire. A backend must reject
  an incomplete derived representation rather than supply undefined values.
  Normalization/optimization must preserve explicit defaults, zero-iteration
  values, state and source attribution.
- **EXEC-019 — limits of static validation.** Validation is bounded and free of
  user-code/device side effects. It does not prove general termination, every
  dynamic array bound, resource availability or absence of runtime failure.
  Such outcomes must follow the node/runtime error and cancellation contracts.

## 6. Diagnostics — EXEC-020

Every blocking result MUST provide a stable code, human-readable reason, stage
blocked (validation/build/start), invalid-vs-unsupported classification and source
attribution where available. Structure diagnostics need their scope/region/output;
edge diagnostics need the endpoint or wire identity. Navigation must reveal hidden
regions without changing program meaning, bindings or compile-time selection.

Severity and blocking impact are independent. A warning may prevent launch
because a provider is absent; a non-blocking warning must not automatically break
semantic validity. Tools MUST NOT hide a blocking reason behind an optional
“show warnings” filter. Results must refresh with the document/configuration;
stale source locations must not navigate into unrelated new objects.

The [IDE diagnostics contract](../IDE/Execution%20diagnostics.md) describes the
Graiphic Studio presentation. Button appearance does not define language meaning.

## 7. Conformance and controlled evolution — EXEC-021

For each claimed family/profile/target, maintain this chain:

published rule → validator → localizable diagnostic → normalization contract →
positive test → negative test → mutation/persistence test where applicable →
dated result identifying implementation revision and target.

The [acceptance matrix](../docs/execution-eligibility-acceptance-matrix.md)
retains the 137 study scenarios as a qualification backlog, **not 137 passing
tests**. [Studio coverage](../docs/execution-eligibility-studio-coverage.md)
separates verified authoring checks from unsupported or unqualified public
language/backend behavior.

Changes must:

1. Retain these EXEC identifiers; do not recycle removed IDs.
2. State the changed rule, motivation, compatibility/migration effect and owner.
3. Update source/semantic/profile/IR contracts together when meaning changes.
4. Add positive, negative and edit/reload tests before claiming support.
5. Record actual evidence and remaining gaps; do not turn “not tested” into pass.
6. Increment this document's revision and add a change-history entry through the
   repository's normal review process.

### Change history

| Revision | Date | Change |
| --- | --- | --- |
| 1.0 | 2026-09-16 | Establish a stable admission reference, independent gates, recursive coverage, diagnostics and evidence governance; incorporate the supplied revision-2 validation study without asserting unverified implementation coverage. |
