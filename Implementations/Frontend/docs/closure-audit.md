<a id="audit-de-fermeture-du-frontend-p28"></a>

# P28 Frontend closure audit

`tools/audit_frontend_closure.py` checks the actual production translation units
listed in `production-sources.cmake`. It does not compile, configure CMake, copy
source files or create packages. Python 3.10 or later suffices, with no external
Python dependency.

From the workspace root or a complete three-repository snapshot:

```text
python FROG/Implementations/Frontend/tools/audit_frontend_closure.py --root FROG/Implementations/Frontend --output frontend-closure-audit.json
```

`--root` also accepts the FROG root or the common root of all three repositories.
To additionally check the actual C/CLI adapters, add:

```text
--source src/FrontendApi.cpp --source src/FrontendOperation.cpp --source src/FrontendCli.cpp
```

A missing file is a failure, not a silent exclusion. The default list remains
exactly that of the production generator; the report retains every inspected root.

<a id="contrat-du-contrôle"></a>

## Check contract

- `set(FROG_FRONTEND_PRODUCTION_SOURCES ...)` must appear exactly once and contain
  literal paths. CMake substitutions are rejected. `--sources` also accepts a text
  or JSON list (`{"sources":[...]}`).
- Includes resolve only within the Frontend owner: `include`,
  `include/frog_native_engine`, `src`, `snapshot/api`, `snapshot/sdk/include` and
  `snapshot/generated`, plus any `--include-dir` confined to that same owner.
  A link to a file outside the owner fails.
- Every conditional branch is traversed. Dynamic, unresolved, private Runtime/Studio,
  Canvas, controller, Win32 and LLVM/Clang includes are rejected. C/C++20 standard
  headers form an explicit list; unknown platform headers remain visible.
- Private model/session identifiers are searched outside C++ comments and literals.
  Extraction now admits **explicitly listed** neutral data owners: `WidgetInstance`,
  `DiagramNode`, `FrogDocumentStateData` and their codecs resolve in the public Frontend.
  Their historical names do not make them a Canvas. This is no general permission
  for files prefixed `Widget` or `FrogDocument`: contents/dependencies remain checked.
  `FrontPanelCanvasScene`, `LoadedFrogDocument`, controllers, private repair/recovery
  and runtime sessions remain excluded.
- The Zstandard 1.5.7 document decoder is a bounded, explicit static dependency.
  Its C sources/notices are retained. Only branches disabled by effective definitions
  (`ZSTD_LEGACY_SUPPORT=0`) and compiler-intrinsic headers declared per file receive
  documented exceptions. This imports neither the compressor nor private runtime.
- Public SDK value/schema types `frog/sdk/runtime_*` and historical `frog::runtime`
  namespaces of semantic contracts remain allowed. This permits no private
  `frog/runtime/*` include.
- Every visited file and manifest has a SHA-256; mutation during the audit fails.
  The report contains include edges, relative paths, sizes and localized reasons.
  It publishes no machine-absolute origin.

Exit codes: `0` conforming closure, `1` violation, `2` configuration/read failure.
Diagnostic codes are stable; the message is localized evidence, not a language
semantic-classification rule.

This conservative lexical check replaces neither the compiler, linked-binary symbol
inspection nor FIR/Event and C ABI/CLI/Studio tests. Success does not prove SDK-copy
parity with its owner; parity uses `source-origin.json` snapshot captures and existing
CMake configuration. Its `move` hashes remain extraction captures without preventing
later changes in the new owner. The license inventory therefore distinguishes
`original_hash` and `current_hash`.

The complete E2 record of 1 November 2026 traversed 45 production sources and the three
adapters above: 48 root units, 179 files and 313 edges, with no private Runtime/Studio
include, concrete editor model, Win32, LLVM or missing include. Evidence is retained at
`FROG-STUDIO/native-portable-engine/artifacts/modularisation-etape2-p28-2026-11-01/independent-frontend-full-closure-audit.json`.
This concerns lexical closure of that source state, not a successful binary or link.

Phase A on 2 November then rebuilt and installed standalone kit content
`c640d266a4de9d9581e6394d80d7152ce3d91d64278b960ab6777a524a056a62`, with no
Studio/Runtime/LLVM sources, headers or libraries among its inputs. Its six targeted
CTest cases, 6,614 semantic checks and installed-prefix C consumer passed. Integrator
receipt `artifacts/p28-implementation-2026-11-02/phase-a-result.json` also identifies
kit manifest `fed3baac219da2bc68c99b9d08e6560b73ca68c100e17cbfdfeb33d1e09f0102`
and binaries. The integrator retains it; its path is not a kit build dependency.
It qualifies neither clean Windows, later phase-B changes nor backend execution.

<a id="même-source-snapshot-que-le-produit"></a>

## Same source snapshot as the product

Complete capture reuses
`FROG-STUDIO/native-portable-engine/tools/Export-StudioSourceSnapshot.ps1`:
`source-snapshot.json` manifest, three identified repositories, local revisions/overlays,
portable paths and verified hashes. Run its `-VerifyDirectory` mode before auditing
the Frontend in that capture. This tool adds closure evidence; it replaces neither
that export nor introduces another package format. Do not export during active editing.

A successful audit changes no distribution rights. Read `provenance-and-licensing.md`
and `license-inventory.json` before distribution.
