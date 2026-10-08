# Engineering lifecycle and IEC 61499 documentation update

**Date:** 8 October 2026. **Scope:** documentation lot A, non-normative lot B
proposal and an informative IEC 61499 evaluation. Publication was explicitly
authorized on 8 October; the earlier prepared changes had remained local under
the previous instruction not to publish without authorization.

## Starting state

Public `main` and the working baseline were both
`b890993ae999a6f2916657ba4eeb4d9a08515934`. The GitHub connector confirmed that
public revision before this update. Existing lot A wording and the associated
traceability proposal were preserved. Their generic industrial-bridge paragraphs
did not explicitly cover IEC 61499; that omission has been addressed.

An unrelated pre-existing change in `docs/execution-eligibility-studio-coverage.md`
is excluded from this publication and retained locally. No implementation or
private repository change forms part of this delivery.

## Files and change classification

| Document | Change and status |
| --- | --- |
| [Architecture](../FROG-Architecture.md) | Clarifies the three concerns, industrial bridge families, optional engineering links, evidence and execution boundaries. Preserves historical anchors. |
| [README](../Readme.md) | Adds visible entry points for the digital thread and IEC 61499 evaluation. |
| [Strategy](../FROG-Strategy.md) and [strategy directory](../Strategy/Readme.md) | Non-normative rationale and IEC 61499 reading path, with no integration or partnership claim. |
| [Pipeline diagram](../ExecutionPipelineDiagram.md) | Adds lifecycle context beside the existing execution pipeline. Artifact references are distinct from executable wires. |
| [Repository guide](../FROG-Repository-Guide.md) | Connects the architectural, strategic and proposed documents to their existing owners. |
| [Project status](../FROG-Project-Status.md) | Distinguishes documentary clarification, proposals, implementation and evidence. |
| [Traceability proposal](proposals/engineering-traceability.md) | Lot B preparation only: associated document, identities/revisions, evidence, 15 proposed rules and 34 proposed cases. No normative format adopted or test result supplied. |
| [IEC 61499 evaluation](proposals/iec-61499-bridge.md) | Informative candidate paths, semantic review checklist, six proposed validation scenarios and explicit deferred work. No bridge implemented. |
| This report | Records scope, checks, preservation and open decisions. |

The existing Pages navigation tools were run. Their fixed section inventory does
not index `docs/proposals/` or this report. The new proposals are discoverable
through the explicit README, strategy, guide and status links, but are not added
to the site's generated global search inventory in this lot. Generated navigation
is left unchanged; a Windows-only Assets/assets regrouping was not included.

## Boundaries preserved

- Canonical source and Diagram remain authoritative; an external requirement is
  not an implicit executable input or scheduling decision.
- Loading, structural validation, editable representation, semantic acceptance,
  canonical FIR, lowering and backend realization remain separate.
- The public interface exists independently of an optional Front Panel.
- Associated lifecycle records are outside `.frog`; `frog.document.draft` is
  not promoted to the public `spec_version` format.
- Source/FIR mapping keeps its existing obligations. Optional lifecycle links,
  provenance, actual observations, requirement satisfaction and approval differ.
- Runtime, compilation and hybrid realization remain possible downstream paths;
  a bridge declares a subset and rejects unsupported content.

Normative schemas, standard primitives, execution semantics, profile contracts,
reference implementations, executable examples, versions, license, CLA and
governance are unchanged. No migration is introduced.

## Checks

The checks for this delivery are documentary. Independent review read the full
Digital Thread brief and reviewed lot A, lot B and the IEC 61499 additions against
the published ownership boundaries. Local references, fragments, preserved
anchors, duplicate identifiers, illustrative JSON and Markdown/HTML structure
were inspected. Final results:

- 10 documents checked: 134 local references, including 35 fragments; no missing
  target, path-case mismatch, missing fragment or duplicate explicit identifier.
- All 55 historical explicit `id`/`name` anchors and 93 historical static anchors
  including generated heading anchors are retained in the seven existing pages.
- `git diff --check`: passed; ordinary LF/CRLF conversion notices only.
- `pwsh -NoProfile -File .github/scripts/build-pages-navigation.ps1`: completed.
- `pwsh -NoProfile -File .github/scripts/verify-pages-navigation.ps1`: passed.
  The inventory retains 734 search paths and 20 root entries, with the discovery
  limitation described above.
- Independent contractual review: favorable; no confirmed boundary violation or
  implemented IEC compatibility claim.

IEC context is based on primary [IEC](https://webstore.iec.ch/en/publication/5506),
[Eclipse 4diac](https://eclipse.dev/4diac/doc/intro/iec61499.html) and
[UniversalAutomation.org](https://universalautomation.org/uao-technology/)
pages accessed on 8 October 2026. The public IEC description and the implementation
documentation were checked; the full licensed IEC standard was not audited.
All external HTTP links and browser rendering were not exhaustively tested.

No CTest, reference-runtime, industrial-target or traceability conformance tests
were executed for this documentation change. The proposed ETR cases and IEC
scenarios are definitions for later work, not passing test evidence.

## Deferred decisions

Lot B still needs adopted naming/versioning, grammar, schemas, relationships,
revision and digest rules, capability declarations, fixtures and a validator.
The IEC direction still needs a selected target and integration path, access and
rights review, semantic mapping, explicit bridge contract, implementation and
actual target tests. Timing, distributed behavior, safety and certification need
separate evidence. None is delivered by publishing these pages.
