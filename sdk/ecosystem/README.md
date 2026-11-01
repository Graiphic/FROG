# Common FROG Frontend: public interoperability contracts

**1 November 2026 — documentation, JSON Schemas and synthetic JSON examples.**
The common Frontend library and CLI are intended to be open source. Their code and
binaries are deferred to a later release after integration, qualification and
provenance/licence review. This update includes neither implementation nor private
Runtime code. The existing [C++ Runtime contract SDK](../cpp/docs/runtime-contract.md)
is a separate surface, not the Frontend C ABI or a universal bridge ABI.

- [External-tool guide](../../docs/frontend-tools-guide.md): validation versus target admission.
- [Operation contract](docs/frontend-api.md): inputs, resolver, outcomes, limits and cancellation.
- [Result documents](docs/frontend-result-contracts.md): API-3 document versions and source attribution.
- [Synthetic examples](fixtures/README.md): examples and explicit schema mapping.

| Surface | Exact schema |
| --- | --- |
| Bounded semantic context | [frontend-semantic-context.schema.json](schemas/frontend-semantic-context.schema.json) |
| Explicit CLI dependency lock, version 1 | [frontend-dependencies.schema.json](schemas/frontend-dependencies.schema.json) |
| Authored type-fragment reference | [frontend-type-reference.schema.json](schemas/frontend-type-reference.schema.json) |
| Type-definition dependency envelope | [frontend-type-definitions.schema.json](schemas/frontend-type-definitions.schema.json) |
| One diagnostic, version 0.2 | [diagnostic-0.2.schema.json](schemas/diagnostic-0.2.schema.json) |
| API-3 provenance, version 0.3 | [frontend-context-0.3.schema.json](schemas/frontend-context-0.3.schema.json) |
| Capability descriptor, version 0.2 | [frontend-capabilities-0.2.schema.json](schemas/frontend-capabilities-0.2.schema.json) |
| Source correspondences, version 0.1 | [frontend-source-maps.schema.json](schemas/frontend-source-maps.schema.json) |

Schemas describe data shapes. Completed semantic acceptance, operation completion,
backend admission and successful package execution are separate facts. Versions of
Source/FIR, profiles, Frontend software/API and bridge protocol are independent.
A schema/fixture publication is not executable capability evidence.
