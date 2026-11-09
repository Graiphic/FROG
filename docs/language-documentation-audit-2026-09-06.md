<a id="audit-documentaire-frog--studio--6-septembre-2026"></a>

# FROG / Studio documentation audit — 6 September 2026

<a id="périmètre-et-preuve"></a>

## Scope and evidence

Audit of local files and Git history, without fetch, push, code generation or
launching Studio. Local remote-tracking references do not prove the current state
of published sites. Pre-existing modifications were preserved.

| Repository | Local HEAD at the start | Observed state |
| --- | --- | --- |
| FROG | `55e4554` — Timed Loop publication, 30 August | Branch `agent/fix-default-asset-path-case`, modifications and untracked local contracts |
| FROG-STUDIO | `606db16` — authoring and typed structures, 5 September | Branch `agent/complete-studio-interaction-conformance`, many local changes |
| Graiphic-Studio | `19a4f4b` — realization documentation, 25 August | `main`, clean before this pass |

Initial inventory: 560 Markdown files in FROG. Link checking traversed 250 documents:
`Readme.md`, `GOVERNANCE.md`, the four orientation `FROG-*.md` files and every family
below. Documents created concurrently during this intervention are excluded from
that initial count.

| FROG family traversed | Markdown | Authority examined |
| --- | ---: | --- |
| Expression | 28 | Canonical source, types, widgets, structures and schemas |
| Language | 7 | Validated meaning and execution rules before FIR |
| IR | 12 | Derived representation, preservation, lowering and handoff |
| IDE | 11 | Editing and observation concepts, without redefining the language |
| Conformance | 184 | Public accept/reject/preserve expectations by stage |
| Versioning | 2 | Corpus version and surface status |

The 32 Graiphic-Studio Markdown files were traversed for links and organization.
FROG-STUDIO's `docs/` and `native-portable-engine/docs/` contained 67 inventoried
documentation files, including 50 Markdown; targeted reading covered the README,
structure contracts and delivery/architecture reports. This is neither exhaustive
semantic reading of every paragraph, visual validation of every capture nor
execution of every example.

<a id="autorité-à-conserver"></a>

## Authority to preserve

The published separation is sound: `Expression/` defines authored content;
`Language/` defines accepted meaning; `IR/` derives that meaning without becoming
its primary owner; `Conformance/` publishes expected outcomes. `Versioning/`
describes publication boundaries, not new semantics. See `Language/Readme.md:53`,
`GOVERNANCE.md:166` and `GOVERNANCE.md:330`.

`IDE/Editors.md:83` assigns source, meaning, types and cross-tool contracts to FROG.
Graiphic-Studio documentation describes product workflows; FROG-STUDIO's `STR-*`
requirements govern its gestures, visuals and implementation evidence. A realization
or reference validator does not become normative merely by existing. Implemented
appearance does not prove execution.

`Versioning/Readme.md:172` declares **corpus `0.1-draft`**, while line 187 declares
**source `spec_version = 0.1`**. This is not contradictory: the distinction is
explicit. Do not confuse these with `metadata.program_version`, `wfrog_version`,
an extension profile or the Studio build number.

<a id="constats-prioritaires"></a>

## Priority findings

1. **A green CI does not guarantee schema checks.** At audit start,
   `Validator/tests/test_public_structure_contract.py:10` made `jsonschema`
   optional and conditionally skipped validation tests.
   `.github/workflows/reference-workspace.yml` and `reference-pipeline.yml`
   installed only `pytest`. Existing assertions therefore did not guarantee
   execution of the Draft 2020-12 engine. Dependency/CI correction belongs to the
   central pass; this report does not announce its result.
2. **Public status is contradictory and outdated.** `FROG-Project-Status.md:42`
   lists examples 01–20 and line 319 lists 01–24, whereas 47 numbered directories
   exist. `Versioning/Matrix.md:23` correctly bounds the public runtime to 01–15,
   but its closure/kind tables list only 01–07; lines 254 and 285 still mention
   01–09. Runner `Pipeline/check_examples01_10_full.py:2` correctly describes 01–15
   and intentionally retains its old name for compatibility. Mere presence does
   not establish public execution of examples 16–47.
3. **Accepted local contracts ≠ verified Git publication.** Profiles
   `Expression/Typed Binding Contract v1.md` and `Case Editing Profile v1.md`
   were untracked despite saying “accepted” on line 3. The former explicitly
   retains unfinished stages on lines 154–162, including Studio envelope migration.
   Preserve these limits and bind every profile publication to a verifiable
   revision and capability matrix.
4. **Loop width: product contradiction corrected.** FROG already specifies `i64`
   for count/index (`Expression/Control structures.md:716`, `:721`, `:868`;
   `Language/Control structures.md:410`). Product requirements `STR-FOR-022` and
   `STR-WHILE-010` still described this width as undecided. They now align with
   FROG; LabVIEW I32 remains historical and imposes no saturation rule on the language.
5. **No general source/IR schema check identified in the runner.** Searching
   Python schema references under `Implementations/Reference/` identified the
   dedicated structure-node test, but no general invocation of the root `.frog`
   schema or IR family. Manual artifact checks are not exhaustive Draft 2020-12
   coverage. `Expression/schema/frog.schema.json:5` honestly announces a conservative,
   mainly top-level scope; acceptance by this schema proves neither complete typing,
   meaning nor lowering.

<a id="liens-et-navigation"></a>

## Links and navigation

Documentation updated after these findings: `FROG-Project-Status.md` and
`Versioning/Matrix.md` now distinguish the 47 existing directories from public
runtime closure 01–15. Older tables are explicitly historical and partial; no
kind or coverage level was extrapolated to directories absent from those tables.
`Versioning/Readme.md` links the compatibility/profile contract. The project-status
test-install command uses the explicit dependency file.

The static scan of 250 FROG documents examined 356 local HTML `href/src` and inline
Markdown references. One certainly missing path was corrected: the logo in
`Conformance/invalid/12_public_interface_declaration_must_not_require_front_panel_widget_existence.md:2`
now goes up two levels. This Windows scan does not certify anchors, external links,
reference-style Markdown links or Linux-sensitive case differences.

The scan of 32 Graiphic-Studio documents examined 126 local references. It flagged
26 references in `docs/index.md` as unresolvable relative to that file: 25 `docs/...`
links and one `assets/...` image. All targets exist from the repository root. The
25 links remain a direct-GitHub-Markdown issue. The site uses a Docsify homepage
and `relativePath: true` (`index.html:193–197`); the plugin at `index.html:213`
rewrites **images**. Check the homepage and direct `#/docs/index` route before
classifying every warning as a deployed-site error or changing paths.

The existing `.github/scripts/verify-pages-navigation.ps1` first ran read-only:
**FAIL**, first sidebar path missing: `/Expression/Case%20Editing%20Profile%20v1.md`.
This concerns a local addition, not proof of a remote publication failure. Its
generated sections exclude `Versioning/`; it is not a check of all editorial links.

After creating convergence documents and obtaining coordination approval, the
official `build-pages-navigation.ps1` and verifier ran under `pwsh`, as in the
workflow: **PASS**, 728 search paths and 19 root entries. The compatibility contract
and Case profile are in navigation. The generator is unchanged; reports remain
discoverable through editorial README/contract links. A supplementary check of
seven new or modified entry documents verified 47 local references with no missing
target. This does not certify anchors or the remote site.

<a id="corrections-de-tests-vérifiées"></a>

## Verified test corrections

Tab/Tree failures came from tests predating published contracts, not a retroactive
justification using `fa8373f`: that commit changed neither Tab nor Tree.

- `7f20ae55` (19 May) publishes five `page_label_display*` parts, their property
  and SVG markers. The Tab test retains an exact set and also checks
  `svg_or_host_surface` bindings, consistent with `Default/Tab.md:111`.
- `ca21e49e`, then `5a884a34` (22 May), publish Tree column headers and `active_cell`,
  respectively, both host overlays.
- `d8935c9c` (22 May) adds five Tree symbol SVGs. They are not shell geometry.
  Tests now separately check six exact shell markers, overlays, the exact five-symbol
  registry and the sole `item_symbol_path` marker in each symbol resource.

Actual targeted validation: Python 3.11, `-B -m pytest -p no:cacheprovider`, files
`test_tab_deepening.py` and `test_tree_deepening.py`: **7 PASS**. This is neither a
global workspace result nor runtime evidence.

<a id="conclusion-actionnable"></a>

## Actionable conclusion

Organization should follow existing responsibilities, not merge all three
repositories. Convergence should use a versioned capability registry linking
**FROG contract → source/profile form → positive and negative fixtures → Studio
test → validation/lowering/runtime status → product documentation**. Human-readable
matrices can derive from it; normative decisions still require review by their layer.

Recommended priorities: make test dependencies explicit without silent skips;
synchronize 01–15 / 16–47 matrices; publish profiles with limits/evidence; separately
check GitHub links and Docsify routes; bind product pages to a verified build/capture.
Keep incomplete profiles explicitly incomplete and reject unsupported exports.
This documentation pass claims no completed general envelope migration, build or
remote publication.

<a id="annexe--échecs-du-rendu-python-ui-après-audit-complet"></a>

## Appendix — Python UI rendering failures after the complete audit

Bounded reproduction: eight `Runtime/python/tests/test_runtime_ui_slice` files,
05, 06, 08, 09, 10, 11, 12 and 13_15; Python 3.11, isolated dependencies, pytest
cache disabled and a new temporary directory. Before correction: **9 FAIL, 21 PASS**.
After the adaptations below and three new regression cases: **1 FAIL, 32 PASS in
2.70 s**. This is neither the global Reference result nor browser visual validation.
Affected rendering uses the local JSON reader in `Runtime/python/runtime_core.py`,
not the new hardened common reader; the examined discrepancies predate that change.

| Scope | Evidence and treatment |
| --- | --- |
| Numeric 05 | `a301cba` / `7af8c9a` publish new dimensions and `placement_bounds` geometry. The test now follows the 500×128 panel, 96×32 widgets, 220×88 SVG, source colors and `value_face`, without certifying obsolete, ineffective `frame` parameters. Routes, actions and shared-snapshot equality remain tested. |
| Boolean 06 | `869e10a`, then `5d2a24e`, update realization, colors and placement. Assertions follow the `placement_bounds` crop, `state_face` / `focus_ring` parts, off-white and 1px cyan focus. The local button has `data-toggle-target=true` when the current value/hidden input is false; the old `value=true` assertion confused these roles. |
| Enum 08 | `f9af09e` migrates `style.dropdown` properties to `style.popup` and associated skins. Assertions follow the hidden selector and source popup colors. The global `<pre>` ban becomes a source-inspection-link check: this markup now belongs to artifact panels, not a permanent UI-state dump. |
| Path 09 | The current realization has a 0.5px border and hidden SVG focus (`1bfc9a1`, public SVG-focus contract). Assertions match those values while retaining both independent pairs and interaction checks. |
| Button 10–15 | `39f6e27` publishes `style.button_face.*` and `button_face` SVG markers without migrating Python. The core now forwards these properties; the renderer reads their states and aligns geometry, overlay, CSS and JavaScript selector with that part. Older `style.face` / `face` documents/skins remain readable through a bounded adapter; canonical properties take group precedence when present. Tests retain expected colors and press/release/read behavior. |
| Boolean in Button | The indicator renderer now receives its resolved SVG path through `asset_map`, as in Boolean runtime. Its absence actually produced `missing-skin`; this was not an obsolete assertion. Source texts `Off` / `On` replace the old `FALSE` / `TRUE` expectations. |

New cases check canonical/historical custom geometry, all six false/true/hover/pressed
colors, canonical precedence, fallback of an absent state to its base, both generations'
selectors and the Boolean SVG's presence. They do not prove actual CSS/JavaScript
rendering in a browser.

**Initial-pass defect: realization-property inheritance.** Corrected during the
6 September 2026 continuation: realization references are now resolved before package
defaults and instance overrides. The original test and eleven extra regressions pass,
including diagnostics for non-text `class_id` / `target_class` selectors. The latest
single complete Python-suite pass reports **514 passed, no failures or skips in
25.79 s**, with WinLibs explicitly visible to the Python process; see the
[continuation evidence](language-studio-convergence-audit-2026-09-06.md#reprise-du-6-septembre-2026--defaults-boolean).
The finding below remains the historical initial-pass record.

Button example indicators omit source typography. The core does not merge
`boolean.default.wfrog` defaults, so the renderer uses 18px for caption/state text.
The public circular realization specifies **12px and 13px**, respectively (lines 511
and 518). Test 10 retains a failing assertion against those public values instead
of accepting 18px fallback; its old 18px/12px expectations also disagreed with the
current contract. No existing Boolean-core default merge could be reused through
a simple local parameter. The suite therefore cannot be declared green. The precise
follow-up is resolving realization defaults before instance overrides and testing
that precedence, without importing a new generic policy into this bounded correction.
