# Graiphic Studio and public FROG source compatibility

Documentation reconciliation, 13 November 2026. Start with the [canonical Source
guide](../Expression/Frog%20source%20guide.md) or the [Studio draft
profile](../Expression/Studio%20authoring%20document%20profile%20v1.md).
Use the [coverage matrix](#coverage-matrix) to identify the tested operation.

## Common Frontend and published checkpoints

The [common Frontend library and CLI](../Implementations/Frontend/README.md)
are available as source. They own reading, explicit dependency resolution, types,
semantic validation, FIR, diagnostics, maps and provenance. Studio pins the
software, API and content it uses.

- [12 November source checkpoint](frontend-checkpoint-2026-11-12.json)
  records the published content `c815f0a1…0df9a9`, Frontend `0.2.0-local`
  API 3 and Studio `0.0.8.094`. That publication reused qualification.
- [9 November checkpoint](frontend-checkpoint-2026-11-09.json)
  records the standalone build and 6/6 public CTests for `81a502cc…35278e`.
- [2 November checkpoint](frontend-checkpoint-2026-11-02.json) records the
  original source publication, the standalone kit and finite Studio validation.
  The 1 November documentation-only stage is historical.

The checkpoint versions refer to source publications. Tagged binaries,
whole-palette qualification and clean-machine support have separate evidence.

Studio retains historical capture/migration, diagnostic presentation, repair
proposals, transaction preview/application and Undo/Redo. The common Frontend
supports finite Type Definition resolution and explicit captured Source
dependencies in specific profiles. General dependency resolution and arbitrary
ConditionalDisable contexts remain outside the qualified matrix. Refer to the
exact operation result and its provenance for the accepted dependency kinds.

See [the tool guide](frontend-tools-guide.md) and
[operation contract](../sdk/ecosystem/docs/frontend-api.md).

## Stewardship and authority

Graiphic leads FROG's stewardship and develops Graiphic Studio. The public
[governance](../GOVERNANCE.md) remains the decision authority for changes to the
language. Studio is the authority for which authoring operations have actually
been qualified on its files. Its common Frontend owns semantic validation.
Accepted portable behavior is specified
in the public contracts so that other editors, generators, validators and runtimes
can implement it independently.

A product regression or convenient serialization field does not silently amend
the language. Conversely, a public feature is not implemented merely because it
has been specified. Differences must have an explicit owner, profile/migration
decision, diagnostics and qualification evidence.

## Two envelopes currently exist

| Surface | Public source | Studio authoring document |
| --- | --- | --- |
| Envelope | `spec_version: "0.1"` | `format: "frog.document.draft"`; reader 0–9, ordinary current writer 3–8, recovery writer 9 |
| Required sections | `metadata`, `interface`, `diagram` | Ordinary writer emits `metadata`, `frontPanel`, `diagram`. Independent `interface` is optional. Archive-only recovery can omit panel and diagram. |
| Front Panel spelling | Optional `front_panel` | `frontPanel` |
| Public interface | Independent of widgets | Retained independent declarations when present; historical widget-derived projection remains a compatibility path |
| Writer | Public source preservation/transaction writer | Studio editing-model serializer |
| Claim | Bounded source/profile contracts | Product authoring support; no automatic public conformance claim |

Studio's independent public-source reader retains the source tree, exact numeric
tokens and original UTF-8. Its public writer refuses mixed/private envelopes or
unsupported versions. Its editing-model importer has narrower support. These are
different code paths, so a successful source read does not promise full canvas
editing, semantic validation, export migration or execution.

The [Studio numeric array storage profile](studio-array-storage-v1.md)
documents the three modes and private records. The current producer selects
Automatic for new documents and for input without Storage. An explicit Readable
selection remains Readable. Compact draft storage belongs to the Studio envelope.

The [14 October reconciliation](studio-reconciliation-2026-10-14.md) remains a
historical record. Private Run, public native Run and captured Event execution
have separate target and provider coverage.

The [Studio document profile](../Expression/Studio%20authoring%20document%20profile%20v1.md)
describes historical input, current writer output and recovery. Its
[structural schema](../Expression/Schemas/frog.studio-document.schema.json) and
[authoring contract snapshots](../Profiles/StudioAuthoring.md) are public
compatibility documentation; they do not promote an editing projection or
private provider to universal canonical execution.

## Coverage matrix

| Format and profile | GS / Frontend identity | Established coverage and boundary |
| --- | --- | --- |
| Canonical Source 0.1, finite scalar and captured/profile cases | GS 0.0.6.836, API 3, `b6123302…c70f8cd` | Dated kit, installed C consumer, CLI/Studio parity and finite Run/Highlight/Build evidence. Use the 2 November receipt for the scope. |
| Canonical Source 0.1, scalar and Type Definition fragments | API 3, `c815f0a1…0df9a9` | Local CLI checks validate and derive the named fixtures. Source/FIR 0.1 and frontend dependency contracts have separate versions. A CLI derive makes no execution claim. |
| Draft scalar, Array/Cluster, Case, For and Sequence | GS 0.0.8.092/.094, published API 3 checkpoint on .094 | Finite product corpus includes actual authoring, Save/Open and observed outputs of the qualified exported executables. Use the per-program receipt for execution and version attribution. |
| Draft 2, Map/Set and Variant | GS 0.0.7.284 for Map/Set and 0.0.7.656 for Variant | Historical native Save/Open and Run cover typed Map/Set controls and indicators and a Variant containing a Cluster. An incomplete Map is saveable while waiting for its type. The two saved files pass the draft structural schema. |
| Historical draft 0/1 and readable 2 | Studio recovery and common reader | Historical admission and model projection only where supported. The migration and recovery checks preserve original bytes, known edits, wires and explicit refusals. Revision 2 is still accepted as input. |
| Draft 3, numeric Array storage | GS 0.0.8.190, pin `8a675af7…ea6d038` | Edit, Undo/Redo, Save and Open in Automatic and Compact standalone at 10,000,001 I32. Run with expected length and values was qualified at 1,500,001 I32. The larger file round trip establishes persistence. |
| Draft 4/5, exact integer and complex lexical state | Shared writer and structural fixture owner | Conditional revision and lossless lexemes checked. Unsupported complex carriers preserve source and refuse native execution in the recovery fixture. |
| Draft 6/7, symbolic objects and configured external calls | Shared writer, API 3, GS 0.0.7.750 for the scalar DLL case | Authoring and structural checks for Application Control. The revision 7 `gs_add` fixture reopened and produced 5 in Studio and the exported EXE. Migration and execution require a supported signature and provider. |
| Draft 8, linked standard Type Definition | GS 0.0.6.052 historical delivery, API 3 `c815f0a1…0df9a9` CLI, Studio publication fixtures | Historical demonstration ran after Save/Open. The CLI validates `Acquisition.frog` with the exact captured `.ftype` and reports the missing resolver without it. The Studio replay verifies stable IDs and distinct current/default values across publication and reopening. TD-20 Enum/Case resolution failed. |
| Draft 9, explicit recovery | GS 0.0.7.662 recovery fixture and Studio recovery model checks | Projection and archive-only shapes, exact origin preservation and diagnostic revalidation. The known repaired cosine program ran with 10,000 values per Run. An unresolved archive has no runnable graph. |

The CTests used for this reconciliation were already-built component binaries.
Their individual results do not qualify every path on the installed Studio
version. The published checkpoint and the local product identity are tracked
separately.

### Checked-in version baseline

| File | Format and profile | GS Save/Open | Public CI |
| --- | --- | --- | --- |
| Scalar addition and Front Panel | Source `spec_version: "0.1"` | Signed GS 0.0.8.215; Source edit, Save and fresh Open | Sealed bytes, canonical schema, semantic validation and FIR provenance |
| Variant containing a Cluster | Studio draft 2 | 0.0.7.656, T310 UI bench; typed Cluster round trip | Sealed bytes, Studio schema, common Frontend validation |
| Compact Standalone numeric Array | Studio draft 3 | 0.0.8.190, native benchmark; Save/Open of 10,000,001 I32 elements | Sealed bytes and Studio schema. The local Frontend rejected the execution projection at its JSON representation budget. |

See the [files and manifest](../Conformance/studio-save-open/Readme.md)
and the [version advancement criteria](../Versioning/Readme.md#advancing-a-source-format-with-gs-evidence).
The checked-in fixtures supplement the broader historical evidence above.
The workflow builds the common Frontend from the checkout. The Studio
versions in the table identify the native Save/Open evidence.

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
