# Synthetic Frontend data examples

These files illustrate the contracts. They are not complete executable programs,
conformance receipts, native runs or evidence that code is distributed. Repeated
`a`/`b` hashes and `example.*` identities are deliberately synthetic. The type-reference
hash alone is computed over the accompanying type-definition example.

| Example | Schema |
| --- | --- |
| `semantic-context-empty.json`, `semantic-context-resolution.json` | `../schemas/frontend-semantic-context.schema.json` |
| `dependencies.json` | `../schemas/frontend-dependencies.schema.json` |
| `type-reference.json` | `../schemas/frontend-type-reference.schema.json` |
| `types.json` | `../schemas/frontend-type-definitions.schema.json` |
| `diagnostics.json`, `cancelled-diagnostics.json` | Apply `../schemas/diagnostic-0.2.schema.json` to **each array item** |
| `provenance-api3.json` | `../schemas/frontend-context-0.3.schema.json` |
| `capabilities.json` | `../schemas/frontend-capabilities-0.2.schema.json` |
| `source-maps.json` | `../schemas/frontend-source-maps.schema.json` |

The resolution example targets an eligible type-schema location in its profile.
It is not a complete `.frog` fixture. `types.json` supplies one `i64` fragment;
complete nominal/TypeDef semantics still require the applicable full schema and
semantic contract. The CLI lock is host-side file selection, not a core request
to read files. Resolution and graph validity need checks beyond schema validation.
