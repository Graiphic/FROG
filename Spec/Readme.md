<h1 align="center">🐸 FROG Specification</h1>

<p align="center">
<strong>FROG — Free Open Graphical Language</strong><br>
Official Language and File Format Specification
</p>

<hr>

<h2>Overview</h2>

<p>
This repository defines the official specification of <strong>FROG — Free Open Graphical Language</strong>.
</p>

<p>
FROG is an open, hardware-agnostic graphical dataflow language designed to provide a durable and independent foundation for graphical programming.
</p>

<p>
The fundamental executable unit of the language is called a <strong>Frog</strong>.
</p>

<p>
A Frog is stored as a <strong>structured JSON document</strong> describing:
</p>

<ul>
<li>the executable graph</li>
<li>the interface of the program</li>
<li>the graphical interaction layer</li>
<li>the connector used when the Frog is embedded inside another Frog</li>
</ul>

<hr>

<h2>Structure of a <code>.frog</code> File</h2>

<p>
A Frog is represented as a JSON document with the following high-level structure:
</p>

<pre>
{
  "spec_version": "0.1",

  "metadata": {},
  "interface": {},
  "connector": {},

  "diagram": {},
  "front_panel": {},

  "icon": {},
  "catalog": {}
}
</pre>

<p>
Each section describes a specific aspect of the Frog program.
</p>

<hr>

<h2>Specification Contents</h2>

<p>
The following sections introduce the components of a <code>.frog</code> file.
</p>

<ul>

<li>
<a href="#metadata-section"><strong>Metadata</strong></a><br>
Identity, versioning and documentation of the Frog.
</li>

<li>
<a href="#interface-section"><strong>Interface</strong></a><br>
Definition of the public inputs and outputs of the Frog.
</li>

<li>
<a href="#connector-section"><strong>Connector</strong></a><br>
Graphical mapping of interface ports when the Frog is used as a node.
</li>

<li>
<a href="#diagram-section"><strong>Diagram</strong></a><br>
Executable dataflow graph composed of nodes and edges.
</li>

<li>
<a href="#frontpanel-section"><strong>Front Panel</strong></a><br>
Graphical interaction layer connected to diagram ports.
</li>

<li>
<a href="#icon-section"><strong>Icon</strong></a><br>
Optional 40×40 SVG icon representing the Frog in the diagram.
</li>

<li>
<a href="#catalog-section"><strong>Catalog Metadata</strong></a><br>
Optional information used by development environments for indexing and example discovery.
</li>

</ul>

<hr>

<h2 id="metadata-section">Metadata</h2>

<p>
The metadata section defines the identity and documentation of a Frog.
</p>

<pre>
"metadata": {
  "name": "Add",
  "description": "Adds two floating-point numbers.",
  "author": "Author Name",
  "program_version": "0.1.0",
  "tags": ["math", "primitive"]
}
</pre>

<p>
This information is used for documentation, indexing and versioning.
</p>

<p>
Detailed specification: <em>(future document)</em>
</p>

<hr>

<h2 id="interface-section">Interface</h2>

<p>
The interface defines the public contract of a Frog.
</p>

<p>
It declares the inputs and outputs visible when the Frog is used inside another diagram.
</p>

<pre>
"interface": {
  "inputs": [
    { "id": "a", "type": "float64" },
    { "id": "b", "type": "float64" }
  ],
  "outputs": [
    { "id": "result", "type": "float64" }
  ]
}
</pre>

<p>
Interface ports MUST correspond to ports used in the diagram.
</p>

<p>
Detailed specification: <em>(future document)</em>
</p>

<hr>

<h2 id="connector-section">Connector</h2>

<p>
The connector defines how interface ports are represented graphically
when the Frog appears as a node inside another diagram.
</p>

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

<p>
Each connector port MUST reference a port declared in the interface.
</p>

<p>
Detailed specification: <em>(future document)</em>
</p>

<hr>

<h2 id="diagram-section">Diagram</h2>

<p>
The diagram defines the executable logic of the Frog.
</p>

<p>
It is a directed dataflow graph composed of nodes and edges.
</p>

<pre>
"diagram": {
  "nodes": [],
  "edges": []
}
</pre>

<p>
Nodes represent operations and edges represent data connections.
</p>

<p>
Detailed specification: <em>(future document)</em>
</p>

<hr>

<h2 id="frontpanel-section">Front Panel</h2>

<p>
The front panel defines the graphical interaction layer of the Frog.
</p>

<p>
Controls provide inputs to the diagram and indicators expose outputs.
</p>

<pre>
"front_panel": {
  "controls": [],
  "indicators": [],
  "layout": {}
}
</pre>

<p>
Each element MUST bind to a corresponding diagram port.
</p>

<p>
Detailed specification: <em>(future document)</em>
</p>

<hr>

<h2 id="icon-section">Icon</h2>

<p>
A Frog may define an optional icon used in graphical diagrams.
</p>

<pre>
"icon": {
  "size": 40,
  "svg": "&lt;svg&gt;...&lt;/svg&gt;"
}
</pre>

<p>
The icon SHOULD be a 40×40 SVG image.
</p>

<p>
Detailed specification: <em>(future document)</em>
</p>

<hr>

<h2 id="catalog-section">Catalog Metadata</h2>

<p>
The catalog section provides metadata used by development environments
to index Frogs, organize examples and enable search.
</p>

<pre>
"catalog": {
  "is_example": true,
  "title": "Basic Addition Example",
  "keywords": ["math", "example"],
  "categories": ["Examples", "Mathematics"]
}
</pre>

<p>
This section is optional and does not affect execution semantics.
</p>

<p>
Detailed specification: <em>(future document)</em>
</p>

<hr>

<h2>Current Status</h2>

<p>
FROG Specification v0.1 — Draft.
</p>

<p>
This document provides an overview of the Frog file structure.
Detailed specifications for each section will be defined in dedicated documents.
</p>

<hr>

<p align="center">
<strong>FROG — Free Open Graphical Language</strong><br>
An open foundation for graphical programming.
</p>
