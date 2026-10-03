# Graiphic Studio and public FROG source compatibility

Implementation checkpoint: 3 October 2026. This page describes integration status,
not a new source format or a conformance certification. The
[source guide](../Expression/Frog%20source%20guide.md) explains public `.frog` syntax.

## Stewardship and authority

Graiphic leads FROG's stewardship and develops Graiphic Studio. The public
[governance](../GOVERNANCE.md) remains the decision authority for changes to the
language. Studio is the lead authoring implementation and provides concrete
requirements and regression examples. Accepted portable behavior is specified
in the public contracts so that other editors, generators, validators and runtimes
can implement it independently.

A product regression or convenient serialization field does not silently amend
the language. Conversely, a public feature is not implemented merely because it
has been specified. Differences must have an explicit owner, profile/migration
decision, diagnostics and qualification evidence.

## Two envelopes currently exist

| Surface | Public source | Studio authoring document |
| --- | --- | --- |
| Envelope | `spec_version: "0.1"` | `format: "frog.document.draft"`; Readable revision 2, opt-in numeric storage revision 3 |
| Required public sections | `metadata`, `interface`, `diagram` | Studio-owned document model; not a substitute for the public envelope |
| Front Panel spelling | Optional `front_panel` | `frontPanel` |
| Public interface | Independent of widgets | Current authoring writer derives declarations from bound widgets |
| Writer | Public source preservation/transaction writer | Studio editing-model serializer |
| Claim | Bounded source/profile contracts | Product authoring support; no automatic public conformance claim |

Studio's independent public-source reader retains the source tree, exact numeric
tokens and original UTF-8. Its public writer refuses mixed/private envelopes or
unsupported versions. Its editing-model importer has narrower support. These are
different code paths, so a successful source read does not promise full canvas
editing, semantic validation, export migration or execution.

The opt-in [Studio numeric array storage profile](studio-array-storage-v1.md)
documents the three document choices and the exact private binary/base64 records.
It is implemented in Studio with functional qualification pending while tests are
paused. It does not change the public 0.1 schema or claim canonical compact source
interoperability. Readable remains the default; loading compact values currently
expands them into the existing editing model.

The compiled and signed desktop checkpoint is **0.0.4.235**. Studio `main` is
published at
[`36f56ba3e88c5b25d5632485ce35ab907e761011`](https://github.com/Graiphic/FROG-STUDIO/commit/36f56ba3e88c5b25d5632485ce35ab907e761011),
verified against the remote on 3 October 2026 at 08:43 UTC. It includes the
27-item sequential editor queue, the function-selection contour rule, recursive
container work, numeric array storage choices, Enum text sizing and atomic mixed
wire/terminal movement. Remote publication is distinct from CI qualification.
Runtime POC is disabled in this delivery. The last complete Studio regression
attempt failed on 0.0.3.935: 300/403 passed, 103 failed. Tests remain paused;
recent compilation is not a new successful functional gate.

## Integration rules for the ecosystem

- Preserve node, edge, field and port identities through layout changes. Recalculate
  geometry without replacing valid connections or letting sinks redefine sources.
- Transport complete types: Enum definition and representation, ordered cluster
  field IDs/types and array element schema/rank. Current array shape belongs to
  the value unless an extent is contractual.
- For Enums, a domain reference alone cannot hide a contradictory definition.
  Compare declared enumerator names, numeric values and order. Display-only
  item IDs, visibility and disabled state are not a replacement for that definition.
  Preserve stable item references used by Case matching.
- Preserve invalid authoring state for repair, while rejecting unsafe execution.
  An unresolved or incompatible wire must remain inspectable with a diagnostic.
- Use versioned library ports and structure boundaries. Creation from a wire
  cannot add a second source; an indicator may branch from the existing source.
- Keep editor gestures, icon size, fonts, color, selection and window animation
  out of type compatibility and scheduling. Product documentation owns these flows.

The [typed binding contract](../Expression/Typed%20Binding%20Contract%20v1.md)
owns type rules; [execution eligibility](../Language/Execution%20eligibility.md)
owns admission and diagnostics. This integration page does not add a new
shift-register lowering or runtime conversion policy.

## Gates still required

| Gate | Evidence required before claiming delivery |
| --- | --- |
| Public read/preserve/edit | Version/profile/resource bounds; unknown and invalid content retained or explicit refusal; transaction and cancellation fixtures. |
| Draft migration | Explicit migration with read/write/read fixtures preserving interfaces, regions, types, bindings and resources. No extension-only conversion. |
| Contract consumers | Full parity of versioned function/widget/structure manifests and generated or verified Studio projections. |
| Semantic validation | Positive and negative profile fixtures, stable diagnostics, scope/type/required-input checks. |
| Runtime handoff | Exact contract revision pin, supported target capabilities, state continuity and unsupported-feature rejection. |
| Publication | Reviewed commits integrated to the intended branch; CI status recorded separately from local build status. |

The September 6 [convergence audit](language-studio-convergence-audit-2026-09-06.md)
is historical evidence. The [execution coverage record](execution-eligibility-studio-coverage.md)
preserves its bounded qualification results. Neither is a current full-green gate.

Public FROG PRs [15](https://github.com/Graiphic/FROG/pull/15) and
[16](https://github.com/Graiphic/FROG/pull/16) were integrated through normal Git
merges and GitHub reports both as merged/closed. Their historical failing checks
remain failure evidence; main publication is not a claim that those checks passed.
Runtime retains the reviewed FROG SDK revision
`236dc72ddc68bc936d2519acc7651da125d6d19a`, now an ancestor of public main.
This immutable pin does not automatically adopt later contract changes.
Update it only with demonstrated compatibility.
