<a id="utiliser-le-sdk-bridge-c-local"></a>

# Use the local C++ Bridge SDK

The entry point is [`frog/bridge.hpp`](../include/frog/bridge.hpp), a C++20 header
depending only on the standard library. Existing CMake target
**`frog-bridge-protocol`** provides JSON codec, negotiation and `Exchange`; it
creates no process and executes no program. The third-party binary contract remains
UTF-8 IPC, not this header's C++ ABI.

The local E5 rebuild used a standalone SDK + Frontend copy, without Studio, private
Runtime or LLVM build dependencies. The consumer below and protocol test passed
(2/2). Evidence:
`FROG-STUDIO/native-portable-engine/artifacts/modularisation-etape5-bridges-2026-11-01/`.
This qualification uses already installed Windows/MSVC; it qualifies neither clean
Windows, Linux nor a third-party bridge actually performing a computation.

<a id="cmake-et-consommateur-minimal"></a>

## CMake and minimal consumer

CMake 3.24 or later and a C/C++20 toolchain are required. Current integration uses
`add_subdirectory`; current Bridge CMake provides no installed
`find_package(FrogBridge)` package. In the consumer's `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.24)
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif()
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
project(BridgeConsumer LANGUAGES C CXX)
include(CTest)
set(FROG_BRIDGE_SDK_ROOT "" CACHE PATH "Path to the sdk/ecosystem SDK")
add_subdirectory("${FROG_BRIDGE_SDK_ROOT}" frog-bridge-sdk EXCLUDE_FROM_ALL)
add_executable(bridge-encode-describe main.cpp)
target_link_libraries(bridge-encode-describe PRIVATE frog-bridge-protocol)
add_test(NAME bridge-external-consumer COMMAND bridge-encode-describe "${CMAKE_BINARY_DIR}/describe.frame")
```

Create this `main.cpp` in the consumer project. It writes a correctly framed
`describe` request to a binary file; it is neither a process host nor target-execution
evidence.

```cpp
#include <frog/bridge.hpp>
#include <array>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) return 2;
    try {
        frog::bridge::Message request;
        request.identity = {"example.describe", "example.session", 1};
        const auto json = frog::bridge::encodeMessage(request);
        frog::bridge::Message decoded;
        frog::bridge::Error error;
        if (!frog::bridge::decodeMessage(json, decoded, error) ||
            !frog::bridge::validatePayload(decoded, error)) {
            std::cerr << error.code << ": " << error.message << '\n';
            return 3;
        }
        const auto length = static_cast<std::uint32_t>(json.size());
        const std::array<char, 4> header{
            static_cast<char>(length & 255),
            static_cast<char>((length >> 8) & 255),
            static_cast<char>((length >> 16) & 255),
            static_cast<char>((length >> 24) & 255)};
        std::ofstream output(argv[1], std::ios::binary);
        output.write(header.data(), header.size());
        output.write(json.data(), json.size());
        output.close();
        return output ? 0 : 4;
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 5;
    }
}
```

In an already prepared build environment with a short build path:

```text
cmake -S . -B b -G Ninja -DCMAKE_BUILD_TYPE=Release -DFROG_FRONTEND_BUILD_TESTS=OFF -DFROG_BRIDGE_SDK_ROOT=C:/kit/FROG/sdk/ecosystem
cmake --build b --target bridge-encode-describe
b/bridge-encode-describe.exe describe.frame
```

Executed qualification: CMake 3.31.6, Ninja 1.12.1, MSVC 19.44.35228 / tools
14.44.35207, Windows SDK 10.0.26100.0, C++20 Release. The consumer emits a 185-byte
`describe` frame; its executable imports only Kernel32, UCRT and MSVC 14.x standard
libraries. These remain required for these `/MD` binaries; no universal C++ binary
compatibility is claimed. The source kit is the reconstructible reference.

`FROG_BRIDGE_FRONTEND_ROOT` defaults to `FROG/Implementations/Frontend`. This CMake
parameter may select a complete local copy of that same owner. Private link
dependency `frog-frontend-production` reuses its actual JSON/SHA; the consumer
includes none of that owner's headers. The library target above requires no Studio,
LLVM, private Runtime, Python or `jsonschema`. Python tools are separate schema consumers.

<a id="fichiers-à-conserver-dans-un-kit-local"></a>

## Files to retain in a local kit

The minimum with current CMake files includes:

- `sdk/ecosystem/CMakeLists.txt`, `include/`, `src/` and `schemas/`;
- `sdk/ecosystem/tests/bridge_protocol_smoke.cpp`, because its target is declared
  even when excluded from the default build;
- `Implementations/Frontend/CMakeLists.txt`, `production-sources.cmake`,
  `source-origin.json`, `cmake/` and every inventoried path: `src/`, `include/`,
  `manifests/`, `snapshot/sdk/`, `snapshot/generated/` and `snapshot/api/`.
  Also retain C API/CLI sources referenced by CMake.

Copying only `FrogJson.cpp` and `Sha256.cpp` is insufficient: CMake links the production
owner and verifies its inventory. It searches for no Studio/Runtime checkout when
a complete local copy is supplied. To build and run `frog-bridge-protocol-smoke`,
also retain `sdk/ecosystem/fixtures/bridge-*.json` and
`Implementations/Frontend/tests/fixtures/`; the test uses actual API 3 derivations
of public sources.

Retain notices and [Frontend provenance/license observations](../../../Implementations/Frontend/docs/provenance-and-licensing.md).
A local copy and hashes change neither rights to extracted bodies nor grant general
redistribution authorization.

<a id="versions-et-empreintes-p28"></a>

## P28 versions and hashes

| Axis | Current contract |
| --- | --- |
| Source / FIR | `frog.source` / `frog.execution_ir`, each `0.1` |
| Frontend C API / software | API `3` / software `0.1.0-local`, independent |
| Frontend provenance / capabilities | `frog.frontend.context` `0.3` / `frog.frontend.capabilities` `0.2` |
| Bridge wire / description / payload | `0.1` / historical `0.1` or explicit `0.2` / `0.1` |
| Frontend / Bridge diagnostics | `frog.diagnostic` `0.2` / `0.1`, not interchangeable |
| Semantic profiles | Full ID and own `.vN` or `@N` version; no version inferred from FIR |

`graphRequirements(provenance, fir, out, error)` consumes complete documents actually
emitted by API 3 and preserves their provenance. Source `document_id` remains distinct
from FIR `document_id`. Hashes are:

- `firSha256`: exact FIR UTF-8 bytes;
- `contextSha256`: exact provenance bytes; to insert it as an object in a payload,
  use the API's exact JSON token without adding surrounding whitespace/newlines;
- `frontendFingerprint`: SHA-256 of the canonically encoded `frontend` object,
  containing software, API, commit and P28-owner SHA.

Frontend CMake produces `frontend-content-identity.txt` from current inventory inputs.
This SHA is not the Bridge package/backend hash. Studio's source lock separately
records the SDK's `include`, `src`, `schemas` trees and `CMakeLists.txt`. A hash proves
byte identity, not trust or execution admission.

<a id="transport-méthodes-et-limites"></a>

## Transport, methods and limits

Each frame is a little-endian uint32 followed by that many UTF-8 JSON bytes. Check
size before allocation, reserve stdout for frames and separate stderr.
`decodeMessage` preserves the historical envelope; call `validatePayload` before use.
`Exchange` tracks the exact request/session/generation tuple, bounds progress and
accepts only one final response. Stale or cancelled outputs are not published.

`describe` receives `{}` and directly returns a Description. `probe` describes a
service without admitting a graph. `validate` receives full FIR, provenance and
selection; its receipt must precede `lower`, `build`, `deploy` or `run/start`.
`stop`/`observe` use an opaque run handle bound to session/generation. Pause, step,
controls and Events are explicitly advertised `run` commands. A non-exhaustive
operation list defers the decision to `validate`, without authorizing Run. See
[detailed payloads and rules](bridge-methods.md).

| Resource | Default | Advertised bounds / maximum |
| --- | ---: | --- |
| JSON frame | 1,048,576 bytes | 1,024 to 16,777,216; minimum of host/bridge limits |
| Request timeout | 30,000 ms | 1 to 300,000 ms; minimum of host/bridge limits |
| Progress messages | 256 | 1 to 65,536 in Description 0.2 |
| Retained stderr | 65,536 bytes | 0 to 1,048,576 in Description 0.2 |
| Envelope diagnostics | — | 4,096 |
| Format/profile/operation/capability lists | — | 4,096 entries; strings at most 512 characters |
| Dependencies / provenance transformations | — | 4,096 entries each |
| Request/session IDs, tokens, diagnostic messages | — | At most 2,048 characters |
| Provenance Source/FIR URIs/identities | — | At most 8,192 characters |
| Shared JSON | — | Depth 128, 1,000,000 values; frames remain bounded to 16 MiB |

Character limits count Unicode scalars; frames and hashes use bytes. Generations
are exact uint64 values. Transport remains responsible for timeouts, EOF, crashes,
cancellation and termination; the codec library creates no thread or timer.
