# FROG importers

These Apache-2.0 tools produce editable FROG documents. The common Frontend
owns validation and FIR derivation. Studio uses these same scripts.
The examples below use PowerShell and absolute tool paths where appropriate.
Install the [pinned ONNX dependencies](requirements-onnx.txt) into the Python
environment used to run the ONNX importer.

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

## Import LabVIEW RSRC files

In Studio use **File → Import → LabVIEW…**. Choose a `.vi`, `.ctl` or `.llb`
to inspect its resources. **Import** is available when a VI has a supported
source diagram and the common Frontend admits its editable FROG projection.
The result contains `model.frog`, a resource report and a mapping of original
object, terminal and wire UIDs. The selected file is read-only throughout.

The scalar profile covers a standalone diagram with decoded `BDHb`
and `FPHb`, scalar panel terminals, literal connections and the verified DBL
Multiply primitive. I32 and String constants and the DBL multiply path have
been exercised on LabVIEW 2020 files. The converter reads the graph and types
from the source heaps and checks the file digest again at import. Studio
measures its own objects and routes the new diagram with its native layout.

The structured profile also reads the timed Build Array pattern from the
source heaps. It projects three Flat Sequence frames, a counted For Loop,
its iteration terminal, the I32 array shift register and Build Array, and the
two millisecond Tick Counts feeding a U32 subtraction. The editable graph
retains the uninitialized register between runs of the loaded program. A
fresh load starts with an empty I32 array. The report maps terminals and
wires to their source scopes and identifies any unsupported stored values.

Conversion requires a fully recognized graph within one of these profiles.
Implicit coercions, other unrecognized diagrams, subVI calls, stripped
diagrams and library conversion remain outside their scope. Studio uses
its normal widget appearance. Connector-pane
layout and LabVIEW window customization stay in the source file. The reader
is pylabview `5f20e23de6a386021eb955c825cc43d221f09ff1` with the original MIT
notice. The installed Studio uses an isolated bundled Python. For the public
CLI use the pinned [Pillow dependency](requirements-labview.txt).

```powershell
python -m pip install -r requirements-labview.txt
python Import-LabView.py --source sample.vi --analyze --frontend frog-frontend.exe --report labview-report.json
python Import-LabView.py --source sample.vi --frontend frog-frontend.exe --output new-labview-folder
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
