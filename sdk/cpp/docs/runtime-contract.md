# FROG portable Runtime contract

`frog/sdk/runtime_contract.hpp` is the public, framework-free boundary between an
authoring tool and a FROG Runtime implementation.

The contract owns only portable exchange concepts:

- protocol and artifact schema versions;
- stable document, artifact, node, port, edge, widget and session IDs;
- canonical value-type descriptors and scalar Runtime values;
- immutable graph artifacts and UI bindings;
- source identity mappings;
- normalized diagnostics and target capabilities;
- lifecycle commands and typed session events.

It deliberately does not own editor state, Win32 types, provider internals,
scheduling policy, rendering data or mutable document objects. Studio and Runtime
must adapt their private models at this boundary rather than redefining these
exchange types.

The initial value carrier supports Boolean, integer, floating-point, string and
path-compatible scalar values. The type descriptor already reserves arrays,
complex and fixed-point categories; their value carriers and execution semantics
will be added only with the corresponding conformance slices.

Compatibility rules:

1. A different protocol major version is incompatible.
2. A newer minor version may add optional fields but cannot reinterpret existing
   fields.
3. Artifact schema changes require an explicit migration or rejection diagnostic.
4. Runtime events always identify the session, artifact and artifact revision that
   produced them.
5. A Studio must reject events whose artifact revision is no longer current.

## Canonical transport

`frog/sdk/runtime_contract_codec.hpp` provides the canonical dependency-free
binary transport. Its payload is explicitly versioned and little-endian. The
decoder bounds all strings and collections, rejects truncated or trailing data,
and validates the complete artifact before exposing it. Producers emit nodes,
ports, edges, UI bindings and source identities in stable semantic order so the
same document revision always produces the same bytes.

## Opt-in event backend

The separate [event backend contract](event-runtime-contract.md) defines typed
notifications, already-lowered cases, filter responses and dynamic registration
handles. It does not change `ProgramArtifact` schema 1, its codec, or the
default Numeric/Boolean execution profile. Public source/FIR lowering and IDE
host integration are separate requirements, not implied by this backend API.

## Schema 2: owner-checked structured Event Data

This is an explicit extension of the existing `ProgramArtifact` and
`ProgramSession`, not a second graph authority or executor. Schema 1 retains
`FROGART1`, scalar carrier tags 0–5, its original field layout, and the default
Numeric/Boolean and scalar Event-body admission. Schema 2 uses `FROGART2` and
appends `Cluster` and `ObjectReference` categories and immutable carrier tag 6.
Legacy runtimes must reject schema 2. A runtime advertises this extension only
through `supportsTypedEventValues`; the portable executor requires both
`ProgramSession(true, true)` opt-ins. This flag does not claim native producers,
public Source lowering, filter support, Array execution or reference methods.

`runtime_compound_value.hpp` owns the sealed immutable Record factory and the
borrowed reference carrier. Ordered Record fields carry real canonical types;
canonical identities are quoted/escaped and reject structural control bytes.
The factory bounds depth, node count and text data, checks numeric leaf domains,
and rejects arbitrary external mutable carrier subclasses. String data retains
Unicode and CR/LF. A borrowed reference delegates to its actual registry-backed
`RuntimeReferenceAuthority`; no class name, numeric token, HWND or pointer is a
substitute for that capability. The native bridge uses the synchronized existing
registry and weak lifetime, not Canvas or window access from a worker.

Every schema-2 body carries `RuntimeValueOwner`: the owning public source/run ID,
sealed source digest, epoch, source revision and artifact revision. This is
injected by the owner after authoritative Source admission; it is never derived
from the artifact document ID. Compound values are checked against that exact
owner at load and execution, including nested references. Deletion, owner
destruction, registry domain or generation mismatch must reject use; recreating
the same display ID does not revive an old reference. The owning EventSession
must also guard captured fields that are not wired into the body.

`UiBinding.suppliedByEvent` is a schema-2, Control-to-Runtime-only marker for a
compiled Event Data input. It admits the absence of a value before an occurrence
exists; it does not supply a default or fake reference. The actual occurrence
must be injected before running the body. Standalone Start with that input still
unset raises a Runtime fault. Ordinary Value updates do not fabricate an event.

### Record decomposition provider contract

`frog.core.unbundle.by_name` is the existing SDK implementation ID in this
extension. The explicit public definition is now Libraries/Core.md section14a,
with Libraries/Manifests/frog.core.unbundle.by_name.v1.json and the versioned
local Source schema; its publication is not inferred from a private catalogue. It has exactly one
connected, single-cardinality Record input and one or more outputs (at most 64).
`RuntimeNode.selectedFields` contains compiler-resolved field identities in
output-port order. Each identity must exist in the input's canonical Record
schema and each corresponding output must have exactly that field's actual type.
The provider performs name-based extraction and publishes real values on those
output ports; changing output order does not select a different numeric row.
Unknown fields, incompatible outputs, compound casts or selections attached to
another operation are rejected. Record/reference arithmetic is not admitted.

The Source/open-IR lowerer must explicitly resolve an authoritative public
function definition/signature to this provider and populate selectedFields.
The public selection-version1 adapter retains ordered {port,field_id} pairs
and exact resolved cluster output types; implementationId is explicitly bound
to this provider. The SDK, carrier, codec and opt-ins remain unchanged. A
private Studio catalogue ID is not a public Source key, and a definition or
provider alone does not prove Source lowering or native production.

### Structured codec boundary

Schema 2 encodes its exact owner context, resolved selections and deferred-input
flags, followed by immutable Record field content using bounded recursive reads.
Semantic Record equality is explicit (`runtimeValuesEqual`); pointer identity is
not a Record round-trip oracle. Borrowed reference *types* and unset occurrence
slots may appear in an artifact. Live borrowed handles are in-process capabilities
and are never encoded; encoding such a value returns an empty payload. No raw
pointer/token persistence, cross-process registry access or NI binary ABI is
claimed. Scalar schema-1 bytes remain protected by a fixed golden artifact.
