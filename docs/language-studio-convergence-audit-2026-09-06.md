<a id="audit-du-langage-frog-et-convergence-avec-graiphic-studio"></a>

# FROG language audit and Graiphic Studio convergence

Date: 6 September 2026. Local audit/corrections; no remote publication or general
conversion of user files performed.

## Verdict

Language principles are sound: explicit source, UI-independent interfaces,
source/meaning/FIR/target separation, stable identities and class contracts.
Studio introduced concrete editing/typing improvements worth preserving. But
**the system is not yet unified end to end**: several formats and two validation/
derivation paths coexist. Hiding them under `.frog` would imply compatibility
not demonstrated by tests.

This pass corrects demonstrated defects and records convergence decisions in
[Source compatibility and profiles](../Expression/Source%20compatibility%20and%20profiles.md).

<a id="périmètre-et-méthode"></a>

## Scope actually examined

- Initial documentation inventory: 560 FROG Markdown files; links checked in
  250 Expression, Language, IR, IDE, Conformance, Versioning and root documents;
  32 product documents and Studio documentation inventory.
- Targeted reading of schemas, types, structures, `.wfrog`, versions, loading,
  serialization, ports, profiles, validation/derivation, compared with tests,
  examples and Git history.
- Analysis of 119 Numeric/Boolean projections against the compiled catalog and
  12 `.wfrog` realizations shared by public and Studio repositories.
- Strict JSON/root-schema checking of 48 published `.frog` sources in 47 example
  directories, without equating that with semantic validation.
- Positive, negative and preservation tests; Python, C++, PowerShell checks,
  canonical Studio compilation and documentation checks.

This is neither a formal proof of the whole language, manual reading of every
paragraph nor certification of all backends. Documentation scope is detailed in
the [documentation appendix](language-documentation-audit-2026-09-06.md).

<a id="les-formats-réellement-présents"></a>

## Formats actually present

| Format | Role and finding | Decision |
| --- | --- | --- |
| Public `.frog`: `spec_version`, `metadata`, `interface`, `diagram` | Canonical source; optional front panel. | Preserve source authority. |
| Studio: `frog.document.draft`, `draft_revision`, `frontPanel` | Private editing format still written as `.frog` by Studio. | Retain legacy compatibility; explicit migration, not key renaming. |
| `.wfrog` | Class/composition/realization artifact family with distinct contracts. | Do not confuse realization geometry with class value/ports. |
| Examples' `frog_fir_unit` | Useful, tested pattern-based reference artifact. | Do not present it as conforming to canonical Execution IR schema. |
| Execution IR `schema_family: frog.execution_ir` | Public contract with unit, objects, connections, regions, source_map and correspondence. | Preserve this interoperability target; reference adapter remains to be implemented. |

`test_canonical_artifact_boundaries.py` makes both format distinctions explicit.
It checks source/IR schemas, public root fixtures and an intentionally empty
structural IR fixture. The latter is neither executable nor semantic IR coverage
evidence. All 48 example sources are systematically checked. Example 36 redeclared
six style properties in the same objects. Hidden occurrences were removed; the
effective document matches historical Git-HEAD decoding that retained the last
value. This removes ambiguity without changing consumed style values.

<a id="exigences-et-vérification-individuelle"></a>

## Requirements and individual verification

“Corrected” below is limited to the cited consumer/tests; other implementations
are not automatically migrated.

| ID | Requirement | Result of this pass |
| --- | --- | --- |
| LF-01 | One explicit authority per layer. | Clarified in convergence contract and product README. |
| LF-02 | Source version distinct from build, corpus, program and draft. | Confirmed: source 0.1, corpus 0.1-draft, draft_revision 2; no invented version. |
| LF-03 | Unknown version must not be treated as 0.1. | Corrected in Python precheck/public Studio import; diagnostic without acceptance. |
| LF-04 | Future-version sections must not be judged by an old envelope. | Future source without metadata/interface/diagram tested; unsupported_source. |
| LF-05 | Ambiguous or over-capacity UTF-8/JSON gets a diagnostic. | Common Python reader hardened: duplicate keys, NaN/Infinity tokens, integer/float limits and depth. |
| LF-06 | Exact integers do not pass through binary64. | Python integers/exact strings preserved; maximum U64 and boundaries tested. |
| LF-07 | Front panel remains optional. | Public rule confirmed; Studio accepts empty headless source, not yet public graphs with independent interfaces. |
| LF-08 | Public interface is not reconstructed exclusively from widgets. | Not delivered: Studio model must be extracted. Import currently rejected to avoid losing declarations. |
| LF-09 | Import must not silently erase executable constructs it cannot represent. | Studio guard added: ignored interfaces/kinds/fields, flattening losses, ambiguous IDs/endpoints. Bounded protection, not a universal lossless importer. |
| LF-10 | Historical draft/public source are not competing authorities in one document. | Mixed format/spec_version envelope rejected; drafts and historical UI import tested. |
| LF-11 | Stable, unambiguous IDs. | Python validates ports across the whole interface and top-level nodes/edges; malformed IDs rejected. |
| LF-12 | Saveable authored state distinct from valid program. | Preserved: incomplete wires/Case drafts block validation/derivation; no implicit repair normalization. |
| LF-13 | Manifest ports consistent with implementation. | 26 discrepancies corrected in 13 Numeric entries; 119 projections strictly checked on exported fields. |
| LF-14 | Palette entry implies neither signature nor execution. | Increment/decrement remain explicitly blocked/catalog-only; false reduction profiles moved to correct operators. |
| LF-15 | Every signature has a declarative authority. | Partial: Add/Case have authorities/projections; functions/widgets/other structures still need generalization. |
| LF-16 | Aggregate/enum/unknown types have defined identity. | Typed-binding contract retained: rank, ordered fields, enum domain, non-universal unknown; Runtime support separate. |
| LF-17 | Conditional Disable has one Default rule. | Residual “exactly one” changed to “at most one”; selection/default/no-match stay distinct. |
| LF-18 | Explicit loop-counter widths. | Studio requirements aligned to public i64; no implicit LabVIEW I32 rule. |
| LF-19 | Host/execution policies are not guessed. | Unsupported versions/unknown execution_policy fields blocked; known host values checked. Host extensions remain opaque, without certified validity/semantics. |
| LF-20 | Descriptive metadata correctly typed; unknown fields preserved. | Python checks standard fields; opaque descriptive extension retained. Studio does not claim general extension preservation. |
| LF-21 | Derivation must not certify a subgraph while ignoring the rest. | Envelope guard/exact recursive inventories added; accumulator's four internal connections checked. 156 Deriver tests pass. The two semantic validators are not merged. |
| LF-22 | Reference FIR and canonical IR clearly distinguished. | Documentation/boundary test added; canonical adapter not delivered. |
| LF-23 | Schema tests actually run in CI. | Shared dependencies declared, jsonschema import mandatory, conditional skips removed; WidgetValidator tests added to --include-pytest runner. |
| LF-24 | Examples, status and navigation agree. | 47 directories distinguished from 15 public-runtime-corridor examples; historical partial tables explained; official navigation generated/verified, 728 search paths and 19 root entries. |
| LF-25 | Public/Studio classes and realizations agree. | Cluster/Picture/Waveform differences identified; Python Button consumers corrected. Boolean Python defaults resolved in the 6 September continuation, original test and eleven extra regressions pass; no global compatibility claimed. |

<a id="corrections-et-choix-concrets"></a>

## Concrete corrections and choices

<a id="import-studio"></a>

### Studio import

Before correction, reference-validator-accepted `Examples/01_pure_addition/main.frog`
was rejected by Studio for missing frontPanel. Adding it did not solve the problem:
interface_input/output nodes were outside the retained filter; public ports are
not represented independently of widgets. More permissive opening could destroy
the program when saving.

The chosen correction explicitly rejects import/conversion when the model cannot
preserve executable content, checking before/after flattening. The writer remains
draft revision 2. The private guard is isolated in `src/FrogDocumentPublicImport.hpp`;
it creates no second normative language definition. Existing
`frog-document-revision-smoke` coverage is extended.

<a id="fonctions-déclaratives"></a>

### Declarative functions

Nine affected unary projections declared `value` privately versus public/C++ `in`;
the projection now follows `in`. Incorrect increment/decrement reduction profiles
are removed; existing profiles go to Add/Multiply Array Elements. No persisted C++
port ID changes.

Checks compare port order/direction/count, TypeExpr, requirement, connection policy,
declared fallback, resolution, implementation and variadic bounds. Seventeen mutations
must be rejected. Historical CSV does not fully prove cardinality, defaults or arbitrary
TypeExpr containing separators: structured export/shared generation remain preferable
long term. 119 projections do not cover all 239 operations.

<a id="validation-et-dérivation"></a>

### Validation and derivation

Educational `validate_source` and `fir_deriver` support different subsets. Simply
placing the former before the latter would reject already supported examples.
The initial probe showed version 99.0 or an ignored extra node could yield the same
FIR as valid addition. Bounded corrections close those paths without turning pattern
rules into a general compiler. Each rule has a static inventory of allowed nodes/types/
edges/regions checked before execution. The accumulator also checks its four internal
connections: rewiring at unchanged node/edge count must not produce the old calculation.
Edge order/IDs do not change this contract.

This proves no semantic equivalence for every wiring in every rule. Independence
from metadata name is demonstrated for addition; inherited `metadata.name` guards
in rules 06–16 remain, now documented/tested. These remain bounded reference profiles,
not a general language implementation.

The envelope precheck consumes required/properties/versions from the public schema,
with parity tests. Local fields retain specialized validation. The common reader
distinguishes invalid JSON, capacity overflow and unsupported future source. JSON-readable
depth excessive for the validator also yields a diagnostic without modifying loaded
source. No failure produces a partial validated program.

<a id="widgets-et-réalisations"></a>

### Widgets and realizations

Eight of 12 common realizations are byte-identical. Enum/Ring differ in Item 0/Item 1
labels. Other differences are deeper:

- Public Cluster: common class/composition; Studio: reduced control/indicator projection.
  An adapter and composition/round-trip tests are required.
- Picture: public image/control+indicator/viewport versus Studio U32 matrix indicator/1:1
  display. Select an explicit profile before migrating values.
- Waveform: local Studio class/realization with a different envelope from the public
  class package. The current public document already states limits.

Do not merge these contracts by raw file synchronization.

The expanded Python suite also exposed an actual Button-contract-consumption bug:
public `style.button_face`/`button_face` markers were not applied everywhere.
Property forwarding, geometry, overlays and selectors are corrected, with a bounded
old-`face`-skin adapter. The associated Boolean indicator SVG now actually reaches
the renderer. Git history distinguishes obsolete tests from real defects, detailed
in the documentation appendix.

One failure remained initially: `boolean.default.wfrog` typography defaults were
not inherited when omitted by an instance. The renderer used 18/18 px rather than
public 12/13 px. The continuation below resolves manifests before package/instance
overrides. HTML-string tests are not browser CSS/JavaScript visual verification.

<a id="vérifications-de-la-passe-initiale"></a>

## Initial-pass verification

Studio figures below remain historical. Later native delivery/checks appear in the
[modularization and optimization report](https://github.com/Graiphic/FROG-STUDIO/blob/main/docs/source-optimization-follow-up-2026-09-06.md).

- Canonical Studio **0.0.1.245**, `all` compilation, valid link/signature; Desktop
  shortcut targets `build-nmake-verify/frog-engine-shell.exe`. No second Studio executable.
- Targeted static analysis of `FrogDocument.cpp`/import guard: clang-tidy passes
  after removing two unnecessary temporary concatenations. No claimed rerun of
  the previous global static analysis.
- CTest excluding UI/analysis/coverage: **115/117 passed** on final 0.0.1.245 in
  97.66 s. Pre-existing failures: `frog-studio-architecture-budget-contract`
  (20 over-budget files) and `frog-studio-front-panel-ownership-contract`. Thresholds
  were not raised. Catalog, hashes and architecture index regenerated/verified:
  427 files, 5,886 compiler functions, 239,157 lines, no stale index. Direct document
  revision/limit tests also pass.
- Complete `pytest Implementations/Reference`: **502 passed, 1 failed, no skips**,
  25.66 s. Remaining failure: `Runtime/python/tests/test_runtime_ui_slice10.py:91`,
  Boolean inheritance above. All 48 example sources pass strict JSON/root schema;
  156 Deriver tests pass, including 01–16 API/CLI references.
- Standard `check_reference_workspace.py`: **PASS**, Examples 01–15, 6.54 s.
  Validation of 20 `.wfrog` families: 0 errors, 1 pre-existing missing
  `Picture.placement_bounds` class-document warning. Pipeline success cancels neither
  the pytest UI failure nor format differences.
- Python/LLVM native bridges 05–15: **11 passed**, temporary libraries compiled with
  local WinLibs. Initially skipped because the Python process PATH lacked a compiler;
  rerun with explicitly configured toolchain.
- Official navigation generated/verified; **47 local links in seven documents,
  no missing target**. The 47 example directories are not 47 implemented Runtime corridors.

Limits remain explicit: no certification of every backend, complete canonical IR,
UI performance or universal import/export.

Reproducible commands from FROG root, Python 3.11 and native toolchain visible in
the process PATH for dynamic bridges:

```text
python -m pip install -r Implementations/Reference/requirements-test.txt
python -m pytest Implementations/Reference -q
python Implementations/Reference/check_reference_workspace.py --include-pytest
pwsh -File .github/scripts/verify-pages-navigation.ps1
```

From `FROG-STUDIO/native-portable-engine`, after canonical-wrapper compilation and
official catalog generation:

```text
ctest --test-dir build-nmake-verify -LE "^(ui|analysis|coverage)$" --output-on-failure -j 1
```

These commands do not request Git publication. New files/modifications must be
versioned with local dependencies before publication; local execution success
does not prove green remote CI.

<a id="suite-ordonnée"></a>

## Ordered follow-up

1. **Realization `.wfrog` defaults**: Python Boolean corrected in the 6 September
   continuation; generalization to other families follows their own contracts.
2. **UI-independent public source model**: preservable interfaces, references,
   regions/extensions; pure-source tests without a front panel.
3. **Transactional canonical reader/writer**: cross-tool golden files, explicit
   draft migration, original preservation and diagnostics.
4. **Common contract/capability registry**: classes, functions, structures, full
   fields and generated projections; separate read/preserve/edit/validate/lower/execute
   statuses, not one “supported” Boolean.
5. **Unified semantic validation/FIR**: same diagnostics/type resolver, each profile's
   closure, source attribution and public-IR adapter.
6. **Cluster/Picture/Waveform/Variant profiles**: source, packages, semantics, runtime
   and migrations documented together before promising portability.

Convergence does not mean merging repositories or renaming every structure. It means
one authority per contract, proven compatibility, visible limits and migratable changes
without losing user work.

<a id="reprise-du-6-septembre-2026--defaults-boolean"></a>

## Continuation of 6 September 2026: Boolean defaults

LF-25 is corrected in Python Boolean/Button consumers.
`boolean_runtime.merge_boolean_realization_defaults` loads `realization_refs` manifests,
filters `target_classes`, then reuses existing class/role merging. Order is explicit:
manifest defaults, package-local defaults, instance properties/visual. Explicit
`false`/`0` values retain precedence. Source documents/packages/manifests are unchanged.
No public font size is copied into the renderer.

Resolution runs once at runtime initialization; later rendering/events reuse resolved
properties. The common strict reader rejects missing declared manifests or ambiguous
JSON. Scope remains both Boolean classes and direct example-package references
expressed from repository root. Button defaults, including historical `style.face`
aliases, retain their existing path. This bounded reader resolves neither a general
package-dependency graph nor an arbitrary widget library.

Local continuation evidence, Python 3.11 and existing dependencies under
`Source/.tools/frog-language-audit-python`:

- Targeted `test_boolean_realization_defaults.py`, `test_runtime_ui_slice06.py` and
  `test_runtime_ui_slice10.py`: **22 passed**, 0.84 s. The original test now confirms
  public 12/13 px. Eleven added regressions cover three-level precedence, class/role,
  source preservation, explicit values, single loading and invalid-manifest diagnostics,
  including non-text `class_id` / `target_class` selectors.
- One final `pytest Implementations/Reference -q` pass: **514 passed, no failures or
  skips**, **25.79 s**, with `Source/.tools/WinLibs/mingw64/bin` in Python's PATH/Path.
  This includes eleven LLVM dynamic bridges 05–15 and supersedes fragmented early runs.
- `python Implementations/Reference/check_reference_workspace.py`: **PASS**, Examples
  01–15, 8.34 s. Twenty `.wfrog` families: 0 errors; pre-existing
  `Picture.placement_bounds` warning retained.
- Official navigation regenerated/verified after the new test file: **729 search
  paths**. `git diff --check` passes on local changes.

The first targeted launch encountered an inaccessible global pytest temporary
directory; reported runs use a new `--basetemp` under FROG's `.pytest_tmp`. The first
complete-suite attempt also lacked `jsonschema` in user Python; existing local
dependencies were added to `PYTHONPATH` without global installation. On this Windows
host, Python needed a normalized clang PATH for native tests; skipped tests were
not counted as passes.

Other convergence limits remain: UI-independent Studio model, transactional public
writer, FIR-to-canonical-IR adapter, shared capability registry and Cluster/Picture/
Waveform profiles. This correction changes no Studio artifact and replaces no browser
CSS/JavaScript visual check. Studio evidence above remains attributed to the initial
pass and its build version.
