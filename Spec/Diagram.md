<h1 align="center">🐸 FROG Diagram Specification</h1>

<p align="center">
Definition of executable graphs in <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
<li><a href="#overview">1. Overview</a></li>
<li><a href="#purpose">2. Purpose of the Diagram</a></li>
<li><a href="#location">3. Location in a .frog file</a></li>
<li><a href="#structure">4. Diagram Structure</a></li>
<li><a href="#nodes">5. Node Model</a></li>
<li><a href="#edges">6. Edge Model</a></li>
<li><a href="#layout">7. Layout Information</a></li>
<li><a href="#dependencies">8. Frog Dependencies</a></li>
<li><a href="#subfrogs">9. Sub-Frog Invocation</a></li>
<li><a href="#validation">10. Graph Validation</a></li>
<li><a href="#execution">11. Execution Semantics</a></li>
<li><a href="#examples">12. Example</a></li>
<li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>diagram</strong> defines the executable dataflow graph of a Frog.
</p>

<p>
It contains the nodes representing operations and the edges representing
data connections between them.
</p>

<p>
Execution semantics are derived exclusively from this graph.
</p>

<hr/>

<h2 id="purpose">2. Purpose of the Diagram</h2>

The diagram provides:

<ul>
<li>the executable logic of the Frog</li>
<li>the complete data dependency graph</li>
<li>the scheduling constraints for execution</li>
</ul>

Nodes represent computation.

Edges represent data flow.

<hr/>

<h2 id="location">3. Location in a .frog file</h2>

The diagram appears as a top-level object.

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

The diagram section MUST exist.

<hr/>

<h2 id="structure">4. Diagram Structure</h2>

Example structure:

<pre>
"diagram": {
  "nodes": [],
  "edges": [],
  "dependencies": []
}
</pre>

Fields:

<ul>
<li><strong>nodes</strong> — graph nodes</li>
<li><strong>edges</strong> — data connections</li>
<li><strong>dependencies</strong> — external Frog references</li>
</ul>

<hr/>

<h2 id="nodes">5. Node Model</h2>

A node represents an operation.

Example node:

<pre>
{
  "id": "node_1",
  "kind": "primitive",
  "type": "Add",
  "layout": {
    "x": 120,
    "y": 80
  }
}
</pre>

Node fields:

<ul>
<li><strong>id</strong> — unique identifier</li>
<li><strong>kind</strong> — node category</li>
<li><strong>type</strong> — operation name</li>
<li><strong>layout</strong> — graphical position</li>
</ul>

Possible node kinds:

<ul>
<li>primitive</li>
<li>subfrog</li>
<li>interface_input</li>
<li>interface_output</li>
</ul>

<hr/>

<h2 id="edges">6. Edge Model</h2>

Edges define data connections.

Example:

<pre>
{
  "id": "edge_1",
  "from": {
    "node": "node_1",
    "port": "result"
  },
  "to": {
    "node": "node_2",
    "port": "a"
  }
}
</pre>

Fields:

<ul>
<li><strong>id</strong> — unique edge identifier</li>
<li><strong>from</strong> — source node and port</li>
<li><strong>to</strong> — destination node and port</li>
</ul>

Edges represent directional data flow.

<hr/>

<h2 id="layout">7. Layout Information</h2>

Nodes may include layout information for graphical editors.

Example:

<pre>
"layout": {
  "x": 240,
  "y": 120
}
</pre>

Layout information is optional and only affects graphical rendering.

It does not influence execution.

<hr/>

<h2 id="dependencies">8. Frog Dependencies</h2>

When a diagram uses nodes implemented by other Frogs, those Frogs must be referenced.

Example:

<pre>
"dependencies": [
  {
    "name": "Math.Add",
    "path": "lib/math/add.frog"
  }
]
</pre>

Fields:

<ul>
<li><strong>name</strong> — logical identifier</li>
<li><strong>path</strong> — relative file path</li>
</ul>

Dependencies allow reusable Frog modules.

<hr/>

<h2 id="subfrogs">9. Sub-Frog Invocation</h2>

Nodes may reference other Frog programs.

Example:

<pre>
{
  "id": "node_2",
  "kind": "subfrog",
  "ref": "Math.Add",
  "layout": {
    "x": 320,
    "y": 80
  }
}
</pre>

Field:

<ul>
<li><strong>ref</strong> — dependency identifier</li>
</ul>

This enables hierarchical program composition.

<hr/>

<h2 id="validation">10. Graph Validation</h2>

Implementations MUST enforce the following rules:

<ul>
<li>Node identifiers MUST be unique</li>
<li>Edge identifiers MUST be unique</li>
<li>Edges MUST reference existing nodes</li>
<li>Referenced ports MUST exist</li>
<li>Data types MUST be compatible</li>
</ul>

Invalid graphs MUST fail validation.

<hr/>

<h2 id="execution">11. Execution Semantics</h2>

Execution follows a dataflow model.

<ul>
<li>A node becomes executable when all its inputs are available.</li>
<li>Nodes may execute in parallel.</li>
<li>Execution order is determined by data dependencies.</li>
</ul>

The diagram defines the execution graph.

<hr/>

<h2 id="examples">12. Example</h2>

Example diagram:

<pre>
"diagram": {
  "dependencies": [
    {
      "name": "Math.Add",
      "path": "lib/math/add.frog"
    }
  ],
  "nodes": [
    {
      "id": "input_a",
      "kind": "interface_input",
      "type": "a"
    },
    {
      "id": "input_b",
      "kind": "interface_input",
      "type": "b"
    },
    {
      "id": "add_node",
      "kind": "subfrog",
      "ref": "Math.Add"
    },
    {
      "id": "output",
      "kind": "interface_output",
      "type": "result"
    }
  ],
  "edges": [
    {
      "id": "e1",
      "from": { "node": "input_a", "port": "value" },
      "to": { "node": "add_node", "port": "a" }
    },
    {
      "id": "e2",
      "from": { "node": "input_b", "port": "value" },
      "to": { "node": "add_node", "port": "b" }
    },
    {
      "id": "e3",
      "from": { "node": "add_node", "port": "result" },
      "to": { "node": "output", "port": "value" }
    }
  ]
}
</pre>

<hr/>

<h2 id="summary">13. Summary</h2>

The diagram defines the executable structure of a Frog.

It provides:

<ul>
<li>a directed dataflow graph</li>
<li>a precise dependency model</li>
<li>a hierarchical composition mechanism</li>
</ul>

The diagram is the authoritative definition of execution logic.
