<a id="frontend-local--opérations-et-résolution-explicite"></a>

# Local Frontend: operations and explicit resolution

Experimental local implementation `0.2.0-local`, C ABI3. Source/FIR remain `0.1`;
`frog.document.draft` accepts explicitly declared revisions `0` through `9`.
Profile revisions are independent of ABI, software and bridge protocol. This
document describes code requiring qualification: its presence proves neither
compilation, execution nor publication.

<a id="génération-commune"></a>

## Shared generation

`FrontendOperation.cpp` calls production C++ producers `CanonicalExecutionIr::derive`
or `CanonicalEventExecutionIr::derive`. The composed profile retains its existing
routing through the former. The separate public Event profile is selected for a
public Event Structure outside the composed profile. There is no fallback to an
interpreter, Canvas, backend or different language version. `frog.document.draft`
is an explicitly declared input, not an implicit fallback from `frog.source`.

`validate` performs the same derivation and sealing checks as `derive`, then omits
FIR from its return. `derive` returns complete FIR only on success. Rejection,
cancellation or a limit never returns an executable prefix. Frontend success is
neither LLVM admission, operator realization nor execution evidence.

Original source is captured in an immutable owner shared with common readers;
derived projections retain separate identities. An empty context means `{}`,
without OS, environment, Studio or previous-request inheritance. Draft reading,
resolution and semantic migrations are now implemented in the common library.
They produce a detached projection without rewriting input. Repair proposals,
previews and application with history/Undo remain private to Studio. Qualification
of this `0.2.0-local` extraction has not yet been established.

<a id="profil-de-résolution-frogfrontendresolution1--contrat-conservé"></a>

## Resolution profile `frog.frontend.resolution@1` — preserved contract

The first realization is deliberately bounded to actual type definitions in the
composed profile. Context is a closed object:

```json
{
  "profile": "frog.frontend.resolution@1",
  "dependencies": [
    {"kind": 2, "source_pointer": "/diagram/nodes/0/studio_type_schema"}
  ]
}
```

The RFC6901 pointer must identify an actual authored object in a `studio_type_schema`
field or subschema. Context targets cannot overlap. The host supplies no arbitrary
replacement. The targeted authored object contains exactly:

```json
{
  "$ref": "dep:types",
  "identity": "example-types",
  "version": "1",
  "sha256": "<64 lowercase hexadecimal digits of the exact content>",
  "pointer": "/definitions/T"
}
```

The resolver receives kind `TYPE_DEFINITION=2`, the unmodified reference, actual
owning URI, its pointer and expected identity/version/hash. It supplies exact JSON
bytes and an explicit canonical URI. The returned document is itself a closed object:

```json
{
  "profile": "frog.frontend.type-definitions@1",
  "identity": "example-types",
  "version": "1",
  "definitions": {
    "T": {"canonical": "u64", "choice_kind": "none", "field_names": [], "choices": [], "children": []}
  }
}
```

This schema is illustrative: selected content must be a complete schema actually
accepted by existing type-codec owner `canonicalObjectReferenceReadType`. Nominal
identities, enums, field labels, ranks, dimensions and exact values are not simplified.
Nested references use the same `$ref` object and resolver.

After selecting `/definitions/...`, the Frontend expands references in a detached
projection, validates every fragment with that codec and submits the projection
to the actual producer. Missing/incorrect dependencies or resolved types contradicting
other declared types fail generation. This has an actual semantic effect; it is not
a decorative inventory.

`version` identifies the type-document revision; `@1` identifies its envelope profile.
The Frontend checks computed SHA-256, identity and content version against authored
information and returned metadata. One identity cannot denote multiple URIs,
versions or contents within a request; one URI cannot denote multiple identities.
Equivalent repeats share a capture. Cycles and missing fragments are explicit errors.

Every normal `resolve` return is followed exactly once by `release`, regardless of
status, including rejection, invalid metadata, limits and cancellation. Borrowed
bytes are never used after `release`; only request-owned immutable captures survive.
A resolver exception is not a normal return. Callback exceptions are contained and
diagnosed; hosts must respect the C ABI. There is no implicit cross-request cache
or file/network access.

Source, library-contract and profile-contract kinds are not implemented by this
first resolution profile. They are explicitly rejected without erasing references
or granting capabilities. Type documents cannot inject native code. SubFROG calls
remain subject to the producer's actual call contracts.

`conditional_disable` is an optional resolution-context field. In `@1`, only an empty
object is accepted: nonempty context is rejected with
`frontend.conditional_context_unsupported` at its exact location. The `@1` Source
path receives no nonempty symbolic context. Ignoring it or replacing it with backend
settings would be incorrect.

<a id="drafts-et-contexte-frogfrontendresolution2"></a>

## Drafts and `frog.frontend.resolution@2` context

This extension is implemented separately from `@1`, which does not implicitly gain
new parameters. The call declares `source_format_id="frog.document.draft"` and an
exact decimal version `"0"` through `"9"` matching the received envelope revision.
FIR remains `frog.execution_ir/0.1`. Future formats, inconsistent revisions and
malformed sources are rejected with attributed diagnostics.

The common reader, storage normalization (including Zstandard blocks), widget
schemas, TypeDef links, SubFROG calls, conditional specialization, validation and
public-Source migration reuse existing owners. They depend on neither Canvas nor
a runtime session. A recognized revision does not automatically make every document
executable: incomplete data, unavailable diagram, missing dependency and absent
capability remain explicit refusals.

Example context for draft preparation:

```json
{
  "profile": "frog.frontend.resolution@2",
  "dependencies": [],
  "conditional_disable": {
    "built_in": {},
    "project": {"MODE": "release"},
    "target": {}
  }
}
```

The three optional maps contain explicit String names and values. No symbol is
discovered through OS, registry, PATH or backend. Empty context preserves absent
configuration; unresolved conditions remain diagnosed. v2 context reaches draft
preparation; it does not add a second specialization of already projected public Source.

Saved draft links trigger `SOURCE=1` SubFROG and `TYPE_DEFINITION=2` TypeDef requests.
The callback receives the original reference label, owning URI, source pointer and
any identity/version/hash constraints. It returns immutable content, canonical URI,
identity, version and hash; the Frontend recomputes SHA-256, checks those constraints
and preserves incorporated-source provenance. Calls require neither an open Studio
file nor implicit disk searching.

These document links are not the JSON `frog.frontend.type-definitions@1` fragment
envelope above, which remains the Source `@1` path's contract. Library/profile kinds
are unimplemented. Unknown or inconsistent requests trigger no fallback.

The CLI supplies the same snapshots through its dependency lock and calls the same
library as the C consumer. Diagnostic text, identities and maps come from actual
preparation. Reading/migration support does not authorize modifying the original
document or approving a Studio repair.

**Status: implemented, NOT yet qualified on the new kit.** Targeted draft/dependency
tests must run with the exact source/library/CLI identity. Historical Source `@1`
successes do not automatically qualify `@2`.

<a id="limites-et-annulation"></a>

## Limits and cancellation

Zero selects documented limits; nonzero values can only reduce them. Existing
producer limits take precedence:

| Input or work | Maximum effective limit |
| --- | ---: |
| Source and context JSON, each | 8 MiB |
| One dependency | 8 MiB |
| Total unique dependency captures | 64 MiB |
| Combined JSON results, including FIR | 32 MiB |
| Producer FIR | 16 MiB |
| Combined Source/dependency nodes | 1,024 |
| Combined connections | 4,096 |
| Semantic/resolution depth | 64 |
| Unique dependencies | 256 |
| Diagnostics | 256 |
| Type-fragment expansion visits | 4,096 |

Byte limits are checked before copying/UTF-8-scanning large buffers. Aggregate
budget and capture count are checked before another dependency copy. Hidden regions
count. Additional parser/codec/producer constraints remain. Result-size calculation
precedes serialization. If complete diagnostics and provenance cannot fit, the ABI
returns a resource error without truncated JSON.

Cancellation is cooperative, checked before work, before resolution, between bounded
units and after the producer. A callback returning anything other than 0/1 is faulty.
Existing synchronous producers are not preempted mid-execution; no fine-grained
latency bound is claimed.

<a id="résultats-et-provenance"></a>

## Results and provenance

- Diagnostics: array of `frog.diagnostic` objects, version `0.2`; stable code,
  stage, severity, category, URI/document/pointer. Missing Frontend capability is
  distinct from semantic error. FIR pointer `/unit/...` remains `derived_pointer`,
  never presented as a Source pointer.
- Descriptor: `frog.frontend.capabilities`, `schema_version=0.2`; actual build-supplied
  software/API/commit/hash, formats, API operations, principal profiles and effective
  limits. The profile list is marked non-exhaustive; it is not a palette execution matrix.
- Provenance: `frog.frontend.context`, `schema_version=0.3`; exact original source,
  captured dependencies/hashes, explicit context, limits, transformations and
  `closure_complete`. Profiles/operations on success come from actual produced FIR.
  A refusal retains known facts without claiming complete closure. `fir.document_id`
  preserves the distinct FIR identity; this `0.3` extension does not modify closed
  historical schemas.
- Mappings: `frog.frontend.source_maps` envelope, schema `0.1`, containing FIR
  `source_map`/`correspondence`, resolution transformations and authored/FIR hashes.

With resolution, FIR remains sealed to projection bytes actually submitted to the
producer; its hash is never replaced artificially by the original-source hash.
Provenance distinguishes `source.sha256` and `projected_source_sha256`; transformations
link authored locations with dependency fragments. Without resolution, original
bytes pass directly to the common owner.

After immediate cancellation or pre-capture size rejection, the Source hash may be
empty: the Frontend does not traverse a buffer it has just rejected. Capture/callback
order is not semantic identity; dependency provenance is ordered by identity. No
timestamp or implicit machine path enters deterministic results.
