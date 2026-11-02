# Reference C bridge

`frog-reference-c-bridge.exe` is a finite Windows x64 realization of public FIR. It reads the public bridge wire protocol, validates the complete graph, generates C17, invokes an explicitly configured MSVC19.44 compiler and linker, and runs the resulting executable. The peer validates and returns that executable's complete typed outputs. It does not read `.frog`, link Studio/Runtime/LLVM, or compute graph results in C++.

`profile.json` is the compiled discovery description. Its operation list is declarative (`operations_exhaustive:false`, `constraints.deferToValidate:true`), never permission to run. Only complete-graph `validate` owns the strict finite whitelist and the responsible FIR/Source refusal. This source addition has not itself been built or executed by its author; qualification belongs to the integrating canonical build and corpus run.

## Public source build

The local source kit needs this directory, `FROG/sdk/ecosystem` (headers, implementation, schemas and CMake), and its exact public `FROG/Implementations/Frontend` source closure. The SDK owns JSON parsing, encoding and SHA256; this peer includes only `frog/bridge.hpp` and `frog/bridge_json.hpp`. No private Studio or Runtime source is a consumer prerequisite.

From a configured x64 MSVC host environment:

```powershell
cmake -S FROG/Implementations/Bridges/reference-c -B .reference-c-build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build .reference-c-build --target frog-reference-c-bridge
```

`FROG_BRIDGE_SDK_ROOT` can select the installed public source kit; its `FROG_BRIDGE_FRONTEND_ROOT` selects the exact public Frontend owner. The compiler used to build the peer is separate from the exact compiler configured for generated kernels.

The peer inherits `CMAKE_MSVC_RUNTIME_LIBRARY` with the public SDK. A standalone kit may set it consistently to `MultiThreaded`; without an override the canonical build keeps the ordinary CMake dynamic runtime. Generated C kernels always use their independent `/MT` configuration.

## Compiler configuration

Launch with `--compiler-config compiler.json`. The configuration path is relative to the installed peer executable and cannot contain `..`. Configuration bytes are hashed once and retained as immutable admission authority. Paths inside the configuration are absolute local paths; no PATH lookup is used.

```json
{
  "schema":"frog.bridge.reference_c.compiler",
  "schema_version":"0.1",
  "compiler_path":"C:/tools/MSVC/14.44/bin/Hostx64/x64/cl.exe",
  "compiler_sha256":"<exact lower-case SHA256>",
  "compiler_version":"19.44.<exact-build>.<exact-revision>",
  "msvc_version":"14.44.<exact-build>",
  "sdk_version":"10.0.<exact-build>.0",
  "include_paths":["C:/tools/MSVC/14.44/include","C:/tools/WindowsKits/ucrt"],
  "library_paths":["C:/tools/reference-c/lib"],
  "files":[{"path":"C:/tools/MSVC/14.44/bin/Hostx64/x64/cl.exe","sha256":"<exact lower-case SHA256>"}]
}
```

The illustrative inventory above is incomplete. The actual inventory must cover cl.exe, its c1.dll/c2.dll and sibling link.exe, their required non-OS dependencies, **every file** under the explicit include/library search paths, and the selected C runtime/system libraries. A small isolated library directory can contain libcmt.lib, libvcruntime.lib, libucrt.lib, oldnames.lib and kernel32.lib. Missing, changed, duplicate or unlisted search-tree files refuse admission. Reparse points and network paths refuse admission. Each declared file is streamed through Windows SHA256 and held under a read lease for the peer lifetime. File count is bounded to8192, individual files to256MiB, search-tree entries to32768. The compiler's PE file version must equal the configuration and begin `19.44.`.

Windows and its OS loader/API DLLs remain explicit environmental prerequisites. This inventory is not a claim of a clean Windows machine or a redistributable MSVC/SDK license. Compiler ownership, redistribution and source-kit licenses remain unchanged.

The compiler runs with `/std:c17 /MT /fp:strict /O2`, explicit include/library paths and an isolated environment. `CL`, `_CL_` and ambient PATH are not inherited. The exact sibling linker is invoked directly. Each compilation/run uses a fresh private temporary directory. Child trees use a kill-on-close Job, a512MiB process limit, bounded stdout/stderr files and deadlines. The surrounding host Job supplies active cancellation.

## FIR and methods

Use the real standalone Frontend CLI to produce FIR and API3 provenance; for example:

```powershell
frog-frontend derive --source examples/core-add.frog --source-uri fixture:/reference-c/core-add.frog --document-id reference-c:source:core-add --fir-document-id reference-c:fir:core-add --output core-add.fir.json --provenance-output core-add.provenance.json --diagnostics-output core-add.diagnostics.json --source-maps-output core-add.maps.json
```

The transport is public wire0.1: a4-byte little-endian UTF-8 JSON byte count followed by that many bytes. No stdout text surrounds frames. `describe` returns the direct0.2 description. Other methods use `frog.bridge.payload/0.1`; diagnostics remain bridge0.1 admission/build/run diagnostics. They are independent of Frontend API/software/diagnostic versions.

`validate` requires exact FIR bytes, complete API3 context0.3 provenance and selection `graiphic.reference-c/windows-x64/native-fast`. It binds an opaque receipt to exact FIR/provenance hashes, Frontend fingerprint, document identity, selection, compiler configuration and session generation. `lower` creates C; `build` invokes MSVC; `run` with command `start` executes the compiled kernel (building once if required). Handles cannot cross session/generation or graph identities. `observe` and `stop` acknowledge retained **completed** activations only. The peer is synchronous and does not advertise Pause, step, Highlight, Event delivery or an independently running session.

The finite profile supports bool/f64/i32/u32/i64/u64, scalar constants, root controls/indicators, checked finite Core Add, exact IEEE/wrap Add, rank-one f64 Build/Index/Size with at most32 elements, and static For counts0..32 with explicit initialized shift registers. For bodies have no widget effects. Shift histories preserve exact types and simultaneous updates. Ordinary tunnels, Case, While, Event, nominal/enum/record/string/path/reference types and other operations refuse with FIR/Source attribution. A physical carrier never replaces a nominal schema.

f64 inputs/constants must be finite with magnitude at most1e100. A graph executes at most256 f64 additions, including static For multiplicities, so admitted finite additions remain finite; no NaN/infinity domain is claimed. Integer JSON tokens remain exact through i64/u64 extremes; signed arithmetic wraps through unsigned bit-preserving carriers. The constraints in the description give all graph and resource limits.

## Kernel values

Run parameters use `frog.bridge.kernel.inputs/0.1` with `controls:[{widget_id,type,value}]`. Type is the exact FIR `type_ref`. Absent controls use admitted defaults. Unknown, duplicate, mistyped or out-of-domain controls refuse before execution. The peer writes these validated values as a bounded typed binary input file; generated C reads the values and performs all graph arithmetic.

The returned `values` document is `frog.bridge.kernel.outputs/0.1`, with `outcome` and `outputs:[{node_id,port_id,type,value}]`. Completed execution returns every root out port plus every root indicator input `value`. For contributes only its exterior shift-register results. No body-only iteration/read ports escape. Arrays use `{"shape":[N],"elements":[...]}`. i64/u64 values remain JSON integer tokens; f64 output retains signed zero.

An out-of-range Index returns `outcome:"fault"`, `outputs:[]` and `diagnostic:{code:"ARRAY_INDEX_RANGE",message,object_id,source_pointer}`. Results are validated as a complete inventory before publication; no partial outputs or fallback values are fabricated. `examples/corpus.json` and `tests/check_outputs.py` contain the independent static corpus and comparison contract owned by integration tests.

## Retaining qualification artifacts

Before Stop/close, request Observe with `parameters:{"include_artifacts":true}`. This opt-in finite contract0.1 adds `observations.artifacts:[{name,sha256,encoding,data}]`. It exports the generated `kernel.c` as UTF-8, `kernel.exe` as hex, and the exact command/result/stdout/stderr files created by this peer for cl, link and kernel execution. Files are addressed only by their recorded base names under that graph's private workspace; no arbitrary path or other file is exportable. Export is limited to64 files and4MiB of combined **original decoded bytes**; exceeding either limit refuses export without changing the completed run. Ordinary Observe and Run do not copy artifact buffers into responses.

Command records are `<role>-<nonce>.command.json`, where role is `cl`, `link` or `kernel`. Their `frog.bridge.reference_c.command/0.1` document records the explicit application, working directory, argv (including argv0), compiler configuration SHA256, deadline and the exact `CreateProcessW` command-line buffer as `command_line_utf16le_hex`, including its terminal NUL. Matching `.result.json` records contain the actual exit code, elapsed time and hashes of command/stdout/stderr. `.stdout` and `.stderr` use hex export to preserve bytes independently of the compiler's diagnostic code page. The command and result JSON records use UTF-8 export.

The host decodes each entry, verifies its SHA256, and writes the original bytes into its durable evidence directory before closing the peer. It can compare `kernel.c`/`kernel.exe` with `observations.generated_c_sha256`/`executable_sha256`. Workspace files are removed when the peer closes; opt-in export supplies the durable copy without turning executable paths into runtime handles.
