<a id="matrice-dacceptation-de-lexécutabilité"></a>

# Execution eligibility acceptance matrix

Revision 1 — 16 September 2026. **Qualification document, not conformance evidence.**

Source: supplied study “FROG / Graiphic Studio — Complete diagram and structure
validation,” revision 2 of 5 September 2026. Its 137 VAL identifiers are retained
for traceability. Results below are the study's initial expectations, not execution results.

The [EXEC rules](../Language/Execution%20eligibility.md) and published contracts of
the selected profile/target take precedence. A row proposing an extension does not
itself publish a corresponding format/runtime. For example, VAL-100 addresses
absent priority: a profile explicitly publishing priority must be tested against
that priority, not declared ambiguous by default.

Replacing “To qualify” requires evidence identifying test, commit, configuration/target
and result. Global Studio tests are summarized [separately](execution-eligibility-studio-coverage.md).

| ID | Situation | Study expectation | Qualification / evidence |
| --- | --- | --- | --- |
| VAL-001 | Unparseable JSON document. | Reject semantic loading; retain text access if possible. | To qualify — no individual evidence attached |
| VAL-002 | Duplicate node IDs in the same scope. | Blocking identity error. | To qualify — no individual evidence attached |
| VAL-003 | Same local ID in two regions with distinct qualified paths. | Valid if scoped references are correct. | To qualify — no individual evidence attached |
| VAL-004 | Wire to a deleted node. | Reference error; do not count the input as supplied. | To qualify — no individual evidence attached |
| VAL-005 | Wire to a port removed by an interface change. | Port or signature error. | To qualify — no individual evidence attached |
| VAL-006 | Two sources for one input. | Cardinality error. | To qualify — no individual evidence attached |
| VAL-007 | One source with multiple destinations. | Valid if every destination is compatible. | To qualify — no individual evidence attached |
| VAL-008 | Two wires cross without a shared binding. | Add no connection. | To qualify — no individual evidence attached |
| VAL-009 | Direct wire between regions without an explicit boundary. | Scope error. | To qualify — no individual evidence attached |
| VAL-010 | Incomplete wire fragment in an active region. | Saving possible; executable validation blocked. | To qualify — no individual evidence attached |
| VAL-011 | Valid ordinary output without a consumer. | No error merely because it is unused. | To qualify — no individual evidence attached |
| VAL-012 | Isolated node with an empty required input. | Error; do not implicitly erase it to permit Run. | To qualify — no individual evidence attached |
| VAL-013 | Missing Required input. | Blocking error. | To qualify — no individual evidence attached |
| VAL-014 | Missing Optional input with valid contractual default. | Valid; defined default value. | To qualify — no individual evidence attached |
| VAL-015 | Missing Recommended input but complete contract. | Valid, possibly a warning. | To qualify — no individual evidence attached |
| VAL-016 | Graphically present port absent from the effective contract. | Error; renderer must not invent a port. | To qualify — no individual evidence attached |
| VAL-017 | Connection with permitted numeric coercion. | Valid; appropriate diagnostic or coercion dot. | To qualify — no individual evidence attached |
| VAL-018 | Connection with an unauthorized conversion. | Type error. | To qualify — no individual evidence attached |
| VAL-019 | Generic type unresolved after validation. | Explicit blocker, no implicit f64 fallback. | To qualify — no individual evidence attached |
| VAL-020 | Incompatible fixed shapes where an operation requires equality. | Error before execution. | To qualify — no individual evidence attached |
| VAL-021 | Two dynamic sizes that may differ. | Validation possible if the node defines a runtime check. | To qualify — no individual evidence attached |
| VAL-022 | Wired producer that is itself invalid. | Consumer blocked by root cause, not declared ready. | To qualify — no individual evidence attached |
| VAL-023 | Boolean Case with valid selector and outputs in True and False. | Valid. | To qualify — no individual evidence attached |
| VAL-024 | Case without a selector. | MISSING_STRUCTURE_SELECTOR. | To qualify — no individual evidence attached |
| VAL-025 | Boolean Case missing its False branch. | MISSING_CASE_REGION. | To qualify — no individual evidence attached |
| VAL-026 | Boolean Case with two True branches. | DUPLICATE_CASE_MATCH or invalid cardinality. | To qualify — no individual evidence attached |
| VAL-027 | Text Case without a default branch. | MISSING_DEFAULT_REGION. | To qualify — no individual evidence attached |
| VAL-028 | Text Case with two default branches. | Ambiguous selection error. | To qualify — no individual evidence attached |
| VAL-029 | Text Case with duplicate matches. | DUPLICATE_CASE_MATCH. | To qualify — no individual evidence attached |
| VAL-030 | Output defined in True but absent from False. | UNBOUND_REGION_OUTPUT. | To qualify — no individual evidence attached |
| VAL-031 | Default branch present but not producing result. | UNBOUND_REGION_OUTPUT: selection default does not cover the output. | To qualify — no individual evidence attached |
| VAL-032 | Two outputs, only one covered in every branch. | Structure invalid for the remaining output. | To qualify — no individual evidence attached |
| VAL-033 | Correct displayed branch, incorrect hidden branch. | Structure and Run marked invalid. | To qualify — no individual evidence attached |
| VAL-034 | Incomplete output with no external consumer. | Error while that output remains declared. | To qualify — no individual evidence attached |
| VAL-035 | Case without data output and two valid branches. | Valid subject to selector and other contracts. | To qualify — no individual evidence attached |
| VAL-036 | Supplied boundary input unused in a branch. | No error merely because it is unused. | To qualify — no individual evidence attached |
| VAL-037 | Incomplete inner Case feeding an outer output. | Error propagates to the outer structure. | To qualify — no individual evidence attached |
| VAL-038 | Constant selector with another ordinary incomplete branch. | Error under the proposed strict policy; no implicit exclusion. | To qualify — no individual evidence attached |
| VAL-039 | Output absent from False, with known type and valid explicit default. | Valid; replacement value present in validated meaning. | To qualify — no individual evidence attached |
| VAL-040 | Default enabled but output type unresolved. | UNRESOLVED_TYPE; the flag alone is insufficient. | To qualify — no individual evidence attached |
| VAL-041 | Default value incompatible with output type. | INVALID_OUTPUT_DEFAULT. | To qualify — no individual evidence attached |
| VAL-042 | Default enabled with an already present incompatible branch wire. | Error; do not ignore the wire. | To qualify — no individual evidence attached |
| VAL-043 | Default enabled with two producers in the branch. | Cardinality error; no arbitrary choice. | To qualify — no individual evidence attached |
| VAL-044 | Default enabled with an incomplete binding fragment. | Incomplete-edit error, no silent replacement. | To qualify — no individual evidence attached |
| VAL-045 | Boolean default false. | Present and valid value, not an absent field. | To qualify — no individual evidence attached |
| VAL-046 | Numeric default zero. | Present and valid value, not absence. | To qualify — no individual evidence attached |
| VAL-047 | Empty-string default. | Present value if the type allows it. | To qualify — no individual evidence attached |
| VAL-048 | Fixed-size array with empty-array default. | Shape error unless the type is explicitly defined otherwise. | To qualify — no individual evidence attached |
| VAL-049 | Resource type with no constructible default. | Reject default; require a producer or explicit nullable type. | To qualify — no individual evidence attached |
| VAL-050 | Default mode removed with unwired branches. | Branches become immediately invalid. | To qualify — no individual evidence attached |
| VAL-051 | Branch added under strict policy. | New output cell uncovered; error until completed. | To qualify — no individual evidence attached |
| VAL-052 | Branch added with valid persistent policy. | Default applies contractually, visible and serialized. | To qualify — no individual evidence attached |
| VAL-053 | Default-policy round trip. | Same result, type and coverage after reopening. | To qualify — no individual evidence attached |
| VAL-054 | One-shot default generation, then branch addition. | Do not apply an undeclared persistent policy. | To qualify — no individual evidence attached |
| VAL-055 | Integer selector in a bool/string-only reader. | Explicit profile/type refusal; no hidden conversion. | To qualify — no individual evidence attached |
| VAL-056 | Integer selector with non-overlapping ranges and default. | Valid after extended-contract normalization. | To qualify — no individual evidence attached |
| VAL-057 | Overlapping case ranges without priority rules. | OVERLAPPING_CASE_MATCH. | To qualify — no individual evidence attached |
| VAL-058 | Case value outside the selector type's range. | Static error. | To qualify — no individual evidence attached |
| VAL-059 | Item added to exhaustive Enum without default. | Revalidate; insufficient coverage until a branch covers the new item. | To qualify — no individual evidence attached |
| VAL-060 | New Enum member with allowed default. | Selection coverage retained; default outputs still checked. | To qualify — no individual evidence attached |
| VAL-061 | Floating constant into integer Case through non-explicit rounding. | Reject if the contract does not define this adaptation. | To qualify — no individual evidence attached |
| VAL-062 | Basic For without count. | MISSING_LOOP_COUNT. | To qualify — no individual evidence attached |
| VAL-063 | For with valid count and empty body without output. | Valid. | To qualify — no individual evidence attached |
| VAL-064 | For with constant negative count. | INVALID_LOOP_COUNT before execution. | To qualify — no individual evidence attached |
| VAL-065 | For with dynamic count becoming negative. | Contractual runtime check; no invalid iterations launched. | To qualify — no individual evidence attached |
| VAL-066 | For may execute zero iterations; last_value without zero_iteration_value. | MISSING_ZERO_ITERATION_VALUE. | To qualify — no individual evidence attached |
| VAL-067 | For count = 0 with valid zero_iteration_value. | Output equals that value, not undefined memory. | To qualify — no individual evidence attached |
| VAL-068 | For count = 3 with complete body output and valid fallback. | Last iteration's output; fallback unused. | To qualify — no individual evidence attached |
| VAL-069 | Valid zero-iteration fallback but missing body output. | Body-coverage error. | To qualify — no individual evidence attached |
| VAL-070 | Incompatibly typed zero-iteration fallback. | Value or type error. | To qualify — no individual evidence attached |
| VAL-071 | For count = 0 with an invalid body node. | Error under strict policy; zero count does not disable the program. | To qualify — no individual evidence attached |
| VAL-072 | Two body regions in a basic For. | Structural error. | To qualify — no individual evidence attached |
| VAL-073 | Write to read-only index terminal. | Direction or contract error. | To qualify — no individual evidence attached |
| VAL-074 | Unused index. | Valid. | To qualify — no individual evidence attached |
| VAL-075 | Auto-indexed For without count under a permitting profile. | Count derived contractually; no false mandatory requirement. | To qualify — no individual evidence attached |
| VAL-076 | Conditional collection retains no element. | Typed empty output, not an undefined last value. | To qualify — no individual evidence attached |
| VAL-077 | Early exit with an unguaranteed declared fixed-size array. | Shape-contract error or dynamic type required. | To qualify — no individual evidence attached |
| VAL-078 | Post-test While without condition producer. | MISSING_LOOP_CONDITION. | To qualify — no individual evidence attached |
| VAL-079 | Basic While with constant false condition. | Exactly one normal iteration. | To qualify — no individual evidence attached |
| VAL-080 | Basic While with constant true condition. | Language-valid; nontermination warning possible. | To qualify — no individual evidence attached |
| VAL-081 | Condition from an incomplete inner Case. | While invalid by propagation. | To qualify — no individual evidence attached |
| VAL-082 | Stop if True presentation without published polarity inversion/normalization. | Semantic test must detect the divergence. | To qualify — no individual evidence attached |
| VAL-083 | Post-test While with defined output and no zero-iteration fallback. | Valid for this particular requirement. | To qualify — no individual evidence attached |
| VAL-084 | Future-profile pre-test While with output but no zero case. | Error if zero iterations are possible and undefined. | To qualify — no individual evidence attached |
| VAL-085 | Delay without initial field. | MISSING_STATE_INITIAL. | To qualify — no individual evidence attached |
| VAL-086 | Delay with incompatible initial type. | Type or value error. | To qualify — no individual evidence attached |
| VAL-087 | Correctly initialized Delay in valid recurrence. | Temporal cycle allowed subject to other rules. | To qualify — no individual evidence attached |
| VAL-088 | A → B → A cycle without memory. | INVALID_COMBINATIONAL_CYCLE. | To qualify — no individual evidence attached |
| VAL-089 | Component has Delay but A ↔ B subcycle has none. | INVALID_COMBINATIONAL_CYCLE despite memory elsewhere in the component. | To qualify — no individual evidence attached |
| VAL-090 | Non-temporal stateful node in a cycle. | Its classification alone does not legalize the cycle. | To qualify — no individual evidence attached |
| VAL-091 | Combinational cycle drawn inside a loop. | Still invalid; a border supplies no memory. | To qualify — no individual evidence attached |
| VAL-092 | Branch neither updates state nor defines hold/passthrough. | Incomplete-next-state error under the memory contract. | To qualify — no individual evidence attached |
| VAL-093 | Two independent instances of the same stateful sub-FROG. | Distinct states as contracted, no hidden alias. | To qualify — no individual evidence attached |
| VAL-094 | Explicitly disabled region with internal semantic error. | Does not block the active variant if the exclusion profile says so; error retained for inspection. | To qualify — no individual evidence attached |
| VAL-095 | Same region merely graphically hidden. | Its error remains blocking. | To qualify — no individual evidence attached |
| VAL-096 | Diagram Disable without active region under the proposed contract. | Invalid configuration. | To qualify — no individual evidence attached |
| VAL-097 | Diagram Disable with two active regions. | Ambiguous configuration. | To qualify — no individual evidence attached |
| VAL-098 | Active Disable region with uncovered output. | UNBOUND_REGION_OUTPUT. | To qualify — no individual evidence attached |
| VAL-099 | Conditional Disable with unknown required symbol. | UNKNOWN_COMPILE_SYMBOL. | To qualify — no individual evidence attached |
| VAL-100 | Several true compile conditions without defined priority. | AMBIGUOUS_COMPILE_SELECTION. | To qualify — no individual evidence attached |
| VAL-101 | Dependency only in an excluded variant. | Does not block active build; an all-variants audit may check it. | To qualify — no individual evidence attached |
| VAL-102 | Symbol change activates an incorrect branch. | Invalidation and a new blocking diagnostic. | To qualify — no individual evidence attached |
| VAL-103 | Event Structure with unwired optional timeout. | Valid if indefinite waiting is contractually defined. | To qualify — no individual evidence attached |
| VAL-104 | Timeout case missing a shared output. | Error or explicitly allowed default, like any normally terminating case. | To qualify — no individual evidence attached |
| VAL-105 | Reading a data field belonging to another event. | Scope/type error. | To qualify — no individual evidence attached |
| VAL-106 | Sequence uses data produced only in a future frame. | Dependency or scope error. | To qualify — no individual evidence attached |
| VAL-107 | Timed Loop with statically forbidden period. | Configuration error. | To qualify — no individual evidence attached |
| VAL-108 | Correct Timed Loop on a target lacking a required guarantee. | Capability block, without blaming a valid wire. | To qualify — no individual evidence attached |
| VAL-109 | In Place Structure with inconsistent boundary pair. | Boundary-contract error. | To qualify — no individual evidence attached |
| VAL-110 | Temporary reference escapes its permitted lifetime. | Scope/resource error. | To qualify — no individual evidence attached |
| VAL-111 | Parallel region with two unarbitrated producers of one output. | Error; no implicit merge. | To qualify — no individual evidence attached |
| VAL-112 | Expanded Add with valid a/b and six empty optional slots. | Valid; geometry does not define active arity. | To qualify — no individual evidence attached |
| VAL-113 | Add with valid a but missing b. | Missing required input. | To qualify — no individual evidence attached |
| VAL-114 | Add with connected but invalidly typed input_3. | Error; active operand is not ignored. | To qualify — no individual evidence attached |
| VAL-115 | Variadic Multiply in a reader with a binary contract. | Explicit refusal or documented migration; no omitted operand. | To qualify — no individual evidence attached |
| VAL-116 | Call whose active sub-FROG has an error. | BROKEN_DEPENDENCY with access to root cause. | To qualify — no individual evidence attached |
| VAL-117 | Invalid function file unreferenced by the launched entry. | Does not block that entry; may block a requested global build. | To qualify — no individual evidence attached |
| VAL-118 | Called interface changes after caller validation. | Transitive invalidation and port revalidation. | To qualify — no individual evidence attached |
| VAL-119 | Static call path not found. | MISSING_DEPENDENCY. | To qualify — no individual evidence attached |
| VAL-120 | Promised public output without producer or explicit policy. | Interface error even with no caller currently open. | To qualify — no individual evidence attached |
| VAL-121 | Deleted widget still referenced by the diagram. | Semantic reference error. | To qualify — no individual evidence attached |
| VAL-122 | Write to read-only UI property. | Contract error. | To qualify — no individual evidence attached |
| VAL-123 | Palette icon present, runtime provider absent. | No false runnable status. | To qualify — no individual evidence attached |
| VAL-124 | Added branch makes an output incomplete. | Run does not retain the old success. | To qualify — no individual evidence attached |
| VAL-125 | Undo that addition. | Coverage/state restored after checking the correct revision. | To qualify — no individual evidence attached |
| VAL-126 | Move a wire bend without changing binding. | No change to meaning or validity. | To qualify — no individual evidence attached |
| VAL-127 | Move a node across a structural boundary. | Semantic transaction and scope revalidation. | To qualify — no individual evidence attached |
| VAL-128 | Validation of R finishes after editing R+1. | Result marked stale; does not validate R+1. | To qualify — no individual evidence attached |
| VAL-129 | Target changed after successful validation. | Capabilities and variants revalidated. | To qualify — no individual evidence attached |
| VAL-130 | Artifact built for an old revision. | ARTIFACT_REVISION_MISMATCH; no silent launch. | To qualify — no individual evidence attached |
| VAL-131 | Valid program with unavailable runtime. | Launch block distinct from language validity. | To qualify — no individual evidence attached |
| VAL-132 | Precision warning without strict policy. | No automatic semantic block. | To qualify — no individual evidence attached |
| VAL-133 | Strict profile explicitly rejects risky coercion. | Consistent refusal in Studio, CLI and backend. | To qualify — no individual evidence attached |
| VAL-134 | Unbound output artificially injected into FIR. | Backend refuses; no undefined replacement value. | To qualify — no individual evidence attached |
| VAL-135 | Unknown structure without a registered validator. | Unsupported or explicit error; never valid by default. | To qualify — no individual evidence attached |
| VAL-136 | Execution cancelled before normal termination. | No fabricated success result; cancellation protocol. | To qualify — no individual evidence attached |
| VAL-137 | Round trip of an incomplete graph with wire_fragments. | Work preserved without becoming artificially executable. | To qualify — no individual evidence attached |
