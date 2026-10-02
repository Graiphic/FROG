# Read and write a `.frog` program

This guide explains the public source syntax using an existing repository example.
The owning contracts remain [Expression](Readme.md), [Language](../Language/Readme.md),
the versioned [Libraries](../Libraries/Readme.md) and [Versioning](../Versioning/Readme.md).
It adds no new source version or runtime capability.

## Start with the envelope

A canonical `.frog` file is one UTF-8 JSON object. Use `"spec_version": "0.1"`
for the currently published base source format. Readers also accept the historical
numeric spelling `0.1` where the version contract permits it; new writers should
use the string form. JSON object member order and indentation do not define graph
execution. Array order can be meaningful, for example for ordered alternatives.
Members must be unique after JSON escape decoding. JSON comments, trailing commas,
bare `NaN` and bare `Infinity` are not source syntax.

Four sections are required: `spec_version`, `metadata`, `interface`, `diagram`.
An empty interface is represented by empty input/output lists. A Front Panel is
optional; a program can have public inputs and outputs without widgets or windows.

## A complete example

The following program is [Example 01](../Examples/01_pure_addition/main.frog),
formatted for reading. It adds two `f64` inputs and exposes one `f64` result.

```json
{
  "spec_version": "0.1",
  "metadata": {
    "name": "01_pure_addition",
    "description": "Minimal executable reference slice: two public inputs, one frog.core.add primitive, one public output."
  },
  "interface": {
    "inputs": [
      {"id": "a", "type": "f64"},
      {"id": "b", "type": "f64"}
    ],
    "outputs": [{"id": "result", "type": "f64"}]
  },
  "diagram": {
    "nodes": [
      {"id": "in_a", "kind": "interface_input", "interface_port": "a"},
      {"id": "in_b", "kind": "interface_input", "interface_port": "b"},
      {"id": "add_1", "kind": "primitive", "type": "frog.core.add"},
      {"id": "out_result", "kind": "interface_output", "interface_port": "result"}
    ],
    "edges": [
      {"id": "e1", "from": {"node": "in_a", "port": "value"}, "to": {"node": "add_1", "port": "a"}},
      {"id": "e2", "from": {"node": "in_b", "port": "value"}, "to": {"node": "add_1", "port": "b"}},
      {"id": "e3", "from": {"node": "add_1", "port": "result"}, "to": {"node": "out_result", "port": "value"}}
    ]
  }
}
```

| Source fact | Meaning |
| --- | --- |
| `metadata.name` | Program name; it does not identify a node or port. |
| `interface.inputs/outputs[].id` | Stable public port identity, unique across both directions. |
| `interface.*[].type` | Declared value type. In this example `f64` means a 64-bit floating-point value. |
| `nodes[].id` | Node identity in its graph scope. Changing its visible label must preserve this identity. |
| `kind` | Node category. An interface projection and a primitive have different contracts. |
| Primitive `type` | Published operation identity, here `frog.core.add`; it is not the wire's value type. |
| `interface_port` | Reference to the public input or output declaration. |
| `from` / `to` | Source and sink references using node ID and stable port ID. |

The Add port IDs are `a`, `b` and `result`. Interface projections use `value`.
Do not invent port names from icon positions or displayed text. An edge does not
declare an independent authoritative type; the endpoint contracts resolve it.
The source needs no stored execution order for these three edges: data dependencies
determine which inputs the operation needs.

## Add presentation without changing the graph

| Optional section | Owner and purpose |
| --- | --- |
| `front_panel` | Widget instances and explicit value bindings; [Front panel](Front%20panel.md). |
| `connector` | Public interface presentation; [Connector](Connector.md). |
| `icon` | Program icon source; [Icon](Icon.md). |
| `host` | Declared host/window policy; [Host and execution policy](Host%20and%20execution%20policy.md). |
| `execution_policy` | Source-owned execution policy within its published contract. |
| `ide` | Source-owned authoring preferences; [IDE preferences](IDE%20preferences.md). |
| `cache` | Non-authoritative derived information; [Cache](Cache.md). |

Moving a node changes its layout and wire geometry, not IDs, port references,
type declarations or graph ownership. UI selection, font, color and zoom cannot
make an incompatible connection compatible. A widget package `.wfrog` owns its
realization; it is not another program graph or a replacement for `.frog`.

## Types, profiles and nested graphs

Use [Type](Type.md) and [Numeric representations](Numeric%20representations.md)
for the base vocabulary. [Typed Binding Contract v1](Typed%20Binding%20Contract%20v1.md)
adds bounded Enum/cluster/ranked-array rules. Its type strings do not replace the
complete Enum definition or ordered cluster field schema. Do not infer support
from a widget palette or erase these definitions when copying or connecting values.

Structures have family-specific source contracts in
[Control structures](Control%20structures.md). Connections inside a region refer
to that graph's ports. Cross-wall connections go through the declared boundary
faces; screen coordinates do not establish region ownership. Start with a matching
numbered example and its declared profile rather than converting a flat graph into
a structure by inventing JSON fields. A Case extension declares its profile at
the Case node; this does not introduce an undocumented root-level field.

## Reading, validating and running are separate steps

1. Decode strict JSON and recognize the version and source envelope.
2. Check source shape using the published [schema](schema/frog.schema.json).
3. Resolve identities, ports, types, dependencies, regions and required inputs
   using [semantic validation](../Language/Semantic%20validation%20before%20FIR.md).
4. Derive FIR, lower and admit the program only for an explicitly supported target.

The current root schema mainly checks section presence and shape. Passing it is
not proof of full graph correctness, profile support or execution availability.
An unsupported source must produce a precise diagnostic and retain its original
content; silently removing nodes, fields or edges is not a migration.
Broken-wire editor drafts remain distinguishable from valid executable source.

For implementation examples, see the [reference workspace](../Implementations/Reference/Readme.md)
and its [source validator](../Implementations/Reference/Validator/validate_source.md).
The bounded public runtime reference closure is Examples 01–15. Later numbered
examples document additional source/widget/design surfaces; numbering alone
does not make them runnable.

## Graiphic Studio documents

Graiphic Studio 0.0.4.044 still writes authoring documents with
`"format": "frog.document.draft"`, `"draft_revision": 2` and `frontPanel`.
It also has a separate public-source reader/writer with bounded capabilities.
The common `.frog` extension does not make these envelopes interchangeable.
Replacing `format` with `spec_version`, renaming a section or deleting unsupported
content does not convert a document safely. See
[Studio/source compatibility](../docs/studio-source-compatibility.md) and
[Source compatibility and profiles](Source%20compatibility%20and%20profiles.md)
before implementing an importer, writer or editor.
