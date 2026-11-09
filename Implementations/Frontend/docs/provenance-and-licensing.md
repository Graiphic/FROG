<a id="provenance-et-licences--originaux-apache-20-et-décodeur-tiers-bsd"></a>

# Provenance and licenses — Apache-2.0 originals and BSD third-party decoder

Updated 2 November 2026. **Apache-2.0 is accepted for original Graiphic portions
selected for this Frontend and CLI**, including selected files extracted from
Studio and Runtime. The accepted decision is retained in
`FROG-Context/Decisions/frog-common-frontend-apache2.md`. This does not relicense
Studio or Runtime wholesale. Third-party material retains its rights; no new
license choice is requested for the originals.

[LICENSE](../LICENSE) reproduces FROG's Apache-2.0 text exactly, without inventing
a holder or year. [NOTICE](../NOTICE) states the scope. Per-file declarations appear
in [license-inventory.json](license-inventory.json). Its v3 record of 1 November is
historical; the integrator must reconcile it with the `0.2.0-local` extraction and
third-party decoder below. This documentation update changes neither API nor
semantics and claims no new compiled result.

<a id="captures-et-droits-distincts"></a>

## Captures and rights are distinct

The historical `source-origin.json` record contained 178 entries; this number does
not describe the new `0.2.0-local` closure. For the 181 old v2 inventory entries,
v3 retains `upstream`, `earlierOrigins`, `original_hash`, its baseline and complete
historical license observations, including the then-observed `NOASSERTION`, hashes
and `publicationRightsReviewed:false`. Moved files may evolve; their capture does
not require immutability. Snapshot parity remains a separate check.

`NOASSERTION` described the absence of an explicit grant observed at that date.
The 1 November decision grants rights for selected originals; moving them into
FROG had not done so. Historical observations remain readable and are not rewritten
retroactively.

| v3 field | Meaning |
| --- | --- |
| `original_hash` | Retained historical capture; null if absent. |
| `current_hash`, `current_bytes` | Bytes read from the current owner. |
| `capture_relation` | Identical capture, evolution or snapshot parity requiring review. |
| `historicalLicenseObservations` | Earlier per-file observation with its hash. |
| `licenseEvidence`, `declaredLicenseScope` | Apache-2.0 for original Graiphic portions, no invented third-party grant. |
| `thirdPartyReviewFindingIds` | Concrete review items, distinct from accepted rights. |

Runtime provenance of `frog/semantics/*` contracts is preserved. Catalogs, generated
headers, fixtures, tools, CMake, examples and docs have individual entries. v3 listed
files present at capture time; later extractions/notices must enter the current
record before distribution. The inventory includes itself but excludes its own hash
to avoid a cycle. LICENSE, NOTICE and this note have exact hashes; links to the
inventory do not form a circular hash chain.

<a id="fermeture-et-kit-réellement-consultés"></a>

## Closure and kit actually inspected

The historical lexical `tools/audit_frontend_closure.py` record covered the then-45
units and three API/Operation/CLI adapters: **48 units, 179 files, 313 edges, 0 refusals**.
It does not qualify new readers, codecs, resolvers or the `0.2.0-local` decoder.
Its paths, hashes and edges are recorded in v3. No auxiliary report, compiler,
linker or product test ran. This check does not replace binary-symbol analysis.

The E3 kit was inspected in its historical archive, linked to `final-kit-receipt.json`;
the temporary copy had been removed. v3 distinguishes FROG support files, archive
hashes and current bytes. Installed binaries, generated qualification copies and
historical evidence are not sources of the new release. Their receipts remain
historical. FROG support files fall under `FROG/LICENSE` and existing notices; the
new decision does not extend licensing to other private portions. The inventory
does not replace a manifest of the actual assembled candidate package.

The final refresh also lists persistent exporter `tools/export_frontend_candidate.py`,
its `tests/test_candidate_export.py` test and
`examples/installed-c-consumer/CMakeLists.txt`. The exporter's `source_selection`
function reads the current selection; v3 retains hashes without capturing a kit.
The E3 kit's 56 support files remain a distinct historical set, preserving each
earlier observation when a public file evolved. A selection hash is not an archive receipt.

<a id="revue-tierce-ciblée"></a>

## Targeted third-party review

The candidate now contains the Zstandard 1.5.7 document decoder and FSE/HUF/xxHash.
The historical “without Zstd” observation no longer applies. LLVM kernels,
XSF/Cephes/AMOS, libffi, stb and the private runtime remain outside this extraction;
the Frontend carries relevant contracts without embedding their realizations.
The compiler and standard library belong to their separate distributions.

The existing vendor selects BSD licensing, retained in [LICENSE.txt](../third_party/zstd/LICENSE.txt),
with the GPL alternative in COPYING.txt. The 35 initially copied files were compared
byte for byte with the local vendor; the upstream archive was not downloaded or
revalidated during extraction. [UPSTREAM](../third_party/zstd/UPSTREAM.md) retains
version, provenance and exact scope. Meta Platforms and Yann Collet remain attributed;
these sources become neither project-original nor Apache-2.0.

Distributed sources retain notices. Binary distribution must include the BSD license,
conditions and disclaimer. The current inventory/installation must include these
texts and distinguish ten compiled C units from other copied headers. Compilation
and binary closure of this new kit remain to be qualified.

Three historical findings remain in the inventory's `thirdPartyReview`; the current
Zstandard entry must supplement them:

1. **Documentation catalog** — `manifests/function-bindings/studio.function-bindings.v2.inc`
   contains 1,384 URLs to NI, NIST DLMF, Microsoft and mathematical publications,
   together with contract descriptions. No third-party kernel was identified in
   that documentation catalog; this says nothing about the newly added decoder.
   However, the review did not compare every sentence with cited pages: substantial
   quotation/adaptation remains to be determined before publication. Original
   Graiphic portions have a confirmed license; a URL grants no rights to its content.
   References are retained and no third-party text is assumed Apache-licensed.
2. **SHA256** — `src/Sha256.cpp` contains a short implementation and standard constants,
   with captured Studio origin. It cites no third-party implementation; no borrowing
   from an external library was established. Standard constants alone prove neither
   borrowing nor code-text origin; no upstream holder/copyright is invented. The
   original-code grant applies. Any subsequently discovered third-party ancestry
   must be added to the inventory.
3. **Calibration and fixtures** — `numeric_calibration_contracts.hpp` carries
   `nist-its90` identities and defaults, not Runtime thermocouple calculation tables.
   Six fixtures are attributed with verified hashes in `tests/fixtures/provenance.json`.
   This makes neither metadata nor fixtures a third-party implementation and does
   not attribute Graiphic code to NIST.

This search is bounded to retained content, comments, references, captures and kit
provenance. It is neither exhaustive Git-history evidence nor comparison against
every published code/text. Concrete uncertainties remain separate from the accepted
license decision.

<a id="publication-et-qualification"></a>

## Publication and qualification

The candidate remains local. The licensing decision does not authorize an immediate
code push: publication is deferred until the work and release qualification finish.
v3 distinguishes grants for originals, third-party review, technical validation
and publication authorization. Older E2/E3 captures and v2 observations may mention
pre-decision `NOASSERTION`; they remain historical. The current README and
`candidate-profile.md` describe accepted Apache rights; their `0.2.0-local` bytes
must be bound to the current record without reattributing old hashes. Older README
wording must not be presented as its current claim.

The integrator binds exact package content to this inventory, addresses identified
documentation items and the third-party decoder, retains LICENSE/NOTICE and applicable
attribution, then produces receipts for the actually rebuilt candidate. This task
ran no build, product test, download or remote publication and changed no production source.
