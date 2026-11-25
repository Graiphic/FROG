# FROG importers

These Apache-2.0 tools produce editable FROG documents. The common Frontend
owns validation and FIR derivation. Studio uses these same scripts.
The examples below use PowerShell and absolute tool paths where appropriate.
Install the [pinned ONNX dependencies](requirements-onnx.txt) into the Python
environment used to run the ONNX importer.

In Studio, open **FROG Properties → Documentation → Description** to see the
original import path. The first line identifies the source language or format
and the original file on the importing machine. Each translated LabVIEW member
identifies its own VI, including extracted diagrams. C functions identify their
defining file. Rust documents identify the selected crate or source file and
the translated function. This text travels with the document on Save As or
copy. It is editable and can include a local user or network folder name.

## Rust source and Cargo packages

Use Python 3.11 or later, rustc **1.96.0**, and the common FROG Frontend.
For a Cargo manifest the importer uses the matching Cargo executable from the
toolchain. The Rust compiler's human-readable MIR is version-specific.

```powershell
python Import-Rust.py --source Cargo.toml --entry total --analyze --frontend frog-frontend.exe --report analysis.json
$rustImport = Get-Content -Raw analysis.json | ConvertFrom-Json
if ($rustImport.compatible) {
    python Import-Rust.py --source Cargo.toml --entry total --frontend frog-frontend.exe --expected-sha256 $rustImport.source_sha256 --output imported-total
}
```

Open `imported-total/main.frog`. Each reachable helper is an editable sibling
document. Keep the directory together when moving the result. The result also
includes the FIR, Frontend source maps and the identities of the compiler inputs.

Select a standalone `.rs` or a package's `Cargo.toml`. A workspace root with
a single default member is also accepted. Cargo supplies the actual target path
and edition through `metadata --frozen --no-deps`. A library takes precedence
over binaries. A binary-only package must identify a single binary. The compiler
resolves local Rust modules and the package's default features. The entry name
can be omitted when a single root function can be inferred. An ambiguous choice
lists the compiler's function names. Standalone files default to edition 2024.

The default arithmetic profile is **release**. The report records the target,
enabled features, overflow checks and debug assertions. Use `--profile dev` for
the checked development configuration. A reachable checked operation without an
equivalent FROG operation produces a diagnostic. The scalar profile covers
8-, 16-, 32- and 64-bit signed and unsigned integers, Booleans, comparisons,
pure branches and direct calls. Standalone F64 functions can add, subtract,
multiply and compare. The F64 call boundary and F32 do not preserve all
non-finite values on the current native backend and are outside this profile.
Counted I32/U32 loops of the form `i = 0; while i < n { …; i += 1; }`
can become editable For loops with shift registers when the bound is invariant
and the body is in the supported scalar profile.
An unsupported signature, instruction or effect in the selected
call tree blocks conversion. Packages requiring a build script, procedural macro
or external dependency require further translation support. Import does not run
the selected program. The existing destination and the input files are preserved.

### Structured Cargo projects with Charon

Open **File → Import → Rust…**, select the package's `Cargo.toml` and enter the
function's path (for example `algo::frog_acceptance_score`). Click **Analyze**,
then **Import**. When the scalar translator cannot represent the selected Cargo
function, Studio tries the installed Charon backend on the same reviewed source
snapshot. A Charon analysis identifies itself as `charon-llbc` and lists the
editable functions. In the CLI, `--backend charon` selects it explicitly.
`--charon PATH` selects a specific installation.

The extractor is [Charon 0.1.323](https://github.com/AeneasVerif/charon/tree/ab5b6d60f53691bc4edee3b11cc52656c090071d),
revision `ab5b6d60f53691bc4edee3b11cc52656c090071d`, with
`nightly-2026-09-17`. Keep the stable compiler listed above for the source
review. On Windows, from a short checkout of that revision, install both
executables into the usual Cargo user bin directory:

```powershell
rustup toolchain install nightly-2026-09-17 --profile minimal --component rustc-dev --component rust-src --component llvm-tools-preview
cargo +nightly-2026-09-17 install --locked --path charon --no-default-features --features rustc --bin charon --bin charon-driver
```

The reviewed Cargo target, edition, enabled local features and overflow settings
apply to extraction. The extended profile translates fixed-size arrays and
Rust structures into array and cluster operations, and writes through a
supported mutable borrow return to the owning place. A successful import
saves `source.llbc`, its SHA-256 and the extractor revision with the editable
FROGs. Unsupported effects or types in a reachable function stop the import.

## Other importer commands

```powershell
python -m pip install -r requirements-onnx.txt
python Import-OnnxGraph.py model.onnx --frontend frog-frontend.exe --output new-onnx-folder
python Import-QmhExpressionCheck.py --source expression.c --clang clang-cl.exe --frontend frog-frontend.exe --output new-qmh-folder
python Import-CNativeCall.py --source forward.c --library sample.dll --entry forward --symbol sample --clang clang-cl.exe --readobj llvm-readobj.exe --frontend frog-frontend.exe --capture-library-identity --output new-dll-folder
python Import-QmhWin32Panel.py --source calculator.c --clang clang-cl.exe --frontend frog-frontend.exe --output new-panel-folder
```

- ONNX admits IR 7–13 and ai.onnx opsets 7–25 for the verified schemas:
  Boolean And/Or/Xor (schema 7), Not (schema 1), with no attributes.
  Scalars `[]` and fixed rank-one BOOL tensors `[1..64]` remain distinct.
  Typed and raw inline BOOL initializers are decoded by the pinned SDK;
  declared initializer inputs remain overridable. Equal-shape arrays and
  scalar-to-array broadcasting are supported. Different array lengths,
  symbolic/unknown shapes, higher ranks, external weights, custom domains,
  functions/training/configurations and other operators are explicitly refused.
  Bounds: 1 MiB source, 24 nodes, 8 inputs/outputs, 64 declarations.
  Original IR/opsets/bytes are retained; no version renumbering is performed.
  Open `model.frog`. Its mapping and the captured input accompany it.
- The bounded QMH profile admits the exact `checked` and `fail` AST, including
  the sibling header. Open `checked-editable.frog` for Studio editing.
  The directory also contains the established Source and its Frontend receipts.
  `product`, `factor` and `expression_evaluate` are not admitted by that profile.
  Their refusals identify the reachable members and the source locations of
  recursive calls and C runtime effects. A missing entry is distinct from an
  unsupported definition. No executable partial document is emitted.
- The DLL profile accepts one direct C wrapper with one to four scalars by
  value, a scalar return, Windows x64 cdecl and a named, non-forwarded export.
  The DLL must have no PE imports or delay imports. The caller declares
  `--sha256` or selects `--capture-library-identity` to pin the selected file.
  Keep the copied DLL with the generated document. Import inspects the file
  without loading or executing it.
- The Win32 profile preserves the matching calculator panel as editable widgets.
  The mapping records each imported field and the native behavior still
  unbound. It attributes the shared queue call sites, close/Abort source order
  and the transitive producer/consumer check for error EDIT handle access.
  A changed binding is refused at its C source location; editable error fields
  are not inferred to be computational inputs. These observations do not
  bind callbacks to FROG events or qualify the complete evaluator.
  Open `qmh-front-panel.frog`.

Each destination must be new. Refusals include the responsible phase or source
location where available. Clang runs in syntax-only mode. For C sources using
Windows or C runtime headers, install the compatible Microsoft SDK/toolset.
Repeat `--include` for explicit include directories when needed.

See [NOTICE](NOTICE) for dependency versions and licenses.

## Import LabVIEW projects and RSRC files

Choose an XML `.lvproj` in **File → Import → LabVIEW…** to open a new
`.frprj` in Project Explorer. The local project importer reads explicit
members from **My Computer**, keeps the virtual folder tree, and converts
supported VIs and control definitions to `.frog` and `.ftype`. A linked
front-panel control retains a relative reference to the converted definition.
Each VI undergoes Frontend validation against its actual dependencies before
publication. Missing members, unsupported target items and untranslatable VIs
produce a diagnostic. Keep the project and its member directories together.

In Studio use **File → Import → LabVIEW…**. Choose a `.vi`, `.ctl` or `.llb`
to inspect its resources. **Import** is available when a VI has a supported
source diagram and the common Frontend admits its editable FROG projection.
The result contains `model.frog`, a resource report and a mapping of original
object, terminal and wire UIDs. The selected file is read-only throughout.
LabVIEW error clusters with the ordered fields `status` (Boolean), `code`
(I32) and `source` (String) use the Studio palette's Error Cluster Control
or Indicator according to their original role. Import preserves the source
label, field values and connector bindings, including warnings with false
status and a nonzero code.
The review window identifies the selected file, explains its import status
in the current Studio language and lists the known reasons execution is blocked.
Use **Save report…** to export the current result as UTF-8 text, including
the reason for a refusal. **Show in folder** selects that report in Explorer.
After a successful import, `import-report.txt` is available beside `model.frog`.

To use a VI as a subprogram, drop one `.vi` file from Explorer onto the
parent Block Diagram. Studio converts the file on release and places the
call at the pointer. Double-click the call to open the converted child.
Inputs and outputs come from the VI connector and retain their original
slot identities, directions and data types. Studio uses its connector layout
for the call and displays the stored VI icon when it can decode it.
A verified empty connector gives a call with zero terminals.

For a saved parent, the generated files are in a unique folder under
`Imports` beside the parent. Keep this folder with the caller when moving
the project. For an Untitled parent, Studio uses the normal Documents
import folder. An editing-only conversion is identified at placement.
If the connector cannot be verified, the diagram stays unchanged and the
status message points to the import report.

Studio applies **Clean Up Diagram** during both menu imports and VI drops.
It saves the layout in each newly generated FROG, including dependencies
and nested diagrams, before opening the document or placing the call.
The original VI and the placement of existing objects in the caller are
preserved. The import report identifies any drawing that needs manual review.

The scalar profile covers a standalone diagram with decoded `BDHb`
and `FPHb`, scalar panel terminals, literal connections and the verified DBL
Multiply primitive. I32 and String constants and the DBL multiply path have
been exercised on LabVIEW 2020 files. The converter reads the graph and types
from the source heaps and checks the file digest again at import. Studio
measures its own objects and routes the new diagram with its native layout.

A VI containing only unwired front-panel terminals can also be imported for
execution. The importer verifies each terminal against its saved panel widget
and reads its initializer, including embedded cluster typedef values. Running
this diagram returns immediately and leaves the indicator at its current value.

The structured profile also reads the timed Build Array pattern from the
source heaps. It projects three Flat Sequence frames, a counted For Loop,
its iteration terminal, the I32 array shift register and Build Array, and the
two millisecond Tick Counts feeding a U32 subtraction. The editable graph
retains the uninitialized register between runs of the loaded program. A
fresh load starts with an empty I32 array. The report maps terminals and
wires to their source scopes and identifies any unsupported stored values.

The structured profile also supports the LabVIEW 2020 loop that divides an
I32 iteration index by an I32 divisor, computes a double-precision cosine
in radians and appends the result to a rank-one DBL array. An initialized
shift register starts from its wired array on each Run. A supported Waveform
Graph indicator wired inside the loop receives the updated array there.

For LabVIEW 2020 files with a primary and an auxiliary type map, inspection
reads both TM80 sections and uses the primary map to decode DFDS defaults.
The report lists declared VI and type-definition links from LIvi and diagram
references from LIbd. These describe the source file. Each called VI and its
effects still require a supported translation for an executable import.

For a VI with decoded source diagrams and partially supported operations,
**Import for editing** opens its editable document. The importer follows
local VI dependency paths under the selected file's directory and emits
relative FROG references when connector identities, types and directions
match. Keep the output directory together when moving the project.
Case regions, signals, terminal identities and cluster fields retain their
source mappings. Saved internal diagrams whose owner needs a LabVIEW host
are available as separate documents.

An operation requiring LabVIEW remains an editable object with its source
terminals and an explanation in Execution diagnostics. Run and Build stay
blocked on those operations. The mapping records member paths, source
descriptors and unresolved bindings. The executable import profiles still
require a fully translated program accepted by the common Frontend.
Stripped diagrams and whole-library conversion remain outside these profiles.
Studio uses its normal widget appearance. The reader
is pylabview `5f20e23de6a386021eb955c825cc43d221f09ff1` with the original MIT
notice. The installed Studio uses an isolated bundled Python. For the public
CLI use the pinned [Pillow dependency](requirements-labview.txt).

```powershell
python -m pip install -r requirements-labview.txt
python Import-LabView.py --source sample.vi --analyze --frontend frog-frontend.exe --report labview-report.json
python Import-LabView.py --source sample.vi --frontend frog-frontend.exe --output new-labview-folder
python Import-LabView.py --source project.vi --editable-only --frontend frog-frontend.exe --output editable-project
```

## Import a CMake C project

The C Project importer reads an existing Ninja or Ninja Multi-Config build.
It requires both **compile_commands.json** and the **CMake File API
codemodel-v2** reply from that build directory. The codemodel determines
the selected target's sources and CMake dependencies; the compilation
database determines how each source is parsed. The importer does not
configure CMake, run a build command, or execute the input project.

For the redistributable fixture, run the following preparation explicitly.
On Windows use an x64 developer prompt with Ninja and the same x64 Clang
toolchain as the build. On a platform using the GNU Clang driver, select
the corresponding Clang binary.

```powershell
$build = Join-Path (Get-Location) c-project-build
New-Item -ItemType Directory -Force (Join-Path $build '.cmake/api/v1/query') | Out-Null
New-Item -ItemType File -Force (Join-Path $build '.cmake/api/v1/query/codemodel-v2') | Out-Null
cmake -S fixtures/c-project -B $build -G 'Ninja Multi-Config' '-DCMAKE_C_COMPILER=clang-cl' '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON'
cmake --build $build --config Debug --target reference
cmake --build $build --config Release --target reference
& (Join-Path $build 'Debug/reference.exe')
& (Join-Path $build 'Release/reference.exe')
```

In Studio, select **File → Import → C Project…**, choose the build's
**compile_commands.json**, and enter target **calculation**, configuration
**Debug** or **Release**, and entry function **compute**. For a single-config
build, enter the configuration name in the codemodel, or leave it empty
when the configuration has no name.

The equivalent standalone command is:

```powershell
python Import-CProject.py --source c-project-build/compile_commands.json --target calculation --configuration Debug --entry compute --clang 'C:/Program Files/LLVM/bin/clang-cl.exe' --frontend path/to/frog-frontend.exe --output imported-debug
```

Open **main.frog**. Double-click an imported call to inspect its child.
The four child documents must be kept with the caller. Use the normal
Studio Save command for edits; if you relocate the project as a whole,
keep its relative layout. The **import-map.json** records the target,
configuration, database digest, and C function locations. The initial
**main.fir.json** and source maps are import evidence; export FIR again
after editing if you need the current revision.

The fixture inputs 0, 5, 29 and 4294967295 produce 24, 39, 111 and 21 in
Debug, and 36, 51, 123 and 33 in Release. The generated header is created
by the explicit CMake preparation. The two local functions named
**adjust** retain distinct identities.

The initial profile covers x86-64 Clang/clang-cl C targets (executables,
static and object libraries), up to 64 translation units and 48 reachable
functions. Translated functions use one or two U32 inputs and a single U32
return expression made of direct calls and unsigned addition, subtraction,
multiplication, or supported constants/conversions. Runtime linking,
function pointers, recursive calls, general statements and other scalar
types are outside this profile. A DLL import is refused here; the separate
scalar NativeCall importer remains available with its declared DLL input.
Import refuses unsupported compile settings, unresolved reachable symbols
and missing headers without producing an executable partial document.
For missing generated headers, run the project's own preparation
explicitly, then retry.

## Fixtures

Run `python fixtures/and_or.py new-fixture-folder` to generate the supported
And → Or and And → Xor models. Xor is now admitted; `outside-profile.onnx`
uses a valid ONNX Equal operation as the unsupported-operation neighbor. `fixtures/expression.c` and its
header exercise the bounded expression profile. `fixtures/panel.c` exercises
the layout profile with 27 editable widgets and an independent test layout.
Use it for syntax inspection only. To build the scalar DLL
fixture, run these commands from a scratch directory using the two source
files under `fixtures/`.

```powershell
clang-cl /nologo /TC /std:c17 /W4 /WX /O2 /c /GS- /Zl /clang:--target=x86_64-pc-windows-msvc sample.c /Fosample.obj
lld-link /dll /noentry /nodefaultlib /machine:x64 /Brepro /out:sample.dll sample.obj
```

The scalar fixture returns `a + 2*b` modulo 2³². The caller is
`fixtures/forward.c`.

## Analyze first, or import an ordinary C function

Studio's **File → Import → ONNX… / C…** uses the same analysis and conversion
code. The preparation form lists actual metadata, compatibility, diagnostics
and compilation context before publication. Change the file, copy technical
details, adjust include folders/macros, or cancel without replacing open files.
The analyzed source digest is checked again before a result is opened.
Studio supplies Python/ONNX/Clang/Frontend using its installed runtime; no PATH
setup or manual conversion command is needed. Source C is parsed, not run.

```powershell
python Import-OnnxGraph.py model.onnx --analyze --report model-analysis.json
python Import-CProject.py --single-file --source example.c --clang path/to/clang-cl.exe --analyze --report c-analysis.json
python Import-CProject.py --single-file --source example.c --entry calculate --clang path/to/clang-cl.exe --frontend path/to/frog-frontend.exe --output imported-function
```

Repeat `--include folder` and `--define NAME=value` for the single-file context.
Select the actual function discovered by Clang. The pure profile covers total,
side-effect-free one-return expressions with one or two homogeneous U32 or BOOL
arguments and the same return type. U32 supports +, −, ×, &, |, ^ modulo 2^32.
BOOL supports !, &&, ||, &, |, ^ and 0/1; eager dataflow is only used after the
entire reachable closure is proved pure and total in this bounded profile.
Effects/short-circuit side effects, floating point, mixed conversions, pointers,
recursion, unsupported statements and unresolved functions are refused. Direct
calls become editable sub-FROGs with explicit ports, identities and source maps.
This reuses the C Project translator; it does not change CMake target discovery.
The specialized QMH, scalar DLL and Win32 layout-only profiles remain separate.
Their names are suggestions until their own AST/ABI guards accept the input.

Run `python fixtures/boolean_compatibility.py new-fixtures` for redistributable
18-operation XOR and 19-operation And/Or/Not equivalent two-bit adders, vector
cases, a valid outside-profile model and a pure C source. These are identified
**equivalents**, not copies of unavailable user models. Sixteen four-bit inputs
are checked against independent integer addition and ONNX ReferenceEvaluator.
`fixtures/test_compatibility.py --frontend path/to/frog-frontend.exe --clang
path/to/clang-cl.exe` exercises formats, schema versions, refusals, original
identity, actual Frontend admission and a deliberately compiled Windows x64 C
oracle. UI/Run/EXE qualification is separate; a passed JSON/checker test alone
does not establish it.
