# FROG Case Editing Profile v1

Status: accepted authoring/semantic contract, 2026-09-05. Authority: the FROG project.
Profile ID: `frog.case-editing@1`, additive to `frog.typed-binding@1`.

This document standardizes the Case editing extensions implemented in Studio.
It does not declare a runtime backend capable merely because it can read these records.
Conditional Disable is a different structure and is unaffected.

## Identity and dispatch

Region `id`, `title`, `matches`, `default`, `draft_text` and visible-region ID have
distinct roles. IDs are stable scope identities. A title describes the treatment;
it is not a selector value. Navigation and numeric radix are authoring state only.

An activation selects exactly one region from a typed, already supplied selector
value. All explicit matchers are tested before the optional fallback. Overlap
between different regions is an error, never an order-dependent priority.
Default does not absorb wrong types or invalid selector values. The enclosing
dataflow controls when a widget value is read and when the structure is activated.

Boolean has exactly two explicit values and no Default. Integer domains supported
by this editing profile are i8/u8/i16/u16/i32/u32/i64/u64. Ring transports its
underlying integer. Strings require a fallback. Enum exhaustive coverage is proved
against item IDs in a resolved nominal domain; otherwise a Default is required.
Other integer spellings recognized by TypeRef are not an executable capability claim.

## Matchers and source form

The containing node declares `"case_profile": "frog.case-editing@1"`.
Existing `boolean_exact`, `integer_exact`, `string_exact`, `enum_item` remain valid.
New matchers are:

```json
{"kind":"integer_range","lower":"10","upper":"20"}
{"kind":"integer_range","lower":"21"}
{"kind":"integer_range","upper":"-1"}
{"kind":"enum_range","lower":"idle","upper":"running"}
```

Integer endpoints are exact decimal strings; absent endpoints mean the declared
integer domain minimum/maximum. Bounds are inclusive and must be ordered/in-range.
No binary64 intermediary is permitted. Enum endpoints are existing stable item IDs;
coverage is inclusive in declaration order, never sorted by label or numeric payload.
Changing the Enum order while ranges exist requires explicit revalidation. Replacing
an Enum domain requires explicit reassociation even when the item spellings match.

The editor accepts comma unions, `a..b`, `a..`, `..b`, and radix 2/8/10/16.
Explicit 0x/0b/0o prefixes override the selected radix. Stored integers remain decimal.
Unquoted `Default` is reserved; `"Default"` is a literal string. `""`, whitespace,
commas and quoted `..` survive unchanged. Quoted String editor escapes include
backslash, quote, `\n`, `\r`, `\t`, `\s`, `\b`, `\f`, `\xHH`, `\uHHHH`.
The editor treats escape code points as UTF-8; it is not a legacy LabVIEW code-page
decoder. String ranges and surrogate-pair escapes are explicitly unsupported in v1.
Case-insensitive matching uses deterministic ASCII A–Z folding; it does not claim
Unicode locale-sensitive equivalence. A policy change creating collisions is refused.

## Saveable work versus buildable programs

Invalid text is saved as `draft_text` without replacing committed matchers or code.
A new empty non-Default region is written as `"draft": true, "matches": []`.
An empty string matcher is NOT an empty region. Drafts, missing selector, incompatible
matchers, duplicate regions, missing values, overlap and incomplete coverage block
semantic/build readiness. Every Case branch is checked, including hidden branches.

One String Default region is valid in this profile. A reader without profile support
must reject unsupported execution/export, not silently remove regions/ranges/drafts.
This is a normative reader obligation, not a retrofit of already distributed binaries.
Legacy `match` Boolean/String inputs remain readable without interpreting String
backslashes for a second time. `default` remains separate from explicit matchers.

## Editing transactions

Default moves atomically and never deletes explicit values or treatments. Removing
the flag keeps the region, even if it becomes incomplete. Adding creates a draft.
Duplication copies treatment objects with fresh IDs and remapped local references,
but not matchers, Default or Front Panel widgets/terminals. Omitted terminal edges
remain broken fragments. Nested region/port IDs retain their scope-local identity.

Delete, clear, swap, shift and reorder are distinct commands. Swap/shift move treatment
bodies and titles, preserving selector matchers/Default. Reorder moves whole region
records, preserving dispatch. Empty-case removal warns about changed fallback behavior.
Frame removal announces the visible treatment retained and the other treatments lost;
rewiring follows existing tunnel identity and cannot borrow another case's producer.
Front Panel terminals are retained outside deleted treatments. Semantic mutations
participate in Undo/Redo and are disabled while the structure is locked or running.
Navigation remains available and does not invalidate a running activation.

## Capability limits

Error Cluster status/code matching needs an explicit type identity and a separate
matcher/capability contract. An arbitrary structural cluster does not become an error
by shape. Error selectors, implicit float rounding, full Unicode/string-range behavior
and runtime lowering are not delivered by this authoring profile. Targets must report
their own capabilities before execution. No parity claim can be inferred from the UI.

## Behavioral references

- [NI: entering values for each Case subdiagram](https://www.ni.com/docs/en-US/bundle/labview/page/entering-values-for-each-case-structure-subdiagram.html)
- [NI: specifying a Default subdiagram](https://www.ni.com/docs/en-US/bundle/labview/page/specifying-a-default-subdiagram-for-case-structures.html)
- [NI: adding and duplicating subdiagrams](https://www.ni.com/docs/en-US/bundle/labview/page/adding-and-duplicating-subdiagrams.html)

These are behavior references, not an assertion of bit-for-bit compatibility with
every LabVIEW version. Studio's pristine String/Enum templates are explicit product
choices; migrations of authored structures always preserve existing treatments.
