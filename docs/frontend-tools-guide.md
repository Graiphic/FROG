# FROG validation and FIR: guide for external tools

**Contract publication: 1 November 2026. Documentation, JSON Schemas and synthetic
examples only. The common library and CLI are intended for a later open-source
code release; no executable implementation is delivered by this update.**

## One shared Frontend

The common Frontend owns `.frog` reading, explicit type/dependency resolution,
structural and semantic validation, FIR derivation, localized diagnostics, source
maps and provenance. Studio and the CLI use the same versioned semantic owner.
External consumers must not reproduce a smaller competing `.frog` to FIR generator.
The library is independent of Studio/Canvas, Windows GUI, LLVM and private Runtime
sessions. The command-line host supplies file I/O explicitly.

```text
source bytes + immutable dependencies + declared semantic context
  -> common Frontend: validate / derive
  -> complete public FIR
  -> selected bridge: full-graph target admission / lowering / compilation
  -> runtime services / host / package
```

Source format, FIR, Frontend software/API, semantic profiles and bridge wire protocol
have separate versions. The [operation contract](../sdk/ecosystem/docs/frontend-api.md)
and [result documents](../sdk/ecosystem/docs/frontend-result-contracts.md) identify
those versions and fields. Studio embeds a tested pin; a new public version does
not automatically replace that pin.

## Which question is answered?

| Gate | What it establishes |
| --- | --- |
| Read/integrity | The input bytes can be inspected safely. |
| Format/compatibility | Envelope, version and profiles are recognized. A migration is explicit. |
| Structure/semantics | Identities, types, bindings, ports, edges, active regions, state and effects satisfy their applicable contracts. |
| Target admission | The selected backend/providers can realize the whole admitted graph. |
| Build/run | Compilation, linkage, packaging and execution actually succeeded. |

A structurally valid JSON document is not automatically a semantically valid program.
A valid program may be unsupported by a target. A refusal to complete validation is
not evidence that the source is corrupt. Incomplete editable programs can still be
source documents; saveability and execution eligibility are separate. See
[execution eligibility](../Language/Execution%20eligibility.md) and
[the semantic gate](../Language/Semantic%20validation%20before%20FIR.md).

## Inputs, in plain terms

- **Source:** exact immutable UTF-8 bytes, source URI/technical ID and explicit
  format/version. An extension does not select a format. Source and FIR IDs are independent.
- **Semantic context:** declared inputs that can influence meaning, such as a
  profile's resolution bindings or, when supported, compile-time selection symbols.
  Empty means `{}`; the core inherits no OS, environment, PATH or Studio symbols.
- **Resolver:** a host service that supplies identified/versioned dependency bytes.
  A snapshot does not change during validation, even if the original file changes.
  The core has no implicit disk/network/registry/CWD search.
- **Limits and cancellation:** ceilings bound source/dependency/result sizes and
  work. The host can cancel, for example after an edit or closure. These are operation
  controls, not language-invalidity rules or runtime Stop.

## Results

`validate` returns an operation outcome, diagnostics, provenance, available source
correspondences and, if resolution changed the source, its complete detached
projection. It does not return FIR. `derive` uses the same semantic checks and returns
complete FIR only after success. `describe` reports versions, capabilities and limits
without validating a program.

An API call may succeed technically while its program is rejected or unsupported.
Diagnostics identify stage/category/stable code, severity and known source objects,
ports, wires or regions. Preserve those fields instead of matching message text.
The Frontend never applies a repair. An editor can propose a transaction and validate
its resulting source again before accepting it.

## Current bounded checkpoint

The local integration uses software `0.1.0-local`, API `3` and Source/FIR `0.1`.
API-3 provenance is `0.3`; diagnostic/capability documents remain `0.2` and source
maps `0.1`. Published schemas describe these shapes, not a universal capability set.

The public resolution profile currently binds kind-2 Type Definition fragments.
Kinds 1/3/4 and nonempty public ConditionalDisable contexts are not yet generally
supported. An empty context and exact kind-2 bindings have explicit examples.
The historical Studio `frog.document.draft` envelope requires Studio-owned capture
or migration; it is not canonical public Source merely because it also uses a
`.frog` filename. Keep [the two envelopes](studio-source-compatibility.md) distinct.

## Consuming the schemas

Start at [the schema/data index](../sdk/ecosystem/README.md). Schemas use JSON Schema
Draft 2020-12. Register their `$id` values and referenced schemas locally; an ID does
not promise a live HTTP download endpoint. Use a strict UTF-8 JSON reader rejecting
duplicate members and non-finite tokens and preserving exact integer tokens.
Shape validation cannot verify dependency hashes, resolve files, prove callback
lifetimes, detect all graph errors or admit a backend.

Examples are synthetic illustrations, not conformance or native execution receipts.
Future code publication requires its own version, provenance/licence review,
reconstruction instructions and qualification evidence. This publication does not
grant rights over private Studio or Runtime implementation.
