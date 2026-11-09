# Bridge method contracts

This document defines method payloads `frog.bridge.payload` version `0.1` under
the existing `frog.bridge.message` wire protocol `0.1`. The source convenience
API is [frog/bridge.hpp](../include/frog/bridge.hpp). Third-party integration uses
bounded UTF-8 IPC; the C++ header is not a binary plugin ABI and exposes no
Runtime, Canvas, Widget, LLVM or Studio objects.

The historical [wire schema](../schemas/bridge-message.schema.json) and
[description 0.1](../schemas/bridge-describe.schema.json) remain unchanged.
`decodeMessage` admits the historical opaque object payload. `validatePayload`
is a separate admission step before a host uses a method's contents. A
historical fixture with `fixture_only` data proves envelope compatibility;
it is not an executable validation request.

## Framing and discovery

Framing remains a four-byte unsigned little-endian length followed by exactly
that many UTF-8 JSON bytes. Check the length before allocating. Stdout carries
frames only; stderr is a separate bounded diagnostic log. The host starts the
explicitly selected executable without a shell or PATH discovery. There is no
network listener or published service endpoint.

On Windows the process still sees inherited standard handles. The parent may
back these handles with private local named endpoints to obtain overlapped I/O
and enforce write deadlines on a hung peer. Those endpoints have unique names,
reject remote clients, require the first instance and an explicit inherited
handle whitelist. This transport detail does not change the public framing or
introduce a discoverable named-pipe service.

The initial envelope ceiling is 1 MiB. After describe, the effective ceiling is
the smaller host/bridge limit, always at most 16 MiB. The effective request
deadline is also the smaller advertised/local value. JSON is read by the same
strict parser as the production Frontend: duplicate members, malformed UTF-8,
surrogates, excessive nesting and excessive value counts are rejected. The
codec preserves numeric lexemes and projects generations exactly to uint64;
it never rounds an identity through binary64.

`describe` request uses `{}`. Its successful response payload is a **direct**
Description document, not a method wrapper. Both historical 0.1 and explicit
[0.2](../schemas/bridge-describe-0.2.schema.json) are supported. Description 0.2
adds software and engine identities, targets with independent hardware/backend
identities, realization profiles, type families, services, commands, constraints
and progress/stderr budgets. Encoding a 0.1 description omits these additions.

`operations_exhaustive` is explicit in 0.2 and implicitly true in 0.1. An
exhaustive list missing a required operation rejects discovery. A nonexhaustive
list defers that decision to complete-graph `validate`; it grants no permission
to lower, build or run. Declared constraints are descriptive, not an admission
receipt. `describe` and `validate` are mandatory methods.

## Artifact and provenance binding

`graphRequirements` adapts the real Frontend API **3** provenance document
`frog.frontend.context` **0.3** and the complete FIR bytes. It requires a closed
dependency capture, matching FIR family/version/document identity and the
exact operation set present in `unit.objects[].kind`. It preserves the entire
provenance document, including dependencies, transformations, semantic context
and limits. It does not run a second Source parser, infer a different profile,
or perform target admission.

FIR and provenance versions are independent. Semantic profile IDs remain the
exact strings emitted by the Frontend, such as
`frog.canonical.widget-f64-add.v1` or `frog.studio.composed-program@1`.
The independently negotiated profile version is the decimal suffix following
`.v` or `@`; it is never inferred from the FIR version. A profile without an
explicit supported suffix is refused rather than guessed.

`firSha256` hashes the exact supplied FIR UTF-8 bytes. `contextSha256` hashes the
exact provenance bytes. `frontendFingerprint` hashes the canonical JSON object
containing the Frontend software/API/commit/digest identities. Authored Source
identity stays in provenance; `GraphRequirements.documentId` is the distinct
FIR document identity. These hashes establish byte binding, not third-party
authenticity. The bridge must validate the complete graph using its real owner.

The payload's `fir` is `{family, version, sha256, utf8}`. `utf8` is a JSON
**string containing the exact FIR document**, not a replacement FIR object.
`provenance` is the complete provenance object. `selection` contains
`engine_id`, `target_id` and `profile_id`. For the current built-in LLVM CPU
bridge these are `graiphic.llvm.cpu`, `windows-x64`, and `native-fast` or
`native-debuggable`; the independent backend identity is `llvm-22.1.7`.

`encodeMessage` preserves `payloadJson` bytes after strict validation, including
the provenance object's spelling. A producer that reconstructs provenance with
a different JSON spelling must calculate its context hash from those exact
transmitted bytes; it cannot reuse a receipt for the previous spelling.

## Method payloads

Except for direct describe responses and the empty legacy forms above, a
payload begins with `schema: "frog.bridge.payload"`, `schema_version: "0.1"`,
`method` equal to the envelope method, and `phase` equal to the envelope kind.
See [the payload schema](../schemas/bridge-payload.schema.json). No native
pointer, private ABI structure, executable path or in-process object handle is
transported.

| Method | Request | Successful final response |
| --- | --- | --- |
| describe | `{}` | Direct Description 0.1 or 0.2 |
| probe | `{}` or versioned request | Versioned `observations` object |
| validate | Complete `fir`, `provenance`, `selection` | `receipt` |
| lower | Same complete graph and `receipt` | `lowered_handle` |
| build | Same complete graph and `receipt` | `artifact_handle` |
| deploy | Same complete graph and `receipt` | `deployment_handle` |
| run / start | Same complete graph and `receipt`; optional `command: "start"` | `run_handle`; optional `values` |
| run / explicit command | Current `run_handle`, advertised `command`, optional `parameters` | Current `run_handle`; optional `values` |
| stop | Current `run_handle` | Current `run_handle` |
| observe | Current `run_handle`; optional `parameters` | Current `run_handle`; optional `observations` |

A receipt contains `token`, `fir_sha256`, `context_sha256`, `engine_id`,
`target_id` and `profile_id`. Its token is bounded opaque text. The producing
bridge owns and verifies the receipt's association with the exact admitted
graph, provider/configuration and request/session generation. Structural
validation of the token text does not issue admission. Modifying FIR,
provenance, target or profile invalidates reuse. Receipt issuance follows real
complete-graph target admission; a successful describe/probe cannot issue one.

Artifact, lowered and deployment handles contain only bounded opaque `token`.
A run handle additionally contains `session_id` and exact `generation`, which
must match its envelope. Owners must scope and expire every handle; the codec
does not turn arbitrary strings into Runtime resources. Request IDs remain
independent, so stop/observe can address an existing run with a new request ID.

Pause, resume, step, control changes and Event injection use `run` with explicit
`command` names advertised in Description 0.2. Neither their availability nor
their argument semantics are implied by `run`. Unsupported commands are
explicit refusals. `parameters`, `values` and `observations` are bounded JSON
objects interpreted by the advertised command/service contract. No generic
bridge code fabricates equivalent execution for an unavailable command.

An error/cancelled response may carry `{}` and never needs a fabricated receipt
or handle. It still carries its final envelope status and diagnostic array.
Advisory progress is carried by the existing envelope progress record and does
not admit an artifact or commit output.

## State, diagnostics and limits

`Exchange` accepts only progress and one final response for the exact
request/session/generation and method. A stale tuple returns
`bridge.stale_message`, increments `discarded()` and leaves the current
exchange unchanged. The host discards it and continues waiting for the current
tuple. Progress is bounded in count and obeys `completed <= total` (therefore
total zero requires completed zero). A message after the final response is an
error. After cancellation no output is admitted, even if the peer finishes.

Bridge envelope diagnostics retain their historical `frog.diagnostic` **0.1**
shape and attributed URI/pointer/object/port/region. Frontend result diagnostics
are `frog.diagnostic` **0.2** and are not interchangeable with these records.
Host adaptation must retain their origin and category rather than relabeling
them by inspecting message text. Stderr is never a successful protocol result.

The transport owner enforces deadlines, frame/stderr ceilings, EOF/crash
handling and termination; `Exchange` does not spawn a timer or process. Killing
a bridge process does not prove a physical device stopped or deployment rolled
back. Unconfirmed effects remain explicit. No receipt or output is published
after cancellation or from an obsolete generation.

The portable smoke test reads the original wire fixtures and invokes the real
API-3 Frontend for public pure-add, record and Event fixtures before checking
negotiation and byte binding. It establishes protocol behavior; a fixture-only
peer does not qualify a target backend, external installation or device.
