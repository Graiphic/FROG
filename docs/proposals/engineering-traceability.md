# Engineering traceability — associated-document proposal

**Status: non-normative proposal, 8 October 2026.** This is the proposed lot B
prepared alongside the lot A documentation update. It is **not** an adopted
profile, canonical source syntax, FIR format, implemented connector or conformance
claim. The working identifiers `ETR-PROP-*` and `ETR-TEST-*` are discussion labels,
not published FROG diagnostic codes. No example in this page records an executed test.

The [architecture overview](../../FROG-Architecture.md#engineering-lifecycle-and-digital-thread-integration)
explains the optional role of FROG in an engineering digital thread. This proposal
explores a small exchange surface linking requirements, source revisions and test
records. It adds no mandatory `.frog` field, primitive, migration or network service.
Programs without these links retain the same language obligations as before.

## Reading and ownership

| Existing owner | Boundary retained by this proposal |
| --- | --- |
| [Canonical source scope](../../Expression/Canonical%20source%20scope.md) and [Metadata](../../Expression/Metadata.md) | Source facts, descriptive metadata, external references and regenerable caches. `external_reference` is not an already adopted generic ALM link schema. |
| [Source compatibility and profiles](../../Expression/Source%20compatibility%20and%20profiles.md) | Explicit read/preserve/edit/validate/lower/execute claims and safe version handling. The private `frog.document.draft` format remains separate from public `spec_version` source. |
| [Execution eligibility](../../Language/Execution%20eligibility.md) | Loadability, structural validity, semantic acceptance, target buildability, current artifact and launch prerequisites. |
| [Source provenance](../../Expression/Source%20provenance.md) | Optional `ide.provenance` authoring/import/generation/review attestations; not a universal test-result store. |
| [Identity and Mapping](../../IR/Identity%20and%20Mapping.md) | Required source/FIR attribution, recoverability and scoped identity, independent of optional external links. |
| [Lowering](../../IR/Lowering.md) and [Backend contract](../../IR/Backend%20contract.md) | Faithful downstream realization, accepted subsets, dependencies and explicit consumer assumptions. |
| [Profiles](../../Profiles/Readme.md) and [Interop](../../Profiles/Interop.md) | Optional capabilities with defined owners. This proposal adds no HTTP, industrial protocol, ALM or foreign-call primitive. |
| [Execution diagnostics](../../IDE/Execution%20diagnostics.md) | Source and target diagnostics remain distinct from organizational release decisions. |
| [Versioning](../../Versioning/Readme.md) and [Governance](../../GOVERNANCE.md) | Corpus/source/program versions and steward-led adoption remain unchanged. |

A future adopted optional profile could own an associated traceability format and
its support claims. If a later design embeds any new fact inside `.frog`, its
syntax and compatibility require a separate `Expression/` decision. This document
does not reserve an official extension, namespace, profile name or version number.

## Three separate concerns

1. **Program and execution:** source, meaning, validated FIR, lowering, backend
   contracts and realization determine accepted computation.
2. **Traceability and evidence:** qualified identities and revisions connect
   requirements, source, transformations, artifacts, runs and observations.
3. **Policy and authorization:** an organization evaluates records under an
   identified policy revision and may approve or reject a release or deployment.

A policy can refuse a release without declaring a well-typed program invalid.
Conversely, an approval cannot make invalid source semantically acceptable.
An engineering relationship is not a graph wire, and a requirement text does not
execute. If a threshold, period or deadline policy affects behavior, the applicable
source/profile/execution contract must represent it explicitly.

The external system remains authoritative for its requirement and baseline.
FROG remains a language; this surface is neither an ALM/PLM database nor a digital
twin. No specific vendor, cloud, transport, authentication system or AI model is
required. An offline program can remain valid and executable without resolving an
ALM link, subject to its actual executable capabilities and deployment policy.

## Placement: an associated versioned document

The first proposed slice uses a document **outside `.frog`**, for example
`engineering-links.example.json`. It may live in an engineering or test dossier;
it need not be adjacent to every program. An adapter may import an offline export
or resolve authorized remote references. An external registry is optional, not
the sole mandatory storage location.

Embedding links in source would improve single-file transport but would also
require source-schema ownership, privacy and compatibility decisions. A registry-only
design risks losing exportability and disconnected access. Associated documents
avoid a source-format change while making revision and missing-link handling explicit.

The document is not FIR, a `.wfrog` widget package or executable source. Reviewed
links and historical measurements are not safely regenerable caches; `cache`
cannot be their only copy. FIR retains the mapping its own contract requires,
without absorbing an external lifecycle database. Large traces remain separate
referenced artifacts.

## Qualified identity, revision and correspondence

An artifact reference would distinguish these concepts:

| Information | Proposed purpose |
| --- | --- |
| Artifact kind | Requirement, source, source object, FIR, build artifact, test, run or result. |
| Authority and optional project/container | Disambiguate identical IDs in different engineering systems. |
| Logical ID | Stable identity within that authority, separate from its display label. |
| Revision or baseline | Identify the intended state, without silently substituting the latest version. |
| Optional locator | Locate content; a mutable URL is not by itself an immutable identity. |
| Optional digest | Bind content under a named algorithm and explicit scope/canonicalization rule. |
| Optional source-object selector | Qualify program, revision, source scope and local object ID; never infer identity from geometry or a similar label. |

Existing local IDs and scopes remain valid; this is not a migration to globally
unique node UUIDs. Copying between programs creates a different context and does
not automatically transfer approvals. Renaming, deletion, relocation and import
need explicit correspondence or unresolved status, not heuristic rebinding to the
first label match.

Three revisions are distinct: saved document content; semantic/build configuration
including entry point, active dependencies, profiles and options; and engineering
baseline including requirements, tests and policy. Moving a node visually may
change document bytes without changing calculation. A requirement, oracle, native
dependency or build-option change can invalidate evidence applicability while
source text remains unchanged.

This proposal defines **no semantic hashing algorithm**. Exact file-byte digests
and immutable repository references can be useful when their scope is declared;
they do not prove semantic equivalence. Any future digest design must reconcile
with the existing source-provenance contract rather than redefine its attestations.

Source-to-FIR transformations can map one-to-many or many-to-one. Inlining,
normalization, fusion and elimination preserve the attribution required by
`IR/Identity and Mapping`; they do not promise a separately observable runtime
value for every source object. Observations identify the artifact/instance actually
running, even when an IDE displays another revision. Undo may restore source;
it does not automatically restore an approval whose dependencies or policy changed.

## Proposed relationships and their limits

| Working relation | Direction | Meaning | Does not establish |
| --- | --- | --- | --- |
| `implements` | Program/source object → requirement | Declared implementation intent. | Correct behavior or satisfaction. |
| `verifies` | Test case → requirement | Declared evaluation purpose. | Coverage sufficiency or a successful run. |
| `derived_from` | Derived artifact → origin | Claimed transformation correspondence. | Semantic preservation without validation. |
| `execution_of` | Instance/run → executable artifact | Attribution of execution. | Satisfaction of environment assumptions. |
| `result_of` | Result → test run | Attribution of observations. | Approval for release. |
| `review_of` | Review → versioned subject | Scope of a review. | Satisfaction of every product requirement. |

These are proposed vocabulary terms, not `.frog` fields. Unknown relations are
not interpreted by resemblance. A future satisfaction conclusion should identify
an evaluator, method, scope, evidence and policy, with outcomes such as satisfied,
not satisfied or indeterminate. A lone `satisfies` edge is not sufficient proof.

## Evidence and independent states

An inspectable test record would identify:

- the subject source revision, entry point and requirements baseline;
- the toolchain, profiles, options, selected dependencies, build manifest and artifacts;
- the actual run/instance, launched artifact digest, target and host configuration;
- test-case revision, stimulus/data revision, oracle and acceptance criteria;
- observations, units, errors, result and coverage limits;
- relevant time, duration and clock source, without assuming synchronized clocks;
- integrity references, producer and any separately attributed review/decision.

References may point to separate pieces. A log or measurement can support a dossier
without constituting a formal proof or regulatory qualification. Debug instrumentation
may affect timing; optimized and debug runs can expose different observations.
Missing probe data is not zero and is not a favorable result.

| Dimension | Illustrative values, not adopted FROG statuses |
| --- | --- |
| Format reading | `recognized`, `unsupported`, `malformed` |
| Reference resolution | `resolved`, `unresolved`, `access_denied` |
| Freshness/applicability | `current`, `stale`, `unknown` |
| Test result | `passed`, `failed`, `inconclusive`, `not_run` |
| Integrity | `checked`, `invalid`, `unchecked` |
| Issuer trust | `trusted`, `untrusted`, `unknown` |
| Policy decision | `approved`, `rejected`, `pending`, `not_evaluated` |

A stale report remains a historical record of its original subject. It is not
rewritten to fit the latest source or baseline. Language validity, toolchain
conformance, application test success, requirement satisfaction and deployment
authorization remain separate claims. A valid signature identifies an issuer and
covered content; it establishes neither functional correctness nor approval
rights. Missing authoring provenance means unknown/unattested, not automatically AI-generated.

## Conceptual associated-document example

**Example only.** The keys below are not an adopted schema. IDs and revisions are
fictitious, not release-grade evidence. This JSON is neither `.frog` nor FIR and
records **no test execution, success or approval**.

```json
{
  "proposal_format": "engineering-traceability-draft-example",
  "example_only": true,
  "artifacts": [
    {
      "key": "requirement",
      "kind": "requirement",
      "authority": "example-engineering",
      "container": "demo-project",
      "id": "REQ-ADD-001",
      "revision": "baseline-demo-1"
    },
    {
      "key": "program",
      "kind": "frog_source",
      "authority": "example-source-repository",
      "id": "addition.frog",
      "revision": "source-demo-1"
    },
    {
      "key": "test_case",
      "kind": "test_case",
      "authority": "example-test-suite",
      "id": "TEST-ADD-001",
      "revision": "test-demo-1"
    }
  ],
  "links": [
    {
      "id": "link-implementation",
      "from": "program",
      "relation": "implements",
      "to": "requirement",
      "asserted_by": "example-engineering-team"
    },
    {
      "id": "link-verification",
      "from": "test_case",
      "relation": "verifies",
      "to": "requirement",
      "asserted_by": "example-test-team"
    }
  ]
}
```

Adding a result would require an actual identified run and evaluated artifact,
not a fabricated `passed` field. Parsing this illustrative JSON establishes only
JSON syntax, not traceability conformance.

## Proposed contract rules for later review

All following rules are proposed obligations of a **future adopted format**,
not added requirements on current FROG readers or programs.

| ID | Proposed rule |
| --- | --- |
| ETR-PROP-01 | Recognize the declared document version or report unsupported support explicitly. |
| ETR-PROP-02 | Qualify local and external references without identity ambiguity. |
| ETR-PROP-03 | Never substitute another revision when resolving a locator. |
| ETR-PROP-04 | Define relationship direction and meaning; do not guess unknown vocabulary. |
| ETR-PROP-05 | Declared implementation intent does not establish requirement satisfaction. |
| ETR-PROP-06 | Attribute results to their run and evaluated artifact, not the current IDE document alone. |
| ETR-PROP-07 | Re-evaluate applicability when covered content changes, preserving historical evidence. |
| ETR-PROP-08 | Distinguish unresolved, inaccessible, nonexistent and malformed references as the adopted contract specifies. |
| ETR-PROP-09 | Rejecting a traceability document does not automatically invalidate the referenced program. |
| ETR-PROP-10 | A tool claiming preservation retains unknown extensions when safe, or explicitly reports loss/refuses destructive writing. |
| ETR-PROP-11 | Never silently rewrite historical results to correspond to newer source. |
| ETR-PROP-12 | Evaluate integrity, issuer trust and approval separately. |
| ETR-PROP-13 | Reading/validating links executes no program, device command or deployment. |
| ETR-PROP-14 | Remote resolution is separately authorized, bounded and disableable. |
| ETR-PROP-15 | Support claims name read, preserve, edit, resolve, validate, evidence-production and synchronization capabilities separately. |

## Proposed conformance cases

**Definitions only: none of these cases was executed for this proposal.** Lot A
prepares the expectations; later adopted lot B would supply executable fixtures,
validation rules and results. A passing JSON parser is not their implementation.

| ID | Case | Proposed expected result / boundary |
| --- | --- | --- |
| ETR-TEST-01 | Program without a traceability document. | No new core obligation; apply only selected profiles/policies. |
| ETR-TEST-02 | Add/change associated links with fixed program and configuration. | Same computation/effects within the evaluated scope; engineering review state may change. |
| ETR-TEST-03 | Exact reference to an available source object and revision. | Deterministic resolution of that subject. |
| ETR-TEST-04 | Same local node ID in two programs. | Qualified identity prevents collision. |
| ETR-TEST-05 | Referenced object is missing. | Unresolved/invalid link under its contract; no similar-label substitution. |
| ETR-TEST-06 | Requirement available only at another revision. | No silent revision replacement. |
| ETR-TEST-07 | Requirements server unavailable. | Resolution unavailable or known offline state; no automatic semantic error. |
| ETR-TEST-08 | Access denied. | Distinct from absence or malformed reference. |
| ETR-TEST-09 | `implements` link without a test. | No automatic satisfied conclusion. |
| ETR-TEST-10 | Linked test never run. | `not_run` or equivalent, never `passed`. |
| ETR-TEST-11 | Successful test on the exact configuration. | Result applies only to that identified run and scope. |
| ETR-TEST-12 | Failed test with valid source. | Negative test outcome, not fabricated type invalidity. |
| ETR-TEST-13 | Interrupted/timed-out run or missing data. | Failure/inconclusive under its contract, never default success. |
| ETR-TEST-14 | Semantic source change after test. | Preserve old evidence; re-evaluate applicability. |
| ETR-TEST-15 | Presentation-only change. | Identify documentary change without assuming calculation changed. |
| ETR-TEST-16 | Requirement baseline or oracle changes, source unchanged. | Do not transfer old conclusion automatically. |
| ETR-TEST-17 | Native dependency or build option changes. | Reassess build/run evidence applicability. |
| ETR-TEST-18 | Observe old instance while editing newer source. | Attribute observations to the old artifact/instance. |
| ETR-TEST-19 | Optimization fuses source objects. | Keep required correspondence without inventing a bijection. |
| ETR-TEST-20 | Value unobservable in optimized mode. | Explicit limitation, no fabricated value. |
| ETR-TEST-21 | Altered attestation or incorrect digest. | Invalid integrity; retain history according to policy. |
| ETR-TEST-22 | Valid signature from an unauthorized approver. | Distinguish integrity, trust and approval. |
| ETR-TEST-23 | No authoring provenance. | Unknown/unattested origin, not automatic AI classification. |
| ETR-TEST-24 | Unknown relationship or format version. | Safe preservation or explicit limitation, no guessed interpretation. |
| ETR-TEST-25 | Copy graph to another program. | Reassess identities/links; approvals are not inherited automatically. |
| ETR-TEST-26 | Delete or move object outside its scope. | Reassess link; do not rebind by graphical position. |
| ETR-TEST-27 | Import/export through two tools. | Preserve relationship meaning or report loss. |
| ETR-TEST-28 | Malformed external evidence document. | Diagnose that document without corrupting `.frog`. |
| ETR-TEST-29 | Release policy requires absent evidence. | Explicit release refusal, not invented source error. |
| ETR-TEST-30 | Dangerous locator in a reference. | No unauthorized request or code execution. |
| ETR-TEST-31 | Bridge lacks a required primitive. | Reject at the correct gate, no implicit semantic fallback. |
| ETR-TEST-32 | Target supported but timing guarantee unproven. | Successful build implies no real-time claim. |
| ETR-TEST-33 | Remote requirement-status update. | Separate write authorization from reading/exporting results. |
| ETR-TEST-34 | Private stimuli or traces. | Minimal documented export, no default public disclosure. |

Non-interference is evaluated against the property claimed, not an unconditional
byte-identical-binary requirement: timestamps, debug symbols or packaging may
change bytes without changing behavior. Identity preservation and evidence
freshness are independent checks, not name-based repair heuristics.

## First example and deferred examples

The proposed first exercise uses the public
[01_pure_addition dossier](../../Examples/01_pure_addition/Readme.md) under the
[example dossier standard](../../Examples/example_dossier_standard.md). Its typed
source and documented pipeline need to be read and run before claiming a result.
A synthetic requirement could ask that the chosen inputs `2` and `3` produce `5`
in the example's actual declared type/domain. This tests that case, not every
addition or overflow rule.

The future dossier would link a pinned requirement baseline, source revision,
validation/FIR/lowering/backend artifacts, actual executable/configuration, test
revision, input data, oracle and observed run. It would then change source or
oracle and separately the requirements baseline, retaining the old report but
rejecting its automatic applicability. No existing `.frog` or evidence file is
modified by this proposal, and no run result is supplied here.

A sensor/test-bench scenario remains conceptual until acquisition and output
capabilities are specified and supported. A timing example is deferred until
start/end events, clocks, units, instrumentation, workload, errors and a statistical
or worst-case criterion are defined. A favorable mean is not a maximum bound;
a result on one configuration does not cover every architectural target class.

## Industrial bridges and support claims

An industrial bridge describes a domain of downstream integration. Runtime,
compilation and hybrid are realization strategies, not competing language laws.
Data exchange, calling an external capability and realizing a whole FROG program
are separate support claims. A bridge states versions, accepted profiles/subsets,
primitive and structure semantics, types/conversions/overflow, causality, state
lifetime/reset, events, timing/memory assumptions, errors, external calls/ownership,
host dependencies, mapping and observation limits, with explicit rejection cases.

The first proposal defines no new universal time, event or distribution model.
No PLC/FPGA/MCU, protocol, deterministic deadline or safety support follows from
source portability, an LLVM build or this prose. A future standards/vendor study
would need semantic preservation and target tests. Existing Interop capabilities
remain unchanged; remote ALM access by an engineering tool is not automatically
a standardized network primitive inside a FROG graph.

The separate [IEC 61499 evaluation note](iec-61499-bridge.md) identifies one
informative industrial direction and the questions preceding a target experiment.
It does not implement the bridge or adopt this traceability format.

## Security, privacy and assisted changes

Requirements, comments and imported files are untrusted work content, not
instructions granting an agent repository, network or device authority. A source
change follows an identified baseline, reviewable diff, structural and semantic
validation, derivation, build and tests before a separately authorized deployment.
FIR may support analysis; editing it is not a bypass around rejected source.

Format validation is local and has no external effects. Optional remote resolution
needs explicit access policy, size/time/redirect limits and an offline mode.
Reading a requirement does not authorize changing its status. Credentials remain
with the integration, never in source, FIR or public examples. Observations may
open a change request; they do not automatically rewrite or redeploy a live graph.

Identifiers, local paths, locators, source, stimuli and traces may be sensitive.
Export only what the audience may receive; opaque IDs/revisions can replace
private content where appropriate. Hashing alone is not anonymization. Open
specification does not require public user artifacts. Integrity, trust, safety,
regulatory qualification and release approval need their own evidence and policy.

## Decisions required before adoption

The recommended first slice is an inspectable local associated document linking
one requirement, one program and one test. The following remain **open**:

- official name and contract version, adoption owner and publication decision;
- exact grammar, required/optional fields, duplicate handling, cardinalities and reference constraints;
- revision/baseline conventions, source-scope selectors and locator forms;
- minimal relationship vocabulary and precise directions, without premature risk/safety ontologies;
- evidence-run schema, result and resolution states, transitions and applicability rules;
- digest scope/canonicalization, integrity and reconciliation with source provenance;
- unknown-extension preservation, loss/refusal behavior and safe resource limits;
- separate reader/editor/resolver/validator/evidence/synchronization capability claims;
- schemas, valid and invalid examples, bounded validator and reproducible conformance evidence;
- later embedded-source references, if justified, through a separate `Expression/` decision.

A possible later adopted owner is a dedicated optional profile under `Profiles/`,
with conformance cases and examples following existing conventions. No such file
or normative schema is created here. The corpus posture, `.frog spec_version`
and `metadata.program_version` are not overloaded to version associated links.

## Change classification and validation boundary

This draft and the linked lot A pages are a documentation clarification plus a
non-normative proposal against public commit
`b890993ae999a6f2916657ba4eeb4d9a08515934`. Normative source/FIR schemas,
primitive/structure contracts, example sources, reference implementations, license,
CLA and governance are unchanged. No source migration or private implementation
publication is part of this lot.

Document checks can establish local links, preserved anchors, syntax and navigation.
They cannot establish a completed traceability profile, an executed ETR test,
reference-runtime qualification or an industrial integration. Adoption and
implementation remain separate work subject to the existing governance process.
