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
