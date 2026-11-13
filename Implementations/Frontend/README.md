<a id="frog-frontend--candidate-locale-api-3"></a>

# FROG Frontend — local API 3 candidate

This directory contains the Frontend core extracted from Studio's semantic owners, its C-interface library and standalone CLI. It reads explicitly declared `frog.source` bytes and `frog.document.draft` documents, resolves host-supplied dependencies, validates the language and produces FIR with diagnostics, mappings and provenance. Ordinary, structured, composed and Event producers share this owner. Reading and semantic migrations that leave the original unchanged are shared; repair proposals, previews and transactional application remain private to Studio.

**Local candidate `0.2.0-local`, API 3.** Studio and the CLI use the same readers, validation rules and FIR producers. Results are attributed to an exact snapshot: the six CTest cases and 6,614 checks from the first standalone qualification apply to `c640d266a4de9d9581e6394d80d7152ce3d91d64278b960ab6777a524a056a62`, not automatically to subsequent changes. Each final kit has its own manifest and separate qualification receipt. This is not a published release, whole-palette qualification or clean-Windows qualification. See the [profile](docs/candidate-profile.md) and [shared architecture and maintenance procedure](docs/architecture-and-maintenance.md).

<a id="contrat-public-et-versions"></a>

## Source checkpoint — 9 November 2026

The refreshed sources retain candidate software `0.2.0-local` and public API 3.
Their exact production content is `81a502cc9dcf06dd01f661ee7e5f05924970d70118d287b06959b9b78835278e`,
consumed by Studio `0.0.7.564`. See the [dated source checkpoint](../../docs/frontend-checkpoint-2026-11-09.json)
for the standalone checks actually performed and their limits. This source update
is separate from a tagged binary release and does not update Studio's pin automatically.
The existing Zstandard 1.5.7 document codec now includes its bounded writer as well
as its decoder; original BSD notices and the selected third-party licence remain intact.

## Source checkpoint — 12 November 2026

The current sources retain candidate software `0.2.0-local` and public API 3.
Their production content is `c815f0a1e9d5f466598f5e780cec556d3b046d71b326b98502bee49f7f0df9a9`, consumed by Studio `0.0.8.094`.
The [dated source checkpoint](../../docs/frontend-checkpoint-2026-11-12.json)
separates these source identities from prior finite application tests. No new
standalone public build or tagged binary release is claimed for this publication;
Studio's existing pin is preserved. Original third-party notices remain intact.

## Public contract and versions

Consumers use [`frog/frontend.h`](snapshot/api/frog/frontend.h), installed as `include/frog/frontend.h`, and target `Frog::frog-frontend`. The `include/frog_native_engine` headers, C++ owners and `runOperation` are implementation details shared with Studio, without a guaranteed public C++ ABI.

| Axis | Candidate status |
| --- | --- |
| Software | `0.2.0-local`; base commit and content SHA256 in describe/provenance |
| C ABI | `3`; exact structure sizes, reserved fields zeroed |
| Public Source | `frog.source`, version `0.1` |
| Studio documents | `frog.document.draft`, explicit reader revisions `0` through `9`. See the [writer and recovery coverage matrix](../../docs/studio-source-compatibility.md#coverage-matrix). |
| FIR | `frog.execution_ir`, version `0.1` |
| JSON results | Diagnostics `0.2`, context/provenance `0.3`, capabilities `0.2`, maps `0.1` |
| Semantic profiles | Their own identities and revisions, preserved in FIR/provenance |

API 3 requires explicit, independent Source and FIR identities. `describe` exposes exact formats, principal profiles and limits. Its `profiles_exhaustive: false` list is not a complete function catalog. ABI, software, Source, FIR, profile and bridge-protocol versions are separate axes. Accepted FIR still requires admission by its target realization.

<a id="construire-et-installer-sans-studio"></a>

## Build and install without Studio

Prerequisites: C++20, CMake 3.24 or later, and a configured C/C++ toolchain. The kit contains declaration snapshots and the BSD-licensed Zstandard 1.5.7 decoder; it searches for no private Studio/Runtime checkout, LLVM or numerical provider. The phase-A standalone reconstruction used Windows x64 and MSVC 14.44/19.44 (compiler `19.44.35228`). No other platform or clean Windows machine is declared qualified here. Python is required for enabled CLI tests.

From the kit's **FROG** root, in a prepared build environment with a short build path:

```powershell
cmake -S Implementations/Frontend -B b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TRY_COMPILE_CONFIGURATION=Release -DFROG_FRONTEND_BUILD_TESTS=ON
cmake --build b --parallel 6 --target frog-frontend-cli frog-frontend-c-consumer frog-frontend-integration frog-frontend-draft-integration frog-frontend-draft-dependency-smoke
ctest --test-dir b --output-on-failure -R '^frog-frontend-(c-consumer|cli-scalar|cli-type-definitions|integration|draft-integration|draft-dependency-smoke)$'
cmake --install b --prefix installed
```

Target `frog-frontend-cli` produces `frog-frontend` (`frog-frontend.exe` on Windows). The installed library is static in this build. Select a consistent C/C++ runtime for all targets and the consumer; an MSVC kit may supply `-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded`. The API imposes no universal CRT choice.

To check an installed consumer, copy `tests/frontend_c_consumer.c` into an external project with this `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.24)
project(FrontendConsumer LANGUAGES C CXX)
find_package(FrogFrontend CONFIG REQUIRED)
add_executable(frontend-consumer frontend_c_consumer.c)
set_target_properties(frontend-consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(frontend-consumer PRIVATE Frog::frog-frontend)
```

Configure with `-DCMAKE_PREFIX_PATH=<absolute-path-to-installed>`, build, then run `frontend-consumer`. It includes only `<frog/frontend.h>` and links libraries from that prefix. This path passed for the phase-A snapshot above; library, CLI and consumer hashes appear in `phase-a-result.json`. This recipe does not reattribute that result to a new capture. Inside the Studio workspace, every build remains coordinated by its canonical wrapper and lock.

<a id="utiliser-la-cli"></a>

## Use the CLI

Ninja/Windows example from the FROG root; adjust the `.exe` suffix on other platforms:

```powershell
./b/frog-frontend.exe describe --descriptor-output descriptor.json
./b/frog-frontend.exe validate --source Implementations/Frontend/tests/fixtures/pure-add.frog --source-uri fixture:/add.frog --document-id source:add --fir-document-id fir:add --source-format frog.source --source-version 0.1 --fir-version 0.1 --diagnostics-output diagnostics.json --source-maps-output maps.json --provenance-output provenance.json
./b/frog-frontend.exe derive --source Implementations/Frontend/tests/fixtures/pure-add.frog --source-uri fixture:/add.frog --document-id source:add --fir-document-id fir:add --source-format frog.source --source-version 0.1 --fir-version 0.1 --output result.fir.json --diagnostics-output diagnostics.json --source-maps-output maps.json --provenance-output provenance.json
```

`validate` uses the same producers and sealing checks as `derive`, but does not return FIR. `derive` requires `--output`; `validate` rejects it. `describe` accepts no Source, context or dependency. It writes its descriptor to stdout; the other two operations write their diagnostics array to stdout. Optional outputs are separate JSON documents.

Additional options: `--semantic-context FILE`, `--dependencies LOCK.json`, and `--max-source-bytes`, `--max-dependency-bytes`, `--max-total-dependency-bytes`, `--max-result-bytes`, `--max-nodes`, `--max-connections`, `--max-nesting-depth`, `--max-dependencies`, `--max-diagnostics`. Numbers are unsigned decimal integers. Zero selects the default; a nonzero value may only reduce a limit. Parsing errors and unknown options are rejected before the operation.

Exit codes: `0` success, `2` program rejected, `3` unsupported format/profile, `4` cancellation, `5` limit/resource, `64` argument or CLI/API host error. A rejected derivation does not replace the last valid FIR. Writes publish complete files; all auxiliary files together are not one atomic transaction.

<a id="dépendances-explicites-et-profil-fini"></a>

## Explicit dependencies and finite profile

The core reads no disk, network, registry, environment variables, PATH or installed libraries. The caller supplies bytes and metadata. The resolver returns an immutable capture; its views are released before the operation returns. Only the CLI host reads explicitly selected files and paths from its lock.

`frog.frontend.resolution@1` retains its contract: `TYPE_DEFINITION=2` fragments in actual `studio_type_schema` fields, without a nonempty ConditionalDisable context. The new `frog.frontend.resolution@2` explicitly describes the `built_in`, `project` and `target` symbols used to prepare drafts; no value is implicitly read from Windows or a backend. Draft SubFROG `SOURCE=1` and TypeDef `TYPE_DEFINITION=2` dependencies are resolved from saved references through the same callback, with immutable bytes and verified hashes. This extension does not implement library/profile contracts. The [resolution contract](frontend-resolution.md) distinguishes these paths, envelopes and limits.

```powershell
./b/frog-frontend.exe derive --source Implementations/Frontend/examples/type-definition/main.frog --source-uri fixture:/nominal.frog --document-id source:nominal --fir-document-id fir:nominal --source-format frog.source --source-version 0.1 --fir-version 0.1 --semantic-context Implementations/Frontend/examples/type-definition/context.json --dependencies Implementations/Frontend/examples/type-definition/dependencies.json --output nominal.fir.json
```

Identified fixtures cover scalar addition, exact I64/U64, Record, nested Map/Array/Cluster/Enum, Event and preserved references. Hidden regions remain checked. Widget/diagram DTOs, codecs and resolvers are the extracted neutral owners; they contain no Canvas, controller or private recovery state. This finite corpus does not promise every palette type/structure/function combination or backend execution. Lifetime, concurrency and diagnostic contracts and remaining limits appear in [candidate-profile.md](docs/candidate-profile.md).

<a id="provenance-pin-et-licence"></a>

## Provenance, pin and license

`source-origin.json` retains origin captures and snapshot declarations. `cmake/FrontendInputs.cmake` computes current identity; Studio compares it against its software/API/content pin. `frontend-content-identity.txt` identifies build inputs. Frontend content SHA and executable SHA are different evidence. Local overlays cannot be reconstructed from the base commit alone.

**Apache-2.0 is accepted for the selected original Graiphic files of the common Frontend, CLI and their contracts/examples/tests**, including necessary Studio or Runtime extractions. The [published decision of 1 November 2026](https://github.com/Graiphic/FROG/blob/85a90ebe2c2a78cc4f42f7efce1c8ca990df4d26/sdk/ecosystem/docs/frontend-licensing.md), recorded locally in `FROG-Context/Decisions/frog-common-frontend-apache2.md`, retains third-party licenses/notices and excludes the rest of Studio, private repairs, the rest of Runtime and optimization recipes.

`NOASSERTION` entries in the [E2 inventory](docs/license-inventory.json) and its [observations](docs/provenance-and-licensing.md) describe the historical state before that decision; they do not reopen the Apache choice for the affected Graiphic files. The integrator must bind the candidate inventory to this agreement and retain origin/current hashes and third-party notices for the exact distributed content. The licensing decision neither qualifies a new package nor authorizes publishing its code now.

The Zstandard document decoder and its FSE/HUF/xxHash components remain third-party material. See [its provenance and BSD license](third_party/zstd/UPSTREAM.md); they are not relicensed under Apache-2.0. Their presence qualifies neither the runtime nor reading every historical file.
