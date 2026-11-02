# Ecosystem architecture baseline

Contract status: experimental 0.x architecture baseline.
This document specifies the common boundaries for package tooling. It does not
assert that package installation, a Store, a bridge host, or a hardware target
has been implemented or qualified.

The versioned schemas in this SDK define the serialized fields and accepted
profiles. This document does not create a competing JSON grammar. Unsupported
versions and profiles require an explicit diagnostic; they must not be guessed
from a filename or silently interpreted as an older version.

## Content families and opening behavior

| Content | Suffix | Intended operation |
| --- | --- | --- |
| Installable library or toolkit | `.bfrog` | Inspect, resolve, then plan installation |
| Transportable project | `.pfrog` | Inspect and create a working copy |
| Transportable example | `.efrog` | Preview without execution, then create a working copy |
| Execution bridge | `.bridgefrog` | Inspect, install, configure, then activate |
| Driver requiring an OS installation lifecycle | `.driverfrog` | Inspect and prepare an explicit system installation plan |

These suffixes identify different user journeys. They do not grant trust,
privilege, execution capability, or a license. A user-space instrument library
may be an ordinary toolkit; it does not require the OS-driver family merely
because its author calls it a driver.

Existing document formats retain their roles:

- `.frog` is a program in an explicitly recognized source profile.
- `.frprj` is the editable project format; `.gsproj` is a legacy project input.
- `.ftype` identifies the existing type-definition authoring format.
- `.wfrog` describes widgets and their supported realization profiles.

An archive transporting a project does not replace its editable project format.
A project template is a creation intent within a project profile, not an
additional extension. A widget package is not a general-purpose installation
archive.

Filename extensions provide an expected family. A reader must identify the
manifest magic, family, profile and supported versions, and reject an
extension/content mismatch. Opening or previewing a package never implies
running its code, installing a dependency, or loading a native module.

## Identity and version axes

Publisher namespaces use a reverse-DNS style qualified identity. Package,
export and type identities are stable within their declared namespaces.
A declared namespace is not proof of domain ownership or publisher trust.

Labels, palette positions, filenames and installation paths are presentation
or location data. They must not be the only persistent identity. Renaming or
moving an object does not reassign its identity. Duplicate identities in the
same namespace and ambiguous references are errors.

A resolved reference retains the publisher, package, export or type identity,
the exact selected version and its accepted origin. Published package versions
are immutable. An origin must not replace an immutable version with different
content. A public registry must not substitute a package with the same name
for a dependency resolved from a private origin.

Migration aliases are explicit and versioned. Cycles, collisions and ambiguous
targets are rejected; a missing identity is not repaired by matching a display
name.

Package versions use strict semantic version syntax. Library API compatibility
is explicit and must not be inferred from the artifact ABI. The following
axes remain distinct:

| Version axis | Meaning |
| --- | --- |
| Source format/profile | Which program documents can be read and admitted |
| FIR format/profile | Which intermediate representation can be consumed |
| Frontend software/API | The implementation and its public library contract |
| Operation/type/widget contracts | The published semantics referenced by a program |
| Package release/API | Distributed content and the API it exposes |
| Compiled artifact ABI | Target architecture, calling contract and realization constraints |
| Bridge protocol/profile | The bridge interaction and supported execution surface |
| Runtime, toolchain and system dependencies | The concrete environment that realizes the target |

An equal version string is not proof that different profiles or ABIs are
compatible. Version constraints must use the published resolver grammar.
Unknown operators and unsupported future versions fail explicitly instead
of widening the accepted set.

## Package container and manifest

The distribution families use a standard ZIP container and a common JSON
manifest encoded as UTF-8. The schema defines the identifying magic, manifest
entry name, family, profile, version and required fields. A separate manifest
profile describes each content family.

The manifest and inventory make these facts explicit:

- Stable package and publisher identity, release version and origin.
- Family, profile and contract versions required by the reader.
- Source, compiled or mixed content posture.
- Relative payload paths, sizes and cryptographic digests.
- Export, type, widget, example, help and palette contributions by reference.
- Dependencies and the phases and locations where they are required.
- Target/ABI metadata for compiled artifacts.
- Trust and rights metadata appropriate to the declared profile.

A distribution manifest references authoritative function, type and widget
contracts. It must not redefine their execution semantics or implement a second
semantic catalog. A palette entry is a presentation contribution pointing to an
export, not a new function identity.

Readers reject duplicate JSON keys, duplicate identities, unsupported profiles
and inconsistent family/extension combinations. The schema explicitly governs
extensions; an unknown field is not an executable capability.

Inventory paths are relative and normalized. Absolute paths, traversal outside
the container, duplicate paths and ambiguous destination aliases are rejected.
Links and special filesystem entries are not silently extracted as ordinary
files. Implementations publish and enforce compressed/decompressed size,
entry-count and nesting limits before activation.

Manifest and signature coverage are defined by the versioned signature profile.
The signature envelope identifies the exact manifest bytes and payload digests
it covers; it cannot recursively require a digest of itself. Package creation
must be reproducible for the inputs and metadata declared by its profile.
An integrity check must inspect the actual payload rather than trusting
inventory text alone.

Reading a manifest is not evidence that ZIP encoding, bounded extraction,
signature verification or installation has been implemented. Those capabilities
require their own implementation and conformance results.

### Current 0.1 intake surface

The [manifest schema](../schemas/manifest.schema.json) and shared
[`contract.py`](../contract.py) are the serialization and validation authorities.
The current identifying values are `schema="graiphic.distribution"` and
`format_version="0.1"`. The `profile` field selects `library`, `project`,
`example`, `bridge` or `driver`; it represents the distribution family without
an additional `family` property. Semantic Frontend profiles have independent
identities and versions.

| Current field | Contract |
| --- | --- |
| `identity` | `publisher`, `package`, exact release `version` |
| `content_mode` | `source`, `compiled` or `mixed` |
| `files` | Relative `path`, `sha256`, `size` and declared `role` |
| `contributions` | Stable `id`, `kind`, contract reference `path`/`sha256`/`symbol`, and `payload` |
| `dependencies` | Identity, `origin`, constraint, `consumption_phases`, `required_location` and `installation_scope` |
| `license` | Declared expression and an inventory-backed license document |
| `required_rights` | Required actions; this is not a grant or authorization |
| `entry_point` | Required for project/example; `.frprj` or `.frog` respectively |
| `targets` | Required for compiled/mixed content; target identity, ABI, artifact digest and exactly one permitted provenance reference |

`source` content excludes compiled artifacts. `compiled` requires an artifact
and excludes files with the source role. `mixed` requires both source and artifact
roles. All required ABI and provenance metadata remain present for compiled and
mixed content.

Each target retains `frontend_provenance`. Only the `bridge` profile may instead
use `build_provenance` for an independently compiled native bridge executable;
such a binary must not invent a Frontend derivation. The two fields are mutually
exclusive, and the chosen relative path must name an inventoried file. Other
profiles still require `frontend_provenance`. This additive manifest 0.1 shape
does not establish the truth of a build receipt or grant execution authorization.

The intake validates a JSON contract and an explicitly supplied expected suffix.
[`builder.py`](../builder.py) produces an input plan with
`executable_build_performed=false`. [`manager.py`](../manager.py) produces an
intake decision with `installed`, `trusted`, `authorized` and `ready_to_execute`
all false. Neither command packs or extracts an archive, resolves or installs
dependencies, compiles a program, verifies the actual payload bytes, or grants
usage rights.

Both consumers preserve `license`, `required_rights` and the entire accepted
manifest in `validated_manifest`. A consumer-specific plan or decision must not
replace that record with a projection that silently discards valid fields.

The current checks compare declared hashes against declared inventory entries.
They do not establish payload integrity on disk. The intake limits the JSON
contract to 4 MiB, the inventory to 4096 files, each declared file to 256 MiB and
the declared total to 1 GiB. These are intake limits, not a qualified decompression
policy. Unknown manifest fields are rejected by the closed 0.1 schema.

`manifest.json` at the ZIP root is the proposed archive entry name for the archive
codec. It is not yet a filename enforced by these two intake commands.
Package acquisition origin and a future resolved lock remain explicit external
context; the top-level identity does not invent an unimplemented origin field.

## Dependency and environment model

A requirement identifies a package or capability, an allowed origin, a version
constraint and compatible platform/ABI information. It separately declares:

| Dimension | Examples |
| --- | --- |
| `consumption_phases` | build, run, deploy, debug, test |
| `installation_scope` | project, user, machine |
| `required_location` | author, build, target |
| Resolution status | unresolved, resolved, unavailable, incompatible |
| Acquisition and activation | available in cache, acquired, installed, activated |

These dimensions must not be collapsed into a single scope string.
A toolchain on the build host is not a runtime library on the execution target.
A shared OS driver is not a project-local package.

All three fields are required independently in intake 0.1. A shared system
dependency uses machine installation scope at its required author/build/target
location; machine is not a fourth location. Rights such as develop and update
are not dependency consumption phases.
The version interval is `minimum` inclusive to `maximum_exclusive` exclusive.
Its endpoints and the declared dependency version must be stable numeric
`major.minor.patch` versions; the declared version must lie inside the interval.
Prerelease/build-metadata dependency versions and free-form range operators are
not resolved by this intake. Future resolver support requires an explicit
contract update. `required_rights` remains distinct from dependency phases.

Resolution records exact versions, origins, package and artifact digests, profiles
and target context in a lock. Changes to origin or digest invalidate the result
even when a display name and version are unchanged. Compatible immutable package
versions may coexist in a cache; each environment selects its own resolved set.
Shared system dependencies require explicit conflict handling.

A probe is read-only, bounded and cancellable. Detection, acquisition, installation,
activation, program admission and successful execution are separate results.
Failure at one stage cannot be reported as success at a later stage.

Offline operation uses only a verified cache and a lock satisfying the applicable
trust and rights policy. Missing content produces an actionable diagnostic.
There is no silent network fallback or replacement by a similarly named package.

## Installation and recovery

Installation follows distinct stages:

```text
inspection → plan → acquisition/staging → verification → durable commit → activation
```

The plan exposes ownership, conflicts, affected environments and external effects.
Ordinary local installation defaults to user scope. Machine scope and OS-driver
installation require an explicit applicable authorization process.
Preview and inspection do not invoke installers.

The transaction records preconditions, intended changes and observed outcomes.
It never deletes unowned user files to make a package appear repaired.
Recoverable intermediate states remain visible until explicitly resolved.
Acknowledging or dismissing a notification does not resolve or remove recovery
data.

A transaction engine may atomically publish files it owns. It must not promise
universal rollback for OS installers, registry changes, services, drivers or
firmware. Those effects require specific plans, observed results, documented
compensations where available, and restart/recovery instructions.
Partial failure is a first-class result.

Installation does not prove activation or target capability. A bridge can be
installed but unavailable, and an installed bridge can correctly reject a
program outside its admitted profile.

## Libraries, types and widgets

Exports and types retain stable identities across presentation changes.
Persistent references are resolved against the accepted package/version/origin.
Snapshots retained for authoring recovery are not substitute execution
authorities for a missing dependency.

Widget contributions distinguish three levels:

1. Appearance of an existing class.
2. Bounded composition of existing classes.
3. A new class with explicit value, property, method and event contracts.

An SVG or palette icon does not define a widget's value or behavior.
Widget profiles reference their authoritative contracts. A new semantic
capability must extend the shared semantic implementation and conformance
suite; it must not create a widget-specific source compiler.

Declarative, bounded content is the default. Native third-party code requires
a separate declared profile, capabilities, activation policy and isolation.
A manifest path must not trigger loading of a private editor C++ plugin.

## Projects, examples and working copies

A transportable project contains an explicitly selected closure of the editable
project, programs and redistributable resources. Dependencies excluded from that
closure remain declared. User profile data, private paths and secrets are not
implicitly included.

The source archive is immutable. Opening creates a working copy. Document-local
identities and their references remain coherent; any project/session identity
regenerated by the operation is explicitly specified and remapped in the copied
closure. Template creation lists the identities it regenerates.
Matching a name in the destination is not authority to reconnect an external
reference.

An example adds presentation, expected results and help references. Preview is
separate from execution. The renderer may inspect data required to display the
diagram but must not run the program or activate third-party code.

Documents are read through their recognized profiles. A private authoring
envelope is not made public by renaming fields. Unknown or incompatible content
requires preservation and an explicit diagnostic or migration, not silent removal.

## Frontend and bridge boundary

A shared public Frontend owns source reading, dependency/type resolution,
structural and semantic validation, FIR generation and source attribution.
Its library and minimal CLI do not require the editor UI or LLVM.
The implementation and precise version used by an IDE are explicitly pinned;
a newly available release is not silently adopted.

A bridge consumes admitted public FIR and declares the FIR/protocol versions,
profiles, operations, types, shapes, memory constraints and effects it supports.
It owns lowering, toolchain integration, deployment and the session on its target.
It does not read an editor canvas or clone a private source-to-FIR generator.

The initial extension boundary is out of process through a versioned, bounded
local protocol. [Protocol 0.1](bridge-protocol.md) specifies framing, handshake,
cancellation and resource budgets. Its process host and transport conformance
remain P09 work. A manifest alone does not establish that this host exists.

The current [Frontend context](../schemas/frontend-context.schema.json),
[capabilities](../schemas/frontend-capabilities.schema.json) and
[bridge description](../schemas/bridge-describe.schema.json) schemas support
contract intake and bridge protocol 0.1 negotiation. Source/FIR family and version,
Frontend software version, API epoch, commit/digest, semantic profiles/operations
and resolved dependencies remain separate. The
[message envelope](../schemas/bridge-message.schema.json) distinguishes request,
session and generation identity. Acceptance of these records neither runs a
Frontend nor starts a bridge process; methods advertised by a descriptor still
need real implementation and qualification.

Program admission precedes Run. Unsupported source objects are identified through
FIR attribution. There is no hidden CPU execution under a GPU or FPGA label.
Mixed realizations require an explicit partition and transfer plan.

Stop, Pause, stepping, probes and execution highlighting are separate capabilities.
A device reset is not automatically equivalent to a CPU Stop. Process failure or
cancellation cannot manufacture successful completion.

## Exact target qualification

Qualification binds evidence to an exact bridge, profile, software/runtime/toolchain,
OS/architecture, hardware or board, driver/firmware, graph and test data.
Numeric precision and permitted differences are explicit.

Inspection, simulation, compilation, deployment and observed hardware execution
are different evidence levels. A simulation does not qualify a physical board.
A named FPGA profile in a schema is not an implemented FPGA backend.
Different vendors and targets require their own evidence.

Unavailable hardware or licenses remain pending, unsupported or blocked with a
reason. A fixture can validate a contract without claiming hardware qualification.
No hardware family is universally qualified by one passing target.

## Trust, rights and service boundaries

Integrity, declared publisher identity, verified signature, local trust,
usage rights and execution capability remain distinct. A valid digest proves
neither publisher identity nor permission to execute.

Local and downloaded packages use the same structural and inventory checks.
Unapproved content can be inspected; activation is governed by explicit policy.
Local unsigned fixtures must not be labeled signed or published.
Signature profiles use established, versioned cryptographic implementations,
with defined coverage and interoperability tests. Expiration, rotation and
revocation have explicit online and offline states.

Rights may distinguish develop, build, deploy, run and update, together with
subject, scope and validity. They are not the payment itself and do not alter
program semantics. Service secrets must not be included in distributable packages.
Opening an already admitted free local document does not require a Store connection.
Commercial offline rights follow an explicitly chosen policy; this baseline
does not invent grace periods or legal entitlements.

Development defaults to local or sandbox rights and commerce adapters.
No payment provider, commission, production account, legal role, hosting provider
or region is selected by this contract. Those production decisions remain pending.
Sandbox evidence is never reported as a real purchase, payout or hosted service.

Registry metadata, immutable artifact storage and rights services are separate
interfaces. A local fixture of those interfaces is not a hosted Store.
A publication version is immutable; corrections create a new version.
Backups require restoration evidence before recovery can be qualified.

## Diagnostics, conformance and publication

Readers and consumers return identifiable diagnostics tied to the package,
dependency, source object or environment that failed. Unknown versions, duplicate
identities, profile mismatches and missing origins are distinguishable failures.
A diagnostic must not be hidden by silently weakening validation.

Each contract has a version, owner, change history and conformance fixtures.
Incompatible changes require an explicit migration or refusal.
Shared catalogs remain the single authority for semantic definitions.

Implementation state and distribution state are independent:

- Implementation: planned, ready, in progress, implemented but unverified,
  qualified, or blocked.
- Distribution: local, candidate, or actually published.
- Check result: pass, fail, skipped, unsupported, or blocked.

A skipped check does not pass. A package being published does not qualify its
hardware targets. Local qualification does not authorize external publication.
Evidence records exact versions, digests, artifacts and the user or API path
actually exercised.

This baseline defines contracts for implementation and testing. It does not
substitute a manifest, a mock or documentation for a completed executable feature.

