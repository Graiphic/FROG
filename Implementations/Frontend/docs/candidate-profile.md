<a id="profil-de-la-candidate-frontend-commune"></a>

# Common Frontend candidate profile

Local candidate `0.2.0-local` / API 3, statically linked into Studio and used by the CLI. Source/FIR remain `0.1`; drafts declare exact revisions `0` through `9`. Qualifications apply to specific hashes, separately from the recipes below. Initial standalone snapshot `c640d266a4de9d9581e6394d80d7152ce3d91d64278b960ab6777a524a056a62` was rebuilt, installed and called by an external C consumer (six CTest cases, 6,614 checks). Later captures require their own inventory-bound receipt; this initial success qualifies neither new bytes nor a later Studio executable.

<a id="propriétaire-et-frontière"></a>

## Owner and boundary

`src/FrontendApi.cpp` owns the C contract and immutable results. `src/FrontendOperation.cpp` owns capture, explicit resolution, budgets, diagnostics and orchestration of the actual `CanonicalExecutionIr` and `CanonicalEventExecutionIr` producers. The former retains existing structured/composed paths. Studio and the CLI consume the same owner.

The library receives bytes, identities, options and callbacks, with no implicit disk, network, registry, Studio-project, environment or PATH access. The CLI explicitly hosts files for the C API. Draft reading and semantic migration belong to the common Frontend. `FrogDraftData`, `FrogWidgetDataView`, widget DTOs and their codecs carry required data without Canvas or `LoadedFrogDocument`. Private facades reuse these owners; controllers and recovery state remain in Studio. Repair proposals/application remain private to Studio; target admission, providers, compilation and execution retain separate owners. The installed interface is `frog/frontend.h` and `Frog::frog-frontend`; internal C++ headers and typed owners retained by Studio are not a second public ABI.

<a id="opérations-et-versions"></a>

## Operations and versions

| Operation | Actual work | Accepted result |
| --- | --- | --- |
| `describe` | Implementation identity, API operations, formats, principal profiles and limits | Complete descriptor, no Source or FIR |
| `validate` | Same resolution, producer and sealing as derive | Diagnostics/maps/provenance; no returned FIR document |
| `derive` | Same work, then publication of the accepted document | Complete FIR, diagnostics/maps/provenance |

Each validate/derive request supplies immutable authored bytes, explicit URI, `source_document_id`, `fir_document_id`, Source format/version and exact FIR version. Source and FIR identities remain distinct; neither a display name nor output path replaces them. URIs trigger no implicit access; relative references reach the resolver with their owning URI and JSON Pointer.

| Identity/version | Value or role |
| --- | --- |
| C API | `3`, exact version/sizes, reserved fields zeroed |
| Software | `0.2.0-local`, independent of ABI |
| `frontend.commit` | Base revision recorded at build time, without overlays |
| `frontend.sha256` | Content identified by `FrontendInputs.cmake`, distinct from PE/archive hash |
| Public Source / FIR | `frog.source/0.1` / `frog.execution_ir/0.1` |
| Historical documents | `frog.document.draft`, explicitly declared revisions `0` through `9` |
| Provenance | `frog.frontend.context`, schema `0.3` |
| Capabilities / maps | `frog.frontend.capabilities/0.2` / `frog.frontend.source_maps/0.1` |
| Profiles and bridge | Their own semantic revisions; separate downstream protocol |

The descriptor announces principal profiles `frog.canonical.widget-f64-add.v1`, `frog.canonical.typed-structures.v1`, `frog.studio.composed-program@1`, `frog.canonical.event-scalar.v1`, `frog.canonical.event-captured.v1`, `frog.frontend.resolution@1` and `frog.frontend.resolution@2`. `profiles_exhaustive: false` prevents presenting this as an exhaustive operator/realization matrix. On success, provenance.profiles and provenance.operations come from actual produced FIR. The descriptor retains `native_execution_claim: false` and local distribution.

<a id="périmètre-fini-de-langage"></a>

## Finite language scope

`tests/fixtures/provenance.json` retains each Source's hash and origin:

| Fixture | Contract exercised |
| --- | --- |
| `pure-add.frog` | Controls/indicator and f64 addition through public Source |
| `record-exact.frog` | Nested Record, exact I64/U64 and preserved schemas |
| `nominal-nested.frog` | Complete Map/Array/Cluster/Enum; identities/values not reduced to physical carriers |
| `event-values.frog` | Event regions and typed captured values, including hidden regions |
| `pane-unbundle.frog` | Preserved panel/control references and Event data |
| `event-product.frog` | Production Event graph through the same public boundaries |

The six phase-A CTest cases executed are `frog-frontend-c-consumer`, `frog-frontend-cli-scalar`, `frog-frontend-cli-type-definitions`, `frog-frontend-integration`, `frog-frontend-draft-integration` and `frog-frontend-draft-dependency-smoke`. They cover TypeDef substitution; malformed/future Source rejection; references/cycles/conflicts; budgets, cancellation, leases, concurrency and result lifetimes. Draft tests exercise exact types, conditions and dependencies; they execute neither LLVM nor Runtime. The 6,614 checks apply to this finite corpus, not every function.

Studio files `regressions/native-qmh1-joined-cleanup.frog` and `regressions/native-state-machine-calculate3.frog` also passed validate/derive through the standalone API/CLI with unchanged input bytes. Three historical-fixture rejections remain attributed to pre-existing guards: old cluster `class_ref` without adaptation, scalar `u8` constant outside the selected scalar profile, and Unbundle by Name without stable identifier paths. Comparing earlier sources is not rerunning an old binary.

Accepted schemas retain fields, names, nominal identities, enums, ranks and children. I64/U64 do not pass through `double`. Structures retain region owners, terminals, connections and attribution. Hidden regions are not exempt from validation. Acceptance belongs to the actual producer/profile; this corpus does not generalize to every palette type/structure/operation combination.

E7 reuses this Frontend to derive scalar, I64/U64, Array32 and static-For Sources before a C17 peer. The peer's finite profile, value constraints and Enum/operation refusals are not general Frontend rules. Event, references or operations in FIR do not promise execution by a given peer.

<a id="résolution-typedef-implémentée"></a>

## Implemented TypeDef resolution

On public Source `0.1`, an empty context means `{}`, with no inherited settings. The initial profile uses the closed `frog.frontend.resolution@1` envelope in [frontend-resolution.md](../frontend-resolution.md). Its `{kind:2, source_pointer:...}` entries target an actual `studio_type_schema` or subschema. Unknown fields, arbitrary targets, overlapping pointers and contradictory references are rejected.

The authored `$ref` object declares reference, identity, version, SHA256 and fragment pointer. The callback returns a `frog.frontend.type-definitions@1` document with its own identity/version and bytes. The Frontend computes the hash, checks metadata/content, expands references in a detached projection and calls the same producer. Equivalent repeats share a capture within the request; contradictory URI/identity/content and cycles are rejected. No implicit dependency cache survives a new operation.

Resolve/release are either both defined or both absent. Every normal resolve return is followed by exactly one release for every status, before the operation returns. Owned captures remain independent of borrowed views. Callbacks run synchronously on the caller thread; no C++ exception may cross the C ABI.

The CLI preloads snapshots from a `{"format":"frog.frontend.dependencies","version":"1","dependencies":[...]}` lock. An entry contains kind, reference, source_uri, identity, version and path, with optional requesting_source_uri. A relative path resolves against the lock, not in the core. Selection by kind/reference/owner must be unambiguous. A lock cannot grant an unimplemented dependency kind to the language.

The `@1` contract above remains unchanged, including rejection of nonempty `conditional_disable`. New `frog.frontend.resolution@2` carries explicit `built_in`, `project`, `target` symbol maps for the draft path. The Frontend reads drafts `0..9`, resolves TypeDef links and SubFROG calls through callback-supplied captures, then uses the same FIR producers. It preserves the original document and identities; no repair proposal is implicitly applied. Targeted paths were executed in phase A; Studio paths and new captures retain their own receipts.

For drafts, SOURCE=1 and TYPE_DEFINITION=2 are actual resolver requests originating from document references and attributed to their URI/pointer. LIBRARY_CONTRACT=3 and PROFILE_CONTRACT=4 are not implemented. Reading a recognized revision guarantees neither semantic validity nor backend admission; a recovery copy without a usable diagram may remain rejected. See the [v2 contract](../frontend-resolution.md).

<a id="résultats-erreurs-et-publication"></a>

## Results, errors and publication

`FROG_FRONTEND_OK` means the C call returned a result, even for a rejected program. `outcome` distinguishes SUCCESS, REJECTED, UNSUPPORTED, CANCELLED and LIMIT_EXCEEDED. A non-OK C status—argument, ABI, BUSY, memory or internal error—provides no consumable result. Invalid arguments are not fabricated program errors.

Diagnostics carry stable code, stage, severity, category and known location. Missing Frontend capability, semantic error, integrity and dependency remain distinct. Human messages are not a classification API. FIR location `/unit/...` is `derived_pointer`, never a fabricated authored pointer. Related locations are optional and not invented.

Every result contains complete diagnostics, maps and provenance. Rejection/cancellation/limits return no executable FIR or partial projection. Provenance retains known facts; before capture, the Source hash may be empty. Describe fabricates no requested Source/FIR.

After accepted resolution, `resolved_source_json` supplies the **complete projected Source** used by the producer only when different, on successful validate/derive. Provenance distinguishes `source.sha256`, `projected_source_sha256` and authored→dependency transformations. The FIR source seal remains that of derived bytes. The CLI currently exposes no dedicated option for this buffer.

The CLI publishes diagnostics/maps/provenance separately, then FIR only on successful derive. Rejection or an auxiliary error before publication leaves the last valid FIR intact. Files are not one transaction. Options/exit codes appear in the [README](../README.md).

<a id="limites-annulation-et-concurrence"></a>

## Limits, cancellation and concurrency

Zero selects the default; a nonzero request can only reduce the effective maximum. These budgets do not guarantee admission of every language combination of that size.

| Effective maximum budget | Value |
| --- | ---: |
| Source and context JSON, each | 8 MiB |
| One dependency / total unique captures | 8 MiB / 64 MiB |
| Combined JSON results, including projection | 32 MiB |
| Producer FIR | 16 MiB |
| Combined Source/dependency nodes / connections | 1,024 / 4,096 |
| Semantic/resolution depth | 64 |
| Unique dependencies / diagnostics | 256 / 256 |
| TypeDef expansion visits | 4,096 |

Byte limits are checked before copies/scans of large buffers. Hidden regions count. Parser, codec and producer constraints remain active. Exhausting the diagnostics budget retains a terminal diagnostic; it does not convert rejection to success. If no complete result fits, the call returns a resource error rather than truncated JSON.

Cancellation is cooperative: 0 continues, 1 cancels; another value is a callback failure. Polls surround bounded units and the synchronous producer, without mid-producer preemption. A blocking resolver remains the host's responsibility. No maximum latency or real-time guarantee is announced.

Inputs, identities and views remain immutable until synchronous return. Configuration is copied at context creation. One call operates per context; concurrent/reentrant calls return BUSY. Independent contexts may work concurrently. No hidden worker starts.

A result owns its buffers and outlives its context. Views remain valid until `frog_frontend_result_destroy`; they are not individually released/modified. `result_get` requires exact size and zeroed reserved fields. Destroying a context/result during use violates the caller contract; the same owner's destroy functions release its allocations.

<a id="historique-et-qualification-de-candidate"></a>

## History and candidate qualification

| Lot | Local evidence scope | Retained limit |
| --- | --- | --- |
| E2 | Producer/adapter extraction and lexical audit | Source audit distinct from linking; original hashes distinct from current hashes |
| E3 | API3 library/CLI, installation/C consumer, Studio/C ABI/CLI document parity on a bounded corpus | Results bound to E3 bytes, without full Studio suite or clean Windows |
| E5 | Bridge SDK, independent wire and public source consumers | Protocol declarations/fixtures distinct from native execution |
| E7 | Public-kit rebuild and C17 peer, common FIR, actual outputs/artifacts/commands | Finite C profile and local toolchain; no universal palette/clean-Windows coverage |
| E8 | Pin/identity checks, public boundary, receipts and targeted qualification | Preserve neighboring failures/unexecuted checks; bind results to latest hashes |
| P28 phase A, 2 November | Standalone rebuilt kit, six CTest passes, 6,614 checks, installation/C consumer and current QMH/state drafts | Only content `c640d266a4de9d9581e6394d80d7152ce3d91d64278b960ab6777a524a056a62`; no clean Windows, new memory evidence or backend execution |
| P28 phase B | Studio integration and original-byte parity in progress | Unqualified; no delivered-application result claimed |

The integrator retains evidence in Studio's local `modularisation-etape{2,3,5,7,8}-...` artifacts. These internal paths are not public-kit dependencies. The historical E3 Studio `.738` reference is not the candidate's current version.

`artifacts/p28-implementation-2026-11-02/phase-a-result.json` retains A qualification: kit manifest `fed3baac219da2bc68c99b9d08e6560b73ca68c100e17cbfdfeb33d1e09f0102`, Frontend content and installed-library/CLI/consumer SHAs. The integrator retains it; it is not a kit input. `public-phase-benchmark.json` remains a measurement attributed to that same snapshot, without a gain conclusion; memory was not measured.

Sealing phase B requires updated exact content, library/CLI/Studio binary SHAs, returned identity, matching Studio pin, toolchain versions, audit and relevant parity checks. Reuse existing public Frontend tests, new targeted draft/dependency tests, Studio parity modes, the SDK protocol test and existing audit. Historical successes do not qualify new bytes without verification. Protocol success does not qualify a backend.

From the FROG root, these commands are recipes, not new results:

```text
python -B Implementations/Frontend/tools/audit_frontend_closure.py --root Implementations/Frontend --source src/FrontendApi.cpp --source src/FrontendOperation.cpp --source src/FrontendCli.cpp --output frontend-closure.json
python -B Implementations/Frontend/tools/audit_frontend_closure.py --root . --public-consumer --output public-consumer-closure.json
```

The second check requires the public SDK/C-peer owners. Lexical closure replaces neither compilation nor inspection of direct/delayed binary imports. Runtime-loaded OS dependencies remain a separate prerequisite.

<a id="licence-et-diffusion"></a>

## License and distribution

The [Apache-2.0 decision published on 1 November 2026](https://github.com/Graiphic/FROG/blob/85a90ebe2c2a78cc4f42f7efce1c8ca990df4d26/sdk/ecosystem/docs/frontend-licensing.md), recorded in `FROG-Context/Decisions/frog-common-frontend-apache2.md`, accepts that license for selected original Graiphic Frontend/CLI files and contracts/examples/tests, including necessary extractions. Third-party origins retain licenses/notices. The rest of Studio, private repairs, the rest of Runtime and optimization recipes are excluded.

The E2 `NOASSERTION` inventory remains historical pre-decision evidence without reopening the accepted choice. Distribution inventory must link current files to that agreement, retain original_hash/current_hash and attribute any third-party content. An audit or SPDX statement does not replace exact-package traceability.

The licensing decision is published separately; it does not publish this candidate or its code. This documentation update produces no remote publication, global Studio qualification, clean-Windows qualification or new binary version.

The Zstandard 1.5.7 decoder (including FSE/HUF and xxHash) is now a static document-reading dependency. It retains BSD licensing and notices; no third-party numerical realization, LLVM or private runtime is embedded for this reading. Its source closure and kit linkage were exercised by phase A's standalone rebuild. This proves neither every historical document's compatibility nor clean Windows. Every new capture must retain decoder notices/provenance.
