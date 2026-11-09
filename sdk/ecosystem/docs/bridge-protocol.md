# Bridge protocol 0.1

Status: local P01 contract. Only envelope/description validation and compatibility
negotiation are implemented here. Process hosting, transport framing, cancellation
and method execution are P09 work, not qualified by these records.

## Transport and envelope

The initial host starts one explicitly selected bridge process with inherited
anonymous stdin/stdout pipes. It does not discover an executable through PATH,
interpret a shell command, load a bridge into Studio's address space, or expose
a network listener. Acquisition, trust and launch authorization are prior gates.

Each frame is a four-byte unsigned little-endian byte length followed by exactly
that many UTF-8 JSON bytes. Zero length, truncated frames, duplicate JSON members,
malformed UTF-8, isolated surrogates and unsupported envelope versions are errors.
The initial maximum is 1 MiB. After describe, use the smaller advertised maximum
and host ceiling (hard maximum 16 MiB). Validate the length before allocation.
Stdout contains frames only; bounded human logs use stderr.

The [message schema](../schemas/bridge-message.schema.json) carries request_id,
session_id and generation independently. IDs are technical strings, not paths.
A host assigns them; the bridge echoes them. A generation increments when a
session is replaced. Messages from an obsolete tuple cannot mutate the current
session, its progress or its output. Validation already rejects such envelopes.

A response includes status and diagnostics. Progress is advisory, identifies its
stage, and has completed <= total. total=0 denotes an unquantified stage and
requires completed=0. Progress never implies success. The method-specific payload
is opaque to this envelope; P09 must version and validate each method's payload
before use. An accepted envelope alone is not method admission.

## Handshake and compatibility

1. Send describe using protocol 0.1, with an empty payload.
2. Admit the description schema and exact protocol version. Describe and validate
   are mandatory; other methods are optional explicit capabilities.
3. Independently check FIR family/version, each semantic profile/version and all
   required operations against the graph context and bridge description.
4. Run bridge validate on the complete admitted FIR before lower/build/run.
   Structural admission by the host does not prove the bridge implements the graph.

[contract.negotiate_bridge](../contract.py) implements step 2/3 contract checks
against supplied, validated descriptions. Fixtures are not the production
capability catalog. No fallback to another version, profile, CPU or generator is
permitted on rejection. Source/FIR/API/software identities remain separate.

## Lifecycle and failure

Declared methods are describe, probe, validate, lower, build, deploy, run, stop
and observe. Unsupported methods fail explicitly. A bridge cannot reinterpret
stop as a universally safe device reset. Pause, step, probes and highlighting
require further explicit capability contracts; they are not implied by run.

The host uses the smaller advertised timeout and its local ceiling. Each stage
has one final response. It may emit bounded progress before that response, never
after. Cancellation uses stop for the request/session when supported; the host
stops admitting its outputs immediately. Terminating a failed process does not
prove that an external device stopped or that deployment was rolled back.
Incomplete outputs stay uncommitted and the observed outcome is reported.

A crashed, timed-out or malformed bridge cannot report success through logs.
EOF without a final response is a transport failure. A future protocol version
must be negotiated explicitly and cannot silently reuse this envelope.

The C Frontend library's API epoch is unrelated to this IPC protocol. Bridges
consume public FIR and lower it downstream; they never reimplement .frog parsing
or source-to-FIR semantics.


## Authored Source provenance and prepared FIR

API-3 provenance version 0.3 admits `frog.source` version `0.1` and
`frog.document.draft` revisions `0` through `9`. These fields identify the
immutable authored input; they do not ask a bridge to parse or compile that
input. A draft requires `projected_source_sha256`, the hash of the complete
public Source produced by the Frontend. The FIR's exact Source hash must match
this projection (or the authored hash when no projection exists). The authored
URI, document identity and hash remain distinct and are not rewritten as public
Source metadata to pass negotiation.

Captured dependencies admit Source (`kind=1`) and Type Definition (`kind=2`),
with their exact identity, version, canonical URI, SHA-256 and explicit-resolver
origin. Other kinds remain refused by this profile. An accepted provenance
still requires complete-graph backend admission; it does not expand the bridge's
supported operations, types or target capabilities.
