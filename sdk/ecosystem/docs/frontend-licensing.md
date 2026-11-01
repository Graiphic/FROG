# Common Frontend and CLI: Apache-2.0 decision

**Status: accepted by the project maintainer on 1 November 2026.**

Graiphic has approved **Apache License 2.0** for the Graiphic-owned files selected
for the common FROG Frontend library and its command-line interface. The license
text is the repository's existing [LICENSE](../../../LICENSE).

## Scope of the decision

The common component includes reading, explicit dependency/type resolution,
structural and semantic validation, FIR derivation, diagnostics, source maps,
provenance and the necessary public contracts, examples and tests. Studio and the
CLI consume the same versioned semantic owner. The approved open-source direction
is an autonomous library and CLI, not a separate limited reference generator.

The decision covers original Graiphic-owned files selected for this component,
including the selected files extracted from Studio or Runtime. Their former
location in a private implementation does not require a second decision about
the chosen license for those Graiphic-owned files.

Studio's editor, repair UI and historical integration, the rest of the private
Runtime, backend optimization recipes and unrelated services are outside this
decision. This document does not relicense those repositories as a whole.

## Provenance and third-party material

The release manifest must identify the actual files and their origin. Existing
file-specific third-party licenses, copyrights, notices and attribution must be
preserved. Copied or adapted third-party material remains third-party material;
it must not be relabeled as original Graiphic code merely because it moved.

Historical extraction inventories used `NOASSERTION` where no explicit license
declaration had been observed. For original Graiphic-owned files in the approved
scope, this accepted decision now supplies the project's licensing direction.
The next inventory refresh should record that decision separately from the
historical observation and capture hashes. Entries of uncertain or third-party
origin still require accurate classification and their applicable notices.

The future code release must apply the selected license and appropriate file
markers to the actual approved payload, preserve applicable notices and publish
its provenance. This documentation update does not change source headers,
inventories or files currently being compiled in the integration workspace.

## Publication remains a separate step

This commit publishes **the licensing decision and documentation only**. It does
not publish the extracted implementation, C header, CLI binaries or private
Runtime code. Code publication remains deferred until the main integration work
has finished and its qualification and release preparation are complete.

The decision to open the Graiphic-owned common Frontend under Apache-2.0 is
accepted; implementation qualification, identification of the exact release
payload and preservation of third-party obligations remain separate work.

See the [operation contract](frontend-api.md), [result contracts](frontend-result-contracts.md)
and [external-tool guide](../../../docs/frontend-tools-guide.md).
