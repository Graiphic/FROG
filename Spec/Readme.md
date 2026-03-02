<h1 align="center">🐸 FROG Specification</h1>

<p align="center">
  <strong>FROG — Free Open Graphical Language</strong><br/>
  Official Language and File Format Specification
</p>

<hr/>

<h2>1. Introduction</h2>

<p>
This repository defines the official specification of <strong>FROG — Free Open Graphical Language</strong>.
</p>

<p>
FROG is an open, hardware-agnostic graphical dataflow language designed to provide a durable and independent foundation for graphical programming.
</p>

<p>
This document defines the language itself — not an IDE, not a runtime, not a compiler.
</p>

<p>
The specification describes:
</p>

<ul>
  <li>The FROG Expression file format (<code>.frog</code>)</li>
  <li>The diagram model</li>
  <li>The front panel model</li>
  <li>The node and edge model</li>
  <li>Versioning and validation rules</li>
</ul>

<p>
Implementations must conform to this specification.
</p>

<hr/>

<h2>2. Design Principles</h2>

<h3>2.1 Language Independence</h3>
<p>
The language must remain independent from:
</p>
<ul>
  <li>Any specific IDE implementation</li>
  <li>Any runtime implementation</li>
  <li>Any compiler backend</li>
</ul>

<h3>2.2 Explicit Graph Model</h3>
<p>
A FROG program is an explicit directed dataflow graph composed of:
</p>
<ul>
  <li>Nodes</li>
  <li>Edges</li>
  <li>Typed ports</li>
  <li>Metadata</li>
</ul>

<h3>2.3 Determinism</h3>
<p>
Execution semantics must be derivable from data availability and graph structure.
</p>

<h3>2.4 Open Representation</h3>
<p>
FROG source files must be:
</p>
<ul>
  <li>Human-readable</li>
  <li>Versionable</li>
  <li>Tool-independent</li>
  <li>Free of hidden compiled artifacts</li>
</ul>

<hr/>

<h2>3. File Format — <code>.frog</code></h2>

<h3>3.1 General Structure</h3>

<p>
A FROG file is a structured JSON document with a required version declaration.
</p>

<pre>
{
  "frog_version": "0.1",
  "metadata": {},
  "interface": {},
  "front_panel": {},
  "diagram": {}
}
</pre>

<h3>3.2 Required Top-Level Fields</h3>

<ul>
  <li><code>frog_version</code> (string) — MUST be present</li>
  <li><code>metadata</code> (object) — MUST be present</li>
  <li><code>interface</code> (object) — MUST be present</li>
  <li><code>diagram</code> (object) — MUST be present</li>
  <li><code>front_panel</code> (object) — MUST be present</li>
</ul>

<hr/>

<h2>4. Metadata Section</h2>

<pre>
"metadata": {
  "name": "ExampleVI",
  "description": "Short description",
  "author": "Author Name",
  "version": "0.1.0",
  "tags": ["math", "example"]
}
</pre>

<p>
This section describes the identity and documentation of the program.
</p>

<hr/>

<h2>5. Interface Definition</h2>

<pre>
"interface": {
  "inputs": [
    { "id": "a", "type": "float" },
    { "id": "b", "type": "float" }
  ],
  "outputs": [
    { "id": "result", "type": "float" }
  ]
}
</pre>

<p>
The interface defines the public contract of the program.
</p>

<hr/>

<h2>6. Diagram Model</h2>

<pre>
"diagram": {
  "nodes": [],
  "edges": []
}
</pre>

<h3>6.1 Node Structure</h3>

<pre>
{
  "id": "node_1",
  "kind": "primitive",
  "type": "Add",
  "ports": {
    "inputs": [
      { "id": "a", "type": "float" },
      { "id": "b", "type": "float" }
    ],
    "outputs": [
      { "id": "result", "type": "float" }
    ]
  },
  "parameters": {},
  "layout": {
    "x": 100,
    "y": 200
  }
}
</pre>

<h3>6.2 Edge Structure</h3>

<pre>
{
  "id": "edge_1",
  "from": { "node": "node_1", "port": "result" },
  "to": { "node": "node_2", "port": "a" }
}
</pre>

<hr/>

<h2>7. Front Panel Model</h2>

<pre>
"front_panel": {
  "controls": [],
  "indicators": [],
  "layout": {}
}
</pre>

<p>
Front panel elements must be explicitly bound to diagram ports.
</p>

<h3>7.1 Control Example</h3>

<pre>
{
  "id": "control_a",
  "type": "numeric",
  "binding": {
    "node": "node_1",
    "port": "a"
  },
  "layout": {
    "x": 50,
    "y": 50
  }
}
</pre>

<hr/>

<h2>8. Icon Definition</h2>

<p>
Each program MAY define a 40x40 SVG icon.
</p>

<pre>
"icon": {
  "size": 40,
  "svg": "&lt;svg&gt;...&lt;/svg&gt;"
}
</pre>

<hr/>

<h2>9. Versioning Policy</h2>

<p>
The specification follows semantic versioning:
</p>

<pre>
MAJOR.MINOR.PATCH
</pre>

<ul>
  <li><strong>MAJOR</strong> — breaking structural changes</li>
  <li><strong>MINOR</strong> — backward-compatible additions</li>
  <li><strong>PATCH</strong> — documentation clarifications</li>
</ul>

<p>
Each <code>.frog</code> file MUST declare its <code>frog_version</code>.
</p>

<hr/>

<h2>10. Normative Terminology</h2>

<ul>
  <li><strong>MUST</strong> — required for compliance</li>
  <li><strong>SHOULD</strong> — recommended</li>
  <li><strong>MAY</strong> — optional</li>
  <li><strong>MUST NOT</strong> — prohibited</li>
</ul>

<hr/>

<h2>11. Current Status</h2>

<p>
FROG Specification v0.1 — Draft.
</p>

<p>
The goal of v0.1 is to define:
</p>

<ul>
  <li>A minimal but coherent file format</li>
  <li>A deterministic structural model</li>
  <li>A stable foundation for future evolution</li>
</ul>

<hr/>

<h2>12. License</h2>

<p>
This specification is released under the Apache License 2.0.
</p>

<hr/>

<p align="center">
<strong>FROG — Free Open Graphical Language</strong><br/>
An open foundation for graphical programming.
</p>
