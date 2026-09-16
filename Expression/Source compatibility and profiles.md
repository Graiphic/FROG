# Source compatibility and implementation profiles

Status: accepted convergence decisions, 2026-09-06. Authority: the FROG project.
Classification: clarification and implementation-safety requirements; no new
`spec_version` and no claim of universal implementation support.

This document connects [canonical source scope](Canonical%20source%20scope.md),
[typed bindings](Typed%20Binding%20Contract%20v1.md) and
[version governance](../Versioning/Readme.md). It does not replace their ownership.
Its acceptance does not retrofit readers that have already been distributed.

## One language, explicit implementation boundaries

FROG source remains independent of an IDE. Graiphic Studio's proven authoring
behaviour may motivate a public language improvement, but a product-private
serialization field is not automatically a new language rule. Conversely, a
publicly specified feature is not automatically implemented by Studio or Runtime.

| Concern | Authoritative owner | Consumers |
| --- | --- | --- |
| Source sections, identities and compatibility | Expression and Versioning | Readers, writers, editors, validators |
| Execution meaning, activation, state and cycles | Language | Semantic validators, FIR derivation, runtimes |
| Primitive ports and local behaviour | Versioned Libraries contracts | Studio, help, validation, lowering |
| Widget value, role and class capabilities | Widget class + instance TypeRef | Bindings, controls, indicators, Runtime |
| Structure boundary rules and shared type variables | Versioned structure contracts | Editor adapters, validation, lowering |
| Appearance, gestures and navigation | IDE/product contracts and realizations | Host UI; never wire compatibility |
| Backend availability | Target/provider capabilities | Build and Run readiness |

The public envelope is `spec_version`, `metadata`, `interface`, `diagram`, with
the optional sections declared by [the source schema](schema/frog.schema.json).
The writer uses string `"0.1"`; the current base reader also accepts numeric
`0.1`, as the existing schema requires. A front panel is optional. A graphical
connector is a projection of interface ports; it must not define or replace them.

Studio's `format: "frog.document.draft"` / `draft_revision` / `frontPanel`
envelope is a separately versioned legacy authoring format. Its `.frog` file
extension does not certify canonical source conformance. Support for that draft
must not be removed by merely replacing its keys with public names.

## Independent capability claims

A consumer must distinguish these capabilities for a declared version/profile:

1. **Read**: decode source without claiming its meaning is supported.
2. **Preserve**: retain unsupported source content without silent loss.
3. **Edit**: apply a supported transaction without corrupting retained content.
4. **Validate**: establish structural and semantic validity for a stated subset.
5. **Lower**: derive the supported execution-facing representation.
6. **Execute**: a target/provider implements that lowering and required policy.

Palette visibility, a recognized type, a passing JSON Schema check, or a saved
file establishes none of the later capabilities by itself. An unsupported version
must not be classified as semantically accepted or `unsupported_but_valid` unless
validity has actually been established. A reader may instead report
`unsupported_source` while retaining the original loaded artifact.

`frog.typed-binding@1` and `frog.case-editing@1` are bounded profiles, not synonyms
for all of FROG v0.1. They retain their explicit rollout gates. Non-Case structure
families must not pretend to consume the Case manifest simply because they share
an implementation engine.

## Reader and writer safety

- A source document is one UTF-8 JSON object. JSON member names must be unique
  within each object after escape decoding. A duplicate must not be silently
  resolved by keeping the first or last value. The same member name in different
  objects remains valid.
- Bare `NaN`, `Infinity` and `-Infinity` are not JSON numbers. They must not be
  accepted as a hidden transport extension. This says nothing about a later
  explicit typed representation of special floating-point values.
- A reader must preserve an exact JSON integer within its supported range or
  diagnose its limitation. It must not silently turn a finite JSON numeric token
  into infinity, or a nonzero token into zero through range underflow. Typed
  exact integer strings used by Case remain strings.
- Invalid UTF-8 and malformed input produce load diagnostics, not a partially
  accepted program. Implementation resource/numeric limits must be documented
  separately from language type availability.
- Source node, edge and port IDs are identity, not display labels. Interface port
  IDs are unique across inputs and outputs. Region-local IDs retain their scope.
- Unsupported executable fields must block unsafe import, export or execution.
  A reader that cannot preserve them may refuse import with a precise diagnostic;
  dropping unknown nodes and then dropping their incident edges is not migration.
- Unknown descriptive metadata may remain opaque and non-executable. A reader
  must not claim lossless editing merely because a few known properties survive.
- Preservable invalid authoring state, such as broken wires or Case draft text,
  is distinct from valid executable source. Saving a draft must not legalize it.

The Python reference reader currently keeps exact Python integers and finite
binary64 JSON fractions. Fractions beyond that reader's range are diagnosed as
`unsupported_json_number`, not redefined as invalid FROG numeric types.
Excessive nesting can be readable JSON yet exceed the reference validator's
copy/analysis capacity. It then yields `source_resource_limit` without a partial
validated program or modification of the loaded artifact.

## Decisions retained from both implementations

- Keep FROG's explicit interface, source/meaning/FIR separation, stable IDs,
  optional front panel and target-independent semantics.
- Keep Studio's transactional authoring, broken-wire preservation, typed binding
  resolver, enum domain identity, ordered cluster fields, array ranks and Case
  editing rules, where their public contracts and tests are already explicit.
- Preserve established public and persisted port IDs. For the published numeric
  unary operations, the port is `in`, not an alternative private `value` spelling.
- Keep loop count/index `i64` as the existing FROG contract specifies. Behavioural
  inspiration from LabVIEW does not import its integer-width choices.
- Conditional Disable permits at most one Default. Conditions select the first
  matching ordered alternative; Default is fallback. No match without Default is
  a diagnostic. It is not a run-time Boolean Case.
- Keep program identity (`metadata.name`) distinct from host window presentation
  (`host.front_panel_window` version 1). Studio's private property migration is
  not a silent amendment of the public host contract.
- Do not publish private scheduler priority, reentrancy or execution-system hints
  as portable language law without semantics, capability negotiation and tests.
- Keep `.wfrog` realization geometry separate from widget class value/port law.
  A locally rendered Waveform or Variant palette entry is not Runtime support.

## Safe migration gates

| Gate | Acceptance evidence required |
| --- | --- |
| G1 — safe legacy boundary | Existing drafts still load; unsupported public constructs produce diagnostics without data loss. |
| G2 — independent public model | Interface-only and no-front-panel programs are representable; nested scopes and unknown preservable fields survive. |
| G3 — canonical writer | Golden public read/write/read fixtures preserve all semantic facts; no synthetic interface derived solely from widgets. |
| G4 — declarative contract generation | Function/widget/structure consumers are generated from versioned authorities, or verified projections have full field parity. |
| G5 — target handoff | Supported profiles lower and execute under explicit target capabilities; unsupported profiles fail before execution. |

G1 is a safety prerequisite, not proof of G2–G5. Migration must be explicit and
transactional, preserve the original file until successful validation/save, and
offer diagnostics instead of silently rewriting unsupported source. No automatic
repository-wide conversion is authorized by this design decision.

## Change discipline

A source-visible change requires a bounded contract, version/profile decision,
schema impact review, accept/reject/preserve fixtures, consumer tests and migration
notes. Documentation is stored beside its owner and linked from the entry points.
Tests for a declared contract must run in CI; missing schema dependencies must
fail setup rather than produce a green build with skipped validation.

See the [convergence audit](../docs/language-studio-convergence-audit-2026-09-06.md)
for observed implementation gaps and evidence. That report is non-normative and
does not turn planned migration gates into delivered functionality.
