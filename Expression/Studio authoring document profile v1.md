# Graiphic Studio authoring document profile, version 1

Implementation reconciliation: 14 October 2026. **Graiphic Studio is the evidence
authority for the product behavior and serialization described here.** Public
documentation follows that evidence; a proposed feature is not implementation.

This publicly documented compatibility profile describes Studio's existing
`frog.document.draft` envelope. It is not a rename of canonical `spec_version`
source and does not silently change FROG's Language, FIR or backend semantics.
The profile document version, draft revision, application version and program
version are four different identities.

## Authority and scope

The implementation owners are `FrogDocumentFormat`, `FrogDocument` and
`FrogDocumentSerialization` in Graiphic Studio. They own version admission,
load/save and the actual editing model. Their serializers, typed resolvers and
qualified product fixtures are evidence for this profile. The
[reconciliation record](../docs/studio-reconciliation-2026-10-14.md) identifies
the captured revisions and separates compiled delivery from work in progress.

The [structural schema](Schemas/frog.studio-document.schema.json) describes
current writer-shaped documents. It is not a complete model of permissive
historical import, type compatibility, version-feature gates or execution.
Historical readable revisions 0/1 remain a migration responsibility of Studio;
this schema does not remove their loader support.

## Envelopes and revision admission

| Fact | Studio authoring document | Canonical public source |
| --- | --- | --- |
| Format identity | `format: "frog.document.draft"` | `spec_version: "0.1"` |
| Format revision | Integer `draft_revision` | Source compatibility version |
| Front Panel member | `frontPanel` | Optional `front_panel` |
| Graph | Editing state, including repairable invalid state | Published source-owned graph |
| Authority | Independent retained declarations plus Studio authoring model | Exact public Source tree |
| Execution | Requires actual admitted target/provider | Requires validation, FIR/lowering and target admission |

A document declaring both `format` and `spec_version` is refused: it cannot
select two source authorities. Changing a filename or replacing key spellings
is not a migration. A public Source reader can preserve more constructs than
the editing importer can materialize; reading and canvas editing are separate.

The current resolver admits integer revisions 0 through 7. Ordinary readable
writer output starts at revision 2 and advances when a feature requires it:

| Revision | Existing implementation boundary |
| --- | --- |
| 0/1 | Historical authoring input; explicit migration when required |
| 2 | Ordinary readable authoring output |
| 3 | Opt-in numeric Array storage envelope |
| 4 | Exact integer lexical state that older readers must not narrow |
| 5 | Complete complex lexical state |
| 6 | Symbolic Application Control references and named operations |
| 7 | Configured Connectivity, native calls and .NET metadata |

Feature requirements compose: the writer selects the required revision for
the whole document, including nested values and node configurations. A future
revision is refused rather than partially decoded. Revision admission alone
does not prove that every configured operation can execute.

## Top-level writer model

Current output includes `format`, `draft_revision`, `language`, `title`,
`metadata`, `interface`, `frontPanel` and `diagram`. Host and execution policy
are serialized independently. `storage`, `compilation`, `icon`, `ide` and
legacy `interface_map` are emitted when their corresponding model requires
them. Detailed field occurrences are in the
[serializer field inventory](../docs/studio-serialization-fields-2026-10-14.md).

`metadata.name` is program identity and may differ from the document title.
`metadata.program_version` is not `draft_revision`. A stored `data.default`
projection is not a runtime input command and must not replace a current value.

The independently retained interface declaration is preserved when present.
Legacy documents can still use the historical interface projection from widget
bindings. Loading such a projection does not establish an independent canonical
public interface. Never regenerate a retained declaration from widget order.

## Widgets, roles and presentation

A value widget carries stable `id`, `classId`, `role`, `valueType` and `bounds`,
with optional `props`, `state`, `geometry`, `typography`, `binding` and realization
references in `runtime`. A static visual has its separate `type: "staticVisual"`
shape; it is not a value terminal. Realization paths are presentation references,
not proof of execution availability.

`control` and `indicator` have different data directions. During admitted Run,
controls can submit typed transient inputs and indicators remain read-only.
Authoring selection, resize and placement are locked; scrolling or moving Array
indices remains navigation. Runtime observations must not overwrite the saved
defaults or append authoring Undo operations.

Labels and captions remain separate surfaces. The caption default-size posture
and explicit override must survive save/load. `typography.resolvedPointSize` is
a resolved presentation value; it does not mean every child has an explicit
custom size. Existing geometry must not be replaced by a generic 20-point box.

### Numeric values

`props` retains `display.format.kind`, `display.format.digits`,
`display.format.precision_type`, `display.format.hide_trailing_zeros` and the
other declared format options. Each destination formats its own observation.
Formatting changes presentation only: six significant digits and six fractional
digits are distinct, and neither rounds the underlying wire value.

Wide integers and complete complex values use the existing revisioned lexical
state representation. Do not deserialize them through a mandatory `double`.
The [Array storage profile](../docs/studio-array-storage-v1.md) owns compact
numeric records. A format declaration never supplies a missing numeric value.

### Enum, Ring and Dropdown String

Enum preserves its domain, ordered items, representation and stable item IDs.
Ring preserves its numeric item values; a selected index is not the transported
value. Dropdown String uses a String widget with `display.string.dropdown`,
item metadata, selected identity and `text.value`. Its execution data is String;
numeric selection bookkeeping must not turn its terminal into a numeric Enum.

Create Control/Constant/Indicator from a terminal uses the existing semantic
terminal-name resolver. Stable port IDs can remain `a`, `b` or `result` while
visible names express `x`, `y` or `x+y`. Preserve the selected terminal's spaces
and symbols and the existing duplicate-name policy. Palette-created names are
not renamed by this rule.

### Array and Cluster

Array has an element type/template, rank, true data shape and elements; viewport
indices and visible cell counts are presentation. A real zero, false or empty
String is present data. Slots beyond true dimensions are absent previews and
must not grow the value simply because they are visible.

Cluster fields use stable field IDs and complete ordered type metadata. A
container's bounds do not become its child's hit zone. Descendant data, indices
and container content are separate action targets. Resize or template editing
must retain field IDs, values, connections and the parent's autosizing policy.

## Diagram, structures and repair state

`diagram.nodes` and `diagram.edges` carry stable identities. Edges address
`from` and `to` node/port identities; `bends`, layout and labels do not define
causality. A structure owns nested `regions[].diagram`. The editable model may
be flattened internally, but serialization follows the explicit owner and
region. Literal Cluster children retain `constant_owner_node_id`,
`constant_field_id` and `constant_field_order`.

For/While index terminals remain I64 and conditional terminals Boolean. Their
saved normalized positions are geometry, never a special scheduling rule.
Typed tunnels retain their identities and modes. A future tunnel preview is
not a connection until committed, and existing tunnels cannot be moved by the
coordination of newly previewed tunnels.

`wire_fragments`, unresolved fields and incomplete authoring configuration can
be retained for repair. A saved draft is not necessarily runnable. The real
validator must reject invalid types, missing required drivers, cycles and
unsupported execution before starting an old or unrelated artifact.

Event rows, selected data fields and response shelf fields are authoring
configuration. `event_response_fields` does not silently define dispatch or
arbitration. The separate
[captured Event profile](Event%20Captured%20Profile%20v1.md) owns its public
packet, Source ownership and supported lowering.

## Symbolic references and operations

Revision 6 records `studio_application_control` with profile
`frog.studio.application_control@1`, kind `reference`, `property`, `invoke` or
`close`, `expected_class`, complete `value_schema`, symbolic `target`, ordered
property rows, `method_id` and parameter bindings.

The target distinguishes `unbound`, `this_frog`, `this_application`,
`front_panel` and `widget`, with document/widget identity. It is not a persisted
OS pointer. Complete schema includes canonical type, choice kind, field names,
choices and recursive child schemas. Short labels such as Boolean or String
do not erase the referenced object's identity or type.

Property/Invoke adaptation follows the connected reference schema. Compatible
member configuration is preserved; incompatible members remain diagnosable.
Read/write permissions and method signatures govern actual port directions.
Editor services do not establish a graph execution provider or public lowering.

The working tree may contain a reference-value Front Panel widget or reference
fields inside Clusters before that new product lot is compiled and qualified.
These are explicitly tracked as work in progress in the reconciliation record;
the prior qualified reference-node wiring is a different capability.

Revision 7 retains `studio_connectivity`, `studio_native_call` and .NET reference
metadata with their exact signatures and owner/lifetime requirements. Assembly
inspection is not proof of execution, and Framework support is not .NET Core
support. Unknown or unavailable providers require diagnostics.

## Interface Map and Custom Pattern

Widget `binding` uses `mode: "widget_value"`, explicit input/output ID,
`interface_map_slot` and connection requirement. Slot identity is independent
from its rectangle, display color and current list position.

Disconnect clears only the selected association and returns that slot to neutral
appearance. It does not remove or renumber the slot. Copy creates an independent
unbound object and must leave the original's association unchanged.

Custom descriptors `custom_v1`, `custom_v2` and `custom_v3` belong to the existing
`custom_v1` binding-map implementation profile; descriptor version and profile
identity are different. New editor dimensions independently range from 8 to
128 pixels for width and height. These are product editor bounds, not a global
limit on all FROG icons or historical import. Reset restores the opening layout;
Clear bindings starts from an empty layout. Perimeter handles and their hover
animation are transient and never execution data.

## Capability and compatibility claims

The [Studio authoring catalogue](../Profiles/StudioAuthoring.md) publishes the
actual declarative contracts as a product compatibility snapshot. Its authoring
entries, resolver policies and `runtime_availability` descriptors do not imply
that all those operations are admitted by a native backend.

Keep Read, Preserve, Edit, Validate, Lower and Execute separate. Keep private
simulation, native compiled execution, captured Event execution and replay
separate. Preserve unsupported source or refuse explicitly; never discard
unknown executable fields and call the result a successful migration.

The new schema is structural documentation. It has not been wired into Studio's
loader by this documentation reconciliation and is not a new runtime gate.
