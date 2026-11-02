# FROG Frontend API 3

**P28 experimental local implementation; qualification is recorded separately.**
[`frontend.h`](../include/frog/frontend.h) specifies the C ABI implemented in
`Implementations/Frontend`. The software identity is `0.1.0-local`; this is not
a published release or, by itself, evidence of a compiled or qualified artifact.
Studio and the CLI consume the same C++ source-to-FIR implementation extracted
from the production frontend. There is no second generator or bridge-specific
interpretation of Source.

The former API 1 was a P01 declaration-only contract with no delivered binary.
API 2 added the explicit `semantic_context_json` request span; no API-2 binary
was delivered. API 3 deliberately separates the authored Source document ID
from the caller-selected FIR document ID and returns the exact resolved Source
when projection changed it. The request appends required `fir_document_id` and
the result view appends `resolved_source_json`. API-1/API-2 callers and structure
sizes are refused; recompile against this header and explicitly request API 3.
This experimental ABI change does
not change the separate Source/FIR format versions `0.1`, semantic profile
revisions `@1`, or the bridge protocol. Software, API, formats and profiles remain
independent version axes.

## Boundary and operations

The library reads exact Source bytes, resolves explicit dependencies through
the supplied resolver, validates language structure and semantics, and derives
FIR with diagnostics, source maps and provenance. It does not execute Source,
load native libraries, compile with LLVM, create a Runtime session or require
Studio, Canvas, a renderer, the Windows GUI or a checkout of private code.

| Operation | Successful result |
| --- | --- |
| `validate` | The source and closure satisfy the requested Source/FIR profiles and frontend semantics; no FIR is returned. |
| `derive` | The same admission as `validate`, plus complete canonical FIR and source-to-IR maps. |
| `describe` | The implementation's exact identity, supported format/profile versions, configured limits and API capabilities; no source or resolver is used. |

Validation cannot use a weaker language check than derivation. A memory or
output-budget failure during derivation can still prevent an artifact after
semantic validation. Backend capabilities remain a separate downstream gate:
frontend success neither proves native execution nor grants external effects.
Unknown operations/profiles, unresolved types, incompatible versions and
duplicate technical identities produce explicit refusals. The library must
never silently change profiles, guess another format, lower to a private
artifact or omit executable nodes to make a request succeed.

## ABI, lifetimes and concurrency

The ABI uses only C declarations, standard integer/size types, opaque handles
and explicit UTF-8 spans. No C++ containers, exceptions, private headers or
compiler-specific object layout cross it. API 3 uses the platform's ordinary
C ABI (`__cdecl` on Windows); it is not an IPC wire format or a cross-bitness
ABI. Shared-library export visibility may be supplied through
`FROG_FRONTEND_API`; static linking is the initial Studio integration.

Callers zero-initialize public structures and set `struct_size = sizeof(...)`.
API 3 requires an exact version, exact structure sizes and zero reserved
fields. Future values and sizes are rejected, never treated as compatible by
prefix. The API version returned by `frog_frontend_api_version` identifies the
ABI only; supported Source/FIR profiles must be obtained through `describe`.

- Inputs are caller-owned and immutable until the synchronous call returns.
  UTF-8 spans are length-delimited; no implicit NUL terminator is read.
  Null plus zero length denotes empty. A null pointer with nonzero length,
  embedded NUL or malformed UTF-8 is rejected.
- The config pointer is required at context creation, even when all limit
  fields are zero. Request identity spans are limited to 8,192 bytes each.
  The facade validates these small UTF-8 identities and pointer/length pairs;
  the common operation checks source/context budgets before scanning their
  UTF-8 content. No extra source-buffer copy is retained by the facade.
- Source bytes are not normalized, parsed through floating point or replaced
  by a display buffer. Exact I64/U64 literals, IEEE-relevant lexical policy,
  nominal type identities, enum identities and composite schemas retain the
  semantics of the production implementation.
- A context copies its config at creation. One operation may use a context at
  a time; overlapping or callback-reentrant operations return `BUSY`.
  Independent contexts can operate concurrently. No hidden worker requires
  callbacks to run on an arbitrary thread.
- Successful C operation calls return a non-null, immutable result handle,
  even when the program is rejected. Result views remain valid until
  `result_destroy` and do not depend on the context remaining alive.
  Callers must not free or modify individual spans.
- `result_get` requires a valid result and a zeroed view with its exact
  `struct_size`. On success it fills all fields. On a failure the caller must
  not consume fields. Multiple read-only queries of one result are permitted;
  destruction must not overlap any use.
- `context_destroy(NULL)` and `result_destroy(NULL)` are no-ops. Destroying a
  context during an operation is a caller error. The owning library's destroy
  functions must be used; an allocator from a different CRT is never used.
- No C++ exception may escape the library or a callback. Hosts must translate
  their own callback exceptions into declared failure statuses.

For create/operation functions, a valid output pointer is set to null before
processing. A non-`OK` C status denotes an API/resource/internal failure with
no usable result. `OK` means a result is available, not that the program was
admitted. Result outcomes distinguish `SUCCESS`, `REJECTED`,
`UNSUPPORTED`, `CANCELLED` and `LIMIT_EXCEEDED`. Invalid API arguments do
not become misleading source diagnostics. An allocation failure which prevents
even diagnostics returns `OUT_OF_MEMORY`.

## Explicit identity and dependency resolution

Each validate/derive request supplies immutable bytes, an explicit source URI,
the original technical `source_document_id`, an independent `fir_document_id`,
exact source format ID/version and exact desired FIR version. All are required
and nonempty. `fir_document_id` supplies the FIR envelope's `document_id`;
`source_document_id` remains the authored identity in diagnostics and provenance.
The library never substitutes one identity for the other. The declared format/version must match
the actual envelope. Names, installation paths and labels are not technical
object identities. No `latest`, wildcard or semantic-version range selects
another frontend or FIR revision.

`semantic_context_json` is an explicit JSON object defining the semantic
compilation context, including any supported ConditionalDisable inputs. An
empty span means `{}`. It never inherits process environment variables, the
operating system, compiler host, Studio settings or a previous operation's
context. Unknown or malformed context settings are refused by the common
frontend; they are not silently converted to defaults. The exact effective
context belongs to the request's provenance and deterministic inputs. A
conditional source decision must use this context, never backend availability.

The opaque source URI is the basis supplied to the resolver, never a license
to use the process current directory. An unsaved document can use a stable
non-file URI. Relative authored references are passed unchanged together with
the owning URI and JSON Pointer; the host must resolve or refuse them.

Both resolver callbacks are set, or both are null. With no resolver, a program
requiring external dependencies is refused with an attributable unresolved
dependency diagnostic. The library performs **no implicit filesystem, network,
registry, environment-variable, project-search-path or installed-library I/O**.

The resolver is called synchronously on the operation thread with the dependency
kind and actual authored reference. Source, TypeDef, library-contract and
profile-contract dependencies are distinct. It returns one immutable snapshot:
canonical source URI, technical identity/version, exact bytes and an optional
host-owned lease token. The frontend checks the dependency's own envelope and
declared identity/version, computes the content SHA-256, and checks any authored
digest; host metadata cannot override incompatible contents. Contract files
describe semantics and signatures. Returning a contract does not inject native
code or grant support for an operation the implementation does not implement.

The frontend initializes the resolved structure to zero and sets its
`struct_size` before calling the host. For each resolver callback that returns
normally, `release` is called exactly once with that structure, for every
status, including malformed output and cancellation. Callback-provided views
remain valid and unchanged until release; no view is accessed afterwards.
Release happens before the enclosing API operation returns. Hosts may leave
all payload fields empty on refusal; `error_message` then adds useful context.

A request uses one snapshot per resolved identity. Repeated references reuse
the captured snapshot when equivalent; contradictory identity/URI/version/
content evidence is an explicit conflict. Resolver results cannot change an
already captured dependency during the operation. Cycles, ambiguous aliases,
missing dependencies, denied access and unsupported versions are diagnosed
separately. No persistent implicit dependency cache bypasses a fresh request.
The source/dependency closure and hashes are returned in provenance.

## Limits and cancellation

All limits are unsigned counts. Zero selects the API-3 default below;
nonzero is a caller ceiling clamped to the implementation hard cap; it cannot
raise that cap. A profile may impose stricter declared caps. The effective values are reported by `describe` and per-request
provenance. These ceilings do not promise support for graphs of that size.

| Limit | Default | Counted scope |
| --- | ---: | --- |
| Source bytes | 8 MiB | Exact root UTF-8 buffer |
| One dependency | 8 MiB | Exact dependency buffer |
| Total dependency bytes | 64 MiB | Unique captured dependency snapshots |
| Result bytes | 32 MiB | Sum of all returned JSON buffers, including resolved Source |
| Nodes | 1,024 | All root/dependency graph nodes, including non-visible regions |
| Connections | 4,096 | All root/dependency graph connections |
| Nesting depth | 64 | Maximum semantic region/type nesting |
| Dependencies | 256 | Unique resolved snapshots |
| Diagnostics | 256 | Returned entries, including the terminal limit diagnostic |

Counts use checked arithmetic. Exceeding a ceiling returns a limit diagnostic
and no partial FIR. If the diagnostic count is exhausted, the final reserved
entry reports that fact; truncated diagnostics cannot convert rejection into
success. The root source, immutable captured dependencies and every materialized
output must remain within their caps. Metadata for `describe` is also covered
by the result budget. An unrepresentably small result budget can return an API
resource failure rather than a partial JSON document.

Cancellation is cooperative and request-local. The callback returns 0 or 1;
other values produce a callback-failure diagnostic. Poll before dependency
resolution and between bounded parse, validation and generation units. After
observing cancellation, stop new work and resolver calls, release all captured
leases, return `CANCELLED` and no FIR. A blocking resolver must implement its
own cancellation; the frontend cannot safely interrupt a host callback.
Cancellation must not mutate source, publish artifacts or alter another request.
There is no latency guarantee before P28 measurements.

## Result documents

JSON document families are separate from C ABI version 3. Each document carries
its own format ID and exact format version. Their schemas and fixtures are
owned by the common SDK contract work; implementation must not invent private
alternatives. The following payload responsibilities are normative:

The local API-3 documents use `frog.diagnostic` version `0.2` (an array of
records), `frog.frontend.context` schema version `0.3`,
`frog.frontend.capabilities` schema version `0.2`, and
`frog.frontend.source_maps` schema version `0.1`. Historical 0.1 schemas remain
unchanged; the 0.2 schemas have explicit filename suffixes. See
[result contracts](frontend-result-contracts.md) for their locations and the
read-only checker for actual library/CLI output.

| View | Required contents |
| --- | --- |
| `fir_json` | Complete admitted FIR, including actual types, operation/profile identities, regions, connections, state/effect contracts and source attribution. Present only for successful derive. |
| `diagnostics_json` | Stable code, severity, stage, message, source URI/document ID and JSON Pointer or object/port/connection identity when known; explicit related locations. Empty list is valid. Human text is not a machine identity. |
| `source_maps_json` | Source-to-IR mappings with original and derived IDs, including nested scopes and ports. Resolved/migrated identities remain distinct from authored identities; presentation-only omissions are explicit where relevant. Empty where no IR exists. |
| `provenance_json` | Exact root hash and URI/document ID, dependency closure/hashes, separate format/API/software/profile identities, applied limits and any declared transformation chain. Rejected operations retain known provenance without pretending to have completed a closure. |
| `descriptor_json` | Software version, commit and content/build identity, API version, supported exact Source/FIR versions and semantic profiles, default/hard limits and supported operations. Describe only. |
| `resolved_source_json` | Exact complete Source bytes actually submitted to the producer after dependency projection. Present only for successful validate/derive when those bytes differ from the authored Source. Empty for unchanged Source, refusals and describe. |

Diagnostics is present for every result. Maps and provenance are valid complete
documents (possibly empty), not null JSON fragments. All non-success operations
have an empty FIR view. A successful describe has no FIR; validate has neither
FIR nor a descriptor. A failed derive must not return an executable prefix.
The resolved Source is a complete Source document, not a patch or a second
semantic representation. Its SHA-256 is `projected_source_sha256` in provenance;
`source.sha256` still identifies the authored input. `provenance.fir.document_id`
records the independently requested FIR identity, including on a refusal.
This new field belongs to context schema `0.3`; the closed historical `0.2`
schema remains unchanged and must not be used to validate API-3 provenance.
All result spans, including resolved Source, survive context destruction and
remain valid only until the owning result is destroyed. Result-budget failure
clears both FIR and resolved Source rather than returning a partial artifact.

Fixed inputs, dependency snapshots, profiles and implementation produce
deterministic FIR, maps and semantic provenance. Do not include wall-clock time,
unstable pointer values, random IDs or working-directory-dependent paths in
their identity. Resolver request order is not itself a semantic dependency
ordering. Diagnostics may explain resource/cancellation outcomes, which are
not deterministic program semantics. Object-member ordering differences cannot
relabel a mismatched source/IR pair as equivalent.

## Independent version axes and Studio pinning

| Axis | Meaning |
| --- | --- |
| Source format version | Input syntax/envelope and declared language profiles |
| FIR format version | Output IR representation and admission contract |
| Frontend API version | This library ABI; local experimental version 3 |
| Frontend software version | Concrete implementation identity; `0.1.0-local`, not a published release |
| Semantic operation/profile versions | Actual type, state, effect and operation meaning |
| Bridge protocol version | Independent downstream transport/session contract |

Compatibility is checked for each axis. A matching API is not proof that a
Source, FIR or operation profile is supported. No renamed private field or
placeholder counts as public compatibility.

Studio initially links the frontend statically at an exact tested software
version, commit and SHA-256 inventory/artifact identity. A newer public release
does not replace the embedded frontend automatically, and the user does not
select a different frontend per project. The private Studio adapter captures
current document state and applies explicit historical migrations, preserving
the source/derived attribution chain; semantic inference and FIR generation
remain in the common implementation. Bridges consume admitted FIR and perform
their own downstream lowering/capability admission.

The pinned static Studio integration may use the private C++ `runOperation`
boundary used by the C facade. Its result can retain immutable Source and typed
FIR owners so Studio does not parse and rederive the same accepted artifact.
This is not a public C++ ABI or a Runtime interface. The operation still performs
the same argument checks, budgets, semantic admission, sealing and JSON result
accounting as a C call. Only a successful derive supplies typed FIR; a successful
validate withholds it just as it withholds `fir_json`.

An optional already captured `FrogSourceDocument` is reused only if its original
bytes match the exact request bytes. Equal pointer/length can establish this
without another scan; otherwise exact byte equality is required. A mismatch is
an API argument error, with no hidden reread, repair or fallback copy. Limits
are checked before scanning large buffers. Reusing a parsed immutable Source
does not reuse semantic validation or bypass the real producer. Both Studio
and the public C facade therefore consume the same authority and version pin.

## Local CLI for P28

The CLI implementation uses this exact API, without a parallel producer:

- `frog-frontend describe`: print the descriptor and exact supported versions.
- `frog-frontend validate --source FILE --source-uri URI --document-id ID --fir-document-id FIR_ID --source-format ID --source-version VERSION --fir-version VERSION`: run the same language admission as derive.
- `frog-frontend derive` with the same arguments plus `--output FILE`: emit complete FIR only after success; diagnostics/maps/provenance are available as separate JSON outputs.

The current CLI host reads explicitly selected files and the explicit
`--dependencies` lock manifest, then supplies the resolver. It does not implement
implicit dependency roots or searches. It must not add an implicit CWD,
network fetch or environment lookup inside the library. Failed derivation must
not overwrite a previous valid output or leave a partial new artifact. Exit
codes must distinguish success, program refusal/unsupported input, cancellation,
resource exhaustion and CLI/internal failure; P28 implements and tests them.
Implementation source is not proof of a compiled or qualified CLI. Record the
actual linked artifact and its focused checks separately.

## Qualification, distribution and licensing

`local`, `candidate` and `published` describe dissemination, independently of
implemented/qualified status. P01 API 1 and experimental API 2 remain historical;
P28 API 3 is local.
A candidate requires
implementation, dependency/license inventory, conformance and actual CLI/Studio
evidence; published additionally requires explicit authorized publication.
No schema, header, Python example or mock is executable-capability evidence.

The [accepted Apache-2.0 decision](frontend-licensing.md) grants this license to
the selected Graiphic-owned common Frontend/CLI files, including retained
Studio/Runtime origins. Historical NOASSERTION observations remain in the exact
[release inventory](../../../Implementations/Frontend/docs/license-inventory.json).
Third-party material keeps its own license and notices; unrelated Studio,
private Runtime and optimization implementations remain excluded. Preparing
this candidate does not publish its code or a private C++ ABI.


## Current finite candidate and documentation publication

The public documentation commits59763f28 and85a90ebe shipped contracts/data
only. Their Source/FIR/software/API version separation is preserved. This file
retains the richer local API3 lifetime/limit contract; see the actual current
[candidate profile](../../../Implementations/Frontend/docs/candidate-profile.md)
for admitted forms and explicit refusals. The library implements only kind2
type-definition resolution; Source/library/profile dependencies and a nonempty
ConditionalDisable context are explicitly unsupported. The CLI has an explicit
`--dependencies` lock, not an implicit search-root mechanism. Numeric limit0
selects a default; nonzero ceilings are clamped to the implementation hard cap.
The C header remains the authority for ABI3; this is not a second RPC format.
