# Contract changes

## P28 API-2 result documents — local, 2026-11-01

Adds explicit 0.2 diagnostic, provenance/context and capabilities schemas, plus
the 0.1 source-maps envelope referencing the existing FIR component schemas.
The 0.1 intake/negotiation schemas and synthetic API-1 fixtures are preserved.
New synthetic shape fixtures and contract tests do not claim derivation or
execution. `tests/validate_frontend_results.py` checks explicit actual output
files with local schemas, without invoking another producer or fetching schemas.

API 2, result-document versions, Source/FIR formats, software provenance and
semantic profiles remain separate version axes. Production source extraction
does not grant redistribution rights to private implementation material.

## 0.1 — initial local baseline, 2026-10-29

Adds distribution intake schemas and two consumers sharing one validator,
separate Source/FIR/Frontend/API/bridge negotiation, source-attributed diagnostics,
evidence records, and C API 1 declarations for the future common Frontend.

This is an experimental baseline, not a released Frontend. It does not assert
compatibility with the existing limited Python derivation prototypes or rename
private Studio FIR profiles into public ones. P28 must document each semantic
migration and replace production generation with the same extracted implementation
used by the CLI, preserving authored documents and source attribution.

No existing .frog, .frprj, .gsproj, .ftype or .wfrog file is migrated by these tools.
No installed Studio component or Windows association is changed.

Before the first baseline was frozen, review separated dependency
consumption_phases, installation_scope and required_location. The transient
scopes/location fields were never released. Rights develop/update belong to
required_rights, not dependency phases.

Future incompatible changes require a new contract version, explicit rejection
or migration, and conformance fixtures. Software release changes alone do not
change Source or FIR syntax.

