# Public captured Event declarations, version 1

This additive Source/openIR contract admits unfiltered Pane Mouse Down and
Mouse Up. Existing Timeout, scalar Value Change, explicit User notifications,
OldVal/NewVal declarations and their previously emitted scalar open IR remain
valid. It does not reinterpret private studio_bindings or a Canvas projection.

A region explicitly names kind mouse_down or mouse_up, scope pane and source
/front_panel. That path identifies the actual public Source Front Panel
object. The private main_pane editor alias, VI/control mouse sources and filter
variants are not accepted by this profile.

The region declares all five event_payload_fields in the following order:

| Field identity | Canonical FROG type | Body port |
| --- | --- | --- |
| pane_ref | ref<"frog.front_panel"> | event_data.pane_ref |
| coords | cluster<"x":f64,"y":f64> | event_data.coords |
| button | u32 | event_data.button |
| mods | cluster<"menu_key":bool,"keypad":bool,"double_click":bool> | event_data.mods |
| platform_mods | cluster<"shift":bool,"control":bool,"alt":bool,"windows":bool> | event_data.platform_mods |

These are the established FROG Pane catalogue wire types. They are not new NI
ABI codes or pointer encodings. NI documents the corresponding Pane reference,
coordinates, mouse button and modifier fields for
[Mouse Down](https://www.ni.com/docs/en-US/bundle/labview-api-ref/page/properties-and-methods/vi-server/generic/gobject/pane/notify/mousedown.html)
and
[Mouse Up](https://www.ni.com/docs/en-US/bundle/labview-api-ref/page/properties-and-methods/vi-server/generic/gobject/pane/notify/mouseup.html).
NI identifies the left and right buttons as 1 and 2. The two cited pages do not
define a coordinate space or numeric representation for Coords. This FROG
contract therefore states its own coordinate convention explicitly: x/y are
logical content coordinates of /front_panel, computed with the actual current
client-to-canvas pan/zoom transform at the occurrence. They are never reread
after subsequent navigation.
The Windows native producer separately declares middle=3, X1=4 and X2=5 as
FROG mappings. Mouse keypad is false; double_click is true only for a real
WM_*DBLCLK occurrence. menu_key captures Control on Windows independently of
the separate platform Control/Shift/Alt/Windows fields. No higher-button NI
ABI code is asserted.

event_data_fields selects distinct names from the five declared payload
fields and the existing common source:string, type:string and time:u64 fields.
Common fields preserve the previously published FROG contract. New selected
payload inputs have no invented source default. The open IR marks them
supplied_by_event and attributes their exact Source declarations. Lowering
creates real deferred ProgramArtifact-v2 bindings only for these selected
Event Data inputs. The dispatcher must inject actual captured values before
execution; standalone execution cannot invent them.

Body topology/type/DAG admission remains mandatory before emitting open IR.
The body operations are scalar Widget Value, f64 Add and the explicitly
published typed-cluster Unbundle extension described below. Captured cluster
values and borrowed references travel through actual typed Event Data. No
aggregate Front Panel widget or Array/Enum value carrier is introduced.
The body is lowered through the existing ProgramSession/EventSession with
typed capability and an exact source/run owner. Readiness may validate
descriptors with the actual retained Source snapshot owner, without starting
a session or producing values/references. Actual Start recompiles with a
distinct live occurrence owner. A readiness stamp is never a runtime packet
authority.

The host obtains capturedOwner from the live public session and captures
actual values and borrowed references against that owner. postCaptured
admits only its exact compiled subscription/schema. The source/run owner is
separate from Source document, open-IR, SDK artifact and body identities.
Borrowed references reuse the actual ObjectReferenceRegistry; their domain,
generation and strict class/type are checked at enqueue and again at delivery,
including unconnected fields. Source aliases, deleted/recreated objects and
packets from a previous Start are refused. A live reference is never serialized.

This contract declares the Source and SDK handoff. Real Win32 WM_* production,
logical coordinate capture, Source-projected reference inventory, and UI
delivery require separate native integration tests. No editor runtimeAvailable
flag is promoted by these declarations. Structured filters, their deferred
host actions, keyboard/window/menu/drag producers, panel locking, multiple
subscribers, ordinary loop execution and typed public User Event nodes remain
separate required work.


## Captured Pane Mouse Down filter

The same Event Structure/Source realization admits `mouse_down_filter` with
`scope: pane` and `source: /front_panel`. Its compiled EventIdentity carries
`filter=true`; no unrelated private event class or numeric NI event code is
used. The existing five ordered `event_payload_fields` and historical H fields
remain exact occurrence data, not editable widget defaults.

An optional regional `event_response_fields` array declares objects containing
`id`, `type`, and `body_port`. The only permitted fields are:

| id | type | body_port |
| --- | --- | --- |
| discard | bool | event_response.discard |
| button | u32 | event_response.button |
| mods | cluster<"menu_key":bool,"keypad":bool,"double_click":bool> | event_response.mods |
| platform_mods | cluster<"shift":bool,"control":bool,"alt":bool,"windows":bool> | event_response.platform_mods |

A body connection targets the real Event Structure source-node identity and
the declared response body port. Only actually connected response terminals
become support Indicator nodes and RuntimeToIndicator SDK bindings. A declared
but unwired response creates no default producer and cannot make the body
incomplete. Unwired Discard is false; unwired Button/Mods/PlatMods preserve the
incoming captured data. H, PaneRef and Coords are read-only and cannot be
declared or driven as responses. The open IR retains the response declaration
and actual connection attribution; its Source seal covers both.

The external host enqueues a captured filter through `postCapturedFilter`
with a nonzero action token. The existing Event consumer executes its actual
ProgramArtifact and returns the correlated result. The native owner, not the
SDK or IR, owns the deferred default action. A canceled, stopped, failed, stale
or expired action must not use that response. This contract provides no
synchronous UI wait and no second runtime target or capability profile.

The verified writable fields and default Discard behavior follow the primary
[NI Pane Mouse Down? documentation](https://www.ni.com/docs/en-US/bundle/labview-api-ref/page/properties-and-methods/vi-server/generic/gobject/pane/filter/mousedown.html).
Coordinate and Windows modifier conventions remain the FROG conventions
specified above; this does not declare a LabVIEW binary ABI. The original
advanced Event requirements still include other source classes, multiple
subscribers, field decomposition, aggregate/User Event authoring, other native
producers and full lifecycle behavior; this addition does not close those work
items or promote any private catalogue runtime availability.

## Public Unbundle by Name, selection version 1

The public definition is Core.md section14a and
Libraries/Manifests/frog.core.unbundle.by_name.v1.json. Its cluster identity rules
reuse frog.typed-binding@1; no new type expression or mandatory primitive-wide
binding_profile field is introduced. A Source validator for this
extension must load Expression/schema/frog.unbundle-by-name-node.v1.schema.json
and validate each matching primitive, in addition to the existing structure and
open-IR schemas. The executable producer enforces the same syntax and performs
the required graph/cluster resolution; a syntactic schema does not supply types.

A primitive declares type frog.core.unbundle.by_name, required integer
selected_fields_version=1, and ordered selected_fields objects with exact keys
port and field_id. The sole record input must have a real driver. In phase one
the producer retains actual nodes, declared output identities and connections;
phase two topologically resolves the input's exact transportable canonical
cluster schema, selects direct field identities and commits exact output types.
Source array order is the stable output order, independently of layout and
display labels. Port IDs follow the existing IR id_string regex and the real
1024-byte port intake ceiling (or a tighter configured limit). Field IDs keep
the canonical cluster identity contract, including quotes/backslashes, without
port regex, ordinal conversion or private path parsing. Missing selections,
version, field/port/driver, duplicate ports/drivers, type mismatch and cycles
are diagnosed at their real Source paths. Derived type strings have an aggregate
byte bound before cloning into the retained graph.

Open IR attributes retain selected_fields_version and the ordered pairs;
source-map records identify the version, array, every pair and each property.
The strict reader regenerates this realization from the exact authoritative
Source, so changing an IR selection or port/type alone cannot alter execution.
The lowerer explicitly binds the published definition to the existing SDK
provider and sets RuntimeNode.selectedFields in resolved output order, under
the existing schema2/EventSession and source/run ownership contract. No new
runtime or LLVM Record backend is involved.

The initial Source producers remain the real captured Pane Coords/Mods/PlatMods
records. Nested Record/reference transport already supported by the SDK is not
a promise of arbitrary public aggregate producers. Array/Enum and aggregate
widgets retain their prior refusals. The current f64 Add-only Source path keeps
its original emission. Qualification requires independent local-schema intake,
Source-to-IR seal negatives, real SDK body outputs and real WM producers where
native UI behavior is claimed; the declaration alone closes none of those tests.
