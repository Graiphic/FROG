# Native Studio example documents

Local documentation proposal, 25 October 2026. Publication and product
qualification are separate from the presence of these sources.

The [versioned example index](index.v1.json) connects actual `.frog` documents,
their explanations, object IDs, expected results and backend requirements. Its
[structural schema](frog.examples.index.schema.json) describes metadata, not
operator semantics. Existing operator, type, binding and execution contracts
remain authoritative.

## Document profiles

The numbered historical dossiers keep their existing paths and Source
authority. The new [Studio corpus](Native/Readme.md) uses the documented
[Studio authoring document profile](../Expression/Studio%20authoring%20document%20profile%20v1.md).
An index entry with `profile: "studio-draft"` identifies
`format: "frog.document.draft"`; it does not claim `spec_version: "0.1"`.
The envelope, draft revision, semantic profile and application version are
different identities. A changed JSON key is not a profile migration.

Each new document contains its own icon, description, comments, controls,
indicators and wiring. The icon incorporates the canonical palette pictogram
of the principal operation. The displayed diagram is rendered from the actual
document, including its saved regions, rather than from a replacement picture.

## Finding and opening examples

Graiphic Studio's `Help → Find Examples…` opens the example browser. Its search
field uses the Search Palettes presentation rules. Categories, text search and
an object filter narrow the same index. An object's Examples command or a link
in Context Help opens this browser on that object. Detailed Help retains its
existing web destination and mechanism.

Selecting a result loads a read-only preview; it does not execute code. Zoom,
pan and case/frame selection affect the preview only. `Open in Studio` creates
a working copy with the declared relative dependencies, then opens it through
Studio's document manager. The installed example is not implicitly overwritten
by Save.

## Expectations and evidence

The index declares the behavior an example is intended to demonstrate. It is
not a certificate that every target passed. The runner loads the same file,
checks its types, saves/reopens it and compares actual observations with
independent expected values. Integer lexemes retain I64/U64 precision, arrays
retain dimensions, and clusters retain ordered stable field identities and
nominal schemas. Floating tolerances, signed zero, NaN and infinities are
explicit.

| Mode | Meaning |
| --- | --- |
| `execution` | The declared backend must execute and produce the expected typed outputs |
| `expected-diagnostic` | The named failure is expected; an unrelated refusal is a failure |
| `expected-abort` | A real infinite wait must register, stop and restart without retained waits |
| `persistence` | The document demonstrates authoring/codec behavior without claiming executable closure |
| `interactive` | The named UI interaction needs its own actual interaction evidence |

`native-fast` and `native-debug` use the selected native compilation and session
path. `simulation` identifies an explicit private reference path. A native
refusal never selects Simulation automatically, and a Simulation-only example
does not establish normal Studio Run support. Results distinguish `pass`,
`fail`, `skipped`, `unsupported` and `blocked`; only `pass` is a successful
proof of the particular tested mode.

For developer validation, the built `frog-example-runner` accepts `--index`,
`--family`, `--example`, `--backend` and `--report`. `--native-only` selects only
declared native runs, as used by the installed-package verifier. The same
family subsets are registered in CTest. Regression cadence and explicit pauses
remain host policy; this page does not grant permission to run a suspended
global gate.

## C library and dependencies

The [portable C sample](Native/lib/README.md) ships with source, fixed-width
header, reproducible CMake build and a relative runtime library dependency.
Its noncommutative operation makes ABI argument order observable independently
of visual row order. The I64 example exceeds 2^53 without a floating conversion.
Missing libraries, upstream errors and unsupported signatures are explicit
cases. Configuring a pointer, buffer or callback does not establish that the
selected NativeCall profile supports it.

Example dependencies are relative to the index root and are copied preserving
their relative paths. Installed execution uses the product's admitted LLVM and
providers, without requiring a developer SDK or the original build directory.
The package gate must fail if a promised native example or dependency is absent.

## Semantic decisions

FROG Add retains its public extensible 2..16 left reduction. Square is unary.
Integer Divide, Square Root and Reciprocal use real results in the admitted
numeric corridor. Authored Round To Nearest uses ties to even; a historically
explicit FENV profile remains a distinct versioned behavior. Max/Min likewise
keeps its historical explicit profile separate from the authored NaN-aware v2
contract.

The raw-byte String profile uses byte offsets and lengths, preserves NUL and
does not normalize or repair invalid UTF-8 produced by byte operations. Its
ASCII case conversion is not locale-sensitive Unicode case folding. Older
explicit UTF-8 String profiles remain separate. Flat Sequence exposes completed
frame results as they become available; Stacked Sequence publishes exterior
results after its final frame. Page visibility does not remove hidden frame
content from the document or its execution dependencies.

Backend support remains per operation, effective type, configuration and
target. A palette entry or a row in an audit is not validation of every
overload, hardware target, Matrix variant or external runtime.
