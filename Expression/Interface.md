<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Interface Specification</h1>

<p align="center">
  Definition of public program interfaces for <strong>.frog</strong> files<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose-of-the-interface">2. Purpose of the Interface</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#location-in-a-frog-file">4. Location in a .frog File</a></li>
  <li><a href="#interface-structure">5. Interface Structure</a></li>
  <li><a href="#public-input-ports">6. Public Input Ports</a></li>
  <li><a href="#public-output-ports">7. Public Output Ports</a></li>
  <li><a href="#port-properties">8. Port Properties</a></li>
  <li><a href="#input-connection-policy">9. Input Connection Policy</a></li>
  <li><a href="#type-system-integration">10. Type System Integration</a></li>
  <li><a href="#reserved-and-future-oriented-properties">11. Reserved and Future-Oriented Properties</a></li>
  <li><a href="#relation-with-the-diagram">12. Relation with the Diagram</a></li>
  <li><a href="#relation-with-the-connector">13. Relation with the Connector</a></li>
  <li><a href="#relation-with-the-front-panel">14. Relation with the Front Panel</a></li>
  <li><a href="#validation-rules">15. Validation Rules</a></li>
  <li><a href="#examples">16. Examples</a></li>
  <li><a href="#extensibility">17. Extensibility</a></li>
  <li><a href="#summary">18. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <code>interface</code> section defines the public logical contract of a FROG program.
It describes what data the FROG accepts from the outside and what data it exposes to the outside.
</p>

<p>
The interface specifies:
</p>

<ul>
  <li>the public inputs of the program,</li>
  <li>the public outputs of the program,</li>
  <li>their stable identifiers,</li>
  <li>their declared types,</li>
  <li>their public connection expectations when applicable.</li>
</ul>

<p>
The interface is the public boundary of the executable unit.
It is the contract used when a FROG is reused by another FROG, by a toolchain component, or by a hosting runtime.
</p>

<p>
The interface does not define:
</p>

<ul>
  <li>diagram layout,</li>
  <li>connector geometry,</li>
  <li>front-panel layout,</li>
  <li>widget rendering,</li>
  <li>execution scheduling,</li>
  <li>UI composition.</li>
</ul>

<p>
The interface also does not define repository-wide version policy.
Top-level <code>spec_version</code> identifies the source-format compatibility target of the <code>.frog</code> file, while published specification corpus versioning remains governed centrally in <code>Versioning/Readme.md</code>.
</p>

<hr/>

<h2 id="purpose-of-the-interface">2. Purpose of the Interface</h2>

<p>
The interface provides a stable and explicit contract between reusable program units.
</p>

<p>
It enables:
</p>

<ul>
  <li>composition of FROGs into larger graphs,</li>
  <li>static validation of public connections,</li>
  <li>clear and auditable program boundaries,</li>
  <li>safe reuse across tools, runtimes, and libraries.</li>
</ul>

<p>
The interface is part of the canonical source definition of a FROG.
It remains independent from IDE-specific presentation details and from non-authoritative cache artifacts.
</p>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document defines the public logical contract only.
It relies on other specifications for concepts outside that scope.
</p>

<ul>
  <li><code>Type.md</code> defines the FROG type system used by interface ports.</li>
  <li><code>Diagram.md</code> defines how public interface ports participate in the executable graph.</li>
  <li><code>Front panel.md</code> defines user-facing widget composition and presentation.</li>
  <li><code>Widget.md</code> defines widget classes and widget value semantics.</li>
  <li><code>Widget interaction.md</code> defines object-style widget access in the diagram.</li>
  <li><code>Connector.md</code>, when present, defines the graphical perimeter mapping used when a FROG is reused as a node.</li>
  <li><code>Interface map.md</code> defines the selected Interface Layout Pattern and explicit Front Panel widget-value correspondence for public ports.</li>
  <li><code>Versioning/Readme.md</code> defines the centralized distinction between specification corpus version, top-level <code>spec_version</code>, and program artifact versioning.</li>
</ul>

<p>
Accordingly:
</p>

<ul>
  <li>the interface defines the public contract,</li>
  <li>the diagram defines executable behavior,</li>
  <li>the connector defines reusable-node graphical mapping,</li>
  <li>the front panel defines UI composition,</li>
  <li>the centralized versioning surface defines repository-wide version-transition doctrine.</li>
</ul>

<hr/>

<h2 id="location-in-a-frog-file">4. Location in a .frog File</h2>

<p>
The interface is defined as a top-level JSON object.
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "connector": {},
  "diagram": {},
  "front_panel": {}
}</code></pre>

<p>
The <code>interface</code> section MUST be present in a canonical <code>.frog</code> source file.
</p>

<p>
In this source shape:
</p>

<ul>
  <li>top-level <code>spec_version</code> identifies the source-format compatibility target of the file,</li>
  <li><code>interface</code> defines the public contract of the executable unit,</li>
  <li>the repository-wide specification corpus version remains governed centrally in <code>Versioning/Readme.md</code>.</li>
</ul>

<hr/>

<h2 id="interface-structure">5. Interface Structure</h2>

<p>
The <code>interface</code> object contains two required arrays:
</p>

<ul>
  <li><code>inputs</code></li>
  <li><code>outputs</code></li>
</ul>

<p>
Each entry in these arrays defines one public interface port.
</p>

<pre><code>"interface": {
  "inputs": [],
  "outputs": []
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>inputs</code> MUST exist and MUST be an array.</li>
  <li><code>outputs</code> MUST exist and MUST be an array.</li>
  <li>Each entry in either array MUST be an object.</li>
  <li>Port direction is defined structurally by array membership.</li>
</ul>

<p>
A port listed in <code>inputs</code> is a public input port.
A port listed in <code>outputs</code> is a public output port.
No separate <code>direction</code> property is required in v0.1.
</p>

<p>
The interface defines the logical public contract only.
It does not define graphical placement on reusable nodes.
That responsibility belongs to the <code>connector</code> section.
</p>

<hr/>

<h2 id="public-input-ports">6. Public Input Ports</h2>

<p>
Public input ports represent data entering the FROG from the outside.
</p>

<pre><code>"inputs": [
  {
    "id": "a",
    "type": "f64"
  },
  {
    "id": "b",
    "type": "f64"
  }
]</code></pre>

<p>
Rules:
</p>

<ul>
  <li>Each input port MUST define an <code>id</code>.</li>
  <li>Each input port MUST define a <code>type</code>.</li>
  <li>Input port identifiers MUST be unique across the whole interface.</li>
  <li>Input ports MAY additionally define supported input-only properties such as <code>connection</code> and <code>default</code>.</li>
</ul>

<p>
Input ports represent externally provided values, signals, commands, events, or equivalent public inputs.
</p>

<hr/>

<h2 id="public-output-ports">7. Public Output Ports</h2>

<p>
Public output ports represent data produced by the FROG and exposed to the outside.
</p>

<pre><code>"outputs": [
  {
    "id": "result",
    "type": "f64"
  }
]</code></pre>

<p>
Rules:
</p>

<ul>
  <li>Each output port MUST define an <code>id</code>.</li>
  <li>Each output port MUST define a <code>type</code>.</li>
  <li>Output port identifiers MUST be unique across the whole interface.</li>
  <li>Output ports MUST NOT define input-only properties such as <code>connection</code> or <code>default</code> in v0.1.</li>
</ul>

<p>
Output ports represent values, signals, events, status, or equivalent public outputs made available to other FROGs or to hosting environments.
</p>

<hr/>

<h2 id="port-properties">8. Port Properties</h2>

<p>
Each interface port object contains required logical properties and MAY contain additional metadata.
</p>

<h3>8.1 <code>id</code></h3>

<p>
The <code>id</code> field is the unique logical identifier of the port.
</p>

<pre><code>"id": "temperature"</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>id</code> MUST be a string.</li>
  <li><code>id</code> MUST be unique across all interface ports, including both inputs and outputs.</li>
  <li><code>id</code> MUST remain stable enough to support reuse, validation, connector mapping, and diagram boundary mapping.</li>
</ul>

<h3>8.2 <code>type</code></h3>

<p>
The <code>type</code> field is the declared type of the port.
</p>

<pre><code>"type": "f64"</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>type</code> MUST be a valid FROG type expression.</li>
  <li><code>type</code> MUST use the canonical type syntax defined by <code>Type.md</code>.</li>
  <li><code>type</code> MUST be sufficient for static compatibility checks.</li>
</ul>

<h3>8.3 Optional metadata</h3>

<p>
Additional metadata MAY be attached to a port.
</p>

<pre><code>{
  "id": "gain",
  "type": "f64",
  "description": "Gain factor"
}</code></pre>

<p>
Such metadata does not change the meaning of the public contract unless explicitly defined by a future specification revision or by a stricter active profile.
</p>

<h3>8.4 Port order</h3>

<p>
The logical identity of a port is defined by its <code>id</code>, not by its array position.
</p>

<p>
Tools MAY preserve array order for presentation, documentation, or default connector generation.
Array order alone MUST NOT redefine interface semantics.
</p>

<hr/>

<h2 id="input-connection-policy">9. Input Connection Policy</h2>

<p>
Input ports MAY define a <code>connection</code> property to express how strongly an external connection is expected.
</p>

<pre><code>{
  "id": "gain",
  "type": "f64",
  "connection": "optional",
  "default": 1.0
}</code></pre>

<p>
Allowed values are:
</p>

<ul>
  <li><code>required</code></li>
  <li><code>recommended</code></li>
  <li><code>optional</code></li>
</ul>

<h3>9.1 <code>required</code></h3>

<p>
The input is expected to be connected.
If it is not connected, validation MUST fail unless another rule in the active profile explicitly defines a valid fallback behavior.
</p>

<h3>9.2 <code>recommended</code></h3>

<p>
The input is intended to be connected in normal use.
If it is left unconnected, validation MAY succeed, but tools SHOULD emit a warning, lint message, or equivalent diagnostic.
</p>

<h3>9.3 <code>optional</code></h3>

<p>
The input may be left unconnected without producing a validation error.
A default value MAY be provided when appropriate.
</p>

<h3>9.4 Default behavior when <code>connection</code> is omitted</h3>

<p>
If <code>connection</code> is omitted on an input port, the default value is <code>required</code>.
</p>

<h3>9.5 Use on outputs</h3>

<p>
The <code>connection</code> property applies to input ports only in v0.1.
Output ports do not use <code>required</code>, <code>recommended</code>, or <code>optional</code> in this specification version.
</p>

<h3>9.6 <code>default</code></h3>

<p>
An input port MAY define a <code>default</code> value.
</p>

<pre><code>{
  "id": "enable",
  "type": "bool",
  "connection": "optional",
  "default": true
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li>The default value, when present, MUST be compatible with the declared port type.</li>
  <li>A <code>default</code> value provides a fallback only when the input is not externally connected and when the active validation or execution profile allows such behavior.</li>
  <li>A <code>default</code> value does not by itself redefine the declared connection policy.</li>
  <li>This document does not define a separate implicit coercion mechanism for defaults beyond the general type compatibility rules of FROG.</li>
</ul>

<p>
A <code>default</code> property on an output port is not valid in v0.1.
</p>

<hr/>

<h2 id="type-system-integration">10. Type System Integration</h2>

<p>
The interface relies normatively on the FROG type system defined in <code>Type.md</code>.
</p>

<p>
Examples of valid type expressions in v0.1 include:
</p>

<pre><code>bool
i32
f64
string
array&lt;f64&gt;
array&lt;u8, 1024&gt;</code></pre>

<p>
Type identity, type compatibility, and implicit coercion behavior are not redefined here.
They MUST follow the rules defined in the FROG type specification.
</p>

<p>
In particular:
</p>

<ul>
  <li>canonical type names such as <code>i32</code> and <code>f64</code> MUST be used,</li>
  <li>non-canonical aliases such as <code>int32</code>, <code>float64</code>, or <code>double</code> are not valid canonical type expressions in v0.1,</li>
  <li>user-defined named types and library-defined custom types are outside the scope of base v0.1 unless standardized by a future revision or stricter profile.</li>
</ul>

<hr/>

<h2 id="reserved-and-future-oriented-properties">11. Reserved and Future-Oriented Properties</h2>

<p>
Some concepts commonly found in graphical programming systems are intentionally not standardized in the base v0.1 interface contract.
</p>

<h3>11.1 <code>direction</code></h3>

<p>
A separate <code>direction</code> property is unnecessary in v0.1 because direction is already defined by membership in <code>inputs</code> or <code>outputs</code>.
</p>

<p>
If a tool emits a redundant <code>direction</code> property, consumers SHOULD ignore it unless a stricter profile explicitly defines it.
</p>

<h3>11.2 Dispatch semantics</h3>

<p>
Concepts such as <code>static</code> versus <code>dynamic</code> dispatch for class-like or method-like reusable units are outside the scope of the base interface specification in v0.1.
</p>

<p>
Such semantics MAY be introduced later by a dedicated object model, class system, or stricter profile.
Until then, interface ports MUST NOT assume dispatch behavior solely from interface metadata.
</p>

<h3>11.3 Type adaptation semantics</h3>

<p>
Concepts such as <code>fixed</code> versus <code>adapt_to_type</code> are also outside the scope of the base v0.1 interface contract.
</p>

<p>
Future specifications MAY define controlled polymorphism, generic nodes, or type-adaptive ports.
Until then, declared port types are considered explicit and fixed for validation purposes.
</p>

<hr/>

<h2 id="relation-with-the-diagram">12. Relation with the Diagram</h2>

<p>
The interface defines the external contract of the FROG and MUST be reflected consistently inside the executable graph.
</p>

<p>
In v0.1, public interface participation in the diagram is represented canonically by dedicated boundary node kinds:
</p>

<ul>
  <li><code>interface_input</code> — entry point of a declared public input inside the graph,</li>
  <li><code>interface_output</code> — exit point used to drive a declared public output from the graph.</li>
</ul>

<p>
Conceptually:
</p>

<pre><code>external input  → interface_input.value   → internal graph
internal graph  → interface_output.value  → external output</code></pre>

<p>
Rules:
</p>

<ul>
  <li>An <code>interface_input</code> node MUST reference an existing interface input through <code>interface_port</code>.</li>
  <li>An <code>interface_output</code> node MUST reference an existing interface output through <code>interface_port</code>.</li>
  <li>An <code>interface_input</code> node exposes exactly one output port named <code>value</code>.</li>
  <li>An <code>interface_output</code> node exposes exactly one input port named <code>value</code>.</li>
  <li>Each declared interface input MUST be consumable by the graph.</li>
  <li>Each declared interface output MUST be producible by the graph.</li>
  <li>Unbound, contradictory, or unreachable interface boundary definitions MUST trigger validation errors.</li>
  <li>The diagram MUST NOT silently redefine or contradict the declared interface contract.</li>
</ul>

<p>
For canonical source authoring, tools SHOULD represent each declared public interface port through a dedicated matching boundary node in the owning diagram.
A stricter profile MAY strengthen this recommendation.
</p>

<hr/>

<h2 id="relation-with-the-connector">13. Relation with the Connector</h2>

<p>
The <code>interface</code> section defines the logical public ports of the FROG.
The <code>connector</code> section defines how those ports appear graphically when the FROG is reused as a node inside another diagram.
</p>

<p>
The connector does not redefine the interface.
It only maps existing interface ports to graphical positions on the reusable node boundary.
</p>

<p>
The source-owned Interface Map is specified separately in
<code>Interface map.md</code>. It records the selected layout pattern,
transforms, and widget-value-to-public-port slot correspondence. A connector
projection derived from that map MUST remain consistent with the same public
port identities and MUST NOT rely on hidden IDE state.
</p>

<p>
This mapping is done by referencing the interface port identifier and assigning it to a perimeter <code>slot</code>.
</p>

<pre><code>"connector": {
  "granularity": 40,
  "ports": [
    { "interface_port": "a", "slot": 34 },
    { "interface_port": "b", "slot": 36 },
    { "interface_port": "result", "slot": 14 }
  ]
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li>Every connector port mapping MUST reference an existing interface port.</li>
  <li>The connector MUST NOT create new public ports that do not exist in the interface.</li>
  <li>The connector MUST NOT change the logical type or direction of an interface port.</li>
  <li>The logical contract remains owned by the <code>interface</code> section.</li>
</ul>

<hr/>

<h2 id="relation-with-the-front-panel">14. Relation with the Front Panel</h2>

<p>
The <code>front_panel</code> section defines user-facing widget composition.
It does not define the public logical API of the FROG.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>a front-panel widget does not create a public interface port by itself,</li>
  <li>a control widget is not automatically a public input,</li>
  <li>an indicator widget is not automatically a public output.</li>
</ul>

<p>
If a program chooses to relate public interface behavior to front-panel behavior, that relationship MUST be expressed in the diagram.
</p>

<p>
Examples:
</p>

<ul>
  <li>a public input MAY drive a widget indicator through the diagram,</li>
  <li>a widget control value MAY contribute to a public output through the diagram,</li>
  <li>a public input MAY also be connected to internal logic without any front-panel widget at all.</li>
</ul>

<p>
This separation keeps:
</p>

<ul>
  <li>the public interface stable and tool-agnostic,</li>
  <li>the front panel user-facing and optional at runtime,</li>
  <li>the diagram authoritative for executable behavior.</li>
</ul>

<hr/>

<h2 id="validation-rules">15. Validation Rules</h2>

<p>
An interface is valid only if all of the following hold:
</p>

<ul>
  <li><code>interface</code> exists and is an object,</li>
  <li><code>inputs</code> exists and is an array,</li>
  <li><code>outputs</code> exists and is an array,</li>
  <li>every port object defines a valid <code>id</code>,</li>
  <li>every port object defines a valid <code>type</code>,</li>
  <li>all port identifiers are unique across the whole interface,</li>
  <li>all declared types are valid canonical FROG type expressions,</li>
  <li>input-only properties are used only on inputs,</li>
  <li>all declared default values are type-compatible when present.</li>
</ul>

<p>
When a full FROG is validated, consistency checks MUST also include:
</p>

<ul>
  <li>every <code>interface_input</code> node references an existing public input,</li>
  <li>every <code>interface_output</code> node references an existing public output,</li>
  <li>the diagram does not contradict the declared public contract,</li>
  <li>the connector does not contradict the declared public contract.</li>
  <li>the Interface Map does not reference an unknown pattern, slot, widget, or public port,</li>
  <li>Interface Map widget role and value type agree with public port direction and type.</li>
</ul>

<p>
Validators SHOULD diagnose at least the following error classes:
</p>

<ul>
  <li>duplicate port identifier,</li>
  <li>missing <code>id</code>,</li>
  <li>missing <code>type</code>,</li>
  <li>invalid canonical type expression,</li>
  <li>invalid use of <code>connection</code> on an output port,</li>
  <li>invalid use of <code>default</code> on an output port,</li>
  <li>type-incompatible default value,</li>
  <li>unknown interface port referenced by a diagram boundary node,</li>
  <li>unknown interface port referenced by the connector.</li>
</ul>

<p>
These checks validate the public contract of the source artifact.
They do not, by themselves, redefine top-level <code>spec_version</code> policy or repository-wide corpus-version governance.
</p>

<hr/>

<h2 id="examples">16. Examples</h2>

<h3>16.1 Minimal interface</h3>

<pre><code>"interface": {
  "inputs": [
    { "id": "a", "type": "f64" },
    { "id": "b", "type": "f64" }
  ],
  "outputs": [
    { "id": "result", "type": "f64" }
  ]
}</code></pre>

<h3>16.2 Input connection policy with defaults</h3>

<pre><code>"interface": {
  "inputs": [
    {
      "id": "gain",
      "type": "f64",
      "connection": "optional",
      "default": 1.0,
      "description": "Gain factor"
    },
    {
      "id": "enable",
      "type": "bool",
      "connection": "recommended",
      "default": true
    }
  ],
  "outputs": [
    {
      "id": "signal_out",
      "type": "array&lt;f64&gt;"
    }
  ]
}</code></pre>

<h3>16.3 Canonical public boundary participation in the diagram</h3>

<pre><code>{
  "interface": {
    "inputs": [
      { "id": "a", "type": "f64" },
      { "id": "b", "type": "f64" }
    ],
    "outputs": [
      { "id": "result", "type": "f64" }
    ]
  },
  "diagram": {
    "nodes": [
      {
        "id": "input_a",
        "kind": "interface_input",
        "interface_port": "a"
      },
      {
        "id": "input_b",
        "kind": "interface_input",
        "interface_port": "b"
      },
      {
        "id": "add_1",
        "kind": "primitive",
        "type": "frog.core.add"
      },
      {
        "id": "output_result",
        "kind": "interface_output",
        "interface_port": "result"
      }
    ],
    "edges": [
      {
        "id": "e1",
        "from": { "node": "input_a", "port": "value" },
        "to":   { "node": "add_1", "port": "a" }
      },
      {
        "id": "e2",
        "from": { "node": "input_b", "port": "value" },
        "to":   { "node": "add_1", "port": "b" }
      },
      {
        "id": "e3",
        "from": { "node": "add_1", "port": "result" },
        "to":   { "node": "output_result", "port": "value" }
      }
    ]
  }
}</code></pre>

<h3>16.4 Interface and front panel remain distinct</h3>

<pre><code>{
  "interface": {
    "inputs": [
      { "id": "signal", "type": "f64" }
    ],
    "outputs": [
      { "id": "result", "type": "f64" }
    ]
  },
  "front_panel": {
    "widgets": [
      {
        "id": "ctrl_gain",
        "role": "control",
        "widget": "frog.ui.standard.numeric_control",
        "value_type": "f64"
      },
      {
        "id": "ind_result",
        "role": "indicator",
        "widget": "frog.ui.standard.numeric_indicator",
        "value_type": "f64"
      }
    ]
  }
}</code></pre>

<p>
In this example, the public contract is defined only by <code>interface</code>.
The widgets do not create public ports by themselves.
Any relationship between these widgets and the public interface would be expressed in the diagram.
</p>

<hr/>

<h2 id="extensibility">17. Extensibility</h2>

<p>
The base v0.1 interface contract is intentionally conservative.
It standardizes only the fields necessary to define a stable public contract.
</p>

<p>
Future revisions or stricter profiles MAY extend the interface model with controlled additional semantics, such as:
</p>

<ul>
  <li>richer metadata vocabularies,</li>
  <li>documentation fields with stronger structure,</li>
  <li>object-oriented dispatch metadata,</li>
  <li>controlled genericity or type adaptation,</li>
  <li>profile-specific public-port capabilities.</li>
</ul>

<p>
Any such extension MUST preserve the distinction between:
</p>

<ul>
  <li>logical public interface semantics,</li>
  <li>diagram execution semantics,</li>
  <li>connector presentation semantics,</li>
  <li>front-panel UI semantics.</li>
</ul>

<p>
Such extensions must also remain compatible with the centralized cumulative version model: later source-format versions should normally extend earlier valid interface forms rather than silently replace them, unless an explicit breaking boundary is declared in repository-wide version governance.
</p>

<hr/>

<h2 id="summary">18. Summary</h2>

<p>
The FROG <code>interface</code> section defines the public logical contract of a FROG.
It specifies what enters and what leaves the executable unit, with stable identifiers and declared types.
</p>

<ul>
  <li>Public inputs are declared in <code>inputs</code>.</li>
  <li>Public outputs are declared in <code>outputs</code>.</li>
  <li>Port identity is defined by <code>id</code>, not by position.</li>
  <li>Port types MUST use canonical FROG type syntax.</li>
  <li>Input connection expectations MAY use <code>required</code>, <code>recommended</code>, or <code>optional</code>.</li>
  <li>Public boundary participation in the diagram is represented canonically by <code>interface_input</code> and <code>interface_output</code>.</li>
  <li>The connector maps interface ports graphically but does not redefine them.</li>
  <li>The front panel does not define the public API.</li>
  <li>The interface does not define source-format compatibility law or published specification corpus versioning.</li>
</ul>

<p>
This keeps FROG interfaces explicit, durable, auditable, and reusable across tools, runtimes, and long-term specification evolution.
</p>
