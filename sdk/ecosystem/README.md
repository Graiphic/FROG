# Ecosystem contracts — local P01 baseline and P28 API 3

This directory contains executable **contract intake**, the public Frontend API
3 declarations, and schemas for its result documents. The local production
implementation lives in `Implementations/Frontend`; compilation and qualification
are recorded separately. The Python consumers also assemble and inspect the
stored ZIP distribution profile. Installation, bridge hosting, Store and target
execution use their own hosts.

Studio and the API/CLI use the same extracted production C++ source-to-FIR
generator. These Python consumers do not generate FIR and do not replace it.

## Use the two consumers

From the repository root, with Python 3.11 and the existing reference test
dependencies available:

```text
python -B -m sdk.ecosystem.builder sdk/ecosystem/fixtures/library-source.json --suffix .bfrog
python -B -m sdk.ecosystem.manager sdk/ecosystem/fixtures/library-source.json --suffix .bfrog
```

Both read the same manifest through `contract.py`, use the existing strict JSON
reader, and preserve the complete accepted manifest, license and rights
requirements. Exit 0 means contract intake succeeded; exit 2 is a structured
refusal. Neither success grants trust nor proves payload integrity or execution.

Use `Implementations/Reference/requirements-test.txt` in an isolated environment;
the shared validator uses its existing `jsonschema>=4.18,<5` dependency. No new
cryptographic or package installation dependency is introduced.

```text
python -B -m pytest sdk/ecosystem/tests/test_contracts.py -q
```

This command runs only the P01 contract tests, not the repository's global suite.
The historical fixtures use `fixture.only.*` identities. API-2 result fixtures
also contain synthetic identities and hashes; neither set is signed-package or
executable-capability evidence. The 0.1 negotiation fixtures remain historical
and are not silently relabelled as API-2 result documents.

<a id="sdk-bridge-c-local"></a>

## Local C++ Bridge SDK

Public CMake target `frog-bridge-protocol` provides wire codec 0.1,
descriptions 0.1/0.2, negotiation and exchange tracking. Its `frog/bridge.hpp`
header depends only on the C++20 standard library; linkage reuses the extracted
Frontend owner, without Studio or LLVM. See [CMake usage, the minimal consumer,
hashes and limits](docs/bridge-sdk-cpp.md), then [method payloads](docs/bridge-methods.md).
Standalone reconstruction of this Bridge SDK and example remains to be qualified.

## Contracts

- [Architecture and format families](docs/architecture.md)
- [Frontend C API and CLI boundary](docs/frontend-api.md)
- [API-3 result schemas and inspection of actual output](docs/frontend-result-contracts.md)
- [Public C declarations](include/frog/frontend.h)
- [Bridge protocol 0.1](docs/bridge-protocol.md)
- [Schemas](schemas) and [positive/negative fixtures](fixtures)
- [Local changes and compatibility](CHANGES.md)

There are separate version checks for Source, FIR, Frontend API, Frontend software
provenance, semantic profiles/operations and the bridge protocol. No version axis
is used as a substitute for another.

The manifest is bounded to 4 MiB of actual input, 64 nesting levels and 4096
inventory entries. Each declared file is at most 256 MiB and their declared total
at most 1 GiB. Duplicate JSON members/identities, isolated Unicode surrogates,
unsafe paths, unknown fields/versions and contradictory content modes are refused.
These constraints do not implement ZIP extraction or verify declared payload bytes.

## Assemble and inspect a distribution ZIP

```text
python -B -m sdk.ecosystem.builder path/to/manifest.json --payload-root path/to/payload --output path/to/library.bfrog
python -B -m sdk.ecosystem.manager path/to/library.bfrog --archive
```

The archive contains a root UTF-8 `manifest.json` and exactly the regular files
declared in it. It uses single-volume classic ZIP with stored members, matching
local headers and no extra fields or data descriptors. The reader checks the
ZIP byte budget (1 GiB + 16 MiB), central directory (8 MiB), count (4097
including the manifest), manifest and payload limits above, and path depth (32).
Paths must use NFC and may not contain Windows device names, 8.3 aliases or
case-insensitive collisions. Each file must match its declared size and SHA-256.
The builder preserves the manifest bytes and orders payload entries by path,
so identical inputs yield identical archives. Output is committed without
overwriting an existing file, using a hard link on the destination volume.

These commands do not install, compile, run, or grant trust or rights. The
builder packages already available artifacts. Hosts can use `stage_archive`
to get a verified temporary directory for a subsequent transaction. The
staging directory is removed on cancellation, failure or context exit.
Signature verification and native installation remain integration work.

## Resolve a library closure offline

```text
python -B -m sdk.ecosystem.manager resolve-library toolkit.bfrog --origin local:team --phase run --candidate local:team dependency.bfrog
```

Supply each candidate with its approved origin. The resolver reads and verifies
the local archives, follows the manifest's exact dependency versions, and emits a
deterministic JSON receipt on stdout. The receipt retains phases, required
locations, installation scopes, verified manifest digests and target inventories.
Use `--expect-lock lock.json` to require the same closure and content. Separate
invocations can select different versions from the same cache.

This offline SDK operation does not acquire packages or probe a machine. The
declared origin must come from the caller's policy. It does not attest publisher
identity. Studio's project locks, target admission and activation remain with the
native integration.

New contract code follows this repository's Apache-2.0 license. Local acceptance
does not authorize publication of private implementation files.


## Local Apache-2.0 candidate

The [accepted license policy](docs/frontend-licensing.md) covers the selected
Graiphic-owned common Frontend/CLI files, including retained Studio/Runtime
origins. Historical NOASSERTION observations are kept as history; third-party
materials retain their own attribution. See the exact [candidate scope](../../Implementations/Frontend/docs/candidate-profile.md) and
[license inventory](../../Implementations/Frontend/docs/license-inventory.json).
This local preparation sends no implementation to GitHub. The earlier
[contracts-only publication](https://github.com/Graiphic/FROG/commit/85a90ebe2c2a78cc4f42f7efce1c8ca990df4d26)
is retained independently of the candidate's technical qualification.
