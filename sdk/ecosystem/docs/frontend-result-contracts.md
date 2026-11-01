# API-3 Frontend result documents

**Contract publication only — 1 November 2026.** These are the result data families
of the common Frontend API-3 checkpoint. No executable is included. Their versions
are independent of the C API and Source/FIR.

| Result view | Document/version | Schema |
| --- | --- | --- |
| `diagnostics_json` | Array of `frog.diagnostic` `0.2`; apply schema to each item | [diagnostic-0.2](../schemas/diagnostic-0.2.schema.json) |
| `provenance_json` | `frog.frontend.context` `0.3` | [frontend-context-0.3](../schemas/frontend-context-0.3.schema.json) |
| `descriptor_json` | `frog.frontend.capabilities` `0.2` | [frontend-capabilities-0.2](../schemas/frontend-capabilities-0.2.schema.json) |
| `source_maps_json` | `frog.frontend.source_maps` `0.1` | [frontend-source-maps](../schemas/frontend-source-maps.schema.json) |
| `fir_json` | Complete `frog.execution_ir` `0.1`, only successful `derive` | [existing FIR schemas](../../../IR/schema/README.md) |
| `resolved_source_json` | Complete resolved Source, only successful validate/derive when projection changed the bytes | [Source guide](../../../Expression/Frog%20source%20guide.md) |

Do not validate API-3 provenance with the old experimental provenance 0.2 schema.
Its `fir.document_id` is independent of `source.document_id`. No older closed
schema is silently reinterpreted. Diagnostic/descriptor version 0.2 does not mean C API 2.

## Diagnostics and incomplete assessments

Each diagnostic carries stable code, stage, severity, category, message and source
attribution. Frontend stages are `read`, `resolve`, `validate`, `derive`; categories
are format, compatibility, structure, interface, integrity, semantics, dependency,
frontend-capability and control. Target/backend/runtime categories are later gates,
not values to force into this Frontend-only schema.

Source URI/document ID/JSON Pointer are preserved, plus known object/port/connection/
region identities. Empty pointer is legal when location is unknown. `derived_pointer`
can identify FIR without pretending it is authored. Related locations are supplied
when known, not invented. Capability refusal may carry its stable capability ID.
Message text and diagnostic order are not identities.

Every result contains a diagnostic array, possibly `[]`. Informational cancellation
still blocks successful admission. Use outcome and stage/category as well as severity;
`REJECTED` after unresolved dependency does not prove a negative semantic verdict.
API failure has no result view.

## Provenance and resolved Source

Provenance preserves authored URI/ID/hash, independent requested FIR identity,
Frontend software/API/content identity, requested formats, admitted profiles and
operations, dependency identities/versions/hashes, semantic context, applied limits
and transformations. Before capture a hash can be empty. `closure_complete` states
whether resolution completed; incomplete closure is not success. Profiles/operations
describe admitted FIR, not the entire catalogue.

Resolution keeps authored bytes unchanged. Its complete detached Source can be
returned in `resolved_source_json`; `projected_source_sha256` identifies those bytes.
Transformations retain authored and dependency pointers. On refusal or insufficient
result budget, no partial FIR/resolved Source is returned.

## Source maps and capabilities

Maps reuse the existing [FIR source-map](../../../IR/schema/frog.execution-ir.source-map.schema.json)
and [correspondence](../../../IR/schema/frog.execution-ir.correspondence.schema.json)
contracts. Authored hash is bare hex or empty before capture; `fir_source_sha256`
can be bare hex or `sha256:` plus hex. Successful validation can return maps while
withholding FIR. A refusal may return only available attribution/schema metadata;
a map is not execution evidence.

The descriptor identifies exact software/API/formats and effective/default limits.
Its profile list may be nonexhaustive. `native_execution_claim: false` distinguishes
language admission from native capability. `dissemination` records implementation
state, not whether this schema has been published.

## Strict consumption

Load schemas locally by `$id` and resolve FIR references from the repository.
Duplicate JSON members, non-finite tokens, malformed UTF-8 and isolated surrogates
need strict reader checks beyond JSON Schema. Keep I64/U64 and uint64 budgets exact;
do not round every JSON number through binary64.

The [example index](../fixtures/README.md) maps each synthetic file to its schema.
Examples are shape illustrations, not execution receipts. This update supplies
no validator script, library, header or CLI implementation.
