<h1 align="center">🐸 FROG Interface Specification</h1>

<p align="center">
Definition of program interfaces for <strong>.frog</strong> files<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
<li><a href="#overview">1. Overview</a></li>
<li><a href="#purpose">2. Purpose of the Interface</a></li>
<li><a href="#location">3. Location in a .frog file</a></li>
<li><a href="#structure">4. Interface Structure</a></li>
<li><a href="#inputs">5. Input Ports</a></li>
<li><a href="#outputs">6. Output Ports</a></li>
<li><a href="#port-properties">7. Port Properties</a></li>
<li><a href="#type-system">8. Type System Integration</a></li>
<li><a href="#diagram-binding">9. Binding with the Diagram</a></li>
<li><a href="#connector-relation">10. Relation with Connector</a></li>
<li><a href="#examples">11. Examples</a></li>
<li><a href="#validation">12. Validation Rules</a></li>
<li><a href="#extensibility">13. Extensibility</a></li>
<li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>interface</strong> section defines the public contract of a Frog.
</p>

<p>
It specifies:
</p>

<ul>
<li>the program inputs</li>
<li>the program outputs</li>
<li>their identifiers</li>
<li>their data types</li>
</ul>

<p>
The interface defines how a Frog exchanges data with the outside world and with other Frogs.
</p>

<p>
It is the logical boundary of the program. It does not define graphical layout, connector placement, or execution scheduling.
</p>

<hr/>

<h2 id="purpose">2. Purpose of the Interface</h2>

<p>
The interface provides a stable and explicit contract between reusable program units.
</p>

<p>
It enables:
</p>

<ul>
<li>composition of Frogs into larger graphs</li>
<li>static validation of connections</li>
<li>clear and auditable program boundaries</li>
<li>safe reuse across libraries and tools</li>
</ul>

<p>
The interface is part of the program definition and remains independent from IDE-specific presentation details.
</p>

<hr/>

<h2 id="location">3. Location in a .frog file</h2>

<p>
The interface is defined as a top-level JSON object.
</p>

<pre>
{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "connector": {},
  "diagram": {},
  "front_panel": {}
}
</pre>

<p>
The <code>interface</code> section MUST be present.
</p>

<hr/>

<h2 id="structure">4. Interface Structure</h2>

<p>
The interface object contains two arrays:
</p>

<ul>
<li><strong>inputs</strong></li>
<li><strong>outputs</strong></li>
</ul>

<p>
Each entry in these arrays defines one interface port.
</p>

<pre>
"interface": {
  "inputs": [],
  "outputs": []
}
</pre>

<p>
The interface describes the logical contract only.
It does not define where ports are drawn on a node. That responsibility belongs to the <code>connector</code> section.
</p>

<hr/>

<h2 id="inputs">5. Input Ports</h2>

<p>
Input ports represent data entering the Frog from the outside.
</p>

<pre>
"inputs": [
  {
    "id": "a",
    "type": "float64"
  },
  {
    "id": "b",
    "type": "float64"
  }
]
</pre>

<p>
Rules:
</p>

<ul>
<li>Each input port MUST define an <code>id</code>.</li>
<li>Each input port MUST define a <code>type</code>.</li>
<li>Input port identifiers MUST be unique within the interface.</li>
<li>Input ports represent externally provided values, signals, events, or references.</li>
</ul>

<hr/>

<h2 id="outputs">6. Output Ports</h2>

<p>
Output ports represent data produced by the Frog and exposed to the outside.
</p>

<pre>
"outputs": [
  {
    "id": "result",
    "type": "float64"
  }
]
</pre>

<p>
Rules:
</p>

<ul>
<li>Each output port MUST define an <code>id</code>.</li>
<li>Each output port MUST define a <code>type</code>.</li>
<li>Output port identifiers MUST be unique within the interface.</li>
<li>Output ports represent values, signals, events, or references made available to other Frogs or hosting environments.</li>
</ul>

<hr/>

<h2 id="port-properties">7. Port Properties</h2>

<p>
Each port object contains required logical properties and MAY contain additional metadata.
</p>

<h3>id</h3>

<p>
Unique identifier of the port.
</p>

<pre>
"id": "temperature"
</pre>

<p>
Rules:
</p>

<ul>
<li>MUST be a string.</li>
<li>MUST be unique across all interface ports.</li>
<li>MUST remain stable enough to support reuse, validation, and connector mapping.</li>
</ul>

<h3>type</h3>

<p>
Defines the declared data type of the port.
</p>

<pre>
"type": "float64"
</pre>

<p>
Rules:
</p>

<ul>
<li>MUST reference a valid FROG type.</li>
<li>MUST be sufficient for static compatibility checks.</li>
</ul>

<h3>Optional Properties</h3>

<p>
Additional metadata MAY be attached to a port.
</p>

<pre>
{
  "id": "gain",
  "type": "float64",
  "default": 1.0,
  "description": "Gain factor"
}
</pre>

<p>
Such metadata does not change the meaning of the interface contract unless explicitly defined by a future specification.
</p>

<hr/>

<h2 id="type-system">8. Type System Integration</h2>

<p>
The interface relies on the FROG type system.
</p>

<p>
Examples of valid type expressions may include:
</p>

<pre>
float64
int32
bool
string
array&lt;float64&gt;
</pre>

<p>
Type compatibility MUST be validated during graph validation.
</p>

<p>
Libraries MAY introduce additional types, provided that tools can validate or safely reject them according to the active specification profile.
</p>

<hr/>

<h2 id="diagram-binding">9. Binding with the Diagram</h2>

<p>
Interface ports define the external contract of the Frog and MUST be bound to the internal program graph.
</p>

<p>
A diagram implementation MAY represent interface boundaries using dedicated entry and exit nodes, or any equivalent internal mechanism.
</p>

<p>
Conceptually:
</p>

<pre>
external input  ->  interface input  ->  diagram
diagram         ->  interface output ->  external output
</pre>

<p>
Rules:
</p>

<ul>
<li>Each declared interface input MUST be consumable by the diagram.</li>
<li>Each declared interface output MUST be producible by the diagram.</li>
<li>Unbound or inconsistent interface definitions MUST trigger validation errors.</li>
</ul>

<hr/>

<h2 id="connector-relation">10. Relation with Connector</h2>

<p>
The <code>interface</code> section defines the logical ports of the Frog.
The <code>connector</code> section defines how those ports appear graphically when the Frog is reused as a node inside another diagram.
</p>

<p>
The connector does not redefine the interface.
It only maps existing interface ports to graphical positions on the node perimeter.
</p>

<p>
This mapping is done by referencing the interface port identifier and assigning it to a perimeter <code>slot</code>.
</p>

<pre>
"connector": {
  "granularity": 40,
  "ports": [
    { "interface_port": "a", "slot": 34 },
    { "interface_port": "b", "slot": 36 },
    { "interface_port": "result", "slot": 14 }
  ]
}
</pre>

<p>
Rules:
</p>

<ul>
<li>Each <code>interface_port</code> value in the connector MUST reference an existing port declared in <code>interface</code>.</li>
<li>The connector MUST NOT introduce new logical ports.</li>
<li>The connector MUST NOT modify port types or port semantics.</li>
<li>A port MAY exist in the interface without appearing in the connector if a given tool or profile allows it.</li>
</ul>

<hr/>

<h2 id="examples">11. Examples</h2>

<h3>Minimal Interface</h3>

<pre>
"interface": {
  "inputs": [
    { "id": "a", "type": "float64" }
  ],
  "outputs": [
    { "id": "result", "type": "float64" }
  ]
}
</pre>

<h3>Arithmetic Node with Connector Mapping</h3>

<pre>
{
  "interface": {
    "inputs": [
      { "id": "a", "type": "float64" },
      { "id": "b", "type": "float64" }
    ],
    "outputs": [
      { "id": "result", "type": "float64" }
    ]
  },
  "connector": {
    "granularity": 40,
    "ports": [
      { "interface_port": "a", "slot": 34 },
      { "interface_port": "b", "slot": 36 },
      { "interface_port": "result", "slot": 14 }
    ]
  }
}
</pre>

<h3>Multi-Input Process Node</h3>

<pre>
"interface": {
  "inputs": [
    { "id": "temperature", "type": "float64" },
    { "id": "pressure", "type": "float64" },
    { "id": "enable", "type": "bool", "default": true }
  ],
  "outputs": [
    { "id": "density", "type": "float64" },
    { "id": "status", "type": "string" }
  ]
}
</pre>

<hr/>

<h2 id="validation">12. Validation Rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
<li><code>interface</code> MUST exist.</li>
<li><code>inputs</code> and <code>outputs</code> MUST exist and MUST be arrays.</li>
<li>Each port object MUST define an <code>id</code>.</li>
<li>Each port object MUST define a <code>type</code>.</li>
<li>Port identifiers MUST be unique across the whole interface.</li>
<li>Declared types MUST be syntactically valid according to the active specification profile.</li>
<li>The diagram binding MUST be consistent with the declared interface.</li>
</ul>

<p>
When a connector is present:
</p>

<ul>
<li>each referenced <code>interface_port</code> MUST exist in the interface</li>
<li>connector mappings MUST remain compatible with the interface definition</li>
</ul>

<p>
Unknown properties MUST be ignored unless a stricter profile explicitly defines them.
</p>

<hr/>

<h2 id="extensibility">13. Extensibility</h2>

<p>
Tools and libraries MAY add additional non-breaking metadata fields to port definitions.
</p>

<pre>
{
  "id": "speed",
  "type": "float64",
  "unit": "m/s",
  "description": "Measured linear speed"
}
</pre>

<p>
Examples of such metadata may include:
</p>

<ul>
<li>human-readable descriptions</li>
<li>units</li>
<li>default values</li>
<li>editor hints</li>
<li>documentation annotations</li>
</ul>

<p>
Runtime systems and validators that do not understand these extra properties MUST ignore them unless a stricter profile specifies otherwise.
</p>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
The interface defines the external logical contract of a Frog.
</p>

<p>
It provides:
</p>

<ul>
<li>clear program boundaries</li>
<li>type-aware composition</li>
<li>stable port identifiers</li>
<li>a foundation for reusable graphical nodes</li>
</ul>

<p>
The interface describes <strong>what</strong> a Frog exposes.
The connector describes <strong>where</strong> those ports appear graphically.
Together, they allow Frogs to be safely validated, reused, and composed into larger executable graphs.
</p>
