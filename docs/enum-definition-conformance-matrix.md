# Enum definition consistency

Clarification checkpoint: 2 October 2026. Owner:
[Typed Binding Contract v1](../Expression/Typed%20Binding%20Contract%20v1.md).
This matrix specifies acceptance/rejection/preservation expectations. It is not
a report that a public backend or the current Studio revision passed these cases.

A domain/representation reference must resolve to one consistent ordered
definition. Reusing the same domain with different enumerator names, numeric
values or order cannot make two declarations compatible. The complete definition
must travel through arrays, clusters, Bundle/Unbundle and creation of constants,
controls and indicators; a shortened type string alone is insufficient.

| Case | Expected result | Preservation requirement |
| --- | --- | --- |
| Same domain, representation, ordered names and values | Compatible, subject to direction/scope/source-count checks | Preserve definition and value. |
| Same domain, a name changed | Incompatible definition, clear diagnostic | Retain declarations and broken wire for repair. |
| Same domain, names reordered | Incompatible definition | Do not silently reorder either declaration. |
| Same domain, a numeric value changed | Incompatible definition | Do not coerce an item into another item. |
| Different domain IDs | No implicit domain conversion | Explicit migration/conversion defines its mapping. |
| Same definition, visibility/disabled flags changed | No type difference from those flags | Preserve per-widget presentation. |
| Unresolved definition | Cannot claim complete compatibility | Preserve source and diagnose the limitation. |
| Enum nested in cluster/array | Compare recursively, with ordered field IDs and rank | Preserve nested definitions and values. |
| Node or structure terminal moved | Existing compatibility unchanged | Preserve IDs, references and connections; update geometry only. |
| Item IDs copied or migrated | References still resolve in the copied/migrated domain | Preserve or explicitly map value and Case item references. |

A diagnostic identifies the edge/endpoints and the first differing enumerator
or unresolved domain. Stable Case/value references remain important even when a
presentation-only choice identifier is not a type difference. Compatibility cannot
depend on color, zoom, layout, selection, current value or a downstream indicator.

Qualify serialization, semantic validation and runtime transport separately.
Record profile, exact commit, fixture, target, command and result. A Run icon or
a compiled test executable does not establish runtime qualification of this matrix.
