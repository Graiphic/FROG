# Graiphic Studio authoring document profile, version 1

Implementation reconciliation updated 13 November 2026. This page describes
the output and admission of specific Studio producers. The
[coverage matrix](../docs/studio-source-compatibility.md) attributes the tested
capabilities to their Studio and Frontend identities.

This publicly documented compatibility profile describes Studio's existing
`frog.document.draft` envelope. It is not a rename of canonical `spec_version`
source and does not silently change FROG's Language, FIR or backend semantics.
The profile document version, draft revision, application version and program
version are four different identities.

## Authority and scope

The common Frontend owns [revision admission](../Implementations/Frontend/src/FrogDocumentFormat.cpp),
[writing](../Implementations/Frontend/src/FrogDraftWriter.cpp),
[reading](../Implementations/Frontend/src/FrogDraftReader.cpp), semantic
resolution and FIR. Studio owns its editing projection, transactional file
operations, recovery, and target execution. The
[14 October reconciliation](../docs/studio-reconciliation-2026-10-14.md)
describes the captured producers and evidence from that date.

The [structural schema](Schemas/frog.studio-document.schema.json) describes
ordinary writer-shaped documents from revision 2 and the explicit revision 9
recovery envelope. Historical input 0/1 is handled by the reader and Studio
migration. Full types, nested provider payloads, dependency resolution, repair
and execution require the implementation validators.

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

The revision resolver admits integers 0 through 9. The ordinary writer
currently emits at least revision 3 and always records `storage`, including
when Readable is explicitly selected. The historical October writer emitted
revision 2 without Storage. Features raise the minimum revision.

| Revision | Existing implementation boundary |
| --- | --- |
| 0/1 | Historical authoring input; explicit migration when required |
| 2 | Historical ordinary readable output, still readable |
| 3 | Numeric Array storage envelope, also emitted with current Readable mode |
| 4 | Exact integer lexical state that older readers must not narrow |
| 5 | Complete complex lexical state |
| 6 | Symbolic Application Control references and named operations |
| 7 | Configured Connectivity, native calls and .NET metadata |
| 8 | Linked standard Type Definitions on nodes and widgets |
| 9 | Explicit Studio recovery copy carrying the retained origin |

Feature requirements compose: the writer selects the required revision for
the whole document, including nested values and node configurations. A future
revision is refused rather than partially decoded. Revision admission alone
does not prove that every configured operation can execute.

## Top-level writer model

Ordinary output includes `format`, `draft_revision`, `language`, `title`,
`metadata`, `frontPanel.widgets` and `diagram.nodes` / `diagram.edges`.
The current writer also emits `storage`, `host` and `execution_policy`.
`interface` is conditional on a retained independent declaration.
`interface_map`, `text_defaults`, `native_options`, `compilation`, `icon`,
`ide`, Canvas dimensions and widget groups depend on authored settings.
The [14 October field inventory](../docs/studio-serialization-fields-2026-10-14.md)
remains a dated inventory.

`metadata.name` is program identity and may differ from the document title.
`metadata.program_version` is not `draft_revision`. A stored `data.default`
projection is not a runtime input command and must not replace a current value.

The independently retained interface declaration is preserved when present.
Legacy presentation can use a root `interface_map` and widget bindings.
The current writer preserves the declaration when present and does not infer
one from widget order, slot order or the map.

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

Wide integers and complete complex values use revisioned lexical state. When
emitted, `state.initial_value` preserves the lexical value and
`state.numeric_value_present` identifies the numeric state. Preserve the
lexeme with the complete type when a binary64 carrier cannot represent it.
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

Array has an element type/template, rank, true data shape and elements. Diagram
constants use `array_constant` and its associated template metadata. Panel
widgets carry the collection payload in `props`. Viewport
indices and visible cell counts are presentation. A real zero, false or empty
String is present data. Slots beyond true dimensions are absent previews and
must not grow the value simply because they are visible.

Cluster fields use stable field IDs and complete ordered type metadata. A
container's bounds do not become its child's hit zone. Descendant data, indices
and container content are separate action targets. Resize or template editing
must retain field IDs, values, connections and the parent's autosizing policy.

### Map, Set and Variant

The [collection authoring codec](../Implementations/Frontend/src/CollectionAuthoringState.cpp)
writes `frog.studio.collection@1` as `studio_collection_constant` for a
diagram constant or `props["collection.state"]` for a value widget. It stores
`kind`, `comparison_policy`, complete type models when configured, and `data`.
Map has a key and a value model. Set uses only `value_type` for its elements.
The presentation templates and `visible_rows` / `visible_index` are independent
of cardinality. A missing model with empty data is a repairable authoring state.

For a Variant widget, `props["variant.payload"]` carries the serialized typed
payload. Preserve its embedded value schema, value and attributes where present.
The pretty-printed JSON in the widget, the display switches and the scrollbar
are presentation. In particular, a Variant carrying a Cluster retains the
Variant type and the Cluster's complete schema.

## Diagram, structures and repair state

`diagram.nodes` and `diagram.edges` carry stable identities. A panel terminal
node can reference its widget with `widget`. Edges address
`from` and `to` node/port identities; `bends`, layout and labels do not define
causality. A structure owns nested `regions[].diagram`. The editable model may
be flattened internally, but serialization follows the explicit owner and
region. Literal Cluster children retain `constant_owner_node_id`,
`constant_field_id` and `constant_field_order`.

For/While index terminals remain I64 and conditional terminals Boolean. Their
saved normalized positions are geometry, never a special scheduling rule.
Typed tunnels retain their identities and modes. The common resolver owns
derived selector and tunnel type hints. A future tunnel preview is
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

Front Panel references and reference-valued Cluster fields retain their complete
value schema. Use the relevant product receipt for supported widget and field
combinations. Reference-node wiring, widget authoring and runtime use each require
their own evidence.

Revision 7 retains `studio_connectivity`, `studio_native_call` and .NET reference
metadata with their exact signatures and owner/lifetime requirements. Assembly
inspection is not proof of execution, and Framework support is not .NET Core
support. Unknown or unavailable providers require diagnostics.

### Linked Type Definitions and recovery

Revision 8 writes `type_definition` with `definition_id`, `source_path`,
`resolved_revision`, `schema_fingerprint` and `baseline_widget` on the owning
node or widget, including nested widgets. `baseline_widget` is serialized JSON
inside a string. The writer also carries `unresolved_reason` and `icon_svg`.
An empty source path can represent a session draft in a snapshot. Durable
Studio Save requires the definition to be saved first. The linked
`.ftype` identity, revision and schema must be resolved through the existing
dependency handoff. Preserve the baseline, the instance values and its saved
defaults when the link is unresolved. The baseline supports recovery and
comparison. Execution still needs the applicable resolution and lowering.

Revision 9 is written by Studio recovery. The required
`ide.recoverability.frog_document_recovery` object has `version: 1`,
`original_source_base64`, `working_source_base64` and `issues`. Each issue
carries `category`, `code`, `pointer`, `message`, `interpretation` and
`blocks_run`. Reopening checks the envelope and can revalidate issues against
the retained working source. The projection may carry a panel and graph.
For an unparseable original, the archive-only producer can emit just format,
revision, title, metadata and recovery provenance. Such an archive provides
no diagram to execute. Preserve the original bytes and use the explicit
repair or Save Copy path.

### External resources

Widget realization references in `runtime`, static visual assets, and
`icon.svg` describe presentation. Callable SubFROG paths, `.ftype` links and
configured libraries are dependency identities with their own resolution
rules. Preserve the serialized paths and identifiers. The presence of a path
does not establish a resolved resource, a loaded DLL, an ABI match or a
portable Run provider.

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
