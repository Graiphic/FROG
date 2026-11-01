# Common FROG Frontend: operation contract, API 3

**Public documentation contract, 1 November 2026. Implementation code and binaries
are deferred.** The local common C++ owner already serves Studio and its C/CLI
facades; publishing this contract does not ship that owner. The accepted direction
is an autonomous open-source library and CLI sharing that same implementation.
The initial Studio integration uses a tested static-library pin.

## Responsibilities and operations

The core reads exact public Source, resolves dependencies through explicit host
services, checks structure and meaning, and derives FIR with diagnostics, maps and
provenance. It requires neither Studio/Canvas nor LLVM/private Runtime. It does not
execute source, load user DLLs, search files implicitly or apply editor repairs.

| Operation | Input and successful output |
| --- | --- |
| `describe` | No source/resolver. Descriptor of exact software/API/formats/profiles/limits. |
| `validate` | Immutable request. Acceptance plus diagnostics, provenance, available correspondences and optional resolved Source; no FIR or descriptor. |
| `derive` | Same semantic admission. Also complete FIR and correspondences after success; no partial executable result on refusal. |

`validate` and `derive` must use the same semantic rules. The current production
owner performs derivation/sealing checks for both and withholds FIR for `validate`.
Validation is not a promise of cheap parsing only. Frontend success does not
establish backend support, compilation, execution or permission for external effects.

## Request fields

These are logical/C API fields, **not a new JSON RPC or HTTP request format**.
The published JSON Schemas describe JSON subdocuments and data examples.

| Field/service | Meaning |
| --- | --- |
| `source_bytes` | Exact, immutable, length-delimited UTF-8 Source bytes. |
| `source_uri`, `source_document_id` | Explicit source location and technical identity. URI is not an instruction to fetch. |
| `source_format_id`, `source_format_version` | Exact declared Source family/version, checked against the envelope. |
| `fir_format_version`, `fir_document_id` | Requested FIR format and independently selected output identity, including on `validate`. |
| `semantic_context_json` | Explicit JSON object. Empty span means `{}`; no inherited OS/environment/Studio settings. |
| `resolver` | Paired resolve/release callbacks, or neither. Supplies immutable dependencies, never a second generator. |
| `is_cancelled`, `cancel_user` | Optional cooperative request-local callback and host state. |
| Context configuration `limits` | Nonnegative caller ceilings. Zero selects an advertised default, not unlimited work. |

Input views remain valid until the synchronous operation returns. Results own
read-only views independently of context lifetime until destroyed. One context
permits one operation at a time; overlap is `BUSY`. Independent contexts do not
share mutable request state. API structure sizes/version/reserved fields are
checked exactly. API 3 separates Source and FIR IDs and exposes
`resolved_source_json`; earlier experimental layouts are not compatible prefixes.
The C header is not part of this contracts-only publication. No private C++ object
ABI is made public by this table.

## API status and operation outcome

API status describes the call: `OK`, `INVALID_ARGUMENT`, `UNSUPPORTED_API`, `BUSY`,
`OUT_OF_MEMORY`, `INTERNAL_ERROR`. A non-OK call has no consumable result.

An OK call returns an operation outcome:

| Outcome | Interpretation |
| --- | --- |
| `SUCCESS` | The operation completed and its requested Frontend surface was admitted. |
| `REJECTED` | The request/program cannot proceed; inspect stage/category/code before classifying its cause. |
| `UNSUPPORTED` | A version/profile/feature is outside this Frontend's surface. This does not prove invalid FROG. |
| `CANCELLED` | Work stopped cooperatively. No completed semantic verdict is fabricated. |
| `LIMIT_EXCEEDED` | The effective resource/work/result budget was exceeded. This is not a corruption verdict. |

`REJECTED` alone is not proof of invalid source meaning: dependency unavailability
can refuse an operation before a complete semantic assessment. Diagnostics preserve
the distinction. Only a completed semantic check can establish invalid meaning.
Target-capability refusal belongs downstream and cannot relabel valid Source.

## Explicit semantic context

The bounded [context schema](../schemas/frontend-semantic-context.schema.json)
accepts `{}` or profile `frog.frontend.resolution@1` with exact dependency bindings.
Each binding names kind `2` and a JSON Pointer to an authored type-reference object
in an actual eligible `studio_type_schema` location, including eligible node ports
and widgets. This is profile-scoped, not a new intrinsic core field. Bindings may
not overlap or overwrite arbitrary metadata. Pointer existence, eligibility and
overlap are semantic checks beyond JSON Schema.

A [type reference](../schemas/frontend-type-reference.schema.json) retains `$ref`,
identity, version, SHA-256 and an explicit `/definitions/...` pointer. The resolver
supplies a [type-definition envelope](../schemas/frontend-type-definitions.schema.json).
Expansion produces a detached Source, exact hashes and transformation attribution.
Cycles, contradictory identities/URIs/hashes and missing fragments are refused.

General nonempty ConditionalDisable contexts are not supported at this public
checkpoint; they are explicitly refused rather than ignored. A future profile
must specify selection symbols/order/fallback and version its contract. Empty
context never borrows Windows symbols. Studio historical projection remains
private and is not evidence of broader public-context support.

## Resolver and dependency lifetime

A request carries dependency kind, requesting URI/source pointer, exact authored
reference and expected identity/version/hash. Kinds are Source `1`, Type Definition
`2`, library contract `3`, profile contract `4`; current resolution admits only `2`.
The host returns `RESOLVED`, `NOT_FOUND`, `ACCESS_DENIED`, `RESOLVE_CANCELLED` or
`RESOLVE_ERROR`. Success supplies stable URI, identity, version, bytes and optional
opaque lease. Metadata cannot override the dependency's own meaning.

The core computes SHA-256 over captured bytes and verifies the authored expectation.
A dependency cannot change during the operation. Every normally returned resolve
call is paired with one release, on all statuses, before the enclosing operation
returns. Exceptions cannot cross the ABI. Cancellation releases acquired leases.
Hosts control disk/network access explicitly; core resolution has no implicit
disk, registry, PATH, CWD or network fallback.

The [CLI dependency lock](../schemas/frontend-dependencies.schema.json) describes
host-side file selection: relative paths are based on the explicitly selected lock
file. The core receives captured bytes. A schema-valid kind 1/3/4 lock entry is
still unsupported by the current resolution profile.

## Limits and cancellation

Configuration names `max_source_bytes`, `max_dependency_bytes`,
`max_total_dependency_bytes`, `max_result_bytes`, `max_nodes`, `max_connections`,
`max_nesting_depth`, `max_dependencies` and `max_diagnostics`. Byte limits count
UTF-8 bytes; other limits count named objects/work. Implementations report default,
effective and hard ceilings; profiles may impose smaller ceilings. Zero at input
selects the documented default. Effective ceilings are positive integers. A ceiling
does not promise that every input of that size is realizable.

The cancel callback returns 0 to continue or 1 to cancel. Other values are callback
failures. Cancellation is cooperative between bounded work units. A blocking host
resolver must support its own cancellation; no universal latency guarantee follows.
A cancelled or failed operation returns no executable prefix, mutates no authored
document and must not replace a previous valid output file. This cancellation is
distinct from stopping an already executing program.

## Result payloads and version axes

See [the current result contracts](frontend-result-contracts.md). Source/FIR formats
remain `0.1`; software checkpoint `0.1.0-local` and API `3` are independent. Diagnostic
and descriptor documents use `0.2`, provenance `0.3`, source maps `0.1`.

Planned CLI operations are `describe`, `validate` and `derive`; its host selects
source/context/lock files and outputs explicitly. No CLI binary, compiler, private
Runtime or code release is supplied by this update. Studio repair UI, historical
adapters and backend optimization recipes remain outside this common boundary.
