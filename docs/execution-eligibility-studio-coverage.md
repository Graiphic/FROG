# Execution eligibility — Studio implementation coverage

## Current checkpoint — 14 October 2026

This informative coverage record does not extend the public language or its
execution eligibility contract. The previously recorded signed **0.0.5.313**
belongs to the checkpoint through lot 94. Later reference-node wiring lot 101
was qualified on signed **0.0.5.317**. These are scoped qualification receipts,
not a claim that every active working-tree change belongs to either binary.
See the [complete reconciliation](studio-reconciliation-2026-10-14.md) for the
source baseline, authoring revisions, contracts and remaining implementation gaps.
Regression tests remain paused globally; the user authorized only the targeted
groups below. No complete clean all-target/full-CTest gate or remote publication
is confirmed by this record.

The checkpoints are separate: **87: 4/4 PASS** qualifies normal private Run with
Highlight initially disabled and POC OFF, interactive controls, read-only
indicators and return to authoring at Stop. **89: 7/7 PASS** qualifies the public
captured-Event Unbundle Source→IR→SDK corridor and its neighbors.
**94: 2/2 PASS** qualifies current-font Path/String sizing through real Options,
text-size menus, Size to Text and pointer resize, including creation, values,
types and connections. **101: 2/2 PASS** qualifies actual reference-to-Property/
Invoke wiring, connected type adoption, compatible member preservation,
incompatible diagnostics, nested targets, save and Undo. It does not supply an
Application Control graph execution provider. The later reference-value widget
and Cluster-field working-tree changes remain a separate qualification lot.
These results are not combined into a global gate.

Unbundle reuses the typed Event Record carrier and existing SDK provider
([contract](../sdk/cpp/docs/runtime-contract.md)). It introduces no native LLVM
Record kernel, arbitrary Array/Enum transport or aggregate Front Panel widget.
The qualified native public CPU corridor remains flat f64/Add, distinct from
private Run simulation and the SDK Event corridor. The full execution engine
and broader runtime milestones remain open; the Adapter UI owner still stops
before Clipboard, while the recursive owner has a separate targeted PASS.

All earlier checkpoints below are retained as historical evidence. Their
then-current versions, test-pause statements and pending-publication statements
do not describe the state above; historical failures are not reclassified.

## Preserved historical record


## Current checkpoint — 4 October 2026

This record is informative and does not extend the public language or its
execution eligibility contract. The latest complete local Studio diagnostic
campaign failed on **0.0.4.635: 305/421 passed, 115 failures and one timeout**, without exclusions.
It follows a clean compilation and an incremental continuation, so it is not a clean,
uninterrupted qualification gate. The regression pause has been lifted for
the complete verification and progressive repair requested by the user.

Highlight Execution now evaluates implemented basic numeric, Boolean and
comparison operations, recursive explicit numeric conversions, and
For/While/Sequence structures with invocation-local shift registers. Public
FROG implicit conversions still follow [Type, section 11](../Expression/Type.md);
Studio's explicit To functions have their own documented conversion behavior.
The new portable owner `frog-basic-execution-smoke` passed on signed **635**.
The application owner `frog-highlight-execution-ui-smoke` passed on signed
**635**, with 13 native captures: the unchanged user file test1.frog computes
0 + 0 followed by Increment = 1 through 35 For iterations. Real palette/editor
creation supplies the normal visual fixture geometry.

Runtime POC remains **OFF** in the canonical application. These simulation
results are not evidence of a native compiler/backend supporting the whole
public language, the complete EXEC/VAL matrix or every Studio catalog entry.
Unsupported operations retain explicit diagnostics. The current changes,
adjacent failing owners, real-geometry audit, clean all-target/full-suite gate
and publication are tracked in the
[Studio request queue](https://github.com/Graiphic/FROG-Context/blob/main/Current/studio-request-queue-2026-10-02.md)
and the dated verification report in FROG-STUDIO
(`native-portable-engine/docs/reponses/2026-10-04-reprise-integrale-execution-bindings.md`).
Publication of the current correction series is still pending.

## Historical checkpoint — 2 October 2026

This coverage record is informative. The canonical local Studio binary is
**0.0.4.047**; Studio main **314c338d41a6633b891b9e28271a88afa635981f** has been published and its remote
commit verified. Runtime POC remains OFF. Tests are paused;
the latest full attempt failed on **0.0.3.935**, **300/403 passed, 103 failed**.
Recent compilation and Git publication do not qualify the complete EXEC/VAL matrix.

See [Studio/source compatibility](studio-source-compatibility.md) for the current
source-envelope and integration limits. The results below belong to their dated
September revisions and remain historical evidence; they are not a fresh gate
for 0.0.4.047. The September working-branch commits are now ancestors of Studio main.

Revision 2 — 16 September 2026. This record is **informative**.
The [EXEC rules](../Language/Execution%20eligibility.md) are the contract;
this page records available evidence, not automatic equivalence between the
private editing model and every public FROG source.

## Qualification

<a id="checkpoint-de-publication-du-16-septembre-2026"></a>

### Publication checkpoint of 16 September 2026

The corrections below are included in FROG-STUDIO
[`48f300f`](https://github.com/Graiphic/FROG-STUDIO/commit/48f300fa2e9d45770f2d6a9b9f4af9d8168352b3),
published on `agent/complete-studio-interaction-conformance`, without merging into `main`.
The current delivery is **0.0.2.513**, Authenticode **Valid**, with the canonical
Desktop shortcut verified. Its delivery record also describes subsequent changes:
animated diagnostic focus, icon formats and the Timestamp palette.

Before publication, **16/16 targeted tests** were rerun successfully:
catalogs/contracts and typing, navigation, Timestamp, Variant selection,
Win32 diagnostics, document readiness, Custom icon, binding profile, version
and graphical outlines. This is neither a full rebuild of every suite nor
qualification of the 137 VAL scenarios. Runtime POC remains OFF.
The previous audit's clipboard failure is not declared resolved.

<a id="livraison-initiale--compilation-et-runtime-séparés"></a>

### Initial delivery: compilation and runtime separated

Delivery **0.0.2.479**, valid Authenticode signature and canonical Desktop shortcut
verified. This section describes the initial local delivery; its sources are now
included in the Studio checkpoint above.

`win32_execution_validation_smoke.cpp`: **110 checks passed**. Five targeted CTest
suites pass: fixture conformance, document readiness, structure execution validation,
Win32 diagnostics and binary-version contract. Configuration: Windows, Runtime POC OFF,
WARP test rendering.

| Targeted situation | Verified local result |
| --- | --- |
| New empty diagram without an interface obligation | Buildable, no prohibition overlay; absent runtime explained separately. |
| Two numeric constants into Add's a/b, output unused (VAL-011) | Valid, one global runtime diagnostic, no false error for Add/constants. |
| Add missing required inputs (VAL-012/013, Add scope) | Compilation blocked and prohibition overlay; navigation to the object available. |
| Public source without a validator/lowerer | Still blocked / unsupported, not treated as an empty editing diagram. |
| Run on Front Panel and Block Diagram | Each actual handler opens the same diagnostics service; capture released. |
| Presentation | Light/dark, sizes 18/24/32, French at 15 points, actual wrapping, Tab/Escape, second icon unchanged. |

A visual check in the actual executable confirmed the empty document, valid Add
and corrected Block Diagram Run click. It revealed an old test gap: invoking the
Front Panel handler with a Diagram HWND did not prove the Diagram handler forwarded
the click. The test now uses both distinct handlers.

Delivered binary SHA-256:
`CEEB01D39BF2B1E5B26D2291B8498016E35574368A28059BADD1439230F1F23E`.
Validation generates no artifact and does not enable the runtime. This targeted
evidence alone qualifies neither every VAL variant nor public backends.

<a id="qualification-précédente-publiée"></a>

### Previous published qualification

| Rules | Observed coverage | Evidence / limit |
| --- | --- | --- |
| EXEC-001, 003–007 | Portable validation of identities, ports, required inputs, fragments, bindings and editing-model types | FROG-STUDIO tests: document_readiness_smoke.cpp. Not exhaustive qualification of the public language. |
| EXEC-008–015 | Studio model checks: region outputs, defaults, loop count/condition, zero case, boundary/scope, Event, Sequence, Timed, registers and instantaneous cycles | structure_execution_validation_smoke.cpp: targeted positive/negative cases; profiles and backend guarantees require separate qualification. |
| EXEC-002, 020 | Per-document/model cache, correction, undo/redo, stale rows and navigation into hidden nested regions | win32_execution_validation_smoke.cpp, private Windows desktop. |
| EXEC-001, 018, 020 | Absent runtime and unsupported public source remain visible causes; no fabricated execution | The current Studio delivery sets FROG_STUDIO_ENABLE_RUNTIME_POC=OFF. |
| IDE contract | Triangle retained with red overlay; second icon unchanged; Run inspectable; mouse capture released; blockers not hidden by filtering | Same native test, 73 checks passed on 16 September 2026, including light/dark, three sizes, actual clicks on both panels and French labels. A result for tested code, not global product qualification. |
| EXEC-016–019 | Complete dependency closure, public profiles, artifacts and all targets | Not qualified by the tests above. No “fully supported” status claimed. |
| EXEC-021 | Detailed tracking of the study's 137 scenarios | The matrix retains “To qualify” until scenario/revision/target evidence is attached. |

Implementation repository: Graiphic/FROG-STUDIO, directory
native-portable-engine/tests/. These tests prove neither 137 individual scenarios
nor every backend.

<a id="contrat-public-et-extensions-dédition"></a>

## Public contract and editing extensions

A source's canonical representation, profile and semantic contract remain authoritative.
Extra primitives or structures in a Studio development branch are not silently promoted
into the base contract. In particular, extended loop modes, enum/integer selectors,
timing guarantees and public-source conversions require their own contracts and evidence.

For a retained public source without a validated executable projection,
public_source_readiness_unavailable indicates a Studio validator/lowerer limitation;
it does not establish that the source is invalid FROG.

<a id="mise-à-jour-de-la-preuve"></a>

## Evidence update

Local delivery verified on 16 September 2026: Studio **0.0.2.470**, valid Authenticode
signature and Desktop shortcut targeting canonical `build-nmake-verify/frog-engine-shell.exe`.
Four targeted CTest suites passed: `frog-ui-fixture-conformance-smoke`,
`frog-document-readiness-smoke`, `frog-structure-execution-validation-smoke` and
`frog-win32-execution-validation-smoke`. The native test uses a private Windows
desktop with `FROG_UI_TEST_FORCE_WARP_SVG=1`.
This evidence concerns locally tested code, subsequently published in private
FROG-STUDIO at commit `ac8fa9618d671c2983cee69e9ee76b5af7aa2e0a`
(working branch, not a main release). No exhaustive validation of 137 scenarios is claimed.

Additional prepublication check: 18/19 targeted Studio tests passed.
`frog-win32-inline-text-clipboard-smoke` fails on `string_control` with
“Studio exported incorrect clipboard text,” including an isolated rerun.
Its cause remains undiagnosed; the targeted diagnostics success is not global
qualification. The Event/Program Session backend passed its two separate tests;
full Studio integration remains distinct.

Qualification must record the EXEC rule and VAL scenario, implementation commit,
test command, configuration/target, result and evidence artifact. Failures and
limits remain explicit. Executable versions do not replace this information.
