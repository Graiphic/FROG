# Graiphic Studio authoring compatibility snapshot

Snapshot: 14 October 2026. Studio is the implementation evidence authority.
This page makes the actual declarative editor contracts accessible to readers,
generators and other authoring tools. It does not publish proprietary execution
implementation code and does not redefine canonical FROG execution.

The [source envelope profile](../Expression/Studio%20authoring%20document%20profile%20v1.md)
documents the actual `.frog` authoring shape. The
[snapshot manifest](Studio/snapshot-2026-10-14.json) identifies every origin and
SHA-256. The function files are byte-identical copies of Studio's active version 2
authoring contract families, not a second edited source of authority.

## Complete declared function surface

The snapshot contains **535 entries across 15 families**: 490 `partial`,
17 `editor_managed`, 2 `defined` and 26 `blocked`. These are contract states,
not counts of functions proved executable on every backend.

| Family | Entries | Actual contract snapshot |
| --- | ---: | --- |
| ApplicationControl | 4 | [studio.applicationcontrol.v2.json](Studio/authoring/functions/studio.applicationcontrol.v2.json) |
| Array | 58 | [studio.array.v2.json](Studio/authoring/functions/studio.array.v2.json) |
| Boolean | 16 | [studio.boolean.v2.json](Studio/authoring/functions/studio.boolean.v2.json) |
| Cluster | 9 | [studio.cluster.v2.json](Studio/authoring/functions/studio.cluster.v2.json) |
| Comparison | 19 | [studio.comparison.v2.json](Studio/authoring/functions/studio.comparison.v2.json) |
| Connectivity | 130 | [studio.connectivity.v2.json](Studio/authoring/functions/studio.connectivity.v2.json) |
| Error | 24 | [studio.error.v2.json](Studio/authoring/functions/studio.error.v2.json) |
| File | 32 | [studio.file.v2.json](Studio/authoring/functions/studio.file.v2.json) |
| Mathematics | 13 | [studio.mathematics.v2.json](Studio/authoring/functions/studio.mathematics.v2.json) |
| Numeric | 106 | [studio.numeric.v2.json](Studio/authoring/functions/studio.numeric.v2.json) |
| Path | 19 | [studio.path.v2.json](Studio/authoring/functions/studio.path.v2.json) |
| String | 58 | [studio.string.v2.json](Studio/authoring/functions/studio.string.v2.json) |
| Structures | 16 | [studio.structures.v2.json](Studio/authoring/functions/studio.structures.v2.json) |
| Timestamp | 8 | [studio.timestamp.v2.json](Studio/authoring/functions/studio.timestamp.v2.json) |
| Variant | 23 | [studio.variant.v2.json](Studio/authoring/functions/studio.variant.v2.json) |

The copied [function metadata schema](Studio/authoring/functions/studio.function-contracts.schema.v2.json)
retains its original `$id`: the source metadata authority is not renamed by
hosting a copy here. [Widgets](Studio/authoring/studio.widgets.v1.json) and
[structures](Studio/authoring/studio.structures.v1.json) retain their existing
`studio_private` authority and `provides_runtime_capability: false` posture.

## Reading these contracts correctly

Each entry exposes stable catalogue identity, definition/profile references,
ports and direction, type expressions and defaults, connection policy,
variadicity, resolver policy, Array/Cluster behavior, errors, effects and declared
runtime availability. Preserve that metadata; do not translate an unresolved
custom resolver into an invented portable rule.

A `catalog_id` is not necessarily a public primitive `type`. A palette item,
contract status or `runtime_availability` record is not a successful target
preparation. The active backend must actually admit the operation, effective
types, structures and providers. Private simulation support, SDK provider
support, native compilation and public Source lowering are separate facts.

Visible semantic terminal names are resolved by Studio's shared help/name
mechanism. They can differ from stable port IDs and metadata fallback names.
Creating an object from a terminal must use that same semantic resolution, not
rename IDs or introduce a catalogue-specific naming table.

The [terminal label snapshot](Studio/authoring/terminal-labels.v1.json) records
the shared English presentation labels, separately from the unchanged contract
metadata. Dynamic Property/Invoke members still require their actual resolver.
One concrete compatibility discrepancy is Quotient & Remainder: legacy output
ID `quotient` is the upper **R / remainder** terminal; ID `remainder` is the lower
**IQ / integer quotient** terminal. The current evaluator and help agree on this
association. Their generic metadata fallback names are not execution semantics.
Preserve endpoint IDs and existing connections; do not rename a saved graph.

The [reconciliation record](../docs/studio-reconciliation-2026-10-14.md) lists
qualification and remaining gaps. This documentation pass does not qualify a
changed product binary. Reconcile future changes against Studio, then regenerate
the snapshot and review contract/version effects; never edit these copies as an
independent implementation catalogue.
