# Write, generate or import FROG

Start with a canonical `.frog` Source document. Choose a target and an exact
Frontend version before you generate a larger graph. For documents that Studio
saves as `frog.document.draft`, use the separate [Studio draft profile](../Expression/Studio%20authoring%20document%20profile%20v1.md)
and its documented migration and repair paths.

## 1. Start from a complete program

[Example 01](../Examples/01_pure_addition/main.frog) is a complete headless
Source program. Its inputs `a = 2.25` and `b = 3.75` produce `result = 6.0`
in the public reference Runtime. The [source guide](../Expression/Frog%20source%20guide.md)
walks through every section and edge. For a first run through the common
Frontend CLI, use [the complete scalar graph with a Front Panel](../Implementations/Frontend/tests/fixtures/pure-add.frog).
Its controls default to 1 and 2 and its indicator receives 3 in the qualified
reference paths.

Give graph objects stable IDs and connect endpoints by node and port ID.
For an importer, retain the external-to-FROG identity mapping and the complete
types across runs. Report any operation, type or scope that you cannot map.
Widget labels, diagram positions and visual slot order do not define ports.
Keep the original input and write a separate candidate for validation.

| Need | Authority |
| --- | --- |
| Source sections, identities, edges and optional data | [Source guide](../Expression/Frog%20source%20guide.md), [Source schema](../Expression/schema/frog.schema.json), [source profiles](../Expression/Source%20compatibility%20and%20profiles.md) |
| Operation IDs and ports | [Library index](../Libraries/Readme.md), [Core contracts](../Libraries/Core.md), and versioned manifests such as [Add](../Libraries/Manifests/frog.core.add.v1.json) and [Unbundle by Name](../Libraries/Manifests/frog.core.unbundle.by_name.v1.json). Add uses `a`, `b` and `result`. Read each applicable manifest or operation contract for direction, cardinality and typing. The [dated Studio catalogue snapshot](../Profiles/StudioAuthoring.md) helps resolve Studio-specific IDs within its stated version. |
| Types, values and scopes | [Type](../Expression/Type.md), [typed binding](../Expression/Typed%20Binding%20Contract%20v1.md), [numeric representations](../Expression/Numeric%20representations.md) and [structure contracts](../Expression/Control%20structures.md). Retain the Array rank, ordered Cluster fields and Enum domain and items. |
| Front Panel, bindings and events | [Front Panel](../Expression/Front%20panel.md), [widget class contracts](../Expression/Widget%20class%20contract.md), [captured Event profile](../Expression/Event%20Captured%20Profile%20v1.md) and [UI library](../Libraries/UI.md). |
| Dependencies and refusal diagnostics | [Resolver contract](../Implementations/Frontend/frontend-resolution.md), [Frontend outputs](../sdk/ecosystem/docs/frontend-result-contracts.md), [execution eligibility](../Language/Execution%20eligibility.md). |
| Studio versions and tested profiles | [Compatibility matrix](studio-source-compatibility.md), [12 November checkpoint](frontend-checkpoint-2026-11-12.json) and the [qualified example paths](#3-qualified-example-paths) below. |

## 2. Validate, derive and select a target

Run these PowerShell commands from the repository root with `frog-frontend.exe`
on PATH, or set `$cli` directly to its absolute path. The
[Frontend README](../Implementations/Frontend/README.md) gives the standalone
build commands. Use the exact kit and inspect `describe`. These commands use
the complete scalar with Front Panel and write their outputs under the
temporary directory.

```powershell
$cli = (Get-Command frog-frontend.exe -ErrorAction Stop).Source
$out = Join-Path ([IO.Path]::GetTempPath()) ('frog-generator-' + [guid]::NewGuid())
New-Item -ItemType Directory -Path $out | Out-Null
$src = @(
  '--source', 'Implementations/Frontend/tests/fixtures/pure-add.frog',
  '--source-uri', 'fixture:/generator/ui.frog',
  '--document-id', 'source:ui',
  '--fir-document-id', 'fir:ui',
  '--source-format', 'frog.source',
  '--source-version', '0.1',
  '--fir-version', '0.1'
)
& $cli describe --descriptor-output (Join-Path $out 'descriptor.json')
if ($LASTEXITCODE -ne 0) { throw 'Frontend describe failed' }
& $cli validate @src --diagnostics-output (Join-Path $out 'validate.json')
if ($LASTEXITCODE -ne 0) { throw 'Source validation failed' }
& $cli derive @src --output (Join-Path $out 'program.fir.json') `
  --diagnostics-output (Join-Path $out 'derive.json') `
  --source-maps-output (Join-Path $out 'maps.json') `
  --provenance-output (Join-Path $out 'provenance.json')
if ($LASTEXITCODE -ne 0) { throw 'FIR derivation failed' }
```

1. **JSON and schema.** Check UTF-8 JSON and the envelope against its schema.
   For a set of existing conformance cases, use the repository's
   [reference validator](../Implementations/Reference/Validator/Readme.md).
2. **Semantic validation and FIR.** Run the common Frontend with the declared
   Source format, version and semantic profile. If the graph requires dependencies,
   pass the explicit `--semantic-context` and `--dependencies` inputs documented
   by the [CLI](../Implementations/Frontend/README.md). Retain diagnostic codes,
   source URIs and pointers as supplied. A nonzero exit blocks the next step.
3. **Target admission.** Submit FIR and required provenance to the selected
   backend. Its capability list guides selection. Its whole-graph validation
   decides whether it can build or run the program.
4. **Execution.** Compare observed outputs and faults with explicit expectations
   on that target. Derivation alone supplies no observed value.

## 3. Qualified example paths

The examples below use Source and FIR 0.1 where applicable. The CLI replay used
**Frontend 0.2.0-local, API 3**, content
`c815f0a1e9d5f466598f5e780cec556d3b046d71b326b98502bee49f7f0df9a9`,
the checkpoint associated with **Studio 0.0.8.094**. Execution receipts below
name the actual execution owner. For the public Python reference Runtime
examples, run from the repository root:

```text
python -B -X utf8 Implementations/Reference/Runtime/check_examples01_10_runtime_acceptance.py
```

| Source | Tested gates and expected result |
| --- | --- |
| [Scalar, Example 01](../Examples/01_pure_addition/main.frog) | Schema and Python reference Runtime acceptance pass. `result = 6.0` for 2.25 + 3.75. The checkpoint CLI refuses this public-interface corridor with `IR_PUBLIC_INTERFACE_UNSUPPORTED`. |
| [Scalar with IHM, Example 02](../Implementations/Frontend/tests/fixtures/pure-add.frog) | Schema, CLI validation and FIR pass. [Python reference oracle](../Implementations/Reference/Runtime/acceptance/example02_ui_value_roundtrip.snapshot.json) gives `ind_result = 3.0`. The [C counterpart](../Implementations/Bridges/reference-c/examples/core-add.frog) also produced 3 on its qualified native target. |
| Case branch | The [canonical structure fixture](../Conformance/valid/11_public_structure_families/case.frog) passes the public schema checks. The combined fixture receives `EVENT_IR_OBJECT` in the checkpoint CLI. The separate [Studio Draft 2 example](../Examples/Native/Structures/structure.case.bool.true/main.frog) selects the True branch and produces I64 `41` in a native-fast run, verified against the [example index](../Examples/index.v1.json). |
| [For with three iterations](../Implementations/Bridges/reference-c/examples/for-3.frog) | Schema and checkpoint CLI/FIR pass. The [native oracle](../Implementations/Bridges/reference-c/examples/expected/for-three.json) records an I64 seed of `9007199254740993` and result `9007199254740996`. |
| [Array build, index and size](../Implementations/Bridges/reference-c/examples/array-32.frog) | Schema and checkpoint CLI/FIR pass. For index 31 the [native oracle](../Implementations/Bridges/reference-c/examples/expected/array-last.json) records shape `[32]`, length 32 and element 31.25. |
| [Cluster/Record exact values](../Implementations/Frontend/tests/fixtures/record-exact.frog) and [nested Array/Cluster/Enum](../Implementations/Frontend/tests/fixtures/nominal-nested.frog) | Schema and checkpoint CLI/FIR pass. [Frontend contract checks](../Implementations/Frontend/tests/frontend_integration.cpp) verify exact I64/U64, negative zero and NUL in the Record and preservation of the complete nested nominal schema and data. Native-fast replays of separate Studio draft examples check [Array to Cluster](../Examples/Native/array/array.to-cluster.padding/main.frog) with fields 7, 8, 0 and [ordered Unbundle](../Examples/Native/cluster/cluster.unbundle.order/main.frog) with `9007199254740993` and `"kept"`. |
| [Enum with Front Panel, Example 08](../Examples/08_enum_value_roundtrip/main.frog) | Schema and Python reference Runtime acceptance pass. `fault` reaches both `result_mode` and `mode_result`. The checkpoint CLI refuses this public-interface corridor. The separate [Studio Enum Case](../Examples/Native/Structures/structure.case.enum.exact/main.frog) produces I64 `41` in the native-fast replay. |
| [Captured Event with Front Panel](../Implementations/Frontend/tests/fixtures/event-product.frog) | Schema and checkpoint CLI/FIR pass. The Studio native headless owner ran the same Source bytes with a captured Value Change from 2.5 to 12 and a bias of 3. The `sum` indicator receives 15. The public-only replay covers Source and FIR; native dispatch uses Studio’s LLVM owner. |

The Studio draft replays used native-fast with example runner SHA-256
`4220cd38dafdc92865628026611ea18e41786be43775d178e81a3c3a0a0dfd05` and the
public [example index](../Examples/index.v1.json). The Event replay used the
Studio native handoff test owner SHA-256
`3a92e1e92d7d0ec37af7a10f9a3aed1c6326f7b3d500c0a8fd6459aecaf4bd06`.
Those component runs use their own binaries. They cover neither interactive
desktop use nor an exported Event EXE.

The [reference C corpus](../Implementations/Bridges/reference-c/examples/corpus.json)
contains inputs, expected results, negatives, source hashes and frozen FIR
hashes. The standalone finite target `graiphic.reference-c/windows-x64/native-fast`
was qualified with API 3 during the **Studio 0.0.6.766** integration. The public
CLI executable used in that qualification has SHA-256
`2af8361e0c90b85d8d8dbd70187759988ffe28505bc7845e2666ed85a685ad8f`
and the peer `0e499db34b23d41d06621dd946f816a239c440c7fe183ea3c2c4d37578d06cff`.
The checkpoint CLI replay above does not requalify C execution. For an exact-kit
rerun, use the [bridge setup](../Implementations/Bridges/reference-c/README.md)
and its [public corpus runner](../Implementations/Bridges/reference-c/tests/run_public_corpus.py).
From the repository root, set `$cli` and `$peer` to the exact installed
executables. `compiler.json` is the inventory next to the peer as described in
the bridge setup. The output directory must be new.

```powershell
$proof = Join-Path ([IO.Path]::GetTempPath()) ('frog-reference-c-' + [guid]::NewGuid())
python -B -X utf8 Implementations/Bridges/reference-c/tests/run_public_corpus.py `
  --frontend $cli --peer $peer --compiler-config compiler.json `
  --sdk-root sdk/ecosystem --output-root $proof
```

The runner also compares fresh FIR hashes to the frozen corpus. A mismatch
requires a newly attributed qualification before reporting equivalent execution.

## 4. Deliberate refusals

| Gate | Witness and expected result |
| --- | --- |
| Envelope | [Missing required section](../Conformance/invalid/01_missing_required_section/case.frog) fails the Source schema and the CLI reports `frontend.source_envelope_invalid`. |
| Semantics | Copy the CLI scalar fixture and change the first edge's destination port from `a` to `generated_wrong_port`. The JSON shape passes and CLI validation rejects the endpoint with `IR_ENDPOINT`. |
| Target admission | [Nominal Enum](../Implementations/Bridges/reference-c/examples/unsupported-enum-u32.frog) and [subtraction](../Implementations/Bridges/reference-c/examples/unsupported-subtract.frog) derive FIR. The qualified finite C target refuses them with `reference_c.nominal_type_unsupported` and `reference_c.operation_unsupported`. |
| Runtime fault | [Array index below zero](../Implementations/Bridges/reference-c/examples/inputs/array-negative-index.json) is an input to an admitted graph. The native oracle expects `ARRAY_INDEX_RANGE` and zero published outputs. |

## 5. Optional isolated LLM trial

Use a fresh session with an already authorized model. Supply only the public
revision, [the compact index](../llms.txt) and the documents it links. Ask for a
complete scalar with a Front Panel, using two non-default input values and a
stated expected indicator result. Ask for the authorities behind every
operation and port. Then request a second candidate with an unsupported port
and an explanation of which gate should reject it. Save the exact prompt,
model/version and unedited responses. Check both files against the schema and
the Frontend; check the valid result on an admitted target. Record elapsed time,
manual corrections, diagnostic codes and observed outputs separately. No LLM
trial is part of the qualification recorded above.
