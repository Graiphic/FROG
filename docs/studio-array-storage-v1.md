# Studio numeric array storage profile, version 1

Implementation notes reconciled 13 November 2026. The compact records are
part of the Studio authoring profile. The
[14 October reconciliation](studio-reconciliation-2026-10-14.md) retains the
original rollout evidence. See the [coverage matrix](studio-source-compatibility.md)
for the current evidence on persistence and Run.

## Envelope and author intent

The format is `frog.document.draft`. Automatic is the current default for new
documents and when loading a document without Storage. The ordinary writer
records all three modes, including explicit Readable, starting at revision 3:

```json
"storage": { "version": 1, "array_mode": "automatic" }
```

The three tokens are `readable`, `automatic` and `compact_standalone`.
Unknown versions or modes are errors. Changing this choice must
not change identities, types, shape, rank, field order, viewport or values.
The complete document determines the final revision: exact integer state can
require 4, complex state 5, Application Control 6, Connectivity 7 and linked
Type Definitions 8. Readable remains available at these revisions. See the
[writer profile](../Expression/Studio%20authoring%20document%20profile%20v1.md).
Readers reject revisions beyond their supported range before projection.
Mixing a private `format` with public `spec_version` remains invalid.
Revision 3 preserves Studio's historical private byte-string compatibility for
unrelated existing binary String constants. This admission is specific to the
private editing adapter; the public JSON/source reader remains strict UTF-8.

For supported numeric Diagram constants, Automatic requires compact storage
from 4,096 cells or when the remaining readable cell budget is insufficient.
Other eligible Automatic writes use the readable byte threshold and compare the
complete encoded object with the readable representation. Compact standalone
requests the compact encoding for supported values. The codec chooses raw or
Zstandard per block and embeds its payload. See the
[storage producer](../Implementations/Frontend/src/FrogDocumentArrayStorage.cpp)
for the exact guards.

## Supported values

The initial writer supports homogeneous numeric constant arrays in the Diagram
and numeric Array widgets in the Front Panel, including widgets inside clusters.
Text, Enum, cluster and composed widget cells remain readable. Diagram widget
template strings also remain readable until their resource accounting shares the
document decoder's budget. Empty arrays remain readable.

There is one authoritative representation per array value field. No full readable
copy accompanies the binary payload. Metadata surrounding that field stays JSON.

All representations preserve **authoring text**. Typed pages store canonical
numeric values as bits and retain exact text separately for noncanonical or
incomplete cells. A runtime must still use the authored type's own value
interpretation and diagnostics.

## Binary record profiles

The original two profiles concatenate records in serialized order. Each record
is a four-byte **unsigned little-endian** UTF-8 byte length, followed by that many
bytes. Records may cross chunk boundaries. There is no alignment padding, byte
order mark, terminator or platform-dependent C++ layout.

| Encoding token | Replaced field | Record contents |
| --- | --- | --- |
| `numeric-cell-text-le32-v1` | Diagram `array_constant.values` | Exact UTF-8 cell string. Shape and row order keep their existing authoring meaning. |
| `numeric-elements-json-le32-v1` | Array widget `props.elements` | UTF-8 JSON for one existing numeric element record, retaining its `indices`, numeric `value`, `defined` and `enabled` flags. Missing cells are not synthesized. |
| `numeric-cell-bits-le-v1` | Paged numeric diagram `array_constant.values` | `count` fixed-width little-endian words in row order, then optional text exception records. |

The field is replaced by an object with `encoding`, `count`, `decoded_bytes` and
`chunks`. `decoded_bytes` includes all lengths and data. These bounded integers
fit exactly in binary64. In the word profile, `element_type` must match the
node's exact element type. I8/I16/I32 and U8/U16/U32 use 32-bit words (signed
values are sign extended); I64/U64 use 64-bit words. F32/F64 use IEEE 754 bits,
including the sign of zero. Type-domain validation remains at the existing
authoring/Run boundaries. Each exception is LE32 index, LE32 UTF-8 byte length,
then that many bytes. Exceptions must name distinct zero placeholders in
strictly increasing order. Their exact text is restored on Open. Shape, rank
and count are checked together. Readers supporting only the legacy encoding
reject the new profile explicitly; Readable remains available on export.

Decoded bytes are split at fixed 65536-byte offsets. Every chunk except the last
has 65536 decoded bytes. Each chunk declares `codec`, `decoded_bytes`, `crc32` and
`data`. Supported codecs are `raw` and `zstd`. `data` is canonical padded RFC 4648
base64 of either the raw bytes or one complete Zstandard frame. There is no
dictionary or multi-frame concatenation. The writer uses pinned Zstandard 1.5.7,
level 3, and chooses Zstandard only when the compressed bytes are shorter.

`crc32` is eight lowercase hexadecimal digits of the decoded chunk's IEEE CRC-32:
initial state `0xffffffff`, reflected polynomial `0xedb88320`, final XOR
`0xffffffff`. It detects accidental corruption; it is not encryption or an
authenticity signature. Readers verify it for both codecs.

Unknown encoding/codec, noncanonical base64, inconsistent counts, truncated
records, trailing bytes, bad UTF-8, corrupt frames or checksum mismatch reject
the load with a storage diagnostic. They must never become empty arrays or zeros.
Readers bound the frame window, chunk size, decoded bytes and cell count before
allocating. The word profile has a separate aggregate 128 MiB decoded budget
for numeric words and their exceptions. The original formats retain their
64 MiB aggregate decoded budget. Neither budget enlarges the JSON tree budget
or the 64 MiB encoded document limit.
Record profiles must match their value field. Numeric element indices and Boolean
flags are checked before projection; invalid records cannot be silently skipped.

## Present limits and future ecosystem work

The word profile opens directly into Studio's existing numeric pages with
implicit zero pages. This does not implement lazy loading from disk. The
compatibility projection for Run still expands a bounded JSON view; successful
Save/Open of a large array does not qualify that size for Run. The monofile is
rewritten when saved.
Independent deterministic chunks keep unchanged chunk contents stable, but the
initial implementation recompresses them rather than retaining a lazy block cache.

Separated resources, per-array overrides, a global new-document default and an
independent exact external conversion tool remain future work. In Studio, switching
to Readable before saving expands supported compact values, subject to existing
readable JSON limits. Such a conversion is not a public-source export migration.
Promotion into canonical public source requires a separate version/profile decision
and interoperability qualification. The public 0.1 schema remains unchanged.

Owners: Studio's `frog-array-storage-smoke`, `frog-document-revision-smoke` and
`frog-properties-keyboard-ui-smoke`. Compilation and execution evidence must be
reported separately. [Integration status](studio-source-compatibility.md).
