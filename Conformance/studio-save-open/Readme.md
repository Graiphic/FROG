# Studio Save/Open format evidence

These three synthetic Graiphic examples are byte-for-byte outputs of the
Studio Save/Open paths. They are provided under the repository's
[Apache-2.0 license](../../LICENSE). The [manifest](fixtures.json) records their
SHA-256 digests, writer versions and qualified operations.

- **Scalar addition with Front Panel**, canonical Source 0.1. Signed Studio
  0.0.8.215 opened the generated Source, changed the default of B from 2 to 4
  in the Source editor, applied the edit, saved and reopened it in a fresh
  process. The round trip retained the F64 ports, bindings, IDs and two
  diagram notes. CI validates this saved file and derives FIR with its
  Source version and exact file hash in provenance.
- **Variant containing a Cluster**, draft 2. Studio 0.0.7.656, native UI
  regression T310. Save/Open/Run preserved the Bool, Numeric and String fields
  and their order. Variant To Data returned the typed Cluster. The regression
  executable identity is recorded separately from the Studio version.
- **Compact Standalone I32 Array**, draft 3. Studio 0.0.8.190, native large-array
  benchmark. Save/Open verified the same 10,000,001 elements. The large fixture
  qualifies persistence. The CI checks its bytes, envelope and schema; it does
  not expand or execute the Array.

The manifest records reader admission separately from schema coverage. The
shared reader accepts revisions 0–13. The ordinary writer schema covers
2–9 and 12–13. Draft 11 has a dedicated Picture Ring envelope. Draft 10
requires private class resolution. The synthetic feature fixtures in
`format-boundaries/` exercise revision selection, structural validation and FIR derivation. The revision 12
Tab links a page member through the explicit sibling dependency lock. Revision
13 exercises visual stack order and grouping on fields of a typed cluster.

To extend the evidence, save and reopen a redistributable fixture via the
actual GS document path. Verify the feature that distinguishes the revision
and retain the output bytes, Studio and Frontend identities, and results for
the operations claimed. A fixture with dependent artifacts needs its exact
dependency versions and resolution context. Keep the earlier fixtures
immutable and add a separate case for the new behavior.

The [coverage matrix](../../docs/studio-source-compatibility.md#coverage-matrix)
lists additional qualified profiles and historical evidence.
The [focused workflow](../../.github/workflows/source-compatibility.yml)
builds the shared Frontend from this checkout. It checks the sealed inputs
against the schemas and replays the declared semantic validations and version
refusals. Local equivalent, with the repository's
[Python test dependencies](../../Implementations/Reference/requirements-test.txt)
installed:

```powershell
$env:FROG_VERSIONING_FRONTEND = (Resolve-Path '<path-to-frog-frontend.exe>').Path
python -B -m pytest -p no:cacheprovider -q Implementations/Reference/Validator/tests/test_studio_source_versioning.py
```

Studio UI and Run results are attributed to the saved evidence. The public
workflow runs the schema and shared Frontend gates.

The current source reader supports revisions 0 through 11. Draft 11 has a separate [ordinary document schema](../../Expression/Schemas/frog.studio-picture-ring.schema.json) for embedded Picture Rings. The earlier signed examples above retain their original revision and qualification.
