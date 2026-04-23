# FROG Version Matrix

Centralized current-status table for the published FROG specification corpus
FROG — Free Open Graphical Language

* * *

## Contents

- [1. Overview](#1-overview)
- [2. How to Read This Matrix](#2-how-to-read-this-matrix)
- [3. Current Repository-Wide Values](#3-current-repository-wide-values)
- [4. Current Surface Matrix](#4-current-surface-matrix)
- [5. Version Status Legend](#5-version-status-legend)
- [6. Cross-Version Handling Summary](#6-cross-version-handling-summary)
- [7. Cumulative Version Model Summary](#7-cumulative-version-model-summary)
- [8. Maintenance Rule](#8-maintenance-rule)
- [9. Summary](#9-summary)

* * *

## 1. Overview

This document is the centralized detailed status table for the current published FROG specification corpus.

It complements `Versioning/Readme.md` by giving a compact matrix view of:

- the current repository-wide version posture,
- the current scope and status of each major repository surface,
- the difference between stabilized, bounded, draft, deferred, and framing areas,
- the current next closure target where that is useful to state explicitly.

This matrix is a governance and visibility surface.
It does not replace the normative ownership of the technical documents themselves.

* * *

## 2. How to Read This Matrix

Each row should be interpreted as the current repository-visible posture of one surface, not as a promise that every detail inside that surface is equally complete.

The main columns are:

- **Surface** — repository area or cross-cutting closure surface,
- **Current scope version** — current declared bounded version scope for that surface,
- **Status** — stabilization posture for that surface,
- **Primary role** — what architectural responsibility that surface owns,
- **Current posture** — compact description of what is currently true,
- **Next coherent closure step** — the smallest next step that would improve coherence.

This matrix should be read together with:

- `Versioning/Readme.md` for doctrine and transition policy,
- `Readme.md` for repository-wide architecture framing,
- the owning technical surfaces for actual normative law.

* * *

## 3. Current Repository-Wide Values

| Field | Current value | Notes |
|---|---|---|
| Specification corpus version | `0.1-draft` | Current repository-wide published posture under consolidation |
| Reference source-format target | `.frog spec_version = 0.1` | Bounded source compatibility target for the current published subset |
| Current repository-wide status | Bounded published closure with frozen Example 05 reference slice under consolidation | The repository already exposes a real multi-layer corridor, with Example 05 now treated as the frozen bounded executable anchor |
| Versioning doctrine | Additive by default, explicit degraded readability, preservable when safe | Governed centrally in `Versioning/Readme.md` |
| Cumulative version model | Later versions extend earlier valid forms by default | Later versions should normally be read as earlier valid forms plus bounded additions unless an explicit breaking boundary is declared |

* * *

## 4. Current Surface Matrix

| Surface | Current scope version | Status | Primary role | Current posture | Next coherent closure step |
|---|---|---|---|---|---|
| `Readme.md` (root) | `0.1-draft` | Published repository entry surface, broadly aligned | Repository-level entry point and top-level architecture framing | Describes the current multi-layer repository posture, including the frozen Example 05 reference slice, shared acceptance, and the bounded runtime/compiler-family split | Keep root framing synchronized with the actual published Example 05 freeze posture, runtime-family acceptance, and widget baseline growth |
| `Expression/` | `0.1` | Normative, bounded published scope | Canonical source representation and source-visible boundaries | Owns canonical source shape, source sections, source-schema posture, and structural validity for the currently published subset | Keep source extensibility and source-version guidance explicitly compatible with centralized version doctrine |
| `Language/` | `0.1` | Normative, bounded published scope | Validated meaning of accepted programs | Provides semantic law for the currently published subset, especially around control, state, and execution-facing meaning boundaries | Continue tightening semantics only where needed by currently published executable and widget corridors |
| `IR/` | `0.1` | Normative, bounded published scope | Execution-facing normalized representation and downstream corridor staging | Already carries Execution IR, derivation, identity and mapping, schema posture, lowering, and backend contract material | Keep open IR posture tightly aligned with bounded backend-family and reference-path claims |
| `Libraries/` | `0.1` | Normative, bounded published scope | Intrinsic primitive catalog law | Owns primitive namespaces and primitive-local semantics for the bounded published subset | Keep intrinsic-library scope clearly separated from profile-owned capability growth |
| `Profiles/` | `0.1-draft` | Published draft with bounded high-value closure fronts | Optional capability-family and profile-level closure | Publishes meaningful bounded corridors such as Native CPU LLVM and widget-related profile modeling, but remains an active growth front | Stabilize the currently published high-value corridors before widening to additional profile families |
| `IDE/` | `0.1-draft` | Published draft architecture surface | Authoring, observability, inspection, debugging, Program Model | Published and already architecturally meaningful, but not yet a fully frozen ecosystem-wide law surface | Keep IDE-facing boundaries explicit without letting one future IDE behavior become hidden semantic law |
| `Examples/` | `0.1` | Published, bounded, aligned support surface | Illustrative named slices | Provides bounded illustrative mirrors, including the frozen Example 05 executable anchor and the compiler-oriented family under `Examples/compiler/` | Maintain strict alignment with conformance, shared acceptance, and the bounded reference consumption path |
| `Conformance/` | `0.1` | Published, bounded, high-priority public truth surface | Public accept / reject / preserve expectations | Already acts as a major repository-visible truth surface and is now expected to preserve the frozen Example 05 corridor without silent drift | Keep corridor mirroring, family growth, and published acceptance claims aligned with the real repository state |
| `Implementations/Reference/` | `0.1-draft` | Published non-normative bounded executable workspace with frozen Example 05 reference slice | Reference path exercise without normative ownership | Exercises a real source-to-execution reference path for a controlled published subset while remaining explicitly downstream from the specification | Protect the frozen Example 05 corridor through automation and non-regression gates while opening the next widget fronts upstream |
| `Roadmap/` | n/a | Published non-normative framing | Closure sequencing and milestone posture | Helps sequence work but does not define current version truth | Keep roadmap intent distinct from centralized version-state reporting |
| `Strategy/` | n/a | Published non-normative framing | Rationale, positioning, and broader justification | Explains why FROG matters but does not define current version truth | Keep strategy language distinct from normative version governance |
| `Versioning/Readme.md` | `0.1-draft` | Published centralized governance surface | Specification-version governance entry point | Centralizes doctrine, current corpus version, source-version distinction, cumulative version model, and transition logic | Keep doctrine, current corpus values, and cross-version policy aligned with the real published corridor posture |
| `Versioning/Matrix.md` | `0.1-draft` | Published centralized status matrix | Detailed current-state matrix for repository surfaces | Centralizes the current per-surface status table and complements the main versioning governance document | Keep the matrix synchronized with the actual published state of each major surface and with the freeze posture of the current bounded reference slice |
| Bounded executable corridor | `0.1` | Published bounded closure with frozen Example 05 reference slice | Repository-visible source-to-execution corridor for a first controlled subset | Example 05 now acts as the frozen bounded source-to-runtime corridor across source, FIR, lowering, backend contract emission, shared runtime-family acceptance, Python, Rust, C/C++, and first LLVM proof | Preserve the frozen slice and grow additional widget and example fronts around it rather than reopening the current corridor |
| Widget-object corridor | `0.1-draft` | Published bounded closure with frozen numeric seed and active growth | Source-visible widget declaration, interaction, class contract, and profile-level class modeling | Already serious and repository-visible, with the current numeric control/indicator pair now acting as the frozen seed for broader widget closure | Extend the widget baseline from the frozen numeric seed while preserving ownership and object-surface distinctions across Expression, Libraries, Profiles, and IDE-facing documents |
| Additive cross-version doctrine | `0.1-draft` | Published centralized governance doctrine | Repository-wide rule for safe forward evolution and degraded reading | Explicitly published and already part of centralized version law | Keep source-version guidance and safe degraded-handling expectations aligned as later versions are introduced |
| Cumulative version model | `0.1-draft` | Published centralized governance doctrine | Repository-wide interpretation of later versions as bounded extensions of earlier valid forms | Explicitly part of centralized versioning governance and intended to shape later specification evolution | Keep compatibility guidance and change classification aligned with this model as the corpus matures |

* * *

## 5. Version Status Legend

| Status label | Meaning |
|---|---|
| Normative, bounded published scope | Published and binding for the currently declared bounded subset |
| Published, bounded, aligned support surface | Published support area aligned with the bounded published corridor, but not itself the sole source of normative law |
| Published non-normative bounded executable workspace with frozen Example 05 reference slice | Published and useful, explicitly downstream, and currently anchored on the frozen bounded Example 05 corridor |
| Published draft architecture surface | Published and architecturally meaningful, but not yet treated as fully stabilized repository-wide closure law |
| Published draft with bounded high-value closure fronts | Published and already partially closed around important corridors, but still an active stabilization front |
| Published centralized governance surface | Published repository-level governance surface that centralizes doctrine and current corpus-version posture without taking over technical ownership |
| Published centralized status matrix | Published repository-level current-status table that centralizes visibility without replacing technical ownership |
| Published centralized governance doctrine | Published repository-wide rule that governs interpretation across surfaces rather than one isolated technical layer |
| Published repository entry surface, broadly aligned | Published top-level architectural entry point that must remain synchronized with the detailed repository state |
| Published non-normative framing | Published rationale or sequencing surface that must remain distinct from specification law |
| Published bounded closure with frozen Example 05 reference slice | Published bounded corridor that should now be preserved as a stable repository-visible anchor while adjacent surfaces continue to grow |

* * *

## 6. Cross-Version Handling Summary

The current repository-wide versioning doctrine is:

```text
open if possible
inspect what is known
preserve what is unknown when safe
refuse unsafe semantic or executable claims
never silently misinterpret
```

This doctrine applies across the matrix as a repository-wide governance rule.
Detailed policy remains centralized in `Versioning/Readme.md`.

* * *

## 7. Cumulative Version Model Summary

The current repository-wide cumulative version model is:

```text
later version
    =
earlier valid forms
    +
explicit bounded additions
```

This means that later specification evolution should normally preserve earlier valid forms and extend them, rather than silently redefine them.

This cumulative model does not remove the need for degraded-reading and preservation-aware tooling behavior.
It complements that behavior:

- the cumulative model governs how the specification evolves,
- the degraded-reading model governs how older-capability tools handle newer artifacts they do not fully support.

* * *

## 8. Maintenance Rule

This matrix should be updated whenever one of the following changes:

- the declared specification corpus version,
- the bounded published source target,
- the stabilization status of a major repository surface,
- the declared next closure target of a major repository surface,
- the repository adds or removes a major top-level specification or support surface,
- the centralized cross-version doctrine materially changes,
- the freeze posture of the bounded executable reference corridor changes.

This file should remain compact, current, and centralized.
Other documents should reference it rather than re-creating competing status tables.

* * *

## 9. Summary

This file is the central detailed version-status table for the published FROG specification corpus.

It should answer, in one place:

- what the current repository-wide specification version is,
- which surfaces are currently bounded and stabilized,
- which surfaces remain draft or framing-only,
- which cross-cutting corridors are already real,
- which centralized cross-version doctrines are now explicitly in force,
- what is currently treated as the frozen bounded executable reference slice,
- and what the next smallest coherent closure steps are.

Its role is to make the current repository posture readable without scattering competing status statements across the corpus.
