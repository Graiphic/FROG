# Local source-document library profile

This first profile reuses distribution0.1, the existing ZIP codec, the offline
dependency resolver and the document's real callable interface. It does not
extend `spec_version`. Package verification does not qualify installation, FIR,
Run or an exported executable. The bounded source workflow below was accepted
in signed Graiphic Studio **0.0.8.612**, Windows x64, on 17 November 2026.

The manifest has `profile: library`, `content_mode: source` and explicit export
contributions. Each export has a stable ID and symbol. Its contract and payload
reference the same `.frog` file and exact hash: this bounded profile treats the
existing callable document as one unit. It does not publish a separate ABI or
pretend to implement compiled/mixed libraries or API-only contracts. The general
distribution schema continues to admit those other profiles independently.

Publisher, package, exact version, export ID, symbol, allowed local origin,
manifest hash and callable-source hash are retained in a canonical `bfrog:///`
reference. Labels, filenames and palette positions are not identities. An absent
or changed entry is refused; no `latest`, similar filename or alternate symbol
is substituted. A source edit requires an explicit package/lock decision.
Rights in the manifest are requirements, not grants or publisher authentication.

The package manifest defines which document entry a symbol exports. The common
Frontend remains the owner of that document's actual types, ports, semantics,
diagnostics and FIR. SDK archive inspection does not infer types from UI geometry
and does not make an unsupported source executable.

## Reproduce the source archive

From a FROG checkout with its SDK Python dependencies, choose an existing output
directory and run:

```text
python -B sdk/ecosystem/examples/source-document-library/build.py --output <output-directory>/addition.bfrog --origin local:studio --consumer <output-directory>/consumer.frog
python -B -m sdk.ecosystem.manager list-source-exports <output-directory>/addition.bfrog --origin local:studio --staging-parent <existing-temporary-directory>
```

The producer calls the same `build_archive` and `capture_source_library` used by
the CLI. Existing destinations are never replaced. The sample includes the
Apache-2.0 license and a byte-identical Studio-authoring document with inputs
`x`, `y` and output `sum`, all `f64`. It explicitly uses `frog.document.draft`2;
it is not advertised as a canonical Source0.1 program. The example operation is
addition. No private DLL, user archive, absolute author path or binary artifact
is included. The producer prints archive/source identities and explicitly
reports that installation and execution have not occurred.

`list-source-exports` verifies the real archive and its dependency closure before
returning the full admitted manifest, pinned references and lock. Its temporary
staging is owned by the caller and removed normally. Compiled/mixed payloads,
separate API files and unhandled contribution kinds are refused by this host
profile, without changing their general schema admission.

## Studio workflow

1. Open an editable Block Diagram, then **Tools → Local Libraries… → Install .bfrog…**.
   Select the archive. Studio uses its bundled isolated Python/SDK and the native
   transaction owner, with no system Python or PATH requirement. The local
   development origin is `local:studio`; this is not publisher authentication.
2. Select the exact package/version/export in that same catalog and **Place Call**.
   Placement uses the existing SubFROG preview, connector, Create Control/Create
   Indicator commands, wires and Undo transaction. Escape cancels placement.
3. Wire `x`/`y` and `sum`. The independent oracles are `7+11=18`, `-2+5=3` and
   `1.5+2.25=3.75`. The comments and labels do not participate in the calculation.
4. Save and reopen the consumer. Its `bfrog:///` reference remains pinned and
   independent of the consumer's folder. An unavailable/incompatible package
   must remain a localized error, preserving its call and wires.
5. Use the normal Build command. This source profile is compiled through the
   existing common Frontend/native pipeline, with the package's source captured
   for Build and its attribution included alongside the executable. A consumer
   EXE is not a compiled `.bfrog` library/ABI claim.

The local root is `%LOCALAPPDATA%/GraiphicStudio/Libraries`. Installed versions
are immutable: the same identity/version/origin with different manifest bytes
is refused. Package activation is journaled by the existing native transaction
engine. After acceptance, the UI waits for its actual publication result rather
than offering a misleading cancellation of a committed operation. Cancelling
file selection or placement does not change the document.

An installed export is pinned to its exact bytes. Do not edit its installed file
as a substitute for a new library version. An open, changed source snapshot is
refused rather than silently replacing it. Editing the consumer remains normal.

The standalone install CLI delegates to that same native owner:

```text
python -B -m sdk.ecosystem.manager install-source-library addition.bfrog --origin local:studio --root <absolute-local-library-root> --native-installer <absolute-Studio-directory>/frog-bridge-manager.exe
```

This first activation host supports **self-contained source** libraries. P03's
transitive offline resolution remains available in the SDK; transitive package
activation, compiled/mixed payloads, separate API-only contracts, publisher trust,
Store and commerce are not qualified by this workflow. Native resources supplied
by Studio retain their existing contracts and realization owners.

The bundled validator dependencies are jsonschema4.26.0,
jsonschema-specifications2025.9.1, referencing0.37.0, attrs26.1.0 and
rpds-py2026.9.1 (MIT). Their actual wheel licenses/metadata are retained in the
isolated Python runtime. Studio uses the existing Python3.13.13 importer bundle;
these inputs do not install packages into system Python. The sample/SDK code is
Apache-2.0. Package verification alone does not qualify Run or a relocated EXE.

## Accepted scope and compatibility

The acceptance fixture is `org.graiphic.examples/source-addition/1.0.0`, export
`addition`, symbol `addition.entry`, origin `local:studio`, with three real `f64`
ports. Its archive SHA-256 is
`2e77d707f5654f3e36f5b566f85e966bd22f0d5243b065c3db93a431da89e8af`.
Studio's installed catalog, ordinary call placement and Create commands produced
an editable consumer with three wires. Run and Save/Close/Open/Run were exercised
in the signed application. A normal GUI Build from 0.0.8.612 produced a native
consumer executable; its moved delivery folder returned `18`, `3` and `3.75`
for the inputs above with Studio closed, an empty PATH, a fresh profile and the
development library catalog withheld. The executable carried its required
attribution and did not need an installed library catalog to run this sample.

This is a **source-document@1** acceptance, using the existing
`frog.document.draft`2 producer and the common Frontend API3, content identity
`690ec6a8c09832cd998c99d29612c06eec5fe03832917e7ea01a42f687b3816a`.
The canonical Source0.1 public-interface witness still receives
`IR_PUBLIC_INTERFACE_UNSUPPORTED` in that producer path. This host profile does
not change that language contract or make every admitted JSON document callable.
The compiled consumer EXE is not a compiled-library `.bfrog` payload.

Tests also cover exact identity/version/origin/symbol pinning, changed bytes,
missing dependencies and unsupported modes without substitution. Broader type
coverage, private helpers, TypeDefs, library palettes, transitive activation,
compiled/mixed ABI and arbitrary deployment targets remain outside this finite
acceptance. No general clean-Windows or all-version compatibility is implied.
