# FROG Typed Binding Contract v1

Status: accepted architecture contract, 2026-09-05. Authority: the FROG project.
Profile ID: `frog.typed-binding@1`.

Case authoring extensions (exact/range matchers, defaults, drafts and editing
transactions) are specified in [Case Editing Profile v1](Case%20Editing%20Profile%20v1.md).

This additive profile extends [Type.md](Type.md) and [Control structures.md](Control%20structures.md).
The historical base `spec_version: 0.1` remains readable. A Case using the extension declares
`"binding_profile": "frog.typed-binding@1"`. This profile is not a claim that the Studio
draft envelope has already been migrated to the public source envelope, nor that a target
implements every recognized type.

## 1. Ownership

| Artifact | Authority |
|---|---|
| Versioned function library manifest | Stable port IDs, directions, TypeExpr, cardinality, requirement, connection policy and resolver ID |
| Widget class contract | Primary-value presence, supported roles and role-derived direction |
| Widget instance in .frog | Concrete value type, enum domain, array element schema/rank, ordered cluster field schema |
| Versioned structure manifest | Intrinsic terminal faces/scopes, shared type variables, dynamic boundary rules, resolver and lowering IDs |
| .wfrog realization | Visual resources, geometry and presentation defaults; cannot redefine wire compatibility |
| .frog graph | Instances, definition references, regions, boundary IDs and connections by node + port |
| Runtime capability registry | Availability of a lowering/provider, separately from type validity |

An edge MUST NOT own an independent type. All its consumers use the resolved source/target
contract. Display colour, current zoom, widget size and visible Case region are never typing inputs.

## 2. Concrete TypeRef

Internally TypeRef is a recursive value object, not a string prefix convention.
Canonical text is only a persistence/interchange boundary:

```text
bool
i32
f64
string
path
array<f64>
array<f64,rank=2>
array<u8,256>
enum<"application.mode",u16>
cluster<"temperature":f64,"valid":bool>
cluster<"samples":array<f64,rank=2>,"mode":enum<"application.mode",u16>>
```

Rules:

- Rank is part of type identity. Current array dimensions belong to the value, unless an
  extent is explicitly contractual. Rank 1 serializes as `array<T>`; `array<T,N>` retains
  the base fixed-length rank-1 contract. Rank is bounded to 1..16 in this profile.
- `array<array<T>>` is an array of array values, NOT an alias for a rectangular rank-2 array.
  Implementations must not silently flatten the distinction.
- Enum identity is the pair (domain ID, integer representation). Labels are presentation,
  item IDs belong to the domain, and two different domains do not implicitly connect.
  Copy/conversion of a value preserves its domain. A new independent enum has its own domain.
- Ring is a numeric presentation, not a separate transported type: `ring:u16` imports as `u16`.
- Cluster identity is structural: the ordered sequence of stable field IDs and their types.
  This v1 decision does not introduce an additional nominal schema ID. Renaming a displayed
  label or moving a field on screen does not change type; changing its ID, logical order or
  concrete type does. Field IDs are unique/nonempty; field conversion is explicit.
- Picture widgets currently project to `array<u32,rank=2>`. A distinct image-format type is
  deferred; it must not be fabricated from the icon or widget class name.
- `unknown` is an editor resolution state, NOT a universal type or an implicit `f64`.
  It may be retained in an editable draft but is not buildable as an unresolved binding.

Base primitives remain defined by Type.md. Studio's additional numeric spellings
(`i4/u4/i128/u128/biguint`, `f8e4m3/f8e5m2/f16/bf16/f80/f128`,
`c32/c64/c80/c128/c256/fxp/cfx/decimal`) are recognized identities, not a promise of
runtime arithmetic support. `path`, `timestamp` and `variant` likewise require target
capabilities beyond the minimal base profile.

Identity strings use double quotes, with only quote and backslash escaping. Canonical
writers omit unnecessary whitespace. The current parser limits nesting to 32 and text to
64 KiB; malformed syntax is an error, distinguishable from a well-formed unresolved type.

## 3. TypeExpr and resolution

TypeExpr describes a port constraint, never a value. The initial portable grammar supports:

```text
<concrete TypeRef>
any
T:any
T:numeric
T:integer
array<T:numeric,rank>=1>
array<T:any,rank>=1>
same(input)
case_selector
```

`same(input)` requests the resolved type of another port; it does not default to a numeric
type when that port is unresolved. The Case selector constraint accepts bool, integer,
string or Enum; it rejects floating point, arrays and clusters.

Existing function expressions such as `promote_numeric(a,b)`, unions and versioned custom
resolver calls remain registered function-family policies during migration. They MUST NOT
be interpreted as concrete types. New expression forms require a grammar, resolver and tests.
The initial TypeExpr parser does not yet implement a general-purpose polymorphic language.

Resolution distinguishes four outcomes: compatible, numeric coercion required, unresolved,
and incompatible. A diagnostic includes a stable code and the offending binding. Numeric
implicit connections preserve the declared target type; aggregate rank/contractual extents
must match. Cluster fields and enum domains use exact identity. Execution conversion rules
remain separately enforced by the target/provider.

## 4. Structure contract and Case equivalence classes

The first manifest is [case.bindings.v1.json](../Libraries/Structures/case.bindings.v1.json).
Studio consumes a checked-in projection, generating its portable registry entry at build time.
The projection MUST match the public manifest byte-for-byte.

- `selector` is an exterior input; `selected_case` is its read-only interior output in
  every region. Both faces share one type variable S, inferred from the selector source.
- Each dynamic boundary ID introduces a separate type variable T.
- An input tunnel takes its type from its one exterior source.
- An output tunnel unifies sources from ALL regions, including hidden regions and Default.
  Each region can supply at most one source. All supplied types must be exactly equal;
  no hidden promotion across branches is allowed.
- A known type and complete wiring are independent facts. A missing region keeps the tunnel
  incomplete, even when another region has established T. Default-if-unwired may cover absent
  regions; it cannot hide a conflicting type, duplicate source or unresolved source.
- Persisted inferred types are caches, never evidence after rewiring. A disconnected output
  cannot retain an authoritative old type merely because its saved boundary says `bool`.
- Region ownership is semantic, not inferred from geometry. Cross-boundary edges must obey
  the existing graph-scope rules. A source in a deleted region is an error.
- A cycle with no typed evidence stays unresolved. This profile does not itself legalize
  dataflow cycles or supply runtime scheduling.

Case matches use `boolean_exact` with a Boolean `value`, `integer_exact` with a
lexical integer string `value` (large integers must not become JSON floating point),
`string_exact` with a string `value`, or `enum_item` with a stable string `item` ID.
An enum match names the stable item ID. Matching policy/default coverage validation remains
part of Case semantics, separate from tunnel type/completeness validation.

A manifest can combine existing supported structural policies. New execution semantics still
require an implemented, versioned lowering; declaring its ID alone never makes it runnable.

## 5. Compatibility and rollout

Legacy `array:`, `frog.array<...>`, `ring:`, `enum:` and `cluster:` spellings are import
adapters, not competing authorities. Widget metadata supplies missing enum/cluster identities;
a bare legacy spelling cannot prove an identity it does not contain.

The first Studio delivery routes widget/Case bindings, wire checks to those sinks, Case
completeness, readiness diagnostics, Case cache serialization and type colours through
the portable resolver. Enum constants persist domain IDs across widget conversion and reload.
Other structure families and historical visual node identifiers remain in their existing
adapters. Legacy widget envelope fields are not globally rewritten in this first delivery.

Next acceptance gates, in order:

1. Generate all function signatures from versioned manifests and validate the full TypeExpr grammar.
2. Declare widget-class primary-value contracts and migrate the remaining structure families.
3. Have all remaining help, serialization and Runtime lowering consumers use resolved TypeRef.
4. Migrate the Studio draft envelope to the canonical .frog format, with golden-file compatibility tests.

These gates are intentionally tracked as unfinished; the existence of this contract does not
constitute their implementation.
