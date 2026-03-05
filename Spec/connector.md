<h1 align="center">🐸 FROG Connector Specification</h1>

<p align="center">
Definition of connector mapping for <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
<li><a href="#overview">1. Overview</a></li>
<li><a href="#purpose">2. Purpose of the Connector</a></li>
<li><a href="#location">3. Location in a .frog file</a></li>
<li><a href="#structure">4. Connector Structure</a></li>
<li><a href="#patterns">5. Connector Patterns</a></li>
<li><a href="#port-mapping">6. Port Mapping</a></li>
<li><a href="#port-positioning">7. Port Positioning</a></li>
<li><a href="#layout">8. Graphical Layout</a></li>
<li><a href="#examples">9. Examples</a></li>
<li><a href="#validation">10. Validation Rules</a></li>
<li><a href="#extensibility">11. Extensibility</a></li>
<li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>connector</strong> section defines how a Frog appears when it is used as a node inside another diagram.
</p>

<p>
While the <strong>interface</strong> defines the logical inputs and outputs, the connector defines their
<strong>graphical representation and placement</strong>.
</p>

<p>
The connector maps interface ports to positions around the node.
</p>

<hr/>

<h2 id="purpose">2. Purpose of the Connector</h2>

The connector provides:

<ul>
<li>a consistent graphical representation of the Frog</li>
<li>a mapping between interface ports and node edges</li>
<li>a stable node shape for visual programming</li>
</ul>

The connector ensures that:

<ul>
<li>ports appear in predictable positions</li>
<li>connections remain readable</li>
<li>nodes remain visually consistent</li>
</ul>

<hr/>

<h2 id="location">3. Location in a .frog file</h2>

The connector is defined as a top-level object.

Example:

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

The connector section SHOULD exist for Frogs intended to be reused as nodes.

<hr/>

<h2 id="structure">4. Connector Structure</h2>

Example connector definition:

<pre>
"connector": {
  "pattern": "2x2",
  "ports": [
    { "interface_port": "a", "side": "left", "index": 0 },
    { "interface_port": "b", "side": "left", "index": 1 },
    { "interface_port": "result", "side": "right", "index": 0 }
  ]
}
</pre>

Fields:

<ul>
<li><strong>pattern</strong>: connector layout pattern</li>
<li><strong>ports</strong>: mapping of interface ports to graphical positions</li>
</ul>

<hr/>

<h2 id="patterns">5. Connector Patterns</h2>

Connector patterns define the available positions for ports.

Examples:

<ul>
<li>1x1</li>
<li>2x2</li>
<li>3x2</li>
<li>4x2</li>
<li>custom</li>
</ul>

Example:

<pre>
"pattern": "2x2"
</pre>

Patterns determine the number of port positions available on each side.

Implementations MAY support additional patterns.

<hr/>

<h2 id="port-mapping">6. Port Mapping</h2>

Each connector entry maps an interface port to a graphical position.

Example:

<pre>
{
  "interface_port": "temperature",
  "side": "left",
  "index": 0
}
</pre>

Fields:

<ul>
<li><strong>interface_port</strong>: name of the interface port</li>
<li><strong>side</strong>: position side</li>
<li><strong>index</strong>: position index on that side</li>
</ul>

Valid sides:

<ul>
<li>left</li>
<li>right</li>
<li>top</li>
<li>bottom</li>
</ul>

<hr/>

<h2 id="port-positioning">7. Port Positioning</h2>

Ports are placed relative to the node bounding box.

Example layout:

<pre>
       top

 left  NODE  right

      bottom
</pre>

Ports on the same side are ordered using the index value.

Example:

<pre>
index 0
index 1
index 2
</pre>

The lowest index appears closest to the top or left edge.

<hr/>

<h2 id="layout">8. Graphical Layout</h2>

The connector does not define node dimensions.

Node size and layout are determined by the development environment.

The connector only defines:

<ul>
<li>port placement</li>
<li>port ordering</li>
</ul>

Rendering systems are responsible for the visual appearance.

<hr/>

<h2 id="examples">9. Examples</h2>

<h3>Basic Arithmetic Node</h3>

<pre>
"connector": {
  "pattern": "2x2",
  "ports": [
    { "interface_port": "a", "side": "left", "index": 0 },
    { "interface_port": "b", "side": "left", "index": 1 },
    { "interface_port": "result", "side": "right", "index": 0 }
  ]
}
</pre>

<h3>Control System Node</h3>

<pre>
"connector": {
  "pattern": "3x2",
  "ports": [
    { "interface_port": "setpoint", "side": "left", "index": 0 },
    { "interface_port": "measurement", "side": "left", "index": 1 },
    { "interface_port": "enable", "side": "top", "index": 0 },
    { "interface_port": "output", "side": "right", "index": 0 }
  ]
}
</pre>

<hr/>

<h2 id="validation">10. Validation Rules</h2>

Implementations MUST enforce:

<ul>
<li>Each connector port MUST reference an existing interface port</li>
<li>Each interface port SHOULD appear at most once in the connector</li>
<li>Side values MUST be valid</li>
<li>Index values MUST be non-negative integers</li>
</ul>

Invalid connector mappings MUST trigger validation errors.

<hr/>

<h2 id="extensibility">11. Extensibility</h2>

Tools MAY extend the connector definition.

Example:

<pre>
{
  "interface_port": "speed",
  "side": "left",
  "index": 0,
  "label": "Speed Input"
}
</pre>

Unknown properties MUST be ignored by runtimes.

<hr/>

<h2 id="summary">12. Summary</h2>

The connector defines how a Frog appears as a reusable node.

It ensures:

<ul>
<li>consistent port placement</li>
<li>clear visual connections</li>
<li>predictable node interfaces</li>
</ul>

The connector bridges the logical interface and the graphical representation of the program.
